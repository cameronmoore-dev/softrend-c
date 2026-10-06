#pragma once

#include "renderer/vertex.h"
#include "renderer/shader.h"
#include "window/window.h"
#include "colour.h"

typedef struct
{
    u32 x, y;
    u32 w, h;
} viewport_;

typedef struct
{
    viewport_ viewport;
    backbuffer_ *drawbuffer;
    colour_ clear_colour;
} renderer_;

renderer_ renderer_init(backbuffer_ *drawbuffer, u32 clear_colour);
void renderer_clear(renderer_ *renderer);
void renderer_draw(renderer_ *renderer, vertex_* mesh, shader_ *shader);