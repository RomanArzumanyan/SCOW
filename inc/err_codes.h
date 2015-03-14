/*
 * @file err_codes.h
 * @brief Error codes categorization & definition
 *
 * @see error.c
 * @see error.h
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

#ifndef CL_ERR_CODES_H_
#define CL_ERR_CODES_H_

#ifdef __cplusplus
extern "C" {
#endif

/*! \def INVALID_ARG_GIVEN_BASE
 * Base code for errors, connected with invalid arguments.
 */
#undef INVALID_ARG_GIVEN_BASE
#define INVALID_ARG_GIVEN_BASE          (ret_code)(100)

/*! \def FAILED_TO_CREATE_OBJ_BASE
 * Base code for errors, connected with structure creation failures
 */
#undef FAILED_TO_CREATE_OBJ_BASE
#define FAILED_TO_CREATE_OBJ_BASE       (ret_code)(200)

/*! \def OPENCL_RELATED_ERRORS_BASE
 * Base code for errors, happened when interacting with OpenCL objects
 */
#undef OPENCL_RELATED_ERRORS_BASE
#define OPENCL_RELATED_ERRORS_BASE      (ret_code)(300)

/*! \def PARENT_CHILD_ERRORS_BASE
 * Base code for errors happened when parent-child relations are violated
 */
#undef PARENT_CHILD_ERRORS_BASE
#define PARENT_CHILD_ERRORS_BASE        (ret_code)(400)

/*! \def OBJECT_IN_USE_BASE
 * Base code for errors, happened when trying to use blocked objects
 */
#undef OBJECT_IN_USE_BASE
#define OBJECT_IN_USE_BASE              (ret_code)(500)

/*! \def ACCESSORS_BASE
 * Base code for errors, related to function pointers in structures
 */
#undef ACCESSORS_BASE
#define ACCESSORS_BASE                  (ret_code)(600)

/*! \def OUT_OF_RANGE_BASE
 * Base code for errors, related to parameter value acceptable range mismatch
 */
#undef OUT_OF_RANGE_BASE
#define OUT_OF_RANGE_BASE               (ret_code)(700)

/*------------------Invalid argument error codes------------------------------*/
/*! @name INVALID_ARG_GIVEN.
 * Invalid argument error codes.
 **@{*/

/*! \def INVALID_BLOCKING_FLAG
 * Blocking flag is not CL_TRUE nor CL_FALSE
 */
#undef INVALID_BLOCKING_FLAG
#define INVALID_BLOCKING_FLAG           (INVALID_ARG_GIVEN_BASE + 1)

/*! \def INVALID_BUFFER_SIZE
 * Size of given buffer is invalid
 */
#undef INVALID_BUFFER_SIZE
#define INVALID_BUFFER_SIZE             (INVALID_ARG_GIVEN_BASE + 2)

/*! \def INVALID_BUFFER_GIVEN
 * Void buffer or buffer of wrong type given as argument
 */
#undef INVALID_BUFFER_GIVEN
#define INVALID_BUFFER_GIVEN            (INVALID_ARG_GIVEN_BASE + 3)

/*! \def OBJECT_DOESNT_EXIST
 * Attempt to use object, that doesn't exist
 */
#undef OBJECT_DOESNT_EXIST
#define OBJECT_DOESNT_EXIST             (INVALID_ARG_GIVEN_BASE + 4)

/*! \def INVALID_ARG_TYPE
 * Argument of invalid type was passed
 */
#undef INVALID_ARG_TYPE
#define INVALID_ARG_TYPE                (INVALID_ARG_GIVEN_BASE + 5)

/*! \def VOID_ARG_GIVEN
 * Given argument is void
 */
#undef VOID_ARG_GIVEN
#define VOID_ARG_GIVEN                  (INVALID_ARG_GIVEN_BASE + 6)

/*! \def VOID_ARG_GIVEN
 * Given argument is void
 */
#undef ARG_NOT_FOUND
#define ARG_NOT_FOUND                   (INVALID_ARG_GIVEN_BASE + 7)
/**@}*/

/*----------------------Objects creation error codes--------------------------*/
/*! @name FAILED_TO_CREATE_OBJ.
 * Objects creation error codes.
 **@{*/

/*! \def BUFFER_NOT_ALLOCATED
 * Error happened during memory allocation
 */
#undef BUFFER_NOT_ALLOCATED
#define BUFFER_NOT_ALLOCATED            (FAILED_TO_CREATE_OBJ_BASE + 1)
/**@}*/

/*--------------------OpenCL-related error codes------------------------------*/
/*! @name OPENCL_RELATED_ERRORS.
 * OpenCL-related error codes.
 **@{*/

