#ifndef GPU_MST_INCLUDE_PARALLEL_H_
#define GPU_MST_INCLUDE_PARALLEL_H_

#include <opencl.h>
#include <graph.h>

struct ClConfig {
  cl_context context;
  cl_command_queue queue;
  cl_program program;
  cl_device_id device;
  cl_kernel kernel;
};

ClConfig* initOpenCl(const char *kernelPath);
void runKernel(ClConfig *config, Edge *input, Edge *output, int componentNum, int edgesNum);
std::vector<Edge> parallelBoruvka(Graph *g, ClConfig *clConfig);

#endif //GPU_MST_INCLUDE_PARALLEL_H_
