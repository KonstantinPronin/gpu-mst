#include <parallel.h>

ClConfig *initOpenCl(const char *kernelPath) {
  auto *config = new (ClConfig);
  cl_mem memObjects[3] = {0, 0, 0};

  // Creates an OpenCL context on first available platform
  config->context = CreateContext();
  if (config->context == nullptr) {
    std::cerr << "Failed to create OpenCL context." << std::endl;
    return nullptr;
  }

  // Creates a command queue on the device available on the context
  config->queue = CreateCommandQueue(config->context, &config->device);
  if (config->queue == nullptr) {
    Cleanup(config->context, config->queue, config->program, config->kernel, memObjects);
    std::cerr << "Failed to create queue" << std::endl;
    return nullptr;
  }

  // Create OpenCL program from kernel source
  config->program = CreateProgram(config->context, config->device, kernelPath);
  if (config->program == nullptr) {
    Cleanup(config->context, config->queue, config->program, config->kernel, memObjects);
    std::cerr << "Failed to create openCl program" << std::endl;
    return nullptr;
  }

  // Create OpenCL Kernel
  config->kernel = clCreateKernel(config->program, "findMinimumEdges", nullptr);
  if (config->kernel == nullptr) {
    std::cerr << "Failed to create Kernel" << std::endl;
    Cleanup(config->context, config->queue, config->program, config->kernel, memObjects);
    return nullptr;
  }

  return config;
}

void runKernel(ClConfig *config, Edge *input, Edge *output, int componentNum, int edgesNum) {
  cl_int errNum;
  cl_mem memObjects[3] = {0, 0, 0};

  if (!CreateMemObjects(config->context, memObjects, input, edgesNum, componentNum)) {
    Cleanup(config->context, config->queue, config->program, config->kernel, memObjects);
    exit(1);
  }

  /* Set the Kernel arguments */
  errNum = clSetKernelArg(config->kernel, 0, sizeof(cl_mem), &memObjects[0]);
  errNum |= clSetKernelArg(config->kernel, 1, sizeof(cl_mem), &memObjects[1]);
  errNum |= clSetKernelArg(config->kernel, 2, sizeof(cl_mem), &memObjects[2]);
  if (errNum != CL_SUCCESS) {
    std::cerr << "Error setting Kernel arguments." << std::endl;
    Cleanup(config->context, config->queue, config->program, config->kernel, memObjects);
    exit(1);
  }

  size_t globalWorkSize[1] = {(size_t) componentNum};
  size_t localWorkSize[1] = {1};
  cl_event event;

  errNum = clEnqueueNDRangeKernel(config->queue, config->kernel, 1, NULL,
                                  globalWorkSize, localWorkSize,
                                  0, NULL, &event);
  if (errNum != CL_SUCCESS) {
    std::cerr << "Error queuing Kernel for execution." << std::endl;
    Cleanup(config->context, config->queue, config->program, config->kernel, memObjects);
    exit(1);
  }

//  errNum = clWaitForEvents(1, &event);
  errNum = clFinish(config->queue);

  if (errNum != CL_SUCCESS) {
    std::cerr << "Error waiting for execution." << std::endl;
    Cleanup(config->context, config->queue, config->program, config->kernel, memObjects);
    exit(1);
  }


  errNum = clEnqueueReadBuffer(config->queue, memObjects[1], CL_TRUE,
                               0, componentNum * sizeof(Edge), output,
                               0, NULL, NULL);
  if (errNum != CL_SUCCESS) {
    std::cerr << "Error reading result buffer." << std::endl;
    Cleanup(config->context, config->queue, config->program, config->kernel, memObjects);
    exit(1);
  }

  for (auto & memObject : memObjects) {
    if (memObject != 0)
      clReleaseMemObject(memObject);
  }
}

std::vector<Edge> parallelBoruvka(Graph *g, ClConfig *clConfig) {
  std::vector<Edge> result = std::vector<Edge>();
  int edgesNum = g->E;
  int componentsNum = g->V;

  Edge *edges = new Edge[edgesNum];
  for (int i = 0; i < edgesNum; ++i) {
    edges[i].vertex1 = g->edges[i].vertex1;
    edges[i].vertex2 = g->edges[i].vertex2;
    edges[i].component1 = g->edges[i].vertex1;
    edges[i].component2 = g->edges[i].vertex2;
    edges[i].weight = g->edges[i].weight;
  }

  while (result.size() < g->V - 1 && edgesNum > 0) {
    auto *minEdges = new Edge[componentsNum];

    runKernel(clConfig, edges, minEdges, componentsNum, edgesNum);

    for (int i = 0; i < componentsNum; ++i) {
      if (minEdges[i].included) {
        continue;
      }

      int dest = minEdges[i].component2;
      if (minEdges[dest].component2 == i) {
        minEdges[dest].included = true;
      }

      result.push_back(minEdges[i]);
    }

    std::vector<Edge> newEdges;
    componentsNum = createComponents(result, edges, edgesNum, newEdges);
    edgesNum = newEdges.size();

    delete[] edges;

    edges = new Edge[edgesNum];
    for (int i = 0; i < edgesNum; ++i) {
      edges[i] = newEdges[i];
    }

    delete[] minEdges;
  }

  delete[] edges;
  return result;
}