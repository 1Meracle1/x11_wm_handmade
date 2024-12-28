#ifndef WM_RANDR_H
#define WM_RANDR_H

#include "../core/core.h"
#include <xcb/xcb.h>

internal bool RandrInit(Allocator allocator, xcb_connection_t* conn, xcb_window_t root, int* randr_event_base);

#endif