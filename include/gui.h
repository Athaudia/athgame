#ifndef AG_GUI_H
#define AG_GUI_H

#include "vec.h"
#include "surface.h"
#include "font.h"

enum ag_gui_elem_type
{
AG_GUI_NONE,
AG_GUI_BUTTON, AG_GUI_LABEL,
AG_GUI_HPANEL, AG_GUI_VPANEL
};

extern struct ag_font* ag_font_default;

struct ag_gui_elem
{
	enum ag_gui_elem_type type;
	struct ag_vec2i design_pos, design_size, layouted_size, layouted_pos, preferred_size;
	struct ag_gui_elem* parent;
	struct ag_gui_elem** childs;
	int child_count;
	char* text;
	int padding;
};

struct ag_gui
{
	struct ag_vec2i pos, size;
	struct ag_gui_elem* elem;
	struct ag_gui_elem* focused_elem;
};

struct ag_gui_elem* ag_gui_elem_new();
void ag_gui_elem_add_child(struct ag_gui_elem* elem, struct ag_gui_elem* child);
void ag_gui_elem_debug(struct ag_gui_elem* elem);
char* ag_gui_elem_type_to_string(enum ag_gui_elem_type type);
void ag_gui_elem_manage_layout(struct ag_gui_elem* elem);

struct ag_gui_elem* ag_gui_new(char* fname);
void ag_surface_draw_gui(struct ag_surface* surface, struct ag_gui* gui);

#endif
