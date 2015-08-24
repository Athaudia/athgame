#ifndef GUARD_STATE_H
#define GUARD_STATE_H
#include "window.h"

void* ag_state_default_enter();
void ag_state_default_render(void* data, struct ag_window* window);
void ag_state_default_update(void* data, struct ag_window* window);
void ag_state_default_exit(void* data);

struct ag_state
{
	struct ag_window* window;
	double target_fps;
	void* data;
	void* (*enter)();
	void (*render)(void* data, struct ag_window* window);
	void (*update)(void* data, struct ag_window* window);
	void (*exit)(void* data);

	double last_tick, last_sec;
	int frames_since_last_sec;
};

struct ag_state* ag_state_new(struct ag_window* window, double target_fps,
                              void* (*enter)(),
                              void (*render)(void* data, struct ag_window* window),
                              void (*update)(void* data, struct ag_window* window),
                              void (*exit)(void* data));

void ag_state_run(struct ag_state* state);
void ag_state_run_inner(struct ag_state* state);

void ag_state_pop();

extern struct ag_state* ag_state_current;

#endif
