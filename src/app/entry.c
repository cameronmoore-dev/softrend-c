#include <stdio.h>

#include "app/app_context.h"
#include "app/scenes/scn_triangle.h"

int main(void)
{
    printf("Hello World!\n");

    app_context_ app;
    app.window = window_create("SoftRend", 640, 480);
    app.scene_manager = scene_manager_init();

    scn_triangle_data_ scn_triangle_data;
    scn_triangle_data.app = &app;

    scene_ scn_triangle = create_scene(&scn_triangle_data, scn_triangle_init);
    add_scene(&app.scene_manager, &scn_triangle);

    scene_ *current = current_scene(&app.scene_manager);
    current->funcs.start(current->data);
    while (window_is_open(&app.window))
    {
        scene_ *current = current_scene(&app.scene_manager);
        current->funcs.process_events(current->data);
        current->funcs.update(current->data);
        current->funcs.render(current->data);
    }

    app_context_close(&app);

    printf("Goodbye World!\n");
    return 0;
}
