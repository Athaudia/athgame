#ifndef AG_FONT_H
#define AG_FONT_H
//#include <ft2build.h>
//#include FT_FREETYPE_H

#include "surface.h"

struct ag_glyph
{
	//todo: replace with alpha-chan only surface when implemented
	struct ag_surface32* surface;
	struct ag_vec2i bearing;
	uint32_t advance;
};

struct ag_font
{
	struct ag_glyph* glyphs[256];
	uint32_t line_height, line_height_ft, ascender;
};

//extern FT_Library ag_ft;

//struct ag_font* ag_font_new(char* fname, int size);
//struct ag_font* ag_font_new_from_memory(FT_Byte* data, int data_size, int size);
//void ag_font_destroy(struct ag_font* font);
//struct ag_vec2i ag_font_text_size(struct ag_font*, char* text);

#endif
