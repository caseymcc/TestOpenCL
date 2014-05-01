#include "loadKernel.h"

#include <boost/format.hpp>

#include <string>
#include <vector>

#include "CL/cl.hpp"

bool loadKernelFile(cl::Context openCLContext, cl::Device openCLDevice, const std::string &path, std::string kernelName, cl::Kernel &kernel)
{
	FILE *file=fopen(path.c_str(), "rb");

	if(file == NULL)
		return 0;

	std::string kernelSource;
	char buffer[1024];
	size_t read;
	
	while((read=fread(buffer, sizeof(char), 1023, file)) != 0)
		kernelSource.append(buffer, read);

	fclose(file);

	return loadKernel(openCLContext, openCLDevice, kernelSource, kernelName, kernel);
}

bool loadKernel(cl::Context openCLContext, cl::Device openCLDevice, const std::string &kernelSource, std::string kernelName, cl::Kernel &kernel)
{
	cl::Program::Sources programSource(1, std::make_pair(kernelSource.data(), kernelSource.size()));
	cl::Program program=cl::Program(openCLContext, programSource);

	cl_int error=CL_SUCCESS;
	std::vector<cl::Device> programDevices;

	programDevices.push_back(openCLDevice);
	error=program.build(programDevices);
	if(error != CL_SUCCESS)
	{
		if(error == CL_BUILD_PROGRAM_FAILURE)
		{
			std::string str=program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(openCLDevice);
			assert(false);
			return false;
		}
	}
	kernel=cl::Kernel(program, kernelName.c_str(), &error);
	if(error != CL_SUCCESS)
	{
		assert(false);
		return false;
	}
	return true;
}

