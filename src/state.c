#include "state.h"
#include "events.h"
#include <stdio.h>

static struct ag_state* current_state;

struct ag_state* ag_state_new(struct ag_window* window, double target_fps,
                              void* (*enter)(),
                              void (*render)(void* data, struct ag_window* window),
                              void (*update)(void* data, struct ag_window* window),
                              void (*exit)(void* data))
{
	struct ag_state* state = (struct ag_state*)malloc(sizeof(struct ag_state));
	state->window = window;
	state->target_fps = target_fps;
	state->enter = enter?enter:ag_state_default_enter;
	state->render = render?render:ag_state_default_render;
	state->update = update?update:ag_state_default_update;
	state->exit = exit?exit:ag_state_default_exit;
	return state;
}

void ag_state_run(struct ag_state* state)
{
	current_state = state;
	current_state->data = current_state->enter();
	double last_tick = ag_get_time()-1.0/current_state->target_fps;
	double last_sec = ag_get_time();
	int frames_since_last_sec = -1;
	while(current_state)
	{
		double now = ag_get_time();
		if(now-last_tick >= 10.0/current_state->target_fps) //skip detected of atleast 10 frames
		{
			printf("skip detected\n");
			last_tick = ag_get_time()-1.0/current_state->target_fps;
		}

		if(now-last_tick >= 1.0/current_state->target_fps)
		{
			current_state->update(current_state->data, current_state->window);
			if(!current_state)
				break;
			current_state->render(current_state->data, current_state->window);
			ag_window_update(current_state->window);
			last_tick += 1.0/current_state->target_fps;
			++frames_since_last_sec;
		}
		else
			ag_sleep(1);
		
		if(now-last_sec >= 1.0)
		{
			printf("fps: %i/%.0f\n", frames_since_last_sec, current_state->target_fps);
			frames_since_last_sec = 0;
			last_sec += 1.0;
		}
	}
}

void ag_state_pop()
{
	//todo: actually pop instead of kill
	current_state->exit(current_state->data);
	current_state = 0;
}

void* ag_state_default_enter()
{
	return 0;
}

void ag_state_default_render(void* data, struct ag_window* window)
{
	ag_surface_clear(window->surface, (struct ag_color){.r=200, .g=50, .b=100, .a=255});
}

void ag_state_default_update(void* data, struct ag_window* window)
{
	//todo: filter by window
	struct ag_event* event;
	while((event = ag_event_get()))
	{
		if(event->type == AG_EVENT_CLOSE)
			ag_state_pop();
	}
}

void ag_state_default_exit(void* data)
{

}