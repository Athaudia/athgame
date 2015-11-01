#pragma once
#include <stdint.h>
#include "vec.h"
//#include <ft2build.h>
//#include FT_FREETYPE_H

struct ag_font;

struct ag_color32
{
	uint8_t b, g, r, a;
};

extern struct ag_color32 agc_white, agc_black, agc_magic_pink, agc_transparent;

struct ag_color32 ag_color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
struct ag_color32 ag_color32__hue_shift(struct ag_color32 color, float hue);
bool ag_color32__is_equal(struct ag_color32 a, struct ag_color32 b);

struct ag_surface32
{
	struct ag_vec2i size;
	struct ag_color32* data;
};

enum ag_filter
{
	AG_FILTER_UP2_NN, AG_FILTER_UP2_LIN, AG_FILTER_UP2_RGB, AG_FILTER_UP2_SCALE2X,
	AG_FILTER_UP3_NN,
	AG_FILTER_UP4_NN,
	AG_FILTER_DOWN2_TOPLEFT, AG_FILTER_DOWN2_AVG,
	AG_FILTER_HUESHIFT
};

struct ag_filtered_surface32
{
	struct ag_surface32* input; //not owned
	int filter_count;
	enum ag_filter* filters;
	double scale;
	struct ag_surface32** filter_surfaces;
	struct ag_surface32* output;
};


struct ag_surface32* ag_surface32__new(struct ag_vec2i size);
struct ag_surface32* ag_surface32__new_from_file(char* fname);
struct ag_surface32* ag_surface32__new_from_file_with_color_key(char* fname, struct ag_color32);
void ag_surface32__destroy(struct ag_surface32* surface);

void ag_surface32__clear(struct ag_surface32* surface, struct ag_color32 color);
void ag_surface32__blit_to(struct ag_surface32* dst, struct ag_surface32* src, struct ag_vec2i dst_pos);
void ag_surface32__blit_partial_to(struct ag_surface32* dst, struct ag_surface32* src, struct ag_vec2i dst_pos, struct ag_vec2i src_pos, struct ag_vec2i src_size);
void ag_surface32__blit_clipped_to(struct ag_surface32* dst, struct ag_surface32* src, struct ag_vec2i dst_pos, struct ag_vec2i clip_pos, struct ag_vec2i clip_size);
void ag_surface32__blit_with_alphachan_as_color_to(struct ag_surface32* dst, struct ag_surface32* src, struct ag_vec2i dst_pos, struct ag_color32 color);


struct ag_filtered_surface32* ag_filtered_surface32__new(struct ag_surface32* input);
void ag_filtered_surface32__destroy(struct ag_filtered_surface32* surface);
void ag_filtered_surface32__push(struct ag_filtered_surface32* surface, enum ag_filter filter);
void ag_filtered_surface32__update(struct ag_filtered_surface32* surface);
void ag_filtered_surface32__replace_input(struct ag_filtered_surface32* surface, struct ag_surface32* input, bool destroy_input);
void ag_filtered_surface32__reset(struct ag_filtered_surface32* surface);

//surface_filters.c
void ag_surface32__filter_to(struct ag_surface32* dst, struct ag_surface32* src, enum ag_filter scaler);
double ag_filter__get_scale(enum ag_filter filter);

//surface_drawing.c
void ag_surface32__fill_rect(struct ag_surface32* surface, struct ag_vec2i pos, struct ag_vec2i size, struct ag_color32 color);
void ag_surface32__draw_rect(struct ag_surface32* surface, struct ag_vec2i pos, struct ag_vec2i size, struct ag_color32 color);
void ag_surface32__draw_line(struct ag_surface32* surface, struct ag_vec2i start, struct ag_vec2i end, struct ag_color32 color);
void ag_surface32__draw_line_aa(struct ag_surface32* surface, struct ag_vec2i start, struct ag_vec2i end, struct ag_color32 color);

//font.c
void ag_surface32__draw_text(struct ag_surface32* surface, struct ag_font* font, struct ag_vec2i pos, struct ag_color32 color, char* text);
void ag_surface32__draw_text_centered(struct ag_surface32* surface, struct ag_font* font, struct ag_vec2i pos, struct ag_vec2i size, struct ag_color32 color, char* text);
//struct ag_surface32* ag_surface32_new_from_ft_bitmap(FT_Bitmap* bitmap);
