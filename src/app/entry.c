#include <stdio.h>

#include <time.h>

#include "os/_time.h"
#include "app/app_context.h"
#include "app/scenes/scn_triangle.h"
#include "app/scenes/scn_cube.h"

int main(void)
{
    printf("Hello World!\n");

    app_context_ app;
    app.window = window_create("SoftRend-C", 640, 480);

    app.renderer = renderer_init(&app.window.backbuffer, 0x111111);
    app.scene_manager = scene_manager_init();

    // scn_triangle_data_ scn_triangle_data;
    // scn_triangle_data.app = &app;
    // scene_ scn_triangle = create_scene(&scn_triangle_data, scn_triangle_init);
    // add_scene(&app.scene_manager, &scn_triangle);

    f64 delta_time;

    scn_cube_data_ scn_cube_data;
    scn_cube_data.app = &app;
    scn_cube_data.delta_time = &delta_time;
    scene_ scn_cube = create_scene((void *)&scn_cube_data, scn_cube_init);
    add_scene(&app.scene_manager, &scn_cube);

    scene_ *current = current_scene(&app.scene_manager);
    current->funcs.start(current->data);

    timestep_ ts = {};
    get_time(&ts.previous_time);
    while (window_is_open(&app.window))
    {
        update_timestep(&ts, &delta_time);

        scene_ *current = current_scene(&app.scene_manager);
        current->funcs.process_events(current->data);
        current->funcs.update(current->data);
        current->funcs.render(current->data);

        window_sleep(&app.window);
        
        printf("FPS: %f\n", (1.0f/delta_time));
    }

    app_context_close(&app);

    printf("Goodbye World!\n");
    return 0;
}
