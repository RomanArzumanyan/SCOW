/*
 * @file mem_object.c
 * @brief Provides basic abstraction for OpenCL memory object
 *
 * @see mem_object.h
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

#include "mem_object.h"

#include "steel_thread.h"
#include <stdlib.h>
#include <string.h>

/*! \cond PRIVATE
 * Gather event execution time in microseconds
 */
static cl_double Gather_Time_uS(cl_event* event)
{
    cl_double no_time;
    cl_ulong start, end;

    no_time = -1.0;

    OCL_CHECK_EXISTENCE(event, no_time);

    clWaitForEvents(1, event);

    start = end = 0;

    clGetEventProfilingInfo(*event, CL_PROFILING_COMMAND_START,
            sizeof(cl_ulong), &start, NULL);

    clGetEventProfilingInfo(*event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong),
            &end, NULL);

    return (cl_double) (end - start) * (cl_double) (1e-03);
}
/*! \endcond */

/**
 * \related cl_Mem_Object_t
 *
 * This function destroys child structures & release allocated memory
 *
 * @param[in,out] self  pointer to structure, in which 'Destroy' function pointer
 * is defined to point on this function.
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' otherwise.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 *
 */
static ret_code Mem_Object_Destroy(scow_Mem_Object *self)
{
    cl_int ret = CL_SUCCESS;

    OCL_CHECK_EXISTENCE(self, CL_SUCCESS);

    self->error->Destroy(self->error);
    self->timer->Destroy(self->timer);

    /* Unmap object, if mapped. Check for MEM_OBJ_NOT_MAPPED return code, as
     * this is no error when destroying memory object, which may be not mapped.
     */
    ret = self->Unmap(self, CL_TRUE, NULL, DONT_MEASURE, NULL, NULL);
    if (ret != CL_SUCCESS && ret != MEM_OBJ_NOT_MAPPED)
    {
        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);
    }

    /* Release allocated memory for OpenCL memory object. Check for error code
     * CL_INVALID_MEM_OBJECT, as soon as we may go into 'Destroy()' function as
     * result of failed memory object creation attempt - that isn't error.
     */
    ret = clReleaseMemObject(self->cl_mem_object);
    if (ret != CL_SUCCESS && ret != CL_INVALID_MEM_OBJECT)
    {
        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);
    }

    free(self);
    return CL_SUCCESS;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function returns pointer to OpenCL memory object
 *
 * @param[in,out] self  pointer to structure, in which 'Get_Mem_Obj' function pointer
 * is defined to point on this function.
 *
 * @return pointer ot OpenCL memory object of type 'cl_mem', NULL pointer in
 * case of error.
 */
static cl_mem* Mem_Object_Get_Mem_Obj(scow_Mem_Object *self)
{
    OCL_CHECK_EXISTENCE(self, (cl_mem* )0x0);

    return &self->cl_mem_object;
}

