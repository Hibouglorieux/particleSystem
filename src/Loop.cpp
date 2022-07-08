/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 15:40:25 by nathan            #+#    #+#             */
/*   Updated: 2022/07/08 20:54:51 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Loop.hpp"
#include <glfw3.h>
#include <unistd.h>
#include "Camera.hpp"
#include <iomanip>
#include "Particles.hpp"

#define FPS 60.f

bool Loop::shouldStop = false;
double Loop::frameTime = 0.0f;
const double Loop::refreshingRate = 1.0 / FPS;
double Loop::mouseX = 0.0;
double Loop::mouseY = 0.0;
double Loop::fpsRefreshTime = 0.0;
unsigned char Loop::frameCount = 0;
std::vector<Line*> Loop::lines = {};

#define SEC_TO_MICROSEC 1000000
#define CAMERA_MOUVEMENT_SPEED 2.f
#define REFRESH_FPS_RATE 1.0f


void Loop::loop()
{
	glfwSetTime(0);
	glfwGetCursorPos(appWindow::getWindow(), &mouseX, &mouseY);
	glfwSetKeyCallback(appWindow::getWindow(), Loop::keyCallback);
	glfwSetScrollCallback(appWindow::getWindow(), Loop::scrollCallBack);
		glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(appWindow::getWindow()))
	{
		double currentTimer = glfwGetTime();
		frameCount++;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		processInput();

//		glDisable(GL_DEPTH_TEST);
		Particles::update((float)frameTime);
		Particles::draw();	
		for (Line* line : lines)
			line->draw();
		glFinish();

		glfwSwapBuffers(appWindow::getWindow());

		frameTime = glfwGetTime() - currentTimer;
		if (fpsRefreshTime + REFRESH_FPS_RATE < currentTimer)
		{
			std::stringstream ss;
			double fps = (float)frameCount / (currentTimer - fpsRefreshTime);
			if (fps > 60)
				fps = 60;
			ss << std::setprecision(3) << fps;
			glfwSetWindowTitle(appWindow::getWindow(), std::string(std::string("Humangl ") + ss.str()).c_str());
			frameCount = 0;
			fpsRefreshTime = currentTimer;
		}
	}
	for (Line* line : lines)
		delete line;
	lines.clear();
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

	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_UP) == GLFW_PRESS)
		Particles::changeSpeed(0.05f);
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
		Particles::changeSpeed(-0.05f);

	double oldMouseX = mouseX;
	double oldMouseY = mouseY;
	glfwGetCursorPos(appWindow::getWindow(), &mouseX, &mouseY);
	Particles::setCurrentMouse(mouseX, mouseY);
	if (glfwGetMouseButton(appWindow::getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		Particles::lockGravityPoint(true);
	if (glfwGetMouseButton(appWindow::getWindow(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		Particles::lockGravityPoint(false);
		lines.push_back(new Line(mouseX, mouseY));
	}
	if (glfwGetMouseButton(appWindow::getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		Particles::getCamera().rotate(mouseX - oldMouseX, mouseY - oldMouseY);
	// VISUAL RAYCASTING
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
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
		Particles::gravityOnOff();
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
		Particles::initializeParticlesPos(SQUARE);
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
		Particles::initializeParticlesPos(CIRCLE);
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
		Particles::addGravityPoint();
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		Particles::removeGravityPoints();
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		Particles::changeColor(1);
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		Particles::changeColor(-1);
	if (key == GLFW_KEY_I && action == GLFW_PRESS)
		Particles::invertColors();
}

void Loop::scrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
	(void)xoffset;

	if (window != appWindow::getWindow())
		return;
	if (yoffset != 0)
		Particles::getCamera().scroll(yoffset);
}
