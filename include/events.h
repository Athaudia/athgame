#ifndef GUARD_AG_EVENTS_H
#define GUARD_AG_EVENTS_H

#include "vec.h"

enum ag_event_type
{
	AG_EVENT_CLOSE, AG_EVENT_MOUSE_DOWN, AG_EVENT_MOUSE_UP
};

struct ag_event
{
	struct ag_window* window;
	enum ag_event_type type;
	union
	{
		struct
		{
			struct ag_vec2i mouse_pos;
		};
		void* data;
	};
};

struct ag_event_queue_item
{
	struct ag_event* event;
	struct ag_event_queue_item* next;
};

struct ag_event* ag_event_close_new(struct ag_window* window);
struct ag_event* ag_event_mouse_down_new(struct ag_window* window);
struct ag_event* ag_event_mouse_up_new(struct ag_window* window);

void ag_event_destroy(struct ag_event* event);
void ag_event_push(struct ag_event* event);
struct ag_event* ag_event_get();

extern struct ag_event_queue_item* ag_event_queue;

#endif
