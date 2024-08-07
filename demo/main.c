
#include "demo.h"
#include <trs-lib.h>

#define MENU_ABOUT 0
#define MENU_INPUT_FORM 1
#define MENU_TRS_NIC 2
#define MENU_TRS_FS 3
#define MENU_EXIT 4

static menu_item_t main_menu_items[] = {
  MENU_ITEM(MENU_ABOUT, "About"),
  MENU_ITEM(MENU_INPUT_FORM, "Form demo"),
  MENU_ITEM(MENU_TRS_NIC, "TRS-NIC demo"),
  MENU_ITEM(MENU_TRS_FS, "TRS-FS demo"),
  MENU_ITEM(MENU_EXIT, "Exit"),
  MENU_ITEM_END
};

static menu_t main_menu = {
  .title = "TRS-LIB Demo",
  .items = main_menu_items
};


int main(const char* args) {
  bool show_from_left = false;
  uint8_t status;
  while(true) {
    status = menu(&main_menu, show_from_left, true);
    if (status == MENU_ABORT || status == MENU_EXIT) {
      break;
    }
    switch (status) {
    case MENU_ABOUT:
      about();
      break;
    case MENU_INPUT_FORM:
      form_demo();
      break;
    case MENU_TRS_NIC:
      demo_trs_nic();
      break;
    case MENU_TRS_FS:
      demo_trs_fs();
      break;
    }
    show_from_left = true;
  }

  return 0;
}
