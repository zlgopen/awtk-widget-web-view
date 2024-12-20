﻿/**
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

#include "webview.h"
#include "tkc/mem.h"
#include "tkc/utils.h"
#include "base/timer.h"
#include "widgets/pages.h"
#include "base/window_manager.h"

#include "web_view.h"
#include "webview_os_window.h"

static ret_t web_view_create_impl(widget_t* widget);
static ret_t web_view_destroy_impl(widget_t* widget);

static const char* s_webview_init_js =
    "window.url_changed_timer_id = window.setInterval(function() {\n"
    "  window.on_url_changed(window.location.href);\n"
    "}, 1000);\n"
    "window.addEventListener('load', function() {\n"
    "  window.on_url_changed(window.location.href);\n"
    "  const links = document.querySelectorAll('a[target=\"_blank\"]'); \n"
    "  links.forEach(function(link) { \n"
    "    link.addEventListener('click', function(event) { \n"
    "      event.preventDefault(); \n"
    "      window.location.href = link.href; \n"
    "    }); \n"
    "  }); \n"
    "});\n";

static const char* s_webview_deinit_js = "clearInterval(window.url_changed_timer_id);";

static ret_t web_view_set_active(widget_t* widget, bool_t value) {
  web_view_t* web_view = WEB_VIEW(widget);
  return_value_if_fail(widget != NULL && web_view != NULL, RET_BAD_PARAMS);

  if (web_view->impl != NULL) {
    webview_t w = (webview_t)(web_view->impl);
    widget_set_visible(widget, value);
    webview_os_window_t window =
        (webview_os_window_t)webview_get_native_handle(w, WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW);
    webview_os_window_show(window, value);
  }

  return RET_OK;
}

static ret_t web_view_set_active_if_is_current_page(widget_t* widget) {  
  widget_t* my_page = NULL;
  widget_t* current_page = NULL;
  web_view_t* web_view = WEB_VIEW(widget);
  widget_t* pages = widget_find_parent_by_type(widget, WIDGET_TYPE_PAGES);
  return_value_if_fail(web_view != NULL && pages != NULL, RET_BAD_PARAMS);

  my_page = widget->parent;
  while (my_page != NULL && my_page->parent != pages) {
    my_page = my_page->parent;
  }

  current_page = widget_get_child(pages, PAGES(pages)->active);
  if (my_page == current_page) {
    if (web_view->impl == NULL) {
      web_view_create_impl(widget);
    }

    web_view_set_active(widget, TRUE);
  } else {
    web_view_set_active(widget, FALSE);
  }

  return RET_OK;
}

static ret_t web_view_on_pages_changed(void* ctx, event_t* e) {
  return web_view_set_active_if_is_current_page(WIDGET(ctx));
}

static ret_t web_view_on_window_to_background(void* ctx, event_t* e) {
  return web_view_set_active(WIDGET(ctx), FALSE);
}

static ret_t web_view_on_window_to_foreground(void* ctx, event_t* e) {
  return web_view_set_active(WIDGET(ctx), TRUE);
}

static void web_view_on_url_changed(const char* id, const char* req, void* arg) {
  /*req=["https://www.zlg.cn/index.html"] */
  char url[1024] = {0};
  const char* start = req + 2;
  const char* end = req + strlen(req) - 2;
  int len = end - start;
  web_view_t* web_view = WEB_VIEW(arg);
  return_if_fail(web_view != NULL && web_view->impl != NULL);

  tk_strncpy(url, start, len);
  if (tk_str_eq(url, web_view->url)) {
    return;
  }

  web_view->url = tk_str_copy(web_view->url, url);
  if (web_view->on_url_changed != NULL) {
    web_view->on_url_changed(web_view->on_url_changed_ctx, url);
  }

  return;
}

static ret_t web_view_destroy_impl(widget_t* widget);

ret_t web_view_set_url(widget_t* widget, const char* url) {
  web_view_t* web_view = WEB_VIEW(widget);
  return_value_if_fail(web_view != NULL, RET_BAD_PARAMS);

  web_view->url = tk_str_copy(web_view->url, url);
  if (web_view->impl != NULL) {
    webview_t w = (webview_t)(web_view->impl);
    webview_navigate(w, web_view->url);
  }

  return RET_OK;
}

