# AWTK-WIDGET-WEB-VIEW 实现笔记 (3) - MacOS

MacOS 上实现 AWTK-WIDGET-WEB-VIEW 有点麻烦，主要原因是没有一个简单的办法将一个 WebView 嵌入到一个窗口中。所以，我们只能通过创建一个独立的窗口来实现。

## 1. 创建窗口

我对 Object-C 不熟悉，也不熟悉 Cocoa 框架，在 ChatGPT 的帮助下，实现了一个简单的窗口创建函数。这个函数接受一个 SDL_Window 指针作为父窗口，然后创建一个新的窗口，将其设置为父窗口的子窗口。这样，新窗口就会在父窗口的上方显示。

> 在移动窗口时，子窗口也会跟着移动，但是有些延迟。

```c
webview_os_window_t webview_os_window_create(SDL_Window* parent, int x, int y, int w, int h) {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(parent, &info);

    NSRect frame = NSMakeRect(x, y, w, h);
    NSWindow *parentWindow = (NSWindow *)info.info.cocoa.window;
    NSWindow* childWindow = [[NSWindow alloc] initWithContentRect:frame
                                                styleMask:NSWindowStyleMaskBorderless // 无边框样式
                                                  backing:NSBackingStoreBuffered
                                                    defer:NO];
    
    // 设置窗口级别以确保它在父窗口上方
    [childWindow setLevel:NSFloatingWindowLevel]; // 使子窗口浮动在父窗口之上

    // 设置子窗口的初始位置相对于父窗口
    NSRect parentFrame = [parentWindow frame];
    CGFloat childX = NSMinX(parentFrame) + x;
    CGFloat childY = NSMaxY(parentFrame) - y - NSHeight(frame); // 倒置 y 轴以适应窗口坐标系统

    [childWindow setParentWindow:parentWindow]; // 设置父窗口

    // 设置子窗口的位置
    [childWindow setFrame:NSMakeRect(childX, childY, w, h) display:YES];

    // 使子窗口成为关键窗口并显示
    [childWindow makeKeyAndOrderFront:nil];

    // 观察父窗口的移动和大小变化
    [[NSNotificationCenter defaultCenter] addObserverForName:NSWindowDidMoveNotification
                                                      object:parentWindow
                                                       queue:nil
                                                  usingBlock:^(NSNotification *note) {
        NSRect newParentFrame = [parentWindow frame];
        [childWindow setFrameOrigin:NSMakePoint(NSMinX(newParentFrame) + x, NSMaxY(newParentFrame) - y - NSHeight(frame))];
    }];

    [[NSNotificationCenter defaultCenter] addObserverForName:NSWindowDidResizeNotification
                                                      object:parentWindow
                                                       queue:nil
                                                  usingBlock:^(NSNotification *note) {
        NSRect newParentFrame = [parentWindow frame];
        [childWindow setFrameOrigin:NSMakePoint(NSMinX(newParentFrame) + x, NSMaxY(newParentFrame) - y - NSHeight(frame))];
    }];

    // 观察父窗口切换到后面时，自动切换到后面
    [[NSNotificationCenter defaultCenter] addObserverForName:NSWindowDidResignKeyNotification
                                                      object:parentWindow
                                                       queue:nil
                                                  usingBlock:^(NSNotification *note) {
        [childWindow orderOut:nil];
    }];

    [[NSNotificationCenter defaultCenter] addObserverForName:NSWindowDidBecomeKeyNotification
                                                      object:parentWindow
                                                       queue:nil
                                                  usingBlock:^(NSNotification *note) {
        [childWindow makeKeyAndOrderFront:nil];
    }];
    
    return (__bridge void*)childWindow; // 使用 __bridge 传递所有权
}
```

## 2. 调整窗口大小

奇怪的是从 subwindow 获取父窗口的方法不起作用，所以通过传递父窗口的指针来调整子窗口的大小。

```c
void webview_os_window_move_resize(SDL_Window* parent, webview_os_window_t subwindow, int x, int y, int w, int h) {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(parent, &info);
    NSWindow *childWindow = (__bridge NSWindow*)subwindow;
    NSWindow *parentWindow = info.info.cocoa.window;

    NSRect frame = NSMakeRect(x, y, w, h);
    [childWindow setFrame:frame display:YES];
     
    NSRect newParentFrame = [parentWindow frame];
    [childWindow setFrameOrigin:NSMakePoint(NSMinX(newParentFrame) + x, NSMaxY(newParentFrame) - y - NSHeight(frame))];
}
```

## 3. 销毁窗口

销毁窗口时，需要将子窗口从父窗口中移除。

```c
void webview_os_window_destroy(webview_os_window_t subwindow) {
    NSWindow *childWindow = (__bridge NSWindow*)subwindow;
    [[NSNotificationCenter defaultCenter] removeObserver:childWindow];
    [childWindow close];
}
```
