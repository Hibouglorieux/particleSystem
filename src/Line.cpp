/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Line.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/10 03:23:12 by nathan            #+#    #+#             */
/*   Updated: 2020/11/11 15:24:09 by nathan           ###   ########.fr       */
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
	Vec3 cameraPos = Particles::getCamera().getPos();
	Vec3 point1, point2, dir;
	point1 = std::get<0>(points);
	point2 = std::get<1>(points);
	dir	= point1 - point2;
	dir = dir * 60;// make a good line
	data[0] = point1.x;
	data[1] = point1.y;
	data[2] = point1.z;
	data[3] = point1.x + dir.x;
	data[4] = point1.y + dir.y;
	data[5] = point1.z + dir.z;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 2, &data, GL_STATIC_DRAW);


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
	
	glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, 2);
	glPointSize(1.f);
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
