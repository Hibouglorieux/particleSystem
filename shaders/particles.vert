#version 330 core
layout (location = 0) in vec3 pos;
uniform mat4 camera;
uniform mat4 precalcMat;
out vec4 fragPos;

void main()
{
	gl_Position = precalcMat * vec4(pos, 1.0);
	fragPos = camera * vec4(pos, 1.0);
}
