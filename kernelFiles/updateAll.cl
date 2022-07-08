__kernel void updateAll(__global float* ptr, __constant uint* sizePerParticle, __constant float* deltaTime, __constant float3* gravityPoint, __constant int* numberOfGravityPoints, __constant int* shouldUpdatePos)
{
	int i = get_global_id(0) * *sizePerParticle;

	float3 currentVector;
	currentVector.x = ptr[i];
	currentVector.y = ptr[i + 1];
	currentVector.z = ptr[i + 2];

	for (int j = 0; j < numberOfGravityPoints; j++)
	{
		float3 speed = normalize(gravityPoint[j] - currentVector) * *deltaTime;
		ptr[i + 3] += speed.x;
		ptr[i + 4] += speed.y;
		ptr[i + 5] += speed.z;
	}



	if (shouldUpdatePos)
	{
		currentVector.x += ptr[i + 3] * *deltaTime;
		currentVector.y += ptr[i + 4] * *deltaTime;
		currentVector.z += ptr[i + 5] * *deltaTime;
	}
}