ret_t web_view_set_html(widget_t* widget, const char* html) {
  web_view_t* web_view = WEB_VIEW(widget);
  return_value_if_fail(web_view != NULL, RET_BAD_PARAMS);

  web_view->html = tk_str_copy(web_view->html, html);
  if (web_view->impl != NULL) {
    webview_t w = (webview_t)(web_view->impl);
    webview_set_html(w, web_view->html);
  }

  return RET_OK;
}

static ret_t web_view_get_prop(widget_t* widget, const char* name, value_t* v) {
  web_view_t* web_view = WEB_VIEW(widget);
  return_value_if_fail(web_view != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(WEB_VIEW_PROP_URL, name)) {
    value_set_str(v, web_view->url);
    return RET_OK;
  } else if (tk_str_eq(WEB_VIEW_PROP_HTML, name)) {
    value_set_str(v, web_view->html);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t web_view_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(WEB_VIEW_PROP_URL, name)) {
    web_view_set_url(widget, value_str(v));
    return RET_OK;
  } else if (tk_str_eq(WEB_VIEW_PROP_HTML, name)) {
    web_view_set_html(widget, value_str(v));
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t web_view_on_destroy(widget_t* widget) {
  widget_t* window = NULL;
  web_view_t* web_view = WEB_VIEW(widget);
  return_value_if_fail(widget != NULL && web_view != NULL, RET_BAD_PARAMS);

  window = widget_get_window(widget);
  if (window != NULL) {
    widget_off_by_ctx(window, widget);
  }

  
  if (web_view->impl != NULL) {
    webview_t w = (webview_t)web_view->impl;
    webview_eval(w, s_webview_deinit_js);
    webview_terminate(w);
    web_view_destroy_impl(widget);
  }

  TKMEM_FREE(web_view->url);
  TKMEM_FREE(web_view->html);

  return RET_OK;
}

static ret_t web_view_on_paint_self(widget_t* widget, canvas_t* c) {
  web_view_t* web_view = WEB_VIEW(widget);

  (void)web_view;

  return RET_OK;
}

#ifdef WIN32
#define NATIVE_WINDOW_TITLE_BAR_HEIGHT 36
#elif defined(LINUX)
#define NATIVE_WINDOW_TITLE_BAR_HEIGHT 34
#else
#define NATIVE_WINDOW_TITLE_BAR_HEIGHT 30
#endif

static ret_t web_view_get_rect(widget_t* widget, rect_t* rect) {
  point_t pos = {0, 0};
  return_value_if_fail(widget != NULL && rect != NULL, RET_BAD_PARAMS);

  widget_to_global(widget, &pos);
  rect->x = pos.x;
  rect->y = pos.y + NATIVE_WINDOW_TITLE_BAR_HEIGHT;
  rect->w = widget->w;
  rect->h = widget->h;

  return RET_OK;
}

static ret_t web_view_move_resize_webview(widget_t* widget) {
  web_view_t* web_view = WEB_VIEW(widget);
  if (web_view->impl != NULL) {
    rect_t rect = {0, 0, 0, 0};
    webview_t w = (webview_t)(web_view->impl);
    void* os_window = webview_get_native_handle(w, WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW);

    web_view_get_rect(widget, &rect);
    webview_os_window_move_resize(widget, os_window, rect.x, rect.y, rect.w, rect.h);
  }

  return RET_OK;
}

static ret_t web_view_destroy_impl_async(const timer_info_t* info) {
  widget_t* widget = WIDGET(info->ctx);

  web_view_destroy_impl(widget);

  return RET_REMOVE;
}

static ret_t web_view_destroy_impl(widget_t* widget) {
  web_view_t* web_view = WEB_VIEW(widget);
  return_value_if_fail(widget != NULL && web_view != NULL, RET_BAD_PARAMS);

  webview_t w = (webview_t)(web_view->impl);
  if (w != NULL) {
    webview_os_window_t window =
        (webview_os_window_t)webview_get_native_handle(w, WEBVIEW_NATIVE_HANDLE_KIND_UI_WINDOW);
    webview_destroy(w);
    webview_os_window_destroy(window);
  }

  return RET_OK;
}

static ret_t web_view_create_impl(widget_t* widget) {
  webview_t w = NULL;
  void* os_window = NULL;
  rect_t rect = {0, 0, 0, 0};
  web_view_t* web_view = WEB_VIEW(widget);

  web_view_get_rect(widget, &rect);
  os_window = webview_os_window_create(widget, rect.x, rect.y, rect.w, rect.h);

  w = webview_create(0, os_window);
  webview_init(w, s_webview_init_js);

  web_view->impl = w;
  if (TK_STR_IS_NOT_EMPTY(web_view->url)) {
    webview_navigate(w, web_view->url);
  } else if (TK_STR_IS_NOT_EMPTY(web_view->html)) {
    webview_set_html(w, web_view->html);
  }

#ifdef WIN32
  web_view_move_resize_webview(widget);
#endif /*WIN32*/

  webview_bind(w, "on_url_changed", web_view_on_url_changed, web_view);

  return RET_OK;
}

static ret_t web_view_init(widget_t* widget) {
  widget_t* window = widget_get_window(widget);
  widget_t* pages = widget_find_parent_by_type(widget, WIDGET_TYPE_PAGES);

  if (pages != NULL) {
    web_view_set_active_if_is_current_page(widget);
    widget_on(pages, EVT_PAGE_CHANGED, web_view_on_pages_changed, widget);
  } else {
    web_view_create_impl(widget);
  }
  
  if (window != NULL) {
    widget_on(window, EVT_WINDOW_TO_BACKGROUND, web_view_on_window_to_background, widget);
    widget_on(window, EVT_WINDOW_TO_FOREGROUND, web_view_on_window_to_foreground, widget);
  }

  return RET_OK;
}
static ret_t web_view_on_event(widget_t* widget, event_t* e) {
  web_view_t* web_view = WEB_VIEW(widget);
  return_value_if_fail(widget != NULL && web_view != NULL, RET_BAD_PARAMS);

  switch (e->type) {
    case EVT_WINDOW_OPEN: {
      web_view_init(widget);
      break;
    }
    case EVT_MOVE_RESIZE:
    case EVT_RESIZE: {
      if (web_view->impl != NULL) {
        web_view_move_resize_webview(widget);
      }
      break;
    }
  }
  (void)web_view;

  return RET_OK;
}

const char* s_web_view_properties[] = {WEB_VIEW_PROP_URL, WEB_VIEW_PROP_HTML, NULL};

TK_DECL_VTABLE(web_view) = {.size = sizeof(web_view_t),
                            .type = WIDGET_TYPE_WEB_VIEW,
                            .clone_properties = s_web_view_properties,
                            .persistent_properties = s_web_view_properties,
                            .parent = TK_PARENT_VTABLE(widget),
                            .create = web_view_create,
                            .get_prop = web_view_get_prop,
                            .set_prop = web_view_set_prop,
                            .on_paint_self = web_view_on_paint_self,
                            .on_event = web_view_on_event,
                            .on_destroy = web_view_on_destroy};

widget_t* web_view_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(web_view), x, y, w, h);
  web_view_t* web_view = WEB_VIEW(widget);
  return_value_if_fail(web_view != NULL, NULL);

  return widget;
}