/**
 * \related cl_Mem_Object_t
 *
 * 1D buffer can't has width, so, if called, this function returns 0 & set
 * error code.
 *
 * @param[in,out] self  pointer to structure, in which 'Get_Height' function pointer
 * is defined to point on this function.
 *
 * @return always 0. Also, it sets error code via
 * 'error' structure pointer of structure, defined by pointer 'this'
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static size_t Buffer_Get_Width(scow_Mem_Object *self)
{
    OCL_CHECK_EXISTENCE(self, 0);

    self->error->Set_Last_Code(self->error,
    CALLING_UNDEF_ACCESSOR);

    return 0;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function returns width of OpenCL image.
 *
 * @param[in,out] self  pointer to structure, in which 'Get_Width' function pointer
 * is defined to point on this function.
 *
 * @return width of image in case of success. Otherwise it sets error code via
 * 'error' structure pointer of structure, defined by pointer 'this'
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static size_t Image_Get_Width(scow_Mem_Object *self)
{
    OCL_CHECK_EXISTENCE(self, 0);

    return self->width;
}

/**
 * \related cl_Mem_Object_t
 *
 * 1D buffer can't has height, so, if called, this function returns 0 & set
 * error code.
 *
 * @param[in,out] self  pointer to structure, in which 'Get_Height' function pointer
 * is defined to point on this function.
 *
 * @return always 0. Also, it sets error code via
 * 'error' structure pointer of structure, defined by pointer 'this'
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static size_t Buffer_Get_Height(scow_Mem_Object *self)
{
    OCL_CHECK_EXISTENCE(self, 0);

    self->error->Set_Last_Code(self->error,
    CALLING_UNDEF_ACCESSOR);

    return 0;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function returns height of OpenCL image.
 *
 * @param[in,out] self  pointer to structure, in which 'Get_Height' function pointer
 * is defined to point on this function.
 *
 * @return height of image in case of success. Otherwise it sets error code via
 * 'error' structure pointer of structure, defined by pointer 'this'
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static size_t Image_Get_Height(scow_Mem_Object *self)
{
    OCL_CHECK_EXISTENCE(self, 0);

    return self->height;
}

/**
 * \related cl_Mem_Object_t
 *
 * 1D buffer can't has row pitch, so, if called, this function returns 0 & set
 * error code.
 *
 * @param[in,out] self  pointer to structure, in which 'Get_Height' function pointer
 * is defined to point on this function.
 *
 * @return always 0. Also, it sets error code via
 * 'error' structure pointer of structure, defined by pointer 'this'
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static size_t Buffer_Get_Row_Pitch(scow_Mem_Object *self)
{
    OCL_CHECK_EXISTENCE(self, 0);
    self->error->Set_Last_Code(self->error, CALLING_UNDEF_ACCESSOR);

    return 0;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function returns row pitch of OpenCL image.
 *
 * @param[in,out] self  pointer to structure, in which 'Get_Row_Pitch' function pointer
 * is defined to point on this function.
 *
 * @return row pitch of image in case of success. Otherwise it sets error code via
 * 'error' structure pointer of structure, defined by pointer 'this'
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static size_t Image_Get_Row_Pitch(scow_Mem_Object *self)
{
    OCL_CHECK_EXISTENCE(self, 0);

    return self->row_pitch;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function maps OpenCL memory object into Host-accessible memory & returns
 * pointer to mapped memory
 *
 * @param[in,out] self  pointer to structure, in which 'Map' function pointer
 * is defined to point on this function.
 * @param[in] blocking_map flag of type 'cl_bool' that denotes, should operation
 * be blocking or not.
 * @param [in] map_flags mapping flags, that denotes how memory object should be
 * mapped
 * @param[in] time_mode enumeration, that denotes how time measurement should be
 * performed
 * @param[out] evt_to_generate pointer to OpenCL event that will be generated
 * at the end of operation.
 *
 * @return pointer to Host-accessible region of memory in case of success, NULL
 * pointer otherwise. In that case function sets error value, which is available
 * through cl_Error_t structure, defined by pointer 'self->error'
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static void* Buffer_Map(
    scow_Mem_Object     *self, 
    cl_bool             blocking_map,
    cl_map_flags        map_flags, 
    TIME_STUDY_MODE     time_mode,
    cl_event            *evt_to_generate, 
    cl_command_queue    explicit_queue)
{
    cl_int ret;

    cl_event mapping_ready, *p_mapping_ready;

    OCL_CHECK_EXISTENCE(self, NULL);

    if (blocking_map > CL_TRUE)
    {
        self->error->Set_Last_Code(self->error, INVALID_BLOCKING_FLAG);
        return NULL;
    }

    (evt_to_generate != NULL) ?
            (p_mapping_ready = evt_to_generate) : 
            (p_mapping_ready = &mapping_ready);

    // We can't map the object, that is already mapped
    if (self->mapped_to_region != NULL)
    {
        self->error->Set_Last_Code(self->error, BUFFER_IN_USE);
        return VOID_MEM_OBJ_PTR;
    }

    cl_command_queue q =
            (explicit_queue == NULL) ?
                    (self->parent_thread->q_data_dtoh) : (explicit_queue);

    /* Save mapped pointer inside a structure in case if memory object is being
     * destroyed without unmapping it at first.
     */
    self->mapped_to_region = clEnqueueMapBuffer(q, self->cl_mem_object,
            blocking_map, map_flags, 0, self->size, 0,
            NULL, p_mapping_ready, &ret);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS,
            self->error->Set_Last_Code(self->error, ret), NULL);

    switch (time_mode)
    {
    case MEASURE:
        self->timer->current_time_device = Gather_Time_uS(p_mapping_ready);
        self->timer->total_time_device += self->timer->current_time_device;
        break;

    case DONT_MEASURE:
        break;

    default:
        break;
    }

    if (p_mapping_ready != evt_to_generate){
        clReleaseEvent(*p_mapping_ready);
    }

    return self->mapped_to_region;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function map OpenCL Image into Host-accessible memory & returns pointer
 * to mapped memory region
 * @param[in,out] self  pointer to structure, in which 'Map' function pointer
 * is defined to point on this function.
 * @param[in] blocking_map flag of type 'cl_bool' that denotes, should operation
 * be blocking or not.
 * @param [in] map_flags mapping flags, that denotes how memory object should be
 * mapped
 * @param[in] time_mode enumeration, that denotes how time measurement should be
 * performed
 * @param[out] evt_to_generate pointer to OpenCL event that will be generated
 * at the end of operation.
 *
 * @return pointer to Host-accessible region of memory in case of success, NULL
 * pointer otherwise. In that case function sets error value, which is available
 * through cl_Error_t structure, defined by pointer 'self->error'
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static void* Image_Map(
    scow_Mem_Object     *self, 
    cl_bool             blocking_map,
    cl_map_flags        map_flags, 
    TIME_STUDY_MODE     time_mode,
    cl_event            *evt_to_generate, 
    cl_command_queue    explicit_queue)
{
    cl_int ret;

    cl_event mapping_ready, *p_mapping_ready;

    const size_t origin[3] =
    { 0, 0, 0 }, region[3] =
    { self->width, self->height, 1 };

    OCL_CHECK_EXISTENCE(self, NULL);

    if (blocking_map > CL_TRUE)
    {
        self->error->Set_Last_Code(self->error, INVALID_BLOCKING_FLAG);
        return NULL;
    }

    (evt_to_generate != NULL) ?
            (p_mapping_ready = evt_to_generate) : (p_mapping_ready =
                    &mapping_ready);

    // We can't map the object, that is already mapped
    if (self->mapped_to_region != NULL)
    {
        self->error->Set_Last_Code(self->error, BUFFER_IN_USE);
        return VOID_MEM_OBJ_PTR;
    }

    cl_command_queue q =
            (explicit_queue == NULL) ?
                    (self->parent_thread->q_data_dtoh) : (explicit_queue);

    /* Save mapped pointer inside a structure in case if memory object is being
     * destroyed without unmapping it at first.
     */
    self->mapped_to_region = clEnqueueMapImage(q, self->cl_mem_object,
            blocking_map, map_flags, origin, region, &self->row_pitch, NULL, 0,
            NULL, p_mapping_ready, &ret);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS,
            self->error->Set_Last_Code(self->error, ret), NULL);

    switch (time_mode)
    {
    case MEASURE:
        self->timer->current_time_device = Gather_Time_uS(p_mapping_ready);
        self->timer->total_time_device += self->timer->current_time_device;
        break;

    case DONT_MEASURE:
        break;

    default:
        break;
    }

    if (p_mapping_ready != evt_to_generate){
        clReleaseEvent(*p_mapping_ready);
    }

    return self->mapped_to_region;
}

