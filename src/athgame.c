#include "athgame.h"
#include "events.h"
#include "platform.h"
#include "font.h"
#include "gui.h"

void ag_init()
{
	agc_white = (struct ag_color){255,255,255,255};
	agc_black = (struct ag_color){0,0,0,255};
	ag_event_queue = 0;
	ag_platform_init();
	FT_Init_FreeType(&ag_ft);
	ag_font_default = ag_font_new("fonts/Quicksand-Regular.otf", 12);
}

void ag_uninit()
{
	ag_platform_uninit();
}
