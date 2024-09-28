
#include <dos.h>
#include <stdio.h>



static void usage()
{
  printf("Usage: MV <from> <to>\n");
}


int main(char* args)
{
  fcb_t from_fcb;
  fcb_t to_fcb;

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

  dos_err_t err = dos_rename(from_fn, to_fn);
  if (err != 0) {
    printf("Error renaming file (%d)\n", err);
    return -1;
  }

  return 0;
}

