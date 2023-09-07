
#include "header.h"
#include "form.h"
#include "trs-io.h"
#include "screen.h"
#include <string.h>
#include <assert.h>


static window_t* wnd;


static const char* left_brace = NULL;
static const char* right_brace = NULL;


static void draw_input_field(form_input_t* inp, bool has_focus) {
  int8_t i;
  uint8_t idx;
  uint8_t len;
  uint8_t width;

  len = strlen(inp->buf);
  width = inp->width;
  width = (width == 0) ? inp->len : width;

  if (!has_focus) {
    wnd_print(wnd, " ");
    if (len == 0) {
      wnd_print(wnd, "-");
    } else if (len <= width) {
      wnd_print(wnd, inp->buf);
    } else {
      for (i = 0; i < width - 3; i++) {
        wnd_print(wnd, "%c", inp->buf[i]);
      }
      wnd_print(wnd, "...");
    }
    wnd_clear_eol(wnd);
    return;
  }
  
  wnd_print(wnd, left_brace);
  idx = (len > width) ? len - width : 0;
  wnd_print(wnd, inp->buf + idx);
  wnd_print(wnd, FORM_CURSOR);
  for (i = len; i < width - 1; i++) {
    wnd_print(wnd, ".");
  }
  if (len < width) {
    wnd_print(wnd, right_brace);
  }
}

static void draw_checkbox_field(form_checkbox_t* cb, bool has_focus)
{
  wnd_print(wnd, has_focus ? left_brace : " ");
  wnd_print(wnd, *cb->checked ? "Yes" : "No");
  wnd_print(wnd, has_focus ? right_brace : " ");
  wnd_print(wnd, " ");
}

static void draw_select_field_items(form_select_t* sel, uint8_t width)
{
  const uint8_t selected = sel->current;
  uint8_t left = sel->items_str_left;
  const uint8_t item_left = sel->items_start_idx[selected];
  const uint8_t item_right = sel->items_end_idx[selected];

  if (item_left >= left && item_right < left + width) {
    // Selection is within the visible area. No need for scrolling
    // Do nothing
  } else if (item_right >= left + width) {
    // Current selection is to the right of the visible area. Need to scroll visible area to the left
    left += item_right - (left + width - 1);
    sel->items_str_left = left;
  } else {
    // Need to scroll visible area to the right
    left -= (left - sel->items_start_idx[selected]);
    sel->items_str_left = left;
  }

  char ch = sel->items_str[left];
  if (ch != *left_brace) {
    wnd_print(wnd, left == 0 ? " " : "-");
    wnd_printn(wnd, sel->items_str + left + 1, width - 1);
  } else {
    wnd_printn(wnd, sel->items_str + left, width);
  }
}

static void draw_select_field(form_select_t* sel, uint8_t width, bool has_focus)
{
  if (!has_focus) {
    wnd_print(wnd, " ");
    wnd_print(wnd, sel->items[*sel->selected]);
    wnd_clear_eol(wnd);
    return;
  }
  draw_select_field_items(sel, width);
}

static void redraw_form_item(form_t* form, uint8_t n, form_item_t* item, bool has_focus)
{
  wnd_goto(wnd, form->col3, n);
  switch (item->type) {
  case FORM_TYPE_INPUT:
    draw_input_field(&item->u.input, has_focus);
    break;
  case FORM_TYPE_CHECKBOX:
    draw_checkbox_field(&item->u.checkbox, has_focus);
    break;
  case FORM_TYPE_SELECT:
    draw_select_field(&item->u.select, form->col3_width, has_focus);
    break;
  default:
    // Skip
    break;
  }
}

static void redraw_form(form_t* form) {
  int n = form->top_row;
  form_item_t* items = form->form_items;

  while (items->type != FORM_TYPE_END) {
    redraw_form_item(form, n, items, false);
    items++;
    n++;
  }
}

