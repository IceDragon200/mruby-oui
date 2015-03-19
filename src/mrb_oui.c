#include <stdbool.h>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/variable.h>

#include <oui.h>

#include "oui_vec2.h"
#include "oui_rect.h"
#include "oui_context.h"

static struct RClass *oui_module;

void
mrb_mruby_oui_gem_init(mrb_state* mrb)
{
  oui_module = mrb_define_module(mrb, "OUI");

  mrb_oui_vec2_init(mrb, oui_module);
  mrb_oui_rect_init(mrb, oui_module);
  mrb_oui_context_init(mrb, oui_module);

  mrb_define_const(mrb, oui_module, "MAX_DATASIZE",     mrb_fixnum_value(UI_MAX_DATASIZE));
  mrb_define_const(mrb, oui_module, "MAX_DEPTH",        mrb_fixnum_value(UI_MAX_DEPTH));
  mrb_define_const(mrb, oui_module, "MAX_INPUT_EVENTS", mrb_fixnum_value(UI_MAX_INPUT_EVENTS));
  mrb_define_const(mrb, oui_module, "CLICK_THRESHOLD",  mrb_fixnum_value(UI_CLICK_THRESHOLD));
  mrb_define_const(mrb, oui_module, "COLD",             mrb_fixnum_value(UI_COLD));
  mrb_define_const(mrb, oui_module, "HOT",              mrb_fixnum_value(UI_HOT));
  mrb_define_const(mrb, oui_module, "ACTIVE",           mrb_fixnum_value(UI_ACTIVE));
  mrb_define_const(mrb, oui_module, "FROZEN",           mrb_fixnum_value(UI_FROZEN));
  mrb_define_const(mrb, oui_module, "ROW",              mrb_fixnum_value(UI_ROW));
  mrb_define_const(mrb, oui_module, "COLUMN",           mrb_fixnum_value(UI_COLUMN));
  mrb_define_const(mrb, oui_module, "LAYOUT",           mrb_fixnum_value(UI_LAYOUT));
  mrb_define_const(mrb, oui_module, "FLEX",             mrb_fixnum_value(UI_FLEX));
  mrb_define_const(mrb, oui_module, "NOWRAP",           mrb_fixnum_value(UI_NOWRAP));
  mrb_define_const(mrb, oui_module, "WRAP",             mrb_fixnum_value(UI_WRAP));
  mrb_define_const(mrb, oui_module, "START",            mrb_fixnum_value(UI_START));
  mrb_define_const(mrb, oui_module, "MIDDLE",           mrb_fixnum_value(UI_MIDDLE));
  mrb_define_const(mrb, oui_module, "END",              mrb_fixnum_value(UI_END));
  mrb_define_const(mrb, oui_module, "JUSTIFY",          mrb_fixnum_value(UI_JUSTIFY));
  mrb_define_const(mrb, oui_module, "LEFT",             mrb_fixnum_value(UI_LEFT));
  mrb_define_const(mrb, oui_module, "TOP",              mrb_fixnum_value(UI_TOP));
  mrb_define_const(mrb, oui_module, "RIGHT",            mrb_fixnum_value(UI_RIGHT));
  mrb_define_const(mrb, oui_module, "DOWN",             mrb_fixnum_value(UI_DOWN));
  mrb_define_const(mrb, oui_module, "HFILL",            mrb_fixnum_value(UI_HFILL));
  mrb_define_const(mrb, oui_module, "VFILL",            mrb_fixnum_value(UI_VFILL));
  mrb_define_const(mrb, oui_module, "HCENTER",          mrb_fixnum_value(UI_HCENTER));
  mrb_define_const(mrb, oui_module, "VCENTER",          mrb_fixnum_value(UI_VCENTER));
  mrb_define_const(mrb, oui_module, "CENTER",           mrb_fixnum_value(UI_CENTER));
  mrb_define_const(mrb, oui_module, "FILL",             mrb_fixnum_value(UI_FILL));
  mrb_define_const(mrb, oui_module, "BREAK",            mrb_fixnum_value(UI_BREAK));
  mrb_define_const(mrb, oui_module, "BUTTON0_DOWN",     mrb_fixnum_value(UI_BUTTON0_DOWN));
  mrb_define_const(mrb, oui_module, "BUTTON0_UP",       mrb_fixnum_value(UI_BUTTON0_UP));
  mrb_define_const(mrb, oui_module, "BUTTON0_HOT_UP",   mrb_fixnum_value(UI_BUTTON0_HOT_UP));
  mrb_define_const(mrb, oui_module, "BUTTON0_CAPTURE",  mrb_fixnum_value(UI_BUTTON0_CAPTURE));
  mrb_define_const(mrb, oui_module, "BUTTON2_DOWN",     mrb_fixnum_value(UI_BUTTON2_DOWN));
  mrb_define_const(mrb, oui_module, "SCROLL",           mrb_fixnum_value(UI_SCROLL));
  mrb_define_const(mrb, oui_module, "KEY_DOWN",         mrb_fixnum_value(UI_KEY_DOWN));
  mrb_define_const(mrb, oui_module, "KEY_UP",           mrb_fixnum_value(UI_KEY_UP));
  mrb_define_const(mrb, oui_module, "CHAR",             mrb_fixnum_value(UI_CHAR));
  mrb_define_const(mrb, oui_module, "USERMASK",         mrb_fixnum_value(UI_USERMASK));
  mrb_define_const(mrb, oui_module, "ANY",              mrb_fixnum_value(UI_ANY));
}

void
mrb_mruby_oui_gem_final(mrb_state* mrb)
{
  mrb_oui_context_final(mrb, oui_module);
}
