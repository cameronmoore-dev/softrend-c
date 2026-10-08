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
 
    // TODO: This is just for loading the cube model!
    vertex_ vertices[36];
    vec3f positions[8];
    vec2f texture_coords[14];

    u32 index = 0;
    u32 pos_index = 0;
    u32 tex_index = 0;
    char filebuf[256];
    char *newline = fgets(filebuf, sizeof(filebuf), fp);
    while (newline != NULL)
    {
        if (strstr(newline, "v ") != NULL)
        {
            vec3f p;
            sscanf(newline, "%*c %f %f %f", &p.x, &p.y, &p.z);
            positions[pos_index++] = p;
        }

        else if (strstr(newline, "vt ") != NULL)
        {
            vec2f tc;
            sscanf(newline, "%*c%*c %f %f", &tc.x, &tc.y);
            texture_coords[tex_index++] = tc;
        }

        // TODO: handle vertex normals
        // TODO: handle indices

        else if (strstr(newline, "f ") != NULL)
        {
            u32 p0, p1, p2;
            u32 t0, t1, t2;
            sscanf(newline, 
                "%*c " // Prefix character(s)
                "%d%*c%d%*c%*d "    // First face
                "%d%*c%d%*c%*d "    // Second face
                "%d%*c%d%*c%*d ",   // Third face
                &p0, &t0, &p1, &t1, &p2, &t2
            );

            vertices[index+0].pos = vec3_to_vec4(&positions[p0 - 1], 1.0f);
            vertices[index+0].uv = texture_coords[t0 - 1];

            vertices[index+1].pos = vec3_to_vec4(&positions[p1 - 1], 1.0f);
            vertices[index+1].uv = texture_coords[t1 - 1];

            vertices[index+2].pos = vec3_to_vec4(&positions[p2 - 1], 1.0f);
            vertices[index+2].uv = texture_coords[t2 - 1];

            index += 3;
        }

        newline = fgets(filebuf, sizeof(filebuf), fp);
    }

    fclose(fp);

    mesh_ mesh;
    mesh.vertices = (vertex_ *)malloc(sizeof(vertices));
    mesh.num_vertices = index;
    memcpy(mesh.vertices, vertices, sizeof(vertices));
    
    return mesh;
}

void mesh_free(mesh_ *mesh)
{
    free(mesh->vertices);
}