/**
 * \related cl_Mem_Object_t
 *
 * This fucntion unmaps previously mapped memory for OpenCL buffer.
 *
 * @param[in,out] self  pointer to structure, in which 'Unmap' function pointer
 * is defined to point on this function.
 * @param[out] p_mapped_ptr pointer to pointer, that was returned as the
 * result of mapping operation.
 * @param[in] time_mode enumeration, that denotes how time measurement should be
 * performed.
 * @param[out] evt_to_generate pointer to OpenCL event that will be generated
 * at the end of operation.
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' otherwise.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static ret_code Mem_Object_Unmap(
    scow_Mem_Object         *self, 
    cl_bool                 blocking_map,
    void                    **p_mapped_ptr, 
    TIME_STUDY_MODE         time_mode,
    cl_event                *evt_to_generate, 
    cl_command_queue        explicit_queue)
{
    cl_int ret;
    cl_event *p_unmapping_ready;

    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);
    OCL_CHECK_EXISTENCE(self->mapped_to_region, MEM_OBJ_NOT_MAPPED);

    if (p_mapped_ptr)
    {
        OCL_CHECK_EXISTENCE(*p_mapped_ptr, INVALID_BUFFER_GIVEN);

        /* Check if we are trying to unmap pointer, that was mapped from different
         * Memory Object. */
        if (self->mapped_to_region != *p_mapped_ptr)
        {
            OCL_DIE_ON_ERROR(WRONG_PARENT_OBJECT, CL_SUCCESS, NULL,
                    WRONG_PARENT_OBJECT);
        }
    }

    /* We generate event in any case - because later we may want to wait for
     * unmapping completion. */
    (evt_to_generate != NULL) ?
            (p_unmapping_ready = evt_to_generate) :
            (p_unmapping_ready = &self->unmap_evt);

    cl_command_queue q =
            (explicit_queue == NULL) ?
                    (self->parent_thread->q_data_htod) : (explicit_queue);

    ret = clEnqueueUnmapMemObject(q, self->cl_mem_object,
            self->mapped_to_region, 0, NULL, p_unmapping_ready);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    self->mapped_to_region = NULL;
    self->row_pitch = 0;

    if (p_mapped_ptr != NULL)
    {
        *p_mapped_ptr = NULL;
    }

    switch (time_mode)
    {
    case MEASURE:
        self->timer->current_time_device = Gather_Time_uS(p_unmapping_ready);
        self->timer->total_time_device += self->timer->current_time_device;
        break;

    case DONT_MEASURE:
        break;

    default:
        if (blocking_map)
        {
            ret = clWaitForEvents(1, p_unmapping_ready);
            OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);
        }
        break;
    }

    if (p_unmapping_ready != evt_to_generate){
        clReleaseEvent(*p_unmapping_ready);
    }

    return ret;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function read data from Host-accessible memory region, defined by argument
 * 'source' & write that data into OpenCL buffer memory object, defined by
 * argument 'self'
 *
 * @param[in,out] self  pointer to structure, in which 'Write' function pointer
 * is defined to point on this function.
 * @param[in] blocking_flag flag, that denotes, should operation be blocking or not.
 * @param[in] source pointer to Host-accessible memory region, that
 * contain data to be write in OpenCL memory object.
 * @param[in] time_mode enumeration, that denotes how time measurement should be
 * performed.
 * @param[out] evt_to_generate pointer to OpenCL event that will be generated
 * at the end of operation.
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' otherwise.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static ret_code Buffer_Send_To_Device(
    scow_Mem_Object         *self,
    cl_bool                 blocking_flag, 
    void                    *source, 
    TIME_STUDY_MODE         time_mode,
    cl_event                *evt_to_generate, 
    cl_command_queue        explicit_queue)
{
    cl_int ret = CL_SUCCESS;
    cl_event write_ready, *p_write_ready = (cl_event*) 0x0;

    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);
    OCL_CHECK_EXISTENCE(source, INVALID_BUFFER_GIVEN);

    (evt_to_generate != NULL) ?
            (p_write_ready = evt_to_generate) : 
            (p_write_ready = &write_ready);

    cl_command_queue q = (explicit_queue == NULL) ? 
        (self->parent_thread->q_data_htod) : 
        (explicit_queue);

    ret = clEnqueueWriteBuffer(q, self->cl_mem_object, blocking_flag, 0,
            self->size, source, 0, NULL, p_write_ready);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    switch (time_mode)
    {
    case MEASURE:
        self->timer->current_time_device = Gather_Time_uS(p_write_ready);
        self->timer->total_time_device += self->timer->current_time_device;
        break;

    case DONT_MEASURE:
        break;

    default:
        break;
    }

    if (p_write_ready != evt_to_generate){
        clReleaseEvent(*p_write_ready);
    }

    return ret;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function read data from Host-accessible memory region, defined by argument
 * 'source' & write that data into OpenCL Image memory object, definded by
 * argument'self'
 * @param[in,out] self  pointer to structure, in which 'Write' function pointer
 * is defined to point on this function.
 * @param[in] blocking_flag flag, that denotes, should operation be blocking or not.
 * @param[in] source pointer to Host-accessible memory region, that
 * contain data to be write in OpenCL memory object.
 * @param[in] time_mode enumeration, that denotes how time measurement should be
 * performed.
 * @param[out] evt_to_generate pointer to OpenCL event that will be generated
 * at the end of operation.
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' otherwise.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static ret_code Image_Send_To_Device(
    scow_Mem_Object     *self,
    cl_bool             blocking_flag, 
    void                *source, 
    TIME_STUDY_MODE     time_mode,
    cl_event            *evt_to_generate, 
    cl_command_queue    explicit_queue)
{
    cl_int ret = CL_SUCCESS;

    cl_event write_ready, *p_write_ready = (cl_event*) 0x0;

    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);
    OCL_CHECK_EXISTENCE(source, INVALID_BUFFER_GIVEN);

    const size_t origin[3] =
    { 0, 0, 0 }, region[3] =
    { self->width, self->height, 1 };

    (evt_to_generate != NULL) ?
            (p_write_ready = evt_to_generate) : (p_write_ready = &write_ready);

    cl_command_queue q =
            (explicit_queue == NULL) ?
                    (self->parent_thread->q_data_htod) : (explicit_queue);

    ret = clEnqueueWriteImage(q, self->cl_mem_object, blocking_flag, origin,
            region, self->row_pitch, 0, source, 0, NULL, p_write_ready);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    switch (time_mode)
    {
    case MEASURE:
        self->timer->current_time_device = Gather_Time_uS(p_write_ready);
        self->timer->total_time_device += self->timer->current_time_device;
        break;

    case DONT_MEASURE:
        break;

    default:
        break;
    }

    if (p_write_ready != evt_to_generate){
        clReleaseEvent(*p_write_ready);
    }

    return ret;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function reads OpenCL buffer memory object & write that data into
 * Host-accessible memory region.
 *
 * @param[in,out] self  pointer to structure, in which 'Read' function pointer
 * is defined to point on this function.
 * @param[in] blocking_flag flag, that denotes, should operation be blocking or not.
 * @param[out] destination pointer to Host-accessible memory region, where
 * data from OpenCL memory object will be written to
 * @param[in] time_mode enumeration, that denotes how time measurement should be
 * performed.
 * @param[out] evt_to_generate pointer to OpenCL event that will be generated
 * at the end of operation.
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' otherwise.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static ret_code Buffer_Get_From_Device(
    scow_Mem_Object     *self,
    cl_bool             blocking_flag, 
    void                *destination, 
    TIME_STUDY_MODE     time_mode,
    cl_event            *evt_to_generate, 
    cl_command_queue    explicit_queue)
{
    cl_int ret = CL_SUCCESS;

    cl_event read_ready, *p_read_ready = (cl_event*) 0x0;

    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);
    OCL_CHECK_EXISTENCE(destination, INVALID_BUFFER_GIVEN);

    (evt_to_generate != NULL) ?
            (p_read_ready = evt_to_generate) : (p_read_ready = &read_ready);

    cl_command_queue q =
            (explicit_queue == NULL) ?
                    (self->parent_thread->q_data_dtoh) : (explicit_queue);

    ret = clEnqueueReadBuffer(q, self->cl_mem_object, blocking_flag, 0,
            self->size, destination, 0, NULL, p_read_ready);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    switch (time_mode)
    {
    case MEASURE:
        self->timer->current_time_device = Gather_Time_uS(p_read_ready);
        self->timer->total_time_device += self->timer->current_time_device;
        break;

    case DONT_MEASURE:
        break;

    default:
        break;
    }

    if (p_read_ready != evt_to_generate){
        clReleaseEvent(*p_read_ready);
    }

    return ret;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function reads OpenCL Image memory object & write that data into
 * Host-accessible memory region.
 *
 * @param[in,out] self  pointer to structure, in which 'Read' function pointer
 * is defined to point on this function.
 * @param[in] blocking_flag flag, that denotes, should operation be blocking or not.
 * @param[out] destination pointer to Host-accessible memory region, where
 * data from OpenCL memory object will be written to
 * @param[in] time_mode enumeration, that denotes how time measurement should be
 * performed.
 * @param[out] evt_to_generate pointer to OpenCL event that will be generated
 * at the end of operation.
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' otherwise.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static ret_code Image_Get_From_Device(
    scow_Mem_Object         *self,
    cl_bool                 blocking_flag, 
    void                    *destination, 
    TIME_STUDY_MODE         time_mode,
    cl_event                *evt_to_generate, 
    cl_command_queue        explicit_queue)
{
    cl_int ret = CL_SUCCESS;

    cl_event read_ready, *p_read_ready = (cl_event*) 0x0;

    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);
    OCL_CHECK_EXISTENCE(destination, INVALID_BUFFER_GIVEN);

    const size_t origin[3] =
    { 0, 0, 0 }, region[3] =
    { self->width, self->height, 1 };

    (evt_to_generate != NULL) ?
            (p_read_ready = evt_to_generate) : (p_read_ready = &read_ready);

    cl_command_queue q =
            (explicit_queue == NULL) ?
                    (self->parent_thread->q_data_dtoh) : (explicit_queue);

    ret = clEnqueueReadImage(q, self->cl_mem_object, blocking_flag, origin,
            region, self->row_pitch, 0, destination, 0, NULL, p_read_ready);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    switch (time_mode)
    {
    case MEASURE:
        self->timer->current_time_device = Gather_Time_uS(p_read_ready);
        self->timer->total_time_device += self->timer->current_time_device;
        break;

    case DONT_MEASURE:
        break;

    default:
        break;
    }

    if (p_read_ready != evt_to_generate){
        clReleaseEvent(*p_read_ready);
    }

    return ret;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function copies content of one OpenCL buffer memory object into another.
 *
 * @param[in,out] self  pointer to structure, in which 'Copy' function pointer
 * is defined to point on this function.
 * @param[out] dest pointer to another Memory Object structure, where the data
 * from 'self' will be copied to.
 * @param[in] blocking_flag flag, that denotes, should operation be blocking or not.
 * @param[in] time_mode enumeration, that denotes how time measurement should be
 * performed.
 * @param[out] evt_to_generate pointer to OpenCL event that will be generated
 * at the end of operation.
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' otherwise.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static ret_code Buffer_Copy(
    scow_Mem_Object         *self, 
    scow_Mem_Object         *dest,
    cl_bool                 blocking_flag, 
    TIME_STUDY_MODE         time_mode,
    cl_event                *evt_to_generate, 
    cl_command_queue        explicit_queue)
{
    cl_int ret = CL_SUCCESS;

    cl_event copy_ready, *p_copy_ready = (cl_event*) 0x0;

    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);
    OCL_CHECK_EXISTENCE(dest, INVALID_BUFFER_GIVEN);

    // Can't copy distinct memory objects
    if (self->obj_mem_type != dest->obj_mem_type)
    {
        return DISTINCT_MEM_OBJECTS;
    }

    // Can't copy bigger object into smaller one
    if (self->size > dest->size)
    {
        return INVALID_BUFFER_SIZE;
    }

    // If src & dest are the same, no need to copy at all, just reset timer.
    if (self == dest)
    {
        self->timer->current_time_device = 0;
        return CL_SUCCESS;
    }

    (evt_to_generate == NULL) ? (p_copy_ready = &copy_ready) : (p_copy_ready =
                                        evt_to_generate);

    cl_command_queue q =
            (explicit_queue == NULL) ?
                    (self->parent_thread->q_data_dtod) : (explicit_queue);

    ret = clEnqueueCopyBuffer(q, self->cl_mem_object, dest->cl_mem_object, 0, 0,
            self->size, 0, NULL, p_copy_ready);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    switch (time_mode)
    {
    case MEASURE:
        self->timer->current_time_device = Gather_Time_uS(p_copy_ready);
        self->timer->total_time_device += self->timer->current_time_device;
        break;

    default:
        break;
    }

    if (p_copy_ready != evt_to_generate){
        clReleaseEvent(*p_copy_ready);
    }

    return ret;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function copy content of one OpenCL Image memory object into another.
 * @param[in,out] self  pointer to structure, in which 'Copy' function pointer
 * is defined to point on this function.
 * @param[out] dest pointer to another Memory Object structure, where the data
 * from 'self' will be copied to.
 * @param[in] blocking_flag flag, that denotes, should operation be blocking or not.
 * @param[in] time_mode enumeration, that denotes how time measurement should be
 * performed.
 * @param[out] evt_to_generate pointer to OpenCL event that will be generated
 * at the end of operation.
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' otherwise.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static ret_code Image_Copy(
    scow_Mem_Object         *self, 
    scow_Mem_Object         *dest,
    cl_bool                 blocking_flag, 
    TIME_STUDY_MODE         time_mode,
    cl_event                *evt_to_generate, 
    cl_command_queue        explicit_queue)
{
    cl_int ret = CL_SUCCESS;

    cl_event copy_ready, *p_copy_ready = (cl_event*) 0x0;

    const size_t origin[3] =
    { 0, 0, 0 }, region[3] =
    { self->width, self->height, 1 };

    if (self->obj_mem_type != dest->obj_mem_type)
    {
        return DISTINCT_MEM_OBJECTS;
    }

    // If src & dest are the same, no need to copy at all, just reset timer
    if (self == dest)
    {
        self->timer->current_time_device = 0;
        self->timer->total_time_device += self->timer->current_time_device;

        return CL_SUCCESS;
    }

    (evt_to_generate != NULL) ?
            (p_copy_ready = evt_to_generate) : (p_copy_ready = &copy_ready);

    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);
    OCL_CHECK_EXISTENCE(dest, INVALID_BUFFER_GIVEN);

    // Can't copy bigger image into smaller one
    if ((self->row_pitch > dest->row_pitch) || (self->height > dest->height)
            || (self->width > dest->width))
    {
        return INVALID_BUFFER_SIZE;
    }

    cl_command_queue q =
            (explicit_queue == NULL) ?
                    (self->parent_thread->q_data_dtod) : (explicit_queue);

    ret = clEnqueueCopyImage(q, self->cl_mem_object, dest->cl_mem_object,
            origin, origin, region, 0, NULL, p_copy_ready);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    switch (time_mode)
    {
    case MEASURE:
        self->timer->current_time_device = Gather_Time_uS(p_copy_ready);
        self->timer->total_time_device += self->timer->current_time_device;
        break;

    case DONT_MEASURE:
        break;

    default:
        break;
    }

    if (p_copy_ready != evt_to_generate){
        clReleaseEvent(*p_copy_ready);
    }

    return ret;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function swaps pointers to OpenCL memory objects. It can be used in
 * some cases as quicker alternative to copy.
 *
 * @param[in,out] self  pointer to structure, in which 'Swap' function pointer
 * is defined to point on this function.
 * @param[in,out] dest pointer to structure with which memory objects will be
 * swapped
 *
 * @return CL_SUCCESS in case of success, error code of type 'ret_code' otherwise.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static ret_code Mem_Object_Swap(scow_Mem_Object **self, scow_Mem_Object **dest)
{
    scow_Mem_Object* temp;

    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);
    OCL_CHECK_EXISTENCE(dest, INVALID_BUFFER_GIVEN);

    OCL_CHECK_EXISTENCE(*self, INVALID_BUFFER_GIVEN);
    OCL_CHECK_EXISTENCE(*dest, INVALID_BUFFER_GIVEN);

    // Order of conditions is important
    if ((*self)->obj_mem_type != (*dest)->obj_mem_type)
    {
        return DISTINCT_MEM_OBJECTS;
    }

    if ((*self)->mem_flags != (*dest)->mem_flags)
    {
        return DISTINCT_MEM_FLAGS;
    }

    if ((*self)->size != (*dest)->size)
    {
        return INVALID_BUFFER_SIZE;
    }

    // No OpenCL Device-side operations are made
    temp = *self;
    *self = *dest;
    *dest = temp;

    return CL_SUCCESS;
}

