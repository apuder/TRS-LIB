
#include <trs-lib.h>


const char* screen_color_items[] = {
  "WHITE",
  "GREEN",
  "AMBER",
   NULL};

static uint8_t screen_color = 0;

static bool option_1 = true;
static bool option_2 = true;
static bool option_3 = false;

static char name[20 + 1] = {"Mickey"};
static bool name_dirty;

static form_item_t my_form_items[] = {
	FORM_ITEM_HEADER("SECTION 1:"),
	FORM_ITEM_CHECKBOX("First option", &option_1, NULL),
	FORM_ITEM_CHECKBOX("Second option", &option_2, NULL),
	FORM_ITEM_SELECT("Screen color", &screen_color, screen_color_items, NULL),
	FORM_ITEM_HEADER(""),
	FORM_ITEM_HEADER("SECTION 2:"),
	FORM_ITEM_CHECKBOX("Third option", &option_3, NULL),
	FORM_ITEM_INPUT("Name", name, sizeof(name), 0, &name_dirty),
	FORM_ITEM_END
};

static form_t my_form = {
	.title = "Demo Form",
	.form_items = my_form_items
};

void form_demo() {
  uint8_t status;

  status = form(&my_form, false);

  if (status == FORM_ABORT) {
    // User hit <BREAK>
  }
  if (name_dirty) {
    // User edited the name
  }
}
