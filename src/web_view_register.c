/**
 * File:   web_view.c
 * Author: AWTK Develop Team
 * Brief:  Web View
 *
 * Copyright (c) 2024 - 2024 Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2024-11-10 Li XianJing <xianjimli@hotmail.com> created
 *
 */


#include "tkc/mem.h"
#include "tkc/utils.h"
#include "web_view_register.h"
#include "base/widget_factory.h"
#include "web_view/web_view.h"

ret_t web_view_register(void) {
  return widget_factory_register(widget_factory(), WIDGET_TYPE_WEB_VIEW, web_view_create);
}

const char* web_view_supported_render_mode(void) {
  return "OpenGL|AGGE-BGR565|AGGE-BGRA8888|AGGE-MONO";
}
