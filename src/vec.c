#include "vec.h"

#include "math.h"

struct ag_vec2i ag_vec2i(int32_t x, int32_t y)
{
	return (struct ag_vec2i){.x=x, .y=y};
}

struct ag_vec2i ag_vec2i_add(struct ag_vec2i a, struct ag_vec2i b)
{
	return ag_vec2i(a.x+b.x, a.y+b.y);
}

struct ag_vec2i ag_vec2i_sub(struct ag_vec2i a, struct ag_vec2i b)
{
	return ag_vec2i(a.x-b.x, a.y-b.y);
}

struct ag_vec2i ag_vec2i_div(struct ag_vec2i a, int32_t b)
{
	return ag_vec2i(a.x/b, a.y/b);
}

struct ag_vec2i ag_vec2i_div_round(struct ag_vec2i a, int32_t b)
{
	return ag_vec2i(round((double)a.x/(double)b), round((double)a.y/(double)b));
}


int32_t ag_vec2i_prod(struct ag_vec2i vec)
{
	return vec.x*vec.y;
}

struct ag_vec2i ag_vec2i_mult(struct ag_vec2i vec, int32_t mult)
{
	return (struct ag_vec2i){.x=vec.x*mult, .y=vec.y*mult};
}

bool ag_vec2i_point_in_rect(struct ag_vec2i point, struct ag_vec2i rect_pos, struct ag_vec2i rect_size)
{
	if(point.x < rect_pos.x || point.y < rect_pos.y)
		return false;
	if(point.x >= rect_pos.x+rect_size.w || point.y >= rect_pos.y+rect_size.h)
		return false;
	return true;
}
