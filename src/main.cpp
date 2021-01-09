/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <nallani@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 01:52:59 by nathan            #+#    #+#             */
/*   Updated: 2021/01/09 04:39:52 by nathan           ###   ########.fr       */
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

using namespace std;

int		main( void )
{
	if (!appWindow::init())
		return (0);
	if (glewInit() != GLEW_OK)
	{
		std::cerr <<  "Failed to initialize GLEW\n" << std::endl;
		return 0;
	}

	int width, height;
	appWindow::getWindowSize(&width, &height);
	Particles::initialize();

	glViewport(0, 0, width, height);
	Loop::loop();
	Particles::clear();

	return 1;
}
