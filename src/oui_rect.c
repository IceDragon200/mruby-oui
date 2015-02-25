#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include "oui_rect.h"
#include "mrb_helper_macros.h"

static struct RClass *rect_class;

static void
rect_free(mrb_state *mrb, void *ptr)
{
  if (ptr) {
    mrb_free(mrb, ptr);
  }
}

const struct mrb_data_type mrb_oui_rect_type = { "UIrect", rect_free };

mrb_value
mrb_oui_rect_value(mrb_state *mrb, UIrect rect)
{
  UIrect *target;
  mrb_value result = mrb_obj_new(mrb, rect_class, 0, NULL);
  target = (UIrect*)DATA_PTR(result);
  *target = rect;
  return result;
}

static mrb_value
rect_initialize(mrb_state *mrb, mrb_value self)
{
  UIrect *rect;
  mrb_int x = 0;
  mrb_int y = 0;
  mrb_int w = 0;
  mrb_int h = 0;
  mrb_get_args(mrb, "|iiii", &x, &y, &w, &h);
  rect = mrb_malloc(mrb, sizeof(UIrect));
  rect->x = x;
  rect->y = y;
  rect->w = w;
  rect->h = h;
  DATA_PTR(self) = rect;
  DATA_TYPE(self) = &mrb_oui_rect_type;
  return self;
}

base_DEF_ATTR_GET_i(rect_get_x, &mrb_oui_rect_type, UIrect, x);
base_DEF_ATTR_GET_i(rect_get_y, &mrb_oui_rect_type, UIrect, y);
base_DEF_ATTR_GET_i(rect_get_w, &mrb_oui_rect_type, UIrect, w);
base_DEF_ATTR_GET_i(rect_get_h, &mrb_oui_rect_type, UIrect, h);

base_DEF_ATTR_SET_i(rect_set_x, &mrb_oui_rect_type, UIrect, x);
base_DEF_ATTR_SET_i(rect_set_y, &mrb_oui_rect_type, UIrect, y);
base_DEF_ATTR_SET_i(rect_set_w, &mrb_oui_rect_type, UIrect, w);
base_DEF_ATTR_SET_i(rect_set_h, &mrb_oui_rect_type, UIrect, h);

void
mrb_oui_rect_init(mrb_state *mrb, struct RClass *mod)
{
  rect_class = mrb_define_class_under(mrb, mod, "Rect", mrb->object_class);
  MRB_SET_INSTANCE_TT(rect_class, MRB_TT_DATA);

  mrb_define_method(mrb, rect_class, "initialize", rect_initialize, MRB_ARGS_OPT(4));
  mrb_define_method(mrb, rect_class, "x", rect_get_x, MRB_ARGS_NONE());
  mrb_define_method(mrb, rect_class, "y", rect_get_y, MRB_ARGS_NONE());
  mrb_define_method(mrb, rect_class, "w", rect_get_w, MRB_ARGS_NONE());
  mrb_define_method(mrb, rect_class, "h", rect_get_h, MRB_ARGS_NONE());
  mrb_define_method(mrb, rect_class, "x=", rect_set_x, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, rect_class, "y=", rect_set_y, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, rect_class, "w=", rect_set_w, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, rect_class, "h=", rect_set_h, MRB_ARGS_REQ(1));
}
