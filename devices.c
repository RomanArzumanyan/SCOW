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

#include "err_codes.h"
#include "error.h"
#include "platforms.h"
#include "devices.h"

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
	cl_platform_id	*parent_platform,
	cl_device_type	wanted_device_type)
{
	ret_code ret = CL_SUCCESS;
	size_t current_platform_device_num = 0;

	// If no platforms are found at all
	if (!parent_platform){
		ret = CANT_FIND_PLATFORM;
		OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);
	}

	// At current moment only CPU & GPU are supported.
	switch (wanted_device_type){
	case CL_DEVICE_TYPE_CPU:
		ret = clGetDeviceIDs(parent_platform, CL_DEVICE_TYPE_CPU, 0,
			NULL, &current_platform_device_num);
		OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, 0);
		g_all_CPU_num += current_platform_device_num;

		break;

	case CL_DEVICE_TYPE_GPU:
		ret = clGetDeviceIDs(parent_platform, CL_DEVICE_TYPE_GPU, 0,
			NULL, &current_platform_device_num);
		OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, 0);
		g_all_GPU_num += current_platform_device_num;

		break;

	default:
		ret = VALUE_OUT_OF_RANGE;
		OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);
		break;
	}

	return current_platform_device_num;
}

static ret_code Get_Devices(
	cl_platform_id	*parent_platform,
	cl_device_id	*device_ids,
	cl_device_type	wanted_device_type,
	const size_t	num_devices)
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
		g_all_GPU_list = (cl_device_id*)calloc(g_all_CPU_num, sizeof(*g_all_GPU_list));
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
			ret = Get_Devices(g_all_platforms_list[platform], ptr_cpu,
				CL_DEVICE_TYPE_CPU, num_cpus[platform]);
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
			ret = Get_Devices(g_all_platforms_list[platform], ptr_gpu,
				CL_DEVICE_TYPE_GPU, num_gpus[platform]);
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
		cl_device_id *g_all_CPU_list = VOID_OPENCL_DEVICE_ID_PTR;
		g_all_CPU_num = 0;
	}

	if (g_all_GPU_list){
		free(g_all_GPU_list);
		cl_device_id *g_all_GPU_list = VOID_OPENCL_DEVICE_ID_PTR;
		g_all_GPU_num = 0;
	}

	return CL_SUCCESS;
}