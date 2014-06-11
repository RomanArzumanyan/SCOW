#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "error.h"

struct scow_Command_Queue;
struct scow_Event;

/**
* \def VOID_EVENT_PTR
* NULL Event pointer
*/
#ifndef VOID_EVENT_PTR
#define VOID_EVENT_PTR  ((scow_Event*)0x0)
#endif

/**
* \struct scow_Event.
* @brief This structure is wrapper around OpenCL cl_event.
*/
typedef struct scow_Event
{
    /// Structure for error handling.
    scow_Error *p_error;

    /// Original OpenCL Event
    cl_event evt;

    /// Parent Command Queue
    struct scow_Command_Queue *p_parent_q;

    /*! @name Function pointers. */
    /*!@{*/
    /// Points on Event_Destroy().
    ret_code(*Destroy)(
        struct scow_Event   *self);

    /// Points on Event_Unhook().
    ret_code(*Unhook)(
        struct scow_Event   *self);

    /// Points on Event_Set_Status().
    ret_code(*Set_Status)(
        struct scow_Event   *self,
        cl_int              new_status);
    /*!@{*/
} scow_Event;

/**
* @brief This function allocates memory for structure, sets function pointers &
* initializes Event.
*
* @param[in, out] p_parent_queue pointer to parent Command Queue.
*
* @return pointer to allocated structure in case of success,
* \ref VOID_EVENT_PTR otherwise
*
* @warning always use 'Destroy' function pointer to free memory, allocated by
* this function.
*/
scow_Event* Make_Event(
struct scow_Command_Queue   *p_parent_queue);

#ifdef __cplusplus
}
#endif