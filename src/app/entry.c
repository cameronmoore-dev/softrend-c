#include <stdio.h>

#include "app/app_context.h"
#include "app/scenes/scn_triangle.h"

int main(void)
{
    printf("Hello World!\n");

    sr_window *window = window_create("SoftRend", 640, 480);
    scene_manager_ scene_manager = scene_manager_init();

    scn_triangle scn_triangle = scn_triangle_init();
    add_scene(&scene_manager, scn_triangle.scene);
    
    current_scene(&scene_manager)->start();
    while (window_is_open(window))
    {
        scene_ *current = current_scene(&scene_manager);
        current->process_events();
        current->update();
        current->render(window);
    }

    window_cleanup(window);
    scene_manager_close(&scene_manager);

    printf("Goodbye World!\n");
    return 0;
}
