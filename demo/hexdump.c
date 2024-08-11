
#include <dos.h>
#include <stdio.h>


static uint16_t addr = 0;
static uint8_t buf[8];
static uint8_t cnt = 0;

#define SIZE_BUF sizeof(buf)

static void usage()
{
  printf("Usage: HEXDUMP <file>\n");
}

static void show()
{
  if (cnt == 0) {
    return;
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
    printf("%c", (ch < ' ') ? '.' : ch);
  }
  printf("\n");
  addr += cnt;
  cnt = 0;
}

static void dump(uint8_t b)
{
  buf[cnt++] = b;
  if (cnt == SIZE_BUF) {
    show();
  }
}

int main(const char* args)
{
  fcb_t fcb;
  buffer_t buffer;

  err_t err = dos_fspec(args, &fcb);
  if (err != NO_ERR) {
    usage();
    return -1;
  }
  err = dos_open(&fcb, &buffer, 0);
  if (err != 0 && err != ERR_LRL) {
    printf("File not found\n");
    return -1;
  }

  uint16_t ern = dos_getern(&fcb);

  do {
    ern--;
    err = dos_read(&fcb, NULL);
    if (err == ERR_EOF) {
      break;
    }
    uint16_t count = (ern == 0) ? fcb.fname[8] : 256;
    if (count == 0) {
      count = 256;
    }
    for (int i = 0; i < count; i++) {
      dump(buffer[i]);
    }
  } while(ern != 0);
  show();

  dos_close(&fcb);

  return 0;
}

