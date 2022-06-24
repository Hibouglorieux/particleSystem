/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Particles.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/06 03:00:32 by nathan            #+#    #+#             */
/*   Updated: 2022/06/24 23:59:48 by nallani          ###   ########.fr       */
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
cl_mem Particles::gravityPosBuff = NULL;
cl_mem Particles::seedBuff = NULL;
cl_mem Particles::nbParticlesBuff = NULL;
cl_kernel Particles::iniAsCircleKernel = NULL;
cl_kernel Particles::iniAsSquareKernel = NULL;
cl_kernel Particles::updateSpeedKernel = NULL;
cl_kernel Particles::updatePosKernel = NULL;
Shader* Particles::shader = nullptr;
Camera Particles::camera = {0, 0, 50};
Matrix Particles::projMat = Matrix();
float Particles::mouseX = 0.0f;
float Particles::mouseY = 0.0f;
float Particles::speed = 10.0f;
bool Particles::isGravityStatic = false;
bool Particles::noGravity = true;
bool Particles::shouldSaveMouseCoords = false;
std::vector<Vec3> Particles::gravityPoints = {{0, 0 ,0}};

void Particles::initialize()
{
	if (!initialized)
	{
		projMat = Matrix::createProjMatrix(FOV, (float)appWindow::getWindowWidth() / (float)appWindow::getWindowHeight(), NEAR, FAR);
		clProgram::initialize();
		initialized = true;
		initializeBuffers();
		initializeOpenGL();
		initializeOpenCL();
		initializeParticlesPos(SQUARE);
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

    gravityPosBuff = clCreateBuffer(clProgram::getContext(), CL_MEM_READ_ONLY, 
             sizeof(float) * 3, NULL, &errCode);
	clProgram::checkError("clCreateBuffer", errCode);

    seedBuff = clCreateBuffer(clProgram::getContext(), CL_MEM_READ_WRITE, 
             sizeof(unsigned long), NULL, &errCode);
	clProgram::checkError("clCreateBuffer", errCode);

    nbParticlesBuff = clCreateBuffer(clProgram::getContext(), CL_MEM_READ_ONLY, 
             sizeof(unsigned int), NULL, &errCode);
	clProgram::checkError("clCreateBuffer", errCode);

	unsigned int sizePerParticle = SIZE_PER_PARTICLE;
    callCL(clEnqueueWriteBuffer(clProgram::getQueue(), sizePerParticleBuff, CL_TRUE, 0,
            sizeof(unsigned int), &sizePerParticle, 0, NULL, NULL));

	unsigned long seed = SEED;
    callCL(clEnqueueWriteBuffer(clProgram::getQueue(), seedBuff, CL_TRUE, 0,
            sizeof(unsigned long), &seed, 0, NULL, NULL));

	unsigned int nbParticles = NB_PARTICLES;
    callCL(clEnqueueWriteBuffer(clProgram::getQueue(), nbParticlesBuff, CL_TRUE, 0,
            sizeof(unsigned int), &nbParticles, 0, NULL, NULL));
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

	cl_program iniAsCircle = clProgram::createProgram("initializeAsCircle.cl");
    iniAsCircleKernel = clCreateKernel(iniAsCircle, "initializeAsCircle", &errCode);

	cl_program iniAsSquare = clProgram::createProgram("initializeAsSquare.cl");
    iniAsSquareKernel = clCreateKernel(iniAsSquare, "initializeAsSquare", &errCode);

	for (int i = 0; i < 2; i++)
	{
		cl_kernel target = i == 0 ? iniAsCircleKernel : iniAsSquareKernel;
		callCL(clSetKernelArg(target, 0, sizeof(clBuffer), &clBuffer));
		callCL(clSetKernelArg(target, 1, sizeof(seedBuff), &seedBuff));
		callCL(clSetKernelArg(target, 2, sizeof(nbParticlesBuff), &nbParticlesBuff));
		callCL(clSetKernelArg(target, 3, sizeof(sizePerParticleBuff), &sizePerParticleBuff));
	}

	cl_program updateSpeedProgram = clProgram::createProgram("updateSpeed.cl");
    updateSpeedKernel = clCreateKernel(updateSpeedProgram, "updateSpeed", &errCode);
	clProgram::checkError("clCreateKernel", errCode);

	cl_program updatePosProgram = clProgram::createProgram("updatePosition.cl");
	updatePosKernel = clCreateKernel(updatePosProgram, "updatePosition", &errCode);
	clProgram::checkError("clCreateKernel", errCode);


	for (int i = 0; i < 2; i++)
	{
		cl_kernel target = i == 0 ? updateSpeedKernel : updatePosKernel;
		callCL(clSetKernelArg(target, 0, sizeof(clBuffer), &clBuffer));
		callCL(clSetKernelArg(target, 1, sizeof(sizePerParticleBuff), &sizePerParticleBuff));
		callCL(clSetKernelArg(target, 2, sizeof(timeBuff), &timeBuff));
	}
	callCL(clSetKernelArg(updateSpeedKernel, 3, sizeof(gravityPosBuff), &gravityPosBuff));
}

void Particles::initializeParticlesPos(int mod)
{

    cl_kernel kernel;
   	if (mod & CIRCLE)
    	kernel = iniAsCircleKernel;
	else if (mod & SQUARE)
    	kernel = iniAsSquareKernel;
	else
	{
		std::cerr << "Wrong mod received in initializeParticlePos" << std::endl;
		exit(-1);
	}

	cl_command_queue queue = clProgram::getQueue();
	glFinish();
	clEnqueueAcquireGLObjects(queue, 1, &clBuffer, 0, 0, NULL);

	size_t globalWorkSize = 1;
	size_t localWorkSize = 1; //TODO change that ?
	size_t offset = 0;
    callCL(clEnqueueNDRangeKernel(queue, kernel, 1, &offset, 
            &globalWorkSize, &localWorkSize, 0, NULL, NULL));

	callCL(clEnqueueReleaseGLObjects(queue, 1, &clBuffer, 0, 0, NULL));
	callCL(clFinish(queue));
}

void Particles::callCLFunc(cl_int errCode, std::string funcCall, int line)
{
	clProgram::checkError(funcCall + std::string(" at line ") + std::to_string(line), errCode);
}

void Particles::setCurrentMouse(float mouse_x, float mouse_y)
{
	mouseX = mouse_x;
	mouseY = mouse_y;
	if (shouldSaveMouseCoords)
	{
		gravityPoints[0] = camera.unProjectToOrigin(mouseX, mouseY, projMat);
		shouldSaveMouseCoords = false;
	}
}

void Particles::update(float deltaTime)
{
	if (!isGravityStatic)
		gravityPoints[0] = camera.unProjectToOrigin(mouseX, mouseY, projMat);
	if (noGravity)
		return;
	size_t globalWorkSize = NB_PARTICLES;
	size_t localWorkSize = clProgram::getMaxGroupSize(); //TODO change that ?
	size_t offset = 0;

	deltaTime *= speed;
	float deltaTimeMultByAcceleration = deltaTime * 100000000000.0f;
	//std::cout << deltaTime << std::endl;

    callCL(clEnqueueWriteBuffer(clProgram::getQueue(), timeBuff, CL_FALSE, 0,
            sizeof(float), &deltaTimeMultByAcceleration, 0, NULL, NULL));

	clEnqueueAcquireGLObjects(clProgram::getQueue(), 1, &clBuffer, 0, 0, NULL);
	callCL(clFinish(clProgram::getQueue()));

	for (Vec3 gravityPoint : gravityPoints)
	{
		float gravityPos[3];
		gravityPos[0] = gravityPoint.x;
		gravityPos[1] = gravityPoint.y;
		gravityPos[2] = gravityPoint.z;

		callCL(clEnqueueWriteBuffer(clProgram::getQueue(), gravityPosBuff, CL_TRUE, 0,
            sizeof(float) * 3, gravityPos, 0, NULL, NULL));
		callCL(clEnqueueNDRangeKernel(clProgram::getQueue(), updateSpeedKernel, 1, &offset, 
            &globalWorkSize, &localWorkSize, 0, NULL, NULL));
		callCL(clFinish(clProgram::getQueue()));
	}

    callCL(clEnqueueWriteBuffer(clProgram::getQueue(), timeBuff, CL_TRUE, 0,
            sizeof(float), &deltaTime, 0, NULL, NULL));
	callCL(clEnqueueNDRangeKernel(clProgram::getQueue(), updatePosKernel, 1, &offset, 
            &globalWorkSize, &localWorkSize, 0, NULL, NULL));
	callCL(clFinish(clProgram::getQueue()));
	callCL(clEnqueueReleaseGLObjects(clProgram::getQueue(), 1, &clBuffer, 0, 0, NULL));
}

void Particles::addGravityPoint()
{
	if (gravityPoints.size() < 10)
		gravityPoints.push_back(camera.unProjectToOrigin(mouseX, mouseY, projMat));
}

void Particles::removeGravityPoints()
{
	gravityPoints.erase(gravityPoints.begin() + 1, gravityPoints.end());
}

void Particles::draw()
{
	std::array<float, 4> color = PARTICLE_COLOR;
	Vec3 lineDir, lineOrigin;

	shader->use();
	Matrix precalcMat = projMat * camera.getMatrix();
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "camera"), 1, GL_TRUE, camera.getMatrix().exportForGL());
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "precalcMat"), 1, GL_TRUE, precalcMat.exportForGL());
	float* vec3Data = new float[3 * gravityPoints.size()];
	for (size_t i = 0; i < gravityPoints.size(); i++)
	{
		vec3Data[i * 3] = gravityPoints[i].x;
		vec3Data[i * 3 + 1] = gravityPoints[i].y;
		vec3Data[i * 3 + 2] = gravityPoints[i].z;
	}
	glUniform3fv(glGetUniformLocation(shader->getID(), "gravityCenters"), gravityPoints.size(), vec3Data);
	glUniform1i(glGetUniformLocation(shader->getID(), "numberOfGravityCenters"), gravityPoints.size());
    glUniform4fv(glGetUniformLocation(shader->getID(), "myColor"), 1, &color.front());
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, NB_PARTICLES);
}

