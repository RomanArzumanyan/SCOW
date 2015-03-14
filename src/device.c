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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "devices.h"
#include "device.h"
#include "error.h"

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
    OCL_CHECK_EXISTENCE(self, CL_SUCCESS);

    self->error->Destroy(self->error);
    self->error = VOID_ERROR_PTR;
    free(self);

    return CL_SUCCESS;
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
    ret_code ret = CL_SUCCESS;
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);

    if (param == DEVICE_NAME || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_NAME, CL_DEVICE_NAME_SIZE, self->name, NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_EXTENSIONS || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_EXTENSIONS, CL_DEVICE_EXTENSIONS_SIZE, self->extensions,
        NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_VERSION || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_VERSION, CL_DEVICE_VERSION_SIZE, self->device_version,
        NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DRIVER_VERSION || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DRIVER_VERSION, CL_DRIVER_VERSION_SIZE, self->driver_version,
        NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_EXECUTION_CAPABILITIES || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id, CL_DEVICE_EXECUTION_CAPABILITIES,
                sizeof(cl_device_exec_capabilities), &self->exec_capabilities,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_MAX_COMPUTE_UNITS || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &self->max_compute_units,
        NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_MEM_BASE_ADDR_ALIGN || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_MEM_BASE_ADDR_ALIGN, sizeof(cl_uint),
                &self->mem_base_addr_align, NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_MIN_DATA_TYPE_ALIGN_SIZE
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, sizeof(cl_uint),
                &self->min_data_type_align_size,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_MAX_CLOCK_FREQUENCY || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint),
                &self->max_clock_frequency, NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_GLOBAL_MEM_CACHELINE_SIZE
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(cl_uint),
                &self->global_mem_cacheline_size,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_CHAR
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR, sizeof(cl_uint),
                &self->native_vector_width_char,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_SHORT
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT, sizeof(cl_uint),
                &self->native_vector_width_short,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_INT
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, sizeof(cl_uint),
                &self->native_vector_width_int,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_LONG
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, sizeof(cl_uint),
                &self->native_vector_width_long,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_NATIVE_VECTOR_WIDTH_FLOAT
            || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, sizeof(cl_uint),
                &self->native_vector_width_float,
                NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_GLOBAL_MEM_SIZE || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &self->global_mem_size,
        NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == DEVICE_GLOBAL_MEM_CACHE_SIZE || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
        CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cl_ulong),
                &self->global_mem_cache_size, NULL);

        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);
    }

    if (param == CL_DEVICE_MAX_MEM_ALLOC_SIZE || param == DEVICE_ALL_AVAILABLE)
    {
        ret = clGetDeviceInfo(self->device_id,
            CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong),
            &self->max_alloc_mem_size, NULL);

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

scow_Device* Make_Device(cl_device_id given_device)
{
    scow_Device* self;

    self = (scow_Device*) calloc(1, sizeof(*self));
    OCL_CHECK_EXISTENCE(self, VOID_DEVICE_PTR);

    self->device_id = given_device;
    self->error = Make_Error();

    self->Destroy = Device_Destroy;
    self->Gather_Info = Device_Gather_Info;
    self->Print_Info = Device_Print_Info;

    ret_code ret = self->Gather_Info(self, DEVICE_ALL_AVAILABLE);

    if (ret != CL_SUCCESS)
    {
        err_log_func(ret);
        self->Destroy(self);
        return VOID_DEVICE_PTR;
    }

    return self;
}

cl_uint Get_MA_Subdevices_Num(
    cl_device_id                        given_device,
    const cl_device_partition_property  *properties,
    ret_code                            *ret)
{
    OCL_CHECK_EXISTENCE(given_device, ARG_NOT_FOUND);
    OCL_CHECK_EXISTENCE(properties, ARG_NOT_FOUND);
    OCL_CHECK_EXISTENCE(ret, ARG_NOT_FOUND);

    cl_uint num_devices;

    *ret = clCreateSubDevices(given_device, properties, 0, NULL, &num_devices);
    OCL_DIE_ON_ERROR(*ret, CL_SUCCESS, NULL, 0);

    return num_devices;
}

cl_device_id* Make_Subdevices(
    cl_device_id                        given_device,
    const cl_device_partition_property  *properties,
    size_t                              num_devices,
    ret_code                            *ret)
{
    OCL_CHECK_EXISTENCE(given_device, ARG_NOT_FOUND);
    OCL_CHECK_EXISTENCE(properties, ARG_NOT_FOUND);
    OCL_CHECK_EXISTENCE(ret, ARG_NOT_FOUND);

    cl_device_id *sub_devices = (cl_device_id*)calloc(num_devices, sizeof(*sub_devices));
    OCL_CHECK_EXISTENCE(sub_devices, VOID_OPENCL_DEVICE_ID_PTR);

    *ret = clCreateSubDevices(given_device, properties, num_devices, 
        sub_devices, NULL);

    if (*ret != CL_SUCCESS){
        free(sub_devices);
        return (cl_device_id*)NULL;
    }

    return sub_devices;
}
