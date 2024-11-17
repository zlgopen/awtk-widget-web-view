// sdl_subwindow.h
#ifndef SDL_SUBWINDOW_H
#define SDL_SUBWINDOW_H

#include <SDL.h>
#include "tkc/types_def.h"

BEGIN_C_DECLS

/**
 * @class webview_os_window_t
 * webview 需要的窗口句柄
 * @annotation ["fake"]
 */
typedef void* webview_os_window_t;

/**
 * 创建一个子窗口
 * @param parent 父窗口
 * @param x 子窗口的 x 坐标
 * @param y 子窗口的 y 坐标
 * @param w 子窗口的宽度
 * @param h 子窗口的高度
 * @return 返回子窗口句柄
 */
webview_os_window_t webview_os_window_create(SDL_Window* parent, int x, int y, int w, int h);

/**
 * 移动和调整子窗口的大小
 * @param parent 父窗口
 * @param subwindow 子窗口
 * @param x 子窗口的 x 坐标
 * @param y 子窗口的 y 坐标
 * @param w 子窗口的宽度
 * @param h 子窗口的高度
 */
void webview_os_window_move_resize(SDL_Window* parent, webview_os_window_t subwindow, int x, int y, int w, int h);

/**
 * 销毁子窗口
 * @param subwindow 子窗口
 */
void webview_os_window_destroy(webview_os_window_t subwindow);

END_C_DECLS

#endif // SDL_SUBWINDOW_H

