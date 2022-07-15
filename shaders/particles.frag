#version 330 core
in vec3 worldPos;
//in vec3 speedColor;
uniform vec4 myColor;
uniform vec4 ambientColor;
uniform int invertColors;
uniform float distanceLightStrength;
out vec4 FragColor;

struct s_gravityCenterData
{
	vec3 gravityCenterPos;
	float distanceFromGravityCenter;
};

in struct s_gravityCenterData gravityCenterData;

float interpolate(float value, float minimum, float maximum)
{
	float retVal = (value - minimum) / maximum;

	return clamp(retVal, 0.f, 1.f);
}

void main()
{
	float lightValue =  interpolate(gravityCenterData.distanceFromGravityCenter, 1.0f, distanceLightStrength);

	if (invertColors == 1)
		FragColor = myColor + ambientColor * (1 - lightValue);
	else
		FragColor = myColor + ambientColor * lightValue;
	//FragColor = vec4(normalize(speedColor), 1.0);
}
