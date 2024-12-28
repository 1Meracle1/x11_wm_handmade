#include "xcb.h"
#include "monitor.h"
#include "randr.h"
#include "workspace.h"

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <X11/extensions/XTest.h>
#include <time.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_cursor.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/randr.h>

Display              *g_display;
xcb_connection_t     *g_conn;
xcb_screen_t         *g_screen;
xcb_ewmh_connection_t g_ewmh;
int                   g_randr_base;

internal bool EwmhInit()
{
  bool                      ok           = true;
  xcb_intern_atom_cookie_t *ewmh_cookies = xcb_ewmh_init_atoms(g_conn, &g_ewmh);
  if (!xcb_ewmh_init_atoms_replies(&g_ewmh, ewmh_cookies, NULL))
  {
    Error("Failed to initialize EWMH atoms.");
    ok = false;
  }
  else
  {
    xcb_atom_t net_atoms[] = {
        g_ewmh._NET_SUPPORTED,
        g_ewmh._NET_SUPPORTING_WM_CHECK,
        g_ewmh._NET_DESKTOP_NAMES,
        g_ewmh._NET_DESKTOP_VIEWPORT,
        g_ewmh._NET_NUMBER_OF_DESKTOPS,
        g_ewmh._NET_CURRENT_DESKTOP,
        g_ewmh._NET_CLIENT_LIST,
        g_ewmh._NET_ACTIVE_WINDOW,
        g_ewmh._NET_WM_NAME,
        g_ewmh._NET_CLOSE_WINDOW,
        g_ewmh._NET_WM_STRUT_PARTIAL,
        g_ewmh._NET_WM_DESKTOP,
        g_ewmh._NET_WM_STATE,
        g_ewmh._NET_WM_STATE_HIDDEN,
        g_ewmh._NET_WM_STATE_FULLSCREEN,
        g_ewmh._NET_WM_STATE_BELOW,
        g_ewmh._NET_WM_STATE_ABOVE,
        g_ewmh._NET_WM_STATE_STICKY,
        g_ewmh._NET_WM_STATE_DEMANDS_ATTENTION,
        g_ewmh._NET_WM_WINDOW_TYPE,
        g_ewmh._NET_WM_WINDOW_TYPE_DOCK,
        g_ewmh._NET_WM_WINDOW_TYPE_DESKTOP,
        g_ewmh._NET_WM_WINDOW_TYPE_NOTIFICATION,
        g_ewmh._NET_WM_WINDOW_TYPE_DIALOG,
        g_ewmh._NET_WM_WINDOW_TYPE_SPLASH,
        g_ewmh._NET_WM_WINDOW_TYPE_UTILITY,
        g_ewmh._NET_WM_WINDOW_TYPE_TOOLBAR,
    };
    xcb_void_cookie_t cookie = xcb_ewmh_set_supported_checked(
        &g_ewmh, 0, sizeof(net_atoms) / sizeof(xcb_atom_t), net_atoms);
    xcb_generic_error_t *error = xcb_request_check(g_conn, cookie);
    if (error)
    {
      Errorf("Failed set supported ewmh atoms, error_code: %d, major_code: %d, minor_code: %d",
             error->error_code, error->major_code, error->minor_code);
      ok = false;
    }
  }
  return ok;
}

