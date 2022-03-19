
#include "demo.h"
#include <trs-lib.h>

static window_t wnd;

static char name[20 + 1] = {""};

static form_item_t my_form_items[] = {
	FORM_ITEM_INPUT("Filename", name, sizeof(name) - 1, 0, NULL),
	FORM_ITEM_END
};

static form_t my_form = {
	.title = "TRS-FS Demo",
	.form_items = my_form_items
};


void demo_trs_fs()
{
  static const char* path = "CAT.BAS";
  static char buf[256];
  int8_t fd;
  uint16_t br;
  uint8_t status;
  int i;

  set_screen_to_background();
  init_window(&wnd, 0, 3, 0, 0);
  header(my_form.title);
  wnd_print(&wnd, "This demo uses TRS-FS's file system API to print the contents of "
                  "a file residing on the SMB share or MicroSD card.\n\n");

  status = form_inline(&wnd, &my_form, false);

  if (status == FORM_ABORT) {
    return;
  }

  wnd_cls(&wnd);

  fd = trs_fs_open(name, FA_READ);

  if (fd < 0) {
    wnd_popup("File not found");
    get_key();
    return;
  }
  
  wnd_cls(&wnd);

  do {
    trs_fs_read(fd, buf, sizeof(buf), &br);
    for(i = 0; i < br; i++) {
      wnd_print(&wnd, "%c", buf[i]);
    }
  } while(br != 0);
  trs_fs_close(fd);
  get_key();
}

