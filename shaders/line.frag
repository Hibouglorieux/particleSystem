#version 330 core
out vec4 FragColor;
in vec4 fragPos;
uniform vec4 myColor;

void main()
{
	FragColor = myColor;
}
