/*
 * @file device.c
 * @brief Wrapper, that provides common OpenCL Device functionality
 *
 * @see platform.c
 * @see steel_thread.c
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

#include "platform.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * \related cl_Device
 *
 * This function destroy child structures & frees memory, allocated for structure,
 * defined by pointer 'self'
 *
 * @param[in,out] self pointer to structure of type 'cl_Device', in which
 * function pointer 'Destroy' is defined to point on this function

 * @return CL_SUCCESS always
 */
static ret_code Device_Destroy(scow_Device* self)
{
    scow_Device* temp;

    OCL_CHECK_EXISTENCE(self, CL_SUCCESS);

    // Go to the start of the list to destroy all elements one by one
    self = self->To_First_Device(self);

    // Now delete all elements from 1st
    while (self)
    {
        temp = self->next_device;

        self->error->Destroy(self->error);
        self->error = VOID_ERROR_PTR;
        free(self);

        self = temp;
    }

    return CL_SUCCESS;
}

/**
 * \related cl_Device
 *
 * This function returns pointer to last OpenCL Device in list of devices,
 * to which Device defined by pointer 'self' belongs.
 *
 * @param[in,out] self pointer to structure of type 'cl_Device', in which
 * function pointer 'To_Last_Device' is defined to points on this function.

 * @return pointer to last device in case of success, VOID_DEVICE_PTR else.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static scow_Device* Device_To_Last_Device(scow_Device* self)
{
    OCL_CHECK_EXISTENCE(self, VOID_DEVICE_PTR);
    while (self->next_device)
    {
        self = self->next_device;
    }

    return self;
}

/**
 * \related cl_Device
 *
 * This function returns pointer to first OpenCL Device in list of devices,
 * to which Device defined by pointer 'self' belongs.
 *
 * @param[in,out] self pointer to structure of type 'cl_Device', in which
 * function pointer 'To_Last_Device' is defined to points on this function.

 * @return pointer to first device in case of success, VOID_DEVICE_PTR else.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static scow_Device* Device_To_First_Device(scow_Device* self)
{
    OCL_CHECK_EXISTENCE(self, VOID_DEVICE_PTR);
    while (self->prev_device)
    {
        self = self->prev_device;
    }

    return self;
}

/**
 * \related cl_Device
 *
 * This function gathers information about OpenCL Device, defined by pointer 'self'.
 *
 * @param[in,out] self pointer to structure of type 'cl_Device', in which
 * function pointer 'Gather_Info' is defined to point on this function.
 * @param[in] param enumeration member, that defines what kind of information
 * do we want to query.
 *
 * @return CL_SUCCESS in case of successful query, error code of type 'ret_code'
 * in case of error.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static ret_code Device_Gather_Info(scow_Device* self, DEVICE_INFO_PARAM param)
{
    ret_code ret;
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);

    if (param == DEVICE_NAME || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device,
        CL_DEVICE_NAME, CL_DEVICE_NAME_SIZE, self->name, NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_EXTENSIONS || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_EXTENSIONS, CL_DEVICE_EXTENSIONS_SIZE, self->extensions,
        NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_VERSION || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_VERSION, CL_DEVICE_VERSION_SIZE, self->device_version,
        NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DRIVER_VERSION || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DRIVER_VERSION, CL_DRIVER_VERSION_SIZE, self->driver_version,
        NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_EXECUTION_CAPABILITIES || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device, CL_DEVICE_EXECUTION_CAPABILITIES,
                sizeof(cl_device_exec_capabilities), &self->exec_capabilities,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_MAX_COMPUTE_UNITS || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &self->max_compute_units,
        NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_MEM_BASE_ADDR_ALIGN || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_MEM_BASE_ADDR_ALIGN, sizeof(cl_uint),
                &self->mem_base_addr_align, NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_MIN_DATA_TYPE_ALIGN_SIZE
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, sizeof(cl_uint),
                &self->min_data_type_align_size,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_MAX_CLOCK_FREQUENCY || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint),
                &self->max_clock_frequency, NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_GLOBAL_MEM_CACHELINE_SIZE
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(cl_uint),
                &self->global_mem_cacheline_size,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_CHAR
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR, sizeof(cl_uint),
                &self->native_vector_width_char,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_SHORT
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT, sizeof(cl_uint),
                &self->native_vector_width_short,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_INT
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, sizeof(cl_uint),
                &self->native_vector_width_int,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_LONG
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, sizeof(cl_uint),
                &self->native_vector_width_long,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_FLOAT
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, sizeof(cl_uint),
                &self->native_vector_width_float,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_GLOBAL_MEM_SIZE || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &self->global_mem_size,
        NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_GLOBAL_MEM_CACHE_SIZE || param == DEVICE_ALL_AVAILABLE)
    {
        ret |= clGetDeviceInfo(self->device,
        CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cl_ulong),
                &self->global_mem_cache_size, NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    return CL_SUCCESS;
}

/**
 * \related cl_Device
 *
 * This function puts information about OpenCL Device, defined by pointer 'self'
 * to stdout.
 *
 * @param[in] self pointer to structure of type 'cl_Device', in which
 * function pointer 'Print_Info' is defined to point on this function.
 * @param[in] param enumeration member, that defines what kind of information
 * do we want to output.
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' in
 * case of error.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static ret_code Device_Print_Info(const scow_Device* self,
        DEVICE_INFO_PARAM param)
{
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);

    fprintf(stdout, "\n---OpenCL Device info:---\n");

    if (param == DEVICE_NAME || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "name:                       %s\n", self->name);

    if (param == DEVICE_EXECUTION_CAPABILITIES || param == DEVICE_ALL_AVAILABLE)
    {
        //CL_EXEC_KERNEL is mandatory, no need to check
        fprintf(stdout, "exec capabilities:          CL_EXEC_KERNEL\n");

        if (self->exec_capabilities & CL_EXEC_NATIVE_KERNEL)
            fprintf(stdout,
                    "exec capabilities:          CL_EXEC_NATIVE_KERNEL\n");
    }

    if (param == DEVICE_EXTENSIONS || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "extensions:                 %s\n", self->extensions);

    if (param == DEVICE_VERSION || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "OpenCL version supported:   %s\n",
                self->device_version);

    if (param == DRIVER_VERSION || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "OpenCL driver version:      %s\n",
                self->driver_version);

    if (param == DEVICE_MAX_COMPUTE_UNITS || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "max_compute_units:          %u\n",
                self->max_compute_units);

    if (param == DEVICE_MEM_BASE_ADDR_ALIGN || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "mem_base_addr_align:        %u\n",
                self->mem_base_addr_align);

    if (param == DEVICE_MIN_DATA_TYPE_ALIGN_SIZE
            || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "min_data_type_align_size:   %u\n",
                self->min_data_type_align_size);

    if (param == DEVICE_MAX_CLOCK_FREQUENCY || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "max_clock_frequency:        %u\n",
                self->max_clock_frequency);

    if (param == DEVICE_GLOBAL_MEM_CACHELINE_SIZE
            || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "global_mem_cacheline_size:  %u\n",
                self->global_mem_cacheline_size);

    if (param == DEVICE_GLOBAL_MEM_SIZE || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "global_mem_size:            %lu\n",
                self->global_mem_size);

    if (param == DEVICE_GLOBAL_MEM_CACHE_SIZE || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "global_mem_cache_size:      %lu\n",
                self->global_mem_cache_size);

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_CHAR
            || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "native char vector length:  %u\n",
                self->native_vector_width_char);

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_SHORT
            || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "native short vector length: %u\n",
                self->native_vector_width_short);

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_INT
            || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "native int vector length:   %u\n",
                self->native_vector_width_int);

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_LONG
            || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "native long vector length:  %u\n",
                self->native_vector_width_long);

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_FLOAT
            || param == DEVICE_ALL_AVAILABLE)
        fprintf(stdout, "native float vector length: %u\n",
                self->native_vector_width_float);

    return CL_SUCCESS;
}

/**
 * \related cl_Device
 *
 * This function makes OpenCL Device defined by pointer 'self' default for
 * OpenCL platform, defined by pointer 'self->parent_platform'.
 * This function is thread-safe.
 *
 * @param[in,out] self pointer to structure of type 'cl_Device', in which
 * function pointer 'Make_Default' is defined to point on this function.
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' in
 * case of error.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 * @see description of structure 'cl_Platform' for details about parent OpenCL
 * platform.
 */
