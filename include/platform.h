#ifndef GUARD_PLATFORM_H
#define GUARD_PLATFORM_H

#ifdef PLATFORM_WIN32
#include <windows.h>
#include <windowsx.h>
#endif

#include "vec.h"

struct ag_window;

struct ag_platform_window_internal
{
#ifdef PLATFORM_WIN32
	HWND hwnd;
#endif
};

void ag_platform_init();
void ag_platform_uninit();
void ag_platform_window_internal_init(struct ag_platform_window_internal* internal, struct ag_window* window, struct ag_vec2i size);
void ag_platform_window_update(struct ag_window* window);
void ag_platform_window_resize(struct ag_window* window);
double ag_get_time();
void ag_sleep(int ms);
#endif
