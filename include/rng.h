#pragma once
#include <stdint.h>

enum ag_rng_type {AG_RNG_XORSHIFT64};
struct ag_rng
{
	enum ag_rng_type type;
	void* state;
};

struct ag_rng* ag_rng__new(enum ag_rng_type type, uint64_t seed);
void ag_rng__destroy(struct ag_rng* rng);
uint64_t ag_rng__next_u64(struct ag_rng* rng);
uint32_t ag_rng__next_u32(struct ag_rng* rng);
uint16_t ag_rng__next_u16(struct ag_rng* rng);
uint8_t ag_rng__next_u8(struct ag_rng* rng);

