#include "surface.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct ag_color agc_white, agc_black;

struct ag_color ag_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return (struct ag_color){.r=r, .g=g, .b=b, .a=a};
}


uint32_t ag_swap_endian_uint32(uint32_t val)
{
	uint32_t b0,b1,b2,b3;
	uint32_t res;

	b0 = (val & 0x000000ff) << 24u;
	b1 = (val & 0x0000ff00) << 8u;
	b2 = (val & 0x00ff0000) >> 8u;
	b3 = (val & 0xff000000) >> 24u;

	res = b0 | b1 | b2 | b3;
	return res;
}

struct ag_surface* ag_surface_new(struct ag_vec2i size)
{
	struct ag_surface* surface = (struct ag_surface*)malloc(sizeof(struct ag_surface));
	surface->data = (struct ag_color*)malloc(sizeof(struct ag_color) * ag_vec2i_prod(size));
	surface->size = size;
	return surface;
}

#pragma pack(push, 1)
struct bmp_header
{
	uint16_t magic;
	uint32_t file_size;
	uint16_t res1, res2;
	uint32_t pixel_data_offset;

	uint32_t dib_header_size; //only 40 supported atm
	uint32_t width, height;
	uint16_t planes; //only 1 supported
	uint16_t bitdepth; //only 24 supported
	uint32_t compression; //not supported
	uint32_t raw_data_size;
	int32_t hppm, vppm;
	uint32_t pal_count;
	uint32_t used_count;
};
#pragma pack(pop)

struct ag_surface* ag_surface_new_from_file(char* fname)
{
	//todo: check extension, and handle different formats, bmp only for now
	//todo: error handling
	FILE* fil = fopen(fname, "rb");
	struct bmp_header header;
	fread(&header, sizeof(struct bmp_header), 1, fil);

	if(header.magic != 0x4d42)
		return 0;
	if(header.dib_header_size != 40)
		return 0;
	if(header.planes != 1)
		return 0;
	if(header.bitdepth != 24)
		return 0;

	int rowsize = ((header.bitdepth*header.width+31)/32)*4;
	uint8_t* data = (uint8_t*)malloc(rowsize*header.height);
	fseek(fil, header.pixel_data_offset, SEEK_SET);
	fread(data, 1, rowsize*header.height, fil);
	fclose(fil);
	struct ag_surface* surface = ag_surface_new(ag_vec2i(header.width, header.height));
	for(int y = 0; y < header.height; ++y)
		for(int x = 0; x < header.width; ++x)
		{
			surface->data[x+(surface->size.h-y-1)*surface->size.w].b = data[x*3+y*rowsize];
			surface->data[x+(surface->size.h-y-1)*surface->size.w].g = data[x*3+y*rowsize+1];
			surface->data[x+(surface->size.h-y-1)*surface->size.w].r = data[x*3+y*rowsize+2];
			surface->data[x+(surface->size.h-y-1)*surface->size.w].a = 255;
		}

	return surface;
}


void ag_surface_destroy(struct ag_surface* surface)
{
	free(surface->data);
	free(surface);
}


void ag_surface_clear(struct ag_surface* surface, struct ag_color color)
{
	struct ag_color* data = surface->data;
	if(ag_vec2i_prod(surface->size) % 16 == 0)
		for(int i = 0; i < ag_vec2i_prod(surface->size)/16; ++i)
		{
			*(data++) = color; *(data++) = color;
			*(data++) = color; *(data++) = color;
			*(data++) = color; *(data++) = color;
			*(data++) = color; *(data++) = color;
			*(data++) = color; *(data++) = color;
			*(data++) = color; *(data++) = color;
			*(data++) = color; *(data++) = color;
			*(data++) = color; *(data++) = color;
		}
	else
		for(int i = 0; i < ag_vec2i_prod(surface->size); ++i)
			*(data++) = color;
}

void ag_surface_blit_to(struct ag_surface* dst, struct ag_surface* src, struct ag_vec2i dst_pos)
{
	ag_surface_blit_partial_to(dst, src, dst_pos, ag_vec2i(0,0), src->size);
//	for(int y = 0; y < src->size.h; ++y)
//		memcpy(dst->data+dst_pos.x+(dst_pos.y+y)*dst->size.w, src->data+y*src->size.w, src->size.w*4);
}

void ag_surface_blit_partial_to(struct ag_surface* dst, struct ag_surface* src, struct ag_vec2i dst_pos, struct ag_vec2i src_pos, struct ag_vec2i src_size)
{
	printf("ag_surface_blit_partial_to(dst, src, [%i,%i], [%i,%i], [%i,%i]\n", dst_pos.x, dst_pos.y, src_pos.x, src_pos.y, src_size.x, src_size.y);
	if(src_size.x > src->size.x - src_pos.x)
		src_size.x = src->size.x - src_pos.x;
	if(src_size.y > src->size.y - src_pos.y)
		src_size.y = src->size.y - src_pos.y;
	for(int y = 0; y < src_size.h; ++y)
		memcpy(dst->data+dst_pos.x+(dst_pos.y+y)*dst->size.w, src->data+src_pos.x+(y+src_pos.y)*src->size.w, src_size.w*4);
}

