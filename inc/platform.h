/*
 * @file platform.h
 * @brief Provides basic abstraction for OpenCL paltform
 *
 * @see platform.c
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

#ifndef CL_PLATFORM_H_
#define CL_PLATFORM_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*! \def VOID_PLATRORM_INFO_PTR
 * Void pointer to Platform Info.
 */
#undef VOID_PLATRORM_INFO_PTR
#define VOID_PLATFORM_INFO_PTR      ((scow_Platform_Info*)0x0)

#include "typedefs.h"

struct scow_Platform;

typedef enum PLATFORM_INFO_PARAM
{
    PLATFORM_PROFILE_SUPPORTED = 0,
    /*!< OpenCL profile, supported by platform. */

    PLATFORM_VERSION,
    /*!< OpenCL version, supported by platform. */

    PLATFORM_NAME,
    /*!< Name of the platform. */

    PLATFORM_VENDOR,
    /*!< Vendor of the platform. */

    PLATFORM_EXTENSIONS,
    /*!< List of the OpenCL extensions, supported by the platform. */

    PLATFORM_ALL_AVAILABLE
/*!< Combination of all parameters above. */

} PLATFORM_INFO_PARAM;

/*! \struct scow_Platform_Info
 *
 *  This structure  basic functionality for collecting information about Opencl
 *  platform.
 */
typedef struct scow_Platform_Info
{
    struct scow_Platform* parent_platform;
    /*!< Parent scow_Platform, information about what is contained. */

    /*! @name Platform properties. */
    /*!@{*/
    char *cstr_profile_supported,
    /*!< OpenCL profile, supported by platform. */

    *cstr_version,
    /*!< OpenCL version, supported by platform. */

    *cstr_name,
    /*!< Name of the platform. */

    *cstr_vendor,
    /*!< Vendor of the platform. */

    *cstr_extensions;
    /*!< List of the OpenCL extensions, supported by the platform. */
    /*!@}*/

    /*! @name Function pointers. */
    /*!@{*/
    ret_code (*Destroy)(struct scow_Platform_Info *self);
    /*!< Points on Platform_Info_Destroy(). */

    ret_code (*Print)(const struct scow_Platform_Info *self,
            PLATFORM_INFO_PARAM param);
    /*!< Points on Platform_Info_Print(). */

    const char* (*Get_Parameter)(const struct scow_Platform_Info *self,
            PLATFORM_INFO_PARAM param);
    /*!< Points on Platform_Info_Get_Parameter(). */
    /*!@}*/

} scow_Platform_Info;

/**
 * This function allocates memory for structure of type 'scow_Platform_Info', set
 * function pointers, structure fields & returns pointer to structure created
 * in case of success.
 *
 * @param[in] parent_platform Pointer to struct of type 'scow_Platform'
 * which is parent OpenCL platform, information about what is contained.
 *
 * @return pointer to created structure in case of success, VOID_DEVICE_PTR pointer otherwise.
 *
 * @warning always use 'Destroy' function pointer to free memory, allocated by
 * this function.
 */
scow_Platform_Info* Make_Platform_Info(struct scow_Platform* parent_platform);

typedef enum PLATFORM_CREATION_MODE
{
    PLATFORM_CREATE_QUICK = 0,
    /*!< Create platform quick without gathering information about it. */

    PLATFORM_CREATE_AND_GATHER_INFO
/*!< Create platform and gather information about it. It may be slow. */

} PLATFORM_CREATION_MODE;

/*! \def VOID_PLATFORM_PTR
 * Void pointer to Platform
 */
#undef VOID_PLATFORM_PTR
#define VOID_PLATFORM_PTR       ((scow_Platform*)0x0)

struct scow_Error;

/*! \struct scow_Platform
 *
 *  This structure is wrapper for cl_platform_id structure, provided by
 *  OpenCL API. It includes basic functionality for platform, such as:
 *     - Auto-collecting list of available OpenCL platforms
 *     - Navigation through list of platforms
 *     - Auto-detecting available OpenCL Devices of desired type
 *     - Making particular OpenCL Device default for platform
 *     - Making platform default for parent OpenCL Steel Thread structure
 *
 *  @see 'scow_Steel_Thread' structure description for details about parent
 *  OpenCL Steel Thread
 */
typedef struct scow_Platform
{
    struct scow_Error* error;
    /*!< Object for errors handling. */

    cl_platform_id platform;
    /*!< OpenCL platform. */

    scow_Platform_Info* info;
    /*!< Information about OpenCL platform. */

    /*! @name Function pointers. */
    /*!@{*/
    ret_code (*Destroy)(struct scow_Platform *self);
    /*!< Points on Platform_Destroy(). */
    /*!@}*/

} scow_Platform;

scow_Platform* Make_Platform(cl_platform_id given_platform);

#ifdef __cplusplus
}
#endif

#endif /* CL_PLATFORM_H_ */
