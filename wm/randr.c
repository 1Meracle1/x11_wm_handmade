#include "randr.h"
#include "monitor.h"
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/randr.h>

bool g_has_randr_1_5 = false;

internal bool RandrQueryOutputs(Allocator allocator, xcb_connection_t *conn, xcb_window_t root);

internal bool RandrInit(Allocator allocator, xcb_connection_t *conn, xcb_window_t root,
                        int *randr_event_base)
{
  bool                               ok       = true;
  const xcb_query_extension_reply_t *extreply = xcb_get_extension_data(conn, &xcb_randr_id);
  if (!extreply->present)
  {
    Error("RandR is not present");
    ok = false;
  }

  xcb_randr_query_version_reply_t *randr_version;
  if (ok)
  {
    *randr_event_base = extreply->first_event;

    xcb_generic_error_t *err;
    randr_version = xcb_randr_query_version_reply(
        conn, xcb_randr_query_version(conn, XCB_RANDR_MAJOR_VERSION, XCB_RANDR_MINOR_VERSION),
        &err);
    if (err != NULL)
    {
      Errorf("Could not query RandR version: X11 error code %d", err->error_code);
      free(err);
      ok = false;
    }
  }
  g_has_randr_1_5 = (randr_version->major_version >= 1) && (randr_version->minor_version >= 5);
  Infof("Randr version major: %d, minor: %d", randr_version->major_version,
        randr_version->minor_version);
  free(randr_version);

  ok = RandrQueryOutputs(allocator, conn, root);
  xcb_randr_select_input(conn, root,
                         XCB_RANDR_NOTIFY_MASK_SCREEN_CHANGE | XCB_RANDR_NOTIFY_MASK_OUTPUT_CHANGE |
                             XCB_RANDR_NOTIFY_MASK_CRTC_CHANGE |
                             XCB_RANDR_NOTIFY_MASK_OUTPUT_PROPERTY);
  return ok;
}

internal bool RandrQueryOutputs(Allocator allocator, xcb_connection_t *conn, xcb_window_t root)
{
  bool ok = true;
  if (g_has_randr_1_5)
  {
    xcb_generic_error_t                  *err;
    const xcb_randr_get_monitors_reply_t *reply =
        xcb_randr_get_monitors_reply(conn, xcb_randr_get_monitors(conn, root, true), &err);
    if (err)
    {
      Errorf("Failed to get Randr monitors, error code: %d", err->error_code);
      ok = false;
    }
    else
    {
      Infof("Randr monitors found: %d", xcb_randr_get_monitors_monitors_length(reply));
      for (xcb_randr_monitor_info_iterator_t iter = xcb_randr_get_monitors_monitors_iterator(reply);
           iter.rem; xcb_randr_monitor_info_next(&iter))
      {
        xcb_randr_monitor_info_t  *monitor_info = iter.data;
        xcb_get_atom_name_reply_t *atom_reply =
            xcb_get_atom_name_reply(conn, xcb_get_atom_name(conn, monitor_info->name), &err);
        if (err != NULL)
        {
          Errorf("Could not get RandR monitor name: X11 error code %d", err->error_code);
          free(err);
          continue;
        }
        String name =
            Str(xcb_get_atom_name_name(atom_reply), xcb_get_atom_name_name_length(atom_reply));
        Debugf("Randr monitor name: %.*s, primary: %d, output: %d, x: %d, y: %d, width: %d, height: %d",
               StrFmtVal(name), monitor_info->primary, monitor_info->nOutput, monitor_info->x,
               monitor_info->y, monitor_info->width, monitor_info->height);
        AddMonitor(allocator, name, monitor_info->primary == 1, monitor_info->nOutput,
                   monitor_info->x, monitor_info->y, monitor_info->width, monitor_info->height);
      }
    }
  }
  else
  {
    ok = false;
  }

  return ok;
}