/*
 * @file error.c
 * @brief Provides error handling functionality
 *
 * @see error_codes.h
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

#include "error.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*! \def MAX_ERROR_MESSAGE_LEN
 * Maximal length of error message string
 */
#undef MAX_ERROR_MESSAGE_LEN
#define MAX_ERROR_MESSAGE_LEN   (256)

/*! \cond PRIVATE */
typedef struct Error_Code_With_Message
{
    ret_code code;
    /*!< Code of the error. */

    char message[MAX_ERROR_MESSAGE_LEN];
/*!< Error message. */
} Error_Code_With_Message;
/*! \endcond */

/**
 * \related cl_Error_t
 *
 * This function release memory, allocated for 'self' structure
 *
 * @param[in,out] p_self pointer to pointer to structure 'self' of type 'cl_Error_t',
 * in which fptr 'Destroy' is defined to point on this function
 *
 * @return CL_SUCCESS in case of success, error code otherwise.
 *
 * @see err_codes.h
 * @see error.c
 */
static ret_code Error_Destroy(scow_Error* self)
{
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);

    free(self);

    return CL_SUCCESS;
}

/**
 * \related cl_Error_t
 *
 * This function return value 'rt_last_code' of structure 'self'
 *
 * @param[in] self pointer to cl_Error_t structure, in which fptr (*Destroy) is defined to point on this function
 *
 * @return return code of last operation, executed via function pointers of 'self' structure
 */
static ret_code Error_Get_Last_Code(const scow_Error* self)
{
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);

    return self->rt_last_code;
}

/**
 * \related cl_Error_t
 *
 * This function do bitwise addition of error code, given by argument 'code' and field 'rt_last_code' of structure, defined by pointer 'self'
 *
 * @param[out] self pointer to cl_Error_t structure, in which fptr (*Destroy) is defined to point on this function
 * @param[in] code value, that will be written in 'rt_last_code' field of structure 'self'
 *
 * @return CL_SUCCESS always
 */
static ret_code Error_Set_Last_Code(scow_Error* self, const ret_code code)
{
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);

    self->rt_last_code |= code;

    return CL_SUCCESS;
}

/**
 * \related cl_Error_t
 *
 * This function returns message for 'rt_last_code' field of 'this' structure
 *
 * @param[out] self pointer to cl_Error_t structure, in which fptr (*Destroy) is defined to point on this function
 * @return NULL-terminated C string, which contains error message(s)
 *
 * @warning This function allocates memory for message
 */
