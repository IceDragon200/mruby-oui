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

static mrb_state *contextMRB;
static struct RClass *context_class;

static void
context_free(mrb_state *mrb, void *ptr)
{
  UIcontext *context = ptr;
  if (context) {
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

static inline mrb_value
get_current_mrb_context(mrb_state *mrb)
{
  return mrb_iv_get(mrb, mrb_obj_value(context_class),
                         mrb_intern_lit(mrb, "__current"));
}

static mrb_value
context_s_get_current(mrb_state *mrb, mrb_value klass)
{
  return get_current_mrb_context(mrb);
}

static mrb_value
context_s_clear_current(mrb_state *mrb, mrb_value klass)
{
  mrb_iv_set(mrb, mrb_obj_value(context_class),
                  mrb_intern_lit(mrb, "__current"),
                  mrb_nil_value());
  return klass;
}

static mrb_value
context_initialize(mrb_state *mrb, mrb_value self)
{
  UIcontext *context;
  mrb_int item_capacity;
  mrb_int buffer_capacity;
  mrb_get_args(mrb, "ii", &item_capacity, &buffer_capacity);
  context = uiCreateContext(item_capacity, buffer_capacity);
  DATA_PTR(self) = context;
  DATA_TYPE(self) = &mrb_oui_context_type;
  return self;
}

static mrb_value
context_make_current(mrb_state *mrb, mrb_value self)
{
  UIcontext *context = get_context(mrb, self);
  uiMakeCurrent(context);
  mrb_iv_set(mrb, mrb_obj_value(context_class),
                  mrb_intern_lit(mrb, "__current"),
                  self);
  return self;
}

ng_DEF_FUNC_N2(context_set_cursor, uiSetCursor, "ii", mrb_int, mrb_int);
ng_DEF_FUNC_N0_vec2(context_get_cursor, uiGetCursor);
ng_DEF_FUNC_N0_vec2(context_get_cursor_delta, uiGetCursorDelta);
ng_DEF_FUNC_N0_vec2(context_get_cursor_start, uiGetCursorStart);
ng_DEF_FUNC_N0_vec2(context_get_cursor_start_delta, uiGetCursorStartDelta);
ng_DEF_FUNC_N3(context_set_button, uiSetButton, "iib", mrb_int, mrb_int, mrb_bool);
ng_DEF_FUNC_N1_i(context_get_button, uiGetButton, "i", mrb_int);
ng_DEF_FUNC_N0_i(context_get_clicks, uiGetClicks);
ng_DEF_FUNC_N3(context_set_key, uiSetKey, "iib", mrb_int, mrb_int, mrb_bool);
ng_DEF_FUNC_N1(context_set_char, uiSetChar, "i", mrb_int);
ng_DEF_FUNC_N2(context_set_scroll, uiSetScroll, "ii", mrb_int, mrb_int);
ng_DEF_FUNC_N0_vec2(context_get_scroll, uiGetScroll);
ng_DEF_FUNC_N0(context_begin_layout, uiBeginLayout);
ng_DEF_FUNC_N0(context_end_layout, uiEndLayout);
ng_DEF_FUNC_N0(context_update_hot_item, uiUpdateHotItem);
ng_DEF_FUNC_N1(context_process, uiProcess, "i", mrb_int);
ng_DEF_FUNC_N0(context_clear_state, uiClearState);
ng_DEF_FUNC_N0_i(context_item, uiItem);
ng_DEF_FUNC_N2(context_set_frozen, uiSetFrozen, "ib", mrb_int, mrb_bool);

//static mrb_value
//context_set_handle(mrb_state *mrb, mrb_value self)
//{
//  mrb_int item;
//  mrb_value handle;
//  mrb_get_args(mrb, "io", &item, &handle);
//  uiSetHandle(item, mrb_cptr(handle));
//  return self;
//}

//ng_DEF_FUNC_N2_cptr(context_alloc_handle, uiAllocHandle, "ii", mrb_int, mrb_int);

static void
context_handle_func(int item, UIevent event)
{
  mrb_value vals[3];
  const int ai = mrb_gc_arena_save(contextMRB);
  mrb_value mcontext = get_current_mrb_context(contextMRB);
  mrb_value blk = mrb_iv_get(contextMRB, mcontext, mrb_intern_lit(contextMRB, "__handler"));
  vals[0] = mcontext;
  vals[1] = mrb_fixnum_value(item);
  vals[2] = mrb_oui_event_value(contextMRB, event);
  mrb_yield_argv(contextMRB, blk, 3, vals);
  mrb_gc_arena_restore(contextMRB, ai);
}

static mrb_value
context_set_handler(mrb_state *mrb, mrb_value self)
{
  mrb_value blk;
  mrb_get_args(mrb, "|&", &blk);
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "__handler"), blk);
  if (mrb_nil_p(blk)) {
    uiSetHandler(NULL);
  } else {
    uiSetHandler(context_handle_func);
  }
  return blk;
}

