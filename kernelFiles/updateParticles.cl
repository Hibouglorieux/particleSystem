
__constant float3 origin = {0.0f, 0.0f, 0.0f};

inline float3 getAxisFromPoint(__global float3* point)
{
	float3 result = cross(*point, origin);
	return result;
}

float3 attractToPoint(float3 currentVector, float3 target)
{
	return normalize(target - currentVector) * 0.001f;
}

__kernel void updateParticles(__global float* ptr, __constant uint* sizePerParticle, __constant float* deltaTime, __constant float3* cursorPos)
{
	int i = get_global_id(0) * *sizePerParticle;

	float3 currentVector;
	currentVector.x = ptr[i];
	currentVector.y = ptr[i + 1];
	currentVector.z = ptr[i + 2];

	float3 test = attractToPoint(currentVector, *cursorPos);
	ptr[i + 3] += test.x;
	ptr[i + 4] += test.y;
	ptr[i + 5] += test.z;
	ptr[i] = ptr[i] + (ptr[i + 3]);
	ptr[i + 1] = ptr[i + 1] + (ptr[i + 4]);
	ptr[i + 2] = ptr[i + 2] + (ptr[i + 5]);
	
	//take time into account
	//float3 test = normalize(*cursorPos - currentVector) * 10000;
	ptr[i] = ptr[i] + (ptr[i + 3] * *deltaTime);
	ptr[i + 1] = ptr[i + 1] + (ptr[i + 4] * *deltaTime);
	ptr[i + 2] = ptr[i + 2] + (ptr[i + 5] * *deltaTime);
}
