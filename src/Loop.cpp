/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 15:40:25 by nathan            #+#    #+#             */
/*   Updated: 2023/06/19 16:07:27 by nathan           ###   ########.fr       */
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
unsigned int Loop::frameCount = 0;

#define SEC_TO_MICROSEC 1000000
#define CAMERA_MOUVEMENT_SPEED 100.f
#define REFRESH_FPS_RATE 1.0f


void Loop::loop()
{
	glfwSetTime(0);
	glfwGetCursorPos(appWindow::getWindow(), &mouseX, &mouseY);
	glfwSetKeyCallback(appWindow::getWindow(), Loop::keyCallback);
	glfwSetScrollCallback(appWindow::getWindow(), Loop::scrollCallBack);
	glDisable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(appWindow::getWindow()))
	{
		double currentTimer = glfwGetTime();
		frameCount++;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		processInput(frameTime);

		Particles::update((float)frameTime);
		Particles::draw();	
		glFinish();

		glfwSwapBuffers(appWindow::getWindow());

		frameTime = glfwGetTime() - currentTimer;

		// actualize the display of the fps every last REFRESH_FPS_RATE seconds
		if (fpsRefreshTime + REFRESH_FPS_RATE < currentTimer)
		{
			std::stringstream ss;
			double fps = (float)frameCount / (currentTimer - fpsRefreshTime);
			ss << std::setprecision(4) << fps;
			glfwSetWindowTitle(appWindow::getWindow(), std::string(std::string("particleSystem ") + ss.str()).c_str());
			frameCount = 0;
			fpsRefreshTime = currentTimer;
		}
	}
}

void Loop::processInput(float deltaTime)
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
	Particles::getCamera().move(forward, backward, left, right, (shift == true ? 2 : 1)* CAMERA_MOUVEMENT_SPEED * deltaTime);

	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
		Particles::getCamera().moveUp(50 * deltaTime);
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		Particles::getCamera().moveDown(50 * deltaTime);

	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_UP) == GLFW_PRESS)
		Particles::changeSpeed(100 * deltaTime);
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
		Particles::changeSpeed(-100 * deltaTime);

	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_KP_1) == GLFW_PRESS)
		Particles::changeDistanceLightStrength(-40.f * deltaTime);
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_KP_3) == GLFW_PRESS)
		Particles::changeDistanceLightStrength(40.f * deltaTime);

	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_KP_4) == GLFW_PRESS)
		Particles::getCamera().changeProjectionDistance(-10.f * deltaTime);
	if (glfwGetKey(appWindow::getWindow(), GLFW_KEY_KP_6) == GLFW_PRESS)
		Particles::getCamera().changeProjectionDistance(10.f * deltaTime);

	double oldMouseX = mouseX;
	double oldMouseY = mouseY;
	Particles::setCurrentMouse(mouseX, mouseY);
	if (glfwGetMouseButton(appWindow::getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		Particles::lockGravityPoint(true);
	if (glfwGetMouseButton(appWindow::getWindow(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		Particles::lockGravityPoint(false);
	}
	glfwGetCursorPos(appWindow::getWindow(), &mouseX, &mouseY);
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
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
		Particles::getCamera().reset(); // bad method name, is actually locking camera into 'sphere' view
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		Particles::getCamera().freeMovement();
		Particles::initializeParticlesPos(SQUARE);
		Particles::removeGravityPoints();
		Particles::changeDistanceLightStrength(0);
		Particles::getCamera().changeProjectionDistance(0);
		Particles::getCamera().isScalingWithDistance = false;
	}
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
	if (key == GLFW_KEY_KP_2 && action == GLFW_PRESS)
		Particles::changeDistanceLightStrength(0);
	if (key == GLFW_KEY_KP_5 && action == GLFW_PRESS)
		Particles::getCamera().changeProjectionDistance(0);
	if (key == GLFW_KEY_KP_8 && action == GLFW_PRESS)
		Particles::getCamera().isScalingWithDistance = !Particles::getCamera().isScalingWithDistance;
}

void Loop::scrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
	(void)xoffset;

	if (window != appWindow::getWindow())
		return;
	if (yoffset != 0)
		Particles::getCamera().scroll(yoffset);
}
