#include <memory.h>
#include <stdbool.h>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/variable.h>

#include <oui.h>
#include "oui_config.h"
#include "oui_context.h"
#include "oui_rect.h"
#include "oui_vec2.h"

#include "oui_helper_macros.h"

typedef struct UIcontextData {
  mrb_state *mrb;
  mrb_value self;
} UIcontextData;

static struct RClass *context_class;

static void
context_free(mrb_state *mrb, void *ptr)
{
  UIcontextData *data;
  UIcontext *context = ptr;
  if (context) {
    data = (UIcontextData*)uiGetContextHandle(context);
    if (data) {
      mrb_free(mrb, data);
    }
    uiDestroyContext(context);
  }
}

const struct mrb_data_type mrb_oui_context_type = { "UIcontext", context_free };

static mrb_value
mrb_oui_event_value(mrb_state *mrb, UIevent event)
{
  return mrb_fixnum_value(event);
}

static inline UIcontext*
get_context(mrb_state *mrb, mrb_value self)
{
  return (UIcontext*)mrb_data_get_ptr(mrb, self, &mrb_oui_context_type);
}

static mrb_value
context_initialize(mrb_state *mrb, mrb_value self)
{
  UIcontext *context;
  UIcontextData *data;
  mrb_int item_capacity;
  mrb_int buffer_capacity;
  mrb_get_args(mrb, "ii", &item_capacity, &buffer_capacity);
  context = uiCreateContext(item_capacity, buffer_capacity);
  data = mrb_malloc(mrb, sizeof(UIcontextData));
  memset(data, 0, sizeof(UIcontextData));
  data->mrb = mrb;
  data->self = self;
  uiSetContextHandle(context, (void*)data);
  DATA_PTR(self) = context;
  DATA_TYPE(self) = &mrb_oui_context_type;
  return self;
}

d_DEF_FUNC_N2(UIcontext*, get_context, context_set_cursor, uiSetCursor, "ii", mrb_int, mrb_int);
d_DEF_FUNC_N0_vec2(UIcontext*, get_context, context_get_cursor, uiGetCursor);
d_DEF_FUNC_N0_vec2(UIcontext*, get_context, context_get_cursor_delta, uiGetCursorDelta);
d_DEF_FUNC_N0_vec2(UIcontext*, get_context, context_get_cursor_start, uiGetCursorStart);
d_DEF_FUNC_N0_vec2(UIcontext*, get_context, context_get_cursor_start_delta, uiGetCursorStartDelta);
d_DEF_FUNC_N3(UIcontext*, get_context, context_set_button, uiSetButton, "iib", mrb_int, mrb_int, mrb_bool);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_get_button, uiGetButton, "i", mrb_int);
d_DEF_FUNC_N0_i(UIcontext*, get_context, context_get_clicks, uiGetClicks);
d_DEF_FUNC_N3(UIcontext*, get_context, context_set_key, uiSetKey, "iib", mrb_int, mrb_int, mrb_bool);
d_DEF_FUNC_N1(UIcontext*, get_context, context_set_char, uiSetChar, "i", mrb_int);
d_DEF_FUNC_N2(UIcontext*, get_context, context_set_scroll, uiSetScroll, "ii", mrb_int, mrb_int);
d_DEF_FUNC_N0_vec2(UIcontext*, get_context, context_get_scroll, uiGetScroll);
d_DEF_FUNC_N0(UIcontext*, get_context, context_begin_layout, uiBeginLayout);
d_DEF_FUNC_N0(UIcontext*, get_context, context_end_layout, uiEndLayout);
d_DEF_FUNC_N0(UIcontext*, get_context, context_update_hot_item, uiUpdateHotItem);
d_DEF_FUNC_N1(UIcontext*, get_context, context_process, uiProcess, "i", mrb_int);
d_DEF_FUNC_N0(UIcontext*, get_context, context_clear_state, uiClearState);
d_DEF_FUNC_N0_i(UIcontext*, get_context, context_item, uiItem);
d_DEF_FUNC_N2(UIcontext*, get_context, context_set_frozen, uiSetFrozen, "ib", mrb_int, mrb_bool);

//static mrb_value
//context_set_handle(mrb_state *mrb, mrb_value self)
//{
//  UIcontext *ctx;
//  mrb_int item;
//  mrb_value handle;
//  mrb_get_args(mrb, "io", &item, &handle);
//  ctx = get_context(mrb, self);
//  uiSetHandle(ctx, item, mrb_cptr(handle));
//  return self;
//}

//DEF_FUNC_N2_cptr(context_alloc_handle, uiAllocHandle, "ii", mrb_int, mrb_int);

