
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

static void draw_select_field_items(form_select_t* sel)
{
  uint8_t current = sel->first;

  do {
    const char* item;
    bool frame;
    
    item = sel->items[current];
    if (item == NULL) {
      current = 0;
      continue;
    }
    frame = current == *sel->selected;
    wnd_print(wnd, frame ? left_brace : " ");
    wnd_print(wnd, item);
    wnd_print(wnd, frame ? right_brace : " ");
    wnd_print(wnd, " ");
    current++;
  } while (current != sel->first);
}

static void draw_select_field(form_select_t* sel, bool has_focus)
{
  if (!has_focus) {
    sel->first = *sel->selected;
    wnd_print(wnd, " ");
    wnd_print(wnd, sel->items[*sel->selected]);
    wnd_clear_eol(wnd);
    return;
  }
  draw_select_field_items(sel);
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
    draw_select_field(&item->u.select, has_focus);
    break;
  default:
    // Skip
    break;
  }
}

static void redraw_form(form_t* form) {
  int n = form->top_row;
  form_item_t* items = form->form_items;

  if (left_brace == NULL) {
    left_brace = is_m3() ? "[" : "<";
  }

  if (right_brace == NULL) {
    right_brace = is_m3() ? "]" : ">";
  }

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

static bool handle_key_for_select(form_t* form, uint8_t n, char key, form_select_t* sel)
{
  bool dirty;
  uint8_t selected = *sel->selected;
  
  if (key == ' ' || key == KEY_RIGHT) {
    selected++;
    if (sel->items[selected] == NULL) {
      selected = 0;
    }
  }
  if (key == KEY_LEFT) {
    if (selected == 0) {
      while (sel->items[++selected] != NULL) ;
    }
    selected--;
  }
  dirty = *sel->selected != selected;
  *sel->selected = selected;
  wnd_goto(wnd, form->col3, n);
  draw_select_field(sel, true);
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

  items = form->form_items;
  form->top_row = wnd_get_y(wnd);

  num_form_items = 0;
  
  while (items->type != FORM_TYPE_END) {
    items->dirty = false;
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

