#include "gui.h"


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

struct ag_gui* ag_gui_new_from_file(char* fname)
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
			else if(strcmp(lines[i], "onclick") == 0)
			{
				printf("onclick added\n");
				elem->onclick = vals[i];
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
	ag_gui_elem_destroy(start);
	return gui;
}
