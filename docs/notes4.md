# AWTK-WIDGET-WEB-VIEW 实现笔记 (4) - Ubuntu

Ubuntu 上实现 AWTK-WIDGET-WEB-VIEW 开始以为很简单，后来发现是最麻烦的。因为 Ubuntu 上的 webview 库是 基于 GTK 的，而 AWTK 是基于 X11 的，两者的窗口系统不同，所以期间踩了几个大坑。

## 1. 编译

AWTK 在使用 Linux 的输入法时，链接了 GTK3，所以编译 webview 时也要链接 GTK3, 需要指定参数 WEBVIEW_WEBKITGTK_API="4.0"。

```bash
3rd/build_linux.sh

```
cmake -S webview -B build -DWEBVIEW_WEBKITGTK_API="4.0" -DCMAKE_BUILD_TYPE=Debug
mkdir -p build/core/amalgamation

cmake --build  build

mkdir ../bin
cp -avf ./build/core/libwebview*.so* ../bin
```

## 2. 运行

GTK 在 Ubuntu 上默认使用 Wayland，但是 AWTK 使用的是 X11，所以在运行时，需要设置 GDK_BACKEND 为 x11，保证两者使用的是同一个窗口系统。

```
export GDK_BACKEND=x11
```

## 3. 创建窗口

在 Ubuntu 上，创建一个 GTK 窗口，然后将 webview 嵌入到这个窗口中。

不使用 webview 的主循环，在 AWTK 定时器中去处理 GTK 的事件，发现 webview 可以处理窗口事件，但窗口上没有显示内容，后来发现需要调用 gdk\_window\_process\_all\_updates()。

```c
static int s_webview_count = 0;
static uint32_t s_timer_id = 0;

static ret_t web_view_on_timer(const timer_info_t* timer) {
  g_main_context_iteration(NULL, FALSE);
  gdk_window_process_all_updates();

  return RET_REPEAT;
}

static ret_t sdl_window_set_child(SDL_Window* parent, webview_os_window_t os_window, int x, int y,
                                  int w, int h) {
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(parent, &wmInfo);
  Window parent_win = wmInfo.info.x11.window;
  Display* display = wmInfo.info.x11.display;

  GtkWindow* gtk_window = GTK_WINDOW(os_window);
  GdkWindow* gdk_window = gtk_widget_get_window(GTK_WIDGET(gtk_window));
  Window win = gdk_x11_window_get_xid(gdk_window);

  XReparentWindow(display, win, parent_win, x, y);
  gtk_window_move(GTK_WINDOW(gtk_window), x, y);
  gtk_window_resize(GTK_WINDOW(gtk_window), w, h);

  return RET_OK;
}

webview_os_window_t webview_os_window_create(SDL_Window* parent, int x, int y, int w, int h) {
  GtkWidget* gtk_window = gtk_window_new(GTK_WINDOW_POPUP);
  gtk_window_set_decorated(GTK_WINDOW(gtk_window), FALSE);
  gtk_window_move(GTK_WINDOW(gtk_window), x, y);
  gtk_window_resize(GTK_WINDOW(gtk_window), w, h);

  gtk_widget_show_all(gtk_window);

  if (s_webview_count == 0) {
    s_timer_id = timer_add(web_view_on_timer, NULL, 30);
    s_webview_count++;
  }

  sdl_window_set_child(parent, gtk_window, x, y, w, h);

  return (webview_os_window_t)gtk_window;
}
```

## 4. 调整窗口大小

调整窗口大小时，需要调整 GTK 窗口的大小。

> 遗憾的是窗口大小调整了，但是 webview 的内容并没有跟着调整，还需要进一步研究。

```c
void webview_os_window_move_resize(SDL_Window* parent, webview_os_window_t subwindow, int x, int y,
                                   int w, int h) {
  GtkWidget* gtk_window = GTK_WIDGET(subwindow);

  gtk_window_move(GTK_WINDOW(gtk_window), x, y);
  gtk_window_resize(GTK_WINDOW(gtk_window), w, h);
  GList* children = gtk_container_get_children(GTK_CONTAINER(gtk_window));
  if (children != NULL) {
    GtkWidget* widget = GTK_WIDGET(children->data);
    gtk_widget_set_size_request(widget, w, h);
  }
}
```

## 5. 销毁窗口

销毁窗口时，需要销毁 GTK 窗口。在销毁最后一个窗口时，停止定时器。

```c
void webview_os_window_destroy(webview_os_window_t subwindow) {
  GtkWidget* gtk_window = GTK_WIDGET(subwindow);
  gtk_widget_destroy(gtk_window);

  s_webview_count--;
  if (s_webview_count == 0) {
    timer_remove(s_timer_id);
    s_timer_id = 0;
  }
  gdk_window_process_all_updates();
}
```
