#include "config.h"
#include "xcb.h"

#include "../core/core.c"
#include "config.c"
#include "xcb.c"
#include "workspace.c"
#include "monitor.c"
#include "randr.c"
#include "window.c"

int main(void)
{
  Arena    *arena     = ArenaInit(Gigabytes(1));
  Allocator allocator = ArenaAllocator(arena);

  // String root_dir    = StrLit(PROJECT_DIR);
  String wm_name = StrLit("X11 Handmade WM");

  Config config  = {0};
  if (!LoadConfig(allocator, &config) || !Xcb_Init(allocator, wm_name))
  {
    Error("Failed to complete an initialization step");
    return 1;
  }
  // PrintConfig(allocator, &config);

  const u64 frame_time = Seconds(1) / 60;
  bool      running    = true;
  for (; running;)
  {
    u64 frame_start = TimeNow();

    Temp temp = TempBegin(arena);
    {
      if(!Xcb_PollEvents())
      {
        running = false;
      }
    }
    TempEnd(temp);

    u64 frame_end = TimeNow();
    u64 diff      = frame_end - frame_start;
    if (diff < frame_time)
    {
      Sleep(frame_time - diff);
    }
  }

  Xcb_Deinit();
  ArenaDeinit(arena);
  return 0;
}