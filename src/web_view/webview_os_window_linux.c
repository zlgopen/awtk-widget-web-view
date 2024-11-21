#include "webview_os_window.h"
#include "base/timer.h"
#include "base/system_info.h"

#if defined(LINUX)

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <SDL_syswm.h>
#include "tkc/mem.h"
#include "base/idle.h"

static int s_webview_count = 0;
static uint32_t s_timer_id = 0;

static ret_t web_view_on_timer(const timer_info_t* timer) {
  g_main_context_iteration(NULL, FALSE);
  gdk_window_process_all_updates();

  return RET_REPEAT;
}

static ret_t sdl_window_set_child(widget_t* widget, webview_os_window_t os_window, int x, int y,
                                  int w, int h) {
  SDL_SysWMinfo wmInfo;
  SDL_Window* parent = (SDL_Window*)(widget_get_native_window(widget)->handle);

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

webview_os_window_t webview_os_window_create(widget_t* widget, int x, int y, int w, int h) {
  GtkWidget* gtk_window = gtk_window_new(GTK_WINDOW_POPUP);
  SDL_Window* parent = (SDL_Window*)(widget_get_native_window(widget)->handle);

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

void webview_os_window_move_resize(widget_t* widget, webview_os_window_t subwindow, int x, int y,
                                   int w, int h) {
  GtkWidget* gtk_window = GTK_WIDGET(subwindow);
  SDL_Window* parent = (SDL_Window*)(widget_get_native_window(widget)->handle);

  gtk_window_move(GTK_WINDOW(gtk_window), x, y);
  gtk_window_resize(GTK_WINDOW(gtk_window), w, h);
  GList* children = gtk_container_get_children(GTK_CONTAINER(gtk_window));
  if (children != NULL) {
    GtkWidget* widget = GTK_WIDGET(children->data);
    gtk_widget_set_size_request(widget, w, h);
  }
}

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
#endif/*LINUX*/
