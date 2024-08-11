
#include <dos.h>
#include <stdio.h>

static dir_buf_t dir_buf;

static void usage()
{
  printf("Usage: LS [0-7]\n");
}

int main(const char* args)
{
  uint8_t drive = args[0] - '0';
  if (drive > 7 || args[1] != '\0') {
    usage();
    return -1;
  }

  err_t err = dos_ramdir(0, drive, &dir_buf);
  if (err != NO_ERR) {
    printf("Error: %d\n", err);
    return -1;
  }

  int i = 0;

  while (dir_buf[i].fname[0] != '+') {
    dir_t* d = &dir_buf[i];
    uint16_t lrl = (d->lrl == 0) ? 256 : d->lrl;
    uint16_t size = lrl * (d->lst_sec - 1) + d->eof;
    printf("%.15s  %6d\n", d->fname, size);
    i++;
  }

  return 0;
}

