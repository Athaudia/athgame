#ifndef GUARD_PLATFORM_H
#define GUARD_PLATFORM_H

#ifdef PLATFORM_LINUX_FB
#define PLATFORM_LINUX
#endif

#ifdef PLATFORM_WIN32
#include <winsock2.h> //have to include it before windows.h
#include <windows.h>
#include <windowsx.h>
#endif

#ifdef PLATFORM_LINUX_FB
#include <linux/fb.h>
#endif

#include "vec.h"

struct ag_window;

struct ag_platform_window_internal
{
#ifdef PLATFORM_WIN32
	HWND hwnd;
#endif
#ifdef PLATFORM_LINUX_FB
    int fbfd;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    char *fbp;
#endif
};

void ag_platform_init();
void ag_platform_uninit();
void ag_platform_window_internal_init(struct ag_platform_window_internal* internal, struct ag_window* window, struct ag_vec2i size, bool resizeable);
void ag_platform_window_update(struct ag_window* window);
void ag_platform_window_resize(struct ag_window* window);
double ag_get_time();
void ag_sleep(int ms);
#endif
