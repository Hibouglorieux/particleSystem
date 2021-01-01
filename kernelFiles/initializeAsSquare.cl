#define MAX_NUMBER 20.0f
#include "kernelFiles/commonFunctions.cl"

__kernel void initializeAsSquare(__global float *ptr, __global ulong *seed, __constant uint *nbParticles, __constant uint *sizePerParticle)
{
	for (uint i = 0; i < *nbParticles; i++)
	{
		int j = i * *sizePerParticle;
		__private float3 randomFloat;
		ptr[j] = getRandomFloatWithMaxValue(seed, MAX_NUMBER);
		ptr[j + 1] = getRandomFloatWithMaxValue(seed, MAX_NUMBER);
		ptr[j + 2] = getRandomFloatWithMaxValue(seed, MAX_NUMBER);

		//set speed to 0
		ptr[j + 3] = 0;
		ptr[j + 4] = 0;
		ptr[j + 5] = 0;
	}
}
