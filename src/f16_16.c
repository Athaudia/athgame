#include "f16_16.h"

struct f16_16 f16_16(int16_t integer)
{
	return (struct f16_16){.integer=integer, .fractional=0};
}

struct f16_16 f16_16__add(struct f16_16 a, struct f16_16 b)
{
	return (struct f16_16){.val=a.val+b.val};
}

struct f16_16 f16_16__sub(struct f16_16 a, struct f16_16 b)
{
	return (struct f16_16){.val=a.val-b.val};
}

struct f16_16 f16_16__mul(struct f16_16 a, struct f16_16 b)
{
	return (struct f16_16){.val=((int64_t)a.val*(int64_t)b.val)>>16};
}

struct f16_16 f16_16__div(struct f16_16 a, struct f16_16 b)
{
	return (struct f16_16){.val=((((int64_t)a.val)<<16)/((int64_t)b.val))};
}

//static struct f16_16 agc_pi = (struct f16_16){.val=0x3243F};

struct f16_16 f16_16__sin(struct f16_16 a)
{
	a = f16_16__add(a, f16_16__div(f16_16(1), f16_16(4)));
	struct f16_16 z = (struct f16_16){.val=(a.val&0x7fff)<<2};
	z = f16_16__sub(z,f16_16(1));
	return f16_16__mul(  f16_16__mul(f16_16__div(z, f16_16(2)), f16_16__sub(f16_16(3), f16_16__mul(z, z))),  f16_16(1-2*((a.val&0x8000) >> 15)));
}

struct f16_16 f16_16__cos(struct f16_16 a)
{
	a = f16_16__add(a, f16_16__div(f16_16(1), f16_16(2)));
	struct f16_16 z = (struct f16_16){.val=(a.val&0x7fff)<<2};
	z = f16_16__sub(z,f16_16(1));
	return f16_16__mul(  f16_16__mul(f16_16__div(z, f16_16(2)), f16_16__sub(f16_16(3), f16_16__mul(z, z))),  f16_16(1-2*((a.val&0x8000) >> 15)));
}

double f16_16__to_double(struct f16_16 a)
{
	return a.integer + a.fractional/65536.0;
}
