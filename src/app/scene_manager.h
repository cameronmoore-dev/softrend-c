#pragma once

#include "data_structures/stack.h"

typedef struct
{
    void(*start)(void*);
    void(*process_events)(void*);
    void(*update)(void*);
    void(*render)(void*);
    void(*shutdown)(void*);
} scene_funcs_;

typedef struct
{
    scene_funcs_ funcs;
    void *data;
} scene_;

typedef struct
{
    stack_ *scenes;
} scene_manager_;

scene_manager_ scene_manager_init();
void scene_manager_close(scene_manager_ *manager);
void scene_add(scene_manager_ *manager, scene_ *scene, u32 sizeof_data);
void scene_remove(scene_manager_ *manager);
scene_ *current_scene(scene_manager_ *manager);