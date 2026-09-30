#pragma once

#include "defines.h"

typedef struct platform_context platform_context;
typedef struct sr_window
{
    const char *title;
    u32 width;
    u32 height;
    bool isOpen;
} sr_window;

platform_context *window_create_platform_context();
sr_window *window_create(platform_context *context, const char *title, u32 width, u32 height);
void window_cleanup(platform_context *context, sr_window *window);
void window_pump_messages(platform_context *context);

bool window_is_open(sr_window *window);
void window_close(sr_window *window);