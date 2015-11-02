#include "surface.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct ag_color32 agc_white, agc_black, agc_magic_pink, agc_transparent;

struct ag_color32 ag_color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return (struct ag_color32){.r=r, .g=g, .b=b, .a=a};
}

bool ag_color32__is_equal(struct ag_color32 a, struct ag_color32 b)
{
	return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
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

/**
 * Creates a new surface of specified size.
 * \memberof ag_surface32
 * @param size Size of the new surface.
 * @return The new surface.
 */
struct ag_surface32* ag_surface32__new(struct ag_vec2i size)
{
	struct ag_surface32* surface = (struct ag_surface32*)malloc(sizeof(struct ag_surface32));
	surface->data = (struct ag_color32*)malloc(sizeof(struct ag_color32) * ag_vec2i__prod(size));
	surface->size = size;
	return surface;
}

#pragma pack(push, 1)
/**
 * \private
 */
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

/**
 * Loads a file into a surface.
 * 
 * Currently only a subset of .bmp supported.
 * \memberof ag_surface32
 * @param fname Name of the file.
 * @return The new surface.
 */
struct ag_surface32* ag_surface32__new_from_file(char* fname)
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
	struct ag_surface32* surface = ag_surface32__new(ag_vec2i(header.width, header.height));
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

/**
 * Loads a file into a surface, replacing color_key with agc_transparent;
 * 
 * Currently only a subset of .bmp supported.
 * \memberof ag_surface32
 * @param fname Name of the file.
 * @param color_key Color to be replaced with agc_transparent.
 * @return The new surface.
 */
struct ag_surface32* ag_surface32__new_from_file_with_color_key(char* fname, struct ag_color32 color_key)
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
	struct ag_surface32* surface = ag_surface32__new(ag_vec2i(header.width, header.height));
	for(int y = 0; y < header.height; ++y)
		for(int x = 0; x < header.width; ++x)
		{
			struct ag_color32 col = ag_color32(data[x*3+y*rowsize+2], data[x*3+y*rowsize+1], data[x*3+y*rowsize], 255);
			if(ag_color32__is_equal(col, color_key))
				surface->data[x+(surface->size.h-y-1)*surface->size.w] = agc_transparent;
			else
				surface->data[x+(surface->size.h-y-1)*surface->size.w] = col;
		}

	return surface;
}


/**
 * Destroys the surface.
 * \memberof ag_surface32
 * @param surface The surface to destroy.
 */
void ag_surface32__destroy(struct ag_surface32* surface)
{
	free(surface->data);
	free(surface);
}

/**
 * Clears/Fills the surface with a given color.
 * \memberof ag_surface32
 * @param surface The surface to clear.
 * @param color The color that the surface will be after the operation.
 */
void ag_surface32__clear(struct ag_surface32* surface, struct ag_color32 color)
{
	struct ag_color32* data = surface->data;
	if(ag_vec2i__prod(surface->size) % 16 == 0)
		for(int i = 0; i < ag_vec2i__prod(surface->size)/16; ++i)
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
		for(int i = 0; i < ag_vec2i__prod(surface->size); ++i)
			*(data++) = color;
}

/**
 * Draws one surface onto another, ignoring alpha or key color.
 * \memberof ag_surface32
 * @param dst The target surface.
 * @param src The surface that will be drawn onto the target surface.
 * @param dst_pos The position on the target surface.
 */
void ag_surface32__blit_to(struct ag_surface32* dst, struct ag_surface32* src, struct ag_vec2i dst_pos)
{
	ag_surface32__blit_partial_to(dst, src, dst_pos, ag_vec2i(0,0), src->size);
}

/**
 * Draws a sub rectangle of one surface onto another, ignoring alpha or key color.
 * \memberof ag_surface32
 * @param dst The target surface.
 * @param src The surface that will be drawn onto the target surface.
 * @param dst_pos The position on the target surface.
 * @param src_pos The position of the sub rectangle on the source surface.
 * @param src_size The size of the sub rectangle on the source surface.
 */
