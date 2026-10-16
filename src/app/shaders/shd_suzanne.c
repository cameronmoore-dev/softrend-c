#include "shd_suzanne.h"

#include "renderer/shader.h"
#include <renderer/_math.h>

#include <math.h>

#define SHADE_FLAT 0

f32 max(f32 a, f32 b)
{
    return a > b ? a : b;
}

vec3f get_interpolated_normals(vertex_ *mesh, vec3f *barycentric);
vec3f get_face_normal(vertex_ *mesh);

vertex_ suzanne_vertex_shader(suzanne_shader_data_ *data, vertex_ *vertex)
{
    vertex_ vtx;

    vec4f m = mat4_mul_vec4(data->model, &vertex->pos);
    vec4f v = mat4_mul_vec4(data->view, &m);
    vec4f p = mat4_mul_vec4(data->projection, &v);

    vtx.pos = p;
#if SHADE_FLAT
    // If doing flat shading, then use the vertex world position
    // as this is needed to find the face normal during the fragment shader
    vtx.normal = vec4_to_vec3(&m);
#else
    vtx.normal = mat4_mul_vec3(data->model, &vertex->normal);
#endif
    vtx.uv = vertex->uv;

    return vtx;
}

bool suzanne_fragment_shader(suzanne_shader_data_ *data, vec3f *point, vertex_* mesh, colour_* colour)
{
    vec3f light_colour = vec3(1.0f);
    vec3f ambient = vec3(0.33f);

#if SHADE_FLAT
    vec3f normal = get_face_normal(mesh);
#else
    vec3f normal = get_interpolated_normals(mesh, point);
#endif

    f32 diff = max(0.0f, dot(&normal, &data->light_dir));
    vec3f diffuse = vec3_mul_flt(&light_colour, diff);

    vec3f halfway_dir = vec3_add(&data->light_dir, &data->light_dir);
    halfway_dir = normalize(&halfway_dir);

    f32 spec = dot(&normal, &halfway_dir);
    spec = max(0.0f, spec);
    spec = powf(spec, 128);

    vec3f specular = packed_to_rgb(sample_texture(mesh, data->specular, point));
    specular = vec3_mul_flt(&specular, spec);

    vec3f ds  = vec3_add(&diffuse, &specular);
    ds = vec3_add(&ambient, &ds);

    vec4f shading = vec3_to_vec4(&ds, 1.0f);
    vec4f albedo = packed_to_rgba(sample_texture(mesh, data->albedo, point));
    vec4f final = vec4_mul(&shading, &albedo);

    *colour = rgba_to_packed(&final);
    return true;
}

vec3f get_interpolated_normals(vertex_ *mesh, vec3f *barycentric)
{
    vec3f n0  = vec3_mul_flt(&mesh[0].normal, barycentric->x);
    vec3f n1  = vec3_mul_flt(&mesh[1].normal, barycentric->y);
    vec3f n2  = vec3_mul_flt(&mesh[2].normal, barycentric->z);
    vec3f n = vec3_add(&n0, &n1);
    n = vec3_add(&n2, &n);
    
    return normalize(&n);
}

vec3f get_face_normal(vertex_ *mesh)
{
    vec3f ba = vec3_sub(&mesh[1].normal, &mesh[0].normal);
    vec3f ca = vec3_sub(&mesh[2].normal, &mesh[0].normal);
    vec3f c = cross(&ba, &ca);

    return normalize(&c);
}