internal bool Xcb_Init(Allocator allocator, String wm_name)
{
  bool ok   = true;
  g_display = XOpenDisplay(NULL);
  g_conn    = XGetXCBConnection(g_display);
  if (!g_display || !g_conn || xcb_connection_has_error(g_conn))
  {
    Error("Failed to establish X11 connection");
    ok = false;
  }
  if (ok)
  {
    const xcb_setup_t    *setup = xcb_get_setup(g_conn);
    xcb_screen_iterator_t iter  = xcb_setup_roots_iterator(setup);
    g_screen                    = iter.data;

    Xcb_ChangeWindowAttributes(g_screen->root, XCB_CW_EVENT_MASK,
                               XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                                   XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
                                   XCB_EVENT_MASK_POINTER_MOTION);

    xcb_window_t meta_window = xcb_generate_id(g_conn);
    xcb_create_window(g_conn, XCB_COPY_FROM_PARENT, meta_window, g_screen->root, -1, -1, 1, 1, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT, XCB_NONE, NULL);
    xcb_icccm_set_wm_class(g_conn, meta_window, wm_name.size, (char *)wm_name.data);

    xcb_grab_pointer(g_conn, 1, g_screen->root, XCB_EVENT_MASK_POINTER_MOTION, XCB_GRAB_MODE_ASYNC,
                     XCB_GRAB_MODE_ASYNC, XCB_NONE, XCB_NONE, XCB_CURRENT_TIME);
    xcb_cursor_context_t *cursor_ctx;
    if (xcb_cursor_context_new(g_conn, g_screen, &cursor_ctx) == 0)
    {
      xcb_cursor_t cursor = xcb_cursor_load_cursor(cursor_ctx, "left_ptr");
      Xcb_ChangeWindowAttributes(g_screen->root, XCB_CW_CURSOR, cursor);
      xcb_cursor_context_free(cursor_ctx);
    }

    ok = EwmhInit();
  }
  if (ok)
  {
    if (!RandrInit(allocator, g_conn, g_screen->root, &g_randr_base))
    {
      Error(
          "Failed to init monitors from randr, fallback to getting required info from the screen");
      AddMonitor(allocator, StrLit("default"), true, 0, 0, 0, g_screen->width_in_pixels,
                 g_screen->height_in_pixels);
    }
  }
  xcb_flush(g_conn);
  return ok;
}

internal void Xcb_Deinit()
{
  xcb_ungrab_pointer(g_conn, XCB_CURRENT_TIME);
  xcb_disconnect(g_conn);
  XCloseDisplay(g_display);
}

internal void Xcb_ChangeWindowAttributes(xcb_window_t window, int value_mask, int value)
{
  u32 v[1] = {value};
  xcb_change_window_attributes(g_conn, window, value_mask, v);
}

internal WindowType Xcb_WindowType(xcb_window_t window)
{
  WindowType                 window_type = WindowType_Normal;
  xcb_get_property_cookie_t  cookie      = xcb_ewmh_get_wm_window_type(&g_ewmh, window);
  xcb_ewmh_get_atoms_reply_t window_type_atoms;
  if (xcb_ewmh_get_wm_window_type_reply(&g_ewmh, cookie, &window_type_atoms, NULL))
  {
    for (u32 i = 0; i < window_type_atoms.atoms_len; i += 1)
    {
      xcb_atom_t atom = window_type_atoms.atoms[i];
      if (atom == g_ewmh._NET_WM_WINDOW_TYPE_NORMAL)
      {
        window_type = WindowType_Normal;
        break;
      }
      if (atom == g_ewmh._NET_WM_WINDOW_TYPE_DIALOG || atom == g_ewmh._NET_WM_WINDOW_TYPE_UTILITY ||
          atom == g_ewmh._NET_WM_WINDOW_TYPE_TOOLBAR || atom == g_ewmh._NET_WM_WINDOW_TYPE_SPLASH ||
          atom == g_ewmh._NET_WM_WINDOW_TYPE_MENU)
      {
        window_type = WindowType_Floating;
        break;
      }
      if (atom == g_ewmh._NET_WM_WINDOW_TYPE_DOCK)
      {
        window_type = WindowType_Docked;
        break;
      }
    }
    xcb_ewmh_get_atoms_reply_wipe(&window_type_atoms);
  }
  return window_type;
}

