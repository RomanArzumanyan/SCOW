/*
 * @file timer.c
 * @brief Provides abstraction for timing measurement - both on OpenCL Host & Device sides
 *
 * @see timer.h
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

#include "timer.h"
#include "kernel.h"
#include <stdlib.h>

/**
 * \related cl_Timer_t
 *
 * This function release memory, allocated for 'self' structure
 *
 * @param[in,out] self pointer to structure 'self' of type 'cl_Timer_t',
 * in which fptr 'Destroy' is defined to point on this function
 *
 * @return CL_SUCCESS always
 */
static ret_code Timer_Destroy(scow_Timer* self)
{
    OCL_CHECK_EXISTENCE(self, CL_SUCCESS);

    free(self);

    return CL_SUCCESS;
}

/**
 * \related cl_Timer_t
 *
 * This function starts time measurement session.
 *
 * @param[in,out] self pointer to structure 'self' of type 'cl_Timer_t',
 * in which fptr 'Start' is defined to point on this function
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' otherwise
 *
 * @see cl_err_codes.h for details about error codes.
 * @see 'cl_Error_t' for details about error handling.
 */
static ret_code Timer_Start(scow_Timer* self)
{
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);
    // Avoid measurement start if it's already started
    OCL_DIE_ON_ERROR(self->dirty_bit_host, CL_FALSE, NULL, TIMER_IN_USE);

    self->host_clock = clock();
    self->dirty_bit_host = CL_TRUE;

    return CL_SUCCESS;
}

/**
 * \related cl_Timer_t
 *
 * This function stops time measurement session & get difference between start
 * time & stop time. Also, it increments number of calls.
 *
 * @param[in,out] self pointer to structure 'self' of type 'cl_Timer_t', in
 * which fptr 'Stop' is defined to point on this function
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' otherwise
 *
 * @see cl_err_codes.h for details about error codes.
 * @see 'cl_Error_t' for details about error handling.
 */
static ret_code Timer_Stop(scow_Timer* self)
{
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);
    // Avoid measurement finish if it's already finished
    OCL_DIE_ON_ERROR(self->dirty_bit_host, CL_TRUE, NULL, TIMER_IN_USE);

    self->host_clock = clock() - self->host_clock;

    // All timers are in microseconds
    self->current_time_host = ((double) self->host_clock)
            * 1.0e6 / CLOCKS_PER_SEC;

    self->total_time_host += self->current_time_host;
    self->num_calls_host++;
    self->dirty_bit_host = CL_FALSE;

    return CL_SUCCESS;
}

/**
 * \related cl_Timer_t
 *
 * This function reset timer (make Host or Device counters zero)
 *
 * @param[in,out] self pointer to structure 'self' of type 'cl_Timer_t', in
 * which fptr 'Reset' is defined to point on this function
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' otherwise
 *
 * @see cl_err_codes.h for details about error codes.
 * @see 'cl_Error_t' for details about error handling.
 */
static ret_code Timer_Reset(scow_Timer* self, TIME_SIDE what_time)
{
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);

    switch (what_time)
    {
    case HOST_TIME:
        self->current_time_host = 0.0;
        self->dirty_bit_host = CL_FALSE;
        self->num_calls_host = 0;
        self->host_clock = 0;
        self->total_time_host = 0.0;
        break;

    case DEVICE_TIME:
        self->current_time_device = 0.0;
        self->dirty_bit_dev = CL_FALSE;
        self->num_calls_device = 0;
        self->total_time_device = 0;
        break;

    default:
        return INVALID_ARG_TYPE;
        break;
    }

    return CL_SUCCESS;
}

/**
 * \related cl_Timer_t
 *
 * This function returns total time, measured by Timer, defined by pointer
 * 'self'
 *
 * @param[in,out] self pointer to structure 'self' of type 'cl_Timer_t', in
 * which fptr 'Get_Total_Time' is defined to point on this function
 * @param[in] what_time enumeration, that describes what time counter value to
 * return.
 *
 * @return timer value in case of success, -1.0 in case of error.
 *
 * @see cl_err_codes.h for details about error codes.
 * @see 'cl_Error_t' for details about error handling.
 */
static double Timer_Get_Total_Time(scow_Timer* self, TIME_SIDE what_time)
{
    double exec_time = -1.0;
    OCL_CHECK_EXISTENCE(self, exec_time);

    switch (what_time)
    {
    case HOST_TIME:

        return self->total_time_host;
        break;

    case DEVICE_TIME:

        return self->total_time_device;
        break;

    default:
        break;
    }

    return exec_time;
}

/**
 * \related cl_Timer_t
 *
 * This function returns duration of last operation, measured by Timer
 * structure, defined by pointer 'self'
 *
 * @param[in,out] self pointer to structure 'self' of type 'cl_Timer_t', in
 * which fptr 'Get_Last_Time' is defined to point on this function
 * @param[in] what_time enumeration, that describes what time counter value to
 * return.
 *
 * @return timer value in case of success, -1.0 in case of error.
 *
 * @see cl_err_codes.h for details about error codes.
 * @see 'cl_Error_t' for details about error handling.
 */
static double Timer_Get_Last_Time(scow_Timer* self, TIME_SIDE what_time)
{
    double exec_time = -1.0;
    OCL_CHECK_EXISTENCE(self, exec_time);

    switch (what_time)
    {
    case HOST_TIME:
        return self->current_time_host;
        break;

    case DEVICE_TIME:
        return self->current_time_device;
        break;

    default:
        break;
    }

    return exec_time;
}

/**
 * \related cl_Timer_t
 *
 * This function returns number of operations invocations, measured by
 * Timer structure, defined by pointer 'self'
 *
 * @param[in,out] self pointer to structure 'self' of type 'cl_Timer_t', in
 * which fptr 'Get_Num_Calls' is defined to point on this function
 * @param[in] what_time enumeration, that describes what time counter value to
 * return.
 *
 * @return number of calls value in case of success, 0 in case of error.
 *
 * @see cl_err_codes.h for details about error codes.
 * @see 'cl_Error_t' for details about error handling.
 */
static long unsigned int Timer_Get_Num_Calls(scow_Timer* self,
        TIME_SIDE what_time)
{
    OCL_CHECK_EXISTENCE(self, ZERO_TIMES);

    switch (what_time)
    {
    case HOST_TIME:

        return self->num_calls_host;
        break;

    case DEVICE_TIME:

        return self->num_calls_device;
        break;

    default:

        break;
    }

    return ZERO_TIMES;
}

/**
 * \related cl_Timer_t
 *
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
scow_Timer* Make_Timer(scow_Kernel* parent_kernel)
{
    scow_Timer* self = (scow_Timer*) calloc(1, sizeof(*self));
    OCL_CHECK_EXISTENCE(self, VOID_TIME_STUDY_PTR);

    self->Destroy = Timer_Destroy;
    self->Start = Timer_Start;
    self->Stop = Timer_Stop;
    self->Reset = Timer_Reset;
    self->Get_Total_Time = Timer_Get_Total_Time;
    self->Get_Last_Time = Timer_Get_Last_Time;
    self->Get_Num_Calls = Timer_Get_Num_Calls;

    self->parent_kernel = parent_kernel;

    return self;
}
