/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clProgram.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/06 03:02:53 by nathan            #+#    #+#             */
/*   Updated: 2022/06/24 23:54:26 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CLPROGRAM_CLASS_H
# define CLPROGRAM_CLASS_H

#include "ParticleSystem.h"
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <GL/glx.h>
#include <string>
#include <map>

#define IS_CL_VERBOSE true

class clProgram {
public:
	static void initialize();
	static void clCallBack(const char* errinfo, const void* private_info, size_t cb, void* user_data);
	static cl_program& createProgram(std::string sourceFile);
	static void clear();
	static cl_context getContext(){return context;};
	static cl_program& getProgram(std::string name);
	static void checkError(std::string functionName, cl_int errCode = retVal);
	static cl_command_queue getQueue(){return queue;}
	static size_t getMaxGroupSize() {return max_group_size;}
private:
	static std::map<std::string, cl_program> programs;
	static cl_uint ret_num_platforms;
	static cl_device_id device_id;
	static cl_int retVal;
	static cl_context context;
	static cl_command_queue queue;

	static size_t max_group_size;
};

#endif
