#include "window/window.h"

#include <windows.h>

#include <stdio.h>

#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
    #define NOMINMAX
#endif

typedef struct platform_context
{
    HWND        window;
    BITMAPINFO  frame_info;
    HBITMAP     bitmap_handle;
    HDC         device_context;
    RECT        rect;
    RECT        windowed_rect;
} platform_context;

LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

platform_context *window_create_platform_context()
{
    return malloc(sizeof(platform_context));
}

sr_window *window_create(platform_context *context, const char *title, u32 width, u32 height)
{
    const char CLASS_NAME[] = { "SoftRendWindowClass" };
    WNDCLASS wc = 
    {
        .lpfnWndProc = window_proc,
        .hInstance = GetModuleHandle(NULL),
        .lpszClassName = CLASS_NAME,
        .hCursor = LoadCursor(NULL, IDC_ARROW)
    };
    RegisterClass(&wc);

    context->window = CreateWindowEx(
        0, CLASS_NAME,
        title, 
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height, 
        NULL, NULL, 
        GetModuleHandle(NULL), NULL
    );

    sr_window *wnd = malloc(sizeof(sr_window));
    wnd->title = title;
    wnd->width = width;
    wnd->height = height;
    wnd->isOpen = true;

    SetProp(context->window, "SoftRendWindow", (HANDLE)wnd);

    ShowWindow(context->window, SW_NORMAL);
    SetFocus(context->window);

    printf("Hello World!\n");

    return wnd;
}

void window_cleanup(platform_context *context, sr_window *window)
{
    printf("Goodbye World!\n");
    
    DestroyWindow(context->window);
    DeleteObject(context->bitmap_handle);
    DeleteDC(context->device_context);

    free(window);
    free(context);
}

bool window_is_open(sr_window *window)
{
    return window->isOpen;
}

void window_close(sr_window *window)
{
    window->isOpen = false;
}

void window_pump_messages(platform_context *context)
{
    MSG msg = {};
    while (PeekMessage(&msg, context->window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    sr_window *window = GetProp(hwnd, "SoftRendWindow");
    if (!window)
    {
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    switch (msg)
    {
        case WM_CLOSE:
        {
            window_close(window);
            
        } break;

        case WM_DESTROY:
        {
            RemoveProp(hwnd, "SoftRendWindow");
            PostQuitMessage(0);
        } break;

        default:
        {
            return DefWindowProc(hwnd, msg, wparam, lparam);
        } break;
    }
}