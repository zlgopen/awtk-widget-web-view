#include "web_view/web_view.h"
#include "gtest/gtest.h"

TEST(web_view, basic) {
  value_t v;
  widget_t* w = web_view_create(NULL, 10, 20, 30, 40);

  widget_destroy(w);
}
