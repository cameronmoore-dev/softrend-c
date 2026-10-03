#include <stdio.h>

#include "window/window.h"

int main(void)
{
    printf("Hello World!\n");

    platform_context *platform = window_create_platform_context();
    sr_window *window = window_create(platform, "SoftRend", 640, 480);
    printf("Title: %s\n", window->title);
    printf("%d\n", window->width);
    printf("%d\n", window->height);

    while (window_is_open(window))
    {
        window_swap_buffers(platform, window);
    }

    window_cleanup(platform, window);
    printf("Goodbye World!\n");
    
    return 0;
}
