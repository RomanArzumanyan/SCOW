/*
 * @file device.h
 * @brief Header for wrapper, that provides common OpenCL Device functionality
 *
 * @see platform.h
 * @see steel_thread.h
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

#ifndef CL_DEVICE_H_
#define CL_DEVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "error.h"

/*! \def VOID_DEVICE_PTR
 * Void pointer on OpenCL Device
 */
#undef VOID_DEVICE_PTR
#define VOID_DEVICE_PTR             ((scow_Device*)0x0)

/*! \def CL_DEVICE_NAME_SIZE
 * Maximal length of string with OpenCL Device's name
 */
#undef CL_DEVICE_NAME_SIZE
#define CL_DEVICE_NAME_SIZE         (256)

/*! \def CL_DEVICE_EXTENSIONS_SIZE
 * Maximal length of string with list of OpenCL Device extensions description
 */
#undef CL_DEVICE_EXTENSIONS_SIZE
#define CL_DEVICE_EXTENSIONS_SIZE   (1024)

/*! \def CL_DEVICE_EXTENSIONS_SIZE
 * Maximal length of string with list of OpenCL Device version information
 */
#undef CL_DEVICE_VERSION_SIZE
#define CL_DEVICE_VERSION_SIZE      (1024)

/*! \def CL_DEVICE_EXTENSIONS_SIZE
 * Maximal length of string with list of OpenCL Device driver version information
 */
#undef CL_DRIVER_VERSION_SIZE
#define CL_DRIVER_VERSION_SIZE      (1024)

typedef enum DEVICE_CREATION_MODE
{
    DEVICE_CREATE_QUICK = 0,
    /*!< Only create Device and gather no information. It's the fastest way to create Device */

    DEVICE_CREATE_AND_GATHER_INFO
/*!< Create Device and gather information about it. It's slower way to create Device */

} DEVICE_CREATION_MODE;

/*! \var DEVICE_INFO_PARAM
 * This enumeration describes what kind of OpenCL Device properties can be
 * queried.
 */
typedef enum DEVICE_INFO_PARAM
{
    DEVICE_ALL_AVAILABLE = 0,
    /*!< Combinations of all parameters below. */

    DEVICE_NAME,
    /*!< Device name string. */

    DEVICE_EXTENSIONS,
    /*!< List of extension names. */

    DEVICE_EXECUTION_CAPABILITIES,
    /*!< *List of Device execution capabilities. */

    DEVICE_MAX_COMPUTE_UNITS,
    /*!< The number of parallel compute cores on the OpenCL device. */

    DEVICE_MEM_BASE_ADDR_ALIGN,
    /*!< Describes the alignment in bits of the base address of any
     * allocated memory object. */

    DEVICE_MIN_DATA_TYPE_ALIGN_SIZE,
    /*!< The smallest alignment in bytes which can be used for any data type. */

    DEVICE_MAX_CLOCK_FREQUENCY,
    /*!< Maximum configured clock frequency of the device in MHz. */

    DEVICE_GLOBAL_MEM_CACHELINE_SIZE,
    /*!< Size of global memory cache line in bytes. */

    DEVICE_GLOBAL_MEM_SIZE,
    /*!< Size of global device memory in bytes. */

    DEVICE_GLOBAL_MEM_CACHE_SIZE,
    /*!< Size of global memory cache in bytes. */

    DEVICE_NATIVE_VECTOR_WIDTH_CHAR,
    /*!< Native vector width for char data type. */

    DEVICE_NATIVE_VECTOR_WIDTH_SHORT,
    /*!< Native vector width for short data type. */

    DEVICE_NATIVE_VECTOR_WIDTH_INT,
    /*!< Native vector width for int data type. */

    DEVICE_NATIVE_VECTOR_WIDTH_LONG,
    /*!< Native vector width for long data type. */

    DEVICE_NATIVE_VECTOR_WIDTH_FLOAT,
    /*!< Native vector width for float data type. */

    DEVICE_VERSION,
    /*!< OpenCL version, supported by Device. */

    DRIVER_VERSION
/*!< Version of OpenCL driver. */
} DEVICE_INFO_PARAM;

/*! \struct scow_Device
 *
 *  This structure is wrapper for cl_device_id provided by OpenCL API.
 *  It provides basic functionality for OpenCL Device:
 *    - Auto-detection of OpenCL Devices of desired type under particular OpenCL platform,
 *    - Navigation through Devices list under one platform
 *    - Making OpenCL Device default for parent OpenCL platform
 *    - OpenCL Device properties information gathering.
 *
 *  @see 'scow_Platform' structure description for details about parent platform
 */
