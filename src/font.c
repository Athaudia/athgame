#include "font.h"
#include <math.h>
FT_Library ag_ft;

struct ag_font* ag_font_new(char* fname, int size)
{
	struct ag_font* font = (struct ag_font*)malloc(sizeof(struct ag_font));
	FT_Face face;
	FT_New_Face(ag_ft, fname, 0, &face);
	FT_Set_Char_Size(face, 0, size*64, 0, 0);
//	font->line_height_ft = size*64; //face->height acting up... workaround
	font->line_height_ft = face->height;
	font->line_height = font->line_height_ft/64;
	font->ascender = face->ascender/64;
	for(int i = 0; i < 256; ++i)
	{
		uint32_t idx = FT_Get_Char_Index(face, i);
		if(idx)
		{
			FT_Load_Glyph(face, idx, 0);
			FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			font->glyphs[i] = (struct ag_glyph*)malloc(sizeof(struct ag_glyph));
			font->glyphs[i]->surface = ag_surface32_new_from_ft_bitmap(&face->glyph->bitmap);
			font->glyphs[i]->advance = face->glyph->metrics.horiAdvance;
			font->glyphs[i]->bearing = ag_vec2i(face->glyph->metrics.horiBearingX, -face->glyph->metrics.horiBearingY);			
		}
		else
			font->glyphs[i] = 0;
	}
	FT_Done_Face(face);
	return font;
}


struct ag_font* ag_font_new_from_memory(FT_Byte* data, int data_size, int size)
{
	struct ag_font* font = (struct ag_font*)malloc(sizeof(struct ag_font));
	FT_Face face;
	FT_New_Memory_Face(ag_ft, data, data_size, 0, &face);

	FT_Set_Char_Size(face, 0, size*64, 0, 0);
//	font->line_height_ft = size*64; //face->height acting up... workaround
	font->line_height_ft = face->height;
	font->line_height = font->line_height_ft/64;
	font->ascender = face->ascender/64;
	for(int i = 0; i < 256; ++i)
	{
		uint32_t idx = FT_Get_Char_Index(face, i);
		if(idx)
		{
			FT_Load_Glyph(face, idx, 0);
			FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			font->glyphs[i] = (struct ag_glyph*)malloc(sizeof(struct ag_glyph));
			font->glyphs[i]->surface = ag_surface32_new_from_ft_bitmap(&face->glyph->bitmap);
			font->glyphs[i]->advance = face->glyph->metrics.horiAdvance;
			font->glyphs[i]->bearing = ag_vec2i(face->glyph->metrics.horiBearingX, -face->glyph->metrics.horiBearingY);			
		}
		else
			font->glyphs[i] = 0;
	}
	FT_Done_Face(face);
	return font;
}

void ag_font_destroy(struct ag_font* font)
{
	for(int i = 0; i < 256; ++i)
		free(font->glyphs[i]);
	free(font);
}

void ag_surface32_draw_text(struct ag_surface32* surface, struct ag_font* font, struct ag_vec2i pos, struct ag_color32 color, char* text)
{
	struct ag_vec2i start = ag_vec2i_mult(pos, 64);
	struct ag_vec2i p = start;
	while(*text)
	{
		if(*text == '\n')
		{
			start.y += font->line_height_ft;
			p = start;
		}
		else if(font->glyphs[(uint8_t)*text])
		{
			ag_surface32_blit_with_alphachan_as_color_to(surface, font->glyphs[(uint8_t)*text]->surface,  ag_vec2i_div(ag_vec2i_add(p, font->glyphs[(uint8_t)*text]->bearing), 64), color);
			p.x += font->glyphs[(uint8_t)*text]->advance;
		}

		++text;
	}
}

void ag_surface32_draw_text_centered(struct ag_surface32* surface, struct ag_font* font, struct ag_vec2i pos, struct ag_vec2i size, struct ag_color32 color, char* text)
{
	struct ag_vec2i text_size = ag_font_text_size(font, text);
	pos = ag_vec2i_sub(ag_vec2i_add(pos, ag_vec2i_div(size,2)), ag_vec2i_div(text_size,2));
	pos.y += font->ascender;
	ag_surface32_draw_text(surface, font, pos, color, text);
}


struct ag_surface32* ag_surface32_new_from_ft_bitmap(FT_Bitmap* bitmap)
{
	struct ag_surface32* surface = ag_surface32_new(ag_vec2i(bitmap->width, bitmap->rows));
	switch(bitmap->pixel_mode)
	{
	case FT_PIXEL_MODE_GRAY:
		for(int y = 0; y < surface->size.h; ++y)
			for(int x = 0; x < surface->size.w; ++x)
			{
				uint8_t p = bitmap->buffer[x+y*bitmap->pitch];
				surface->data[x+y*surface->size.w] = (struct ag_color32){.r=0,.g=0,.b=0,.a=p};
			}
		break;
	default:
		fprintf(stderr, "unsupported font pixel_mode\n");
	}
	return surface;
}

struct ag_vec2i ag_font_text_size(struct ag_font* font, char* text)
{
	int x = 0;
	struct ag_vec2i size = ag_vec2i(0, font->line_height_ft);
	while(*text)
	{
		if(*text == '\n')
		{
			size.y += font->line_height_ft;
			x = 0;
		}
		else if(font->glyphs[(uint8_t)*text])
		{
			x += font->glyphs[(uint8_t)*text]->advance;
			if(x > size.x)
				size.x = x;
		}

		++text;
	}
	return ag_vec2i_div(size, 64);
}
