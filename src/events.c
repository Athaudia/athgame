#include "events.h"
#include <stdlib.h>

struct ag_event_queue_item* ag_event_queue;

static struct ag_event* last_event = 0;


struct ag_event* ag_event_close_new(struct ag_window* window)
{
	struct ag_event* event = (struct ag_event*)malloc(sizeof(struct ag_event));
	event->window = window;
	event->type = AG_EVENT_CLOSE;
	return event;
}

struct ag_event* ag_event_mouse_down_new(struct ag_window* window, struct ag_vec2i pos)
{
	struct ag_event* event = (struct ag_event*)malloc(sizeof(struct ag_event));
	event->window = window;
	event->type = AG_EVENT_MOUSE_DOWN;
	event->mouse_pos = pos;
	return event;
}

struct ag_event* ag_event_mouse_up_new(struct ag_window* window, struct ag_vec2i pos)
{
	struct ag_event* event = (struct ag_event*)malloc(sizeof(struct ag_event));
	event->window = window;
	event->type = AG_EVENT_MOUSE_UP;
	event->mouse_pos = pos;
	return event;
}

struct ag_event* ag_event_mouse_move_new(struct ag_window* window, struct ag_vec2i pos)
{
	struct ag_event* event = (struct ag_event*)malloc(sizeof(struct ag_event));
	event->window = window;
	event->type = AG_EVENT_MOUSE_MOVE;
	event->mouse_pos = pos;
	return event;
}

struct ag_event* ag_event_gui_clicked_new(struct ag_gui* gui, struct ag_gui_elem* elem, char* id)
{
	struct ag_event* event = (struct ag_event*)malloc(sizeof(struct ag_event));
	event->window = 0;
	event->type = AG_EVENT_GUI_CLICKED;
	event->gui = gui;
	event->gui_elem = elem;
	event->id = id;
	return event;
}


void ag_event_destroy(struct ag_event* event)
{
	free(event);
}

void ag_event_push(struct ag_event* event)
{
	struct ag_event_queue_item* item = (struct ag_event_queue_item*)malloc(sizeof(struct ag_event_queue_item));
	item->next = 0;
	item->event = event;
	struct ag_event_queue_item* last = ag_event_queue;
	if(last == 0)
		ag_event_queue = item;
	else
	{
		while(last->next != 0)
			last = last->next;
		last->next = item;
	}
}

struct ag_event* ag_event_get()
{
	if(last_event)
		ag_event_destroy(last_event);

	struct ag_event* event = 0;
	if(ag_event_queue)
	{
		struct ag_event_queue_item* item = ag_event_queue;
		ag_event_queue = item->next;
		event = item->event;
		free(item);
	}
	last_event = event;
	return event;
}
