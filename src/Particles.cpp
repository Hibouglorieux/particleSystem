/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Particles.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/06 03:00:32 by nathan            #+#    #+#             */
/*   Updated: 2020/12/23 15:07:25 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Particles.hpp"
#include <utility>

#define FOV 60.0f
#define NEAR 0.1f
#define FAR 1000.0f

bool Particles::initialized = false;
GLuint Particles::VAO = 0;
GLuint Particles::VBO = 0;
cl_mem Particles::clBuffer = NULL;
cl_mem Particles::sizePerParticleBuff = NULL;
cl_mem Particles::timeBuff = NULL;
cl_mem Particles::cursorPosBuff = NULL;
cl_program Particles::updateProgram = NULL;
cl_kernel Particles::updateKernel = NULL;
Shader* Particles::shader = nullptr;
Camera Particles::camera = {0, 0, 300};
Matrix Particles::projMat = Matrix::createProjMatrix(FOV, SCREEN_WIDTH / SCREEN_HEIGHT, NEAR, FAR);
float Particles::mouseX = 0.0f;
float Particles::mouseY = 0.0f;

void Particles::initialize()
{
	if (!initialized)
	{
		clProgram::initialize();
		initialized = true;
		initializeBuffers();
		initializeOpenGL();
		initializeOpenCL();
		initializeParticles();
	}
}

void Particles::initializeBuffers()
{
	/*
	 * openGL
	 */
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * SIZE_PER_PARTICLE * NB_PARTICLES, NULL, GL_DYNAMIC_DRAW);//TODO test draw / read / copy
	//TODO try with half type instead of float

    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SIZE_PER_PARTICLE * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, SIZE_PER_PARTICLE * sizeof(float), (void*)(sizeof(float) * 3));
	
	/*
	 * openCL
	 */
	cl_int errCode;
	clBuffer = clCreateFromGLBuffer(clProgram::getContext(), CL_MEM_READ_WRITE, VBO, &errCode);
	clProgram::checkError("clCreateFromGLBuffer", errCode);

	//initialize common reusable buffer
    sizePerParticleBuff = clCreateBuffer(clProgram::getContext(), CL_MEM_READ_ONLY, 
             sizeof(unsigned int), NULL, &errCode);
	clProgram::checkError("clCreateBuffer", errCode);
    timeBuff = clCreateBuffer(clProgram::getContext(), CL_MEM_READ_ONLY, 
             sizeof(float), NULL, &errCode);
	clProgram::checkError("clCreateBuffer", errCode);
    cursorPosBuff = clCreateBuffer(clProgram::getContext(), CL_MEM_READ_ONLY, 
             sizeof(float) * 3, NULL, &errCode);
	clProgram::checkError("clCreateBuffer", errCode);

	unsigned int sizePerParticle = SIZE_PER_PARTICLE;
    callCL(clEnqueueWriteBuffer(clProgram::getQueue(), sizePerParticleBuff, CL_TRUE, 0,
            sizeof(unsigned int), &sizePerParticle, 0, NULL, NULL));
}

void Particles::initializeOpenGL()
{
	shader = new Shader("particles.vert", "particles.frag");
	glPointSize(PARTICLE_SIZE);
	shader->use();
}

void Particles::initializeOpenCL()
{
	cl_int errCode;
	updateProgram = clProgram::createProgram("updateParticles.cl");
    updateKernel = clCreateKernel(updateProgram, "updateParticles", &errCode);
	clProgram::checkError("clCreateKernel", errCode);
	callCL(clSetKernelArg(updateKernel, 0, sizeof(clBuffer), &clBuffer));
	callCL(clSetKernelArg(updateKernel, 1, sizeof(sizePerParticleBuff), &sizePerParticleBuff));
}

void Particles::initializeParticles()
{

	cl_int errCode;
	cl_program ini = clProgram::createProgram("initializeAsCircle.cl");
    cl_kernel kernel = clCreateKernel(ini, "initializeAsCircle", &errCode);
	clProgram::checkError("clCreateKernel", errCode);

	cl_command_queue queue = clProgram::getQueue();
	glFinish();// make sure opengl is done when acquiring buffer
	clEnqueueAcquireGLObjects(queue, 1, &clBuffer, 0, 0, NULL);

    cl_mem seedBuff = clCreateBuffer(clProgram::getContext(), CL_MEM_READ_WRITE, 
             sizeof(unsigned long), NULL, &errCode);
	clProgram::checkError("clCreateBuffer", errCode);
    cl_mem nbParticlesBuff = clCreateBuffer(clProgram::getContext(), CL_MEM_READ_ONLY, 
             sizeof(unsigned int), NULL, &errCode);
	clProgram::checkError("clCreateBuffer", errCode);

	unsigned long seed = SEED;
    callCL(clEnqueueWriteBuffer(queue, seedBuff, CL_TRUE, 0,
            sizeof(unsigned long), &seed, 0, NULL, NULL));
	unsigned int nbParticles = NB_PARTICLES;
    callCL(clEnqueueWriteBuffer(queue, nbParticlesBuff, CL_TRUE, 0,
            sizeof(unsigned int), &nbParticles, 0, NULL, NULL));

    callCL(clSetKernelArg(kernel, 0, sizeof(clBuffer), &clBuffer));
    callCL(clSetKernelArg(kernel, 1, sizeof(seedBuff), &seedBuff));
	callCL(clSetKernelArg(kernel, 2, sizeof(nbParticlesBuff), &nbParticlesBuff));
	callCL(clSetKernelArg(kernel, 3, sizeof(sizePerParticleBuff), &sizePerParticleBuff));

	size_t globalWorkSize = 1;
	size_t localWorkSize = 1; //TODO change that ?
	size_t offset = 0;
	//TODO 
	//1 - initialize small amount of particle with + only number
	//2 - change x y z with random numbers for other particles
	//3 - reverse x y and/or z to fill particles in 7 other square of space
    callCL(clEnqueueNDRangeKernel(queue, kernel, 1, &offset, 
            &globalWorkSize, &localWorkSize, 0, NULL, NULL));

	callCL(clEnqueueReleaseGLObjects(queue, 1, &clBuffer, 0, 0, NULL));

	callCL(clFinish(queue));// need to make sure buffer is released for openGL

	callCL(clReleaseMemObject(seedBuff));
	callCL(clReleaseMemObject(nbParticlesBuff));
    callCL(clReleaseKernel(kernel));
    callCL(clReleaseProgram(ini));
}

