#include "gui.h"
#include <stdio.h>


struct ag_font* ag_font_default;


struct ag_gui_elem* ag_gui_elem_new()
{
	struct ag_gui_elem* elem = (struct ag_gui_elem*)malloc(sizeof(struct ag_gui_elem));
	elem->childs = 0;
	elem->child_count = 0;
	elem->padding = 16;
	elem->parent = 0;
	elem->layouted_size = ag_vec2i(0,0);
	elem->preferred_size = ag_vec2i(100000,100000);
	elem->layouted_pos = ag_vec2i(0,0);
	return elem;
}

struct ag_vec2i ag_gui_elem_type_preffered_size(enum ag_gui_elem_type type)
{
	switch(type)
	{
	case AG_GUI_BUTTON:
		return ag_vec2i(120,30);
	default:
		return ag_vec2i(100000,100000);
	}
}

struct ag_gui_elem* ag_gui_elem_new_from_file(char* fname)
{
	FILE* fil = fopen(fname, "rb");
	fseek(fil, 0, SEEK_END);
	int len = ftell(fil);
	fseek(fil, 0, SEEK_SET);
	char* data = (char*)malloc(sizeof(char)*len+1);
	fread(data, len, 1, fil);
	fclose(fil);
	data[len] = 0; //why -10...?

	char** lines = (char**)malloc(sizeof(char*)*1);
	lines[0] = data;
	int line_count = 1;

	char* newline;
	while((newline = strchr(lines[line_count-1], '\n')))
	{
		lines = (char**)realloc(lines, sizeof(char*)*++line_count);
		lines[line_count-1] = newline+1;
		*newline = 0;
	}


	int* indent = (int*)malloc(sizeof(int)*line_count);
	char** vals = (char**)malloc(sizeof(char*)*line_count);
	for(int i = 0; i < line_count; ++i)
	{
		char* ch = lines[i];
		while(*(ch++) == '\t');
		indent[i] = ch - lines[i] - 1;
		lines[i] = ch-1;

		char* eq = strchr(lines[i], '=');
		if(eq)
		{
			vals[i] = eq+1;
			*eq = 0;
		}
		else
			vals[i] = "";
	}
	
	for(int i = 0; i < line_count; ++i)
		printf("line (%i): (%s) (%s)\n", indent[i], lines[i], vals[i]);

	struct ag_gui_elem* start = ag_gui_elem_new();
	start->type = AG_GUI_NONE;
	struct ag_gui_elem* elem = start;
	struct ag_gui_elem* new_elem;
	int level = 0;
	for(int i = 0; i < line_count; ++i)
	{
		printf("beep\n");
		if(indent[i] == level)
		{
			if(strcmp(lines[i], "button") == 0)
			{
				new_elem = ag_gui_elem_new();
				new_elem->type = AG_GUI_BUTTON;
				ag_gui_elem_add_child(elem, new_elem);
				new_elem->preferred_size = ag_gui_elem_type_preffered_size(AG_GUI_BUTTON);
				elem = new_elem;
				++level;
			}
			else if(strcmp(lines[i], "label") == 0)
			{
				new_elem = ag_gui_elem_new();
				new_elem->type = AG_GUI_LABEL;
				ag_gui_elem_add_child(elem, new_elem);
				elem = new_elem;
				++level;
			}
			else if(strcmp(lines[i], "hpanel") == 0)
			{
				new_elem = ag_gui_elem_new();
				new_elem->type = AG_GUI_HPANEL;
				ag_gui_elem_add_child(elem, new_elem);
				elem = new_elem;
				++level;
			}
			else if(strcmp(lines[i], "vpanel") == 0)
			{
				new_elem = ag_gui_elem_new();
				new_elem->type = AG_GUI_VPANEL;
				ag_gui_elem_add_child(elem, new_elem);
				elem = new_elem;
				++level;
			}
			else if(strcmp(lines[i], "text") == 0)
			{
				elem->text = vals[i];
			}
			else
			{
				fprintf(stderr, "unknown identifier \"%s\"\n", lines[i]);
				exit(0);
			}
		}
		else if(indent[i] < level)
		{
			--level;
			elem = elem->parent;
			--i;
		}
		else
		{
			fprintf(stderr, "something wrent wrong at line %i, indent %i and level %i\n", i, indent[i], level);
		}
	}


	free(data);
	free(lines);
	free(indent);
	free(vals);

	return start;
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
		case AG_GUI_BUTTON: return "button";
		case AG_GUI_LABEL: return "label";
		case AG_GUI_HPANEL: return "hpanel";
		case AG_GUI_VPANEL: return "vpanel";
	}
	return "unknown";
}

void ag_gui_elem_manage_layout(struct ag_gui_elem* elem)
{
	switch(elem->type)
	{
	case AG_GUI_NONE:
		elem->layouted_size = elem->design_size;
		elem->layouted_pos = elem->design_pos;
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
				printf("layouted_size %i %i\n", elem->childs[i]->layouted_size.x, elem->childs[i]->layouted_size.y);

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
			ag_gui_elem_manage_layout(elem->childs[0]);
		}
	default:
		break;
	}
}

static void ag_surface_draw_gui_elem_p(struct ag_surface* surface, struct ag_gui_elem* elem, struct ag_vec2i pos, struct ag_vec2i size)
{
	switch(elem->type)
	{
	case AG_GUI_NONE:
		ag_surface_draw_gui_elem_p(surface, elem->childs[0], pos, size);
		break;
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
			struct ag_vec2i ul1, ul2, ur1, ur2, ll1, ll2, lr1, lr2;
			ul1 = pos; ul2 = ag_vec2i(ul1.x+1, ul1.y+1);
			ur1 = ag_vec2i(pos.x+size.x-1, pos.y); ur2 = ag_vec2i(ur1.x-1, ur1.y+1);
			ll1 = ag_vec2i(pos.x, pos.y+size.y-1); ll2 = ag_vec2i(ll1.x+1, ll1.y-1);
			lr1 = ag_vec2i(pos.x+size.x-1, pos.y+size.y-1); lr2 = ag_vec2i(lr1.x-1, lr1.y-1);
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

void ag_surface_draw_gui_elem(struct ag_surface* surface, struct ag_gui_elem* elem)
{
	if(elem->type == AG_GUI_NONE)
		ag_surface_draw_gui_elem_p(surface, elem, elem->layouted_pos, elem->layouted_size);
}
