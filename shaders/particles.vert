#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 speed;
uniform mat4 camera;
uniform mat4 precalcMat;
uniform vec3 gravityCenters[10];
uniform int numberOfGravityCenters;
out vec3 worldPos;
out vec3 speedColor;
out vec3 gravityCenter;

vec3 getClosestGravityCenter()
{
	int i = 1;
	vec3 closestGCenter = gravityCenters[0];
	float currentDistance = distance(pos, closestGCenter);
	while (i < numberOfGravityCenters)
	{
		float newDistance = distance(pos, gravityCenters[i]);
		if (newDistance < currentDistance)
		{
			currentDistance = newDistance;
			closestGCenter = gravityCenters[i];
		}
		i++;
	}
	return closestGCenter;
}

void main()
{
	gl_Position = precalcMat * vec4(pos, 1.0);
	worldPos = pos;
	speedColor = abs(speed);
	gravityCenter = getClosestGravityCenter();
}
