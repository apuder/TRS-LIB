
#include "screen.h"
#include "trs-io.h"

static const uint8_t scroll_increment = 5;

SCREEN screen;


#ifndef ESP_PLATFORM

#define POKE(a, b) *((volatile uint8_t*) (a)) = (b)
#define PEEK(a) *((volatile uint8_t*) (a))

static uint8_t screen_original_content[SCREEN_WIDTH * SCREEN_HEIGHT];

static uint8_t break_key;

void init_trs_lib()
{
  uint8_t ch;

  // Hardcoded for M1/MIII
  static uint8_t background_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

  memmove(screen_original_content, (void*) SCREEN_BASE, SCREEN_WIDTH * SCREEN_HEIGHT);
  set_screen(SCREEN_BASE, background_buffer,
	     SCREEN_WIDTH, SCREEN_HEIGHT);

  screen.is_uc = false;

  if (!is_m3()) {
    // Enable BREAK key on L2/DOS
    break_key = PEEK(0x400c);
    POKE(0x400c, 0xc9);
    
    // Determine if the screen has lowercase by writing/reading screen memory.
    // An unmodified M1 will read back 32 instead of 96.
    ch = PEEK(SCREEN_BASE);
    POKE(SCREEN_BASE, 96);
    screen.is_uc = PEEK(SCREEN_BASE) != 96;
    POKE(SCREEN_BASE, ch);
  }
}

void exit_trs_lib()
{
  if (!is_m3()) {
    // Reset BREAK key on M1
    POKE(0x400c, break_key);
  }
  set_screen_to_background();
  memmove(screen.current, screen_original_content, SCREEN_WIDTH * SCREEN_HEIGHT);
  screen_show(true);
}
#endif

static void dummy_screen_update_range(uint8_t* from, uint8_t* to)
{
  // Do nothing
}

void screen_update_full()
{
  if (screen.current == screen.screen_base) {
    (*screen.screen_update_range)(screen.screen_base,
				  screen.screen_base + (screen.width * screen.height));
  }
}

void screen_update_range(uint8_t* from, uint8_t* to)
{
  if (screen.current == screen.screen_base) {
    (*screen.screen_update_range)(from, to);
  }
}

void set_screen(uint8_t* screen_base,
		uint8_t* background,
		uint8_t width, uint8_t height)
{
  screen.screen_base = screen_base;
  screen.width = width;
  screen.height = height;
  screen.background = background;
  screen.screen_update_range = dummy_screen_update_range;
  set_screen_to_foreground();
}

void set_screen_callback(screen_update_range_t screen_update_range)
{
  screen.screen_update_range = screen_update_range;
}

void set_screen_to_foreground()
{
  screen.current = screen.screen_base;
}

void set_screen_to_background()
{
  screen.current = screen.background;
}

static void screen_show_from_right()
{
  uint8_t y, cx;
  int8_t left;
  uint8_t* from;
  uint8_t* to;

  cx = 0;
  left = screen.width;

  while (left > 0) {
    uint8_t d = (left >= scroll_increment) ? scroll_increment : left;
    for (y = 0; y < screen.height; y++) {
      to = screen.screen_base + y * screen.width;
      from = to + d;
      memmove(to, from, screen.width - d);

      to = screen.screen_base + (y + 1) * screen.width - d;
      from = screen.background + y * screen.width + cx;
      memmove(to, from, d);
    }
    screen_update_full();
    cx += d;
    left -= scroll_increment;
  }
}

static void screen_show_from_left() {
  uint8_t y;
  int8_t left;
  uint8_t* from;
  uint8_t* to;

  left = screen.width;

  while (left > 0) {
    uint8_t d = (left >= scroll_increment) ? scroll_increment : left;
    for (y = 0; y < screen.height; y++) {
      from = screen.screen_base + y * screen.width;
      to = from + d;
      memmove(to, from, screen.width - d);

      from = screen.background + y * screen.width + left - d;
      to = screen.screen_base + y * screen.width;
      memmove(to, from, d);
    }
    left -= scroll_increment;
    screen_update_full();
  }
}
      
void screen_show(bool from_left) {
  set_screen_to_foreground();
  if (from_left) {
    screen_show_from_left();
  } else {
    screen_show_from_right();
  }
}
