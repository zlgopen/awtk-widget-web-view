/**
 * File:   web_view_register.h
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


#ifndef TK_WEB_VIEW_REGISTER_H
#define TK_WEB_VIEW_REGISTER_H

#include "base/widget.h"

BEGIN_C_DECLS

/**
 * @method  web_view_register
 * 注册控件。
 *
 * @annotation ["global"]
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t web_view_register(void);

/**
 * @method  web_view_supported_render_mode
 * 获取支持的渲染模式。
 *
 * @annotation ["global"]
 *
 * @return {const char*} 返回渲染模式。
 */
const char* web_view_supported_render_mode(void);

END_C_DECLS

#endif /*TK_WEB_VIEW_REGISTER_H*/
