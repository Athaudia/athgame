#ifndef GUARD_AG_EVENTS_H
#define GUARD_AG_EVENTS_H

#include "vec.h"

enum ag_event_type
{
	AG_EVENT_CLOSE, AG_EVENT_MOUSE_DOWN, AG_EVENT_MOUSE_UP, AG_EVENT_MOUSE_MOVE,
	AG_EVENT_GUI_CLICKED
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
		struct
		{
			struct ag_gui* gui;
			struct ag_gui_elem* gui_elem;
			char* id;
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
struct ag_event* ag_event_mouse_down_new(struct ag_window* window, struct ag_vec2i pos);
struct ag_event* ag_event_mouse_up_new(struct ag_window* window, struct ag_vec2i pos);
struct ag_event* ag_event_mouse_move_new(struct ag_window* window, struct ag_vec2i pos);
struct ag_event* ag_event_gui_clicked_new(struct ag_gui* gui, struct ag_gui_elem* elem, char* id);

void ag_event_destroy(struct ag_event* event);
void ag_event_push(struct ag_event* event);
struct ag_event* ag_event_get();

extern struct ag_event_queue_item* ag_event_queue;

#endif
