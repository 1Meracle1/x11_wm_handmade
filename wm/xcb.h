#ifndef WM_XCB_H
#define WM_XCB_H

#include "../core/core.h"
#include "window.h"
#include <xcb/xproto.h>

internal bool Xcb_Init(Allocator allocator, String wm_name);
internal void Xcb_Deinit();

internal void Xcb_ChangeWindowAttributes(xcb_window_t window, int value_mask, int value);

internal WindowType Xcb_WindowType(xcb_window_t window);

internal bool Xcb_PollEvents();

#endif