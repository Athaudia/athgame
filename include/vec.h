#ifndef GUARD_VEC_H
#define GUARD_VEC_H
#include <stdint.h>
#include <stdbool.h>

struct ag_vec2i
{
	union
	{
		int32_t x, w;
	};
	union
	{
		int32_t y, h;
	};
};

struct ag_vec2i ag_vec2i(int32_t x, int32_t y);

struct ag_vec2i ag_vec2i_add(struct ag_vec2i a, struct ag_vec2i b);
struct ag_vec2i ag_vec2i_sub(struct ag_vec2i a, struct ag_vec2i b);
struct ag_vec2i ag_vec2i_div(struct ag_vec2i a, int32_t b);
struct ag_vec2i ag_vec2i_div_round(struct ag_vec2i a, int32_t b);
int32_t ag_vec2i_prod(struct ag_vec2i);

struct ag_vec2i ag_vec2i_mult(struct ag_vec2i vec, int32_t mult);

bool ag_vec2i_point_in_rect(struct ag_vec2i point, struct ag_vec2i rect_pos, struct ag_vec2i rect_size);

#endif