typedef struct scow_Device
{
    scow_Error* error;
    /*!< Structure for errors handling. */

    cl_device_id device_id;
    /*!< OpenCL Device structure from OpenCL API. */

    char name[CL_DEVICE_NAME_SIZE],
    /*!< Device name string. */

    extensions[CL_DEVICE_EXTENSIONS_SIZE],
    /*!< List of extension names. */

    device_version[CL_DEVICE_VERSION_SIZE],
    /*!< Version of OpenCL driver. */

    driver_version[CL_DRIVER_VERSION_SIZE];
    /*!< Version of OpenCL driver. */

    /*! @name Device properties.
     * These fields contain various information about OpenCL Device. */
    /**@{*/
    cl_device_exec_capabilities exec_capabilities;
    /*!< Describe execution capabilities of OpenCL Device. */

    cl_uint max_compute_units,
    /*!< The number of parallel compute cores on the OpenCL Device. */

    mem_base_addr_align,
    /*!< Describes the alignment in bits of the base address of any
     * allocated memory object. */

    min_data_type_align_size,
    /*!< The smallest alignment in bytes which can be used for any data type. */

    max_clock_frequency,
    /*!< Maximum configured clock frequency of the device in MHz. */

    global_mem_cacheline_size,
    /*!< Size of global memory cache line in bytes. */

    native_vector_width_char,
    /*!< Native vector width for char. */

    native_vector_width_short,
    /*!< Native vector width for short. */

    native_vector_width_int,
    /*!< Native vector width for int. */

    native_vector_width_long,
    /*!< Native vector width for long. */

    native_vector_width_float;
    /*!< Native vector width for float. */

    cl_ulong 
    max_alloc_mem_size,
    /*!< Maximal amount of memory, that can be allocated in bytes. */

    global_mem_size,
    /*!< Size of global device memory in bytes. */

    global_mem_cache_size;
    /*!< Size of global memory cache in bytes. */
    /**@}*/

    /*! @name Function pointers. */
    /**@{*/

    /*! Points on Device_Gather_Info(). */
    ret_code (*Gather_Info)(struct scow_Device *self, DEVICE_INFO_PARAM param);

    /*! Points on Device_Print_Info(). */
    ret_code (*Print_Info)(const struct scow_Device *self,
            DEVICE_INFO_PARAM param);

    /*! Points on Device_Destroy(). */
    ret_code (*Destroy)(struct scow_Device *self);

    /**@}*/

} scow_Device;

/**
 * This function detects all OpenCL Devices of wanted type under given platform,
 * allocates memory for list of structures of type 'scow_Device', set
 * function pointers, structure fields & returns pointer to first element of
 * the list of structures in case of success. List of devices contain all
 * OpenCL Devices of wanted types, that were found.
 *
 * @param[in,out] parent_platform pointer to parent OpenCL platform for OpenCL Device created
 * @param[in] creation_type enumeration, that describes how to create Device.
 * @param[in] wanted_device_type type of Device we want to initialize under parent platform.
 * @return pointer to created structure in case of success, \ref VOID_DEVICE_PTR pointer otherwise.
 *
 * @warning always use 'Destroy' function pointer of Device Translation structure
 * to free memory, allocated by this function
 */
scow_Device* Make_Device(cl_device_id given_device);

/**
 * @brief This function make query about how many OpenCL SubSevices can be 
 * retrieved by fission of given OpenCL Device.
 *
 * @param[in] given_device OpenCL Device to be fissioned.
 * @param[in] properties Device fission properties.
 * @param[out] ret operation return code.
 *
 * @return number of available SubDevices. In case of error set
 * value of 'ret' variable to corresponding error code.
 */
cl_uint Get_MA_Subdevices_Num(
    cl_device_id                        given_device,
    const cl_device_partition_property  *properties,
    ret_code                            *ret);

/**
 * @brief This function does fission on given OpenCL Device into
 * SubDevices, allocates memory for them & return allocated array.
 *
 * @param[in] given_device given OpenCL Device to be fissioned.
 * @param[in] properties Device fission properties.
 * @param[in] num_devices desired SubDevices number.
 * @param[out] ret operation return code.
 *
 * @return array of OpenCL SubDevices in case of success, \ref VOID_OPENCL_DEVICE_ID_PTR
 * otherwise. In case of error, error code is returned in 'ret' variable.
 */
cl_device_id* Make_Subdevices(
    cl_device_id                        given_device,
    const cl_device_partition_property  *properties,
    size_t                              num_devices,
    ret_code                            *ret);

#ifdef __cplusplus
}
#endif

#endif /* CL_DEVICE_H_ */