static void
context_handle_func(UIcontext *ctx, int item, UIevent event)
{
  UIcontextData *data;
  mrb_state *mrb;
  mrb_value self;
  mrb_value vals[3];
  data = (UIcontextData*)uiGetContextHandle(ctx);
  mrb = data->mrb;
  self = data->self;
  const int ai = mrb_gc_arena_save(mrb);
  mrb_value blk = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "__handler"));
  vals[0] = self;
  vals[1] = mrb_fixnum_value(item);
  vals[2] = mrb_oui_event_value(mrb, event);
  mrb_yield_argv(mrb, blk, 3, vals);
  mrb_gc_arena_restore(mrb, ai);
}

static mrb_value
context_set_handler(mrb_state *mrb, mrb_value self)
{
  UIcontext *ctx;
  mrb_value blk;
  mrb_get_args(mrb, "|&", &blk);
  ctx = get_context(mrb, self);
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "__handler"), blk);
  if (mrb_nil_p(blk)) {
    uiSetHandler(ctx, NULL);
  } else {
    uiSetHandler(ctx, context_handle_func);
  }
  return blk;
}

d_DEF_FUNC_N2(UIcontext*, get_context, context_set_events, uiSetEvents, "ii", mrb_int, mrb_int);
d_DEF_FUNC_N2(UIcontext*, get_context, context_set_flags, uiSetFlags, "ii", mrb_int, mrb_int);
d_DEF_FUNC_N2_i(UIcontext*, get_context, context_insert, uiInsert, "ii", mrb_int, mrb_int);
d_DEF_FUNC_N2_i(UIcontext*, get_context, context_append, uiAppend, "ii", mrb_int, mrb_int);
d_DEF_FUNC_N2_i(UIcontext*, get_context, context_insert_back, uiInsertBack, "ii", mrb_int, mrb_int);
d_DEF_FUNC_N2_i(UIcontext*, get_context, context_insert_front, uiInsertFront, "ii", mrb_int, mrb_int);
d_DEF_FUNC_N3(UIcontext*, get_context, context_set_size, uiSetSize, "iii", mrb_int, mrb_int, mrb_int);
d_DEF_FUNC_N2(UIcontext*, get_context, context_set_layout, uiSetLayout, "ii", mrb_int, mrb_int);
d_DEF_FUNC_N2(UIcontext*, get_context, context_set_box, uiSetBox, "ii", mrb_int, mrb_int);
d_DEF_FUNC_N5(UIcontext*, get_context, context_set_margins, uiSetMargins, "iiiii", mrb_int, mrb_int, mrb_int, mrb_int, mrb_int);
d_DEF_FUNC_N1(UIcontext*, get_context, context_focus, uiFocus, "i", mrb_int);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_first_child, uiFirstChild, "i", mrb_int);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_next_sibling, uiNextSibling, "i", mrb_int);
d_DEF_FUNC_N0_i(UIcontext*, get_context, context_get_item_count, uiGetItemCount);
d_DEF_FUNC_N0_i(UIcontext*, get_context, context_get_alloc_size, uiGetAllocSize);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_get_state, uiGetState, "i", mrb_int);
//d_DEF_FUNC_N1_cptr(UIcontext*, get_context, context_get_handle, uiGetHandle, "i", mrb_int);
d_DEF_FUNC_N0_i(UIcontext*, get_context, context_get_hot_item, uiGetHotItem);
d_DEF_FUNC_N0_i(UIcontext*, get_context, context_get_focused_item, uiGetFocusedItem);
d_DEF_FUNC_N5_i(UIcontext*, get_context, context_find_item, uiFindItem, "iiiii", mrb_int, mrb_int, mrb_int, mrb_int, mrb_int);

static mrb_value
context_get_handler(mrb_state *mrb, mrb_value self)
{
  return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "__handler"));
}

d_DEF_FUNC_N1_i(UIcontext*, get_context, context_get_events, uiGetEvents, "i", mrb_int);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_get_flags, uiGetFlags, "i", mrb_int);
d_DEF_FUNC_N0_i(UIcontext*, get_context, context_get_key, uiGetKey);
d_DEF_FUNC_N0_i(UIcontext*, get_context, context_get_modifier, uiGetModifier);
d_DEF_FUNC_N1_rect(UIcontext*, get_context, context_get_rect, uiGetRect, "i", mrb_int);
d_DEF_FUNC_N3_i(UIcontext*, get_context, context_contains, uiContains, "iii", mrb_int, mrb_int, mrb_int);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_get_width, uiGetWidth, "i", mrb_int);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_get_height, uiGetHeight, "i", mrb_int);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_get_layout, uiGetLayout, "i", mrb_int);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_get_box, uiGetBox, "i", mrb_int);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_get_margin_left, uiGetMarginLeft, "i", mrb_int);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_get_margin_top, uiGetMarginTop, "i", mrb_int);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_get_margin_right, uiGetMarginRight, "i", mrb_int);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_get_margin_down, uiGetMarginDown, "i", mrb_int);
d_DEF_FUNC_N1_i(UIcontext*, get_context, context_recover_item, uiRecoverItem, "i", mrb_int);
d_DEF_FUNC_N2(UIcontext*, get_context, context_remap_item, uiRemapItem, "ii", mrb_int, mrb_int);
d_DEF_FUNC_N0_i(UIcontext*, get_context, context_get_last_item_count, uiGetLastItemCount);

