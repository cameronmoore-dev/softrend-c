#pragma once

#include "app/scene_manager.h"

typedef struct scn_triangle
{
    scene_ scene;
} scn_triangle;

scn_triangle scn_triangle_init();
void scn_triangle_start();
void scn_triangle_process_events();
void scn_triangle_update();
void scn_triangle_render(sr_window *window);
void scn_triangle_shutdown();