void ag_surface32__blit_partial_to(struct ag_surface32* dst, struct ag_surface32* src, struct ag_vec2i dst_pos, struct ag_vec2i src_pos, struct ag_vec2i src_size)
{
	if(src_size.x > src->size.x - src_pos.x)
		src_size.x = src->size.x - src_pos.x;
	if(src_size.y > src->size.y - src_pos.y)
		src_size.y = src->size.y - src_pos.y;
	for(int y = 0; y < src_size.h; ++y)
		memcpy(dst->data+dst_pos.x+(dst_pos.y+y)*dst->size.w, src->data+src_pos.x+(y+src_pos.y)*src->size.w, src_size.w*4);
}

/**
 * Draws one surface onto another, only drawing inside the target's clipping rectangle, ignoring alpha or key color.
 * \memberof ag_surface32
 * @param dst The target surface.
 * @param src The surface that will be drawn onto the target surface.
 * @param dst_pos The position on the target surface.
 * @param clip_pos The position of the clipping rectangle on the target surface.
 * @param clip_size The size of the clipping rectangle on the target surface.
 */
void ag_surface32__blit_clipped_to(struct ag_surface32* dst, struct ag_surface32* src, struct ag_vec2i dst_pos, struct ag_vec2i clip_pos, struct ag_vec2i clip_size)
{
	struct ag_vec2i src_pos = ag_vec2i(0,0);
	struct ag_vec2i src_size = src->size;
	if(clip_pos.x > dst_pos.x)
	{
		src_pos.x = clip_pos.x - dst_pos.x;
		dst_pos.x = clip_pos.x;
	}
	else
		clip_size.x -= dst_pos.x-clip_pos.x;
	if(clip_pos.y > dst_pos.y)
	{
		src_pos.y = clip_pos.y - dst_pos.y;
		dst_pos.y = clip_pos.y;
	}
	else
		clip_size.y -= dst_pos.y-clip_pos.y;

	 //image size

	 //clip size from img pos
	if(src_size.x > clip_size.x)
		src_size.x = clip_size.x;
	if(src_size.y > clip_size.y)
		src_size.y = clip_size.y;
	if(src_size.x > 0 && src_size.y > 0)
		ag_surface32__blit_partial_to(dst, src, dst_pos, src_pos, src_size);
}

/**
 * Draws one surface onto another, using color key for transparency.
 * \memberof ag_surface32
 * @param dst The target surface.
 * @param src The surface that will be drawn onto the target surface.
 * @param dst_pos The position on the target surface.
 * @param color_key The color which will be considered transparent.
 */
void ag_surface32__blit_with_color_key_to(struct ag_surface32* dst, struct ag_surface32* src, struct ag_vec2i dst_pos, struct ag_color32 color_key)
{
	ag_surface32__blit_partial_with_color_key_to(dst, src, dst_pos, ag_vec2i(0,0), src->size, color_key);
}

/**
 * Draws a sub rectangle of one surface onto another, using color key for transparency.
 * \memberof ag_surface32
 * @param dst The target surface.
 * @param src The surface that will be drawn onto the target surface.
 * @param dst_pos The position on the target surface.
 * @param src_pos The position of the sub rectangle on the source surface.
 * @param src_size The size of the sub rectangle on the source surface.
 * @param color_key The color which will be considered transparent.
 */
void ag_surface32__blit_partial_with_color_key_to(struct ag_surface32* dst, struct ag_surface32* src, struct ag_vec2i dst_pos, struct ag_vec2i src_pos, struct ag_vec2i src_size, struct ag_color32 color_key)
{
	if(src_size.x > src->size.x - src_pos.x)
		src_size.x = src->size.x - src_pos.x;
	if(src_size.y > src->size.y - src_pos.y)
		src_size.y = src->size.y - src_pos.y;
	for(int y = 0; y < src_size.h; ++y)
		for(int x = 0; x < src_size.w; ++x)
		{
			struct ag_color32 col = src->data[src_pos.x+x+(src_pos.y+y)*src->size.w];
			if(!ag_color32__is_equal(col, color_key))
				dst->data[dst_pos.x+x+(dst_pos.y+y)*dst->size.w] = col;
		}
}

