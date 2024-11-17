# AWTK-WIDGET-WEB-VIEW 实现笔记 (2) - Windows

在  Windows 平台上的实现，相对比较顺利，将一个窗口嵌入到另外一个窗口是比较容易的事情。

## 1. 创建窗口

这里有点需要注意：

* 父窗口的大小变化时，子窗口也要跟着变化，否则 webview 显示不出来。
* 创建时窗口的大小先设置为 0，后面再调整，否则 webview 也显示不出来。
  
```c
#include <windows.h>
#include <SDL_syswm.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_SIZE: {
      HWND hwndSub = FindWindowEx(hwnd, NULL, NULL, NULL);
      if (hwndSub) {
        RECT rcClient;
        int width = 0;
        int height = 0;

        GetClientRect(hwnd, &rcClient);
        width = rcClient.right - rcClient.left;
        height = rcClient.bottom - rcClient.top;
        MoveWindow(hwndSub, 0, 0, width, height, TRUE);
      }
      break;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
      EndPaint(hwnd, &ps);
      break;
    }
    default:
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
  return 0;
}

static const char CLASS_NAME[] = "WebViewContainer";

static ret_t webview_os_window_init(HINSTANCE hInstance) {
  WNDCLASSEX wc;

  ZeroMemory(&wc, sizeof(WNDCLASSEX));
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;
  wc.lpfnWndProc = WindowProc;

  RegisterClassEx(&wc);

  return RET_OK;
}

webview_os_window_t webview_os_window_create(SDL_Window* parent, int x, int y, int w, int h) {
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(parent, &wmInfo);

  HWND hwndParent = wmInfo.info.win.window;
  HINSTANCE hInstance = wmInfo.info.win.hinstance;

  webview_os_window_init(hInstance);

  HWND hwndSub = CreateWindowEx(0, CLASS_NAME, "Container Window", WS_CHILD | WS_VISIBLE, x, y, 0,
                                0, hwndParent, NULL, hInstance, NULL);

  if (hwndSub == NULL) {
    return 0;
  }

  ShowWindow(hwndSub, SW_SHOW);
  UpdateWindow(hwndSub);

  if (!hwndSub) {
    printf("Failed to create subwindow: %lu\n", GetLastError());
    return NULL;
  }

  return (webview_os_window_t)hwndSub;
}

```

## 2. 调整窗口大小

resize 窗口时，需要调整子窗口的大小。要注意的是，窗口的大小是以像素为单位的，所以需要考虑缩放因子。

```c
void webview_os_window_move_resize(SDL_Window* parent, webview_os_window_t subwindow, int x, int y,
                                   int w, int h) {
  HWND hwndSub = (HWND)subwindow;
  float scale = system_info()->device_pixel_ratio;

  MoveWindow(hwndSub, x, y, w * scale, h * scale, TRUE);
}
```

## 3. 销毁窗口

销毁窗口时，需要销毁子窗口。

```c
void webview_os_window_destroy(webview_os_window_t subwindow) {
  HWND hwndSub = (HWND)subwindow;
  DestroyWindow(hwndSub);
}
```
