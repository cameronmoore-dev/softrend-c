#pragma once

#include "app/app_context.h"
#include "app/camera.h"
#include "app/shaders/shd_shadowmap.h"
#include "app/shaders/shd_depth.h"

typedef struct scn_shadowmapping_data
{
    app_context_ *app;
    camera_ camera;
    camera_ light_source;

    mat4 model_matrix;
    mat4 light_space_matrix;
    image_ albedo;
    depth_shader_data_ depth_shader_data;
    shadowmap_shader_data_ shadowmap_shader_data;
    shader_ depth_shader;
    shader_ shadowmap_shader;
    
    mesh_ suzanne_mesh;
    mesh_ floor_mesh;

    rendertarget_ depth_map;
    rendertarget_ default_rt;

    f64 *delta_time;
} scn_shadowmapping_data_;

void scn_shadowmapping_init(scene_funcs_ *funcs);
void scn_shadowmapping_start(scn_shadowmapping_data_ *data);
void scn_shadowmapping_process_events(scn_shadowmapping_data_ *data);
void scn_shadowmapping_update(scn_shadowmapping_data_ *data);
void scn_shadowmapping_render(scn_shadowmapping_data_ *data);
void scn_shadowmapping_shutdown(scn_shadowmapping_data_ *data);