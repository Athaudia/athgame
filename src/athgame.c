#include "athgame.h"
#include "events.h"
#include "platform.h"
#include "font.h"
#include "gui.h"
#include "quicksand_regular.h"
#include "state.h"
#include FT_MODULE_H

int ft_size_used;

void* ag_ft_alloc(FT_Memory mem, long size)
{
	//ft_size_used += size;
	//printf("ft used: %i bytes\n", ft_size_used);
	return malloc(size);
}

void* ag_ft_realloc(FT_Memory mem, long cur_size, long new_size, void* block)
{
	//ft_size_used -= cur_size;
	//ft_size_used += new_size;
	//printf("ft used: %i bytes\n", ft_size_used);
	return realloc(block, new_size);
}

void ag_ft_free(FT_Memory mem, void* block)
{
	//ft_size_used -= _msize(block);
	//printf("ft used: %i bytes\n", ft_size_used);
	free(block);
}

void ag_init()
{
	ag_state_current = 0;
	agc_white = (struct ag_color32){255,255,255,255};
	agc_black = (struct ag_color32){0,0,0,255};
	ag_event_queue = 0;
	ag_platform_init();
	ft_size_used = 0;

	struct FT_MemoryRec_* ftmem = (struct FT_MemoryRec_*)malloc(sizeof(struct FT_MemoryRec_));
	ftmem->alloc = ag_ft_alloc;
	ftmem->realloc = ag_ft_realloc;
	ftmem->free = ag_ft_free;
	FT_New_Library(ftmem, &ag_ft);
	FT_Add_Default_Modules(ag_ft);

	//FT_Init_FreeType(&ag_ft);
	ag_font_default = ag_font_new_from_memory(ag_quicksand_regular, ag_quicksand_regular_size, 12);
}

void ag_uninit()
{
	ag_platform_uninit();
}