internal void HandleMapRequest(xcb_map_request_event_t *event)
{
  Debugf("handle map request for window %d", event->window);
  WindowType window_type = Xcb_WindowType(event->window);
  Debugf("window type: %d", window_type);

  xcb_get_property_cookie_t normal_hints_cookie = xcb_get_property(
      g_conn, 0, event->window, XCB_ATOM_WM_NORMAL_HINTS, XCB_GET_PROPERTY_TYPE_ANY, 0, 1024);
  xcb_get_property_cookie_t wm_class_cookie = xcb_get_property(
      g_conn, 0, event->window, XCB_ATOM_WM_CLASS, XCB_GET_PROPERTY_TYPE_ANY, 0, 1024);

  xcb_get_property_reply_t *normal_hints_reply =
      xcb_get_property_reply(g_conn, normal_hints_cookie, NULL);
  if (!normal_hints_reply || normal_hints_reply->type != XCB_ATOM_WM_SIZE_HINTS)
  {
    Error("Failed to retrieve size hints");
  }
  else
  {
    xcb_size_hints_t *size_hints = (xcb_size_hints_t *)xcb_get_property_value(normal_hints_reply);
    Debugf("User specified location: %d, %d", size_hints->x, size_hints->y);
    Debugf("User specified size: %d by %d", size_hints->width, size_hints->height);
    Debugf("Program specified minimum size: %d by %d", size_hints->base_width,
           size_hints->base_height);
  }

  xcb_get_property_reply_t *wm_class_reply = xcb_get_property_reply(g_conn, wm_class_cookie, NULL);
  if (!wm_class_reply || wm_class_reply->type != XCB_ATOM_STRING)
  {
    Error("Failed to retrieve wm class");
  }
  else
  {
    String class_name = StrCstr((char *)xcb_get_property_value(wm_class_reply));
    Debugf("class name: %.*s", StrFmtVal(class_name));
    char  *instance_name_cstr = (char *)class_name.data + class_name.size + 1;
    String instance_name      = StrCstr(instance_name_cstr);
    Debugf("instance name: %.*s", StrFmtVal(instance_name));
  }

  xcb_map_window(g_conn, event->window);
  xcb_flush(g_conn);
}

internal bool Xcb_PollEvents()
{
  bool ok = true;
  for (xcb_generic_event_t *generic_event = xcb_poll_for_event(g_conn); generic_event != NULL;
       generic_event                      = xcb_poll_for_event(g_conn))
  {
    int event_type = generic_event->response_type & ~0x80;
    if (event_type != XCB_MOTION_NOTIFY)
    {
      Debugf("Event type %d", event_type);
    }
    // handle screen change
    if (event_type == g_randr_base + XCB_RANDR_SCREEN_CHANGE_NOTIFY)
    {
    }
    // handle errors
    if (event_type == 0)
    {
      xcb_generic_error_t *error = (xcb_generic_error_t *)generic_event;
      switch (error->error_code)
      {
#define _ERROR_BRANCH(branch)                                                                      \
  case branch:                                                                                     \
  {                                                                                                \
    Error("Polled a XCB error: " #branch);                                                         \
    break;                                                                                         \
  }
        _ERROR_BRANCH(XCB_WINDOW)
        _ERROR_BRANCH(XCB_PIXMAP)
        _ERROR_BRANCH(XCB_ATOM)
        _ERROR_BRANCH(XCB_CURSOR)
        _ERROR_BRANCH(XCB_FONT)
        _ERROR_BRANCH(XCB_MATCH)
        _ERROR_BRANCH(XCB_DRAWABLE)
        _ERROR_BRANCH(XCB_ACCESS)
        _ERROR_BRANCH(XCB_ALLOC)
        _ERROR_BRANCH(XCB_COLORMAP)
        _ERROR_BRANCH(XCB_G_CONTEXT)
        _ERROR_BRANCH(XCB_ID_CHOICE)
        _ERROR_BRANCH(XCB_NAME)
        _ERROR_BRANCH(XCB_LENGTH)
        _ERROR_BRANCH(XCB_IMPLEMENTATION)
#undef _ERROR_BRANCH
      }
    }
    switch (event_type)
    {
    case XCB_MAP_REQUEST:
      HandleMapRequest((xcb_map_request_event_t *)generic_event);
      break;
    }
  }
  return ok;
}