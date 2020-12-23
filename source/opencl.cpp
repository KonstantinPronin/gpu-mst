#include <opencl.h>

/*  Creates an OpenCL context on the available platform using
*  either a GPU or CPU depending on what is available
*/
cl_context CreateContext() {
  /* Some variable's declarations */
  cl_int errNum;
  cl_uint numPlatforms;
  cl_platform_id firstPlatformId;
  cl_context context = NULL;

  /*  Selects an (available) OpenCL platform to run on */
  errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
  if (errNum != CL_SUCCESS || numPlatforms <= ZERO) {
    std::cerr << "Failed to find any OpenCL platforms." << std::endl;
    return NULL;
  }

  /* Sets context properties */
  cl_context_properties contextProperties[] = {
      CL_CONTEXT_PLATFORM,
      (cl_context_properties)firstPlatformId,
      0
  };

  /* Creates an OpenCL context on the platform */
  context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU,
                                    NULL, NULL, &errNum);
  if (errNum != CL_SUCCESS) {
    std::cout << "Could not create GPU context, trying CPU..." << std::endl;
    context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU,
                                      NULL, NULL, &errNum);
    if (errNum != CL_SUCCESS) {
      std::cerr << "Failed to create an OpenCL GPU or CPU context." << std::endl;
      return NULL;
    }
  }

  return context;
}

/* Creates a command queue on the device available on the context */
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device) {
  cl_int errNum;
  cl_device_id *devices;
  cl_command_queue commandQueue = NULL;
  size_t deviceBufferSize = -1;

  /* Gets the size of the devices buffer */
  errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, ZERO, NULL, &deviceBufferSize);
  if (errNum != CL_SUCCESS) {
    std::cerr << "Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)";
    return NULL;
  }

  if (deviceBufferSize <= ZERO) {
    std::cerr << "No devices available.";
    return NULL;
  }

  /* Allocates memory for the devices buffer */
  devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
  errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
  if (errNum != CL_SUCCESS) {
    delete [] devices;
    std::cerr << "Failed to get device IDs";
    return NULL;
  }

  /* Sets command queue properties */
  cl_command_queue_properties cmdQProperties = {
      CL_QUEUE_PROFILING_ENABLE
  };

  /* Chooses the first available device */
  commandQueue = clCreateCommandQueue(context, devices[0], cmdQProperties, NULL);
  if (commandQueue == NULL) {
    delete [] devices;
    std::cerr << "Failed to create commandQueue for device 0";
    return NULL;
  }

  *device = devices[0];
  delete [] devices;
  return commandQueue;
}

/* Create an OpenCL program from the Kernel source file */
cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName) {
  cl_int errNum;
  cl_program program;

  std::ifstream kernelFile(fileName, std::ios::in);
  if (!kernelFile.is_open()) {
    std::cerr << "Failed to open file for reading: " << fileName << std::endl;
    return NULL;
  }

  std::ostringstream oss;
  oss << kernelFile.rdbuf();

  std::string srcStdStr = oss.str();
  const char *srcStr = srcStdStr.c_str();

  program = clCreateProgramWithSource(context, 1, (const char**)&srcStr, NULL, NULL);
  if (program == NULL) {
    std::cerr << "Failed to create CL program from source." << std::endl;
    return NULL;
  }

  errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (errNum != CL_SUCCESS) {
    /* Gets program's errors */
    char buildLog[16384];
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                          sizeof(buildLog), buildLog, NULL);

    std::cerr << "Error in Kernel: " << std::endl;
    std::cerr << buildLog;
    clReleaseProgram(program);
    return NULL;
  }

  return program;
}

/* Create memory objects to be used as arguments to the Kernel */
bool CreateMemObjects(cl_context context, cl_mem memObjects[3], Edge* edges, size_t edgesNumber, size_t vertexNumber) {
  memObjects[0] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(Edge) * edgesNumber, edges, NULL);
  memObjects[1] = clCreateBuffer(context, CL_MEM_READ_WRITE,
                                 sizeof(Edge) * vertexNumber, NULL, NULL);
  memObjects[2] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(int) * 1, &edgesNumber, NULL);

  if (memObjects[0] == NULL || memObjects[1] == NULL || memObjects[2] == NULL) {
    std::cerr << "Error creating memory objects." << std::endl;
    return false;
  }

  return true;
}

/* Cleans up all (created) OpenCL resources */
void Cleanup(cl_context context, cl_command_queue commandQueue,
             cl_program program, cl_kernel kernel, cl_mem memObjects[3]) {
  for (int i = 0; i < 3; i++) {
    if (memObjects[i] != 0)
      clReleaseMemObject(memObjects[i]);
  }
  if (commandQueue != 0)
    clReleaseCommandQueue(commandQueue);

  if (kernel != 0)
    clReleaseKernel(kernel);

  if (program != 0)
    clReleaseProgram(program);

  if (context != 0)
    clReleaseContext(context);
}