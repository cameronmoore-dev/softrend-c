#pragma once

#include "typedefs.h"
#include "renderer/vertex.h"

typedef struct
{
    vertex_ *vertices;
    u32 num_vertices;
} mesh_;

mesh_ mesh_load(const char *path);
void mesh_free(mesh_ *mesh);