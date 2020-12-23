#ifndef GPU_MST_INCLUDE_OPENCL_H_
#define GPU_MST_INCLUDE_OPENCL_H_

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/opencl.h>
#else
#include <CL/cl.hpp>
#endif
#include <iostream>
#include <fstream>
#include <sstream>
#include <graph.h>

#define ZERO 0

cl_context CreateContext();
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device);
cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName);
bool CreateMemObjects(cl_context context, cl_mem memObjects[3], Edge* edges, size_t edgesNumber, size_t vertexNumber);
void Cleanup(cl_context context, cl_command_queue commandQueue,
             cl_program program, cl_kernel kernel, cl_mem memObjects[3]);

#endif //GPU_MST_INCLUDE_OPENCL_H_
