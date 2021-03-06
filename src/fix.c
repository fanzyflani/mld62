#include "common.h"

static const int16_t sintab[256] = {
	0, 402, 803, 1205, 1605, 2005, 2404, 2801,
	3196, 3589, 3980, 4369, 4756, 5139, 5519, 5896,
	6269, 6639, 7005, 7366, 7723, 8075, 8423, 8765,
	9102, 9434, 9759, 10079, 10393, 10701, 11002, 11297,
	11585, 11866, 12139, 12406, 12665, 12916, 13159, 13395,
	13622, 13842, 14053, 14255, 14449, 14634, 14810, 14978,
	15136, 15286, 15426, 15557, 15678, 15790, 15892, 15985,
	16069, 16142, 16206, 16260, 16305, 16339, 16364, 16379,
	16384, 16379, 16364, 16339, 16305, 16260, 16206, 16142,
	16069, 15985, 15892, 15790, 15678, 15557, 15426, 15286,
	15136, 14978, 14810, 14634, 14449, 14255, 14053, 13842,
	13622, 13395, 13159, 12916, 12665, 12406, 12139, 11866,
	11585, 11297, 11002, 10701, 10393, 10079, 9759, 9434,
	9102, 8765, 8423, 8075, 7723, 7366, 7005, 6639,
	6269, 5896, 5519, 5139, 4756, 4369, 3980, 3589,
	3196, 2801, 2404, 2005, 1605, 1205, 803, 402,
	0, -402, -803, -1205, -1605, -2005, -2404, -2801,
	-3196, -3589, -3980, -4369, -4756, -5139, -5519, -5896,
	-6269, -6639, -7005, -7366, -7723, -8075, -8423, -8765,
	-9102, -9434, -9759, -10079, -10393, -10701, -11002, -11297,
	-11585, -11866, -12139, -12406, -12665, -12916, -13159, -13395,
	-13622, -13842, -14053, -14255, -14449, -14634, -14810, -14978,
	-15136, -15286, -15426, -15557, -15678, -15790, -15892, -15985,
	-16069, -16142, -16206, -16260, -16305, -16339, -16364, -16379,
	-16384, -16379, -16364, -16339, -16305, -16260, -16206, -16142,
	-16069, -15985, -15892, -15790, -15678, -15557, -15426, -15286,
	-15136, -14978, -14810, -14634, -14449, -14255, -14053, -13842,
	-13622, -13395, -13159, -12916, -12665, -12406, -12139, -11866,
	-11585, -11297, -11002, -10701, -10393, -10079, -9759, -9434,
	-9102, -8765, -8423, -8075, -7723, -7366, -7005, -6639,
	-6269, -5896, -5519, -5139, -4756, -4369, -3980, -3589,
	-3196, -2801, -2404, -2005, -1605, -1205, -803, -402,
};

int fixtoi(fixed v)
{
	return v>>16;
}

fixed itofix(int v)
{
	return v<<16;
}

fixed fixmul(fixed a, fixed b)
{
	int64_t ae = a;
	int64_t be = b;

	int64_t re = ae*be;
	re >>= 16;

	// TODO: Handle overflow

	return (fixed)re;
}

fixed fixmulf(fixed a, fixed b)
{
	return (a>>8)*(b>>8);
}

fixed fixdiv(fixed a, fixed b)
{
	int64_t ae = a;
	int64_t be = b;

	ae <<= 16;
	int64_t re = ae/be;

	// TODO: Handle overflow
	return (fixed)re;
}

fixed fixsqrt(fixed v)
{
	int i;

	// WARNING: ASSUMES v IS POSITIVE

	fixed ret = 0;
	fixed ret2 = 0;
	for(i = 15; i >= 1; i--)
	{
		fixed tret2 = ret2 + (1<<(i<<1));
		if(tret2 <= v)
		{
			ret += ((1<<8)<<i);
			ret2 = tret2;
		}
	}

	return ret;
}

int32_t intsqrt(int32_t v)
{
	int i;

	// WARNING: ASSUMES v IS POSITIVE

	fixed ret = 0;
	fixed ret2 = 0;
	for(i = 15; i >= 1; i--)
	{
		fixed tret2 = ret2 + (1<<(i<<1));
		if(tret2 <= v)
		{
			ret += (1<<i);
			ret2 = tret2;
		}
	}

	return ret;
}

fixed fixisqrt(fixed v)
{
	// TODO: Proper inverse square root
	float sq = fixsqrt(v);
	
	if(sq == 0) sq = 1; // Prevent div-by-zero (gcc deliberately makes this crash!)
	return fixdiv(0x10000, sq);
}

fixed fixsin(fixed ang)
{
	int subang = ang&0xFF;
	int quoang = (ang>>8)&0xFF;

	fixed v0 = sintab[quoang];
	fixed v1 = sintab[(quoang+1)&0xFF];

	return (v0*(256-subang) + v1*subang + (1<<5))>>6;
}

fixed fixcos(fixed ang)
{
	return fixsin(ang + (FM_PI/2));
}

uint32_t randseed = 12342135; // keyboard mash
fixed fixrand1s(void)
{
	int r = (randseed>>7)&0x1FFFF;
	randseed *= 1103515245U;
	randseed += 12345U;
	randseed &= 0x7FFFFFFFU;
	r -= 0x10000;
	return (fixed)r;
}