/**
 * \related cl_Mem_Object_t
 * This function erases the content of Buffer memory via mapping + unmapping. Be
 * careful when using this on OpenCL Device with discrete memory.
 *
 * @param[in,out] self  pointer to structure, in which 'Erase' function pointer
 * is defined to point on this function.
 *
 * @return \ref CL_SUCCESS in case of success, error code otherwise.
 *
 * @see cl_err_codes.h for detailed error description.
 * @see 'cl_Error_t' structure for error handling.
 */
static ret_code Buffer_Erase(scow_Mem_Object *self)
{
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);

    unsigned char *mapped_area = self->Map(self, CL_TRUE, CL_MAP_WRITE,
            DONT_MEASURE, NULL, NULL);

    ret_code ret = self->error->Get_Last_Code(self->error);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    memset(mapped_area, 0, self->size);

    ret = self->Unmap(self, CL_TRUE, (void**) &mapped_area, DONT_MEASURE, NULL,
            NULL);
    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, ret);

    return ret;
}

static ret_code Mem_Object_Sync(
    scow_Mem_Object     *self,
    MEM_OBJECT_ETHALON  ethalon,
    TIME_STUDY_MODE     time_mode)
{
    ret_code ret = CL_SUCCESS;
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);

    if (!(self->mem_flags & CL_MEM_USE_HOST_PTR)){
        return INVALID_BUFFER_GIVEN;
    }

    switch (ethalon){
    case DEVICE:
        self->Read(self, CL_FALSE, self->host_ptr, time_mode, (cl_event*)0, (cl_command_queue)0);
        break;

    case HOST:
        self->Write(self, CL_FALSE, self->host_ptr, time_mode, (cl_event)0, (cl_command_queue)0);
        break;

    default:
        ret = INVALID_ARG_TYPE;
        break;
    }

    return ret;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function allocates memory for child Memory Object with generic data
 * container & sets function pointers
 *
 * @param[in] self pointer to existing Memory Object, in which function pointer
 * 'Make_Child' is defined to point on this function.
 * @param[in] mem_flags OpenCL memory flags, which will be used for OpenCL
 * sub-buffer creation
 * @param[in] buffer_create_type information about sub-buffer creation type
 * @param[in] buffer_create_info structure with sub-buffer origin & size
 * values.
 *
 * @return pointer to allocated structure in case of success,
 * \ref VOID_MEM_OBJ_PTR otherwise. In case of error it sets error code, which
 * is available via 'error' structure.
 *
 * @warning always use 'Destroy' function pointer to free memory, allocated
 * by this function.
 */
