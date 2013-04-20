/******************************************************************************
* Copyright (C) 2011 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)

double OCLKernels::multTransOCL(double* ptrSource, double* ptrData, double* ptrLevel, double* ptrIndex, double* ptrGlobalResult, size_t sourceSize, size_t storageSize, size_t dims, size_t gpu_partition) {
  double time = 0.0;

  if (isFirstTimeMultTransDP) {
    std::stringstream stream_program_src;

    stream_program_src << "#pragma OPENCL EXTENSION cl_khr_fp64 : enable" << std::endl << std::endl;
    stream_program_src << "__kernel void multTransOCL(__global double* ptrSource," << std::endl;
    stream_program_src << "						__global double* ptrData," << std::endl;
    stream_program_src << "						__global double* ptrLevel," << std::endl;
    stream_program_src << "						__global double* ptrIndex," << std::endl;
    stream_program_src << "						__global double* ptrResult," << std::endl;
    stream_program_src << "						uint sourceSize," << std::endl;
    stream_program_src << "						uint offset)" << std::endl;
    stream_program_src << "{" << std::endl;
    stream_program_src << "	int globalIdx = get_global_id(0);" << std::endl;
    stream_program_src << "	int localIdx = get_local_id(0);" << std::endl;
    stream_program_src << "	globalIdx = globalIdx + offset;" << std::endl;
    stream_program_src << std::endl;
    stream_program_src << "	double eval, index_calc, abs, last, localSupport, curSupport;" << std::endl << std::endl;
    stream_program_src << "	double myResult = 0.0f;" << std::endl << std::endl;
#ifdef USEOCL_LOCAL_MEMORY
    stream_program_src << "	__local double locData[" << dims* OCL_SGPP_LOCAL_WORKGROUP_SIZE << "];" << std::endl;
    stream_program_src << "	__local double locSource[" << OCL_SGPP_LOCAL_WORKGROUP_SIZE << "];" << std::endl << std::endl;
#endif

    for (size_t d = 0; d < dims; d++) {
      stream_program_src << "	double level_" << d << " = ptrLevel[(globalIdx*" << dims << ")+" << d << "];" << std::endl;
      stream_program_src << "	double index_" << d << " = ptrIndex[(globalIdx*" << dims << ")+" << d << "];" << std::endl;
    }

    stream_program_src << std::endl;
    stream_program_src << "	// Iterate over all grid points" << std::endl;
#ifdef USEOCL_LOCAL_MEMORY
    stream_program_src << "	for(int i = 0; i < sourceSize; i+=" << OCL_SGPP_LOCAL_WORKGROUP_SIZE << ")" << std::endl;
    stream_program_src << "	{" << std::endl;

    for (size_t d = 0; d < dims; d++) {
      stream_program_src << "		locData[(localIdx*" << dims << ")+" << d << "] = ptrData[((i+localIdx)*" << dims << ")+" << d << "];" << std::endl;
    }

    stream_program_src << "		locSource[localIdx] = ptrSource[i+localIdx];" << std::endl;
    stream_program_src << "		barrier(CLK_LOCAL_MEM_FENCE);" << std::endl << std::endl;
    stream_program_src << "		for(int k = 0; k < " << OCL_SGPP_LOCAL_WORKGROUP_SIZE << "; k++)" << std::endl;
    stream_program_src << "		{" << std::endl;

    stream_program_src << "			curSupport = locSource[k];" << std::endl << std::endl;
#else
    stream_program_src << "         for(int k = 0; k < sourceSize; k++)" << std::endl;
    stream_program_src << "         {" << std::endl;
    stream_program_src << "                 curSupport = ptrSource[k];" << std::endl << std::endl;

#endif

    for (size_t d = 0; d < dims; d++) {
#ifdef USEOCL_LOCAL_MEMORY
      stream_program_src << "			eval = ((level_" << d << ") * (locData[(k*" << dims << ")+" << d << "]));" << std::endl;
#else
      stream_program_src << "			eval = ((level_" << d << ") * (ptrData[(k*" << dims << ")+" << d << "]));" << std::endl;
#endif
      stream_program_src << "			index_calc = eval - (index_" << d << ");" << std::endl;
      stream_program_src << "			abs = fabs(index_calc);" << std::endl;
      stream_program_src << "			last = 1.0 - abs;" << std::endl;
      stream_program_src << "			localSupport = fmax(last, 0.0);" << std::endl;
      stream_program_src << "			curSupport *= localSupport;" << std::endl;
    }

    stream_program_src << std::endl << "		myResult += curSupport;" << std::endl;
    stream_program_src << "		}" << std::endl << std::endl;
#ifdef USEOCL_LOCAL_MEMORY
    stream_program_src << "		barrier(CLK_LOCAL_MEM_FENCE);" << std::endl;
    stream_program_src << "	}" << std::endl;
#endif
    stream_program_src << "	ptrResult[globalIdx] = myResult;" << std::endl;
    stream_program_src << "}" << std::endl;

    std::string program_src = stream_program_src.str();

    //std::cout << program_src << std::endl;

    // setting the program
    const char* kernel_src = program_src.c_str();
    program_multTransDP = clCreateProgramWithSource(context, 1, &kernel_src, NULL, &err);

    if (err != CL_SUCCESS) {
      std::cout << "OCL Error: Failed to create program! Error Code: " << err << std::endl;
      return 0.0;
    }

    // compiling the program
#ifndef NO_OCL_OPTS
    err = clBuildProgram(program_multTransDP, 0, NULL, "-cl-finite-math-only -cl-fast-relaxed-math", NULL, NULL);
#else
    err = clBuildProgram(program_multTransDP, 0, NULL, "-cl-opt-disable", NULL, NULL);
#endif

    if (err != CL_SUCCESS) {
      std::cout << "OCL Error: OpenCL Build Error. Error Code: " << err << std::endl;

      size_t len;
      char buffer[2048];

      // get the build log
      clGetProgramBuildInfo(program_multTransDP, device_ids[0], CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);

      std::cout << "--- Build Log ---" << std::endl << buffer << std::endl;
      return 0.0;
    }

    // creating the kernel
    for (size_t i = 0; i < num_devices; i++) {
      kernel_multTransDP[i] = clCreateKernel(program_multTransDP, "multTransOCL", &err);

      if (err != CL_SUCCESS) {
        std::cout << "OCL Error: Failed to create kernel! Error Code: " << err << std::endl;
        return 0.0;
      }
    }
  }

  if (isFirstTimeMultDP && isFirstTimeMultTransDP) {
    for (size_t i = 0; i < num_devices; i++) {
      clLevelDP[i] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * dims * storageSize, ptrLevel, NULL);
      clIndexDP[i] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * dims * storageSize, ptrIndex, NULL);
    }
  }

  if (isVeryFirstTimeDP) {
    for (size_t i = 0; i < num_devices; i++) {
      clDataDP[i] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * dims * sourceSize, ptrData, NULL);
    }

    isVeryFirstTimeDP = false;
  }

  cl_mem clSource[MAX_OCL_DEVICE_COUNT], clResult[MAX_OCL_DEVICE_COUNT];

  for (size_t i = 0; i < num_devices; i++) {
    clSource[i] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * sourceSize, ptrSource, NULL);
    clResult[i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(double) * gpu_partition, NULL, NULL);
  }

  cl_uint clSourceSize = (cl_uint)sourceSize;
  cl_uint clOffsets[MAX_OCL_DEVICE_COUNT];

  // determine best fit
  size_t elements_per_gpu = (((gpu_partition / num_devices) / OCL_SGPP_LOCAL_WORKGROUP_SIZE) + 1) * OCL_SGPP_LOCAL_WORKGROUP_SIZE;
  size_t local = OCL_SGPP_LOCAL_WORKGROUP_SIZE;
  size_t offset = 0;
  size_t global[MAX_OCL_DEVICE_COUNT];
  size_t active_devices = 0;
  size_t scheduled_elements = 0;

  for (size_t i = 0; i < num_devices; i++) {
    global[i] = elements_per_gpu;

    // check for padding
    if (scheduled_elements < gpu_partition) {
      global[i] = elements_per_gpu;

      if ((global[i] + scheduled_elements) > gpu_partition) {
        global[i] = gpu_partition - scheduled_elements;
      }
    } else {
      global[i] = 0;
    }

    scheduled_elements += global[i];

    if (global[i] != 0)
      active_devices++;
  }

  // set kernel arguments
  for (size_t i = 0; i < num_devices; i++) {
    clOffsets[i] = (cl_uint)offset;

    if (global[i] > 0) {
      if ( clSetKernelArg(kernel_multTransDP[i], 0, sizeof(cl_mem), &clSource[i]) ||
           clSetKernelArg(kernel_multTransDP[i], 1, sizeof(cl_mem), &clDataDP[i]) ||
           clSetKernelArg(kernel_multTransDP[i], 2, sizeof(cl_mem), &clLevelDP[i]) ||
           clSetKernelArg(kernel_multTransDP[i], 3, sizeof(cl_mem), &clIndexDP[i]) ||
           clSetKernelArg(kernel_multTransDP[i], 4, sizeof(cl_mem), &clResult[i]) ||
           clSetKernelArg(kernel_multTransDP[i], 5, sizeof(cl_uint), &clSourceSize) ||
           clSetKernelArg(kernel_multTransDP[i], 6, sizeof(cl_uint), &clOffsets[i]) != CL_SUCCESS) {
        std::cout << "OCL Error: Failed to create kernel Args for kernel " << i << "!" << std::endl;
        return 0.0;
      }
    }

    //    std::cout << "multTrans: global " << i << ": " << global[i] << " " << offset <<std::endl;
    offset += global[i];
  }

  cl_event clTimings[MAX_OCL_DEVICE_COUNT];
  cl_event GPUDone[MAX_OCL_DEVICE_COUNT];

  // enqueue kernels
  for (size_t i = 0; i < num_devices; i++) {
    if (global[i] > 0) {
      err = clEnqueueNDRangeKernel(command_queue[i], kernel_multTransDP[i], 1, NULL, &(global[i]), &local, 0, NULL, &(clTimings[i]));

      if (err != CL_SUCCESS) {
        std::cout << "OCL Error: Failed to enqueue kernel command! Error Code: " << err << std::endl;
        return 0.0;
      }
    }
  }

  // read data back
  offset = 0;

  for (size_t i = 0; i < num_devices; i++) {
    if (global[i] > 0) {
      err = clEnqueueReadBuffer(command_queue[i], clResult[i], CL_FALSE, sizeof(double) * offset, sizeof(double) * (global[i]), &(ptrGlobalResult[offset]), 0, NULL, &(GPUDone[i]));

      if (err != CL_SUCCESS) {
        std::cout << "OCL Error: Failed to enqueue read buffer command (mult)! Error Code: " << err << std::endl;
        return 0.0;
      }
    }

    offset += global[i];
  }

  // sync GPUs
  clWaitForEvents((cl_uint)active_devices, GPUDone);

  // determine kernel execution time
  for (size_t i = 0; i < num_devices; i++) {
    double tmpTime;
    cl_ulong startTime, endTime;
    startTime = endTime = 0;

    if (global[i] > 0) {
      err = clGetEventProfilingInfo(clTimings[i], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &startTime, NULL);

      if (err != CL_SUCCESS) {
        std::cout << "OCL Error: Failed to read start-time from command queue! Error Code: " << err << std::endl;
      }

      err = clGetEventProfilingInfo(clTimings[i], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &endTime, NULL);

      if (err != CL_SUCCESS) {
        std::cout << "OCL Error: Failed to read end-time from command queue! Error Code: " << err << std::endl;
      }
    }

    tmpTime = (double)(endTime - startTime);
    tmpTime *= 1e-9;

    if (tmpTime > time) {
      time = tmpTime;
    }
  }

  // clean up
  for (size_t i = 0; i < num_devices; i++) {
    clReleaseMemObject(clSource[i]);
    clReleaseMemObject(clResult[i]);

    if (global[i] > 0) {
      clReleaseEvent(clTimings[i]);
      clReleaseEvent(GPUDone[i]);
    }
  }

  isFirstTimeMultTransDP = false;

  return time;
}

double OCLKernels::multOCL(double* ptrAlpha, double* ptrData, double* ptrLevel, double* ptrIndex, double* ptrResult, size_t result_size, size_t storageSize, size_t dims, size_t gpu_partition) {
  double time = 0.0;

  if (isFirstTimeMultDP) {
    std::stringstream stream_program_src;

    stream_program_src << "#pragma OPENCL EXTENSION cl_khr_fp64 : enable" << std::endl << std::endl;
    stream_program_src << "__kernel void multOCL(__global double* ptrAlpha," << std::endl;
    stream_program_src << "						__global double* ptrData," << std::endl;
    stream_program_src << "						__global double* ptrLevel," << std::endl;
    stream_program_src << "						__global double* ptrIndex," << std::endl;
    stream_program_src << "						__global double* ptrResult," << std::endl;
    stream_program_src << "						uint fastStorageSize," << std::endl;
    stream_program_src << "						uint storageSize," << std::endl;
    stream_program_src << "						uint offset)" << std::endl;
    stream_program_src << "{" << std::endl;
    stream_program_src << "	int globalIdx = get_global_id(0);" << std::endl;
    stream_program_src << "	int localIdx = get_local_id(0);" << std::endl;
    stream_program_src << "	globalIdx = globalIdx + offset;" << std::endl;
    stream_program_src << std::endl;
#ifdef USEOCL_LOCAL_MEMORY
    stream_program_src << "	__local double locLevel[" << dims* OCL_SGPP_LOCAL_WORKGROUP_SIZE << "];" << std::endl;
    stream_program_src << "	__local double locIndex[" << dims* OCL_SGPP_LOCAL_WORKGROUP_SIZE << "];" << std::endl;
    stream_program_src << "	__local double locAlpha[" << OCL_SGPP_LOCAL_WORKGROUP_SIZE << "];" << std::endl;
    stream_program_src << std::endl;
#endif
    stream_program_src << "	double eval, index_calc, abs, last, localSupport, curSupport;" << std::endl << std::endl;
    stream_program_src << "	double myResult = 0.0;" << std::endl << std::endl;
    stream_program_src << "	// Create registers for the data" << std::endl;

    for (size_t d = 0; d < dims; d++) {
      stream_program_src << "	double data_" << d << " = ptrData[(globalIdx*" << dims << ")+" << d << "];" << std::endl;
    }

    stream_program_src << std::endl;
#ifdef USEOCL_LOCAL_MEMORY
    stream_program_src << "	// Iterate over all grid points (fast ones, with cache)" << std::endl;
    stream_program_src << "	for(int j = 0; j < fastStorageSize; j+=" << OCL_SGPP_LOCAL_WORKGROUP_SIZE << ")" << std::endl;
    stream_program_src << "	{" << std::endl;

    for (size_t d = 0; d < dims; d++) {
      stream_program_src << "		locLevel[(localIdx*" << dims << ")+" << d << "] = ptrLevel[((j+localIdx)*" << dims << ")+" << d << "];" << std::endl;
      stream_program_src << "		locIndex[(localIdx*" << dims << ")+" << d << "] = ptrIndex[((j+localIdx)*" << dims << ")+" << d << "];" << std::endl;
    }

    stream_program_src << "		locAlpha[localIdx] = ptrAlpha[j+localIdx];" << std::endl;
    stream_program_src << "		barrier(CLK_LOCAL_MEM_FENCE);" << std::endl;
    stream_program_src << std::endl;
    stream_program_src << "		for(int k = 0; k < " << OCL_SGPP_LOCAL_WORKGROUP_SIZE << "; k++)" << std::endl;
    stream_program_src << "		{" << std::endl;
    stream_program_src << "			curSupport = locAlpha[k];" << std::endl << std::endl;

    for (size_t d = 0; d < dims; d++) {
      stream_program_src << "			eval = ((locLevel[(k*" << dims << ")+" << d << "]) * (data_" << d << "));" << std::endl;
      stream_program_src << "			index_calc = eval - (locIndex[(k*" << dims << ")+" << d << "]);" << std::endl;
      stream_program_src << "			abs = fabs(index_calc);" << std::endl;
      stream_program_src << "			last = 1.0 - abs;" << std::endl;
      stream_program_src << "			localSupport = fmax(last, 0.0);" << std::endl;
      stream_program_src << "			curSupport *= localSupport;" << std::endl << std::endl;
    }

    stream_program_src << "			myResult += curSupport;" << std::endl;
    stream_program_src << "		}" << std::endl;
    stream_program_src << std::endl;
    stream_program_src << "		barrier(CLK_LOCAL_MEM_FENCE);" << std::endl;
    stream_program_src << "	}" << std::endl;
    stream_program_src << std::endl;
    stream_program_src << "	// Iterate over all grid points (slow ones, without cache)" << std::endl;
    stream_program_src << "	for(int m = fastStorageSize; m < storageSize; m++)" << std::endl;
    stream_program_src << "	{" << std::endl;
    stream_program_src << "		curSupport = ptrAlpha[m];" << std::endl << std::endl;

    for (size_t d = 0; d < dims; d++) {
      stream_program_src << "		eval = ((ptrLevel[(m*" << dims << ")+" << d << "]) * (data_" << d << "));" << std::endl;
      stream_program_src << "		index_calc = eval - (ptrIndex[(m*" << dims << ")+" << d << "]);" << std::endl;
      stream_program_src << "		abs = fabs(index_calc);" << std::endl;
      stream_program_src << "		last = 1.0 - abs;" << std::endl;
      stream_program_src << "		localSupport = fmax(last, 0.0);" << std::endl;
      stream_program_src << "		curSupport *= localSupport;" << std::endl << std::endl;
    }

    stream_program_src << "		myResult += curSupport;" << std::endl;
    stream_program_src << "	}" << std::endl;
#else
    stream_program_src << "	// Iterate over all grid points (without cache)" << std::endl;
    stream_program_src << "	for(int m = 0; m < storageSize; m++)" << std::endl;
    stream_program_src << "	{" << std::endl;
    stream_program_src << "		curSupport = ptrAlpha[m];" << std::endl << std::endl;

    for (size_t d = 0; d < dims; d++) {
      stream_program_src << "		eval = ((ptrLevel[(m*" << dims << ")+" << d << "]) * (data_" << d << "));" << std::endl;
      stream_program_src << "		index_calc = eval - (ptrIndex[(m*" << dims << ")+" << d << "]);" << std::endl;
      stream_program_src << "		abs = fabs(index_calc);" << std::endl;
      stream_program_src << "		last = 1.0 - abs;" << std::endl;
      stream_program_src << "		localSupport = fmax(last, 0.0);" << std::endl;
      stream_program_src << "		curSupport *= localSupport;" << std::endl << std::endl;
    }

    stream_program_src << "		myResult += curSupport;" << std::endl;
    stream_program_src << "	}" << std::endl;

#endif
    stream_program_src << std::endl;
    stream_program_src << "	ptrResult[globalIdx] = myResult;" << std::endl;
    stream_program_src << "}" << std::endl;

    std::string program_src = stream_program_src.str();

    //std::cout << program_src << std::endl;

    // setting the program
    const char* kernel_src = program_src.c_str();
    program_multDP = clCreateProgramWithSource(context, 1, &kernel_src, NULL, &err);

    if (err != CL_SUCCESS) {
      std::cout << "OCL Error: Failed to create program! Error Code: " << err << std::endl;
      return 0.0;
    }

    // compiling the program
#ifndef NO_OCL_OPTS
    err = clBuildProgram(program_multDP, 0, NULL, "-cl-finite-math-only -cl-fast-relaxed-math", NULL, NULL);
#else
    err = clBuildProgram(program_multDP, 0, NULL, "-cl-opt-disable", NULL, NULL);
#endif

    if (err != CL_SUCCESS) {
      std::cout << "OCL Error: OpenCL Build Error. Error Code: " << err << std::endl;

      size_t len;
      char buffer[2048];

      // get the build log
      clGetProgramBuildInfo(program_multDP, device_ids[0], CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);

      std::cout << "--- Build Log ---" << std::endl << buffer << std::endl;
      return 0.0;
    }

    // creating the kernels
    for (size_t i = 0; i < num_devices; i++) {
      kernel_multDP[i] = clCreateKernel(program_multDP, "multOCL", &err);

      if (err != CL_SUCCESS) {
        std::cout << "OCL Error: Failed to create kernel! Error Code: " << err << std::endl;
        return 0.0;
      }
    }
  }

  if (isFirstTimeMultDP && isFirstTimeMultTransDP) {
    for (size_t i = 0; i < num_devices; i++) {
      clLevelDP[i] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * dims * storageSize, ptrLevel, NULL);
      clIndexDP[i] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * dims * storageSize, ptrIndex, NULL);
    }
  }

  if (isVeryFirstTimeDP) {
    for (size_t i = 0; i < num_devices; i++) {
      clDataDP[i] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * dims * result_size, ptrData, NULL);
    }

    isVeryFirstTimeDP = false;
  }

  cl_mem clAlpha[MAX_OCL_DEVICE_COUNT], clResult[MAX_OCL_DEVICE_COUNT];

  for (size_t i = 0; i < num_devices; i++) {
    clAlpha[i] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * storageSize, ptrAlpha, NULL);
    clResult[i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(double) * gpu_partition, NULL, NULL);
  }

  size_t elements_per_gpu = (((gpu_partition / num_devices) / OCL_SGPP_LOCAL_WORKGROUP_SIZE) + 1) * OCL_SGPP_LOCAL_WORKGROUP_SIZE;
  size_t local = OCL_SGPP_LOCAL_WORKGROUP_SIZE;
  size_t offset = 0;
  size_t global[MAX_OCL_DEVICE_COUNT];
  size_t active_devices = 0;
  size_t scheduled_elements = 0;

  for (size_t i = 0; i < num_devices; i++) {
    global[i] = elements_per_gpu;

    // check for padding
    if (scheduled_elements < gpu_partition) {
      global[i] = elements_per_gpu;

      if ((global[i] + scheduled_elements) > gpu_partition) {
        global[i] = gpu_partition - scheduled_elements;
      }
    } else {
      global[i] = 0;
    }

    scheduled_elements += global[i];

    if (global[i] != 0)
      active_devices++;
  }

  size_t oclStorageSize = (storageSize / OCL_SGPP_LOCAL_WORKGROUP_SIZE) * OCL_SGPP_LOCAL_WORKGROUP_SIZE;

  cl_uint clFastStorageSize = (cl_uint)(oclStorageSize);
  cl_uint clStorageSize = (cl_uint)(storageSize);
  cl_uint clOffsets[MAX_OCL_DEVICE_COUNT];

  for (size_t i = 0; i < num_devices; i++) {
    clOffsets[i] = (cl_uint)offset;

    if (global[i] > 0) {
      // set kernel arguments
      if ( clSetKernelArg(kernel_multDP[i], 0, sizeof(cl_mem), &clAlpha[i]) ||
           clSetKernelArg(kernel_multDP[i], 1, sizeof(cl_mem), &clDataDP[i]) ||
           clSetKernelArg(kernel_multDP[i], 2, sizeof(cl_mem), &clLevelDP[i]) ||
           clSetKernelArg(kernel_multDP[i], 3, sizeof(cl_mem), &clIndexDP[i]) ||
           clSetKernelArg(kernel_multDP[i], 4, sizeof(cl_mem), &clResult[i]) ||
           clSetKernelArg(kernel_multDP[i], 5, sizeof(cl_uint), &clFastStorageSize) ||
           clSetKernelArg(kernel_multDP[i], 6, sizeof(cl_uint), &clStorageSize) ||
           clSetKernelArg(kernel_multDP[i], 7, sizeof(cl_uint), &clOffsets[i]) != CL_SUCCESS) {
        std::cout << "OCL Error: Failed to create kernel Args!" << std::endl;
        return 0.0;
      }
    }

    //    std::cout << "mult: global " << i << ": " << global[i] << " " << offset <<std::endl;
    offset += global[i];
  }

  cl_event clTimings[MAX_OCL_DEVICE_COUNT];
  cl_event GPUDone[MAX_OCL_DEVICE_COUNT];

  // enqueue kernel
  for (size_t i = 0; i < num_devices; i++) {
    if (global[i] > 0) {
      err = clEnqueueNDRangeKernel(command_queue[i], kernel_multDP[i], 1, NULL, &(global[i]), &local, 0, NULL, &(clTimings[i]));

      if (err != CL_SUCCESS) {
        std::cout << "OCL Error: Failed to enqueue kernel command! Error Code: " << err << std::endl;
        return 0.0;
      }
    }
  }

  // read data back
  offset = 0;

  for (size_t i = 0; i < num_devices; i++) {
    if (global[i] > 0) {
      err = clEnqueueReadBuffer(command_queue[i], clResult[i], CL_FALSE, sizeof(double) * offset, sizeof(double) * global[i], &(ptrResult[offset]), 0, NULL, &(GPUDone[i]));

      if (err != CL_SUCCESS) {
        std::cout << "OCL Error: Failed to enqueue read buffer command (multTrans)! Error Code: " << err << std::endl;
        return 0.0;
      }
    }

    offset += global[i];
  }

  // sync GPUs
  clWaitForEvents((cl_uint)active_devices, GPUDone);

  // determine kernel execution time
  for (size_t i = 0; i < num_devices; i++) {
    double tmpTime;
    cl_ulong startTime, endTime;
    startTime = endTime = 0;

    if (global[i] > 0) {
      err = clGetEventProfilingInfo(clTimings[i], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &startTime, NULL);

      if (err != CL_SUCCESS) {
        std::cout << "OCL Error: Failed to read start-time from command queue! Error Code: " << err << std::endl;
      }

      err = clGetEventProfilingInfo(clTimings[i], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &endTime, NULL);

      if (err != CL_SUCCESS) {
        std::cout << "OCL Error: Failed to read end-time from command queue! Error Code: " << err << std::endl;
      }
    }

    tmpTime = (double)(endTime - startTime);
    tmpTime *= 1e-9;

    if (tmpTime > time) {
      time = tmpTime;
    }
  }

  // clean up
  for (size_t i = 0; i < num_devices; i++) {
    clReleaseMemObject(clAlpha[i]);
    clReleaseMemObject(clResult[i]);

    if (global[i] > 0) {
      clReleaseEvent(clTimings[i]);
      clReleaseEvent(GPUDone[i]);
    }
  }

  isFirstTimeMultDP = false;

  return time;
}

