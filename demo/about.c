
#include "demo.h"
#include <trs-lib.h>

static window_t wnd;

void about()
{
  set_screen_to_background();
  init_window(&wnd, 0, 3, 0, 0);
  header("About TRS-LIB");
  wnd_print(&wnd, "TRS-LIB is a small, C-based library that offers simple UI elements for TRS-80 applications. ");
  wnd_print(&wnd, "It can be compiled using sdcc.\n\n");
  wnd_print(&wnd, "Details at: https://github.com/apuder/TRS-LIB");
  screen_show(false);
  get_key();
}

