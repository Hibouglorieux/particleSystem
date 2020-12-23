#version 330 core
in vec3 worldPos;
in vec3 speedColor;
uniform vec3 mouseLineDir;
uniform vec3 mouseLineOrigin;
uniform vec4 myColor;

out vec4 FragColor;


float interpolate(float value, float minimum, float maximum)
{
	float retVal;
	retVal = (value - minimum) / maximum;

	return 1.0f - max(min(retVal, 0.8f), 0.0f);
}

float distanceFromLine(vec3 particlePoint, vec3 origin, vec3 dir)
{
	vec3 originToFrag = particlePoint - origin;
	float dotResult = dot(originToFrag, dir);
	vec3 projected = origin + dir * dotResult;

	return (distance(projected, particlePoint));
}

void main()
{
	//float dist = distanceFromLine(worldPos, mouseLineOrigin, mouseLineDir);
	//float lightValue;
	//lightValue = interpolate(dist, 1.0f, 20.0f);
	//FragColor = myColor * lightValue;
	FragColor = vec4(normalize(speedColor), 1.0);
}
