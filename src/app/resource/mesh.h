#pragma once

#include "renderer/vertex.h"
#include "data_structures/darr.h"

typedef struct mesh
{
    darr_ *vertices;
    u32 num_vertices;
} mesh_;