#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 speed;
uniform mat4 camera;
uniform mat4 precalcMat;
uniform vec3 gravityCenters[10];
uniform int numberOfGravityCenters;
out vec3 worldPos;
//out vec3 speedColor;
out float distanceFromGravityCenter;

struct s_gravityCenterData
{
	vec3 gravityCenterPos;
	float distanceFromGravityCenter;
};

out struct s_gravityCenterData gravityCenterData;

struct s_gravityCenterData getClosestGravityCenter()
{
	int i = 1;
	struct s_gravityCenterData s;
	s.gravityCenterPos = gravityCenters[0];
	s.distanceFromGravityCenter = distance(pos, s.gravityCenterPos);
	while (i < numberOfGravityCenters)
	{
		float newDistance = distance(pos, gravityCenters[i]);
		if (newDistance < s.distanceFromGravityCenter)
		{
			s.distanceFromGravityCenter = newDistance;
			s.gravityCenterPos = gravityCenters[i];
		}
		i++;
	}
	return s;
}

void main()
{
	gl_Position = precalcMat * vec4(pos, 1.0);
	worldPos = pos;
	//speedColor = abs(speed);
	gravityCenterData = getClosestGravityCenter();
}