static bool handle_key_for_input(form_t* form, uint8_t n, char key, form_input_t* inp)
{
  uint8_t len = strlen(inp->buf);

  if (key == KEY_LEFT) {
    if (len == 0) {
      return false;
    }
    len--;
    inp->buf[len] = '\0';
  } else {
    if (len == inp->len) {
      return false;
    }
    inp->buf[len++] = key;
    inp->buf[len] = '\0';
  }
  wnd_goto(wnd, form->col3, n);
  draw_input_field(inp, true);
  return true;
}

static bool handle_key_for_checkbox(form_t* form, uint8_t n, char key, form_checkbox_t* cb)
{
  bool dirty;
  bool checked = *cb->checked;
  
  if (key == ' ') {
    checked = !checked;
  }
  if (key == 'N' || key == 'n') {
    checked = false;
  }
  if (key == 'Y' || key == 'y') {
    checked = true;
  }
  dirty = *cb->checked != checked;
  *cb->checked = checked;
  wnd_goto(wnd, form->col3, n);
  draw_checkbox_field(cb, true);
  return dirty;
}

static void select_set_focus(form_select_t* sel, uint8_t n)
{
  uint8_t from = sel->current;
  uint8_t to = n;

  sel->items_str[sel->items_start_idx[from]] = ' ';
  sel->items_str[sel->items_end_idx[from]] = ' ';
  sel->items_str[sel->items_start_idx[to]] = *left_brace;
  sel->items_str[sel->items_end_idx[to]] = *right_brace;

  *sel->selected = (sel->first + to) % sel->num_items;
}

static bool handle_key_for_select(form_t* form, uint8_t n, char key, form_select_t* sel)
{
  bool dirty;
  uint8_t selected = sel->current;
  
  if (key == ' ' || key == KEY_RIGHT) {
    if ((selected + 1) != sel->num_items) {
      selected++;
    }
  }
  if (key == KEY_LEFT) {
    if (selected != 0) {
      selected--;
    }
  }
  dirty = sel->current != selected;
  if (dirty) {
    select_set_focus(sel, selected);
    sel->current = selected;
    wnd_goto(wnd, form->col3, n);
    draw_select_field(sel, form->col3_width, true);
  }
  return dirty;
}

static void handle_key(form_t* form, uint8_t n, char key, form_item_t* item)
{
  bool dirty = false;

  n += form->top_row;

  switch(item->type) {
  case FORM_TYPE_INPUT:
    dirty = handle_key_for_input(form, n, key, &item->u.input);
    break;
  case FORM_TYPE_CHECKBOX:
    dirty = handle_key_for_checkbox(form, n, key, &item->u.checkbox);
    break;
  case FORM_TYPE_SELECT:
    dirty = handle_key_for_select(form, n, key, &item->u.select);
    break;
  }
  if (item->dirty != NULL) {
    *(item->dirty) |= dirty;
  }
}

static void init_select(form_select_t* sel)
{
  uint8_t n, count;
  uint8_t i = 0;

  sel->first = *sel->selected;
  sel->current = 0;
  sel->items_str_left = 0;

  n = sel->first;
  count = 0;

  do {
    if (count == SELECTION_MAX_ITEMS ||
        sel->items[n] == NULL ||
        strlen(sel->items[n]) + i + 4 >= SELECTION_MAX_LEN) {
      break;
    }
    if (i != 0) {
      sel->items_str[i++] = ' ';
    }
    sel->items_start_idx[count] = i;
    sel->items_str[i++] = ' ';
    uint8_t k = 0;
    while (sel->items[n][k] != '\0') {
      sel->items_str[i++] = sel->items[n][k++];
    }
    sel->items_end_idx[count] = i;
    sel->items_str[i++] = ' ';
    n++;
    count++;
    if (sel->items[n] == NULL) {
      n = 0;
    }
  } while(n != sel->first);

  sel->items_str[i] = '\0';
  sel->num_items = count;

  select_set_focus(sel, 0);
}

