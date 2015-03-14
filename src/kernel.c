/*
 * @file kernel.c
 * @brief Provides basic abstraction for OpenCL kernel
 *
 * @see kernel.h
 *
 * Copyright 2014 by Samsung Electronics, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "steel_thread.h"
#include "device.h"
#include "kernel.h"

/*! \cond PRIVATE */
static cl_double Gather_Time_uS(cl_event* event)
{
    cl_ulong start = 0, end = 0;
    cl_double no_time = -1.0;
    cl_int ret = CL_SUCCESS;

    OCL_CHECK_EXISTENCE(event, no_time);

    ret = clWaitForEvents(1, event);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, no_time);

    ret = clGetEventProfilingInfo(*event, CL_PROFILING_COMMAND_START,
            sizeof(cl_ulong), &start, NULL);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, no_time);

    ret = clGetEventProfilingInfo(*event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong),
            &end, NULL);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, no_time);

    return (cl_double) (end - start) * (cl_double) (1e-03);
}

static cl_uint Get_Args_Num(scow_Kernel* minimal_kernel)
{
    cl_uint num_args = 0;
    cl_int ret_code;

    OCL_CHECK_EXISTENCE(minimal_kernel, 0);

    ret_code = clGetKernelInfo(minimal_kernel->kernel,
            CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &num_args, NULL);

    OCL_DIE_ON_ERROR(ret_code, CL_SUCCESS, NULL, 0);

    return num_args;
}

static char* Read_Source_File(const char *filename)
{
    long int size = 0, res = 0;

    char *src = NULL;

    FILE *file = fopen(filename, "rb");

    if (!file)
        return NULL;

    if (fseek(file, 0, SEEK_END))
    {
        fclose(file);
        return NULL;
    }

    size = ftell(file);
    if (size == 0)
    {
        fclose(file);
        return NULL;
    }

    rewind(file);

    src = (char *) calloc(size + 1, sizeof(char));
    if (!src)
    {
        src = NULL;
        fclose(file);
        return src;
    }

    res = fread(src, 1, sizeof(char) * size, file);
    if (res != sizeof(char) * size)
    {
        fclose(file);
        free(src);

        return (char*)NULL;
    }

    src[size] = '\0'; /* NULL terminated */
    fclose(file);

    return src;
}
/*! \endcond */

/**
 * \related cl_Kernel
 *
 * This function releases child structures & frees allocated memory.
 *
 * @param[in,out] self pointer to structure of type 'cl_Kernel', in which
 * function pointer 'Destroy' is defined to point on this function

 * @return CL_SUCCESS always
 */
static ret_code Kernel_Destroy(scow_Kernel* self)
{
    OCL_CHECK_EXISTENCE(self, CL_SUCCESS);

    if (self->kernel)
    {
        clReleaseKernel(self->kernel);
    }
    if (self->program)
    {
        clReleaseProgram(self->program);
    }

    if (self->timer)
    {
        self->timer->Destroy(self->timer);
    }
    if (self->error)
    {
        self->error->Destroy(self->error);
    }

    free(self);

    return CL_SUCCESS;
}

/**
 * \related cl_Kernel
 *
 * This function sets argument for OpenCL kernel
 *
 * @param[in,out] self pointer to structure of type 'cl_Kernel', in which
 * function pointer 'Set_Arg' is defined to point on this function
 * @param[in] arg_index number of kernel argument
 * @param[in] arg_size size of kernel argument
 * @param[in] ptr_to_arg pointer to argument

 * @return CL_SUCCESS in case of success, error code of type ret_code otherwise.
 *
 * @see cl_err_codes.h for details
 * @see description of structure 'cl_Error_t' for details about error handling
 */
static ret_code Kernel_Set_Arg(scow_Kernel* self, const cl_uint arg_index,
        const size_t arg_size, void* ptr_to_arg)
{
    return clSetKernelArg(self->kernel, arg_index, arg_size, ptr_to_arg);
}

/**
 * \related cl_Kernel
 *
 * This function sets ND dimensions for OpenCL kernel
 *
 * @param[in,out] self pointer to structure of type 'cl_Kernel', in which
 * function pointer 'Set_ND_Sizes' is defined to point on this function
 * @param[in] dimensionality number of problem's dimensions
 * @param[in] global_wg_size Global amount of work items in each dimension
 * @param[in] local_wg_size Size of local work group (if any) in each dimension.
 * This argument is optional. If kernel doesn't utilize local work groups,
 * pass NULL pointer as argument.

 * @return CL_SUCCESS in case of success, error code of type ret_code otherwise.
 *
 * @see cl_err_codes.h for details
 * @see description of structure 'cl_Error_t' for details about error handling
 */
