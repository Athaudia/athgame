#include "platform.h"
#include "window.h"
#include "events.h"
#include "state.h"
#include <stdio.h>

void ag_platform_init()
{
#ifdef PLATFORM_WIN32
	timeBeginPeriod(1);
#endif
}

void ag_platform_uninit()
{
#ifdef PLATFORM_WIN32
	timeEndPeriod(1);
#endif
}

#ifdef PLATFORM_WIN32
LRESULT CALLBACK win_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	struct ag_window* window = (struct ag_window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch(msg)
	{
	case WM_CREATE:
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)((LPCREATESTRUCT)lparam)->lpCreateParams);
		break;
	case WM_DESTROY:
		ag_event_push(ag_event_close_new(window));
		break;
	case WM_PAINT:
		{
			struct ag_surface* surface = window->filtered_surface->output;
			BITMAPINFO info;
			PAINTSTRUCT ps;
			info.bmiHeader.biSize = sizeof(BITMAPINFO);
			info.bmiHeader.biWidth = surface->size.w;
			info.bmiHeader.biHeight = -surface->size.h;
			info.bmiHeader.biPlanes = 1;
			info.bmiHeader.biBitCount = 32;
			info.bmiHeader.biCompression = BI_RGB;
			info.bmiHeader.biSizeImage = 0;
			info.bmiHeader.biXPelsPerMeter = 1;
			info.bmiHeader.biYPelsPerMeter = 1;
			info.bmiHeader.biClrUsed = 0;
			info.bmiHeader.biClrImportant = 0;

			HDC hdc = BeginPaint(hwnd, &ps);
			SetDIBitsToDevice(hdc, 0, 0, surface->size.w, surface->size.h, 0, 0, 0, surface->size.h, surface->data, &info, DIB_RGB_COLORS);
			EndPaint(hwnd, &ps);
			break;
		}
	case WM_SIZE:
		window->size = ag_vec2i(LOWORD(lparam)/window->filtered_surface->scale, HIWORD(lparam)/window->filtered_surface->scale);
		ag_window_reinit_surfaces(window);
		if(ag_state_current)
			ag_state_run_inner(ag_state_current);
		break;
	case WM_MOUSEMOVE:
		window->mouse_pos.x = GET_X_LPARAM(lparam) / window->filtered_surface->scale;
		window->mouse_pos.y = GET_Y_LPARAM(lparam) / window->filtered_surface->scale;
		ag_event_push(ag_event_mouse_move_new(window, window->mouse_pos));
		break;
	case WM_LBUTTONDOWN:
		ag_event_push(ag_event_mouse_down_new(window, window->mouse_pos));
		break;
	case WM_LBUTTONUP:
		ag_event_push(ag_event_mouse_up_new(window, window->mouse_pos));
		break;
	default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}
#endif

void ag_platform_window_internal_init(struct ag_platform_window_internal* internal, struct ag_window* window, struct ag_vec2i size, bool resizeable)
{
#ifdef PLATFORM_WIN32
	WNDCLASSEX wc;
	HWND hwnd;
	RECT rect;
	DWORD style;

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = win_proc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetModuleHandle(0);
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+0);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "yay";
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wc);

	if(resizeable)
		style = WS_OVERLAPPEDWINDOW;
	else
		style = WS_OVERLAPPEDWINDOW&~WS_THICKFRAME&~WS_MAXIMIZEBOX;
	rect.top = 0;
	rect.left = 0;
	rect.right = size.w;
	rect.bottom = size.h;
	AdjustWindowRect(&rect, style, FALSE);

	hwnd = CreateWindow("yay", "<3", style, CW_USEDEFAULT, CW_USEDEFAULT,
	                    rect.right-rect.left, rect.bottom-rect.top, 0, 0, GetModuleHandle(0), window);
	internal->hwnd = hwnd;

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
#endif
}

void ag_platform_window_update(struct ag_window* window)
{
#ifdef PLATFORM_WIN32
	HWND hwnd = window->internal.hwnd;
	MSG msg;
	InvalidateRect(hwnd, 0, FALSE);
	while(PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#endif
}

void ag_platform_window_resize(struct ag_window* window)
{
#ifdef PLATFORM_WIN32
	HWND hwnd = window->internal.hwnd;
	RECT old_rect, rect;
	GetWindowRect(hwnd, &old_rect);
	rect.top = 0;
	rect.left = 0;
	rect.right = window->size.w*window->filtered_surface->scale;
	rect.bottom = window->size.h*window->filtered_surface->scale;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW&~WS_THICKFRAME&~WS_MAXIMIZEBOX, FALSE);
	MoveWindow(hwnd, old_rect.left, old_rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE);
#endif
}

double ag_get_time()
{
#ifdef PLATFORM_WIN32
	LARGE_INTEGER ticks, freq;
	QueryPerformanceCounter(&ticks);
	QueryPerformanceFrequency(&freq);
	return (double)ticks.QuadPart / (double)freq.QuadPart;

#endif
}

void ag_sleep(int ms)
{
#ifdef PLATFORM_WIN32
	Sleep(ms);
#endif
}
