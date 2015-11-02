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

double f16_16__to_double(struct f16_16 a)
{
	return a.integer + a.fractional/65536.0;
}
