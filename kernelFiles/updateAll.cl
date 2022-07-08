__kernel void updateAll(__global float* ptr, __constant uint* sizePerParticle, __constant float* deltaTime, __constant float3* gravityPoint, __constant int* numberOfGravityPoints)
{
	int i = get_global_id(0) * *sizePerParticle;

	float3 currentVector;
	currentVector.x = ptr[i];
	currentVector.y = ptr[i + 1];
	currentVector.z = ptr[i + 2];

	float3 totalSpeed;
	for (int j = 0; j < numberOfGravityPoints; j++)
	{
		totalSpeed += normalize(gravityPoint[j] - currentVector);
	}
	totalSpeed *= *deltaTime;
	ptr[i + 3] += totalSpeed.x;
	ptr[i + 4] += totalSpeed.y;
	ptr[i + 5] += totalSpeed.z;

	ptr[i] = ptr[i] + ptr[i + 3] * *deltaTime;
	ptr[i + 1] = ptr[i + 1] + ptr[i + 4] * *deltaTime;
	ptr[i + 2] = ptr[i + 2] + ptr[i + 5] * *deltaTime;
}