static ret_code Kernel_Set_ND_Sizes(scow_Kernel* self,
        const unsigned int dimensionality, const unsigned int* global_wg_size,
        const unsigned int* local_wg_size)
{
    cl_int ret;

    size_t max_available_local_wg_size, curr_local_wg_size;

    // Local work groups are not mandatory
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);
    OCL_CHECK_EXISTENCE(global_wg_size, INVALID_BUFFER_GIVEN);

    if (dimensionality <= MAX_NUM_DIMENSIONS)
    {
        self->Dimensionality = dimensionality;
    }
    else
    {
        return INVALID_ND_DIMENSIONALITY;
    }

    // Check what local work group size is maximal available for kernel.
    ret = clGetKernelWorkGroupInfo(self->kernel,
        self->parent_steel_thread->device->device_id,
            CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t),
            &max_available_local_wg_size,
            NULL);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    for (int i = 0; i < self->Dimensionality; i++)
    {
        /* If local work groups are present, compute current local work group
         * size & compare it with maximal available.
         */
        if (local_wg_size)
        {
            (i == 0) ?
                    (curr_local_wg_size = local_wg_size[i]) :
                    (curr_local_wg_size *= local_wg_size[i]);

            int good_local_size = curr_local_wg_size
                    <= max_available_local_wg_size;

            if (good_local_size)
            {
                self->Local_Work_Size[i] = local_wg_size[i];
            }
            else
            {
                OCL_DIE_ON_ERROR(INVALID_LOCAL_WG_SIZE, CL_SUCCESS, NULL,
                        INVALID_LOCAL_WG_SIZE);
            }
        }

        /* Check that given global work group size is  multiple to corresponded
         * local work group size.
         */
        int good_global_size = 1;

        if (local_wg_size)
        {
            good_global_size = (global_wg_size[i] % local_wg_size[i] == 0);
        }

        if (good_global_size)
        {
            self->Global_Work_Size[i] = global_wg_size[i];
        }
        else
        {
            OCL_DIE_ON_ERROR(GLOBAL_NOT_MULTIPLE_TO_LOCAL, CL_SUCCESS, NULL,
                    GLOBAL_NOT_MULTIPLE_TO_LOCAL);
        }
    }

    return CL_SUCCESS;
}

/**
 * \related cl_Kernel
 *
 * This function enqueues kernel execution
 *
 * @param[in,out] self pointer to structure of type 'cl_Kernel', in which
 * function pointer 'ND_Range' is defined to point on this function
 * @param[in] queue OpenCL command queue, that will be used for kernel execution
 * @param[in] evt_wait_list_size Size of list of OpenCL events, that must be
 * finished before kernel execution. If kernel doesn't need to wait for any
 * events, pass 0 as argument.
 * @param[in] evt_wait_list Pointer to array of OpenCL events. If kernel doesn't
 * need to wait for any events, pass NULL as argument.
 * @param[out] generated_evt Pointer to OpenCL event, that kernel will produce.
 * If kernel doesn't need to produce any event, pass NULL as argument
 * @param[in] time_measure_flag flag, that denotes if kernel execution time
 * should be gathered.

 * @return CL_SUCCESS in case of success, error code of type ret_code otherwise.
 *
 * @see cl_err_codes.h for details
 * @see description of structure 'cl_Error_t' for details about error handling
 */
static ret_code Kernel_ND_Range(scow_Kernel* self, cl_command_queue* queue,
        cl_uint evt_wait_list_size, const cl_event* evt_wait_list,
        cl_event* generated_evt, TIME_STUDY_MODE time_measure_mode)
{
    cl_int ret;
    cl_double run_time;
    cl_event* p_evt;

    if (generated_evt == NULL)
    {
        // Passing internal event to NDRange()
        self->evt_check_priority = INTERNAL_EVT_PRIORITY;
        p_evt = &self->internal_event;
    }
    else
    {
        // Passing external event to NDRange()
        self->evt_check_priority = EXTERNAL_EVT_PRIORITY;
        self->p_external_event = generated_evt;
        p_evt = generated_evt;
    }

    // Check if we have Local Work Group size organization.
    size_t* Local_Work_Size = NULL;
    int have_local_work_groups = 0;

    for (int i = 0; i < self->Dimensionality; i++)
    {
        if (self->Local_Work_Size[i] != 0)
        {
            have_local_work_groups++;
        }
    }

    if (have_local_work_groups != 0)
    {
        Local_Work_Size = &self->Local_Work_Size[0];
    }

    ret = clEnqueueNDRangeKernel(*queue, self->kernel, self->Dimensionality,
            NULL, self->Global_Work_Size, Local_Work_Size, evt_wait_list_size,
            evt_wait_list, p_evt);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    if (time_measure_mode != DONT_MEASURE)
    {
        run_time = Gather_Time_uS(p_evt);
    }

    switch (time_measure_mode)
    {
    case MEASURE:
        /* Increment num_calls only in this case to have average runtime, which
         * is always consistent. */
        self->timer->current_time_device = run_time;
        self->timer->total_time_device += run_time;
        self->timer->num_calls_device++;
        break;

    default:
        break;
    }

    return ret;
}

