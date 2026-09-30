#include <stdio.h>

#include "window/window.h"

int main(void)
{
    platform_context *platform = window_create_platform_context();
    sr_window *window = window_create(platform, "SoftRend", 640, 480);
    printf("Title: %s\n", window->name);
    printf("%d\n", window->width);
    printf("%d\n", window->height);

    while (window_is_open(window))
    {
        window_pump_messages(platform);
    }

    window_cleanup(platform, window);
    return 0;
}
