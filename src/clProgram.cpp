/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clProgram.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/06 03:02:51 by nathan            #+#    #+#             */
/*   Updated: 2022/07/15 15:07:27 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clProgram.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

#define KERNEL_SOURCE_DIR std::string("kernelFiles/")

cl_uint clProgram::ret_num_platforms = 0;
cl_device_id clProgram::device_id = NULL;   
cl_int clProgram::retVal = 0;
cl_context clProgram::context = NULL;
cl_command_queue clProgram::queue = NULL;
std::map<std::string, cl_program> clProgram::programs = {};
size_t clProgram::max_group_size = 0;

void clProgram::initialize()
{
    cl_platform_id platform_id = NULL;
	cl_uint ret_num_devices = 0;

	// Get platform information
    retVal = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);// get platform (cuda)

#if defined(__APPLE__)

    // OS X                                                                     
    CGLContextObj     kCGLContext     = CGLGetCurrentContext();
    CGLShareGroupObj  kCGLShareGroup  = CGLGetShareGroup(kCGLContext);

    cl_context_properties properties[] = {
      CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
      (cl_context_properties) kCGLShareGroup,
      0
    };

#else

    // Linux                                                                    
    cl_context_properties properties[] = {
      CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
      CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
      CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id,
      0
    };

#endif

	char *name = new char[512];
	if (IS_CL_VERBOSE)
	{
		clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 512, name, nullptr);
		std::cout << "Platform Name: " << std::string(name) << std::endl;
		std::cout << "Number of platforms: " << ret_num_platforms << std::endl;
		clGetPlatformInfo(platform_id, CL_PLATFORM_VERSION, 512, name, nullptr);
		std::cout << "Current platform supports OpenCL: " << name << std::endl;
	}
 
	// Get device information
    retVal = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1, 
            &device_id, &ret_num_devices);// get devices (gpu, cpu..)
	checkError("getDeviceID");
	clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &max_group_size, nullptr);
	std::cout << "MAX SIZE IS: " << max_group_size << std::endl;
	if (IS_CL_VERBOSE)
	{
		std::cout << "There are: " << ret_num_devices << " opencl devices found" << std::endl;
		cl_device_type type;
		clGetDeviceInfo(device_id, CL_DEVICE_TYPE, sizeof(cl_device_type), &type, nullptr);
		std::string deviceType = type == CL_DEVICE_TYPE_GPU ? "GPU" :
			type == CL_DEVICE_TYPE_CPU ? "CPU" : "FLEMME";
		std::cout << "Device type: " << deviceType << std::endl;
		clGetDeviceInfo(device_id, CL_DEVICE_NAME, 512, name, nullptr);
		std::cout << "Device name: " << std::string(name) << std::endl;
	}

    // Create an OpenCL context
    context = clCreateContext(properties, 1, &device_id, clCallBack, NULL, &retVal);
	checkError("clCreateContext");

	if (IS_CL_VERBOSE)
	{
		cl_uint i;
		clGetContextInfo(context, CL_CONTEXT_NUM_DEVICES, sizeof(cl_uint), &i, nullptr);
		std::cout << "There are: " << i << " opencl devices in context" << std::endl;
	}

    // Create a command queue
    queue = clCreateCommandQueueWithProperties(context, device_id, nullptr, &retVal); // 0 is default properties used
	// can also be set to CL_QUEUE_PROFILING_ENABLE to profile queue
	// see https://software.intel.com/content/www/us/en/develop/documentation/iocl-tec-opg/top/performance-debugging-with-intel-sdk-for-opencl-applications/profiling-operations-using-opencl-profiling-events.html
	checkError("clCreateCommandQueue");

	if (IS_CL_VERBOSE)
	{
		// nothing interesting to know about queue
	}

	delete [] name;
}

cl_program& clProgram::createProgram(std::string sourceFile)
{
	std::ifstream file;
	file.open(KERNEL_SOURCE_DIR + sourceFile);
	std::stringstream filestream;
	filestream << file.rdbuf();
	file.close();
	std::string fileAsString = filestream.str();

	const char *source_str = fileAsString.c_str();
	const size_t source_size = fileAsString.size();

	if (programs.find(sourceFile) != programs.end())
		return programs[sourceFile];
    cl_program program = clCreateProgramWithSource(context, 1, 
            (const char **)&source_str, (const size_t *)&source_size, &retVal);
// context, 1 = to number of files loaded, array of source code, array of length of the source code, retval
	
	checkError(std::string("clCreateProgramWithSource, with source: ") + sourceFile);
    retVal = clBuildProgram(program, 1, &device_id, NULL, nullptr, NULL); // might change for instant call and callback once built for performance?
	if (retVal != CL_SUCCESS)
	{
		size_t logLen;
		cl_int errCode;
		errCode = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &logLen);
		if (errCode != CL_SUCCESS)
			checkError("clGetProgramBuildInfo", errCode);
		char *log = new char[logLen];
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, logLen, log, NULL);
		std::cout << "Build log: " << log << std::endl;
		delete [] log;
		checkError(std::string("clBuildProgram, with source: ") + sourceFile);
	}
	programs[sourceFile] = program;
	return programs[sourceFile];
}

cl_program& clProgram::getProgram(std::string name)
{
	if (programs.find(name) == programs.end())
		return createProgram(name);
	return programs.at(name);
}

void clProgram::clear()
{
	for (auto& it: programs)
	{
		clReleaseProgram(programs.at(it.first));
	}
	programs.clear();
    retVal = clReleaseCommandQueue(queue);
	checkError("clReleaseCommandQueue");
    retVal = clReleaseContext(context);
	checkError("clReleaseContext");
	retVal = clReleaseDevice(device_id);
	checkError("clReleaseDevice");
}

void clProgram::clCallBack(const char* errinfo, const void* private_info, size_t cb, void* user_data)
{
	(void)private_info;
	(void)cb;
	(void)user_data;// is unused
	std::cout << std::string(errinfo) << std::endl;
}

void clProgram::checkError(std::string functionName, cl_int errCode)
{
	if (errCode != CL_SUCCESS)
	{
		std::cout << "Error: no CL_SUCCESS(" << (int)errCode << ") with: " << functionName << std::endl;
		exit(-1);
	}
}
