project(SCOW)
cmake_minimum_required(VERSION 2.8)

set (inc_dir ${CMAKE_CURRENT_SOURCE_DIR}/inc)
add_subdirectory(${inc_dir})

set (src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${src_dir})

#SCOW library
add_library(SCOW SHARED ${SCOW_HEADERS} ${SCOW_SOURCE})
include_directories(${SCOW_INC_PATH})

#SCOW tests executable
add_executable(SCOW_APP ${CMAKE_CURRENT_SOURCE_DIR}/main.c)

#Find OpenCL
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(OpenCL REQUIRED)
if(OPENCL_FOUND)
  include_directories(${OPENCL_INCLUDE_DIRS})
  target_link_libraries(SCOW ${OPENCL_LIBRARIES})
endif(OPENCL_FOUND)

#Link app with SCOW library
target_link_libraries(SCOW_APP SCOW)

#Set compiler flags
set(CMAKE_C_FLAGS "-std=c99")
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_BUILD_TYPE Debug)