void
mrb_oui_context_init(mrb_state *mrb, struct RClass *mod)
{
  context_class = mrb_define_class_under(mrb, mod, "Context", mrb->object_class);
  MRB_SET_INSTANCE_TT(context_class, MRB_TT_DATA);

  mrb_define_method(mrb, context_class, "initialize",             context_initialize,             MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "set_cursor",             context_set_cursor,             MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "cursor",                 context_get_cursor,             MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "cursor_delta",           context_get_cursor_delta,       MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "cursor_start",           context_get_cursor_start,       MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "cursor_start_delta",     context_get_cursor_start_delta, MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "set_button",             context_set_button,             MRB_ARGS_REQ(3));
  mrb_define_method(mrb, context_class, "button",                 context_get_button,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "clicks",                 context_get_clicks,             MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "set_key",                context_set_key,                MRB_ARGS_REQ(3));
  mrb_define_method(mrb, context_class, "set_char",               context_set_char,               MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "set_scroll",             context_set_scroll,             MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "scroll",                 context_get_scroll,             MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "begin_layout",           context_begin_layout,           MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "end_layout",             context_end_layout,             MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "update_hot_item",        context_update_hot_item,        MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "process",                context_process,                MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "clear_state",            context_clear_state,            MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "item",                   context_item,                   MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "set_frozen",             context_set_frozen,             MRB_ARGS_REQ(2));
  //mrb_define_method(mrb, context_class, "set_handle",             context_set_handle,             MRB_ARGS_REQ(2));
  //mrb_define_method(mrb, context_class, "alloc_handle",           context_alloc_handle,           MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "set_handler",            context_set_handler,            MRB_ARGS_BLOCK());
  mrb_define_method(mrb, context_class, "set_events",             context_set_events,             MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "set_flags",              context_set_flags,              MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "insert",                 context_insert,                 MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "append",                 context_append,                 MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "insert_back",            context_insert_back,            MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "insert_front",           context_insert_front,           MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "set_size",               context_set_size,               MRB_ARGS_REQ(3));
  mrb_define_method(mrb, context_class, "set_layout",             context_set_layout,             MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "set_box",                context_set_box,                MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "set_margins",            context_set_margins,            MRB_ARGS_REQ(5));
  mrb_define_method(mrb, context_class, "focus",                  context_focus,                  MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "first_child",            context_first_child,            MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "next_sibling",           context_next_sibling,           MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "item_count",             context_get_item_count,         MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "alloc_size",             context_get_alloc_size,         MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "state",                  context_get_state,              MRB_ARGS_REQ(1));
  //mrb_define_method(mrb, context_class, "handle",                 context_get_handle,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "hot_item",               context_get_hot_item,           MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "focused_item",           context_get_focused_item,       MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "find_item",              context_find_item,              MRB_ARGS_REQ(5));
  mrb_define_method(mrb, context_class, "handler",                context_get_handler,            MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "events",                 context_get_events,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "flags",                  context_get_flags,              MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "key",                    context_get_key,                MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "modifier",               context_get_modifier,           MRB_ARGS_NONE());
  mrb_define_method(mrb, context_class, "rect",                   context_get_rect,               MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "contains?",              context_contains,               MRB_ARGS_REQ(3));
  mrb_define_method(mrb, context_class, "width",                  context_get_width,              MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "height",                 context_get_height,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "layout",                 context_get_layout,             MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "box",                    context_get_box,                MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "margin_left",            context_get_margin_left,        MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "margin_top",             context_get_margin_top,         MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "margin_right",           context_get_margin_right,       MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "margin_down",            context_get_margin_down,        MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "recover_item",           context_recover_item,           MRB_ARGS_REQ(1));
  mrb_define_method(mrb, context_class, "remap_item",             context_remap_item,             MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "get_last_item_count",    context_get_last_item_count,    MRB_ARGS_NONE());
}

void
mrb_oui_context_final(mrb_state *mrb, struct RClass *mod)
{
  //
}
