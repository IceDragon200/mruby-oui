#ifndef MRUBY_OUI_RECT_H
#define MRUBY_OUI_RECT_H

#include <stdbool.h>

#include <mruby.h>
#include <mruby/class.h>

#include <oui.h>

extern const struct mrb_data_type mrb_oui_rect_type;
void mrb_oui_rect_init(mrb_state *mrb, struct RClass *mod);
mrb_value mrb_oui_rect_value(mrb_state *mrb, UIrect rect);

#endif
