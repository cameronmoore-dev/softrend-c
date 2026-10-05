#include "app/scene_manager.h"

#include <stdlib.h>

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

scene_ create_scene(void *scene_data, void(*init_fn)(scene_funcs_*))
{
    scene_ scn;
    scn.data = scene_data;
    init_fn(&scn.funcs);

    return scn;
}

void add_scene(scene_manager_ *manager, scene_ *scene)
{
    stack_push(manager->scenes, scene);
}

void remove_scene(scene_manager_ *manager)
{
    stack_pop(manager->scenes);
}

scene_ *current_scene(scene_manager_ *manager)
{
    return (scene_ *)*stack_get_top(manager->scenes);
}