/**
 * Draws one surface onto another, only drawing inside the target's clipping rectangle, using color key for transparency.
 * \memberof ag_surface32
 * @param dst The target surface.
 * @param src The surface that will be drawn onto the target surface.
 * @param dst_pos The position on the target surface.
 * @param clip_pos The position of the clipping rectangle on the target surface.
 * @param clip_size The size of the clipping rectangle on the target surface.
 * @param color_key The color which will be considered transparent.
 */
void ag_surface32__blit_clipped_with_color_key_to(struct ag_surface32* dst, struct ag_surface32* src, struct ag_vec2i dst_pos, struct ag_vec2i clip_pos, struct ag_vec2i clip_size, struct ag_color32 color_key)
{
	struct ag_vec2i src_pos = ag_vec2i(0,0);
	struct ag_vec2i src_size = src->size;
	if(clip_pos.x > dst_pos.x)
	{
		src_pos.x = clip_pos.x - dst_pos.x;
		dst_pos.x = clip_pos.x;
	}
	else
		clip_size.x -= dst_pos.x-clip_pos.x;
	if(clip_pos.y > dst_pos.y)
	{
		src_pos.y = clip_pos.y - dst_pos.y;
		dst_pos.y = clip_pos.y;
	}
	else
		clip_size.y -= dst_pos.y-clip_pos.y;

	 //image size

	 //clip size from img pos
	if(src_size.x > clip_size.x)
		src_size.x = clip_size.x;
	if(src_size.y > clip_size.y)
		src_size.y = clip_size.y;
	if(src_size.x > 0 && src_size.y > 0)
		ag_surface32__blit_partial_with_color_key_to(dst, src, dst_pos, src_pos, src_size, color_key);
}

/**
 * Draws one surface onto another, ignoring the source image's color, and instead alpha blending(using the source image's alpha channel) the given color onto it.
 * \memberof ag_surface32
 * @param dst The target surface.
 * @param src The surface that will be drawn onto the target surface.
 * @param dst_pos The position on the target surface.
 * @param color The color to draw.
 */
void ag_surface32__blit_with_alphachan_as_color_to(struct ag_surface32* dst, struct ag_surface32* src, struct ag_vec2i dst_pos, struct ag_color32 color)
{
	for(int y = 0; y < src->size.h; ++y)
		for(int x = 0; x < src->size.w; ++x)
		{
			int a = src->data[x+y*src->size.w].a;
			struct ag_color32* d = &dst->data[(x+dst_pos.x)+(y+dst_pos.y)*dst->size.w];
			d->r = ((color.r*a) + (d->r*(255-a))) >> 8;
			d->g = ((color.g*a) + (d->g*(255-a))) >> 8;
			d->b = ((color.b*a) + (d->b*(255-a))) >> 8;
		}
}

/**
 * Creates a new filtered surface using input.
 * \memberof filtered_surface32
 * @param input The surface that is going to be filtered.
 * @return The new filtered surface.
 */
struct ag_filtered_surface32* ag_filtered_surface32__new(struct ag_surface32* input)
{
	struct ag_filtered_surface32* surface = (struct ag_filtered_surface32*)malloc(sizeof(struct ag_filtered_surface32));
	surface->input = input;
	surface->scale = 1.0;
	surface->output = input;
	surface->filter_count = 0;
	surface->filters = 0;
	surface->filter_surfaces = (struct ag_surface32**)(malloc(sizeof(struct ag_surface32*)*1));
	surface->filter_surfaces[0] = input;
	return surface;
}

/**
 * Destroys the filtered surface.
 * \memberof filtered_surface32
 * @param surface The filtered surface to destroy.
 */
void ag_filtered_surface32__destroy(struct ag_filtered_surface32* surface)
{
	for(int i = 0; i < surface->filter_count; ++i)
		ag_surface32__destroy(surface->filter_surfaces[i+1]); //1st is input, array is one larger than count
	free(surface->filters);
	free(surface->filter_surfaces);
	free(surface);
}

