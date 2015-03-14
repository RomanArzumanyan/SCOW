/*
* @file devices.h
* @brief header for supported OpenCL Devices list collector
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

#pragma once

#include "CL/cl.h"
#include "typedefs.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Only CPU & GPU are supported for moment
extern cl_device_id 
	*g_all_CPU_list,
	*g_all_GPU_list;

extern size_t
	g_all_CPU_num,
	g_all_GPU_num;

/*! \def VOID_OPENCL_DEVICE_ID_PTR
* Void pointer to list of OpenCL Devices
*/
#undef VOID_OPENCL_DEVICE_ID_PTR
#define VOID_OPENCL_DEVICE_ID_PTR ((cl_device_id*)0x0)

/**
* This function detects all available OpenCL Devices & allocates memory for
* list of them.
*
* @return \ref CL_SUCCESS in case of success, error code of type ret_code otherwise
*
* @warning always use Erase_Devices_List function to free memory,
* allocated by this function.
*/
ret_code Collect_Devices_List(void);

/**
* This function deallocates memory for list of all available OpenCL Devices.
*
* @return \ref CL_SUCCESS in case of success, error code of type ret_code otherwise
*/
ret_code Erase_Devices_List(void);

/**
* @brief This function finds first OpenCL Device with given name within list of registered
* devices.
*
* @param[in] name wanted OpenCL Device name
*
* @return cl_device_id of found OpenCL Device, NULL otherwise
*/
cl_device_id Pick_Device_By_Name(const char* const name);

/**
* @brief This function finds first OpenCL Device of given type within list of registered
* devices.
*
* @param[in] device_type wanted OpenCL Device type
*
* @return cl_device_id of found OpenCL Device, NULL otherwise
*/
cl_device_id Pick_Device_By_Type(const cl_device_type device_type);

/**
* @brief This function finds OpenCL Device of given type under given platform
*
* @param[in] name parent_platform OpenCL Platform, under which Device should
* be found
* @param[in] device_type wanted OpenCL Device type
*
* @return cl_device_id of found OpenCL Device, NULL otherwise
*/
cl_device_id Pick_Device_By_Platform(
	const cl_platform_id	parent_platform,
	const cl_device_type	device_type);

/**
 * @brief This function returns next OpenCL Device of same type & parent OpenCL Platform
 * in list of registered OpenCL Devices. Use it to navigate through Devices
 *
 * @param[in] current_device current OpenCL Device
 *
 * @return cl_device_id of next OpenCL Device, NULL otherwise
 */
cl_device_id Pick_Next_Device(const cl_device_id current_device);

/**
* @brief This function returns previous OpenCL Device of same type & parent OpenCL Platform
* in list of registered OpenCL Devices. Use it to navigate through Devices
*
* @param[in] current_device current OpenCL Device
*
* @return cl_device_id of previous OpenCL Device, NULL otherwise
*/
cl_device_id Pick_Prev_Device(const cl_device_id current_device);

/**
* @brief This function prints names of all OpenCL Devices of given type.
*
* @param[in] dev_type type of wanted OpenCL Device
*
* @return CL_SUCCESS in case of success, error code otherwise
*/
ret_code List_All_Devices(cl_device_type dev_type);

#ifdef __cplusplus
}
#endif
