#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include "oui_vec2.h"
#include "oui_helper_macros.h"

static struct RClass *vec2_class;

static void
vec2_free(mrb_state *mrb, void *ptr)
{
  if (ptr) {
    mrb_free(mrb, ptr);
  }
}

const struct mrb_data_type mrb_oui_vec2_type = { "UIvec2", vec2_free };

mrb_value
mrb_oui_vec2_value(mrb_state *mrb, UIvec2 vec2)
{
  UIvec2 *target;
  mrb_value result = mrb_obj_new(mrb, vec2_class, 0, NULL);
  target = (UIvec2*)DATA_PTR(result);
  *target = vec2;
  return result;
}

static mrb_value
vec2_initialize(mrb_state *mrb, mrb_value self)
{
  UIvec2 *vec2;
  mrb_int x = 0;
  mrb_int y = 0;
  mrb_get_args(mrb, "|ii", &x, &y);
  vec2 = mrb_malloc(mrb, sizeof(UIvec2));
  vec2->x = x;
  vec2->y = y;
  DATA_PTR(self) = vec2;
  DATA_TYPE(self) = &mrb_oui_vec2_type;
  return self;
}

base_DEF_ATTR_GET_i(vec2_get_x, &mrb_oui_vec2_type, UIvec2, x);
base_DEF_ATTR_GET_i(vec2_get_y, &mrb_oui_vec2_type, UIvec2, y);
base_DEF_ATTR_SET_i(vec2_set_x, &mrb_oui_vec2_type, UIvec2, x);
base_DEF_ATTR_SET_i(vec2_set_y, &mrb_oui_vec2_type, UIvec2, y);

void
mrb_oui_vec2_init(mrb_state *mrb, struct RClass *mod)
{
  vec2_class = mrb_define_class_under(mrb, mod, "Vec2", mrb->object_class);
  MRB_SET_INSTANCE_TT(vec2_class, MRB_TT_DATA);

  mrb_define_method(mrb, vec2_class, "initialize", vec2_initialize, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, vec2_class, "x", vec2_get_x, MRB_ARGS_NONE());
  mrb_define_method(mrb, vec2_class, "y", vec2_get_y, MRB_ARGS_NONE());
  mrb_define_method(mrb, vec2_class, "x=", vec2_set_x, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, vec2_class, "y=", vec2_set_y, MRB_ARGS_REQ(1));
}
