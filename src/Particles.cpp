/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Particles.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/06 03:00:32 by nathan            #+#    #+#             */
/*   Updated: 2022/07/15 11:42:56 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Particles.hpp"
#include <utility>

#define FOV 60.0f
#define NEAR 0.01f
#define FAR 100000.0f

#define MAX_NB_OF_GRAVITY_POS 4

bool Particles::initialized = false;
GLuint Particles::VAO = 0;
GLuint Particles::VBO = 0;
cl_mem Particles::clBuffer = NULL;
cl_mem Particles::sizePerParticleBuff = NULL;
cl_mem Particles::timeBuff = NULL;
cl_mem Particles::gravityPosBuff = NULL;
cl_mem Particles::seedBuff = NULL;
cl_mem Particles::nbParticlesBuff = NULL;
cl_mem Particles::numberOfGravityPointsBuff = NULL;
cl_kernel Particles::iniAsCircleKernel = NULL;
cl_kernel Particles::iniAsSquareKernel = NULL;
cl_kernel Particles::updateSpeedKernel = NULL;
cl_kernel Particles::updatePosKernel = NULL;
cl_kernel Particles::updateAllKernel = NULL;
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
int Particles::invertColorsValue = 1;
size_t	Particles::currentColorIndex = 0;
std::vector<std::array<float, 4>> Particles::colors = {
	{0.5f, 0.0f, 0.0f, 1.0},
	{0.5f, 0.1f, 0.0f, 1.0},
	{0.0f, 0.5f, 0.0f, 1.0},
	{0.0f, 0.0f, 0.5f, 1.0},
	{0.5f, 0.0f, 0.3f, 1.0},
	{0.0f, 0.7f, 0.7f, 1.0},
	{0.9f, 0.0f, 0.5f, 1.0}
};
int	Particles::particlesNb = DEFAULT_NB_PARTICLES;
float Particles::distanceLightStrength = 80.f;

void Particles::initialize(int particlesNumber)
{
	if (!initialized)
	{
		if (particlesNumber > 0)
			particlesNb = particlesNumber;
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

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * SIZE_PER_PARTICLE * particlesNb, NULL, GL_DYNAMIC_DRAW);//TODO test draw / read / copy
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
             sizeof(float) * 3 * MAX_NB_OF_GRAVITY_POS, NULL, &errCode);
	clProgram::checkError("clCreateBuffer", errCode);

    seedBuff = clCreateBuffer(clProgram::getContext(), CL_MEM_READ_WRITE, 
             sizeof(unsigned long), NULL, &errCode);
	clProgram::checkError("clCreateBuffer", errCode);

    nbParticlesBuff = clCreateBuffer(clProgram::getContext(), CL_MEM_READ_ONLY, 
             sizeof(int), NULL, &errCode);
	clProgram::checkError("clCreateBuffer", errCode);

	numberOfGravityPointsBuff = clCreateBuffer(clProgram::getContext(), CL_MEM_READ_WRITE,
			sizeof(unsigned int), NULL, &errCode);
	clProgram::checkError("clCreateBuffer", errCode);

	unsigned int sizePerParticle = SIZE_PER_PARTICLE;
    callCL(clEnqueueWriteBuffer(clProgram::getQueue(), sizePerParticleBuff, CL_TRUE, 0,
            sizeof(unsigned int), &sizePerParticle, 0, NULL, NULL));

	unsigned long seed = SEED;
    callCL(clEnqueueWriteBuffer(clProgram::getQueue(), seedBuff, CL_TRUE, 0,
            sizeof(unsigned long), &seed, 0, NULL, NULL));

    callCL(clEnqueueWriteBuffer(clProgram::getQueue(), nbParticlesBuff, CL_TRUE, 0,
            sizeof(int), &particlesNb, 0, NULL, NULL));
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

	cl_program updateAllProgram = clProgram::createProgram("updateAll.cl");
	updateAllKernel = clCreateKernel(updateAllProgram, "updateAll", &errCode);
	clProgram::checkError("clCreateKernel", errCode);

	callCL(clSetKernelArg(updateAllKernel, 0, sizeof(clBuffer), &clBuffer));
	callCL(clSetKernelArg(updateAllKernel, 1, sizeof(sizePerParticleBuff), &sizePerParticleBuff));
	callCL(clSetKernelArg(updateAllKernel, 2, sizeof(timeBuff), &timeBuff));
	callCL(clSetKernelArg(updateAllKernel, 3, sizeof(gravityPosBuff), &gravityPosBuff));
	callCL(clSetKernelArg(updateAllKernel, 4, sizeof(numberOfGravityPointsBuff), &numberOfGravityPointsBuff));
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
	size_t offset = 0;
    callCL(clEnqueueNDRangeKernel(queue, kernel, 1, &offset, 
            &globalWorkSize, nullptr, 0, NULL, NULL));

	callCL(clEnqueueReleaseGLObjects(queue, 1, &clBuffer, 0, 0, NULL));
	callCL(clFinish(queue));
	noGravity = true;
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
		Vec3 projected = camera.unProjectToOrigin(mouseX, mouseY, projMat);
		if (!(std::isnan(projected.x) || std::isnan(projected.y) || std::isnan(projected.z)))
		{
			gravityPoints[0] = projected;
			shouldSaveMouseCoords = false;
		}
	}
}

void Particles::update(float deltaTime)
{
	if (!isGravityStatic)
	{
		Vec3 projected = camera.unProjectToOrigin(mouseX, mouseY, projMat);
		if (!(std::isnan(projected.x) || std::isnan(projected.y) || std::isnan(projected.z)))
			gravityPoints[0] = projected;
	}
	if (noGravity)
		return;
	size_t globalWorkSize = particlesNb;
	size_t offset = 0;

	deltaTime *= speed;
	
    callCL(clEnqueueWriteBuffer(clProgram::getQueue(), timeBuff, CL_TRUE, 0,
            sizeof(float), &deltaTime, 0, NULL, NULL));

	clEnqueueAcquireGLObjects(clProgram::getQueue(), 1, &clBuffer, 0, 0, NULL);
	callCL(clFinish(clProgram::getQueue()));

	for (unsigned int i = 0 ; i < gravityPoints.size(); i++)
	{
		float gravityPos[3];
		gravityPos[0] = gravityPoints[i].x;
		gravityPos[1] = gravityPoints[i].y;
		gravityPos[2] = gravityPoints[i].z;
		callCL(clEnqueueWriteBuffer(clProgram::getQueue(), gravityPosBuff, CL_TRUE, i * sizeof(float) * 3,
            sizeof(float) * 3, gravityPos, 0, NULL, NULL));
	}
	int numberOfGravityPoints = (int)gravityPoints.size();
	callCL(clEnqueueWriteBuffer(clProgram::getQueue(), numberOfGravityPointsBuff, CL_TRUE, 0,
				sizeof(int), &numberOfGravityPoints, 0, NULL, NULL));

	callCL(clEnqueueNDRangeKernel(clProgram::getQueue(), updateAllKernel, 1, &offset, 
            &globalWorkSize, nullptr, 0, NULL, NULL));
	callCL(clFinish(clProgram::getQueue()));
	callCL(clEnqueueReleaseGLObjects(clProgram::getQueue(), 1, &clBuffer, 0, 0, NULL));
}

void Particles::addGravityPoint()
{
	if (gravityPoints.size() < MAX_NB_OF_GRAVITY_POS)
	{	
		Vec3 projected = camera.unProjectToOrigin(mouseX, mouseY, projMat);
		if (std::isnan(projected.x) || std::isnan(projected.y) || std::isnan(projected.z))
		{
			if (!isGravityStatic)
				gravityPoints.push_back(gravityPoints[0]);// last known point
			//else woopsie doopsie
		}
		else
			gravityPoints.push_back(projected);
	}
}

void Particles::removeGravityPoints()
{
	gravityPoints.erase(gravityPoints.begin() + 1, gravityPoints.end());
}

std::array<float, 4> getAmbientColor(const std::array<float, 4>& baseColor)
{
	if (baseColor[0] == 0.5f && baseColor[1] == 0.1f && baseColor[2] == 0)
		return /*std::array<float, 4>*/{0.5f, 0.8f, 0, 1.0f};
	std::array<float, 4> ambientColor;
	ambientColor[0] = baseColor[1] == 0.f ? 0.8f - baseColor[0] : 0.8f;
	ambientColor[1] = baseColor[1] == 0.f ? 0.8f - baseColor[1] : 0.6f;
	ambientColor[2] = baseColor[2] == 0.f ? 0.8f - baseColor[2] : 0.6f;
	ambientColor[3] = 1.f;
	if (baseColor[0] == 0.0f && baseColor[1] == 0.0f)
		ambientColor[0] = 0.f;
	if (baseColor[1] == 0.0f && baseColor[2] == 0.0f)
		ambientColor[1] = 0.f;
	if (baseColor[0] == 0.0f && baseColor[2] == 0.0f)
		ambientColor[2] = 0.f;
	return ambientColor;
}

