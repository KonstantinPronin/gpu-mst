cmake_minimum_required(VERSION 3.15)
project(gpu_mst)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCL REQUIRED)

include_directories(include)
include_directories(${OpenCL_INCLUDE_DIRS})

file(GLOB_RECURSE LIB_SOURCE_FILES "source/*.cpp")

add_library(gpu_mst_lib STATIC ${LIB_SOURCE_FILES})

add_executable(gpu_mst main.cpp)

target_link_libraries(gpu_mst ${OPENCL_LIBRARIES})
target_link_libraries(gpu_mst gpu_mst_lib)