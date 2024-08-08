
#include <dos.h>

int main(const char* args)
{
  fcb_t fcb;
  buffer_t buffer;

  print("Trying to open file '%s'\n", args);
  err_t err = dos_fspec(args, &fcb);
  print("err fspec: %d\n", err);
  if (err != 0) {
    return -1;
  }
  err = dos_open(&fcb, &buffer, 0);
  print("err open: %d\n", err);
  if (err != 0 && err != 42 /*LRLERR*/) {
    return -1;
  }
  uint16_t ern = dos_getern(&fcb);
  print("ern: %d\n", ern);
  err = dos_read(&fcb, NULL);
  print("err read: %d\n", err);
  print("fcb[8]: %d\n", fcb.fname[8]);
  for (int i = 0; i < 10; i++) print("%c", buffer[i]);
  print("\n");
  err = dos_read(&fcb, NULL);
  print("err read: %d\n", err);
  print("fcb[8]: %d\n", fcb.fname[8]);
  return 0;
}

