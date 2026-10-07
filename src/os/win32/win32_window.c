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
    HWND        hwnd;
    BITMAPINFO  bitmap_info;
    HBITMAP     bitmap;
    HDC         device_context;
    RECT        rect;
    RECT        windowed_rect;
} platform_context;

void window_on_resize(sr_window *window, LPARAM lparam);
LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

platform_context *window_create_platform_context()
{
    return malloc(sizeof(platform_context));
}

sr_window *window_create(const char *title, u32 width, u32 height)
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

    sr_window *wnd = calloc(1, sizeof(sr_window));
    wnd->platform  = calloc(1, sizeof(platform_context));

    wnd->platform->hwnd = CreateWindowEx(
        0, CLASS_NAME,
        title, 
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height, 
        NULL, NULL, 
        GetModuleHandle(NULL), NULL
    );

    
    wnd->backbuffer = calloc(1, sizeof(sr_backbuffer));
    wnd->title = title;
    wnd->width = width;
    wnd->height = height;
    wnd->isOpen = true;
    SetProp(wnd->platform->hwnd, "SoftRendWindow", (HANDLE)wnd);
    SetProp(wnd->platform->hwnd, "SoftRendPlatformContext", (HANDLE)wnd->platform);

    // Bitmap setup
    wnd->platform->device_context = CreateCompatibleDC(0);
    wnd->platform->bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    wnd->platform->bitmap_info.bmiHeader.biPlanes = 1;
    wnd->platform->bitmap_info.bmiHeader.biBitCount = 32;
    wnd->platform->bitmap_info.bmiHeader.biCompression = BI_RGB;

    ShowWindow(wnd->platform->hwnd, SW_NORMAL);
    SetFocus(wnd->platform->hwnd);

    memset(wnd->backbuffer->buffer, 0x111111, wnd->backbuffer->width * wnd->backbuffer->height * sizeof(u32));

    return wnd;
}

void window_swap_buffers(sr_window *window)
{
    InvalidateRect(window->platform->hwnd, NULL, false);

    PAINTSTRUCT paint;
    HDC hdc = BeginPaint(window->platform->hwnd, &paint);

    u32 buf_size = window->backbuffer->width * window->backbuffer->height * sizeof(u32);
    memcpy(window->frontbuffer, window->backbuffer->buffer, buf_size);
    BitBlt(hdc, 
        paint.rcPaint.left, paint.rcPaint.top, 
        window->backbuffer->width, window->backbuffer->height, 
        window->platform->device_context, 
        paint.rcPaint.left, paint.rcPaint.top, 
        SRCCOPY);

    EndPaint(window->platform->hwnd, &paint);

    window_pump_messages(window);
}

void window_cleanup(sr_window *window)
{
    DestroyWindow(window->platform->hwnd);
    DeleteObject(window->platform->bitmap);
    DeleteDC(window->platform->device_context);

    free(window->backbuffer->buffer);
    free(window->backbuffer);
    free(window->platform);
    free(window);
}

bool window_is_open(sr_window *window)
{
    return window->isOpen;
}

void window_close(sr_window *window)
{
    window->isOpen = false;
}

void window_pump_messages(sr_window *window)
{
    MSG msg = {};
    while (PeekMessage(&msg, window->platform->hwnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void window_on_resize(sr_window *window, LPARAM lparam)
{
    window->width  = LOWORD(lparam);
    window->height = HIWORD(lparam);

    if (window->platform->bitmap)
    {
        DeleteObject(window->platform->bitmap);
    }

    window->platform->bitmap_info.bmiHeader.biWidth  = window->width;
    window->platform->bitmap_info.bmiHeader.biHeight = window->height;

    window->platform->bitmap = CreateDIBSection( NULL, 
                                        &window->platform->bitmap_info, 
                                        DIB_RGB_COLORS, 
                                        (void**)&window->frontbuffer, 
                                        NULL, 0);
    SelectObject(window->platform->device_context, window->platform->bitmap);

    free(window->backbuffer->buffer);
    window->backbuffer->buffer = malloc(window->width * window->height * sizeof(u32));
    window->backbuffer->width  = window->width;
    window->backbuffer->height = window->height;
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    sr_window *window = GetProp(hwnd, "SoftRendWindow");
    platform_context *window->platform = GetProp(hwnd, "SoftRendPlatformContext");
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

        case WM_SIZE:
        {
            window_on_resize(window, lparam);
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