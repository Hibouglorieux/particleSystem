/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 15:40:25 by nathan            #+#    #+#             */
/*   Updated: 2020/11/07 16:26:29 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Loop.hpp"
#include <glfw3.h>
#include <unistd.h>
#include "Camera.hpp"
#include <iomanip>
#include "Particles.hpp"

bool Loop::shouldStop = false;
double Loop::frameTime = 0.0f;
const double Loop::refreshingRate = 1.0 / 60.0;
double Loop::mouseX = 0.0;
double Loop::mouseY = 0.0;
double Loop::fpsRefreshTime = 0.0;
unsigned char Loop::frameCount = 0;

#define SEC_TO_MICROSEC 1000000
#define CAMERA_MOUVEMENT_SPEED 0.3f
#define REFRESH_FPS_RATE 0.5


void Loop::loop()
{
	glfwSetTime(0);
	glfwGetCursorPos(appWindow::getWindow(), &mouseX, &mouseY);
	glfwSetKeyCallback(appWindow::getWindow(), Loop::keyCallback);
	glfwSetScrollCallback(appWindow::getWindow(), Loop::scrollCallBack);
	while (!glfwWindowShouldClose(appWindow::getWindow()))
	{
		double currentTimer = glfwGetTime();
		frameCount++;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		processInput();

		//update(frameTime);


//		glEnable(GL_STENCIL_TEST);
//		glStencilFunc(GL_EQUAL, 0, 0xFF);
//		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//
//		glEnable(GL_DEPTH_TEST);


//		glDisable(GL_DEPTH_TEST);
//		glDisable(GL_STENCIL_TEST);
		Particles::draw();	
		glFinish();

		glfwSwapBuffers(appWindow::getWindow());

		frameTime = glfwGetTime() - currentTimer;
		if (frameTime < refreshingRate)
		{
			usleep((refreshingRate - frameTime) * SEC_TO_MICROSEC);
		}
		if (fpsRefreshTime + 0.5 > currentTimer)
		{
			std::stringstream ss;
			double fps = (float)frameCount / (currentTimer - fpsRefreshTime);
			ss << std::fixed << std::setprecision(1) << fps;
			glfwSetWindowTitle(appWindow::getWindow(), std::string(std::string("Humangl ") + std::to_string(fps)).c_str());
			frameCount = 0;
			fpsRefreshTime = currentTimer;
		}
	}
}

void Loop::processInput()
{
	glfwPollEvents();
	bool forward, backward, left, right;
	forward = backward = left = right = false;
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(appWindow::getWindow(), true);
	// camera
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_W) == GLFW_PRESS)
		forward = true;
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_S) == GLFW_PRESS)
		backward = true;
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_D) == GLFW_PRESS)
		right = true;
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_A) == GLFW_PRESS)
		left = true;
	bool shift = glfwGetKey(appWindow::getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? true : false;
	Particles::getCamera().move(forward, backward, left, right, (shift == true ? 2 : 1)* CAMERA_MOUVEMENT_SPEED);
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
		Particles::getCamera().moveUp();
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		Particles::getCamera().moveDown();

	double oldMouseX = mouseX;
	double oldMouseY = mouseY;
	glfwGetCursorPos(appWindow::getWindow(), &mouseX, &mouseY);
	Particles::getCamera().rotate(mouseX - oldMouseX, mouseY - oldMouseY);
}

void Loop::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	(void)scancode;
	(void)mods;// might use for speed of camera

	if (window != appWindow::getWindow())
		return;
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		Particles::getCamera().freeMovement();
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		Particles::getCamera().reset();
}

void Loop::scrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
	(void)xoffset;

	if (window != appWindow::getWindow())
		return;
	if (yoffset != 0)
		Particles::getCamera().scroll(yoffset);
}