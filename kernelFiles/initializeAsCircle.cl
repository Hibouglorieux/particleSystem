#define CIRCLE_DIAMETER 20.0f
#include "kernelFiles/commonFunctions.cl"

__kernel void initializeAsCircle(__global float *ptr, __global ulong *seed, __constant uint *nbParticles, __constant uint *sizePerParticle)
{
	for (uint i = 0; i < *nbParticles; i++)
	{
		int j = i * *sizePerParticle;
		__private float3 randomFloat = randomizePosition(seed);
		randomFloat = normalize(randomFloat);
		randomFloat = randomFloat * getRandomFloatWithMaxValue(seed, CIRCLE_DIAMETER);

		ptr[j] = randomFloat.x;
		ptr[j + 1] = randomFloat.y;
		ptr[j + 2] = randomFloat.z;

		//set speed to 0
		ptr[j + 3] = 0;
		ptr[j + 4] = 0;
		ptr[j + 5] = 0;
	}
}
