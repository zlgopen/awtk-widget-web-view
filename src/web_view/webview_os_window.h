// sdl_subwindow.h
#ifndef SDL_SUBWINDOW_H
#define SDL_SUBWINDOW_H

#include <SDL.h>
#include "awtk.h"

BEGIN_C_DECLS

/**
 * @class webview_os_window_t
 * webview 需要的窗口句柄
 * @annotation ["fake"]
 */
typedef void* webview_os_window_t;

/**
 * @method webview_os_window_create
 * 创建webview需要的窗口句柄
 * @param {widget_t*} widget widget对象
 * @param {int} x x坐标
 * @param {int} y y坐标
 * @param {int} w 宽度
 * @param {int} h 高度
 * @return {webview_os_window_t} 返回webview需要的窗口句柄
 */
webview_os_window_t webview_os_window_create(widget_t* widget, int x, int y, int w, int h);

/**
 * @method webview_os_window_move_resize
 * 移动和调整窗口大小
 * @param {widget_t*} widget widget对象
 * @param {webview_os_window_t} subwindow 窗口句柄
 * @param {int} x x坐标
 * @param {int} y y坐标
 * @param {int} w 宽度
 * @param {int} h 高度
 */
void webview_os_window_move_resize(widget_t* widget, webview_os_window_t subwindow, int x, int y, int w, int h);

/**
 * @method webview_os_window_show
 * 显示或隐藏窗口
 * @param {webview_os_window_t} subwindow 窗口句柄
 * @param {bool_t} show 是否显示
 */
void webview_os_window_show(webview_os_window_t subwindow, bool_t show);

/**
 * @method webview_os_window_destroy
 * 销毁窗口
 * @param {webview_os_window_t} subwindow 窗口句柄
 */
void webview_os_window_destroy(webview_os_window_t subwindow);

END_C_DECLS

#endif // SDL_SUBWINDOW_H

