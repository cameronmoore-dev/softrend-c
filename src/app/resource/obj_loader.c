#include "mesh_loader.h"

#include "renderer/_math.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

mesh_ mesh_load(const char *path)
{
    FILE *fp = fopen(path, "r");
    assert(fp && "Failed to load mesh from path");
 
    darr_ *vertices = darr_create(sizeof(vertex_), 3500);
    darr_ *positions = darr_create(sizeof(vec3f), 512);
    darr_ *normals = darr_create(sizeof(vec3f), 512);
    darr_ *texture_coords = darr_create(sizeof(vec2f), 600);

    u32 vtx_index = 0;
    u32 pos_index = 0;
    u32 nrm_index = 0;
    u32 tex_index = 0;
    char newline_buf[1024];
    char *newline = fgets(newline_buf, sizeof(newline_buf), fp);
    while (newline != NULL)
    {
        if (strstr(newline, "v ") != NULL)
        {
            vec3f p;
            sscanf(newline, "%*c %f %f %f", &p.x, &p.y, &p.z);
            darr_replace(positions, pos_index++, &p);
        }

        else if (strstr(newline, "vn ") != NULL)
        {
            vec3f n;
            sscanf(newline, "%*c%*c %f %f %f", &n.x, &n.y, &n.z);
            darr_replace(normals, nrm_index++, &n);
        }

        else if (strstr(newline, "vt ") != NULL)
        {
            vec2f tc;
            sscanf(newline, "%*c%*c %f %f", &tc.x, &tc.y);
            darr_replace(texture_coords, tex_index++, &tc);
        }

        else if (strstr(newline, "f ") != NULL)
        {
            u32 p0, p1, p2;
            u32 t0, t1, t2;
            u32 n0, n1, n2;
            sscanf(newline, 
                "%*c " // Prefix character(s)
                "%d%*c%d%*c%d "    // First face
                "%d%*c%d%*c%d "    // Second face
                "%d%*c%d%*c%d ",   // Third face
                &p0, &t0, &n0, 
                &p1, &t1, &n1, 
                &p2, &t2, &n2
            );

            vertex_ v;
            v.pos = vec3_to_vec4((vec3f *)darr_at(positions, p0 - 1), 1.0f),
            v.normal = *(vec3f *)darr_at(normals, n0 - 1),
            v.uv = *(vec2f *)darr_at(texture_coords, t0 - 1);
            darr_replace(vertices, vtx_index + 0, &v);

            v.pos = vec3_to_vec4((vec3f *)darr_at(positions, p1 - 1), 1.0f);
            v.normal = *(vec3f *)darr_at(normals, n1 - 1);
            v.uv = *(vec2f *)darr_at(texture_coords, t1 - 1);
            darr_replace(vertices, vtx_index + 1, &v);

            v.pos = vec3_to_vec4((vec3f *)darr_at(positions, p2 - 1), 1.0f);
            v.normal = *(vec3f *)darr_at(normals, n2 - 1);
            v.uv = *(vec2f *)darr_at(texture_coords, t2 - 1);
            darr_replace(vertices, vtx_index + 2, &v);

            vtx_index += 3;
        }

        newline = fgets(newline_buf, sizeof(newline_buf), fp);
    }
    fclose(fp);

    mesh_ mesh;
    mesh.vertices = darr_create(sizeof(vertex_), 3500);
    mesh.num_vertices = vtx_index;
    darr_copy(mesh.vertices, vertices, false);
    darr_free_excess(mesh.vertices);

    darr_close(positions);
    darr_close(normals);
    darr_close(texture_coords);
    
    return mesh;
}

void mesh_free(mesh_ *mesh)
{
    darr_close(mesh->vertices);
}