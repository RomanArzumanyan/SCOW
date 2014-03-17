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

#include "err_codes.h"
#include "platforms.h"
#include "platform.h"

cl_platform_id* g_all_platforms_list = VOID_OPENCL_PLATFORM_ID_PTR;
size_t g_num_platforms = 0;

ret_code Get_Num_Platforms(void)
{
	cl_int ret = CL_SUCCESS;

	ret = clGetPlatformIDs(0, NULL, &g_num_platforms);

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