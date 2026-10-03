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
    BITMAPINFO  bitmap_info;
    HBITMAP     bitmap;
    HDC         device_context;
    RECT        rect;
    RECT        windowed_rect;
} platform_context;

void window_on_resize(platform_context *context, sr_window *window, LPARAM lparam);
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
    ZeroMemory(wnd, sizeof(sr_window));
    wnd->backbuffer = malloc(sizeof(sr_backbuffer));
    ZeroMemory(wnd->backbuffer, sizeof(sr_backbuffer));
    wnd->title = title;
    wnd->width = width;
    wnd->height = height;
    wnd->isOpen = true;
    SetProp(context->window, "SoftRendWindow", (HANDLE)wnd);
    SetProp(context->window, "SoftRendPlatformContext", (HANDLE)context);

    // Bitmap setup
    context->device_context = CreateCompatibleDC(0);
    context->bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    context->bitmap_info.bmiHeader.biPlanes = 1;
    context->bitmap_info.bmiHeader.biBitCount = 32;
    context->bitmap_info.bmiHeader.biCompression = BI_RGB;

    ShowWindow(context->window, SW_NORMAL);
    SetFocus(context->window);

    memset(wnd->backbuffer->buffer, 0x111111, wnd->backbuffer->width * wnd->backbuffer->height * sizeof(u32));

    return wnd;
}

void window_swap_buffers(platform_context *context, sr_window *window)
{
    InvalidateRect(context->window, NULL, false);

    PAINTSTRUCT paint;
    HDC hdc = BeginPaint(context->window, &paint);

    u32 buf_size = window->backbuffer->width * window->backbuffer->height * sizeof(u32);
    memcpy(window->frontbuffer, window->backbuffer->buffer, buf_size);
    BitBlt(hdc, 
        paint.rcPaint.left, paint.rcPaint.top, 
        window->backbuffer->width, window->backbuffer->height, 
        context->device_context, 
        paint.rcPaint.left, paint.rcPaint.top, 
        SRCCOPY);

    EndPaint(context->window, &paint);

    window_pump_messages(context);
}

void window_cleanup(platform_context *context, sr_window *window)
{
    DestroyWindow(context->window);
    DeleteObject(context->bitmap);
    DeleteDC(context->device_context);

    free(window->backbuffer->buffer);
    free(window->backbuffer);
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

void window_on_resize(platform_context *context, sr_window *window, LPARAM lparam)
{
    window->width  = LOWORD(lparam);
    window->height = HIWORD(lparam);

    if (context->bitmap)
    {
        DeleteObject(context->bitmap);
    }

    context->bitmap_info.bmiHeader.biWidth  = window->width;
    context->bitmap_info.bmiHeader.biHeight = window->height;

    context->bitmap = CreateDIBSection( NULL, 
                                        &context->bitmap_info, 
                                        DIB_RGB_COLORS, 
                                        (void**)&window->frontbuffer, 
                                        NULL, 0);
    SelectObject(context->device_context, context->bitmap);

    free(window->backbuffer->buffer);
    window->backbuffer->buffer = malloc(window->width * window->height * sizeof(u32));
    window->backbuffer->width  = window->width;
    window->backbuffer->height = window->height;
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    sr_window *window = GetProp(hwnd, "SoftRendWindow");
    platform_context *context = GetProp(hwnd, "SoftRendPlatformContext");
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
            window_on_resize(context, window, lparam);
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