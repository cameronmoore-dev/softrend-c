#pragma once

#include "renderer/vertex.h"
#include "data_structures/darr.h"

typedef struct mesh
{
    darr_ *vertices;
    u32 num_vertices;
} mesh_;

mesh_ mesh_load(const char *path);
void mesh_free(mesh_ *mesh);
void compute_tangents(mesh_ *mesh);