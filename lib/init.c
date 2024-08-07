
#include "screen.h"
#include "trs-io.h"
#include "init.h"


static void dummy_screen_update_range(uint8_t* from, uint8_t* to)
{
  // Do nothing
}

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
             SCREEN_WIDTH, SCREEN_HEIGHT,
             dummy_screen_update_range);

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

  init_dos();
}

void exit_trs_lib()
{
  if (!is_m3()) {
    // Reset BREAK key on M1
    POKE(0x400c, break_key);
  }
  if (did_use_animation()) {
    set_screen_to_background();
    memmove(screen.current, screen_original_content, SCREEN_WIDTH * SCREEN_HEIGHT);
    screen_show(true);
  }
}
