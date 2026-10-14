#include "app/scene_manager.h"

#include <stdlib.h>
#include <string.h>

scene_manager_ scene_manager_init()
{
    scene_manager_ sm = {};
    sm.scenes = stack_create(sizeof(scene_));
    return sm;
}

void scene_manager_close(scene_manager_ *manager)
{
    stack_close(manager->scenes);
}

void scene_add(scene_manager_ *manager, scene_ *scene, u32 sizeof_data)
{
    stack_push(&manager->scenes, (void *)scene);

    scene_ *new_scene = current_scene(manager);
    new_scene->data = malloc(sizeof_data);
    memcpy(&new_scene->funcs, &scene->funcs, sizeof(scene_funcs_));
    memcpy(new_scene->data, scene->data, sizeof_data);

    new_scene->funcs.start(new_scene->data);
}

void scene_remove(scene_manager_ *manager)
{
    free(current_scene(manager)->data);

    stack_pop(manager->scenes);
}

scene_ *current_scene(scene_manager_ *manager)
{
    return (scene_ *)*stack_get_top(manager->scenes);
}