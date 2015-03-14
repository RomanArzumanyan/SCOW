/*
 * @file platform.c
 * @brief Provides basic abstraction for OpenCL paltform
 *
 * @see platform.h
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "platform.h"
#include "error.h"

/* \cond PRIVATE */
static size_t Get_Parameter_Ret_String_Size(scow_Platform_Info* self,
        cl_platform_info info_wanted)
{
    cl_int ret;
    size_t size;

    ret = clGetPlatformInfo(self->parent_platform->platform, info_wanted, 0,
            NULL, &size);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, 0);

    return size;
}

static ret_code Get_Parameter_String(char* where_to_write,
        const unsigned int str_size, scow_Platform_Info* self,
        cl_platform_info info_wanted)
{
    cl_int ret = CL_SUCCESS;

    ret = clGetPlatformInfo(self->parent_platform->platform, info_wanted,
            str_size, where_to_write, NULL);

    return ret;
}

static ret_code Gather_Platform_Info(scow_Platform_Info* self)
{
    cl_int str_size = -1, ret = CL_SUCCESS;

    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);

    // Get information about profile supported
    str_size = Get_Parameter_Ret_String_Size(self, CL_PLATFORM_PROFILE);
    if (str_size < 1)
    {
        return CANT_QUERY_PLATFORM_PARAM;
    }

    self->cstr_profile_supported = (char*) calloc(str_size, sizeof(char));

    ret = Get_Parameter_String(self->cstr_profile_supported, str_size, self,
            CL_PLATFORM_PROFILE);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_PLATFORM_PARAM);

    // Get information about version
    str_size = Get_Parameter_Ret_String_Size(self, CL_PLATFORM_VERSION);

    if (str_size < 1)
    {
        self->Destroy(self);
        return CANT_QUERY_PLATFORM_PARAM;
    }

    self->cstr_version = (char*) calloc(str_size, sizeof(char));

    ret = Get_Parameter_String(self->cstr_version, str_size, self,
            CL_PLATFORM_VERSION);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_PLATFORM_PARAM);

    // Get information about name
    str_size = Get_Parameter_Ret_String_Size(self, CL_PLATFORM_NAME);

    if (str_size < 1)
    {
        self->Destroy(self);
        return CANT_QUERY_PLATFORM_PARAM;
    }

    self->cstr_name = (char*) calloc(str_size, sizeof(char));

    ret = Get_Parameter_String(self->cstr_name, str_size, self,
            CL_PLATFORM_NAME);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_PLATFORM_PARAM);

    // Get information about vendor
    str_size = Get_Parameter_Ret_String_Size(self, CL_PLATFORM_VENDOR);

    if (str_size < 1)
    {
        self->Destroy(self);
        return CANT_QUERY_PLATFORM_PARAM;
    }

    self->cstr_vendor = (char*) calloc(str_size, sizeof(char));

    ret = Get_Parameter_String(self->cstr_vendor, str_size, self,
            CL_PLATFORM_VENDOR);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_PLATFORM_PARAM);

    // Get information about extensions
    str_size = Get_Parameter_Ret_String_Size(self, CL_PLATFORM_EXTENSIONS);

    if (str_size < 1)
    {
        self->Destroy(self);
        return CANT_QUERY_PLATFORM_PARAM;
    }

    self->cstr_extensions = (char*) calloc(str_size, sizeof(char));

    ret = Get_Parameter_String(self->cstr_extensions, str_size, self,
            CL_PLATFORM_EXTENSIONS);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, NULL, CANT_QUERY_PLATFORM_PARAM);

    return ret;
}
/* \endcond */

/**
 * \related cl_Platform_Info_t
 *
 * This function destroy child structures & frees memory, allocated for structure,
 * defined by pointer 'self'
 *
 * @param[in,out] self pointer to structure of type 'cl_Platform_Info_t', in which
 * function pointer 'Destroy' is defined to point on this function

 * @return CL_SUCCESS always
 */
static ret_code Platform_Info_Destroy(scow_Platform_Info* self)
{
    OCL_CHECK_EXISTENCE(self, CL_SUCCESS);

    if (self->cstr_profile_supported)
    {
        free(self->cstr_profile_supported);
        self->cstr_profile_supported = NULL;
    }

    if (self->cstr_version)
    {
        free(self->cstr_version);
        self->cstr_version = NULL;
    }

    if (self->cstr_name)
    {
        free(self->cstr_name);
        self->cstr_name = NULL;
    }

    if (self->cstr_vendor)
    {
        free(self->cstr_vendor);
        self->cstr_vendor = NULL;
    }

    if (self->cstr_extensions)
    {
        free(self->cstr_extensions);
        self->cstr_extensions = NULL;
    }

    free(self);

    return CL_SUCCESS;
}

/**
 * \related cl_Platform_Info_t
 *
 * This function puts information about OpenCL platform into stdout
 *
 * @param[in] self pointer to structure of type 'cl_Platform_Info_t', in which
 * function pointer 'Print' is defined to point on this function

 * @return CL_SUCCESS always
 */
