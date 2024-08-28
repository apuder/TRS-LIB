
#include <dos.h>
#include <stdio.h>


static uint16_t addr = 0;
static uint8_t buf[8];
static uint8_t cnt = 0;

#define SIZE_BUF sizeof(buf)

static void usage()
{
  printf("Usage: CP <from> <to>\n");
}


int main(char* args)
{
  fcb_t from_fcb;
  fcb_t to_fcb;
  sector_t sector;

  if (*args == '\0') {
    usage();
    return -1;
  }
  const char* from_fn = args;

  while (*args != ' ' && *args != '\0') {
    args++;
  }
  if (*args == ' ') {
    *args++ = '\0';
  }
  while (*args == ' ' && *args != '\0') {
    *args++;
  }
  if (*args == '\0') {
    usage();
    return -1;
  }
  const char* to_fn = args;

  // Open <from>
  dos_err_t err = dos_fspec(from_fn, &from_fcb);
  if (err != NO_ERR) {
    printf("Illegal <from> name\n");
    usage();
    return -1;
  }
  err = dos_open(&from_fcb, &sector, 0);
  if (err != 0 && err != ERR_LRL) {
    printf("File '%s' not found\n", from_fn);
    return -1;
  }

  // Create <to>
  err = dos_fspec(to_fn, &to_fcb);
  if (err != NO_ERR) {
    printf("Illegal <to> name\n");
    usage();
    return -1;
  }
  err = dos_init(&to_fcb, &sector, 0);
  if (err != 0 && err != ERR_LRL) {
    printf("File '%s' could not be created (%d)\n", to_fn, err);
    return -1;
  }

  uint16_t ern = dos_getern(&from_fcb);

  while (ern-- != 0) {
    err = dos_read(&from_fcb, NULL);
    if (err != NO_ERR) {
      break;
    }
    err = dos_write(&to_fcb, NULL);
    if (err != NO_ERR) {
      printf("Write err: %d\n", err);
      break;
    }
  }

  // Copy EOF
  to_fcb.fname[8] = from_fcb.fname[8];
  // Close files
  dos_close(&from_fcb);
  dos_setern(&to_fcb);
  dos_close(&to_fcb);

  return 0;
}

