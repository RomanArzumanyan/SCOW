/*
* @file devices.c
* @brief implementation for supported OpenCL Devices list collector
*
* @see devices.c
*
* Copyright 2014 Roman Arzumanyan (roman.arzum@gmail.com)
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

#include "err_codes.h"
#include "error.h"
#include "platforms.h"
#include "devices.h"
#include "device.h"

cl_device_id *g_all_CPU_list = VOID_OPENCL_DEVICE_ID_PTR;
cl_device_id *g_all_GPU_list = VOID_OPENCL_DEVICE_ID_PTR;

size_t g_all_CPU_num = 0;
size_t g_all_GPU_num = 0;

/**
* @brief This function get number of OpenCL Devices of wanted type under 
* given platform & increments number of globally available devices of that 
* kind in variables g_all_CPU_num and g_all_GPU_num.
*
* @param[in] parent_platform OpenCL platform, under wich number of OpenCL
* Devices of wanted type will be queried.
*
* @param[out] wanted_device_type type of wanted OpenCL Device.
*
* @return number of devices found.
*/
static size_t Get_Num_Devices(
	cl_platform_id	parent_platform,
	cl_device_type	wanted_device_type)
{
	ret_code ret = CL_SUCCESS;
	cl_uint current_platform_device_num = 0;

	// If no platforms are found at all
	if (!parent_platform){
		ret = CANT_FIND_PLATFORM;
		OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);
	}

	// Only CPU & GPU are supported.
	switch (wanted_device_type){
	case CL_DEVICE_TYPE_CPU:
		ret = clGetDeviceIDs(parent_platform, CL_DEVICE_TYPE_CPU, 0,
			NULL, &current_platform_device_num);
        if (ret != CL_SUCCESS){
            current_platform_device_num = 0;
        }
		g_all_CPU_num += current_platform_device_num;

		break;

	case CL_DEVICE_TYPE_GPU:
		ret = clGetDeviceIDs(parent_platform, CL_DEVICE_TYPE_GPU, 0,
			NULL, &current_platform_device_num);
        if (ret != CL_SUCCESS){
            current_platform_device_num = 0;
        }
		g_all_GPU_num += current_platform_device_num;

		break;

	default:
        ocl_error_message(VALUE_OUT_OF_RANGE);
        current_platform_device_num = 0;
		break;
	}

	return current_platform_device_num;
}

/**
* @brief This function get list of OpenCL Devices of wanted type under
* given platform.
*
* @param[in] parent_platform OpenCL platform, under wich list of OpenCL
* Devices of wanted type will be gathered.
* @param[in] num_devices number of OpenCL Devices to write into list.
* @param[in] wanted_device_type type of wanted OpenCL Device.
*
* @param[out] device_ids array, in which list will be written
*
* @return number of devices found.
*/
static ret_code Get_Devices(
	cl_platform_id	parent_platform,
	const size_t	num_devices,
	cl_device_type	wanted_device_type,
	cl_device_id	*device_ids)
{
	ret_code ret = clGetDeviceIDs(parent_platform, wanted_device_type,
		num_devices, device_ids, NULL);

	OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

	return ret;
}

