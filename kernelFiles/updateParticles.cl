__kernel void updateParticles(__global float* ptr, __constant uint* sizePerParticle, __constant float* deltaTime)
{
	int i = get_global_id(0) * *sizePerParticle;

	for (int j = 0; j < 3; j++)
	{
		ptr[i + j] = ptr[i + j] + (ptr[i + j + 3] * *deltaTime);
	}
}