static ret_code Device_Make_Default(scow_Device* self)
{
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);
    OCL_CHECK_EXISTENCE(self->parent_platform, INVALID_BUFFER_GIVEN);
    self->parent_platform->default_device = self;

    return CL_SUCCESS;
}

/*! \cond PRIVATE */
static ret_code Get_Device_Num(scow_Platform* parent_platform,
        const cl_device_type device_type_wanted)
{
    cl_int ret;

    if (parent_platform == VOID_PLATFORM_PTR)
    {
        return INVALID_BUFFER_GIVEN;
    }

    ret = clGetDeviceIDs(parent_platform->platform, device_type_wanted, 0, NULL,
            &parent_platform->wanted_devices_num);

    if (ret != CL_SUCCESS)
    {
        // If no suitable OpenCL Devices were found - that's not error
        if (ret == CL_DEVICE_NOT_FOUND)
        {
            parent_platform->wanted_devices_num = 0;
            return CANT_FIND_DEVICE;
        }
        else
        {
            ocl_error_message(ret);
            return GENERIC_OPENCL_ERROR;
        }
    }

    return CL_SUCCESS;
}

static scow_Device* Make_List_Element(scow_Platform* parent_platform,
        DEVICE_CREATION_MODE creation_type,
        const cl_device_type wanted_device_type,
        const cl_device_id given_device_id, const unsigned int device_number)
{
    scow_Device* self;

    OCL_CHECK_EXISTENCE(parent_platform, VOID_DEVICE_PTR);

    self = (scow_Device*) calloc(1, sizeof(*self));

    OCL_CHECK_EXISTENCE(parent_platform, VOID_DEVICE_PTR);

    self->curr_device = self;
    self->parent_platform = parent_platform;
    self->number = device_number;
    self->device = given_device_id;

    self->error = Make_Error();

    self->Destroy = Device_Destroy;
    self->Gather_Info = Device_Gather_Info;
    self->Print_Info = Device_Print_Info;
    self->Make_Default = Device_Make_Default;
    self->To_First_Device = Device_To_First_Device;
    self->To_Last_Device = Device_To_Last_Device;

    if (creation_type != DEVICE_CREATE_QUICK)
    {
        ret_code ret = self->Gather_Info(self, DEVICE_ALL_AVAILABLE);

        if (ret != CL_SUCCESS)
        {
            err_log_func(ret);
            self->Destroy(self);
            return VOID_DEVICE_PTR;
        }
    }

    return self;
}
/*! \endcond */

