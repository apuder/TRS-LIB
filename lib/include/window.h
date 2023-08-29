
#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "defs.h"

typedef struct {
  uint8_t x;
  uint8_t y;
  uint8_t w;
  uint8_t h;
  uint8_t cx;
  uint8_t cy;
  bool clip_line;
  bool clip_window;
} window_t;

void init_window(window_t* wnd, uint8_t x, uint8_t y, int8_t w, int8_t h);
void wnd_set_clip_line(window_t* wnd, bool flag);
void wnd_set_clip_window(window_t* wnd, bool flag);
void wnd_print_str(window_t* wnd, const char* str);
void wnd_print(window_t* wnd, const char* fmt, ...);
void wnd_cls(window_t* wnd);
void wnd_printn(window_t* wnd, char* p, uint16_t n);
void wnd_clear_eol(window_t* wnd);
void wnd_scroll_down(window_t* wnd);
void wnd_scroll_up(window_t* wnd);
void wnd_cr(window_t* wnd);
void wnd_goto(window_t* wnd, uint8_t x, uint8_t y);
uint8_t wnd_get_x(window_t* wnd);
uint8_t wnd_get_y(window_t* wnd);
void wnd_popup(const char* fmt, ...);

#endif
