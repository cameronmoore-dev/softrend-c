#include "renderer.h"
#include "_math.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <stdio.h>

#define MAX_OUTPUT_VERTICES 32

#define LEFT_PLANE      0
#define RIGHT_PLANE     1
#define BOTTOM_PLANE    2
#define TOP_PLANE       3
#define NEAR_PLANE      4
#define FAR_PLANE       5
#define NUM_PLANES      6

bool is_vertex_inside_frustum(vertex_ *vertex, u32 plane);
vertex_ interpolate_vertex(vertex_ *a, vertex_ *b, u32 plane);
u32 clip_triangle_against_plane(vertex_ *input, vertex_ *output, u32 plane, u32 num_vertices);
void rasterize(renderer_ *renderer, face_ *face, shader_ *shader);
void transform_to_screen_space(renderer_ *renderer, face_ *face);
vec3f barycentric(vec2i *p, face_ *face);
f32 winding_order(face_ *face);
rect_ polygon_screen_bounding_box(face_ *face);

renderer_ renderer_init(backbuffer_ *drawbuffer, u32 clear_colour)
{
    renderer_ r = {};
    r.drawbuffer = drawbuffer;
    r.viewport = (viewport_){ 0, 0, drawbuffer->width, drawbuffer->height };
    r.clear_colour.packed = clear_colour;
    r.rendertarget = (rendertarget_)
   {
        .colourbuffer = (u32 *)malloc(drawbuffer->width * drawbuffer->height * sizeof(u32)),
        .depthbuffer  = (f32 *)malloc(drawbuffer->width * drawbuffer->height * sizeof(f32)),
        .w = drawbuffer->width,
        .h = drawbuffer->height
    };
    r.precomputed_downscale = NULL;
    r.downscale_factor = 1;

    return r;
}

void renderer_close(renderer_ *renderer)
{
    free(renderer->precomputed_downscale);
    free(renderer->rendertarget.depthbuffer);
    free(renderer->rendertarget.colourbuffer);
}

void renderer_clear(renderer_ *renderer)
{
    if (!renderer->rendertarget.colourbuffer)
    {
        return;
    }

    for (u32 i = 0; i < renderer->rendertarget.w * renderer->rendertarget.h; i++)
    {
        renderer->rendertarget.colourbuffer[i] = renderer->clear_colour.packed;
        renderer->rendertarget.depthbuffer[i]  = INFINITY;
    }
}

bool is_vertex_inside_frustum(vertex_ *vertex, u32 plane)
{
    switch (plane)
    {
        case LEFT_PLANE:    return vertex->pos.x >= -vertex->pos.w;
        case RIGHT_PLANE:   return vertex->pos.x <=  vertex->pos.w;
        case BOTTOM_PLANE:  return vertex->pos.y >= -vertex->pos.w;
        case TOP_PLANE:     return vertex->pos.y <=  vertex->pos.w;
        case NEAR_PLANE:    return vertex->pos.z >= -vertex->pos.w;
        case FAR_PLANE:     return vertex->pos.z <=  vertex->pos.w;
        default:            return false;
    }
}

vertex_ interpolate_vertex(vertex_ *a, vertex_ *b, u32 plane)
{
    f32 t = 1.0f;
    switch (plane)
    {
        case LEFT_PLANE:    t = (a->pos.x + a->pos.w) / ((a->pos.x + a->pos.w) - (b->pos.x + b->pos.w)); break;
        case RIGHT_PLANE:   t = (a->pos.x - a->pos.w) / ((a->pos.x - a->pos.w) - (b->pos.x - b->pos.w)); break;
        case BOTTOM_PLANE:  t = (a->pos.y + a->pos.w) / ((a->pos.y + a->pos.w) - (b->pos.y + b->pos.w)); break;
        case TOP_PLANE:     t = (a->pos.y - a->pos.w) / ((a->pos.y - a->pos.w) - (b->pos.y - b->pos.w)); break;
        case NEAR_PLANE:    t = (a->pos.z + a->pos.w) / ((a->pos.z + a->pos.w) - (b->pos.z + b->pos.w)); break;
        case FAR_PLANE:     t = (a->pos.z - a->pos.w) / ((a->pos.z - a->pos.w) - (b->pos.z - b->pos.w)); break;
        default:            break;
    }

    vertex_ v = 
    {
        .pos.x = a->pos.x + (b->pos.x - a->pos.x) * t,
        .pos.y = a->pos.y + (b->pos.y - a->pos.y) * t,
        .pos.z = a->pos.z + (b->pos.z - a->pos.z) * t,
        .pos.w = a->pos.w + (b->pos.w - a->pos.w) * t,
        .normal.x = a->normal.x + (b->normal.x - a->normal.x) * t,
        .normal.y = a->normal.y + (b->normal.y - a->normal.y) * t,
        .normal.z = a->normal.z + (b->normal.z - a->normal.z) * t,
        .tangent.x = a->tangent.x + (b->tangent.x - a->tangent.x) * t,
        .tangent.y = a->tangent.y + (b->tangent.y - a->tangent.y) * t,
        .tangent.z = a->tangent.z + (b->tangent.z - a->tangent.z) * t,
        .bitangent.x = a->bitangent.x + (b->bitangent.x - a->bitangent.x) * t,
        .bitangent.y = a->bitangent.y + (b->bitangent.y - a->bitangent.y) * t,
        .bitangent.z = a->bitangent.z + (b->bitangent.z - a->bitangent.z) * t,
        .uv.x = a->uv.x + (b->uv.x - a->uv.x) * t,
        .uv.y = a->uv.y + (b->uv.y - a->uv.y) * t
    };

    return v;
}

