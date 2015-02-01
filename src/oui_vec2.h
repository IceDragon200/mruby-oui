#ifndef MRUBY_OUI_VEC2_H
#define MRUBY_OUI_VEC2_H

#include <stdbool.h>

#include <mruby.h>
#include <mruby/class.h>

#include <oui.h>

extern const struct mrb_data_type mrb_oui_vec2_type;
void mrb_oui_vec2_init(mrb_state *mrb, struct RClass *mod);
mrb_value mrb_oui_vec2_value(mrb_state *mrb, UIvec2 vec2);

#endif
