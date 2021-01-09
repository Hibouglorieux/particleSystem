__kernel void updateSpeed(__global float* ptr, __constant uint* sizePerParticle, __constant float* deltaTime, __constant float3* gravityPoint)
{
	int i = get_global_id(0) * *sizePerParticle;

	float3 currentVector;
	currentVector.x = ptr[i];
	currentVector.y = ptr[i + 1];
	currentVector.z = ptr[i + 2];

	float3 speed = normalize(*gravityPoint - currentVector) * *deltaTime;
	ptr[i + 3] += speed.x;
	ptr[i + 4] += speed.y;
	ptr[i + 5] += speed.z;
}
