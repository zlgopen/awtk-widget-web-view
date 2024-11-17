#include "awtk.h"
#include "web_view_register.h"
#include "web_view/web_view.h"

static ret_t on_go(void* ctx, event_t* e) {
  char text[1024] = {0};
  widget_t* win = WIDGET(ctx);
  widget_t* webview = widget_lookup(win, "webview", TRUE);
  widget_t* url = widget_lookup(win, "url", TRUE);
 
  widget_get_text_utf8(url, text, sizeof(text)-1);
  web_view_set_url(webview, text);

  return RET_OK;
}

static ret_t on_close(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_close_window(win);
  
  return RET_OK;
}

static ret_t on_back(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* webview = widget_lookup(win, "webview", TRUE);
  web_view_back(webview);

  return RET_OK;
}

static ret_t on_forward(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* webview = widget_lookup(win, "webview", TRUE);
  web_view_forward(webview);

  return RET_OK;
}

static ret_t on_reload(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* webview = widget_lookup(win, "webview", TRUE);
  web_view_reload(webview);

  return RET_OK;
}

static  ret_t on_url_changed(void* ctx, const char* url) {
  widget_t* win = WIDGET(ctx);
  widget_t* url_widget = widget_lookup(win, "url", TRUE);
  widget_set_text_utf8(url_widget, url);

  return RET_OK;
}

static ret_t on_quit(void* ctx, event_t* e) {
  tk_quit();

  return RET_OK;
}


static ret_t on_open_webview(void* ctx, event_t* e) {
  widget_t* win = window_open("main");
  widget_t* webview = widget_lookup(win, "webview", TRUE);

  widget_child_on(win, "go", EVT_CLICK, on_go, win); 
  widget_child_on(win, "back", EVT_CLICK, on_back, win);
  widget_child_on(win, "forward", EVT_CLICK, on_forward, win);
  widget_child_on(win, "reload", EVT_CLICK, on_reload, win);
  widget_child_on(win, "close", EVT_CLICK, on_close, win); 

  web_view_set_url_loaded_func(webview, on_url_changed, win);
  on_go(win, NULL);

  return RET_OK;
}

/**
 * 初始化
 */
ret_t application_init(void) {
  web_view_register();

  widget_t* win = window_open("start");
  widget_child_on(win, "webview", EVT_CLICK, on_open_webview, win);
  widget_child_on(win, "quit", EVT_CLICK, on_quit, win); 

  return RET_OK;
}

/**
 * 退出
 */
ret_t application_exit(void) {
  log_debug("application_exit\n");
  return RET_OK;
}
