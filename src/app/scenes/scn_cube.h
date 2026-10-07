#pragma once

#include "app/app_context.h"

typedef struct
{
    app_context_ *app;
} scn_cube_data_;

void scn_cube_init(scene_funcs_ *funcs);
void scn_cube_start(scn_cube_data_ *data);
void scn_cube_process_events(scn_cube_data_ *data);
void scn_cube_update(scn_cube_data_ *data);
void scn_cube_render(scn_cube_data_ *data);
void scn_cube_shutdown(scn_cube_data_ *data);