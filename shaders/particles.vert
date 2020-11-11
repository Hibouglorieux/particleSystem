#version 330 core
layout (location = 0) in vec3 pos;
uniform mat4 camera;
uniform mat4 precalcMat;
out vec3 worldPos;

void main()
{
	gl_Position = precalcMat * vec4(pos, 1.0);
	worldPos = pos;
}