u32 clip_triangle_against_plane(vertex_ *input, vertex_ *output, u32 plane, u32 num_vertices)
{
    u32 accum = 0;
    for (u32 v = 0; v < num_vertices; v++)
    {
        vertex_ *current = &input[v];
        vertex_ *next = &input[(v + 1) % num_vertices];

        bool current_inside = is_vertex_inside_frustum(current, plane);
        bool next_inside = is_vertex_inside_frustum(next, plane);

        if (current_inside && next_inside)
        {
            output[accum++] = *next;
        }

        else if (current_inside && !next_inside)
        {
            output[accum++] = interpolate_vertex(current, next, plane);
        }

        else if (!current_inside && next_inside)
        {
            output[accum++] = interpolate_vertex(current, next, plane);
            output[accum++] = *next;
        }
    }

    return accum;
}

void renderer_draw(renderer_ *renderer, mesh_ *mesh, shader_ *shader)
{
    for (u32 i = 0; i < mesh->num_vertices; i += 3)
    {
        vertex_ input[MAX_OUTPUT_VERTICES] = 
        { 
            shader->vertex(shader->data, darr_at(mesh->vertices, i + 0)),
            shader->vertex(shader->data, darr_at(mesh->vertices, i + 1)),
            shader->vertex(shader->data, darr_at(mesh->vertices, i + 2)),
        };

        u32 num_vertices = 3;
        for (u32 p = 0; p < NUM_PLANES; p++)
        {
            vertex_ output[MAX_OUTPUT_VERTICES];
            num_vertices = clip_triangle_against_plane(input, output, p, num_vertices);
            if (num_vertices == 0)
            {
                break;
            }

            memcpy(input, output, MAX_OUTPUT_VERTICES * sizeof(vertex_));
        }

        for (u32 j = 1; j + 1 < num_vertices; j++)
        {
            face_ new_face = 
            { 
                .vertices[0] = input[0], 
                .vertices[1] = input[j], 
                .vertices[2] = input[j+1] 
            };
            
            transform_to_screen_space(renderer, &new_face);
            if (winding_order(&new_face) <= 0.0f)
            {
                continue;
            }
        
            rasterize(renderer, &new_face, shader);
        }
    }
}

void rasterize(renderer_ *renderer, face_ *face, shader_ *shader)
{
    rect_ bounding_box = polygon_screen_bounding_box(face);
    vec2i p;
    for (p.y = bounding_box.y; p.y <= bounding_box.h; p.y++)
    {
        for (p.x = bounding_box.x; p.x <= bounding_box.w; p.x++)
        {
            // Discard if the point is outside of the viewport's bounds
            if (p.x < renderer->viewport.x || p.x > renderer->viewport.x + (renderer->viewport.w - 1) ||
                p.y < renderer->viewport.y || p.y > renderer->viewport.y + (renderer->viewport.h - 1))
            {
                continue;
            }

            vec3f point = barycentric(&p, face);
            bool point_in_polygon = (point.x >= 0.0f && point.y >= 0.0f && point.z >= 0.0f);
            if (point_in_polygon)
            {
                // u32 flipped_y = renderer->rendertarget.colourbuffer->height - 1.0f - p.y;
                u32 buffer_index = p.x + p.y * renderer->rendertarget.w;
                f32 z = face->vertices[0].pos.z * point.x + 
                        face->vertices[1].pos.z * point.y + 
                        face->vertices[2].pos.z * point.z;

                if (z < renderer->rendertarget.depthbuffer[buffer_index])
                {
                    renderer->rendertarget.depthbuffer[buffer_index] = z;
                    colour_ frag_colour;
                    if (shader->fragment(shader->data, &point, face->vertices, &frag_colour))
                    {
                        vec4f c = packed_to_rgba(frag_colour);
                        vec4f src = 
                        { 
                            .x = c.x * c.w,
                            .y = c.y * c.w,
                            .z = c.z * c.w,
                            .w = c.w
                        };
                        vec4f dest = packed_to_rgba(colour(renderer->rendertarget.colourbuffer[buffer_index]));
                        dest = vec4_mul_flt(&dest, 1.0f - src.w);
                        vec4f blended = vec4_add(&src, &dest);

                        renderer->rendertarget.colourbuffer[buffer_index] = rgba_to_packed(&blended).packed;
                    }
                }
            }
        }
    }
}