static scow_Mem_Object* Buffer_Make_Sub_Buffer(scow_Mem_Object *self,
        cl_mem_flags flags, cl_buffer_create_type buffer_create_type,
        const void *buffer_create_info)
{
    OCL_CHECK_EXISTENCE(self, VOID_MEM_OBJ_PTR);

    cl_int ret;
    scow_Mem_Object* child;

    /* Sub-buffer creation is defined only for Buffer, which isn't someone's
     * child. */
    if (self->obj_mem_type != BUFFER)
    {
        ret = INVALID_ARG_TYPE;
        self->error->Set_Last_Code(self->error, ret);
        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, VOID_MEM_OBJ_PTR);
    }

    if (self->obj_paternity != PARENT_OBJECT)
    {
        ret = WRONG_PARENT_OBJECT;
        self->error->Set_Last_Code(self->error, ret);
        OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, VOID_MEM_OBJ_PTR);
    }

    child = (scow_Mem_Object*) calloc(1, sizeof(*child));
    OCL_CHECK_EXISTENCE(child, VOID_MEM_OBJ_PTR);

    child->obj_mem_type = BUFFER;
    child->obj_paternity = CHILD_OBJECT;
    child->mem_flags = flags;
    child->parent_thread = self->parent_thread;

    child->error = Make_Error();
    child->timer = Make_Timer(VOID_KERNEL_PTR);

    child->Get_Mem_Obj = Mem_Object_Get_Mem_Obj;
    child->Destroy = Mem_Object_Destroy;
    child->Swap = Mem_Object_Swap;
    child->Unmap = Mem_Object_Unmap;

    child->Map = Buffer_Map;
    child->Write = Buffer_Send_To_Device;
    child->Read = Buffer_Get_From_Device;
    child->Copy = Buffer_Copy;
    child->Erase = Buffer_Erase;
    child->Sync = Mem_Object_Sync;

    child->Get_Height = Buffer_Get_Height;
    child->Get_Width = Buffer_Get_Width;
    child->Get_Row_Pitch = Buffer_Get_Row_Pitch;
    child->Make_Child = Buffer_Make_Sub_Buffer;

    child->cl_mem_object = clCreateSubBuffer(self->cl_mem_object, flags,
            buffer_create_type, buffer_create_info, &ret);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, child->Destroy(child), VOID_MEM_OBJ_PTR);

    child->size = ((cl_buffer_region*) buffer_create_info)->size;
    child->origin = ((cl_buffer_region*) buffer_create_info)->origin;

    if (self->host_ptr)
    {
        child->host_ptr = (unsigned char*)self->host_ptr + child->origin;
    }

    return child;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function allocates memory for Memory Object with generic data container
 * & sets function pointers
 *
 * @param[in] parent_thread parent Steel Thread, which gives OpenCL context, etc
 * @param[in] mem_flags OpenCL memory flags, which will be used for OpenCL
 * memory objects creation
 * @param[in] size amount of memory, which will be allocated, in bytes
 * @param[in] host_ptr pointer to Host-side memory region (if any). This argument
 * is optional. If not needed - provide null pointer instead.
 *
 * @return pointer to allocated structure in case of success,
 * \ref VOID_MEM_OBJ_PTR otherwise
 *
 * @warning always use 'Destroy' function pointer to free memory, allocated
 * by this function.
 */
