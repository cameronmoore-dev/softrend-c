#pragma once

#include "window/window.h"

#include "utils/stack.h"

typedef struct scene
{
    void(*start)(void);
    void(*process_events)(void);
    void(*update)(void);
    void(*render)(sr_window*);
    void(*shutdown)(void);
} scene_;

typedef struct scene_manager
{
    stack_ *scenes;
} scene_manager_;

scene_manager_ scene_manager_init();
void scene_manager_close(scene_manager_ *manager);
void add_scene(scene_manager_ *manager, scene_ scene);
void remove_scene(scene_manager_ *manager);
scene_ *current_scene(scene_manager_ *manager);