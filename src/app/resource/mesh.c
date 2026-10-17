#include "mesh.h"
#include "data_structures/darr.h"

#include <stdlib.h>

void mesh_free(mesh_ *mesh)
{
    darr_close(mesh->vertices);
}

void compute_tangents(mesh_ *mesh)
{
    for (u32 i = 0; i < mesh->num_vertices; i += 3)
    {
        vertex_ *v0 = (vertex_*)darr_at(mesh->vertices, i + 0);
        vertex_ *v1 = (vertex_*)darr_at(mesh->vertices, i + 1);
        vertex_ *v2 = (vertex_*)darr_at(mesh->vertices, i + 2);

        vec3f p0 = vec4_to_vec3(&v0->pos);
        vec3f p1 = vec4_to_vec3(&v1->pos);
        vec3f p2 = vec4_to_vec3(&v2->pos);

        vec3f edge1 = vec3_sub(&p1, &p0);
        vec3f edge2 = vec3_sub(&p2, &p0);
        vec2f uv_delta1 = vec2_sub(v1->uv, v0->uv);
        vec2f uv_delta2 = vec2_sub(v2->uv, v0->uv);

        f32 r = 1.0f / (uv_delta1.x * uv_delta2.y - uv_delta2.x * uv_delta1.y);

        vec3f tangent = 
        {
            .x = (uv_delta2.y * edge1.x - uv_delta1.y * edge2.x) * r,
            .y = (uv_delta2.y * edge1.y - uv_delta1.y * edge2.y) * r,
            .z = (uv_delta2.y * edge1.z - uv_delta1.y * edge2.z) * r,
        };

        vec3f bitangent = 
        {
            .x = (-uv_delta2.x * edge1.x + uv_delta1.x * edge2.x) * r,
            .y = (-uv_delta2.x * edge1.y + uv_delta1.x * edge2.y) * r,
            .z = (-uv_delta2.x * edge1.z + uv_delta1.x * edge2.z) * r,
        };

        v0->tangent = tangent;
        v1->tangent = tangent;
        v2->tangent = tangent;
        v0->bitangent = bitangent;
        v1->bitangent = bitangent;
        v2->bitangent = bitangent;
    }
}