#include "vec.h"

#include "math.h"

#define TAU 6.28318530718

struct ag_vec2i ag_vec2i(int32_t x, int32_t y)
{
	return (struct ag_vec2i){.x=x, .y=y};
}

struct ag_vec2i ag_vec2i__add(struct ag_vec2i a, struct ag_vec2i b)
{
	return ag_vec2i(a.x+b.x, a.y+b.y);
}

struct ag_vec2i ag_vec2i__sub(struct ag_vec2i a, struct ag_vec2i b)
{
	return ag_vec2i(a.x-b.x, a.y-b.y);
}

struct ag_vec2i ag_vec2i__div(struct ag_vec2i a, int32_t b)
{
	return ag_vec2i(a.x/b, a.y/b);
}

struct ag_vec2i ag_vec2i__div_round(struct ag_vec2i a, int32_t b)
{
	return ag_vec2i(round((double)a.x/(double)b), round((double)a.y/(double)b));
}

struct ag_vec2i ag_vec2i__mult(struct ag_vec2i vec, int32_t mult)
{
	return (struct ag_vec2i){.x=vec.x*mult, .y=vec.y*mult};
}

int32_t ag_vec2i__prod(struct ag_vec2i vec)
{
	return vec.x*vec.y;
}

bool ag_vec2i__point_in_rect(struct ag_vec2i point, struct ag_vec2i rect_pos, struct ag_vec2i rect_size)
{
	if(point.x < rect_pos.x || point.y < rect_pos.y)
		return false;
	if(point.x >= rect_pos.x+rect_size.w || point.y >= rect_pos.y+rect_size.h)
		return false;
	return true;
}





////////////////

struct ag_vec2f ag_vec2f(float x, float y)
{
	return (struct ag_vec2f){.x=x, .y=y};
}

struct ag_vec2f ag_vec2f__from_angle(float angle) //in turns
{
	return (struct ag_vec2f){.x=cos(angle*TAU), .y=sin(angle*TAU)};
}

struct ag_vec2f ag_vec2f__add(struct ag_vec2f a, struct ag_vec2f b)
{
	return ag_vec2f(a.x+b.x, a.y+b.y);
}

struct ag_vec2f ag_vec2f__sub(struct ag_vec2f a, struct ag_vec2f b)
{
	return ag_vec2f(a.x-b.x, a.y-b.y);
}

struct ag_vec2f ag_vec2f__div(struct ag_vec2f a, float b)
{
	return ag_vec2f(a.x/b, a.y/b);
}

struct ag_vec2f ag_vec2f__mult(struct ag_vec2f vec, float mult)
{
	return (struct ag_vec2f){.x=vec.x*mult, .y=vec.y*mult};
}

float ag_vec2f__prod(struct ag_vec2f vec)
{
	return vec.x*vec.y;
}


///////////////


struct ag_vec2i ag_vec2f__to_vec2i(struct ag_vec2f vec)
{
	return (struct ag_vec2i){.x=(int32_t)vec.x, .y=(int32_t)vec.y};
}

struct ag_vec2i ag_vec2f__to_vec2i_round(struct ag_vec2f vec)
{
	return (struct ag_vec2i){.x=round(vec.x), .y=round(vec.y)};
}

struct ag_vec2f ag_vec2i__to_vec2f(struct ag_vec2i vec)
{
	return (struct ag_vec2f){.x=(float)vec.x, .y=(float)vec.y};
}
