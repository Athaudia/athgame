#include "window.h"
#include <stdlib.h>


void ag_window_reinit_surfaces(struct ag_window* window)
{
	printf("ws: %i", window->size.w);
	if(window->surface)
		ag_surface32_destroy(window->surface);
	if(window->filtered_surface)
		ag_filtered_surface32_destroy(window->filtered_surface);

	window->surface = ag_surface32_new(window->size);
	window->filtered_surface = ag_filtered_surface32_new(window->surface);
	for(int i = 0; i < window->filter_count; ++i)
		ag_filtered_surface32_push(window->filtered_surface, window->filters[i]);
}

struct ag_window* ag_window_new(struct ag_vec2i size, bool resizeable)
{
	struct ag_window* window = (struct ag_window*)malloc(sizeof(struct ag_window));
	window->mouse_pos.x = 0;
	window->mouse_pos.y = 0;
	window->size = size;
	window->surface = 0;
	window->filtered_surface = 0;
	window->filter_count = 0;
	window->filters = 0;

	ag_window_reinit_surfaces(window);

	ag_platform_window_internal_init(&window->internal, window, size, resizeable);
	return window;
}

void ag_window_destroy(struct ag_window* window)
{
	ag_surface32_destroy(window->surface);
	//todo: add platform stuff
	free(window);
}

void ag_window_update(struct ag_window* window)
{
	ag_filtered_surface32_update(window->filtered_surface);
	ag_platform_window_update(window);
}

void ag_window_resize(struct ag_window* window, struct ag_vec2i size)
{
	window->size = size;
	//ag_window_reinit_surfaces(window);
	ag_platform_window_resize(window);
}

void ag_window_add_filter(struct ag_window* window, enum ag_filter filter)
{
	++window->filter_count;
	window->filters = (enum ag_filter*)realloc(window->filters, sizeof(enum ag_filter*)*window->filter_count);
	window->filters[window->filter_count-1] = filter;
	ag_window_reinit_surfaces(window);
	ag_platform_window_resize(window);
}
