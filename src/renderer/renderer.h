#pragma once

#include "renderer/shader.h"
#include "renderer/colour.h"
#include "os/window.h"
#include "app/resource/mesh.h"

#define RENDERPASS_COLOUR   0
#define RENDERPASS_DEPTH    1

typedef struct
{
    u32 x, y;
    u32 w, h;
} viewport_;

typedef struct
{
    vertex_ vertices[3];
} face_;

typedef struct rendertarget
{
    u32 x, y;
    u32 w, h;
    u32 *colourbuffer;
    f32 *depthbuffer;
} rendertarget_;

typedef struct
{
    rendertarget_ rendertarget;
    backbuffer_ *drawbuffer;
    viewport_ viewport;
    vec2i *precomputed_downscale;
    colour_ clear_colour;
    u32 downscale_factor;
    u32 renderpass;
} renderer_;

renderer_ renderer_init(backbuffer_ *drawbuffer, u32 clear_colour);
void renderer_close(renderer_ *renderer);
void renderer_clear(renderer_ *renderer);
void renderer_draw(renderer_ *renderer, mesh_* mesh, shader_ *shader);
void renderer_end_draw(renderer_ *renderer);

rendertarget_ renderer_create_rendertarget(renderer_ *renderer, u32 x, u32 y, u32 w, u32 h);
void renderer_set_renderpass(renderer_ *renderer, u32 pass_type);
void renderer_set_rendertarget(renderer_ *renderer, rendertarget_ target);
void renderer_set_viewport(renderer_ *renderer, u32 x, u32 y, u32 w, u32 h);
void renderer_set_downscale_factor(renderer_ *renderer, u32 factor);