scow_Mem_Object* Make_Buffer(scow_Steel_Thread *parent_thread,
        const cl_mem_flags mem_flags, const size_t size, void *host_ptr)
{
    cl_int ret;
    scow_Mem_Object* self;

    OCL_CHECK_EXISTENCE(parent_thread, VOID_MEM_OBJ_PTR);

    self = (scow_Mem_Object*) calloc(1, sizeof(*self));
    OCL_CHECK_EXISTENCE(self, VOID_MEM_OBJ_PTR);

    self->obj_mem_type = BUFFER;
    self->size = size;
    self->host_ptr = host_ptr;
    self->mem_flags = mem_flags;
    self->parent_thread = parent_thread;

    self->error = Make_Error();
    self->timer = Make_Timer(VOID_KERNEL_PTR);

    self->Get_Mem_Obj = Mem_Object_Get_Mem_Obj;
    self->Destroy = Mem_Object_Destroy;
    self->Swap = Mem_Object_Swap;
    self->Unmap = Mem_Object_Unmap;

    self->Map = Buffer_Map;
    self->Write = Buffer_Send_To_Device;
    self->Read = Buffer_Get_From_Device;
    self->Copy = Buffer_Copy;
    self->Erase = Buffer_Erase;
    self->Sync = Mem_Object_Sync;

    self->Get_Height = Buffer_Get_Height;
    self->Get_Width = Buffer_Get_Width;
    self->Get_Row_Pitch = Buffer_Get_Row_Pitch;
    self->Make_Child = Buffer_Make_Sub_Buffer;

    self->cl_mem_object = clCreateBuffer(self->parent_thread->context,
            self->mem_flags, self->size, self->host_ptr, &ret);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, self->Destroy(self), VOID_MEM_OBJ_PTR);

    return self;
}