widget_t* web_view_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, web_view), NULL);

  return widget;
}

ret_t web_view_back(widget_t* widget) {
  web_view_t* web_view = WEB_VIEW(widget);
  return_value_if_fail(web_view != NULL, RET_BAD_PARAMS);

  if (web_view->impl != NULL) {
    webview_t w = (webview_t)(web_view->impl);
    webview_eval(w, "history.back()");
  }

  return RET_OK;
}
ret_t web_view_forward(widget_t* widget) {
  web_view_t* web_view = WEB_VIEW(widget);
  return_value_if_fail(web_view != NULL, RET_BAD_PARAMS);

  if (web_view->impl != NULL) {
    webview_t w = (webview_t)(web_view->impl);
    webview_eval(w, "history.forward()");
  }

  return RET_OK;
}

ret_t web_view_reload(widget_t* widget) {
  web_view_t* web_view = WEB_VIEW(widget);
  return_value_if_fail(web_view != NULL, RET_BAD_PARAMS);

  if (web_view->impl != NULL) {
    webview_t w = (webview_t)(web_view->impl);
    webview_eval(w, "location.reload()");
  }

  return RET_OK;
}

ret_t web_view_set_url_loaded_func(widget_t* widget, web_view_on_url_changed_t on_url_changed,
                                   void* ctx) {
  web_view_t* web_view = WEB_VIEW(widget);
  return_value_if_fail(web_view != NULL, RET_BAD_PARAMS);

  web_view->on_url_changed = on_url_changed;
  web_view->on_url_changed_ctx = ctx;

  return RET_OK;
}
