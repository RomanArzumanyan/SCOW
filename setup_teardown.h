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