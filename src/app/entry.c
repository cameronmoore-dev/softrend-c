#include <stdio.h>

#include "os/_time.h"
#include "app/app_context.h"
#include "app/scenes/scn_mainmenu.h"

int main(void)
{
    printf("Hello World!\n");

    app_context_ app;
    app.window = window_create("1.Triangle  2.Cube", 640, 480);
    app.renderer = renderer_init(&app.window->backbuffer, 0x111111);
    app.scene_manager = scene_manager_init();
    f64 delta_time;

    scn_mainmenu_data_ mainmenu_data = {};
    mainmenu_data.app = &app;
    mainmenu_data.delta_time = &delta_time;
    scene_ mainmenu;
    mainmenu.data = &mainmenu_data;
    scn_mainmenu_init(&mainmenu.funcs);
    scene_add(&app.scene_manager, &mainmenu, sizeof(scn_mainmenu_data_));

    timestep_ ts = {};
    get_time(&ts.previous_time);
    app.window->frame_start = get_time();
    while (window_is_open(app.window))
    {
        update_timestep(&ts, &delta_time);

        window_pump_messages(app.window);
        current_scene(&app.scene_manager)->funcs.process_events(current_scene(&app.scene_manager)->data);
        current_scene(&app.scene_manager)->funcs.update(current_scene(&app.scene_manager)->data);
        current_scene(&app.scene_manager)->funcs.render(current_scene(&app.scene_manager)->data);

        window_sleep(app.window);
        
        // printf("FPS: %.1f\n", (1.0f/delta_time));
    }

    app_context_close(&app);

    printf("Goodbye World!\n");
    return 0;
}