/**
 * \related cl_Kernel
 *
 * This function enqueues kernel execution without prior argument setting via
 * 'Set_Arg' function pointer.
 *
 * @param[in,out] self pointer to structure of type 'cl_Kernel', in which
 * function pointer 'Launck' is defined to point on this function
 * @param[in] queue OpenCL command queue, that will be used for kernel execution
 * @param[in] evt_wait_list_size Size of list of OpenCL events, that must be
 * finished before kernel execution. If kernel doesn't need to wait for any
 * events, pass 0 as argument.
 * @param[in] evt_wait_list Pointer to array of OpenCL events. If kernel doesn't
 * need to wait for any events, pass NULL as argument.
 * @param[out] generated_evt Pointer to OpenCL event, that kernel will produce.
 * If kernel doesn't need to produce any event, pass NULL as argument
 * @param[in] time_measure_flag flag, that denotes if kernel execution time
 * should be gathered.
 * @param[in] argument structure of type 'cl_Kernel_Arg_t', that describes
 * OpenCL kernel arguments characteristics, such as size and pointer to argument.

 * @return CL_SUCCESS in case of success, error code of type ret_code otherwise.
 *
 * @see cl_err_codes.h for details
 * @see description of structure 'cl_Error_t' for details about error handling
 */
static ret_code Kernel_Launch(scow_Kernel* self, cl_command_queue* queue,
        cl_uint evt_wait_list_size, const cl_event* evt_wait_list,
        cl_event* generated_evt, TIME_STUDY_MODE time_measure_mode, ...)
{
    va_list kernel_arguments;
    cl_int ret;

    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);

    // Other function arguments are kernel arguments. They are optional
    va_start(kernel_arguments, time_measure_mode);

    for (int i = 0; i < self->num_args; i++)
    {
        scow_Kernel_Arg curr_arg = va_arg(kernel_arguments, scow_Kernel_Arg);

        ret = Kernel_Set_Arg(self, i, curr_arg.size, curr_arg.ptr);

        if (ret != CL_SUCCESS)
        {
            va_end(kernel_arguments);
            OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);
        }
    }

    va_end(kernel_arguments);

    return Kernel_ND_Range(self, queue, evt_wait_list_size, evt_wait_list,
            generated_evt, time_measure_mode);
}

/**
 * \related cl_Kernel
 *
 * This function returns OpenCL kernel name
 *
 * @param[in,out] self pointer to structure of type 'cl_Kernel', in which
 * function pointer 'Get_Name' is defined to point on this function

 * @return Pointer to string in case of success, NULL pointer otherwise. In case
 * of fail, function sets value of error code into structure, defined by
 * pointer 'self->error'.
 *
 * @see cl_err_codes.h for details
 * @see description of structure 'cl_Error_t' for details about error handling
 */
static char* Kernel_Get_Name(scow_Kernel* self)
{
    OCL_CHECK_EXISTENCE(self, NULL);

    if (self->kernel == ((cl_kernel) 0x0))
    {
        self->error->Set_Last_Code(self->error, KERNEL_DOESNT_EXIST);
        return NULL;
    }

    // Amount of memory, that will be needed for string with kernel name.
    size_t string_size;

    cl_int ret = clGetKernelInfo(self->kernel, CL_KERNEL_FUNCTION_NAME, 0, NULL,
            &string_size);

    char* kernel_name = (char*) calloc(string_size, sizeof(*kernel_name));

    ret = clGetKernelInfo(self->kernel, CL_KERNEL_FUNCTION_NAME, string_size,
            kernel_name, NULL);

    if (ret != CL_SUCCESS)
    {
        free(kernel_name);
        self->error->Set_Last_Code(self->error, ret);
        return NULL;
    }

    return kernel_name;
}

static ret_code Kernel_Check_Status(scow_Kernel* self)
{
    OCL_CHECK_EXISTENCE(self, VOID_ARG_GIVEN);

    cl_event* p_event;

    (self->evt_check_priority == EXTERNAL_EVT_PRIORITY) ?
            (p_event = self->p_external_event) :
            (p_event = &self->internal_event);

    return clGetEventInfo(*p_event, CL_EVENT_COMMAND_EXECUTION_STATUS,
            sizeof(cl_int), &self->exec_status, NULL);
}

