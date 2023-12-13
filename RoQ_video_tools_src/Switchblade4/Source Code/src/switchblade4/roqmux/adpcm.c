// IMA-compatible ADPCM

static int indexOffsetTable[16] =
{
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8,
};

static int stepTable[89] =
{
	7,  8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 
	25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 
	80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 
	230, 253, 279, 307, 337, 371, 408, 449, 494, 544, 
	598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 
	1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 
	3024, 3327, 3660, 4026, 4428, 4871, 5358, 5894, 
	6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 
	13899, 15289, 16818, 18500, 20350, 22385, 24623, 
	27086, 29794, 32767
};


// ADPCMDecode: Returns a 16-bit sample from a 4-bit prediction
short ADPCMDecode(short previousSample, unsigned char *pIndex, unsigned char bits)
{
	int step;
	int newIndex;
	int error;
	int prediction;

	step = stepTable[*pIndex];

	newIndex = (int)(*pIndex) + indexOffsetTable[bits];
	if(newIndex < 0)
		newIndex = 0;
	else if(newIndex > 88)
		newIndex = 88;

	error = step >> 3;

	if(bits & 4) error += step;
	if(bits & 2) error += step>>1;
	if(bits & 1) error += step>>2;

	if(bits & 8)
		prediction = (int)previousSample - error;
	else
		prediction = (int)previousSample + error;

	if(prediction < -32768)
		prediction = -32768;
	else if(prediction > 32767)
		prediction = 32767;

	*pIndex = (unsigned char)newIndex;

	return (short)prediction;
}


// ADPCMEncode: Returns 4-bit output sample, updates pSample and pIndex
unsigned char ADPCMEncode(short *pSample, unsigned char *pIndex, short newSample)
{
	int diff;
	unsigned char storeBits;
	int step;

	diff = newSample - *pSample;

	if(diff < 0)
	{
		storeBits = 8;
		diff = -diff;
	}
	else
		storeBits = 0;

	step = stepTable[*pIndex];

	if(diff >= step)
	{
		diff -= step;
		storeBits |= 4;
	}

	step >>= 1;

	if(diff >= step)
	{
		diff -= step;
		storeBits |= 2;
	}

	step >>= 1;

	if(diff >= step)
		storeBits |= 1;


	// Decode the result
	*pSample = ADPCMDecode(*pSample, pIndex, storeBits);

	return storeBits;
}
