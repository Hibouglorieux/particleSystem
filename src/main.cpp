/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <nallani@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 01:52:59 by nathan            #+#    #+#             */
/*   Updated: 2022/07/15 11:08:39 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <GL/glew.h>
#include "Utilities.h"
#include "appWindow.hpp"
#include "Camera.hpp"
#include "Loop.hpp"
#include "Matrix.hpp"
#include "clProgram.hpp"
#include "Particles.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <CL/cl.h>

int		main(int argc, char* argv[])
{
	if (!appWindow::init())
		return (0);
	if (glewInit() != GLEW_OK)
	{
		std::cerr <<  "Failed to initialize GLEW\n" << std::endl;
		return 0;
	}
	int particlesNumber = 0;
	if (argc != 1)
		particlesNumber = std::atoi(argv[1]);
	std::cout << particlesNumber << std::endl;

	int width, height;
	appWindow::getWindowSize(&width, &height);
	Particles::initialize(particlesNumber);

	glViewport(0, 0, width, height);
	Loop::loop();
	Particles::clear();

	return 1;
}
