/*
* @file setup_teardown.h
* @brief header for functions, that maintain SCOW setup & tear-down
*
* @see setup_teardown.h
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
#include "typedefs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief this functions set up SCOW - collect all OpenCL Platforms,
* Devices, etc
*
* @return \ref CL_SUCCESS in case of success, error code of type ret_code otherwise
*
* @warning always use SCOW_Tear_Down function to release resources,
* allocated by this function
*/
ret_code SCOW_Set_Up(void);

/**
* @brief this functions tear down SCOW - releases different objects,
* deallocates memory, etc
*
* @return \ref CL_SUCCESS in case of success, error code of type ret_code otherwise
*/
ret_code SCOW_Tear_Down(void);

#ifdef __cplusplus
}
#endif