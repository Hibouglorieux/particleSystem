/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Line.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/10 03:23:12 by nathan            #+#    #+#             */
/*   Updated: 2020/11/10 03:48:28 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Line.hpp"


Line::Line(float mouseX, float mouseY)
{
	points = Particles::getCamera().unProject(mouseX, mouseY, Particles::getProjMat());
	initializeBuffers();
}

void Line::initializeBuffers()
{
	float data[6];
	data[0] = std::get<0>(points).x;
	data[1] = std::get<0>(points).y;
	data[2] = std::get<0>(points).z;
	data[3] = std::get<1>(points).x;
	data[4] = std::get<1>(points).y;
	data[5] = std::get<1>(points).z;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 2, &data, GL_STATIC_DRAW);//TODO test draw / read / copy


    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	shader = new Shader("particles.vert", "line.frag");
}

void Line::draw()
{
	std::array<float, 4> color = LINE_COLOR;
	Matrix precalcMat = Particles::getProjMat() * Particles::getCamera().getMatrix();
	shader->use();
	glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "camera"), 1, GL_TRUE, Particles::getCamera().getMatrix().exportForGL());
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "precalcMat"), 1, GL_TRUE, precalcMat.exportForGL());
    glUniform4fv(glGetUniformLocation(shader->getID(), "myColor"), 1, &color.front());
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
}

Line::~Line()
{
	glDeleteBuffers(1, &VBO);
	VBO = 0;
	glDeleteVertexArrays(1, &VAO);
	VAO = 0;
	delete shader;
	shader = nullptr;
}
