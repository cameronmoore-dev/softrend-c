#include "shd_grass.h"

#include "renderer/shader.h"

vertex_ grass_vertex_shader(grass_shader_data_ *data, vertex_ *vertex)
{
    vec4f m   = mat4_mul_vec4(data->model, &vertex->pos);
    vec4f mv  = mat4_mul_vec4(data->view, &m);
    vec4f mvp = mat4_mul_vec4(data->projection, &mv);
    
    vertex_ out = *vertex;
    out.pos = mvp;

    return out;
}

bool grass_fragment_shader(grass_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour)
{
    *colour = sample_texture(mesh, data->albedo, point);
    if (colour->comps[3] == 0)
    {
        return false;
    }
    
    return true;
}