static char do_input(form_t* form, bool show_from_left)
{
  uint8_t i = 0;
  uint8_t last_i = 99;
  uint8_t n = form->top_row;
  uint8_t num_form_items = form->num_form_items;
  form_item_t* items;
  form_item_t* field;
  bool first_time = true;
  char ch;

  items = form->form_items;

  while (items[i].type == FORM_TYPE_HEADER) i++;
  
  while(true) {
    redraw_form(form);
    field = &items[i];
    if (field->type == FORM_TYPE_SELECT) {
      init_select(&field->u.select);
    }
    while(true) {
      bool redraw = false;
      if (i != last_i) {
        redraw_form_item(form, n + i, field, true);
        last_i = i;
      }
      if (first_time) {
        screen_show(show_from_left);
        set_screen_to_foreground();
        first_time = false;
      }
      ch = get_key();
      switch (ch) {
      case KEY_BREAK:
        redraw_form_item(form, n + i, field, false);
        return ch;
      case KEY_UP:
        do {
          i = (i - 1 + num_form_items) % num_form_items;
        } while (items[i].type == FORM_TYPE_HEADER);
        redraw = true;
        break;
      case KEY_ENTER:
        if (i + 1 == num_form_items) {
          redraw_form_item(form, n + i, field, false);
          return ch;
        }
        // Fallthrough
      case KEY_DOWN:
        do {
          i = (i + 1) % num_form_items;
        } while (items[i].type == FORM_TYPE_HEADER);
        redraw = true;
        break;
      default:
        handle_key(form, i, ch, field);
        break;
      }
      if (redraw) {
        break;
      }
    }
  }
  // Never reached
  return 0;
}

static void compute_form_layout(form_t* form)
{
  bool has_header = false;
  int max = 0;
  form_item_t* items;
  uint8_t num_form_items;

  if (left_brace == NULL) {
    left_brace = is_m3() ? "[" : "<";
  }

  if (right_brace == NULL) {
    right_brace = is_m3() ? "]" : ">";
  }

  items = form->form_items;
  form->top_row = wnd_get_y(wnd);

  num_form_items = 0;
  
  while (items->type != FORM_TYPE_END) {
    if (items->type == FORM_TYPE_SELECT) {
      form_select_t* sel = &items->u.select;
      if (sel->items_ptr != NULL) {
        sel->items = *sel->items_ptr;
      }
    }

    if (items->dirty != NULL) {
      *items->dirty = false;
    }
    num_form_items++;
    if (items->type == FORM_TYPE_HEADER) {
      has_header = true;
    } else {
      int len = strlen(items->label);
      max = (max < len) ? len : max;
    }
    items++;
  }
  form->num_form_items = num_form_items;
  form->col1 = has_header ? 2 : 0;
  form->col3 = has_header ? max + 4 : max + 2;
  form->col2 = form->col3 - 2;
  form->col3_width = wnd->w - form->col3;
}

static void draw_form(form_t* form)
{
  uint8_t n = form->top_row;
  form_item_t* items = form->form_items;
  bool is_header;
  
  while (items->type != FORM_TYPE_END) {
    if (n == wnd->h) {
      panic(0); // XXX
    }
    is_header = items->type == FORM_TYPE_HEADER;
    wnd_goto(wnd, is_header ? 0 : form->col1, n);
    wnd_print(wnd, items->label);
    if (!is_header) {
      wnd_goto(wnd, form->col2, n);
      wnd_print(wnd, ":");
    }
    items++;
    n++;
  }
  redraw_form(form);
} 

uint8_t form(form_t* form, bool show_from_left)
{
  static window_t form_wnd;

  set_screen_to_background();
  header(form->title);
  wnd = &form_wnd;
  init_window(wnd, 0, 3, 0, 0);
  compute_form_layout(form);
  draw_form(form);
  return do_input(form, show_from_left);
}

uint8_t form_inline(window_t* form_wnd, form_t* form,
                    bool show_from_left)
{
  set_screen_to_background();
  wnd = form_wnd;
  compute_form_layout(form);
  draw_form(form);
  return do_input(form, show_from_left);
}

