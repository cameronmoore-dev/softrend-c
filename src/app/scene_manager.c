#include "app/scene_manager.h"

#include <stdlib.h>

#include <stdio.h>

scene_manager_ scene_manager_init()
{
    scene_manager_ sm;
    sm.scenes = stack_create(sizeof(scene_));
    return sm;
}

void scene_manager_close(scene_manager_ *manager)
{
    stack_close(manager->scenes);
}

void add_scene(scene_manager_ *manager, scene_ scene)
{
    stack_push(manager->scenes, &scene);
}

void remove_scene(scene_manager_ *manager)
{
    stack_pop(manager->scenes);
}

scene_ *current_scene(scene_manager_ *manager)
{
    return (scene_ *)*stack_get_top(manager->scenes);
}