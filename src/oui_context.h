#ifndef MRUBY_OUI_CONTEXT_H
#define MRUBY_OUI_CONTEXT_H

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>

extern const struct mrb_data_type mrb_oui_context_type;
void mrb_oui_context_init(mrb_state *mrb, struct RClass *mod);
void mrb_oui_context_final(mrb_state *mrb, struct RClass *mod);

#endif
