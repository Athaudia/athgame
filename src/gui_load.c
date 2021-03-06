#include "gui.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct ag_gui* ag_gui__new_from_file(char* fname)
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
	
	//for(int i = 0; i < line_count; ++i)
	//	printf("line (%i): (%s) (%s)\n", indent[i], lines[i], vals[i]);

	struct ag_gui_elem* start = ag_gui_elem__new();
	start->type = AG_GUI_NONE;
	struct ag_gui_elem* elem = start;
	struct ag_gui_elem* new_elem;
	int level = 0;
	for(int i = 0; i < line_count; ++i)
	{
		if(indent[i] == level)
		{
			if(strcmp(lines[i], "bg") == 0) //special case
			{
				new_elem = ag_gui_elem__new();
				new_elem->type = AG_GUI_NONE;
				new_elem->parent = elem;
				elem->bg = new_elem;
				elem = new_elem;
				++level;
			}
			else if(strcmp(lines[i], "solid") == 0)
			{
				new_elem = ag_gui_elem__new();
				new_elem->type = AG_GUI_SOLID;
				ag_gui_elem__add_child(elem, new_elem);
				elem = new_elem;
				++level;
			}
			else if(strcmp(lines[i], "image") == 0)
			{
				new_elem = ag_gui_elem__new();
				new_elem->type = AG_GUI_IMG;
				ag_gui_elem__add_child(elem, new_elem);
				elem = new_elem;
				++level;
			}
			else if(strcmp(lines[i], "button") == 0)
			{
				new_elem = ag_gui_elem__new();
				new_elem->type = AG_GUI_BUTTON;
				ag_gui_elem__add_child(elem, new_elem);
				new_elem->preferred_size = ag_gui_elem__get_preferred_size(new_elem);
				elem = new_elem;
				++level;
			}
			else if(strcmp(lines[i], "label") == 0)
			{
				new_elem = ag_gui_elem__new();
				new_elem->type = AG_GUI_LABEL;
				ag_gui_elem__add_child(elem, new_elem);
				elem = new_elem;
				++level;
			}
			else if(strcmp(lines[i], "hpanel") == 0)
			{
				new_elem = ag_gui_elem__new();
				new_elem->type = AG_GUI_HPANEL;
				ag_gui_elem__add_child(elem, new_elem);
				elem = new_elem;
				++level;
			}
			else if(strcmp(lines[i], "vpanel") == 0)
			{
				new_elem = ag_gui_elem__new();
				new_elem->type = AG_GUI_VPANEL;
				ag_gui_elem__add_child(elem, new_elem);
				elem = new_elem;
				++level;
			}
			else if(strcmp(lines[i], "size") == 0)
			{
				if(strcmp(vals[i], "auto") == 0 )
				{
					elem->design_size = ag_vec2i(0,0);
				}
				else
				{
					if(vals[i][0] == '*')
					{
						elem->design_width_relative = true;
						++vals[i];
					}
					else
						elem->design_width_relative = false;
					char* end;
					elem->design_size.w = strtol(vals[i], &end, 10);
					++end;

					
					if(end[0] == '*')
					{
						elem->design_height_relative = true;
						++end;
					}
					else
						elem->design_height_relative = false;
					elem->design_size.h = strtol(end, &end, 10);
					++end;
				}
				
			}
			else if(strcmp(lines[i], "text") == 0)
			{
				elem->text = vals[i];
			}
			else if(strcmp(lines[i], "file") == 0)
			{
				elem->filename = vals[i];
			}
			else if(strcmp(lines[i], "onclick") == 0)
			{
				elem->onclick = vals[i];
			}
			else if(strcmp(lines[i], "color") == 0)
			{
				//todo: actually read colour
				char* end;
				int r = strtol(vals[i], &end, 10);
				int g = strtol(end+1, &end, 10);
				int b = strtol(end+1, &end, 10);
				elem->color = ag_color32(r, g, b, 255);
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

	//todo: move data into gui struct and keep until destruction
	//free(data);

	free(lines);
	free(indent);
	free(vals);

	struct ag_gui* gui = (struct ag_gui*)malloc(sizeof(struct ag_gui));
	gui->elem = start->childs[0];
	gui->size = ag_vec2i(0,0);
	gui->pos = ag_vec2i(0,0);
	gui->focused_elem = 0;
	ag_gui_elem__destroy(start);
	return gui;
}
