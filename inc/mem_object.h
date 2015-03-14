/*
 * @file mem_object.h
 * @brief Provides basic abstraction for OpenCL memory object
 *
 * @see mem_object.c
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

#ifndef CL_MEM_OBJECT_H_
#define CL_MEM_OBJECT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "kernel.h"

/*!
 * \def VOID_MEM_OBJ_PTR
 * Pointer to void Memory Object
 */
#undef VOID_MEM_OBJ_PTR
#define VOID_MEM_OBJ_PTR    ((scow_Mem_Object*)0x0)

typedef enum MEM_OBJECT_PATERNITY
{
    /*! scow_Mem_Object with object type PARENT_MEM_OBJECT is parent memory object. */
    PARENT_OBJECT = 0,

    /*! scow_Mem_Object with object type CHILD_MEM_OBJECT is child memory object. */
    CHILD_OBJECT
} MEM_OBJECT_PATERNITY;

typedef enum MEM_OBJECT_TYPE
{
    /*! Generic memory object, that can store any data in C-style array */
    BUFFER = 0,

    /*! Memory Object, made to store OpenCL Image. */
    IMAGE
} MEM_OBJECT_TYPE;

typedef enum MEM_OBJECT_ETHALON
{
    /*! Indicates that OpenCL Host has ethalon information about shared memory object*/
    HOST = 0,

    /*! Indicates that OpenCL Device has ethalon information about shared memory object*/
    DEVICE
} MEM_OBJECT_ETHALON;

/*!
 * \struct scow_Mem_Object
 *
 * @brief This structure implements OpenCL memory objects - buffer & image
 *
 * This structure implements idea of OpenCL memory object, which can be of both
 * available types - generic array or image. It provides interface for
 * interaction with OpenCL Device-side memory as follows:
 *      - Send data to Device
 *      - Get data from Device
 *      - Map data
 *      - Unmap data
 *      - Copy data
 *      - Fast swap data without operations on Device side
 *
 * @example cl_mem_object_sample.c
 */
