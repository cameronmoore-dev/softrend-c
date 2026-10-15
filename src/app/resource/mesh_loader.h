#pragma once

#include "app/resource/mesh.h"

mesh_ mesh_load(const char *path);
void mesh_free(mesh_ *mesh);