ret_code Collect_Devices_List(void)
{
	ret_code ret = CL_SUCCESS;
	
	// Arrays with numbers of CPU & GPU for every found platform
	size_t
		*num_cpus,
		*num_gpus;

	cl_bool no_platforms = 
		(g_num_platforms == 0) || (!g_all_platforms_list);

	if (no_platforms){
		ret = CANT_FIND_PLATFORM;
		OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);
	}

	num_cpus = (size_t*)calloc(g_num_platforms, sizeof(*num_cpus));
	num_gpus = (size_t*)calloc(g_num_platforms, sizeof(*num_gpus));

	// Get number of CPUs & GPUs under every found platform
	for (size_t platform = 0; platform < g_num_platforms; platform++){
		num_cpus[platform] = Get_Num_Devices(g_all_platforms_list[platform],
			CL_DEVICE_TYPE_CPU);

		num_gpus[platform] = Get_Num_Devices(g_all_platforms_list[platform],
			CL_DEVICE_TYPE_GPU);
	}

	if (g_all_CPU_num){
		g_all_CPU_list = (cl_device_id*)calloc(g_all_CPU_num, sizeof(*g_all_CPU_list));
	}

	if (g_all_GPU_num){
		g_all_GPU_list = (cl_device_id*)calloc(g_all_GPU_num, sizeof(*g_all_GPU_list));
	}

	if (g_all_CPU_num + g_all_GPU_num == 0){
		free(num_cpus);
		free(num_gpus);

		ret = CANT_FIND_DEVICE;
		OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);
	}

	// Now get Devices in global list
	cl_device_id
		*ptr_cpu = g_all_CPU_list,
		*ptr_gpu = g_all_GPU_list;

	for (size_t platform = 0; platform < g_num_platforms; platform++){
		// Get CPUs
		if (num_cpus[platform] != 0){
			ret = Get_Devices(g_all_platforms_list[platform], num_cpus[platform],
				CL_DEVICE_TYPE_CPU, ptr_cpu);
			ptr_cpu += num_cpus[platform];

			if (ret != CL_SUCCESS){
				free(num_cpus);
				free(num_gpus);

				ret = CANT_FIND_DEVICE;
				OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);
			}
		}

		// Get GPUs
		if (num_gpus[platform] != 0){
			ret = Get_Devices(g_all_platforms_list[platform], num_gpus[platform],
				CL_DEVICE_TYPE_GPU, ptr_gpu);
			ptr_gpu += num_gpus[platform];

			if (ret != CL_SUCCESS){
				free(num_cpus);
				free(num_gpus);

				ret = CANT_FIND_DEVICE;
				OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);
			}
		}
	}

	// Release allocated memory
	free(num_cpus);
	free(num_gpus);

	num_cpus = NULL;
	num_gpus = NULL;

	return ret;
}

ret_code Erase_Devices_List(void)
{
	if (g_all_CPU_list){
		free(g_all_CPU_list);
		g_all_CPU_list = VOID_OPENCL_DEVICE_ID_PTR;
		g_all_CPU_num = 0;
	}

	if (g_all_GPU_list){
		free(g_all_GPU_list);
		g_all_GPU_list = VOID_OPENCL_DEVICE_ID_PTR;
		g_all_GPU_num = 0;
	}

	return CL_SUCCESS;
}

cl_device_id Pick_Device_By_Name(const char* const device_name)
{
	// 2 different types of OpenCL Devices are supported at the moment
	size_t devices_types = 2;
	cl_device_id *devices = g_all_CPU_list;
	size_t num_devices = g_all_CPU_num;

	for (size_t i = 0; i < devices_types; i++){
		OCL_CHECK_EXISTENCE(devices, NULL);
		for (size_t device = 0; device < num_devices; device++){
			OCL_CHECK_EXISTENCE(devices[device], NULL);

			// First we get device name
			size_t name_len;

			ret_code ret = clGetDeviceInfo(devices[device],
				CL_DEVICE_NAME, NULL, NULL, &name_len);
			OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, NULL);

			char *name = (char*)calloc(name_len, sizeof(*name));

            ret = clGetDeviceInfo(devices[device],
                CL_DEVICE_NAME, name_len, name, NULL);
			if (ret != CL_SUCCESS){
				free(name);
				OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, NULL);
			}

			// Then check it against given
			if (strstr(name, device_name)){
				free(name);
				return devices[device];
			}

			free(name);
		}

		devices = g_all_GPU_list;
		num_devices = g_all_GPU_num;

	}

	// If not found, return NULL
	return NULL;
}

cl_device_id Pick_Device_By_Type(const cl_device_type device_type)
{
	switch (device_type){
	case CL_DEVICE_TYPE_CPU:
		OCL_CHECK_EXISTENCE(g_all_CPU_list, NULL);
		return g_all_CPU_num > 0 ? g_all_CPU_list[0] : NULL;
		break;

	case CL_DEVICE_TYPE_GPU:
		OCL_CHECK_EXISTENCE(g_all_GPU_list, NULL);
		return g_all_GPU_num > 0 ? g_all_GPU_list[0] : NULL;
		break;

	default:
		return NULL;
		break;
	}
}