/**
 * \related cl_Kernel
 *
 * This function allocates memory for structure, sets function pointers &
 * initializes OpenCL kernel.
 *
 * @param[in] parent_steel_thread parent Steel Thread, which gives context, etc
 * @param[in] how_to_get_sources enumeration, that describes in what way
 * kernel source or pre-built program is provided
 * @param[in] source this argument can either be filename, if kernel source is
 * provided in file, or pre-built program goes in file, or it's a string with
 * source code
 * @param[in] kernel_name name of OpenCL kernel
 * @param[in] extra_params extra parameters, that will be used during programm
 * building stage, if pre-built program isn't provided.
 *
 * @return pointer to allocated structure in case of success,
 * \ref VOID_KERNEL_PTR otherwise
 *
 * @warning always use 'Destroy' function pointer to free memory, allocated by
 * this function.
 */
scow_Kernel* Make_Kernel(scow_Steel_Thread* parent_steel_thread,
        OPENCL_SOURCES_MODE how_to_get_sources, const char* source,
        const char* kernel_name, const char* extra_params)
{
    scow_Kernel* self;
    cl_int ret;
    char* src_file;

    OCL_CHECK_EXISTENCE(parent_steel_thread, VOID_KERNEL_PTR);
    OCL_CHECK_EXISTENCE(kernel_name, VOID_KERNEL_PTR);

    if (strlen(kernel_name) > OCL_KERNEL_NAME_MAX_LEN)
    {
        return VOID_KERNEL_PTR;
    }

    self = (scow_Kernel*) calloc(1, sizeof(*self));

    OCL_CHECK_EXISTENCE(self, VOID_KERNEL_PTR);

    self->Destroy = Kernel_Destroy;
    self->Set_ND_Sizes = Kernel_Set_ND_Sizes;
    self->Get_Name = Kernel_Get_Name;
    self->Launch = Kernel_Launch;
    self->Check_Status = Kernel_Check_Status;

    self->parent_steel_thread = parent_steel_thread;
    self->error = Make_Error();
    self->timer = Make_Timer(self);

    strcpy(self->name, kernel_name);

    switch (how_to_get_sources)
    {
    case READ_FROM_FILES:
        // Argument 'file' acts as filename
        src_file = Read_Source_File(source);

        if (src_file == NULL)
        {
            self->Destroy(self);
            error_message("Error while reading kernel source file.\n");
            return VOID_KERNEL_PTR;
        }

        break;

    case READ_FROM_STRING:
        // Argument 'file' acts as string with source code;
        src_file = source;
        break;

    default:
        self->Destroy(self);
        return VOID_KERNEL_PTR;
        break;
    }

    // Creating & building program
    char* build_params = (char*) calloc(
            strlen(self->parent_steel_thread->init_params)
                    + strlen(extra_params) + 2, sizeof(*build_params));

    strcat(build_params, self->parent_steel_thread->init_params);
    strcat(build_params, extra_params);

    self->program = clCreateProgramWithSource(
            self->parent_steel_thread->context, 1, (const char**) &src_file,
            NULL, &ret);

    ret |= clBuildProgram(self->program, 0, NULL, build_params, NULL, NULL);

    free(build_params);

    if (ret != CL_SUCCESS)
    {
        size_t len = 0;
        char *buffer;

        clGetProgramBuildInfo(self->program,
            self->parent_steel_thread->device->device_id,
                CL_PROGRAM_BUILD_LOG, 0, NULL, &len);

        buffer = calloc(len, sizeof(char));

        clGetProgramBuildInfo(self->program,
            self->parent_steel_thread->device->device_id,
                CL_PROGRAM_BUILD_LOG, len, buffer, NULL);

        error_message(buffer);

        free(buffer);

        if (how_to_get_sources == READ_FROM_FILES)
        {
            free(src_file);
        }

        self->Destroy(self);
        return VOID_KERNEL_PTR;
    }

    // Creating the kernel
    self->kernel = clCreateKernel(self->program, self->name, &ret);

    if (ret != CL_SUCCESS)
    {
        ocl_error_message(ret);

        if (how_to_get_sources == READ_FROM_FILES)
        {
            free(src_file);
        }

        self->Destroy(self);
        return VOID_KERNEL_PTR;
    }

    if (how_to_get_sources == READ_FROM_FILES)
    {
        free(src_file);
    }

    self->num_args = Get_Args_Num(self);
    return self;
}

