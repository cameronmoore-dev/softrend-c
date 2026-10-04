#pragma once

#include "defines.h"

typedef struct platform_context platform_context;

typedef struct sr_backbuffer
{
    u32 width;
    u32 height;
    u32 *data;
} sr_backbuffer;

typedef struct sr_window
{
    platform_context *platform;
    u32 *frontbuffer;
    sr_backbuffer *backbuffer;
    const char *title;
    u32 width;
    u32 height;
    bool isOpen;
} sr_window;

sr_window *window_create(const char *title, u32 width, u32 height);
void window_swap_buffers(sr_window *window);
void window_cleanup(sr_window *window);
void window_pump_messages(sr_window *window);

bool window_is_open(sr_window *window);
void window_close(sr_window *window);