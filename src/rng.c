#include "rng.h"

#include <stdlib.h>

struct ag_rng* ag_rng__new(enum ag_rng_type type, uint64_t seed)
{
	struct ag_rng* rng = (struct ag_rng*)malloc(sizeof(struct ag_rng));
	rng->type = type;
	switch(type)
	{
	case AG_RNG_XORSHIFT64:
		if(seed == 0)
			seed = 0xffffffffffffffffULL;
		rng->state = malloc(sizeof(uint64_t));
		*((uint64_t*)rng->state) = seed;
	}
	return rng;
}

void ag_rng__destroy(struct ag_rng* rng)
{
	free(rng->state);
	free(rng);
}

uint64_t ag_rng__next_u64(struct ag_rng* rng)
{
	switch(rng->type)
	{
	case AG_RNG_XORSHIFT64:
		*((uint64_t*)rng->state) ^= *((uint64_t*)rng->state) >> 12;
		*((uint64_t*)rng->state) ^= *((uint64_t*)rng->state) << 25;
		*((uint64_t*)rng->state) ^= *((uint64_t*)rng->state) >> 27;
		return *((uint64_t*)rng->state) * 2685821657736338717ULL;
	}
}

uint32_t ag_rng__next_u32(struct ag_rng* rng)
{
	return (uint32_t)ag_rng__next_u64(rng);
}

uint16_t ag_rng__next_u16(struct ag_rng* rng)
{
	return (uint16_t)ag_rng__next_u64(rng);
}

uint8_t ag_rng__next_u8(struct ag_rng* rng)
{
	return (uint8_t)ag_rng__next_u64(rng);
}