void renderer_end_draw(renderer_ *renderer)
{
    u32 drawbuffer_size = renderer->drawbuffer->width * renderer->drawbuffer->height;
    bool buffers_equal = drawbuffer_size == 
                         renderer->rendertarget.w * renderer->rendertarget.h;

    if (buffers_equal)
    {
        memcpy(renderer->drawbuffer->data, renderer->rendertarget.colourbuffer, drawbuffer_size * sizeof(u32));
    }
    else
    {
        for (u32 i = 0; i < drawbuffer_size; i++)
        {
            vec2i coord = renderer->precomputed_downscale[i];
            renderer->drawbuffer->data[i] = renderer->rendertarget.colourbuffer[coord.x + coord.y * renderer->rendertarget.w];
        }
    }
}

void transform_to_screen_space(renderer_ *renderer, face_ *face)
{
    for (u32 i = 0; i < 3; i++)
    {
        vec4f *vpos = &face->vertices[i].pos;

        // Perspective Division
        vpos->x /= vpos->w;
        vpos->y /= vpos->w;
        vpos->z /= vpos->w;

        // To Screen Space
        vpos->x = (vpos->x + 1.0f) * (renderer->viewport.w * 0.5f) + (renderer->viewport.x - 0.1f);
        vpos->y = (vpos->y + 1.0f) * (renderer->viewport.h * 0.5f) + (renderer->viewport.y - 0.1f);
    }
}

vec3f barycentric(vec2i *p, face_ *face)
{
    vec4f *a = &face->vertices[0].pos;
    vec4f *b = &face->vertices[1].pos;
    vec4f *c = &face->vertices[2].pos;
    f32 denom = (b->y - c->y) * (a->x - c->x) + (c->x - b->x) * (a->y - c->y);
    f32 wx = ((b->y - c->y) * (p->x - c->x) + (c->x - b->x) * (p->y - c->y)) / denom;
    f32 wy = ((c->y - a->y) * (p->x - c->x) + (a->x - c->x) * (p->y - c->y)) / denom;
    f32 wz = 1.0f - wx - wy;

    return (vec3f){ wx, wy, wz };
}

f32 winding_order(face_ *face)
{
    vec4f *a = &face->vertices[0].pos;
    vec4f *b = &face->vertices[1].pos;
    vec4f *c = &face->vertices[2].pos;

    return (b->x - a->x) * (c->y - a->y) - (b->y - a->y) * (c->x - a->x);
}

rect_ polygon_screen_bounding_box(face_ *face)
{
    vec4f *a = &face->vertices[0].pos;
    vec4f *b = &face->vertices[1].pos;
    vec4f *c = &face->vertices[2].pos;

    f32 left_most   = fminf(a->x, fminf(b->x, c->x));
    f32 bottom_most = fminf(a->y, fminf(b->y, c->y));
    f32 right_most  = fmaxf(a->x, fmaxf(b->x, c->x));
    f32 top_most    = fmaxf(a->y, fmaxf(b->y, c->y));

    rect_ bb = 
    {
        .x = left_most,
        .y = bottom_most,
        .w = right_most,
        .h = top_most
    };
    return bb;
}

void renderer_set_rendertarget(renderer_ *renderer, rendertarget_ target)
{
    renderer->rendertarget.colourbuffer = realloc(renderer->rendertarget.colourbuffer, target.w * target.h * sizeof(u32));
    renderer->rendertarget.depthbuffer  = realloc(renderer->rendertarget.depthbuffer,  target.w * target.h * sizeof(f32));
    renderer->rendertarget.w = target.w;
    renderer->rendertarget.h = target.h;
    // void *tmp = realloc(renderer->rendertarget.colourbuffer, target.w * target.h);
    // if (tmp)
    // {
    //     renderer->rendertarget.colourbuffer = (u32 *)tmp;
    //     tmp = realloc(renderer->rendertarget.depthbuffer, target.w * target.h);
    //     if (tmp)
    //     {
    //         renderer->rendertarget.depthbuffer = (f32 *)tmp;
    //     }
    // }
    renderer_set_viewport(renderer, target.x, target.y, target.w, target.h);
}

void renderer_set_viewport(renderer_ *renderer, u32 x, u32 y, u32 w, u32 h)
{
    renderer->viewport.x = x;
    renderer->viewport.y = y;
    renderer->viewport.w = w;
    renderer->viewport.h = h;
}

void renderer_set_downscale_factor(renderer_ *renderer, u32 factor)
{
    if (renderer->downscale_factor == 1 && factor > 1)
    {
        renderer->downscale_factor = factor;
        u32 buf_size = renderer->drawbuffer->width * renderer->drawbuffer->height;
        renderer->precomputed_downscale = (vec2i *)malloc(buf_size * sizeof(vec2i));

        u32 target_width  = renderer->drawbuffer->width / factor;
        u32 target_height = renderer->drawbuffer->height / factor;
        u32 computed_index = 0;
        for (u32 y = 0; y < renderer->drawbuffer->height; y++)
        {
            for (u32 x = 0; x < renderer->drawbuffer->width; x++)
            {
                u32 sx = floorf(x * target_width  / renderer->drawbuffer->width);
                u32 sy = floorf(y * target_height / renderer->drawbuffer->height);
                renderer->precomputed_downscale[computed_index] = (vec2i){ sx, sy };
                computed_index++;
            }
        }
    }
}