/**
 * Adds a new filter to the filter stack.
 * \memberof filtered_surface32
 * @param surface The surface which to add the filter to.
 * @param filter The filter to add.
 */
void ag_filtered_surface32__push(struct ag_filtered_surface32* surface, enum ag_filter filter)
{
	++surface->filter_count;
	surface->filters = (enum ag_filter*)realloc(surface->filters, sizeof(enum ag_filter*)*surface->filter_count);
	surface->filter_surfaces = (struct ag_surface32**)realloc(surface->filter_surfaces, sizeof(struct ag_surface32*)*(1+surface->filter_count));
	surface->filters[surface->filter_count-1] = filter;
	double new_mult = ag_filter__get_scale(filter);
	struct ag_surface32* old = surface->filter_surfaces[surface->filter_count-1];
	//surface->filter_surfaces[surface->filter_count] = ag_surface32_new(ag_vec2i_mult(old->size, new_mult));
	surface->filter_surfaces[surface->filter_count] = ag_surface32__new(ag_vec2i(old->size.x*new_mult, old->size.y*new_mult));
	surface->scale *= new_mult;
	surface->output = surface->filter_surfaces[surface->filter_count];
}

/**
 * Updates the output of the filtered surface.
 * \memberof filtered_surface32
 * @param surface The surface to update.
 */
void ag_filtered_surface32__update(struct ag_filtered_surface32* surface)
{
	for(int i = 0; i < surface->filter_count; ++i)
		ag_surface32__filter_to(surface->filter_surfaces[i+1], surface->filter_surfaces[i], surface->filters[i]);
}

/**
 * Replaces the input surface of a filtered surface with a different one.
 * \memberof filtered_surface32
 * @param surface       The filtered surface.
 * @param input         The new input surface.
 * @param destroy_input Call ag_surface32_destroy on the old input surface?
 */
void ag_filtered_surface32__replace_input(struct ag_filtered_surface32* surface, struct ag_surface32* input, bool destroy_input)
{
	if(destroy_input)
		ag_surface32__destroy(surface->filter_surfaces[0]);
	for(int i = 1; i < surface->filter_count+1; ++i)
		ag_surface32__destroy(surface->filter_surfaces[i]);
	surface->input = input;
	surface->filter_surfaces[0] = input;
	for(int i = 0; i < surface->filter_count; ++i)
	{
		double new_mult = ag_filter__get_scale(surface->filters[i]);
		surface->filter_surfaces[i+1] = ag_surface32__new(ag_vec2i(surface->filter_surfaces[i]->size.x*new_mult, surface->filter_surfaces[i]->size.y*new_mult));
	}
	surface->output = surface->filter_surfaces[surface->filter_count];
}

/**
 * Removes all filters, making the filtered surface a 1:1 representation
 * @param surface The Filtered surface
 */
void ag_filtered_surface32__reset(struct ag_filtered_surface32* surface)
{
	for(int i = 0; i < surface->filter_count; ++i)
		ag_surface32__destroy(surface->filter_surfaces[i+1]);
	surface->filter_count = 0;
	surface->filters = (enum ag_filter*)realloc(surface->filters, sizeof(enum ag_filter*)*surface->filter_count);
	surface->filter_surfaces = (struct ag_surface32**)realloc(surface->filter_surfaces, sizeof(struct ag_surface32*)*(1+surface->filter_count));

	surface->scale = 1;
	surface->filter_surfaces[0] = surface->input;
	surface->output = surface->input;
}

/**
 * \private
 */
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

/** http://www.cs.rit.edu/~ncs/color/t_convert.html
 * \private
 */
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

/**
 * \private
 */
struct ag_color32 ag_color32__hue_shift(struct ag_color32 color, float hue)
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
	return (struct ag_color32){.r = (uint8_t)(rf*255), .g = (uint8_t) (gf*255), .b = (uint8_t)(bf*255), .a = color.a};
}
