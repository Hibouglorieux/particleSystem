#version 330 core
in vec3 worldPos;
in vec3 speedColor;
in vec3 gravityCenter;
uniform vec4 myColor;
uniform vec4 ambientColor = {0.5, 0.5, 0.0, 1.0};
out vec4 FragColor;


float interpolate(float value, float minimum, float maximum)
{
	float retVal;
	retVal = (value - minimum) / maximum;

	return 1.0f - max(min(retVal, 1.0f), 0.0f);
}

void main()
{
	//float dist = distanceFromLine(worldPos, mouseLineOrigin, mouseLineDir);
	float dist = distance(gravityCenter, worldPos);
	float lightValue;
	lightValue = interpolate(dist, 1.0f, 20.0f);
	FragColor = myColor + ambientColor * lightValue;
	//FragColor = vec4(normalize(speedColor), 1.0);
}
