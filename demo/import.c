
#include <dos.h>

int main(const char* args)
{
  fcb_t fcb;
  buffer_t buffer;

  print("Trying to open file '%s'\n", args);
  err_t err = dos_fspec(args, &fcb);
  print("err: %d\n", err);
  err = dos_open(&fcb, &buffer, 0);
  print("err: %d\n", err);
  return 0;
}

