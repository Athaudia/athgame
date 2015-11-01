#include "window.h"
#include <stdlib.h>
#include <stdio.h>

void ag_window__reinit_surfaces(struct ag_window* window)
{
	printf("ws: %i", window->size.w);
	if(window->surface)
		ag_surface32__destroy(window->surface);
	if(window->filtered_surface)
		ag_filtered_surface32__destroy(window->filtered_surface);

	window->surface = ag_surface32__new(window->size);
	window->filtered_surface = ag_filtered_surface32__new(window->surface);
	for(int i = 0; i < window->filter_count; ++i)
		ag_filtered_surface32__push(window->filtered_surface, window->filters[i]);
}

/**
 * Creates new window
 * \memberof ag_window
 * @param size Size of the window. (sometimes a different size than given will be set)
 * @param resizeable Should the window be resizeable? (Not avaialble on all platforms)
 * @return The new window.
 */
struct ag_window* ag_window__new(struct ag_vec2i size, bool resizeable)
{
	struct ag_window* window = (struct ag_window*)malloc(sizeof(struct ag_window));
	window->mouse_pos.x = 0;
	window->mouse_pos.y = 0;
	window->size = size;
	window->surface = 0;
	window->filtered_surface = 0;
	window->filter_count = 0;
	window->filters = 0;

	ag_window__reinit_surfaces(window);

	ag_platform_window_internal_init(&window->internal, window, size, resizeable);
	return window;
}

void ag_window__destroy(struct ag_window* window)
{
	ag_surface32__destroy(window->surface);
	//todo: add platform stuff
	free(window);
}

void ag_window__update(struct ag_window* window)
{
	ag_filtered_surface32__update(window->filtered_surface);
	ag_platform_window_update(window);
}

void ag_window__resize(struct ag_window* window, struct ag_vec2i size)
{
	window->size = size;
	//ag_window_reinit_surfaces(window);
	ag_platform_window_resize(window);
}

void ag_window__add_filter(struct ag_window* window, enum ag_filter filter)
{
	++window->filter_count;
	window->filters = (enum ag_filter*)realloc(window->filters, sizeof(enum ag_filter*)*window->filter_count);
	window->filters[window->filter_count-1] = filter;
	ag_window__reinit_surfaces(window);
	ag_platform_window_resize(window);
}