/**
 * \related cl_Mem_Object_t
 *
 * This function allocates memory for Memory Object with OpenCL image & sets
 * function pointers.
 *
 * @param[in] parent_thread parent Steel Thread, which gives OpenCL context, etc
 * @param[in] mem_flags OpenCL memory flags, which will be used for OpenCL
 * memory objects creation
 * @param[in] image_format OpenCL image format, that describe characteristics
 * @param[in] width image width
 * @param[in] height image height
 * @param[in] host_ptr pointer to Host-side memory region (if any). This argument
 * is optional. If not needed - provide null pointer instead.
 *
 * @return pointer to allocated structure in case of success,
 * \ref VOID_MEM_OBJ_PTR otherwise
 *
 * @warning always use 'Destroy' function pointer to free memory, allocated
 * by this function.
 */
scow_Mem_Object* Make_Image(
    scow_Steel_Thread       *parent_thread,
    const cl_mem_flags      mem_flags, 
    const cl_image_format   *image_format,
    const size_t            width, 
    const size_t            height, 
    const size_t            row_pitch,
    void                    *host_ptr)
{
    cl_int ret;
    scow_Mem_Object* self;

    OCL_CHECK_EXISTENCE(parent_thread, VOID_MEM_OBJ_PTR);
    OCL_CHECK_EXISTENCE(image_format, VOID_MEM_OBJ_PTR);

    self = (scow_Mem_Object*) calloc(1, sizeof(*self));
    OCL_CHECK_EXISTENCE(self, VOID_MEM_OBJ_PTR);

    self->obj_mem_type = IMAGE;
    self->parent_thread = parent_thread;
    self->host_ptr = host_ptr;
    self->mem_flags = mem_flags;
    self->width = width;
    self->height = height;
    self->row_pitch = 0;

    self->error = Make_Error();
    self->timer = Make_Timer(VOID_KERNEL_PTR);

    self->Get_Mem_Obj = Mem_Object_Get_Mem_Obj;
    self->Destroy = Mem_Object_Destroy;
    self->Swap = Mem_Object_Swap;
    self->Unmap = Mem_Object_Unmap;

    self->Map = Image_Map;
    self->Write = Image_Send_To_Device;
    self->Read = Image_Get_From_Device;
    self->Copy = Image_Copy;
    self->Erase = NULL;
    self->Sync = Mem_Object_Sync;

    self->Get_Height = Image_Get_Height;
    self->Get_Width = Image_Get_Width;
    self->Get_Row_Pitch = Image_Get_Row_Pitch;
    self->Make_Child = NULL;

#ifdef CL_USE_DEPRECATED_OPENCL_1_1_APIS
	self->cl_mem_object = clCreateImage2D(self->parent_thread->context,
            mem_flags, image_format, self->width, self->height, self->row_pitch,
            host_ptr, &ret);
#else
    cl_image_desc image_desc = {
        .image_type         = CL_MEM_OBJECT_IMAGE2D,
        .image_width        = self->width,
        .image_height       = self->height,
        .image_array_size   = 1,
        .image_row_pitch    = self->row_pitch,
        .image_slice_pitch  = host_ptr ? (self->row_pitch * self->height) : 0,
        .num_mip_levels     = 0,
        .num_samples        = 0,
        .buffer             = NULL
    };
    self->cl_mem_object = clCreateImage(self->parent_thread->context,
        mem_flags, image_format, &image_desc, host_ptr, &ret);
#endif

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, self->Destroy(self), VOID_MEM_OBJ_PTR);

    return self;
}