static const char* Error_Get_Error_Message(const scow_Error* self)
{
    OCL_CHECK_EXISTENCE(self, NULL);

    char* error_message = (char*) calloc(ERR_MSG_MAX_LEN,
            sizeof(*error_message));

    switch (self->Get_Last_Code(self))
    {
    case CL_SUCCESS:
        strcpy(error_message, "Successful operation. No errors happened.\n");
        ;
        break;

    case BUFFER_NOT_ALLOCATED:
        strcpy(error_message, "Failed to allocate memory for buffer.\n");
        ;
        break;

    case INVALID_BLOCKING_FLAG:
        strcpy(error_message,
                "Invalid blocking flag - not CL_TRUE, nor CL_FALSE.\n");
        break;

    case INVALID_BUFFER_SIZE:
        strcpy(error_message, "Invalid buffer size given.\n");
        break;

    case INVALID_BUFFER_GIVEN:
        strcpy(error_message, "Invalid buffer given.\n");
        break;

    case OBJECT_DOESNT_EXIST:
        strcpy(error_message,
                "Attmept to access object, that doesn't exist.\n");
        break;

    case INVALID_ARG_TYPE:
        strcpy(error_message, "Argument of invalid type was passed.\n");
        break;

    case VOID_ARG_GIVEN:
        strcpy(error_message, "Given argument is void.\n");
        break;

    case ARG_NOT_FOUND:
        strcpy(error_message, "Given argument not found.\n");
        break;

    case CANT_FIND_PARAMS:
        strcpy(error_message, "Invalid OpenCL initialization params given.\n");
        break;

    case CANT_CREATE_CONTEXT:
        strcpy(error_message, "Can't create OpenCL context.\n");
        break;

    case CANT_CREATE_PROGRAM:
        strcpy(error_message, "Can't create OpenCL program with source.\n");
        break;

    case CANT_CREATE_CMD_QUEUE:
        strcpy(error_message, "Can't create OpenCL command queue.\n");
        break;

    case CANT_FIND_DEVICE:
        strcpy(error_message, "Can't find suitable OpenCL Device(s).\n");
        break;

    case CANT_FIND_PLATFORM:
        strcpy(error_message, "Can't find OpenCL platform(s).\n");
        break;

    case CANT_FIND_KERNEL_SOURCE:
        strcpy(error_message,
                "Can't find file(s) with OpenCL C source code.\n");
        break;

    case CANT_INIT_OPENCL:
        strcpy(error_message, "Can't initialize OpenCL.\n");
        break;

    case CANT_SET_ND_SIZE:
        strcpy(error_message, "Invalid ND dimension(s) given.\n");
        break;

    case CANT_QUERY_DEVICE_PARAM:
        strcpy(error_message,
                "Can't query OpenCL Device for one of the parameter(s).\n");
        break;

    case CANT_QUERY_PLATFORM_PARAM:
        strcpy(error_message,
                "Can't query OpenCL platform for one of the parameter(s).\n");
        break;

    case GENERIC_OPENCL_ERROR:
        strcpy(error_message, "Generic OpenCL-related error.\n");
        break;

    case DISTINCT_MEM_OBJECTS:
        strcpy(error_message,
                "Attempt to do operation between Buffer & Image.\n");
        break;

    case DISTINCT_MEM_FLAGS:
        strcpy(error_message,
                "Attempt to do operation objects with different memory allocation flags.\n");
        break;

    case MEM_OBJ_NOT_MAPPED:
        strcpy(error_message, "Memory object is not mapped.\n");
        break;

    case MEM_OBJ_INCONSISTENT:
        strcpy(error_message, "Memory object is inconsistent.\n");
        break;

    case KERNEL_DOESNT_EXIST:
        strcpy(error_message,
                "Trying to operate with kernel, which doesn't exist.\n");
        break;

    case INVALID_LOCAL_WG_SIZE:
        strcpy(error_message,
                "Given Local Workgroup size is invalid for kernel.\n");
        break;

    case INVALID_GLOBAL_WG_SIZE:
        strcpy(error_message,
                "Given Global Workgroup size is invalid for kernel.\n");
        break;

    case GLOBAL_NOT_MULTIPLE_TO_LOCAL:
        strcpy(error_message,
                "Given Global Workgroup size isn't multiple to Local Workgroup size.\n");
        break;

    case INVALID_ND_DIMENSIONALITY:
        strcpy(error_message, "Given kernel dimensionality is invalid.\n");
        break;

    case INVALID_EVENT:
        strcpy(error_message, "Invalid or non-existent OpenCL event.\n");
        break;

    case VALUE_OUT_OF_RANGE:
        strcpy(error_message, "Value lays out of acceptable range.\n");
        break;

    case TIMER_IN_USE:
        strcpy(error_message, "Can't access timer - it's already in use.\n");
        break;

    case BUFFER_IN_USE:
        strcpy(error_message, "Can't access buffer - it's already in use.\n");
        break;

    case CANT_SET_DEFAULT_OBJ:
        strcpy(error_message,
                "Can't set default object. Possibly - can't lock mutex.\n");
        break;

    case CALLING_STUB_ACCESSOR:
        strcpy(error_message,
                "Can't call non-implemented function through pointer.\n");
        break;

    case CALLING_UNDEF_ACCESSOR:
        strcpy(error_message,
                "Calling function, that is unapplicable for object of this type. \n");
        break;

    case PROVIDING_UNDEF_PTR:
        strcpy(error_message, "Providing undefined pointer. \n");
        break;

    default:
        strcpy(error_message, "Unspecified error.\n");
        break;
    }

    return error_message;
}

/**
 * \related cl_Error_t
 *
 * This function allocates memory for structure of type 'cl_Error_t', set
 * function pointers, structure fields & returns pointer to structure created
 * in case of success.
 *
 * @return pointer to created structure in case of success, \ref VOID_ERROR_PTR
 * otherwise.
 *
 * @see cl_err_codes.h for detailed error description.
 *
 * @warning always use 'Destroy' function pointer inside a Host Translation
 * structure to free memory, allocated by this function.
 */
scow_Error* Make_Error(void)
{
    scow_Error* self = (scow_Error*) calloc(1, sizeof(*self));
    OCL_CHECK_EXISTENCE(self, VOID_ERROR_PTR);

    self->Destroy = Error_Destroy;
    self->Get_Last_Code = Error_Get_Last_Code;
    self->Set_Last_Code = Error_Set_Last_Code;
    self->Get_Error_Message = Error_Get_Error_Message;

    return self;
}
