#version 330 core
in vec3 worldPos;
in vec3 speedColor;
uniform vec4 myColor;
uniform vec4 ambientColor;
uniform int invertColors;
out vec4 FragColor;

struct s_gravityCenterData
{
	vec3 gravityCenterPos;
	float distanceFromGravityCenter;
};

in struct s_gravityCenterData gravityCenterData;

float interpolate(float value, float minimum, float maximum)
{
	float retVal;
	retVal = (value - minimum) / maximum;

	return 1.0f - max(min(retVal, 1.0f), 0.0f);
}

void main()
{
	float lightValue =  interpolate(gravityCenterData.distanceFromGravityCenter, 1.0f, 80.0f);
	if (invertColors == 1)
		FragColor = myColor + ambientColor * lightValue;
	else
		FragColor = myColor + ambientColor * (1 - lightValue);
	//FragColor = vec4(normalize(speedColor), 1.0);
}