void Particles::callCLFunc(cl_int errCode, std::string funcCall, int line)
{
	clProgram::checkError(funcCall + std::string(" at line ") + std::to_string(line), errCode);
}

void Particles::setCurrentMouse(float mouse_x, float mouse_y)
{
	mouseX = mouse_x;
	mouseY = mouse_y;
}

void Particles::update(float deltaTime)
{
	size_t globalWorkSize = NB_PARTICLES;
	size_t localWorkSize = 64; //TODO change that ?
	size_t offset = 0;

	float cursorPos[3];
	camera.unProjectToOrigin(mouseX, mouseY, projMat);
	Vec3 cursorPosVec = std::get<0>(camera.unProject(mouseX, mouseY, projMat));
	cursorPos[0] = cursorPosVec.x;
	cursorPos[1] = cursorPosVec.y;
	cursorPos[2] = cursorPosVec.z;
	//cursorPos[2] = 0.0f;

    callCL(clEnqueueWriteBuffer(clProgram::getQueue(), timeBuff, CL_TRUE, 0,
            sizeof(float), &deltaTime, 0, NULL, NULL));
	callCL(clSetKernelArg(updateKernel, 2, sizeof(timeBuff), &timeBuff));

    callCL(clEnqueueWriteBuffer(clProgram::getQueue(), cursorPosBuff, CL_TRUE, 0,
            sizeof(float) * 3, cursorPos, 0, NULL, NULL));
	callCL(clSetKernelArg(updateKernel, 3, sizeof(cursorPosBuff), &cursorPosBuff));


	clEnqueueAcquireGLObjects(clProgram::getQueue(), 1, &clBuffer, 0, 0, NULL);

    callCL(clEnqueueNDRangeKernel(clProgram::getQueue(), updateKernel, 1, &offset, 
            &globalWorkSize, &localWorkSize, 0, NULL, NULL));

	callCL(clEnqueueReleaseGLObjects(clProgram::getQueue(), 1, &clBuffer, 0, 0, NULL));

	callCL(clFinish(clProgram::getQueue()));
}

void Particles::draw()
{
	std::array<float, 4> color = PARTICLE_COLOR;
	Vec3 lineDir, lineOrigin;
	std::pair<Vec3, Vec3> points = camera.unProject(mouseX, mouseY, projMat);
	lineOrigin = std::get<0>(points);
	lineDir = lineOrigin - std::get<1>(points);
	lineDir = lineDir.getNormalized();
	float vec3Data[6];
	for (int i = 0; i < 3; i++)
	{
		vec3Data[i] = lineOrigin[i];
		vec3Data[i + 3] = lineDir[i];
	}

	shader->use();
	Matrix precalcMat = projMat * camera.getMatrix();
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "camera"), 1, GL_TRUE, camera.getMatrix().exportForGL());
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "precalcMat"), 1, GL_TRUE, precalcMat.exportForGL());
	glUniform3fv(glGetUniformLocation(shader->getID(), "mouseLineOrigin"), 1, vec3Data);
	glUniform3fv(glGetUniformLocation(shader->getID(), "mouseLineDir"), 1, &vec3Data[3]);
    glUniform4fv(glGetUniformLocation(shader->getID(), "myColor"), 1, &color.front());
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, NB_PARTICLES);
}

void Particles::clear()
{
	callCL(clReleaseMemObject(sizePerParticleBuff));
	callCL(clReleaseMemObject(clBuffer));//TODO check if needed
	callCL(clReleaseMemObject(timeBuff));
	callCL(clReleaseMemObject(cursorPosBuff));
	callCL(clReleaseKernel(updateKernel));
	callCL(clReleaseProgram(updateProgram));
	glDeleteBuffers(1, &VBO);
	VBO = 0;
	glDeleteVertexArrays(1, &VAO);
	VAO = 0;
	delete shader;
	shader = nullptr;
	std::cout << "Cleared Particles" << std::endl;
	clProgram::clear();
}
