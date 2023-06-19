/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <nallani@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 01:52:59 by nathan            #+#    #+#             */
/*   Updated: 2023/06/19 16:06:20 by nathan           ###   ########.fr       */
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

#define MAX_NB_PARTICLES 8'000'000

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
	if (particlesNumber <= 0)
		particlesNumber = DEFAULT_NB_PARTICLES;
	if (particlesNumber > MAX_NB_PARTICLES)
	{
		std::cout << "Warning, too many particles given, will initialize at 8 000 000 particles instead" << std::endl;
		particlesNumber = MAX_NB_PARTICLES;
	}

	int width, height;
	appWindow::getWindowSize(&width, &height);
	Particles::initialize(particlesNumber);

	glViewport(0, 0, width, height);
	Loop::loop();
	Particles::clear();

	return 1;
}
