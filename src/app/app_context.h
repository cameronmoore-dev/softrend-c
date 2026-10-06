#pragma once

#include "window/window.h"
#include "renderer/renderer.h"
#include "app/scene_manager.h"

typedef struct
{
    window_ window;
    renderer_ renderer;
    scene_manager_ scene_manager;
} app_context_;

void app_context_close();