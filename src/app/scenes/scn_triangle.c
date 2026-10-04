#include "app/scenes/scn_triangle.h"

#include "app/app_context.h"

#include <stdio.h>

scn_triangle scn_triangle_init(sr_window *window)
{
    scn_triangle scn;
    scn.scene.start = scn_triangle_start;
    scn.scene.process_events = scn_triangle_process_events;
    scn.scene.update = scn_triangle_update;
    scn.scene.render = scn_triangle_render;
    scn.scene.shutdown = scn_triangle_shutdown;

    return scn;
}

void scn_triangle_start()
{
}

void scn_triangle_process_events()
{
}

void scn_triangle_update()
{
}

void scn_triangle_render(sr_window *window)
{
    window_swap_buffers(window);
    window_pump_messages(window);
}

void scn_triangle_shutdown()
{
    printf("Shutting Down\n");
}