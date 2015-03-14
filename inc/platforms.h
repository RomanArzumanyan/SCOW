/*
* @file platforms.h
* @brief header for supported OpenCL Platforms list collector
*
* @see platforms.c
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

#pragma once

#include "CL/cl.h"
#include "typedefs.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Global list of all available platforms & it's size
extern cl_platform_id *g_all_platforms_list;
extern size_t g_num_platforms;

/*! \def VOID_OPENCL_PLATFORM_ID_PTR
* Void pointer to list of OpenCL Platforms
*/
#undef VOID_OPENCL_PLATFORM_ID_PTR
#define VOID_OPENCL_PLATFORM_ID_PTR ((cl_platform_id*)0x0)

/**
* This function detects all available OpenCL platforms & allocates memory for
* list of them.
*
* @return \ref CL_SUCCESS in case of success, error code of type ret_code otherwise
*
* @warning always use Erase_Platforms_List function to free memory, 
* allocated by this function.
*/
ret_code Collect_Platforms_List(void);

/**
* This function deallocates memory for list of all available OpenCL platforms.
*
* @return \ref CL_SUCCESS in case of success, error code of type ret_code otherwise
*/
ret_code Erase_Platforms_List(void);

/**
* @brief This function finds platform with given name within list of found OpenCL platforms
*
* @param name name of wanted platform
*
* @return cl_platform_id in case of success, NULL otherwise
*/
cl_platform_id Pick_Platform_By_Name(const char* name);

/**
* @brief This function finds first platform within list of found OpenCL platforms
*
* @return cl_platform_id in case of success, NULL otherwise
*/
cl_platform_id Pick_First_Platform(void);

/**
* @brief This function finds last platform within list of found OpenCL platforms
*
* @return cl_platform_id in case of success, NULL otherwise
*/
cl_platform_id Pick_Last_Platform(void);

/**
* @brief This function finds next platform within list of found OpenCL platforms
* by given current platform. Use it to traverse in platforms.
*
* @return cl_platform_id in case of success, NULL otherwise
*/
cl_platform_id Pick_Next_Platform(cl_platform_id current_platform);

/**
* @brief This function finds previous platform within list of found OpenCL platforms
* by given current platform. Use it to traverse in platforms.
*
* @return cl_platform_id in case of success, NULL otherwise
*/
cl_platform_id Pick_Prev_Platform(cl_platform_id current_platform);

/**
* @brief This function finds platform within list of registered OpenCL platforms,
* which contains OpenCL Device of wanted type.
*
* @return cl_platform_id in case of success, NULL otherwise
*/
cl_platform_id Pick_Platform_By_Device_Type(const cl_device_type device_type);

#ifdef __cplusplus
}
#endif