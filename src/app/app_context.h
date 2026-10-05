#pragma once

#include "window/window.h"
#include "app/scene_manager.h"

typedef struct
{
    window_ window;
    scene_manager_ scene_manager;
} app_context_;

void app_context_close();