/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Line.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/10 03:23:14 by nathan            #+#    #+#             */
/*   Updated: 2020/11/10 03:39:51 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef LINE_CLASS_H
# define LINE_CLASS_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <utility>
#include <vector>
#include "Particles.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
# define LINE_COLOR {0.9f, 0.2f, 0.1f, 1.0}

class Line {
public:
	Line(float mouseX, float mouseY);
	void draw();
	~Line(void);
private:
	Shader* shader;
	void initializeBuffers();
	std::pair<Vec3, Vec3> points;
	GLuint VBO, VAO;
};

#endif
