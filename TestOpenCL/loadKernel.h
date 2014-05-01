#ifndef _loadKerenl_h_
#define _loadKerenl_h_

#include <string>
#include "CL/cl.hpp"

bool loadKernelFile(cl::Context openCLContext, cl::Device openCLDevice, const std::string &path, std::string kernelName, cl::Kernel &kernel);
bool loadKernel(cl::Context openCLContext, cl::Device openCLDevice, const std::string &kernelSource, std::string kernelName, cl::Kernel &kernel);

#endif //_loadKerenl_h_