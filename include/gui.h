#ifndef AG_GUI_H
#define AG_GUI_H

#include "vec.h"
#include "surface.h"
#include "font.h"
#include "events.h"

enum ag_gui_elem_type
{
AG_GUI_NONE, AG_GUI_SOLID, AG_GUI_IMG,
AG_GUI_BUTTON, AG_GUI_LABEL,
AG_GUI_HPANEL, AG_GUI_VPANEL
};

extern struct ag_font* ag_font_default;

enum ag_gui_elem_state
{
	AG_GUI_ELEM_STATE_NONE, AG_GUI_ELEM_STATE_PRESSED
};

struct ag_gui_elem
{
	enum ag_gui_elem_type type;
	struct ag_vec2i design_pos, design_size, layouted_size, layouted_pos, preferred_size;
	struct ag_gui_elem* parent;
	struct ag_gui_elem* bg;
	struct ag_gui_elem** childs;
	enum ag_gui_elem_state state;
	char* onclick; //name of onclick event
	bool design_width_relative;
	bool design_height_relative;
	struct ag_color32 color;
	int child_count;
	char* text;
	char* filename;
	struct ag_surface32* surface;
	int padding;
};

struct ag_gui
{
	struct ag_vec2i pos, size;
	struct ag_gui_elem* elem;
	struct ag_gui_elem* focused_elem;
};

struct ag_gui_elem* ag_gui_elem__new();
void ag_gui_elem__destroy(struct ag_gui_elem* elem);
void ag_gui_elem__destroy_recursive(struct ag_gui_elem* elem); //todo: implement
void ag_gui_elem__add_child(struct ag_gui_elem* elem, struct ag_gui_elem* child);
void ag_gui_elem__debug(struct ag_gui_elem* elem);
char* ag_gui_elem_type__to_string(enum ag_gui_elem_type type);
void ag_gui_elem__manage_layout(struct ag_gui_elem* elem);
struct ag_vec2i ag_gui_elem__get_absolute_pos(struct ag_gui_elem* elem);
struct ag_vec2i ag_gui_elem__get_preferred_size(struct ag_gui_elem* elem);

void ag_surface32__draw_gui(struct ag_surface32* surface, struct ag_gui* gui);
void ag_gui__manage_layout(struct ag_gui* gui);
struct ag_gui* ag__gui_new_from_file(char* fname);
void ag_gui__process_event(struct ag_gui* gui, struct ag_event* event);
#endif
