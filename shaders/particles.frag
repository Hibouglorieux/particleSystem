#version 330 core
out vec4 FragColor;
in vec4 fragPos;
uniform vec4 myColor;


float interpolate(float value, float minimum, float maximum)
{
	float retVal;
	retVal = (value - minimum) / maximum;

	return 1.0f - max(min(retVal, 0.8f), 0.0f);
}

void main()
{
	float dist = length(fragPos);
	float lightValue = 0.0f;
	lightValue = interpolate(dist, 6.0f, 15.0f);
	FragColor = myColor * lightValue;

}