typedef struct scow_Mem_Object
{
    MEM_OBJECT_TYPE obj_mem_type;
    /*!< Type of memory object - Image or Buffer. */

    MEM_OBJECT_PATERNITY obj_paternity;
    /*!< Flag, which denotes if this object is child of another object. */

    scow_Error* error;
    /*!< Structure for error handling. */

    scow_Timer* timer;
    /*!< Structure for time measurements. */

    scow_Kernel* kernel;
    /*!< Minimal kernel, that is used for memory operations on Device side. */

    struct scow_Steel_Thread* parent_thread;
    /*!< Parent Steel Thread. */

    cl_mem cl_mem_object;
    /*!< OpenCL memory object - buffer or image. */

    cl_mem_flags mem_flags;
    /*!< Memory allocation flags. */

    cl_event unmap_evt;
    /*!< Internal event, that can be used for unmapping waiting. */

    /*! @name Memory object characteristics. */
    /**@{*/
    size_t size,
    /*!< Size of memory object in bytes. Applicable only for buffers. */

    width,
    /*!< Image width. Applicable only for images. */

    height,
    /*!< Image height. Applicable only for images. */

    row_pitch,
    /*!< Image row pitch. Applicable only for images. This field is valid
     * only when image is mapped. */

    origin;
    /*!< Start address of child memory object within bounds of parent
     *   memory object. For parent memory objects is always zero. */
    /**@}*/

    void *host_ptr,
    /*!< Pointer to memory, allocated by Host (if any). */

    *mapped_to_region;
    /*!< Pointer to mapped memory, if any mapping was made. */

    /*! @name Fucntion pointers. */
    /**@{*/
    size_t (*Get_Width)(struct scow_Mem_Object *self);
    /*!< Points on Buffer_Get_Width() or Image_Get_Width(). */

    size_t (*Get_Height)(struct scow_Mem_Object *self);
    /*!< Points on Buffer_Get_Height() or Image_Get_Height(). */

    size_t (*Get_Row_Pitch)(struct scow_Mem_Object *self);
    /*!< Points on Buffer_Get_Row_Pitch() or Image_Get_Row_Pitch(). */

    cl_mem* (*Get_Mem_Obj)(struct scow_Mem_Object *self);
    /*!< Points on Mem_Object_Get_Mem_Obj(). */

    void* (*Map)(struct scow_Mem_Object *self, cl_bool blocking_map,
            cl_map_flags map_flags, TIME_STUDY_MODE time_mode,
            cl_event* evt_to_generate, cl_command_queue explicit_queue);
    /*!< Points on Buffer_Map() or Image_Map(). */

    ret_code (*Unmap)(struct scow_Mem_Object *self, cl_bool blocking_map,
            void** p_mapped_ptr, TIME_STUDY_MODE time_mode,
            cl_event* evt_to_generate, cl_command_queue explicit_queue);
    /*!< Points on Buffer_Unmap() or Image_Unmap(). */

    ret_code (*Write)(struct scow_Mem_Object *self,
            cl_bool blocking_flag, void* source, TIME_STUDY_MODE time_mode,
            cl_event* evt_to_generate, cl_command_queue explicit_queue);
    /*!< Points on Buffer_Send_To_Device() or Image_Send_To_Device(). */

    ret_code (*Read)(struct scow_Mem_Object *self,
            cl_bool blocking_flag, void* destination, TIME_STUDY_MODE time_mode,
            cl_event* evt_to_generate, cl_command_queue explicit_queue);
    /*!< Points on Buffer_Get_From_Device() or Image_Get_From_Device(). */

    ret_code (*Copy)(struct scow_Mem_Object *self, struct scow_Mem_Object *dest,
            cl_bool blocking_flag, TIME_STUDY_MODE time_mode,
            cl_event* evt_to_generate, cl_command_queue explicit_queue);
    /*!< Points on Buffer_Copy() or Image_Copy(). */

    ret_code (*Swap)(struct scow_Mem_Object **self,
            struct scow_Mem_Object **dest);
    /*!< Points on Mem_Object_Swap(). */

    ret_code (*Erase)(struct scow_Mem_Object* self);
    /*!< Points on Buffer_Erase() of Image_Erase(). */

    ret_code(*Sync)(struct scow_Mem_Object* self, MEM_OBJECT_ETHALON ethalon,
        TIME_STUDY_MODE time_mode);
    /*!< Points on Mem_Object_Sync(). */

    struct scow_Mem_Object* (*Make_Child)(struct scow_Mem_Object *self,
            cl_mem_flags flags, cl_buffer_create_type buffer_create_type,
            const void *buffer_create_info);
    /*!< Points on Buffer_Make_Sub_Buffer(). */

    ret_code (*Destroy)(struct scow_Mem_Object *self);
/*!< Points on Mem_Object_Destroy(). */
/**@}*/

} scow_Mem_Object;

/*!
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
scow_Mem_Object* Make_Buffer(struct scow_Steel_Thread *parent_thread,
        const cl_mem_flags mem_flags, const size_t size, void* host_ptr);

/*!
 * This function allocates memory for Memory Object with OpenCL image & sets
 * function pointers.
 *
 * @param[in] parent_thread parent Steel Thread, which gives OpenCL context, etc
 * @param[in] mem_flags OpenCL memory flags, which will be used for OpenCL
 * memory objects creation
 * @param[in] image_format OpenCL image format, that describe characteristics
 * @param[in] width image width
 * @param[in] height image height
 * @param[in] row_pitch image row pitch (linesize in bytes)
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
    struct scow_Steel_Thread    *parent_thread,
    const cl_mem_flags          mem_flags,
    const cl_image_format       *image_format,
    const size_t                width, 
    const size_t                height, 
    const size_t                row_pitch,
    void                        *host_ptr);

#ifdef __cplusplus
}
#endif

#endif /* CL_MEM_OBJECT_H_ */
