/*
 * @file timer.h
 * @brief Provides abstraction for timing measurement - both on OpenCL Host & Device sides
 *
 * @see timer.c
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

#ifndef CL_TIME_STUDY_H_
#define CL_TIME_STUDY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "error.h"
#include <time.h>

struct scow_Kernel;

/*! \def VOID_TIME_STUDY_PTR
 * Void pointer to Timer
 */
#undef VOID_TIME_STUDY_PTR
#define VOID_TIME_STUDY_PTR     ((scow_Timer*)0x0)

/*! \def ZERO_TIMES
 * Macro introduced to denote zero number of function / kernel calls
 */
#undef ZERO_TIMES
#define ZERO_TIMES              (0)

typedef enum TIME_STUDY_MODE
{
    MEASURE = 0,
    /*!< Only obtain time measurement result for current operation. */

    DONT_MEASURE
/*!< Don't measure time. */
} TIME_STUDY_MODE;

typedef enum TIME_SIDE
{
    HOST_TIME = 0,
    /*!< Return time measurements for operations on Host side. */

    DEVICE_TIME
/*!< Return time measurements for operations on Device side. */
} TIME_SIDE;

/*!
 * \struct scow_Timer
 * This structure is a simple timer, which can measure time on OpenCL Host side
 * via clock() and on OpenCL Device side via events profiling. Also it can
 * accumulate number of calls & total time elapsed. This timer is embedded
 * almost in all other structures.
 */
typedef struct scow_Timer
{
    long unsigned int num_calls_host,
    /*!< How many times the timer was called on Host. */

    num_calls_device;
    /*!< How many times the timer was called on Device. */

    clock_t host_clock;
    /*!< Timer for current command time measurement session. */

    /*! @name Timers. */
    /*!@{*/
    double current_time_host,
    /*!< Last operation on Host execution time in microseconds. */

    total_time_host,
    /*!< Total time amount spent on Host, registered with current Time_Study
     * in microseconds. */

    current_time_device,
    /*!< Last operation on Device execution time in microseconds. */

    total_time_device;
    /*!< Total time amount spent on Device, registered with current Time_Study
     * in microseconds. */
    /*!@}*/

    /*! @name Dirty bits. */
    /*!@{*/
    cl_bool dirty_bit_host,
    /*!< Indicates that time on Host is being measured & not finished yet. */

    dirty_bit_dev;
    /*!< Indicates that time on Device is being measured & not finished yet. */
    /*!@}*/

    struct scow_Kernel* parent_kernel;
    /*!< Parent OpenCL kernel if any (Timer may not have parent kernel). */

    /*! @name Function pointers.*/
    /*!@{*/
    ret_code (*Destroy)(struct scow_Timer *self);
    /*!< Points on Timer_Destroy(). */

    ret_code (*Start)(struct scow_Timer *self);
    /*!< Points on Timer_Start(). */

    ret_code (*Stop)(struct scow_Timer *self);
    /*!< Points on Timer_Stop(). */

    ret_code (*Reset)(struct scow_Timer *self, TIME_SIDE what_time);
    /*!< Points on Timer_Reset(). */

    double (*Get_Total_Time)(struct scow_Timer *self, TIME_SIDE what_time);
    /*!< Points on Timer_Get_Total_Time(). */

    double (*Get_Last_Time)(struct scow_Timer *self, TIME_SIDE what_time);
    /*!< Points on Timer_Get_Last_Time(). */

    long unsigned int (*Get_Num_Calls)(struct scow_Timer *self,
            TIME_SIDE what_time);
/*!< Points on Timer_Get_Num_Calls(). */
/*!@}*/

} scow_Timer;

/*!
 * This function allocates memory for Timer & sets function pointers.
 *
 * @param[in] parent_kernel pointer to Minimal Kernel. As soon as Timer can
 * measure kernel execution, it can be attached to kernel. This argument is
 * optional - if you don't need to measure kernel time, provide void pointer as
 * argument.
 *
 * @return pointer to allocated structure in case of success,
 * \ref VOID_TIME_STUDY_PTR otherwise
 *
 * @warning always use 'Destroy' function pointer to free memory, allocated by
 * this function.
 */
scow_Timer* Make_Timer(struct scow_Kernel *parent_kernel);

#ifdef __cplusplus
}
#endif

#endif /* CL_TIME_STUDY_H_ */
