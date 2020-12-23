extern inline int getRandomNumber(__global ulong *seed)
{
	float randomisedFloat;
	int randomNum;
	ulong comparator = 0xFFFFFFFFFFFF;

	*seed = (*seed * 0x5DEECE66DL + 0xBL) & comparator;
	randomNum = *seed >> 16;
	return randomNum;
}

extern inline float getRandomFloatWithMaxValue(__global ulong *seed, float maxValue)
{
	float randomNumber = (float)getRandomNumber(seed);
	randomNumber = randomNumber / (float)(0xFFFFFFFF / maxValue);
	return randomNumber;
}

extern inline float3 randomizePosition(__global ulong *seed)
{
	__private float3 randomisedFloat;

	randomisedFloat.x = (float)getRandomNumber(seed);
	randomisedFloat.y = (float)getRandomNumber(seed);
	randomisedFloat.z = (float)getRandomNumber(seed);

	return randomisedFloat;
}
