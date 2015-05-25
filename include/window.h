#pragma once
#include "platform.h"
#include "surface.h"
#include "vec.h"

struct ag_window
{
	struct ag_surface32* surface;
	struct ag_vec2i size;
	int filter_count;
	enum ag_filter* filters;
	struct ag_filtered_surface32* filtered_surface;
	struct ag_vec2i mouse_pos;
	struct ag_platform_window_internal internal;
};

struct ag_window* ag_window_new(struct ag_vec2i size, bool resizeable);
void ag_window_destroy(struct ag_window* window);
void ag_window_update(struct ag_window* window);
void ag_window_reinit_surfaces(struct ag_window* window);
void ag_window_resize(struct ag_window* window, struct ag_vec2i size);
void ag_window_add_filter(struct ag_window* window, enum ag_filter filter);
