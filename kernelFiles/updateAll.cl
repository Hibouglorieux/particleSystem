__kernel void updateAll(__global float* ptr, __constant uint* sizePerParticle, __constant float* deltaTime, __constant float3* gravityPoint, __constant int* numberOfGravityPoints)
{
	int i = get_global_id(0) * *sizePerParticle;

	float3 currentVector;
	currentVector.x = ptr[i];
	currentVector.y = ptr[i + 1];
	currentVector.z = ptr[i + 2];

	// updateSpeed
	if (*numberOfGravityPoints)
	{
		float3 totalSpeed = {0, 0, 0};
		for (int j = 0; j < *numberOfGravityPoints; j++)
		{
			if (currentVector.x != gravityPoint[j].x
				&& currentVector.y != gravityPoint[j].y
				&& currentVector.z != gravityPoint[j].z)
				totalSpeed += normalize(gravityPoint[j] - currentVector);
		}

		totalSpeed *= (*deltaTime * 1);
		ptr[i + 3] += totalSpeed.x;
		ptr[i + 4] += totalSpeed.y;
		ptr[i + 5] += totalSpeed.z;
	}

	//updatePos
	ptr[i] = ptr[i] + ptr[i + 3] * *deltaTime;
	ptr[i + 1] = ptr[i + 1] + ptr[i + 4] * *deltaTime;
	ptr[i + 2] = ptr[i + 2] + ptr[i + 5] * *deltaTime;
}
