#include <windows.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "window.h"
#include "events.h"
#include "athgame.h"
#include "state.h"
#include "font.h"
#include "gui.h"
#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H



struct mystate
{
	struct ag_surface* bg;
	struct ag_font* font;
	struct ag_gui* gui;
	int tick;
};

void* mystate_enter()
{

	struct mystate* mystate = (struct mystate*)malloc(sizeof(struct mystate));
	mystate->bg = ag_surface_new_from_file("mario.bmp");
	mystate->tick = 0;
	mystate->gui = ag_gui_new_from_file("test.agg");
	return mystate;
}

void mystate_render(void* data, struct ag_window* window)
{
	ag_surface_clear(window->surface, ag_color(30,21,24,255));
	
	struct mystate* mystate = (struct mystate*)data;

	mystate->gui->size = window->surface->size;
	mystate->gui->pos = ag_vec2i(0,0);
	ag_gui_manage_layout(mystate->gui);
	ag_surface_draw_gui(window->surface, mystate->gui);
	//ag_surface_blit_to(window->surface, mystate->bg, ag_vec2i(0,0));
	//ag_surface_blit_partial_to(window->surface, mystate->bg, ag_vec2i(0,0), ag_vec2i(16,16), ag_vec2i(320,240));
	//ag_surface_blit_partial_to(window->surface, mystate->bg, ag_vec2i(270,0), ag_vec2i(16,16), ag_vec2i(48,16));
	//ag_surface_blit_clipped_to(window->surface, mystate->bg, window->mouse_pos, ag_vec2i(160,160), ag_vec2i(48,16));
	//ag_surface_fill_rect(window->surface, ag_vec2i(20,20), ag_vec2i(1,mystate->font->line_height), (struct ag_color){255,255,255,255});
}

void mystate_update(void* data, struct ag_window* window)
{
	struct mystate* mystate = (struct mystate*)data;

	struct ag_event* event;
	while((event = ag_event_get()))
	{
		switch(event->type)
		{
		case AG_EVENT_CLOSE:
			ag_state_pop();
			break;
		case AG_EVENT_GUI_CLICKED:
			printf("event: %s\n", event->id);
			if(strcmp("large", event->id) == 0)
				ag_window_resize(window, ag_vec2i(320*3,240*3));
			else if(strcmp("small", event->id) == 0)
				ag_window_resize(window, ag_vec2i(320,240));
			else if(strcmp("quit", event->id) == 0)
				ag_state_pop();
			break;
		default:
			ag_gui_process_event(mystate->gui, event);
		}
	}

	mystate->tick++;
	//if(mystate->tick)
	//	ag_window_resize(window, ag_vec2i_add(window->size, ag_vec2i(1,1)));
}


int main()
{
	ag_init();
	struct ag_window* window = ag_window_new(ag_vec2i(320, 240), true);
	ag_window_add_filter(window, AG_FILTER_UP2_SCALE2X);
	struct ag_state* state = ag_state_new(window, 60, mystate_enter, mystate_render, mystate_update, 0);


	ag_state_run(state);

	ag_uninit();
	return 0;
}
