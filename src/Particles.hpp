/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Particles.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/06 03:00:37 by nathan            #+#    #+#             */
/*   Updated: 2020/11/07 18:38:21 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PARTICLES_CLASS_H
# define PARTICLES_CLASS_H
# define NB_PARTICLES 6
# define PARTICLE_SIZE 10.f
# define PARTICLE_COLOR {0.1f, 0.9f, 0.1f, 1.0}
# define SEED 42
# define callCL(x) callCLFunc(x, #x, __LINE__)
#include <CL/cl.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "clProgram.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Utilities.h"

class Particles {
public:
	static void initialize();
	static void initializeOpenGL();
	static void initializeParticles();
	static void draw();
	static Camera& getCamera(){return camera;}
	static void clear();
private:
	static void callCLFunc(cl_int errCode, std::string funcCall, std::string line);
	static void initializeBuffers();
	static bool initialized;
	static GLuint VAO, VBO;
	static cl_mem clBuffer;
	static Camera camera;
	static Matrix projMat;
	static Shader* shader;
};

#endif
