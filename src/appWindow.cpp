/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   appWindow.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <nallani@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 01:55:16 by nathan            #+#    #+#             */
/*   Updated: 2022/06/24 23:31:59 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "appWindow.hpp"

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
	//	glfwWindowHint(GLFW_SAMPLES, 4); // antilia4ing x4
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // openGL 4.5
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5); // "
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // no support from old openGL
		// Open a window and create its OpenGL context
		window = glfwCreateWindow( mode->width, mode->height, "particleSystem", NULL, NULL);

		if( window == nullptr )
		{
			std::cerr << "Failed to create a glfw appWindow" << std::endl;
			glfwTerminate();
			return 0;
		}

		glfwMakeContextCurrent(window);
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); // enable sticky keys
	}
	initialized = true;
	return (1);
}

void	appWindow::getWindowSize(int* width, int* height)
{
	glfwGetWindowSize(window, width, height);
}

int		appWindow::getWindowWidth()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return width;
}

int		appWindow::getWindowHeight()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return height;
}

GLFWwindow*	appWindow::getWindow()
{
	return window;
}
