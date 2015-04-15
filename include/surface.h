#pragma once
#include <stdint.h>
#include "vec.h"
#include <ft2build.h>
#include FT_FREETYPE_H

struct ag_font;

struct ag_color
{
	uint8_t b, g, r, a;
};

extern struct ag_color agc_white, agc_black;

struct ag_color ag_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
struct ag_color ag_hue_shift(struct ag_color color, float hue);

struct ag_surface
{
	struct ag_vec2i size;
	struct ag_color* data;
};

enum ag_filter
{
	AG_FILTER_UP2_NN, AG_FILTER_UP2_LIN, AG_FILTER_UP2_RGB, AG_FILTER_UP2_SCALE2X,
	AG_FILTER_UP3_NN,
	AG_FILTER_UP4_NN,
	AG_FILTER_DOWN2_TOPLEFT, AG_FILTER_DOWN2_AVG,
	AG_FILTER_HUESHIFT
};

struct ag_filtered_surface
{
	struct ag_surface* input; //not owned
	int filter_count;
	enum ag_filter* filters;
	double scale;
	struct ag_surface** filter_surfaces;
	struct ag_surface* output;
};


struct ag_surface* ag_surface_new(struct ag_vec2i size);
struct ag_surface* ag_surface_new_from_file(char* fname);
void ag_surface_destroy(struct ag_surface* surface);

void ag_surface_clear(struct ag_surface* surface, struct ag_color color);
void ag_surface_blit_to(struct ag_surface* dst, struct ag_surface* src, struct ag_vec2i dst_pos);
void ag_surface_blit_partial_to(struct ag_surface* dst, struct ag_surface* src, struct ag_vec2i dst_pos, struct ag_vec2i src_pos, struct ag_vec2i src_size);
void ag_surface_blit_clipped_to(struct ag_surface* dst, struct ag_surface* src, struct ag_vec2i dst_pos, struct ag_vec2i clip_pos, struct ag_vec2i clip_size);
void ag_surface_blit_with_alphachan_as_color_to(struct ag_surface* dst, struct ag_surface* src, struct ag_vec2i dst_pos, struct ag_color color);


struct ag_filtered_surface* ag_filtered_surface_new(struct ag_surface* input);
void ag_filtered_surface_destroy(struct ag_filtered_surface* surface);
void ag_filtered_surface_push(struct ag_filtered_surface* surface, enum ag_filter filter);
void ag_filtered_surface_update(struct ag_filtered_surface* surface);

//surface_filters.c
void ag_surface_filter_to(struct ag_surface* dst, struct ag_surface* src, enum ag_filter scaler);
double ag_filter_get_scale(enum ag_filter filter);

//surface_drawing.c
void ag_surface_fill_rect(struct ag_surface* surface, struct ag_vec2i pos, struct ag_vec2i size, struct ag_color color);
void ag_surface_draw_rect(struct ag_surface* surface, struct ag_vec2i pos, struct ag_vec2i size, struct ag_color color);
void ag_surface_draw_line(struct ag_surface* surface, struct ag_vec2i start, struct ag_vec2i end, struct ag_color color);
void ag_surface_draw_line_aa(struct ag_surface* surface, struct ag_vec2i start, struct ag_vec2i end, struct ag_color color);

//font.c
void ag_surface_draw_text(struct ag_surface* surface, struct ag_font* font, struct ag_vec2i pos, struct ag_color color, char* text);
void ag_surface_draw_text_centered(struct ag_surface* surface, struct ag_font* font, struct ag_vec2i pos, struct ag_vec2i size, struct ag_color color, char* text);
struct ag_surface* ag_surface_new_from_ft_bitmap(FT_Bitmap* bitmap);