ng_DEF_FUNC_N2(context_set_events, uiSetEvents, "ii", mrb_int, mrb_int);
ng_DEF_FUNC_N2(context_set_flags, uiSetFlags, "ii", mrb_int, mrb_int);
ng_DEF_FUNC_N2_i(context_insert, uiInsert, "ii", mrb_int, mrb_int);
ng_DEF_FUNC_N2_i(context_append, uiAppend, "ii", mrb_int, mrb_int);
ng_DEF_FUNC_N2_i(context_insert_back, uiInsertBack, "ii", mrb_int, mrb_int);
ng_DEF_FUNC_N2_i(context_insert_front, uiInsertFront, "ii", mrb_int, mrb_int);
ng_DEF_FUNC_N3(context_set_size, uiSetSize, "iii", mrb_int, mrb_int, mrb_int);
ng_DEF_FUNC_N2(context_set_layout, uiSetLayout, "ii", mrb_int, mrb_int);
ng_DEF_FUNC_N2(context_set_box, uiSetBox, "ii", mrb_int, mrb_int);
ng_DEF_FUNC_N5(context_set_margins, uiSetMargins, "iiiii", mrb_int, mrb_int, mrb_int, mrb_int, mrb_int);
ng_DEF_FUNC_N1(context_focus, uiFocus, "i", mrb_int);
ng_DEF_FUNC_N1_i(context_first_child, uiFirstChild, "i", mrb_int);
ng_DEF_FUNC_N1_i(context_next_sibling, uiNextSibling, "i", mrb_int);
ng_DEF_FUNC_N0_i(context_get_item_count, uiGetItemCount);
ng_DEF_FUNC_N0_i(context_get_alloc_size, uiGetAllocSize);
ng_DEF_FUNC_N1_i(context_get_state, uiGetState, "i", mrb_int);
//ng_DEF_FUNC_N1_cptr(context_get_handle, uiGetHandle, "i", mrb_int);
ng_DEF_FUNC_N0_i(context_get_hot_item, uiGetHotItem);
ng_DEF_FUNC_N0_i(context_get_focused_item, uiGetFocusedItem);
ng_DEF_FUNC_N5_i(context_find_item, uiFindItem, "iiiii", mrb_int, mrb_int, mrb_int, mrb_int, mrb_int);

static mrb_value
context_get_handler(mrb_state *mrb, mrb_value self)
{
  return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "__handler"));
}

ng_DEF_FUNC_N1_i(context_get_events, uiGetEvents, "i", mrb_int);
ng_DEF_FUNC_N1_i(context_get_flags, uiGetFlags, "i", mrb_int);
ng_DEF_FUNC_N0_i(context_get_key, uiGetKey);
ng_DEF_FUNC_N0_i(context_get_modifier, uiGetModifier);
ng_DEF_FUNC_N1_rect(context_get_rect, uiGetRect, "i", mrb_int);
ng_DEF_FUNC_N3_i(context_contains, uiContains, "iii", mrb_int, mrb_int, mrb_int);
ng_DEF_FUNC_N1_i(context_get_width, uiGetWidth, "i", mrb_int);
ng_DEF_FUNC_N1_i(context_get_height, uiGetHeight, "i", mrb_int);
ng_DEF_FUNC_N1_i(context_get_layout, uiGetLayout, "i", mrb_int);
ng_DEF_FUNC_N1_i(context_get_box, uiGetBox, "i", mrb_int);
ng_DEF_FUNC_N1_i(context_get_margin_left, uiGetMarginLeft, "i", mrb_int);
ng_DEF_FUNC_N1_i(context_get_margin_top, uiGetMarginTop, "i", mrb_int);
ng_DEF_FUNC_N1_i(context_get_margin_right, uiGetMarginRight, "i", mrb_int);
ng_DEF_FUNC_N1_i(context_get_margin_down, uiGetMarginDown, "i", mrb_int);
ng_DEF_FUNC_N1_i(context_recover_item, uiRecoverItem, "i", mrb_int);
ng_DEF_FUNC_N2(context_remap_item, uiRemapItem, "ii", mrb_int, mrb_int);
ng_DEF_FUNC_N0_i(context_get_last_item_count, uiGetLastItemCount);

void
mrb_oui_context_init(mrb_state *mrb, struct RClass *mod)
{
  contextMRB = mrb;
  context_class = mrb_define_class_under(mrb, mod, "Context", mrb->object_class);
  MRB_SET_INSTANCE_TT(context_class, MRB_TT_DATA);

  mrb_define_method(mrb, context_class, "initialize",             context_initialize,             MRB_ARGS_REQ(2));
  mrb_define_method(mrb, context_class, "make_current",           context_make_current,           MRB_ARGS_NONE());
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

  mrb_define_class_method(mrb, context_class, "current", context_s_get_current, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, context_class, "clear_current", context_s_clear_current, MRB_ARGS_NONE());
}

void
mrb_oui_context_final(mrb_state *mrb, struct RClass *mod)
{
  contextMRB = NULL;
}
