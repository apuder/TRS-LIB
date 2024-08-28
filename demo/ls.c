
#include <dos.h>
#include <key.h>
#include <stdio.h>

static dir_buf_t dir_buf;
static uint8_t line_cnt = 0;

static void usage()
{
  printf("Usage: LS [0-7]\n");
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

int main(const char* args)
{
  uint8_t drive = args[0] - '0';
  if (drive > 7 || args[1] != '\0') {
    usage();
    return -1;
  }

  dos_err_t err = dos_ramdir(0, drive, &dir_buf);
  if (err != NO_ERR) {
    printf("Error: %d\n", err);
    return -1;
  }

  int i = 0;

  while (dir_buf[i].fname[0] != '+') {
    dir_t* d = &dir_buf[i];
    uint16_t lrl = (d->lrl == 0) ? 256 : d->lrl;
    uint16_t n = d->ern;
    if (d->eof != 0) {
      n--;
    }
    uint16_t size = lrl * n + d->eof;
    printf("%.15s  %6d", d->fname, size);
    if (next_line()) {
      break;
    }
    i++;
  }

  return 0;
}

