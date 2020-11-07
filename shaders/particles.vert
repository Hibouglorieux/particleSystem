#version 330 core
layout (location = 0) in vec3 pos;
uniform mat4 precalcMat;

void main()
{
	gl_Position = precalcMat * vec4(pos, 1.0);
}