/**
 * \related cl_Device
 *
 * This function detects all OpenCL Devices of wanted type under given platform,
 * allocates memory for list of structures of type 'cl_Device', set
 * function pointers, structure fields & returns pointer to first element of
 * the list of structures in case of success. List of devices contain all
 * OpenCL Devices of wanted types, that were found.
 *
 * @param[in,out] parent_platform pointer to parent OpenCL platform for OpenCL Device created
 * @param[in] creation_type enumeration, that describes how to create Device.
 * @param[in] wanted_device_type type of Device we want to initialize under parent platform.
 * @return pointer to created structure in case of success, \ref VOID_DEVICE_PTR pointer otherwise.
 *
 * @warning always use 'Destroy' function pointer of Device Translation structure
 * to free memory, allocated by this function
 */
scow_Device* Make_Devices(scow_Platform* parent_platform,
        DEVICE_CREATION_MODE creation_type,
        const cl_device_type wanted_device_type)
{
    ret_code ret;

    // Where to store OpenCL Devices before we create cl_Device array
    cl_device_id* temp_storage;

    scow_Device* self;

    OCL_CHECK_EXISTENCE(parent_platform, VOID_DEVICE_PTR);

    ret = Get_Device_Num(parent_platform, wanted_device_type);

    /* Selected platform may not has suitable OpenCL Devices - that's not error.
     * So we should make a check.
     */
    if (parent_platform->wanted_devices_num > 0)
    {
        temp_storage = (cl_device_id*) calloc(
                parent_platform->wanted_devices_num, sizeof(*temp_storage));

        OCL_CHECK_EXISTENCE(temp_storage, VOID_DEVICE_PTR);
    }
    else
    {
        // Inform parent platform, that no suitable OpenCL Devices were found
        if (ret == CANT_FIND_DEVICE)
        {
            parent_platform->error->Set_Last_Code(parent_platform->error,
                    CANT_FIND_DEVICE);
        }

        return VOID_DEVICE_PTR;
    }

    // Get all OpenCL Devices of wanted type into array
    ret = clGetDeviceIDs(parent_platform->platform, wanted_device_type,
            parent_platform->wanted_devices_num, temp_storage, NULL);

    if (ret != CL_SUCCESS)
    {
        free(temp_storage);
        return VOID_DEVICE_PTR;
    }

    // Root is made separately
    self = Make_List_Element(parent_platform, creation_type, wanted_device_type,
            temp_storage[0], 0);

    if (self == VOID_DEVICE_PTR)
    {
        free(temp_storage);
        return self;
    }

    for (int i = 1; i < parent_platform->wanted_devices_num; i++)
    {
        self->next_device = Make_List_Element(parent_platform, creation_type,
                wanted_device_type, temp_storage[i], i);

        if (self->next_device == VOID_DEVICE_PTR)
        {
            free(temp_storage);
            self->Destroy(self);
            return self;
        }

        // Insert element in the list
        self->next_device->prev_device = self;

        self = self->next_device;
    }

    free(temp_storage);

    // We should return pointer to first element of the list
    return self->To_First_Device(self);
}
