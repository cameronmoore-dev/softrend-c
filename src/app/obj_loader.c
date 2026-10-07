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

    u32 index = 0;
    u32 pos_index = 0;
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

        // TODO: handle vertex normals and texture coords
        // TODO: handle faces and indices

        else if (strstr(newline, "f ") != NULL)
        {
            u32 p0, p1, p2;
            sscanf(newline, 
                "%*c %d%*c%*d%*c%*d "
                "%d%*c%*d%*c%*d "
                "%d%*c%*d%*c%*d ", 
                &p0, &p1, &p2
            );

            vertices[index++].pos = vec3_to_vec4(&positions[p0 - 1], 1.0f);
            vertices[index++].pos = vec3_to_vec4(&positions[p1 - 1], 1.0f);
            vertices[index++].pos = vec3_to_vec4(&positions[p2 - 1], 1.0f);
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