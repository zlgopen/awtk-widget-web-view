#import <SDL.h>
#import <SDL_syswm.h>
#import <Cocoa/Cocoa.h>
#include "webview_os_window.h"

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
    CGFloat childY = NSMaxY(parentFrame) - y - NSHeight(frame); // 倒置y轴以适应窗口坐标系统

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

void webview_os_window_destroy(webview_os_window_t subwindow) {
    NSWindow *childWindow = (__bridge NSWindow*)subwindow;
    [[NSNotificationCenter defaultCenter] removeObserver:childWindow];
    [childWindow close];
}
