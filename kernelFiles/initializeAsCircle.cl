#define MAX_NUMBER 10.0f
#define CIRCLE_DIAMETER 6.0f
#include "kernelFiles/commonFunctions.cl"

__kernel void initializeAsCircle(__global float *ptr, __global ulong *seed, __constant uint *nbParticles, __constant uint *sizePerParticle)
{
	//need this call because wtf
	//getRandomNumber(seed);
	for (uint i = 0; i < *nbParticles; i++)
	{
		int j = i * *sizePerParticle;
		__private float3 randomFloat = randomizePosition(seed);
		randomFloat = normalize(randomFloat);
		randomFloat = randomFloat * getRandomFloatWithMaxValue(seed, CIRCLE_DIAMETER);

		ptr[j] = randomFloat.x;
		ptr[j + 1] = randomFloat.y;
		ptr[j + 2] = randomFloat.z;
		//now random speed
		j += 3;
		float3 randomSpeed = normalize(randomizePosition(seed)) * 100000;
		ptr[j] = randomSpeed.x;
		ptr[j + 1] = randomSpeed.y;
		ptr[j + 2] = randomSpeed.z;
	}
}
