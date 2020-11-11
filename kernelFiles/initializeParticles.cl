//__attribute__((work_group_size_hint(X, Y, Z))) 

#define MAX_NUMBER 10.0f
#define CIRCLE_DIAMETER 2.0f

float getRandomDiameter(__global ulong *seed)
{
	__private int randomNum;
	__private float randomDiameter;
	__private ulong comparator = (1L << 48) - 1;
	*seed = (*seed * 0x5DEECE66DL + 0xBL) & comparator;
	randomNum = *seed >> 16;

	randomDiameter = (float)randomNum / (float)(0xFFFFFFFF / CIRCLE_DIAMETER);
	return randomDiameter;
}

float3 randomCircle(float3 randomisedFloat, __global ulong *seed)
{
	randomisedFloat = normalize(randomisedFloat) * getRandomDiameter(seed);
	return randomisedFloat;
}

float3 randomSquare(__global ulong *seed)
{
	__private float3 randomisedFloat;
	__private int3 randomNum;
	__private ulong comparator = (1L << 48) - 1;

	*seed = (*seed * 0x5DEECE66DL + 0xBL) & comparator;
	randomNum.x = *seed >> 16;

	*seed = (*seed * 0x5DEECE66DL + 0xBL) & comparator;
	randomNum.y = *seed >> 16;

	*seed = (*seed * 0x5DEECE66DL + 0xBL) & comparator;
	randomNum.z = *seed >> 16;

	randomisedFloat.x = (float)randomNum.x / (float)(0xFFFFFFFF / MAX_NUMBER);
	randomisedFloat.y = (float)randomNum.y / (float)(0xFFFFFFFF / MAX_NUMBER);
	randomisedFloat.z = (float)randomNum.z / (float)(0xFFFFFFFF / MAX_NUMBER);
	randomisedFloat = randomCircle(randomisedFloat, seed);
	return randomisedFloat;
}

__kernel void initializeParticles(__global float *ptr, __global ulong *seed, __constant uint *nbParticles)
{
	for (uint i = 0; i < *nbParticles; i++)
	{
		__private float3 randomFloat = randomSquare(seed);

		//printf("x: %f y: %f z: %f\n", randomFloat.x, randomFloat.y, randomFloat.z);
		ptr[i * 3] = randomFloat.x;
		ptr[i * 3 + 1] = randomFloat.y;
		ptr[i * 3 + 2] = randomFloat.z;
	}
}
