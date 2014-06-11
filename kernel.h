/*
 * @file kernel.h
 * @brief Provides basic abstraction for OpenCL kernel
 *
 * @see kernel.c
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

#ifndef CL_MINIMAL_KERNEL_H_
#define CL_MINIMAL_KERNEL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "timer.h"

/*! \def VOID_KERNEL_PTR
 * Void pointer to Kernel
 */
#undef VOID_KERNEL_PTR
#define VOID_KERNEL_PTR     ((scow_Kernel*)0x0)

/*! \def OCL_KERNEL_NAME_MAX_LEN
 * Maximal length of Kernel length string
 */
#undef OCL_KERNEL_NAME_MAX_LEN
#define OCL_KERNEL_NAME_MAX_LEN     (256)

/*! \def MAX_NUM_DIMENSIONS
 * Maximal number of dimensions for kernel run
 */
#undef MAX_NUM_DIMENSIONS
#define MAX_NUM_DIMENSIONS          (3)

/*! \cond PRIVATE */
// Identifies, what event is in favor for MinimalKernel Check_Status() function
#undef INTERNAL_EVT_PRIORITY
#define INTERNAL_EVT_PRIORITY       (0)

#undef EXTERNAL_EVT_PRIORITY
#define EXTERNAL_EVT_PRIORITY       (1)
/*! \endcond */

typedef enum OPENCL_SOURCES_MODE
{
    /*! Read kernel source from files. */
    READ_FROM_FILES = 0,

    /*! Read kernel source from string. */
    READ_FROM_STRING,

    /*! Load pre-built OpenCL program & make kernel from it. */
    READ_FROM_BINARY
} OPENCL_SOURCES_MODE;

/*! \struct scow_Kernel_Arg
 *
 * This structure is used in Kernel launching mechanism. It contains
 * size of kernel argument in bytes & pointer to argument.
 */
typedef struct scow_Kernel_Arg
{
    size_t size;
    /*!< Size of argument in bytes. */

    void* ptr;
/*!< Pointer to argument. */

} scow_Kernel_Arg;

/*! Callback, that can be called on particular OpenCL event status. */
typedef void (*OpenCL_Callback)(cl_event event,
        cl_int event_command_exec_status, void* user_data);

/*! \struct scow_Kernel
 *
 *  This structure is wrapper for cl_kernel provided by OpenCL API.
 *  It provides basic functionality for OpenCL kernel, such as:
 *    - Auto-initializing of the kernel by name & parent Steel Thread
 *    - Ability to measure kernel execution time
 *    - Ability to check available ND sizes before kernel execution
 *
 *  @see 'scow_Steel_Thread' structure description for details about parent
 *  OpenCL Steel Thread
 *  @see 'cl_Time_Study_t' structure description for details about time
 *  measurements.
 */
typedef struct scow_Kernel
{
    scow_Error* error;
    /*!< Structure for errors handling. */

    scow_Timer* timer;
    /*!< Structure for time measurement. */

    cl_kernel kernel;
    /*!< OpenCL kernel. */

    cl_program program;
    /*!< OpenCL program. */

    cl_int exec_status;
    /*!< Kernel execution status. */

    /*! \cond PRIVATE */
    cl_event
    // Event, used for different auxiliary purposes, such as status acquire, etc
    internal_event,
    // Pointer to external event to kernel (if any)
            *p_external_event;

    // This flag denotes what event to check, if we want to check kernel status
    cl_bool evt_check_priority;
    /*! \endcond */

    char name[OCL_KERNEL_NAME_MAX_LEN];
    /*!< Name of the kernel. */

    /*! @name ND Range characteristics. */
    /*!@{*/
    size_t Dimensionality,
    /*!< Number of problem's dimensions. */

    num_args,
    /*!< Number of kernel arguments*/

    Global_Work_Size[3],
    /*!< Global amount of work items in each dimension. */

    Local_Work_Size[3];
    /*!< Size of local work group (if any) in each dimension. */
    /*!@}*/

    struct scow_Steel_Thread* parent_steel_thread;
    /*!< Parent OpenCL Steel_Thread which gives program, context, etc. */

    /*! @name Function pointers. */
    /*!@{*/
    ret_code (*Set_ND_Sizes)(struct scow_Kernel *self,
            const unsigned int dimensionality,
            const unsigned int *global_wg_size,
            const unsigned int *local_wg_size);
    /*!< Points on Kernel_Set_ND_Sizes(). */

    ret_code (*Launch)(struct scow_Kernel *self, cl_command_queue *queue,
            cl_uint evt_wait_list_size, const cl_event *evt_wait_list,
            cl_event *generated_evt, TIME_STUDY_MODE time_measure_mode, ...);
    /*!< Points on Kernel_Launch(). */

    char* (*Get_Name)(struct scow_Kernel *self);
    /*!< Points on Kernel_ND_Range().
     * @warning The pointed function allocates memory for string. */

    ret_code (*Check_Status)(struct scow_Kernel *self);
    /*!< Points on Kernel_Check_Status. */

    ret_code (*Destroy)(struct scow_Kernel *p_self);
    /*!< Points on Kernel_Destroy(). */
    /*!@{*/

} scow_Kernel;

/*!
 * This function allocates memory for structure, sets function pointers &
 * initializes OpenCL kernel.
 *
 * @param[in] parent_steel_thread parent Steel Thread, which gives context, etc
 * @param[in] how_to_get_sources enumeration, that describes in what way
 * kernel source or pre-built program is provided
 * @param[in] source this argument can either be filename, if kernel source is
 * provided in file, or pre-built program goes in file, or it's a string with
 * source code
 * @param[in] kernel_name name of OpenCL kernel
 * @param[in] extra_params extra parameters, that will be used during programm
 * building stage, if pre-built program isn't provided.
 *
 * @return pointer to allocated structure in case of success,
 * \ref VOID_KERNEL_PTR otherwise
 *
 * @warning always use 'Destroy' function pointer to free memory, allocated by
 * this function.
 */
scow_Kernel* Make_Kernel(struct scow_Steel_Thread *parent_steel_thread,
        enum OPENCL_SOURCES_MODE how_to_get_sources, const char* source,
        const char* kernel_name, const char* extra_params);

#ifdef __cplusplus
}
#endif

#endif /* CL_MINIMAL_KERNEL_H_ */
