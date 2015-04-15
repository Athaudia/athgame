#include "gui.h"
#include <stdio.h>


struct ag_font* ag_font_default;


struct ag_gui_elem* ag_gui_elem_new()
{
	struct ag_gui_elem* elem = (struct ag_gui_elem*)malloc(sizeof(struct ag_gui_elem));
	elem->childs = 0;
	elem->child_count = 0;
	elem->padding = 2;
	elem->parent = 0;
	elem->onclick = 0;
	elem->layouted_size = ag_vec2i(0,0);
	elem->preferred_size = ag_vec2i(100000,100000);
	elem->layouted_pos = ag_vec2i(0,0);
	elem->text = 0;
	elem->filename = 0;
	elem->surface = 0;
	elem->bg = 0;
	elem->state = AG_GUI_ELEM_STATE_NONE;
	elem->color = ag_color(0,0,0,255);
	return elem;
}

void ag_gui_elem_destroy(struct ag_gui_elem* elem)
{
	free(elem->text);
	free(elem->childs);
	free(elem);
}


void ag_gui_elem_add_child(struct ag_gui_elem* elem, struct ag_gui_elem* child)
{
	elem->childs = realloc(elem->childs, sizeof(struct ag_gui_elem*)*++elem->child_count);
	elem->childs[elem->child_count-1] = child;
	child->parent = elem;
}

static void ag_gui_elem_debug_p(struct ag_gui_elem* elem, int level)
{
	for(int i = 0; i < level; ++i) printf("\t");
	printf("%s\n", ag_gui_elem_type_to_string(elem->type));
	for(int i = 0; i < elem->child_count; ++i)
		ag_gui_elem_debug_p(elem->childs[i], level+1);
}

void ag_gui_elem_debug(struct ag_gui_elem* elem)
{
	printf("--------------------\n");
	ag_gui_elem_debug_p(elem, 0);
	printf("--------------------\n");
}

char* ag_gui_elem_type_to_string(enum ag_gui_elem_type type)
{
	switch(type)
	{
		case AG_GUI_NONE: return "none";
		case AG_GUI_SOLID: return "solid";
		case AG_GUI_IMG: return "image";
		case AG_GUI_BUTTON: return "button";
		case AG_GUI_LABEL: return "label";
		case AG_GUI_HPANEL: return "hpanel";
		case AG_GUI_VPANEL: return "vpanel";
	}
	return "unknown";
}

