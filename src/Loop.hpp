/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 15:40:30 by nathan            #+#    #+#             */
/*   Updated: 2022/06/24 23:24:55 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef LOOP_CLASS_H
# define LOOP_CLASS_H

#include <vector>
#include "ParticleSystem.h"
#include "appWindow.hpp"
//TMP
#include "Line.hpp"

class Loop {
public:
	static void loop();
private:
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scrollCallBack(GLFWwindow* window, double xoffset, double yoffset);
	static void processInput();
	static bool shouldStop;
	static double frameTime;
	static double fpsRefreshTime;
	static unsigned char frameCount;
	static double mouseX, mouseY;
	const static double refreshingRate;
	static std::vector<Line*> lines;
};

#endif