/*! \def CANT_FIND_PARAMS
 * No OpenCL initialization parameters given
 */
#undef CANT_FIND_PARAMS
#define CANT_FIND_PARAMS                (OPENCL_RELATED_ERRORS_BASE + 1)

/*! \def CANT_CREATE_CONTEXT
 * Can't create OpenCL context
 */
#undef CANT_CREATE_CONTEXT
#define CANT_CREATE_CONTEXT             (OPENCL_RELATED_ERRORS_BASE + 2)

/*! \def CANT_CREATE_PROGRAM
 * Can't create program with source
 */
#undef CANT_CREATE_PROGRAM
#define CANT_CREATE_PROGRAM             (OPENCL_RELATED_ERRORS_BASE + 3)

/*! \def CANT_CREATE_CMD_QUEUE
 * Can'r create command queue
 */
#undef CANT_CREATE_CMD_QUEUE
#define CANT_CREATE_CMD_QUEUE           (OPENCL_RELATED_ERRORS_BASE + 4)

/*! \def CANT_FIND_DEVICE
 * Can't find suitable OpenCL Device
 */
#undef CANT_FIND_DEVICE
#define CANT_FIND_DEVICE                (OPENCL_RELATED_ERRORS_BASE + 5)

/*! \def CANT_FIND_PLATFORM
 * Can't find suitable OpenCL platform
 */
#undef CANT_FIND_PLATFORM
#define CANT_FIND_PLATFORM              (OPENCL_RELATED_ERRORS_BASE + 6)

/*! \def CANT_FIND_KERNEL_SOURCE
 * Can't find files with OpenCL C source code
 */
#undef CANT_FIND_KERNEL_SOURCE
#define CANT_FIND_KERNEL_SOURCE         (OPENCL_RELATED_ERRORS_BASE + 7)

/*! \def CANT_INIT_OPENCL
 * Generic error while initializing OpenCL
 */
#undef CANT_INIT_OPENCL
#define CANT_INIT_OPENCL                (OPENCL_RELATED_ERRORS_BASE + 8)

/*! \def CANT_SET_ND_SIZE
 * ND Range dimensions given are invalid for kernel to execute on OpenCL Device
 */
#undef CANT_SET_ND_SIZE
#define CANT_SET_ND_SIZE                (OPENCL_RELATED_ERRORS_BASE + 9)

/*! \def CANT_QUERY_DEVICE_PARAM
 * Can't query OpenCL Device for parameter
 */
#undef CANT_QUERY_DEVICE_PARAM
#define CANT_QUERY_DEVICE_PARAM         (OPENCL_RELATED_ERRORS_BASE + 10)

/*! \def CANT_QUERY_PLATFORM_PARAM
 * Can't query OpenCL Platform for parameter
 */
#undef CANT_QUERY_PLATFORM_PARAM
#define CANT_QUERY_PLATFORM_PARAM       (OPENCL_RELATED_ERRORS_BASE + 11)

/*! \def GENERIC_OPENCL_ERROR
 * Generic OpenCL-related error
 */
#undef GENERIC_OPENCL_ERROR
#define GENERIC_OPENCL_ERROR            (OPENCL_RELATED_ERRORS_BASE + 12)

/*! \def DISTINCT_MEM_OBJECTS
 * Trying to use 2 different memory objects - buffer & image
 */
#undef DISTINCT_MEM_OBJECTS
#define DISTINCT_MEM_OBJECTS            (OPENCL_RELATED_ERRORS_BASE + 13)

/*! \def DISTINCT_MEM_FLAGS
 * Trying to operate with objects, that have different memory allocation flags
 */
#undef DISTINCT_MEM_FLAGS
#define DISTINCT_MEM_FLAGS              (OPENCL_RELATED_ERRORS_BASE + 14)

/*! \def MEM_OBJ_NOT_MAPPED
 * Trying to do (un)mapping operations to object, which is not mapped
 */
#undef MEM_OBJ_NOT_MAPPED
#define MEM_OBJ_NOT_MAPPED              (OPENCL_RELATED_ERRORS_BASE + 15)

/*! \def MEM_OBJ_INCONSISTENT
 * Host pointer & mapping fields have mutual exclusive values (e. g. image plane
 * is indicated to be mapped, but Host pointer is NULL). This means that Memory
 * Object has lost consistency FOR EXTERNAL STRUCTURE.
 */
#undef MEM_OBJ_INCONSISTENT
#define MEM_OBJ_INCONSISTENT            (OPENCL_RELATED_ERRORS_BASE + 16)

