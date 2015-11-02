#pragma once
#include <stdint.h>

//typedef int32_t f16_16;
struct f16_16
{
	union
	{
		int32_t val;
		struct
		{
			//little endian only atm
			uint16_t fractional;
			int16_t integer;
		};
	};
};

struct f16_16 f16_16(int16_t integer);
struct f16_16 f16_16__add(struct f16_16 a, struct f16_16 b);
struct f16_16 f16_16__sub(struct f16_16 a, struct f16_16 b);
struct f16_16 f16_16__mul(struct f16_16 a, struct f16_16 b);
struct f16_16 f16_16__div(struct f16_16 a, struct f16_16 b);
double f16_16__to_double(struct f16_16 a);
