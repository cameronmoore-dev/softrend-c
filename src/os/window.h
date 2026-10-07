#pragma once

#include "typedefs.h"

typedef struct platform_context platform_context;

typedef struct
{
    u32 width;
    u32 height;
    u32 *data;
} backbuffer_;

typedef struct
{
    backbuffer_ backbuffer;
    u32 *frontbuffer;
    platform_context *platform;
    const char *title;
    u32 width;
    u32 height;
    bool isOpen;
} window_;

window_ window_create(const char *title, u32 width, u32 height);
void window_swap_buffers(window_ *window);
void window_cleanup(window_ *window);
void window_pump_messages(window_ *window);

bool window_is_open(window_ *window);
void window_close(window_ *window);