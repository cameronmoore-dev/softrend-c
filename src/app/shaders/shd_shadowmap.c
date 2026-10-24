#include "shd_shadowmap.h"

#include "renderer/shader.h"

f32 shadow_calculation(shadowmap_shader_data_ *data, vec3f *frag_world_pos);

vertex_ shadowmap_vertex_shader(shadowmap_shader_data_ *data, vertex_ *vertex)
{
    vec4f m = mat4_mul_vec4(data->model, &vertex->pos);
    vec4f v = mat4_mul_vec4(data->view, &m);
    vec4f p = mat4_mul_vec4(data->projection, &v);

    vertex_ vtx = *vertex;
    vtx.pos = p;

    // Use vertex normal as world space vertex position
    vtx.normal = vec4_to_vec3(&m);

    return vtx;
}

bool shadowmap_fragment_shader(shadowmap_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour)
{
    vec3f ambient = vec3(0.33f);

    f32 inv_w0 = 1.0f / mesh[0].pos.w;
    f32 inv_w1 = 1.0f / mesh[1].pos.w;
    f32 inv_w2 = 1.0f / mesh[2].pos.w;
    vec3f pcf0 = vec3_div_flt(&mesh[0].normal, mesh[0].pos.w);
    vec3f pcf1 = vec3_div_flt(&mesh[1].normal, mesh[1].pos.w);
    vec3f pcf2 = vec3_div_flt(&mesh[2].normal, mesh[2].pos.w);

    f32 inverse = inv_w0 * point->x + inv_w1 * point->y + inv_w2 * point->z;
    vec3f fwp0 = vec3_mul_flt(&pcf0, point->x);
    vec3f fwp1 = vec3_mul_flt(&pcf1, point->y);
    vec3f fwp2 = vec3_mul_flt(&pcf2, point->z);
    vec3f fwp = vec3_add(&fwp0, &fwp1);
    fwp = vec3_add(&fwp, &fwp2);
    fwp = vec3_div_flt(&fwp, inverse);

    f32 shadow = 1.0f - shadow_calculation(data, &fwp);
    vec3f s = vec3_add_flt(&ambient, shadow);
    vec4f shading = vec3_to_vec4(&s, 1.0f);
    vec4f texture = packed_to_rgba(sample_texture(mesh, data->albedo, point));
    vec4f final = vec4_mul(&shading, &texture);

    *colour = rgba_to_packed(&final);
    return true;
}

f32 shadow_calculation(shadowmap_shader_data_ *data, vec3f *frag_world_pos)
{
    vec4f fwp4 = vec3_to_vec4(frag_world_pos, 1.0f);
    vec4f light_position = mat4_mul_vec4(data->light_space_matrix, &fwp4);
    vec3f projection = 
    {
        .x = light_position.x / light_position.w,
        .y = light_position.y / light_position.w,
        .z = light_position.z / light_position.w,
    };

    projection.x = projection.x * 0.5f + 0.5f;
    projection.y = projection.y * 0.5f + 0.5f;

    if (projection.x < 0.0f || projection.x > 1.0f ||
        projection.y < 0.0f || projection.y > 1.0f)
    {
        return 0.0f;
    }

    s32 sx = (s32)(projection.x * (data->shadow_map_size - 1));
    s32 sy = (s32)(projection.y * (data->shadow_map_size - 1));
    f32 shadow_depth = data->shadow_map[sx + sy * data->shadow_map_size];

    return ((projection.z - 0.03f) > shadow_depth) 
        ? 1.0f 
        : 0.0f;
}