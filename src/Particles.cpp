/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Particles.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/06 03:00:32 by nathan            #+#    #+#             */
/*   Updated: 2020/11/07 18:41:31 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Particles.hpp"

#define FOV 60.0f
#define NEAR 0.1f
#define FAR 1000.0f

bool Particles::initialized = false;
GLuint Particles::VAO = 0;
GLuint Particles::VBO = 0;
cl_mem Particles::clBuffer = NULL;
Shader* Particles::shader = nullptr;
Camera Particles::camera = {};
Matrix Particles::projMat = Matrix::createProjMatrix(FOV, SCREEN_WIDTH / SCREEN_HEIGHT, NEAR, FAR);

void Particles::initialize()
{
	if (!initialized)
	{
		clProgram::initialize();
		initialized = true;
		initializeBuffers();
		initializeOpenGL();
		initializeParticles();
	}
}

void Particles::initializeBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * NB_PARTICLES, NULL, GL_DYNAMIC_DRAW);//TODO test draw / read / copy
	//TODO try with half type instead of float

    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	cl_int errCode;
	clBuffer = clCreateFromGLBuffer(clProgram::getContext(), CL_MEM_READ_WRITE, VBO, &errCode);
	clProgram::checkError("clCreateFromGLBuffer", errCode);
}

void Particles::initializeOpenGL()
{
	shader = new Shader("particles.vert", "particles.frag");
	glPointSize(PARTICLE_SIZE);
	shader->use();
}

void Particles::initializeParticles()
{

	cl_int errCode;
	cl_program ini = clProgram::createProgram("initializeParticles.cl");
    cl_kernel kernel = clCreateKernel(ini, "initializeParticles", &errCode);
	clProgram::checkError("clCreateKernel", errCode);

	cl_command_queue queue = clProgram::getQueue();
	glFinish();// make sure opengl is done when acquiring buffer
	clEnqueueAcquireGLObjects(queue, 1, &clBuffer, 0, 0, NULL);

    cl_mem seedBuff = clCreateBuffer(clProgram::getContext(), CL_MEM_READ_WRITE, 
             sizeof(int), NULL, &errCode);
	clProgram::checkError("clCreateBuffer", errCode);
	size_t seed = SEED;
    errCode = clEnqueueWriteBuffer(queue, seedBuff, CL_TRUE, 0,
            sizeof(int), &seed, 0, NULL, NULL);
	clProgram::checkError("clEnqueueWriteBuffer", errCode);

    errCode = clSetKernelArg(kernel, 0, sizeof(clBuffer), &clBuffer);
	clProgram::checkError("clSetKernelArg", errCode);
    errCode = clSetKernelArg(kernel, 1, sizeof(seedBuff), &seedBuff);

	size_t globalWorkSize = NB_PARTICLES;
	size_t localWorkSize = 2; //TODO change that ?
	size_t offset = 0;
	//TODO 
	//1 - initialize small amount of particle with + only number
	//2 - change x y z with random numbers for other particles
	//3 - reverse x y and/or z to fill particles in 7 other square of space
    errCode = clEnqueueNDRangeKernel(queue, kernel, 1, &offset, 
            &globalWorkSize, &localWorkSize, 0, NULL, NULL);
	clProgram::checkError("clEnqueueNDRangeKernel", errCode);

	errCode = clEnqueueReleaseGLObjects(queue, 1, &clBuffer, 0, 0, NULL);
	clProgram::checkError("clEnqueueReleaseGLObjects", errCode);

	errCode = clFinish(queue); // need to make sure buffer is released for openGL
	clProgram::checkError("clFinish", errCode);
    errCode = clReleaseMemObject(seedBuff);
	clProgram::checkError("clReleaseMemObject", errCode);
    errCode = clReleaseKernel(kernel);
	clProgram::checkError("clReleaseKernel", errCode);
    errCode = clReleaseProgram(ini);
	clProgram::checkError("clReleaseProgram", errCode);
}

void Particles::callCLFunc(cl_int errCode, std::string funcCall, std::string line)
{
	clProgram::checkError(line + funcCall, errCode);
}

void Particles::draw()
{
	std::array<float, 4> color = PARTICLE_COLOR;
	Matrix precalcMat = projMat * camera.getMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "precalcMat"), 1, GL_TRUE, precalcMat.exportForGL());
    glUniform4fv(glGetUniformLocation(shader->getID(), "myColor"), 1, &color.front());
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, NB_PARTICLES);
}

void Particles::clear()
{
	glDeleteBuffers(1, &VBO);
	VBO = 0;
	glDeleteVertexArrays(1, &VAO);
	VAO = 0;
	delete shader;
	shader = nullptr;
	clProgram::clear();
}
