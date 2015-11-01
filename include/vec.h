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

struct ag_vec2f
{
	union
	{
		float x, w;
	};
	union
	{
		float y, h;
	};
};


struct ag_vec2i ag_vec2i(int32_t x, int32_t y);
struct ag_vec2i ag_vec2i__add(struct ag_vec2i a, struct ag_vec2i b);
struct ag_vec2i ag_vec2i__sub(struct ag_vec2i a, struct ag_vec2i b);
struct ag_vec2i ag_vec2i__div(struct ag_vec2i a, int32_t b);
struct ag_vec2i ag_vec2i__div_round(struct ag_vec2i a, int32_t b);
struct ag_vec2i ag_vec2i__mult(struct ag_vec2i vec, int32_t mult);
int32_t ag_vec2i__prod(struct ag_vec2i);
bool ag_vec2i__point_in_rect(struct ag_vec2i point, struct ag_vec2i rect_pos, struct ag_vec2i rect_size);

struct ag_vec2f ag_vec2f(float x, float y);
struct ag_vec2f ag_vec2f__from_angle(float angle); //in turns
struct ag_vec2f ag_vec2f__add(struct ag_vec2f a, struct ag_vec2f b);
struct ag_vec2f ag_vec2f__sub(struct ag_vec2f a, struct ag_vec2f b);
struct ag_vec2f ag_vec2f__div(struct ag_vec2f a, float b);
struct ag_vec2f ag_vec2f__mult(struct ag_vec2f vec, float mult);
float ag_vec2f__prod(struct ag_vec2f);

struct ag_vec2i ag_vec2f__to_vec2i(struct ag_vec2f vec);
struct ag_vec2i ag_vec2f__to_vec2i_round(struct ag_vec2f vec);
struct ag_vec2f ag_vec2i__to_vec2f(struct ag_vec2i vec);

#endif
