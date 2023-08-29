
#ifndef __FORM_H__
#define __FORM_H__

#include "defs.h"
#include "window.h"
#include "key.h"
#include "panic.h"

#define FORM_OK 0
#define FORM_ABORT 1

#define FORM_TYPE_END 0
#define FORM_TYPE_HEADER 1
#define FORM_TYPE_INPUT 2
#define FORM_TYPE_CHECKBOX 3
#define FORM_TYPE_SELECT 4
#define FORM_TYPE_FORM 5

#define FORM_CURSOR "\260"

#define FORM_ITEM_HEADER(l) { .type = FORM_TYPE_HEADER, .label = l }
#define FORM_ITEM_END { .type = FORM_TYPE_END }

#define FORM_ITEM_CHECKBOX(l, c, d)	{ .type = FORM_TYPE_CHECKBOX, .label = l, .dirty = d, .u = {.checkbox = {.checked = c}}}
#define FORM_ITEM_SELECT(l, sel, it, d) { .type = FORM_TYPE_SELECT, .label = l, .dirty = d, .u = {.select = {.selected = sel, .items = it, .items_ptr = NULL}}}
#define FORM_ITEM_SELECT_PTR(l, sel, itp, d) { .type = FORM_TYPE_SELECT, .label = l, .dirty = d, .u = {.select = {.selected = sel, .items = NULL, .items_ptr = itp}}}
#define FORM_ITEM_INPUT(l, b, n, w, d) { .type = FORM_TYPE_INPUT, .label = l, .dirty = d, .u = { .input = {.buf = b, .len = n, .width = w }}}

struct _form_t;

typedef struct {
  char* buf;
  uint8_t len;
  uint8_t width;
} form_input_t;

typedef struct {
  bool* checked;
} form_checkbox_t;

#define SELECTION_MAX_LEN 256
#define SELECTION_MAX_ITEMS 20

typedef struct {
  uint8_t* selected;
  const char** items;
  const char*** items_ptr;
  // Internal
  uint8_t first;
  uint8_t current;
  uint8_t items_str_left;
  uint8_t num_items;
  char items_str[SELECTION_MAX_LEN];
  uint8_t items_start_idx[SELECTION_MAX_ITEMS];
  uint8_t items_end_idx[SELECTION_MAX_ITEMS];
} form_select_t;

typedef struct {
  uint8_t type;
  const char* label;
  bool* dirty;
  union {
    form_input_t input;
    form_checkbox_t checkbox;
    form_select_t select;
    struct _form_t* form;
  } u;
} form_item_t;

typedef struct _form_t {
  const char* title;
  uint8_t top_row;
  form_item_t* form_items;
  uint8_t num_form_items;
  uint8_t col1;
  uint8_t col2;
  uint8_t col3;
  uint8_t col3_width;
} form_t;

uint8_t form(form_t* form, bool show_from_left);
uint8_t form_inline(window_t* form_wnd, form_t* form,
                    bool show_from_left);

#endif