void ag_surface_blit_clipped_to(struct ag_surface* dst, struct ag_surface* src, struct ag_vec2i pos, struct ag_vec2i clip_pos, struct ag_vec2i clip_size)
{
	struct ag_vec2i src_pos = ag_vec2i(0,0);
	struct ag_vec2i src_size = src->size;
	if(clip_pos.x > pos.x)
	{
		src_pos.x = clip_pos.x - pos.x;
		pos.x = clip_pos.x;
	}
	else
		clip_size.x -= pos.x-clip_pos.x;
	if(clip_pos.y > pos.y)
	{
		src_pos.y = clip_pos.y - pos.y;
		pos.y = clip_pos.y;
	}
	else
		clip_size.y -= pos.y-clip_pos.y;

	 //image size

	 //clip size from img pos
	if(src_size.x > clip_size.x)
		src_size.x = clip_size.x;
	if(src_size.y > clip_size.y)
		src_size.y = clip_size.y;
	if(src_size.x > 0 && src_size.y > 0)
		ag_surface_blit_partial_to(dst, src, pos, src_pos, src_size);
}

void ag_surface_blit_with_alphachan_as_color_to(struct ag_surface* dst, struct ag_surface* src, struct ag_vec2i dst_pos, struct ag_color color)
{
	for(int y = 0; y < src->size.h; ++y)
		for(int x = 0; x < src->size.w; ++x)
		{
			int a = src->data[x+y*src->size.w].a;
			struct ag_color* d = &dst->data[(x+dst_pos.x)+(y+dst_pos.y)*dst->size.w];
			d->r = ((color.r*a) + (d->r*(255-a))) >> 8;
			d->g = ((color.g*a) + (d->g*(255-a))) >> 8;
			d->b = ((color.b*a) + (d->b*(255-a))) >> 8;
		}
}

struct ag_filtered_surface* ag_filtered_surface_new(struct ag_surface* input)
{
	struct ag_filtered_surface* surface = (struct ag_filtered_surface*)malloc(sizeof(struct ag_filtered_surface));
	surface->input = input;
	surface->scale = 1.0;
	surface->output = input;
	surface->filter_count = 0;
	surface->filters = 0;
	surface->filter_surfaces = (struct ag_surface**)(malloc(sizeof(struct ag_surface*)*1));
	surface->filter_surfaces[0] = input;
	return surface;
}

void ag_filtered_surface_destroy(struct ag_filtered_surface* surface)
{
	for(int i = 0; i < surface->filter_count; ++i)
		ag_surface_destroy(surface->filter_surfaces[i+1]); //1st is input, array is one larger than count
	free(surface->filters);
	free(surface->filter_surfaces);
	free(surface);
}

void ag_filtered_surface_push(struct ag_filtered_surface* surface, enum ag_filter filter)
{
	++surface->filter_count;
	surface->filters = (enum ag_filter*)realloc(surface->filters, sizeof(enum ag_filter*)*surface->filter_count);
	surface->filter_surfaces = (struct ag_surface**)realloc(surface->filter_surfaces, sizeof(struct ag_surface*)*(1+surface->filter_count));
	surface->filters[surface->filter_count-1] = filter;
	double new_mult = ag_filter_get_scale(filter);
	struct ag_surface* old = surface->filter_surfaces[surface->filter_count-1];
	surface->filter_surfaces[surface->filter_count] = ag_surface_new(ag_vec2i_mult(old->size, new_mult));
	surface->scale *= new_mult;
	surface->output = surface->filter_surfaces[surface->filter_count];
}

void ag_filtered_surface_update(struct ag_filtered_surface* surface)
{
	for(int i = 0; i < surface->filter_count; ++i)
		ag_surface_filter_to(surface->filter_surfaces[i+1], surface->filter_surfaces[i], surface->filters[i]);
}



void ag_rgb_to_hsv( float r, float g, float b, float *h, float *s, float *v )
{
	float min, max, delta;
	min = r;
	if(g < min) min = g;
	if(b < min) min = b;
	max = r;
	if(g > max) max = g;
	if(b > max) max = b;
	*v = max;				// v
	delta = max - min;
	if( max > 0 )
		*s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		//*h = -1;
		*h = 0;
		return;
	}
	if( r >= max )
		*h = ( g - b ) / delta;		// between yellow & magenta
	else if( g >= max )
		*h = 2.f + ( b - r ) / delta;	// between cyan & yellow
	else
		*h = 4.f + ( r - g ) / delta;	// between magenta & cyan
	*h *= 60.f;				// degrees
	if( *h < 0 )
		*h += 360.f;
}

//http://www.cs.rit.edu/~ncs/color/t_convert.html
void ag_hsv_to_rgb( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	if( s <= 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h /= 60.f;			// sector 0 to 5
	//i = floor( h );
	i = (int) h;
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}


struct ag_color ag_hue_shift(struct ag_color color, float hue)
{
	if(hue == 0.f)
		return color;
	float rf = color.r/255.f;
	float gf = color.g/255.f;
	float bf = color.b/255.f;

	float h, s, v;

	ag_rgb_to_hsv(rf, gf, bf, &h, &s, &v);
	//s = 0;
	h += hue;
	while(h >= 360.f)
		h -= 360.f;
	ag_hsv_to_rgb(&rf, &gf, &bf, h, s, v);
	return (struct ag_color){.r = (uint8_t)(rf*255), .g = (uint8_t) (gf*255), .b = (uint8_t)(bf*255), .a = color.a};
}