void ag_gui_elem_manage_layout(struct ag_gui_elem* elem)
{
	if(elem->bg)
	{
		elem->bg->layouted_pos = ag_vec2i(0,0);
		elem->bg->layouted_size = elem->layouted_size;
		ag_gui_elem_manage_layout(elem->bg);
	}

	switch(elem->type)
	{
	case AG_GUI_NONE:
		elem->childs[0]->layouted_size = elem->layouted_size;
		elem->childs[0]->layouted_pos = ag_vec2i(0,0);
		ag_gui_elem_manage_layout(elem->childs[0]);
		break;
	case AG_GUI_HPANEL:
		{
			int size = (elem->layouted_size.x+elem->padding)/elem->child_count - elem->padding;
			int rest = elem->layouted_size.x - ((size+elem->padding)*elem->child_count-elem->padding);
			for(int i = 0; i < elem->child_count; ++i)
			{
				elem->childs[i]->layouted_size = ag_vec2i(size+((i==elem->child_count-1)?rest:0), elem->layouted_size.h);

				struct ag_vec2i dif = ag_vec2i(0,0);
				if(elem->childs[i]->preferred_size.w < elem->childs[i]->layouted_size.w)
					dif.w = elem->childs[i]->layouted_size.w - elem->childs[i]->preferred_size.w;
				if(elem->childs[i]->preferred_size.h < elem->childs[i]->layouted_size.h)
					dif.h = elem->childs[i]->layouted_size.h - elem->childs[i]->preferred_size.h;
				elem->childs[i]->layouted_size = ag_vec2i_sub(elem->childs[i]->layouted_size, dif);

				elem->childs[i]->layouted_pos = ag_vec2i(i*(size+elem->padding)+dif.x/2, dif.y/2);
				ag_gui_elem_manage_layout(elem->childs[i]);
			}
		}
		break;
	case AG_GUI_VPANEL:
		{
			int size = (elem->layouted_size.y+elem->padding)/elem->child_count - elem->padding;
			int rest = elem->layouted_size.y - ((size+elem->padding)*elem->child_count-elem->padding);
			for(int i = 0; i < elem->child_count; ++i)
			{
				elem->childs[i]->layouted_size = ag_vec2i(elem->layouted_size.w, size+((i==elem->child_count-1)?rest:0));

				struct ag_vec2i dif = ag_vec2i(0,0);
				if(elem->childs[i]->preferred_size.w < elem->childs[i]->layouted_size.w)
					dif.w = elem->childs[i]->layouted_size.w - elem->childs[i]->preferred_size.w;
				if(elem->childs[i]->preferred_size.h < elem->childs[i]->layouted_size.h)
					dif.h = elem->childs[i]->layouted_size.h - elem->childs[i]->preferred_size.h;
				elem->childs[i]->layouted_size = ag_vec2i_sub(elem->childs[i]->layouted_size, dif);

				elem->childs[i]->layouted_pos = ag_vec2i(dif.x/2, i*(size+elem->padding)+dif.y/2);
				ag_gui_elem_manage_layout(elem->childs[i]);
			}
		}
		break;
	case AG_GUI_BUTTON:
		if(elem->child_count)
		{
			elem->childs[0]->layouted_pos = ag_vec2i(elem->padding, elem->padding);
			elem->childs[0]->layouted_size = ag_vec2i(elem->layouted_size.x - elem->padding*2, elem->layouted_size.y - elem->padding*2);
			if(elem->state == AG_GUI_ELEM_STATE_PRESSED)
			{
				elem->childs[0]->layouted_pos = ag_vec2i_add(elem->childs[0]->layouted_pos, ag_vec2i(2,2));
			}
			ag_gui_elem_manage_layout(elem->childs[0]);
		}
	default:
		break;
	}
}

struct ag_vec2i ag_gui_elem_get_absolute_pos(struct ag_gui_elem* elem)
{
	struct ag_vec2i pos = ag_vec2i(0,0);
	while(elem)
	{
		pos = ag_vec2i_add(pos, elem->layouted_pos);
		elem = elem->parent;
	}
	return pos;
}


static void ag_surface_draw_gui_elem_p(struct ag_surface* surface, struct ag_gui_elem* elem, struct ag_vec2i pos, struct ag_vec2i size)
{
	if(elem->bg)
		ag_surface_draw_gui_elem_p(surface, elem->bg, pos, size);

	switch(elem->type)
	{
	case AG_GUI_NONE:
		ag_surface_draw_gui_elem_p(surface, elem->childs[0], pos, size);
		break;
	case AG_GUI_SOLID:
		printf("size %i,%i\n", size.x, size.y);
		ag_surface_fill_rect(surface, pos, size, elem->color);
		break;
	case AG_GUI_IMG:
		if(!elem->surface && elem->filename)
			elem->surface = ag_surface_new_from_file(elem->filename); //todo: load from cache
		if(elem->surface)
			ag_surface_blit_to(surface, elem->surface, pos);
	case AG_GUI_HPANEL:
	case AG_GUI_VPANEL:
		for(int i = 0; i < elem->child_count; ++i)
			ag_surface_draw_gui_elem_p(surface, elem->childs[i], ag_vec2i_add(pos, elem->childs[i]->layouted_pos), elem->childs[i]->layouted_size);
		break;
	case AG_GUI_BUTTON:
		{
			struct ag_color col[5];
			for(int i = 0; i < 5; ++i)
				col[i] = ag_color(255*i/4, 255*i/16, 255*i/8, 255);
			if(elem->state == AG_GUI_ELEM_STATE_PRESSED)
				for(int i = 0; i < 5; ++i)
					col[4-i] = ag_color(255*i/4, 255*i/16, 255*i/8, 255);

			struct ag_vec2i ul1, ul2, ur1, ur2, ll1, ll2, lr1, lr2;
			ul1 = pos; ul2 = ag_vec2i(ul1.x+1, ul1.y+1);
			ur1 = ag_vec2i(pos.x+size.x-1, pos.y); ur2 = ag_vec2i(ur1.x-1, ur1.y+1);
			ll1 = ag_vec2i(pos.x, pos.y+size.y-1); ll2 = ag_vec2i(ll1.x+1, ll1.y-1);
			lr1 = ag_vec2i(pos.x+size.x-1, pos.y+size.y-1); lr2 = ag_vec2i(lr1.x-1, lr1.y-1);
			if(!elem->bg)
				ag_surface_fill_rect(surface, pos, size, col[2]);
			ag_surface_draw_line(surface, ul1, ur1, col[4]);
			ag_surface_draw_line(surface, ul1, ll1, col[4]);
			ag_surface_draw_line(surface, lr1, ll1, col[0]);
			ag_surface_draw_line(surface, lr1, ur1, col[0]);

			ag_surface_draw_line(surface, ul2, ur2, col[3]);
			ag_surface_draw_line(surface, ul2, ll2, col[3]);
			ag_surface_draw_line(surface, lr2, ll2, col[1]);
			ag_surface_draw_line(surface, lr2, ur2, col[1]);
			if(elem->child_count)
				ag_surface_draw_gui_elem_p(surface, elem->childs[0], ag_vec2i_add(pos, elem->childs[0]->layouted_pos), elem->childs[0]->layouted_size);
		}
		break;
	case AG_GUI_LABEL:
		ag_surface_draw_text_centered(surface, ag_font_default, pos, size, ag_color(127*1.5,95*1.5,112*1.5,255), elem->text);
		break;
	default:
		break;
	}	
}