static ret_code Platform_Info_Print(const scow_Platform_Info* self,
        PLATFORM_INFO_PARAM param)
{
    OCL_CHECK_EXISTENCE(self, INVALID_BUFFER_GIVEN);

    fprintf(stdout, "\n---OpenCL Platform info:---\n");

    if (param == PLATFORM_PROFILE_SUPPORTED || param == PLATFORM_ALL_AVAILABLE)
        fprintf(stdout, "profile:       %s\n", self->cstr_profile_supported);

    if (param == PLATFORM_VERSION || param == PLATFORM_ALL_AVAILABLE)
        fprintf(stdout, "version:       %s\n", self->cstr_version);

    if (param == PLATFORM_NAME || param == PLATFORM_ALL_AVAILABLE)
        fprintf(stdout, "name:          %s\n", self->cstr_name);

    if (param == PLATFORM_VENDOR || param == PLATFORM_ALL_AVAILABLE)
        fprintf(stdout, "vendor:        %s\n", self->cstr_vendor);

    if (param == PLATFORM_EXTENSIONS || param == PLATFORM_ALL_AVAILABLE)
        fprintf(stdout, "extensions:    %s\n", self->cstr_extensions);

    return CL_SUCCESS;
}

/**
 * \related cl_Platform_Info_t
 *
 * This function returns NULL-terminated C string with information about OpenCL
 * platform. Memory for string is not allocated during the function call.
 *
 * @param[in] self pointer to structure of type 'cl_Platform_Info_t', in which
 * function pointer 'Get_Parameter' is defined to point on this function
 *
 * @param[in] wanted_parameter enumeration, that describes, what type of
 * platform information will be returned in string
 *
 * @return pointer to NULL-terminated string in case of success, NULL pointer
 * otherwise
 */
static const char* Platform_Info_Get_Parameter(const scow_Platform_Info* self,
        PLATFORM_INFO_PARAM wanted_parameter)
{
    char* ret = NULL;

    OCL_CHECK_EXISTENCE(self, NULL);

    switch (wanted_parameter)
    {
    case PLATFORM_PROFILE_SUPPORTED:
        ret = self->cstr_profile_supported;
        break;

    case PLATFORM_VERSION:
        ret = self->cstr_version;
        break;

    case PLATFORM_NAME:
        ret = self->cstr_name;
        break;

    case PLATFORM_VENDOR:
        ret = self->cstr_vendor;
        break;

    case PLATFORM_EXTENSIONS:
        ret = self->cstr_extensions;
        break;

    default:
        break;
    }

    return ret;
}

/**
 * \related cl_Platform_Info_t
 *
 * This function allocates memory for structure of type 'cl_Platform_Info_t', set
 * function pointers, structure fields & returns pointer to structure created
 * in case of success.
 *
 * @param[in] parent_platform Pointer to struct of type 'cl_Platform'
 * which is parent OpenCL platform, information about what is contained.
 *
 * @return pointer to created structure in case of success, VOID_DEVICE_PTR pointer otherwise.
 *
 * @see cl_err_codes.h for detailed error description.
 */
scow_Platform_Info* Make_Platform_Info(scow_Platform* parent_platform)
{
    scow_Platform_Info* self;
    ret_code ret;

    OCL_CHECK_EXISTENCE(parent_platform, VOID_PLATFORM_INFO_PTR);

    self = (scow_Platform_Info*) calloc(1, sizeof(*self));
    OCL_CHECK_EXISTENCE(self, VOID_PLATFORM_INFO_PTR);

    self->Destroy = Platform_Info_Destroy;
    self->Get_Parameter = Platform_Info_Get_Parameter;
    self->Print = Platform_Info_Print;

    self->parent_platform = parent_platform;

    ret = Gather_Platform_Info(self);

    OCL_DIE_ON_ERROR(ret, CL_SUCCESS, self->Destroy(self),
            VOID_PLATFORM_INFO_PTR);

    return self;
}

/*
 *
 * cl_Platform functions
 *
 */

/**
 * \related cl_Platform
 *
 * This function destroy child structures & frees memory, allocated for structure,
 * defined by pointer 'self'
 *
 * @param[in,out] self pointer to structure of type 'cl_Platform', in which
 * function pointer 'Destroy' is defined to point on this function

 * @return CL_SUCCESS always
 */
static ret_code Platform_Destroy(scow_Platform* self)
{
    OCL_CHECK_EXISTENCE(self, CL_SUCCESS);

    self->error->Destroy(self->error);
    self->info->Destroy(self->info);
    free(self);

    return CL_SUCCESS;
}

scow_Platform* Make_Platform(cl_platform_id given_platform)
{
    scow_Platform* self;

    self = (scow_Platform*) calloc(1, sizeof(*self));
    OCL_CHECK_EXISTENCE(self, VOID_PLATFORM_PTR);

    self->Destroy = Platform_Destroy;
    self->platform = given_platform;
    self->error = Make_Error();
    self->info = Make_Platform_Info(self);

    OCL_CHECK_EXISTENCE_AND_DO(self->info, self->Destroy(self),
                VOID_PLATFORM_PTR);

    return self;
}
