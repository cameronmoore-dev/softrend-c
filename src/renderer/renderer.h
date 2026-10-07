#pragma once

#include "renderer/shader.h"
#include "renderer/colour.h"
#include "os/window.h"
#include "app/mesh_loader.h"

typedef struct
{
    u32 x, y;
    u32 w, h;
} viewport_;

typedef struct
{
    vertex_ vertices[3];
} face_;

typedef struct
{
    viewport_ viewport;
    backbuffer_ *drawbuffer;
    f32 *depthbuffer;
    colour_ clear_colour;
} renderer_;

renderer_ renderer_init(backbuffer_ *drawbuffer, u32 clear_colour);
void renderer_clear(renderer_ *renderer);
void renderer_draw(renderer_ *renderer, mesh_* mesh, shader_ *shader);