/*! \def KERNEL_DOESNT_EXIST
 * Trying to operate with OpenCL kernel, which doesn't exist
 */
#undef KERNEL_DOESNT_EXIST
#define KERNEL_DOESNT_EXIST             (OPENCL_RELATED_ERRORS_BASE + 17)

/*! \def INVALID_LOCAL_WG_SIZE
 * Local WorkGroup size is invalid for OpenCL kernel
 */
#undef INVALID_LOCAL_WG_SIZE
#define INVALID_LOCAL_WG_SIZE           (OPENCL_RELATED_ERRORS_BASE + 18)

/*! \def INVALID_GLOBAL_WG_SIZE
 * Global WorkGroup size is invalid for OpenCL kernel
 */
#undef INVALID_GLOBAL_WG_SIZE
#define INVALID_GLOBAL_WG_SIZE          (OPENCL_RELATED_ERRORS_BASE + 19)

/*! \def GLOBAL_NOT_MULTIPLE_TO_LOCAL
 * Global WorkGroup size isn't multiple of Local WorkGroup size
 */
#undef GLOBAL_NOT_MULTIPLE_TO_LOCAL
#define GLOBAL_NOT_MULTIPLE_TO_LOCAL    (OPENCL_RELATED_ERRORS_BASE + 20)

/*! \def INVALID_ND_DIMENSIONALITY
 * Given dimensionality is invalid for kernel
 */
#undef INVALID_ND_DIMENSIONALITY
#define INVALID_ND_DIMENSIONALITY       (OPENCL_RELATED_ERRORS_BASE + 21)

/*! \def INVALID_EVENT
 * Invalid or non-existent event given
 */
#undef INVALID_EVENT
#define INVALID_EVENT                   (OPENCL_RELATED_ERRORS_BASE + 22)
/**@}*/

/*----------------------Parent-child error codes------------------------------*/
/*! @name PARENT_CHILD_ERRORS.
 * Parent-child error codes.
 **@{*/

/*! \def WRONG_PARENT_OBJECT
 * Invalid parent object given
 */
#undef WRONG_PARENT_OBJECT
#define WRONG_PARENT_OBJECT             (PARENT_CHILD_ERRORS_BASE + 1)
/**@}*/

/*--------------------------Object-in-use error codes-------------------------*/
/*! @name OBJECT_IN_USE.
 * Object-in-use error codes.
 **@{*/

/*! \def TIMER_IN_USE
 * Timer is in use & can't be re-entered
 */
#undef TIMER_IN_USE
#define TIMER_IN_USE                    (OBJECT_IN_USE_BASE + 1)

/*! \def BUFFER_IN_USE
 * Buffer is in use (e. g. mapped) & can't be accessed
 */
#undef BUFFER_IN_USE
#define BUFFER_IN_USE                   (OBJECT_IN_USE_BASE + 2)

/*! \def CANT_SET_DEFAULT_OBJ
 * Can't set default object (e. g. default OpenCL Device for given OpenCL platform)
 */
#undef CANT_SET_DEFAULT_OBJ
#define CANT_SET_DEFAULT_OBJ            (OBJECT_IN_USE_BASE + 3)
/**@}*/

/*--------------------------Accessor-related error codes----------------------*/
/*! @name ACCESSORS.
 * Accessor-related error codes.
 **@{*/

/*! \def CALLING_STUB_ACCESSOR
 * Trying to call non-implemented function pointer
 */
#undef CALLING_STUB_ACCESSOR
#define CALLING_STUB_ACCESSOR           (ACCESSORS_BASE + 1)

/*! \def CALLING_UNDEF_ACCESSOR
 * Calling function pointer, which is undefined for object (e. g. trying to get
 * image characteristics for buffer object or vice versa).
 */
#undef CALLING_UNDEF_ACCESSOR
#define CALLING_UNDEF_ACCESSOR          (ACCESSORS_BASE + 2)

/*! \def PROVIDING_UNDEF_PTR
 * Providing NULL function pointer, that may lead to unhandled page exception.
 */
#undef PROVIDING_UNDEF_PTR
#define PROVIDING_UNDEF_PTR             (ACCESSORS_BASE + 3)
/**@}*/

/*----------------------Out of range error codes------------------------------*/
/*! @name OUT_OF_RANGE.
 * Out of range error codes.
 **@{*/
/*! \def VALUE_OUT_OF_RANGE
 * Generic out-of-range error
 */
#undef VALUE_OUT_OF_RANGE
#define VALUE_OUT_OF_RANGE              (OUT_OF_RANGE_BASE + 3)
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CL_ERR_CODES_H_ */
