/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   appWindow.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <nallani@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 01:55:16 by nathan            #+#    #+#             */
/*   Updated: 2023/06/19 16:00:30 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "appWindow.hpp"
#include "Particles.hpp" // cheap fix for updateProjMat

#define WIDTH 1920
#define HEIGHT 1080
#define FULLSCREEN

bool appWindow::initialized = false;
GLFWwindow* appWindow::window = nullptr;
const GLFWvidmode* appWindow::mode = nullptr;

int appWindow::init()
{
	if (!initialized)
	{
		// Initialise GLFW
		if( !glfwInit() )
		{
			std::cerr << "Failed to initialize GLFW, bad return code on glfwInit" << std::endl;
			return (0);
		}
		int count;
		mode = glfwGetVideoMode(*glfwGetMonitors(&count));
		//glfwWindowHint(GLFW_SAMPLES, 4); // antilia4ing x4
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // openGL 4.5
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5); // "
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // no support from old openGL
		// Open a window and create its OpenGL context
#ifdef FULLSCREEN
		window = glfwCreateWindow( mode->width, mode->height, "particleSystem", NULL, NULL); // fullscreen
#else
		window = glfwCreateWindow(1920, 1080, "particleSystem", NULL, NULL); // fullscreen
#endif

		if( window == nullptr )
		{
			std::cerr << "Failed to create a glfw appWindow" << std::endl;
			glfwTerminate();
			return 0;
		}

		glfwMakeContextCurrent(window);
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); // enable sticky keys
	}
	glfwSetFramebufferSizeCallback(window, &resizeCallback);
	initialized = true;
	return (1);
}

void appWindow::resizeCallback(GLFWwindow* window, int width, int height)
{
	if (window != nullptr) // avoid warning
	{
		glViewport(0, 0, width, height);
		Particles::updateProjMat();
	}
}

void	appWindow::getWindowSize(int* width, int* height)
{
	glfwGetWindowSize(window, width, height);
}

int		appWindow::getWindowWidth()
{
#ifndef FULLSCREEN
	return WIDTH;
#else
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return width;
#endif
}

int		appWindow::getWindowHeight()
{
#ifndef FULLSCREEN
	return HEIGHT;
#else
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return height;
#endif
}

GLFWwindow*	appWindow::getWindow()
{
	return window;
}