void Particles::lockGravityPoint(bool gravityStatic)
{
	isGravityStatic = gravityStatic;
	shouldSaveMouseCoords = true;
}

void Particles::changeSpeed(float value)
{
	speed += value;
	if (speed < 0.3f)
		speed = 0.3f;
}

void Particles::gravityOnOff()
{
	noGravity = !noGravity;
}

void Particles::clear()
{
	callCL(clReleaseMemObject(sizePerParticleBuff));
	callCL(clReleaseMemObject(clBuffer));//TODO check if needed
	callCL(clReleaseMemObject(timeBuff));
	callCL(clReleaseMemObject(gravityPosBuff));
	callCL(clReleaseMemObject(seedBuff));
	callCL(clReleaseMemObject(nbParticlesBuff));

    callCL(clReleaseKernel(iniAsCircleKernel));
    callCL(clReleaseKernel(iniAsSquareKernel));
	callCL(clReleaseKernel(updateSpeedKernel));
	callCL(clReleaseKernel(updatePosKernel));

	glDeleteBuffers(1, &VBO);
	VBO = 0;
	glDeleteVertexArrays(1, &VAO);
	VAO = 0;
	delete shader;
	shader = nullptr;
	std::cout << "Cleared Particles" << std::endl;
	clProgram::clear();
}
