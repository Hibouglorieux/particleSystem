__kernel void updatePosition(__global float* ptr, __constant uint* sizePerParticle, __constant float* deltaTime)
{
	int i = get_global_id(0) * *sizePerParticle;

	ptr[i] = ptr[i] + ptr[i + 3] * *deltaTime;
	ptr[i + 1] = ptr[i + 1] + ptr[i + 4] * *deltaTime;
	ptr[i + 2] = ptr[i + 2] + ptr[i + 5] * *deltaTime;
}
