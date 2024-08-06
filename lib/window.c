
#include "window.h"
#include "screen.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

char print_buf[160];

static uint8_t* get_screen_pos0(window_t* wnd, uint8_t x, uint8_t y) {
  return (screen.current + (wnd->y + y) * screen.width +
		     (wnd->x + x));
}

static uint8_t* get_screen_pos(window_t* wnd) {
  return get_screen_pos0(wnd, wnd->cx, wnd->cy);
}

void init_window(window_t* wnd, uint8_t x, uint8_t y, int8_t w, int8_t h) {
  wnd->cx = 0;
  wnd->cy = 0;
  wnd->x = x;
  wnd->y = y;
  wnd->w = (w > 0) ? w : screen.width + w - x;
  wnd->h = (h > 0) ? h : screen.height + h - y;
  wnd->clip_line = false;
  wnd->clip_window = false;
}

void wnd_set_clip_line(window_t* wnd, bool flag) {
  wnd->clip_line = flag;
}

void wnd_set_clip_window(window_t* wnd, bool flag) {
  wnd->clip_window = flag;
}

void wnd_cr(window_t* wnd) {
  wnd->cx = 0;
  wnd->cy++;
}

void wnd_goto(window_t* wnd, uint8_t x, uint8_t y) {
  wnd->cx = x;
  wnd->cy = y;
}

uint8_t wnd_get_x(window_t* wnd) {
  return wnd->cx;
}

uint8_t wnd_get_y(window_t* wnd) {
  return wnd->cy;
}

void wnd_print_str(window_t* wnd, const char* str) {
  const char* start_of_word;
  uint8_t len;
  uint8_t* p, from;

  while (*str != '\0') {
    const char* next = str;
    uint8_t* from = get_screen_pos(wnd);

    if (*next == '\n') {
      if (wnd->cy + 1 == wnd->h) {
        if (wnd->clip_window) {
          // We are in the last line. Just exit
          return;
        }
        wnd_scroll_up(wnd);
      } else {
        wnd->cx = 0;
        wnd->cy++;
      }
      str++;
      continue;
    }

    while ((*next != '\0') && (*next == ' ')) {
      next++;
    }

    start_of_word = next;

    while ((*next != '\0') && (*next != ' ') && (*next != '\n')) {
      next++;
    }

    len = next - str;

    // If we are in the last line, add space for 3 for ellipses
    if (wnd->cy + 1 == wnd->h) {
      len += 3;
    }
    
    // Fits in current line?
    if (wnd->cx + len > wnd->w) {
      // No. Do we have another line?
      if (wnd->clip_line || ((wnd->cy + 1 == wnd->h) && wnd->clip_window)) {
        // No. Stop outputting and print "..."
        from = p = get_screen_pos(wnd);
        *p++ = '.';
        *p++ = '.';
        *p++ = '.';
        screen_update_range(from, p);
        return;
      }
      // Yes. Continue on next line
      if (wnd->cy + 1 == wnd->h) {
        wnd_scroll_up(wnd);
      } else {
        wnd->cy++;
        wnd->cx = 0;
      }

      str = start_of_word;
    }

    from = p = get_screen_pos(wnd);
  
    while (str != next) {
      *p++ = SCREEN_TO_UC(screen.is_uc, *str);
      str++;
      wnd->cx++;
    }
    screen_update_range(from, p);
  }
} 

void wnd_print(window_t* wnd, const char* fmt, ...) {
  va_list argp;

  va_start(argp, fmt);
  vsprintf(print_buf, fmt, argp);
  va_end(argp);

  wnd_print_str(wnd, print_buf);
}

static void window_screen_update(window_t* wnd)
{
  uint8_t* from = get_screen_pos0(wnd, 0, 0);
  uint8_t* to = get_screen_pos0(wnd, wnd->w - 1, wnd->h - 1) + 1;
  screen_update_range(from, to);
}

void wnd_cls(window_t* wnd) {
  uint8_t x, y;

  for (y = 0; y < wnd->h; y++) {
    uint8_t* p = get_screen_pos0(wnd, 0, y);

    for (x = 0; x < wnd->w; x++) {
      *p++ = ' ';
    }
  }
  wnd->cx = 0;
  wnd->cy = 0;
  window_screen_update(wnd);
}  

void wnd_printn(window_t* wnd, char* p, uint16_t n)
{
  uint16_t x;

  uint8_t* from = get_screen_pos(wnd);
  uint8_t* to = from;
  for (x = 0; x < n; x++) {
    if (*p == '\0') {
      break;
    }
    *to++ = *p++;
  }
  screen_update_range(from, to);
}

void wnd_clear_eol(window_t* wnd)
{
  uint8_t x;

  uint8_t* from = get_screen_pos(wnd);
  uint8_t* to = from;
  for (x = wnd->cx; x < wnd->w; x++) {
    *to++ = ' ';
  }
  screen_update_range(from, to);  
}

void wnd_scroll_up(window_t* wnd) {
  int8_t x, y;
  uint8_t* clear = NULL;

  for (y = 0; y < wnd->h - 1; y++) {
    uint8_t* to = get_screen_pos0(wnd, 0, y);
    uint8_t* from = to + screen.width;
    clear = from;
    
    for (x = 0; x < wnd->w; x++) {
      *to++ = *from++;
    }
  }
 
  for (x = 0; x < wnd->w; x++) {
    *clear++ = ' ';
  }

  wnd->cx = 0;
  wnd->cy = wnd->h - 1;
  window_screen_update(wnd);
}

void wnd_scroll_down(window_t* wnd) {
  int8_t x, y;
  uint8_t* clear = NULL;

  for (y = wnd->h - 1; y > 0; y--) {
    uint8_t* to = get_screen_pos0(wnd, 0, y);
    uint8_t* from = to - screen.width;
    clear = from;
    
    for (x = 0; x < wnd->w; x++) {
      *to++ = *from++;
    }
  }
 
  for (x = 0; x < wnd->w; x++) {
    *clear++ = ' ';
  }

  wnd->cx = 0;
  wnd->cy = 0;
  window_screen_update(wnd);
}

void wnd_popup(const char* fmt, ...) {
  va_list argp;
  char msg[84];
  int len;
  int cx;
  int cy;
  uint8_t* p0;
  uint8_t* p1;
  uint8_t* p2;
  uint8_t* p0_from;
  uint8_t* p1_from;
  uint8_t* p2_from;
  int x, y;
  
  va_start(argp, fmt);
  vsprintf(msg, fmt, argp);
  va_end(argp);

  len = strlen(msg) + 2;
  cx = (screen.width - len) / 2;
  cy = screen.height / 2 - 1;
  p0 = screen.current + cy * screen.width + cx;
  p1 = p0 + screen.width;
  p2 = p1 + screen.width;
  p0_from = p0;
  p1_from = p1;
  p2_from = p2;

  for (x = 0; x < len; x++) {
    *p0++ = 176;
    *p2++ = 131;
    if (x == 0 || x == len - 1) {
      *p1 = 191;
    } else {
      *p1 = SCREEN_TO_UC(screen.is_uc, msg[x - 1]);
    }
    p1++;
  }
  screen_update_range(p0_from, p0);
  screen_update_range(p1_from, p2);
  screen_update_range(p2_from, p2);  
}
