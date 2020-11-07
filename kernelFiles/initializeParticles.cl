//__attribute__((work_group_size_hint(X, Y, Z))) 
float random(__local int *seed)
{
	__private float randomisedFloat;
	randomisedFloat = 1;
	//do operation on seed
	//"normalize float"
	return randomisedFloat;
}

__kernel void initializeParticles(__global float *ptr, __local int* seed)
{
	size_t i = get_global_id(0) * 3;
	__private float randomFloat = random(seed);
	__global float3 *particles = (global float3 *)&ptr[i];
	particles.x = randomFloat;
	//ptr[i] = (float)i;
	//ptr[i + 1] = (float)i;
	//ptr[i + 2] = 0;
}
