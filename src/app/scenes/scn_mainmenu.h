#pragma once

#include "app/app_context.h"

typedef struct
{
    app_context_ *app;
    f64 *delta_time;
} scn_mainmenu_data_;

void scn_mainmenu_init(scene_funcs_ *funcs);
void scn_mainmenu_start(scn_mainmenu_data_ *data);
void scn_mainmenu_process_events(scn_mainmenu_data_ *data);
void scn_mainmenu_update(scn_mainmenu_data_ *data);
void scn_mainmenu_render(scn_mainmenu_data_ *data);
void scn_mainmenu_shutdown(scn_mainmenu_data_ *data);