cl_device_id Pick_Device_By_Platform(
	const cl_platform_id	parent_platform,
	const cl_device_type	device_type)
{
    cl_device_id *devices;
    size_t num_devices;

    switch (device_type){
    case CL_DEVICE_TYPE_CPU:
        devices = g_all_CPU_list;
        num_devices = g_all_CPU_num;
        break;
        
    case CL_DEVICE_TYPE_GPU:
        devices = g_all_GPU_list;
        num_devices = g_all_GPU_num;
        break;

    default:
        return NULL;
        break;
    }

    OCL_CHECK_EXISTENCE(devices, NULL);
    if (!num_devices){
        return NULL;
    }

    for (size_t device = 0; device < num_devices; device++){
        cl_platform_id curr_platform;

        ret_code ret = clGetDeviceInfo(devices[device],
            CL_DEVICE_PLATFORM, sizeof(curr_platform), &curr_platform, NULL);
        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, NULL);

        if (curr_platform == parent_platform){
            return devices[device];
        }
    }

    return NULL;

}

cl_device_id Pick_Next_Device(const cl_device_id current_device)
{
    OCL_CHECK_EXISTENCE(current_device, NULL);

    cl_device_type curr_device_type;
    cl_device_id *devices;
    size_t num_devices;

    ret_code ret = clGetDeviceInfo(current_device, CL_DEVICE_TYPE,
        sizeof(curr_device_type), &curr_device_type, NULL);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, NULL);

    switch (curr_device_type){
    case CL_DEVICE_TYPE_CPU:
        devices = g_all_CPU_list;
        num_devices = g_all_CPU_num;
        break;

    case CL_DEVICE_TYPE_GPU:
        devices = g_all_GPU_list;
        num_devices = g_all_GPU_num;
        break;

    default:
        return NULL;
        break;
    }

    // Current Device is within list of registered OpenCL Devices & is not last
    cl_bool good_device =
        (&current_device >= devices) && (&current_device < devices + num_devices - 1);

    return good_device ? *(&current_device + 1) : NULL;
}

cl_device_id Pick_Prev_Device(const cl_device_id current_device)
{
    OCL_CHECK_EXISTENCE(current_device, NULL);

    cl_device_type curr_device_type;
    cl_device_id *devices;
    size_t num_devices;

    ret_code ret = clGetDeviceInfo(current_device, CL_DEVICE_TYPE,
        sizeof(curr_device_type), &curr_device_type, NULL);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, NULL);

    switch (curr_device_type){
    case CL_DEVICE_TYPE_CPU:
        devices = g_all_CPU_list;
        num_devices = g_all_CPU_num;
        break;

    case CL_DEVICE_TYPE_GPU:
        devices = g_all_GPU_list;
        num_devices = g_all_GPU_num;
        break;

    default:
        return NULL;
        break;
    }

    // Current Device is within list of registered OpenCL Devices & is not first
    cl_bool good_device =
        (&current_device > devices) && (&current_device < devices + num_devices);

    return good_device ? *(&current_device - 1) : NULL;
}

ret_code List_All_Devices(cl_device_type dev_type)
{
    cl_device_id *devices = NULL;
    size_t num_devices = 0;
    if (dev_type == CL_DEVICE_TYPE_CPU){
        devices = g_all_CPU_list;
        num_devices = g_all_CPU_num;
    }
    else if (dev_type == CL_DEVICE_TYPE_GPU){
        devices = g_all_GPU_list;
        num_devices = g_all_GPU_num;
    }
    else{
        return VALUE_OUT_OF_RANGE;
    }

    if (!devices || !num_devices){
        return CANT_FIND_DEVICE;
    }

    for (size_t i = 0; i < num_devices; i++){
        char name[CL_DEVICE_NAME_SIZE];
        ret_code ret = CL_SUCCESS;

        ret = clGetDeviceInfo(devices[i],
            CL_DEVICE_NAME, CL_DEVICE_NAME_SIZE, name, NULL);
        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_DEVICE_PARAM);

        printf("%s\n", name);
    }
}
