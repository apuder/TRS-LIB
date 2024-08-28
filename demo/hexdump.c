
#include <dos.h>
#include <key.h>
#include <stdio.h>


static uint16_t addr = 0;
static uint8_t buf[8];
static uint8_t cnt = 0;
static uint8_t line_cnt = 0;

#define SIZE_BUF sizeof(buf)

static void usage()
{
  printf("Usage: HEXDUMP <file>\n");
}

static bool next_line()
{
  char ch = 0;
  if (line_cnt++ == 15) {
    ch = get_key();
    line_cnt = 0;
  }
  printf("\n");
  return ch == KEY_BREAK;
}

static bool show()
{
  if (cnt == 0) {
    return false;
  }
  printf("%04X: ", addr);
  for (int i = 0; i < cnt; i++) {
    printf("%02X ", buf[i]);
  }
  for (int i = 0; i < SIZE_BUF - cnt; i++) {
    printf("   ");
  }
  for (int i = 0; i < cnt; i++) {
    char ch = buf[i];
    printf("%c", (ch < ' ' || ch >= 192) ? '.' : ch);
  }
  addr += cnt;
  cnt = 0;
  return next_line();
}

static bool dump(uint8_t b)
{
  buf[cnt++] = b;
  if (cnt == SIZE_BUF) {
    return show();
  }
  return false;
}

int main(const char* args)
{
  fcb_t fcb;
  sector_t sector;

  dos_err_t err = dos_fspec(args, &fcb);
  if (err != NO_ERR) {
    usage();
    return -1;
  }
  err = dos_open(&fcb, &sector, 0);
  if (err != 0 && err != ERR_LRL) {
    printf("File not found\n");
    return -1;
  }

  uint16_t ern = dos_getern(&fcb);
  bool abort = false;

  while (ern-- != 0) {
    err = dos_read(&fcb, NULL);
    if (err != NO_ERR) {
      break;
    }
    uint16_t count = (ern == 0) ? fcb.fname[8] : 256;
    if (count == 0) {
      count = 256;
    }
    for (int i = 0; i < count; i++) {
      abort = dump(sector[i]);
      if (abort) {
        break;
      }
    }
    if (abort) {
      break;
    }
  }

  if (!abort) {
    show();
  }

  dos_close(&fcb);

  return 0;
}

