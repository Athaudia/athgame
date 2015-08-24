#include "state.h"
#include "events.h"
#include <stdio.h>

struct ag_state* ag_state_current;

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
	ag_state_current = state;
	ag_state_current->data = ag_state_current->enter();
	state->last_tick = ag_get_time()-1.0/ag_state_current->target_fps;
	state->last_sec = ag_get_time();
	state->frames_since_last_sec = -1;
	while(ag_state_current)
	{
		ag_state_run_inner(ag_state_current);
	}
}

void ag_state_run_inner(struct ag_state* state)
{
	double now = ag_get_time();
	if(now-state->last_tick >= 10.0/ag_state_current->target_fps) //skip detected of atleast 10 frames
	{
		printf("skip detected\n");
		state->last_tick = ag_get_time()-1.0/ag_state_current->target_fps;
	}

	if(now-state->last_tick >= 1.0/ag_state_current->target_fps)
	{
		ag_state_current->update(ag_state_current->data, ag_state_current->window);
		if(!ag_state_current)
			return;
		ag_state_current->render(ag_state_current->data, ag_state_current->window);
		ag_window_update(ag_state_current->window);
		state->last_tick += 1.0/ag_state_current->target_fps;
		++state->frames_since_last_sec;
	}
	else
		ag_sleep(1);
	
	if(now-state->last_sec >= 1.0)
	{
		printf("fps: %i/%.0f\n", state->frames_since_last_sec, ag_state_current->target_fps);
		state->frames_since_last_sec = 0;
		state->last_sec += 1.0;
	}
}

void ag_state_pop()
{
	//todo: actually pop instead of kill
	ag_state_current->exit(ag_state_current->data);
	ag_state_current = 0;
}

void* ag_state_default_enter()
{
	return 0;
}

void ag_state_default_render(void* data, struct ag_window* window)
{
	ag_surface32_clear(window->surface, (struct ag_color32){.r=200, .g=50, .b=100, .a=255});
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