void Particles::draw()
{
	std::array<float, 4> color = colors[currentColorIndex];
	std::array<float, 4> ambientColor = getAmbientColor(color);
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
	delete[] vec3Data;
	glUniform1i(glGetUniformLocation(shader->getID(), "numberOfGravityCenters"), gravityPoints.size());
    glUniform1i(glGetUniformLocation(shader->getID(), "invertColors"), invertColorsValue);
    glUniform4fv(glGetUniformLocation(shader->getID(), "myColor"), 1, &color.front());
    glUniform4fv(glGetUniformLocation(shader->getID(), "ambientColor"), 1, &ambientColor.front());
    glUniform1f(glGetUniformLocation(shader->getID(), "distanceLightStrength"), distanceLightStrength);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, particlesNb);
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

void Particles::changeColor(int dir)
{
	if (dir > 0)
		currentColorIndex++;
	else if (dir < 0)
	{
		if (currentColorIndex == 0)
			currentColorIndex = colors.size() - 1;
		else
			currentColorIndex--;
	}
	if (currentColorIndex >= colors.size())
		currentColorIndex = 0;
}

void Particles::invertColors()
{
	if (invertColorsValue == 1)
		invertColorsValue = -1;
	else invertColorsValue = 1;
}

void Particles::changeDistanceLightStrength(float value)
{
	if (distanceLightStrength + value > 0)
		distanceLightStrength += value;
}

void Particles::clear()
{
	callCL(clReleaseMemObject(sizePerParticleBuff));
	callCL(clReleaseMemObject(clBuffer));//TODO check if needed
	callCL(clReleaseMemObject(timeBuff));
	callCL(clReleaseMemObject(gravityPosBuff));
	callCL(clReleaseMemObject(seedBuff));
	callCL(clReleaseMemObject(nbParticlesBuff));
	callCL(clReleaseMemObject(numberOfGravityPointsBuff));

    callCL(clReleaseKernel(iniAsCircleKernel));
    callCL(clReleaseKernel(iniAsSquareKernel));
	callCL(clReleaseKernel(updateSpeedKernel));
	callCL(clReleaseKernel(updatePosKernel));
	callCL(clReleaseKernel(updateAllKernel));

	glDeleteBuffers(1, &VBO);
	VBO = 0;
	glDeleteVertexArrays(1, &VAO);
	VAO = 0;
	delete shader;
	shader = nullptr;
	std::cout << "Cleared Particles" << std::endl;
	clProgram::clear();
}
