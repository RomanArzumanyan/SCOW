#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "error.h"

    struct scow_Command_Queue;
    struct scow_Steel_Thread;
    struct scow_Event;

    /**
     * \def Task
     * Pointer to Task function, which is interface to Command Queue.
     */
    typedef ret_code(*Task)(
        struct scow_Command_Queue   *p_q,
        ...);

    /**
    * \def VOID_CMD_Q_PTR
    * NULL Command Queue pointer
    */
    #ifndef VOID_CMD_Q_PTR
    #define VOID_CMD_Q_PTR  ((scow_Command_Queue*)0x0)
    #endif

    /**
     * \struct scow_Command_Queue
     * @brief This structure provides wrapper around OpenCL cl_command_queue.
     */
    typedef struct scow_Command_Queue
    {
        /// Structure for error handling.
        scow_Error *p_error;

        /// Original OpenCL Command Queue.
        cl_command_queue cmd_q;

        /// Pointer to parent Steel Thread, from which Device is taken.
        struct scow_Steel_Thread *p_parent_thread;

        /*! @name Function pointers. */
        /*!@{*/
        /// Points on Command_Queue_Destroy().
        ret_code(*Destroy)(
            struct scow_Command_Queue   *self);

        /// Points on Command_Queue_Enqueue().
        scow_Event* (*Enqueue)(
            struct scow_Command_Queue   *self,
            Task                        task);

        /// Points on Command_Queue_Hook_N_Push().
        scow_Event* (*Hook_N_Push)(
            struct scow_Command_Queue   *self,
            Task                        task);

        /// Points on Command_Queue_Hook_N_Flush().
        scow_Event* (*Hook_N_Flush)(
            struct scow_Command_Queue   *self,
            Task                        task);
        /*!@{*/

    } scow_Command_Queue;

    /**
    * @brief This function allocates memory for structure, sets function pointers &
    * initializes Command Queue.
    *
    * @param[in, out] p_steel_thread pointer to parent Steel Thread.
    * @param[in] properties OpenCL Command Queue properties.
    *
    * @return pointer to allocated structure in case of success,
    * \ref VOID_CMD_Q_PTR otherwise
    *
    * @warning always use 'Destroy' function pointer to free memory, allocated by
    * this function.
    */
    scow_Command_Queue* Make_Command_Queue(
        struct scow_Steel_Thread    *p_steel_thread,
        cl_command_queue_properties properties);

#ifdef __cplusplus
}
#endif