/*
 * @file steel_thread.h
 * @brief Provides container for most frequently used OpenCL objects - contex & queues
 *
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

#ifndef CL_STEEL_THREAD_H_
#define CL_STEEL_THREAD_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "typedefs.h"

/*! \def CL_BUILD_PARAMS_STRING_SIZE
 * Maximal length of string with additional program build parameters
 */
#undef CL_BUILD_PARAMS_STRING_SIZE
#define CL_BUILD_PARAMS_STRING_SIZE (256)

/*! \def CL_KERNEL_FILE_NAME_SIZE
 * Maximal length of path to kernel
 */
#undef CL_KERNEL_FILE_NAME_SIZE
#define CL_KERNEL_FILE_NAME_SIZE    (256)

/*! \def VOID_STEEL_THREAD_PTR
 * Void pointer to Steel Thread
 */
#undef VOID_STEEL_THREAD_PTR
#define VOID_STEEL_THREAD_PTR       ((scow_Steel_Thread*)0x0)

struct scow_Error;
struct scow_Device;
struct scow_Platform;

/*! \struct scow_Steel_Thread
 *
 * This structure contains minimal amount of objects, that are required to
 * work with OpenCL, such as:
 *   - List of OpenCL platforms (which includes list of OpenCL Devices for each
 *     platform) and information about platforms & devices
 *   - OpenCL context
 *   - OpenCL program
 *   - Command queues:
 *     - Host to Device
 *     - Device to Host
 *     - Device to Device
 *     - Queue for kernel ND range
 *
 * Also it provides functionality as follows:
 *   - Auto-detection of OpenCL platforms & OpenCL Devices
 *   - Ability to load/store pre-compiled OpenCL program
 *   - Ability to load & compile OpenCL program from source files and/or strings
 *
 * @example cl_steel_thread_t_sample.c
 */
typedef struct scow_Steel_Thread
{
    struct scow_Error* error;
    /*!< Structure for errors handling. */

    struct scow_Device *device;
    /*!< OpenCL Device, around which Steel Thread is wrapped. */

    struct scow_Platform* platform;
    /*!< OpenCL platform, to which Device belongs to. */

    char init_params[CL_BUILD_PARAMS_STRING_SIZE];
    /*!< Additional built parameters, that will be passed at OpenCL program build
     * stage*/

    cl_context context;
    /*!< OpenCL context. */

    /*! @name Command queues.
     * These are command queues, that are used most often - for Host-Device
     * intercommunication & kernel execution. */
    /*!@{*/
    cl_command_queue q_cmd,
    /*!< Generic queue for kernel execution. */

    q_data_htod,
    /*!< Generic queue for Host to Device data transmission. */

    q_data_dtoh,
    /*!< Generic queue for Device to Host data transmission. */

    q_data_dtod;
    /*!< Generic queue for Device to Device data transmission. */
    /*!@}*/

    /*! @name Function pointers. */
    /*!@{*/
    ret_code (*Destroy)(struct scow_Steel_Thread *self);
    /*!< Points on Steel_Thread_Destroy(). */

    ret_code (*Wait_For_Commands)(struct scow_Steel_Thread *self);
    /*!< Points on Steel_Thread_Wait_For_Commands(). */

    ret_code (*Wait_For_Data)(struct scow_Steel_Thread *self);
    /*!< Points on Steel_Thread_Wait_For_Data(). */

    ret_code(*FlushCmd)(struct scow_Steel_Thread* self);
    /*!< Points on Steel_Thread_Flush_Cmd()*/
/*!@}*/

} scow_Steel_Thread;

/*!
 * This function allocates memory for structure, initialize OpenCL & set
 * function pointers
 *
 * @param[in] given_device OpenCL device, around which Steel Thread will be wrapped.
 *
 * @return pointer to allocated structure in case of success,
 * \ref VOID_STEEL_THREAD_PTR otherwise
 *
 * @warning always use 'Destroy' function pointer to free memory, allocated by
 * this function.
 */
scow_Steel_Thread* Make_Steel_Thread(cl_device_id given_device);

#ifdef __cplusplus
}
#endif

#endif /* CL_STEEL_THREAD_H_ */
