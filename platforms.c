/*
* @file platforms.c
* @brief implementation for supported OpenCL Platforms list collector
*
* @see platforms.h
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

#include "err_codes.h"
#include "platforms.h"
#include "platform.h"
#include "devices.h"
#include "error.h"

cl_platform_id* g_all_platforms_list = VOID_OPENCL_PLATFORM_ID_PTR;
size_t g_num_platforms = 0;

ret_code Get_Num_Platforms(void)
{
	cl_int ret = CL_SUCCESS;

	cl_uint num_platforms;
	ret = clGetPlatformIDs(0, NULL, &num_platforms);

	g_num_platforms = num_platforms;

	OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, 0);

	return ret;
}

ret_code Collect_Platforms_List(void)
{
	ret_code ret = CL_SUCCESS;
	Get_Num_Platforms();

	if (g_num_platforms == 0){
		ret_code ret = CANT_FIND_PLATFORM;
		OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);
	}

	g_all_platforms_list = (cl_platform_id*)calloc(g_num_platforms, sizeof(*g_all_platforms_list));

	// Now collect the list of ID's itself
	ret = clGetPlatformIDs(g_num_platforms, g_all_platforms_list, NULL);
	OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

	return ret;
}

ret_code Erase_Platforms_List(void)
{
	if (g_all_platforms_list){
		free(g_all_platforms_list);
		g_all_platforms_list = VOID_OPENCL_PLATFORM_ID_PTR;
		g_num_platforms = 0;
	}

	return CL_SUCCESS;
}

cl_platform_id Pick_Platform_By_Name(const char* platform_name)
{
	OCL_CHECK_EXISTENCE(g_all_platforms_list, NULL);
	for (size_t platform = 0; platform < g_num_platforms; platform++){
		OCL_CHECK_EXISTENCE(g_all_platforms_list[platform], NULL);

		// First we get platform name
		size_t name_len;

		ret_code ret = clGetPlatformInfo(g_all_platforms_list[platform],
			CL_PLATFORM_NAME, NULL, NULL, &name_len);
		OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, NULL);

		char *name = (char*)calloc(name_len, sizeof(*name));

		ret = clGetPlatformInfo(g_all_platforms_list[platform],
			CL_PLATFORM_NAME, name_len, name, NULL);
		if (ret != CL_SUCCESS){
			free(name);
			OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, NULL);
		}

		// Then check it against given
		if (strstr(name, platform_name)){
			free(name);
			return g_all_platforms_list[platform];
		}

		free(name);
	}

	// If not found, return NULL
	return NULL;
}

cl_platform_id Pick_First_Platform(void)
{
	return g_all_platforms_list ? NULL : g_all_platforms_list[0];
}

cl_platform_id Pick_Last_Platform(void)
{
	return g_all_platforms_list ? NULL : g_all_platforms_list[g_num_platforms];
}

cl_platform_id Pick_Next_Platform(cl_platform_id current_platform)
{
	// Current platform is within list of OpenCL Platforms & it's not last
	cl_bool good_platform =
		(&current_platform >= g_all_platforms_list) &&
		(&current_platform <  g_all_platforms_list + g_num_platforms - 1);

	return good_platform ? *(&current_platform + 1) : NULL;
}

cl_platform_id Pick_Prev_Platform(cl_platform_id current_platform)
{
	// Current platform is within list of OpenCL Platforms & it's not first
	cl_bool good_platform =
		(&current_platform > g_all_platforms_list) &&
		(&current_platform < g_all_platforms_list + g_num_platforms);

	return good_platform ? *(&current_platform - 1) : NULL;
}

cl_platform_id Pick_Platform_By_Device_Type(const cl_device_type device_type)
{
	// Looks ugly, but MSVC can't compile it other way
	switch (device_type){
	case CL_DEVICE_TYPE_CPU:
		// Check that there are registered OpenCL Devices of wanted type
		OCL_CHECK_EXISTENCE(g_all_CPU_list, NULL);
		if (g_all_CPU_num == 0){
			return NULL;
		}

		// Check to what OpenCL Platform does registered Devices from list belongs to
		for (size_t device = 0; device < g_all_CPU_num; device++){
			cl_platform_id platform;

			ret_code ret = clGetDeviceInfo(g_all_CPU_list[device],
				CL_DEVICE_PLATFORM, sizeof(platform), &platform, NULL);
			OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, NULL);

			return platform;
		}

		break;

	case CL_DEVICE_TYPE_GPU:
		// Check that there are registered OpenCL Devices of wanted type
		OCL_CHECK_EXISTENCE(g_all_GPU_list, NULL);
		if (g_all_GPU_num == 0){
			return NULL;
		}

		// Check to what OpenCL Platform does registered Devices from list belongs to
		for (size_t device = 0; device < g_all_GPU_num; device++){
			cl_platform_id platform;

			ret_code ret = clGetDeviceInfo(g_all_GPU_list[device],
				CL_DEVICE_PLATFORM, sizeof(platform), &platform, NULL);
			OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, NULL);

			return platform;
		}

		break;

	default:
		return NULL;
		break;
	}

	// If not found, return NULL
	return NULL;
}