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
scene_ create_scene(void *scene_data, void(*init_fn)(scene_funcs_*));
void add_scene(scene_manager_ *manager, scene_ *scene);
void remove_scene(scene_manager_ *manager);
scene_ *current_scene(scene_manager_ *manager);