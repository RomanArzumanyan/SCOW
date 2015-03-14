/*
 * @file error.h
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

#ifndef CL_ERRORS_H_
#define CL_ERRORS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "err_codes.h"
#include "typedefs.h"

/*! \def CL_DEBUG
 * Macro, which turns on debugging options, such as verbose log, etc.
 */
#undef CL_DEBUG
#define CL_DEBUG    (1)

#if (CL_DEBUG == 1)
#include <stdio.h>
#endif

/*! \def err_log_func
 * Macro, which outputs to stderr error code 'ERR_CODE', function name, file name
 * & string, on which error happened, if \ref CL_DEBUG is set to 1.
 */
#define err_log_func(ERR_CODE)
#undef err_log_func

/*! \def error_message
 * Macro, which outputs error message to stderr, if \ref CL_DEBUG is set to 1.
 */
#define error_message(ERR_STRING)
#undef error_message

/*! \def ocl_error_message
 * Macro, which outputs OpenCL error code to stderr, if \ref CL_DEBUG is set to 1.
 */
#define ocl_error_message(OCL_RET_CODE)
#undef ocl_error_message

#if(CL_DEBUG == 1)

#ifndef err_log_func
#define err_log_func(ERR_CODE)\
        do {\
            fprintf(stderr, "Error happened in function %s at line %d at file %s. Error code: %d\n", __FUNCTION__, __LINE__, __FILE__, ERR_CODE);\
        } while(0)
#endif

#ifndef error_message
#define error_message(ERR_STRING)\
        do {\
            fprintf(stderr, "%s\n", ERR_STRING);\
        } while(0)
#endif

#ifndef ocl_error_message
#define ocl_error_message(OCL_RET_CODE)\
        do {\
            fprintf(stderr, "OpenCL error %d happened at line %d in function %s at file %s", OCL_RET_CODE, __LINE__, __FUNCTION__, __FILE__);\
        } while(0)
#endif

#else

#ifndef err_log_func
#define err_log_func(ERR_CODE)
#endif

#ifndef error_message
#define error_message(ERR_STRING)
#endif

#ifndef ocl_error_message
#define ocl_error_message(OCL_RET_CODE)
#endif

#endif

/*! \def OCL_DIE_ON_ERROR
 * Macro, that checks RESULT against EXPECTED_RESULT, and if they are not
 * equal, calls CALLBACK & returns RET_VALUE.
 */
#define OCL_DIE_ON_ERROR(RESULT, EXPECTED_RESULT, CALLBACK, RET_VALUE)
#undef OCL_DIE_ON_ERROR

// Define macro, that will check return code & optionally die with error message
#ifndef OCL_DIE_ON_ERROR
#define OCL_DIE_ON_ERROR(RESULT, EXPECTED_RESULT, CALLBACK, RET_VALUE) \
do {\
    if((RESULT) != (EXPECTED_RESULT))\
    {\
        if((CALLBACK)) {}; \
        err_log_func(RESULT);\
        return (RET_VALUE);\
    }\
} while(0)
#endif

/*! \def OCL_CHECK_EXISTENCE
 * Macro, that checks that POINTER isn't NULL, and if it is NULL, it returns
 * RET_VALUE
 */
#define OCL_CHECK_EXISTENCE(POINTER, RET_VALUE)
#undef OCL_CHECK_EXISTENCE

#ifndef OCL_CHECK_EXISTENCE
#define OCL_CHECK_EXISTENCE(POINTER, RET_VALUE)\
do {\
    if((POINTER) == ((void*)0))\
    {\
        return (RET_VALUE);\
    }\
} while(0)
#endif

/*! \def OCL_CHECK_EXISTENCE_AND_DO
 * Macro, that checks that POINTER isn't NULL, and if it is NULL, it calls
 * CALLBACK & returns RET_VALUE
 */
#define OCL_CHECK_EXISTENCE_AND_DO(POINTER, CALLBACK, RET_VALUE)
#undef OCL_CHECK_EXISTENCE_AND_DO

#ifndef OCL_CHECK_EXISTENCE_AND_DO
#define OCL_CHECK_EXISTENCE_AND_DO(POINTER, CALLBACK, RET_VALUE)\
do {\
    if((POINTER) == ((void*)0))\
    {\
        if((CALLBACK)) {};\
        return (RET_VALUE);\
    }\
} while(0)
#endif

/*! \def VOID_ERROR_PTR
 * Void pointer to Error structure
 */
#undef VOID_ERROR_PTR
#define VOID_ERROR_PTR      ((scow_Error*)0x0)

/*! \def ERR_MSG_MAX_LEN
 * Max length of error message string
 */
#undef ERR_MSG_MAX_LEN
#define ERR_MSG_MAX_LEN     (256)

/*! \struct scow_Error
 *
 *  This structure is made for simple error handling. It includes functional:
 *    - Setting/Getting error code(s) for structures, that includes scow_Error
 *    - Getting error message
 */
typedef struct scow_Error
{
    ret_code rt_last_code;
    /*!< Last operation return code. */

    /*! @name Function pointers. */
    /**@{*/

    /*! Points to Error_Get_Last_Code(). */
    ret_code (*Get_Last_Code)(const struct scow_Error *self);

    /*! Points to Error_Set_Last_Code(). */
    ret_code (*Set_Last_Code)(struct scow_Error *self, const ret_code code);

    /*! Points to Error_Get_Error_Message(). */
    const char* (*Get_Error_Message)(const struct scow_Error *self);

    /*! Points to Error_Destroy(). */
    ret_code (*Destroy)(struct scow_Error *self);

/**@}*/
} scow_Error;

/**
 * This function allocates memory for structure of type 'scow_Error', set
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
scow_Error* Make_Error(void);

#ifdef __cplusplus
}
#endif

#endif /* CL_ERRORS_H_ */
