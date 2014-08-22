/*
 * @file steel_thread.c
 * @brief Provides container for most frequently used OpenCL objects - contex & queues
 *
 * @see steel_thread.h
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
#include <stdio.h>

#include "steel_thread.h"
#include "error.h"
#include "device.h"
#include "platform.h"

static ret_code Init_OpenCL(scow_Steel_Thread* self)
{
    cl_int ret;
    cl_command_queue_properties q_props = CL_QUEUE_PROFILING_ENABLE;

    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);

    // Create context for default plarform & default device
    self->context = clCreateContext(
        NULL, 1, &self->device->device_id, NULL, NULL, &ret);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_CREATE_CONTEXT);

    // Create command queue for kernel execution
    self->q_cmd = clCreateCommandQueue(self->context,
        self->device->device_id, q_props, &ret);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_CREATE_CMD_QUEUE);

    // Create command queue for data transmission
    self->q_data_htod = clCreateCommandQueue(self->context,
        self->device->device_id, q_props, &ret);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_CREATE_CMD_QUEUE);

    self->q_data_dtoh = clCreateCommandQueue(self->context,
        self->device->device_id, q_props, &ret);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_CREATE_CMD_QUEUE);

    self->q_data_dtod = clCreateCommandQueue(self->context,
        self->device->device_id, q_props, &ret);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_CREATE_CMD_QUEUE);

    return CL_SUCCESS;
}
/*! \endcond */

/**
 * \related cl_Steel_Thread_t
 *
 * This function destroy child structures & frees memory, allocated for structure,
 * defined by pointer 'self'
 *
 * @param[in,out] self pointer to structure of type 'cl_Steel_Thread_t', in which
 * function pointer 'Destroy' is defined to point on this function

 * @return CL_SUCCESS always
 */
static ret_code Steel_Thread_Destroy(scow_Steel_Thread* self)
{
    OCL_CHECK_EXISTENCE(self, CL_SUCCESS);

    // Releasing OpenCL objects if any
    if (self->q_cmd)
    {
        clReleaseCommandQueue(self->q_cmd);
    }
    if (self->q_data_htod)
    {
        clReleaseCommandQueue(self->q_data_htod);
    }
    if (self->q_data_dtoh)
    {
        clReleaseCommandQueue(self->q_data_dtoh);
    }
    if (self->q_data_dtod)
    {
        clReleaseCommandQueue(self->q_data_dtod);
    }

    if (self->context)
    {
        clReleaseContext(self->context);
    }

    if (self->platform)
    {
        self->platform->Destroy(self->platform);
    }

    if (self->device){
        self->device->Destroy(self->device);
    }

    self->error->Destroy(self->error);

    free(self);

    return CL_SUCCESS;
}

/**
 * \related cl_Steel_Thread_t
 *
 * This function will not return unless all enqueued tasks in queue 'q_common_data'
 * of particular structure, defined by pointer 'self' will be finished.
 *
 * @param[in,out] self pointer to structure of type 'cl_Steel_Thread_t', in which
 * function pointer 'Destroy' is defined to point on this function
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' in
 * case of error.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static ret_code Steel_Thread_Wait_For_Data(scow_Steel_Thread* self)
{
    ret_code ret;

    ret = clFinish(self->q_data_htod);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    ret = clFinish(self->q_data_dtoh);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    ret = clFinish(self->q_data_dtod);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    return ret;
}

/**
 * \related cl_Steel_Thread_t
 *
 * This function will not return unless all enqueued tasks in queue 'q_common_cmd'
 * of particular structure, defined by pointer 'self' will be finished.
 *
 * @param[in,out] self pointer to structure of type 'cl_Steel_Thread_t', in which
 * function pointer 'Destroy' is defined to point on this function
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' in
 * case of error.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static ret_code Steel_Thread_Wait_For_Cmd(scow_Steel_Thread* self)
{
    return clFinish(self->q_cmd);
}

/**
* \related cl_Steel_Thread_t
*
* This function flushes Command Queue, in which kernels executions are
* issued by default.
*
* @param[in,out] self pointer to structure of type 'cl_Steel_Thread_t', in which
* function pointer 'FlushCmd' is defined to point on this function
*
* @return CL_SUCCESS in case of success, error code of type 'ret_code' in
* case of error.
*
* @see cl_err_codes.h for detailed error description.
* @see 'cl_Error_t' structure for error handling.
*/
static ret_code Steel_Thread_Flush_Cmd(scow_Steel_Thread* self)
{
    return clFlush(self->q_cmd);
}

/**
 * \related cl_Steel_Thread_t
 *
 * This function allocates memory for structure, initialize OpenCL & set
 * function pointers
 *
 * @param[in] device_type type of OpenCL device(s) we want to initialize
 * @param[in] init_params Additional parameters, that will be passed at OpenCL
 * program building stage for all objects, that will have this Steel Thread as
 * parent one.
 *
 * @return pointer to allocated structure in case of success,
 * \ref VOID_STEEL_THREAD_PTR otherwise
 *
 * @warning always use 'Destroy' function pointer to free memory, allocated by
 * this function.
 */
scow_Steel_Thread* Make_Steel_Thread(cl_device_id given_device)
{
    scow_Steel_Thread* self = (scow_Steel_Thread*) calloc(1, sizeof(*self));
    OCL_CHECK_EXISTENCE(self, VOID_STEEL_THREAD_PTR);

    self->error             = Make_Error();
    self->Destroy           = Steel_Thread_Destroy;
    self->Wait_For_Commands = Steel_Thread_Wait_For_Cmd;
    self->Wait_For_Data     = Steel_Thread_Wait_For_Data;
    self->FlushCmd          = Steel_Thread_Flush_Cmd;

    // Get OpenCL Platform, to which OpenCL Device belongs to;
    cl_platform_id platform;
    ret_code ret = clGetDeviceInfo(given_device, CL_DEVICE_PLATFORM, sizeof(platform),
        &platform, NULL);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, self->Destroy(self), VOID_STEEL_THREAD_PTR);

    // And initialize SCOW wrappers around then
    self->device = Make_Device(given_device);
    OCL_CHECK_EXISTENCE_AND_DO(self->device, self->Destroy(self),
        VOID_STEEL_THREAD_PTR);

    self->platform = Make_Platform(platform);
    OCL_CHECK_EXISTENCE_AND_DO(self->platform, self->Destroy(self),
        VOID_STEEL_THREAD_PTR);

    // Init OpenCL at last
    ret = Init_OpenCL(self);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, self->Destroy(self), NULL);

    return self;
}
