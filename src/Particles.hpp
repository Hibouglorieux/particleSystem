/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Particles.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/06 03:00:37 by nathan            #+#    #+#             */
/*   Updated: 2023/06/19 16:07:03 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PARTICLES_CLASS_H
# define PARTICLES_CLASS_H
# define DEFAULT_NB_PARTICLES 1'000'000
# define SIZE_PER_PARTICLE 6
# define PARTICLE_SIZE 0.01f
# define SEED 42L
# define callCL(x) callCLFunc(x, #x, __LINE__)
# define CIRCLE 1
# define SQUARE CIRCLE << 1
#include "ParticleSystem.h"
#include <CL/cl.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "clProgram.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Utilities.h"

class Particles {
public:
	static void initialize(int particlesNumber);
	static void initializeOpenGL();
	static void initializeOpenCL();
	static void initializeParticlesPos(int mod);
	static void update(float deltaTime);
	static void addGravityPoint();
	static void removeGravityPoints();
	static void draw();
	static Camera& getCamera(){return camera;}
	static void clear();
	static Matrix getProjMat() {return projMat;}// TODO remove
	static void updateProjMat();
	static void setCurrentMouse(float mouse_x, float mouse_y);
	static void lockGravityPoint(bool isGravityStatic);
	static void changeSpeed(float value);
	static void gravityOnOff();
	static void	changeColor(int dir);
	static void	invertColors();
	static void changeDistanceLightStrength(float value);
private:
	static void callCLFunc(cl_int errCode, std::string funcCall, int line);
	static void initializeBuffers();
	static bool initialized;
	static GLuint VAO, VBO;
	static cl_mem clBuffer, sizePerParticleBuff, timeBuff, gravityPosBuff, seedBuff, nbParticlesBuff, numberOfGravityPointsBuff;
	static cl_kernel iniAsCircleKernel, iniAsSquareKernel, updateAllKernel;
	static Camera camera;
	static Matrix projMat;
	static Shader* shader;
	static bool isGravityStatic, shouldSaveMouseCoords, noGravity;
	static float mouseX, mouseY;
	static float speed;
	static std::vector<Vec3> gravityPoints;
	static std::vector<std::array<float, 4>> colors;
	static size_t currentColorIndex;
	static int invertColorsValue;
	static int particlesNb;

	static float distanceLightStrength;
};

#endif
