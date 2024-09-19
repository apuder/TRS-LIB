
#include <dos.h>
#include <key.h>
#include <stdio.h>



static void usage()
{
  printf("Usage: REMOVE <file>\n");
}

int main(const char* args)
{
  fcb_t fcb;

  dos_err_t err = dos_fspec(args, &fcb);
  if (err != NO_ERR) {
    usage();
    return -1;
  }
  err = dos_open(&fcb, NULL, 0);
  if (err != 0 && err != ERR_LRL) {
    printf("File not found\n");
    return -1;
  }

  err = dos_remove(&fcb);
  if (err != NO_ERR) {
    printf("Error removeing file\n");
    return -1;
  }

  return 0;
}

