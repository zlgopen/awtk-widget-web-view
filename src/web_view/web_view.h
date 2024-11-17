/**
 * File:   web_view.h
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


#ifndef TK_WEB_VIEW_H
#define TK_WEB_VIEW_H

#include "base/widget.h"

BEGIN_C_DECLS

typedef ret_t (*web_view_on_url_changed_t)(void* ctx, const char* url);

/**
 * @class web_view_t
 * @parent widget_t
 * @annotation ["scriptable","design","widget"]
 * Web View
 * 在xml中使用"web\_view"标签创建控件。如：
 *
 * ```xml
 * <!-- ui -->
 * <web_view x="c" y="50" w="100" h="100"/>
 * ```
 *
 * 可用通过style来设置控件的显示风格，如字体的大小和颜色等等。如：
 * 
 * ```xml
 * <!-- style -->
 * <web_view>
 *   <style name="default" font_size="32">
 *     <normal text_color="black" />
 *   </style>
 * </web_view>
 * ```
 */
typedef struct _web_view_t {
  widget_t widget;

  /**
   * @property {char*} url
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * url。
   */
  char* url;

  /**
   * @property {char*} html
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * HTML Content。
   */
  char* html;

  /*private*/
  void* impl;
  web_view_on_url_changed_t on_url_changed;
  void* on_url_changed_ctx;
} web_view_t;

/**
 * @method web_view_create
 * @annotation ["constructor", "scriptable"]
 * 创建web_view对象
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} web_view对象。
 */
widget_t* web_view_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method web_view_cast
 * 转换为web_view对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget web_view对象。
 *
 * @return {widget_t*} web_view对象。
 */
widget_t* web_view_cast(widget_t* widget);


/**
 * @method web_view_set_url
 * 设置 url。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} url url。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t web_view_set_url(widget_t* widget, const char* url);

/**
 * @method web_view_set_html
 * 设置 HTML Content。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} html HTML Content。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t web_view_set_html(widget_t* widget, const char* html);

/**
 * @method web_view_back
 * 后退。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t web_view_back(widget_t* widget);

/**
 * @method web_view_forward
 * 前进。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t web_view_forward(widget_t* widget);

/**
 * @method web_view_reload
 * 重新加载。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t web_view_reload(widget_t* widget);

/**
 * @method web_view_set_url_loaded_func
 * 设置url加载完成的回调函数。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {web_view_on_url_changed_t} on_url_changed url加载完成的回调函数。
 * @param {void*} ctx 上下文。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t web_view_set_url_loaded_func(widget_t* widget, web_view_on_url_changed_t on_url_changed, void* ctx);

#define WEB_VIEW_PROP_URL "url"
#define WEB_VIEW_PROP_HTML "html"

#define WIDGET_TYPE_WEB_VIEW "web_view"

#define WEB_VIEW(widget) ((web_view_t*)(web_view_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(web_view);

END_C_DECLS

#endif /*TK_WEB_VIEW_H*/
