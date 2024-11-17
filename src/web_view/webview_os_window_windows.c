#include "webview_os_window.h"
#include "base/timer.h"
#include "base/system_info.h"

#if defined(_WIN32) || defined(_WIN64)
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

void webview_os_window_move_resize(SDL_Window* parent, webview_os_window_t subwindow, int x, int y,
                                   int w, int h) {
  HWND hwndSub = (HWND)subwindow;
  float scale = system_info()->device_pixel_ratio;

  MoveWindow(hwndSub, x, y, w * scale, h * scale, TRUE);
}

void webview_os_window_destroy(webview_os_window_t subwindow) {
  HWND hwndSub = (HWND)subwindow;
  DestroyWindow(hwndSub);
}

#endif /*WINDOWS*/