void ag_surface_draw_gui(struct ag_surface* surface, struct ag_gui* gui)
{
	ag_surface_draw_gui_elem_p(surface, gui->elem, gui->elem->layouted_pos, gui->elem->layouted_size);
}

void ag_gui_manage_layout(struct ag_gui* gui)
{
	gui->elem->layouted_size = gui->size;
	gui->elem->layouted_pos = gui->pos;
	ag_gui_elem_manage_layout(gui->elem);
}

struct ag_gui_elem* ag_gui_get_elem_from_coord(struct ag_gui* gui, struct ag_vec2i coord)
{
	if(!ag_vec2i_point_in_rect(coord, gui->pos, gui->size))
		return 0;
	coord = ag_vec2i_sub(coord, gui->pos);
	struct ag_gui_elem* elem = gui->elem;
	
	while(true)
	{
		outer:
		switch(elem->type)
		{
		case AG_GUI_BUTTON:
		case AG_GUI_LABEL:
			return elem;
		default:
			for(int i = 0; i < elem->child_count; ++i)
			{
				if(ag_vec2i_point_in_rect(coord, elem->childs[i]->layouted_pos, elem->childs[i]->layouted_size))
				{
					coord = ag_vec2i_sub(coord, elem->childs[i]->layouted_pos);
					elem = elem->childs[i];
					goto outer;
				}
			}
			return elem;
		}
	}
	return 0; //unreachable
}

void ag_gui_process_event(struct ag_gui* gui, struct ag_event* event)
{
	switch(event->type)
	{
	case AG_EVENT_MOUSE_DOWN:
		{
			struct ag_gui_elem* elem = ag_gui_get_elem_from_coord(gui, event->mouse_pos);
			elem->state = AG_GUI_ELEM_STATE_PRESSED;
			gui->focused_elem = elem;
		}
		break;
	case AG_EVENT_MOUSE_MOVE:
		{
			if(gui->focused_elem)
			{
				if(ag_vec2i_point_in_rect(event->mouse_pos, ag_vec2i_add(gui->pos, ag_gui_elem_get_absolute_pos(gui->focused_elem)), gui->focused_elem->layouted_size))
					gui->focused_elem->state = AG_GUI_ELEM_STATE_PRESSED;
				else
					gui->focused_elem->state = AG_GUI_ELEM_STATE_NONE;
			}
		}
		break;
	case AG_EVENT_MOUSE_UP:
		{
			if(gui->focused_elem)
			{
				if(gui->focused_elem->state == AG_GUI_ELEM_STATE_PRESSED)
				{
					gui->focused_elem->state = AG_GUI_ELEM_STATE_NONE;
					if(gui->focused_elem->onclick)
						ag_event_push(ag_event_gui_clicked_new(gui, gui->focused_elem, gui->focused_elem->onclick));
				}
				gui->focused_elem = 0;
			}
		}
	default:
		break;
	}
}
