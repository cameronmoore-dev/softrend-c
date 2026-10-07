#include "renderer.h"
#include "_math.h"

#include <stdlib.h>
#include <math.h>

#include <stdio.h>

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
    r.depthbuffer = (f32 *)malloc(drawbuffer->width * drawbuffer->height * sizeof(f32));
    r.clear_colour.packed = clear_colour;

    return r;
}

void renderer_clear(renderer_ * renderer)
{
    if (!renderer->drawbuffer->data)
    {
        return;
    }

    u32 buf_size = renderer->drawbuffer->width * renderer->drawbuffer->height;
    for (u32 i = 0; i < buf_size; i++)
    {
        renderer->drawbuffer->data[i] = renderer->clear_colour.packed;
        renderer->depthbuffer[i] = INFINITY;
    }
}

void renderer_draw(renderer_ *renderer, mesh_ *mesh, shader_ *shader)
{
    for (u32 i = 0; i < mesh->num_vertices; i += 3)
    {
        face_ face = 
        {
            .vertices[0] = shader->vertex(shader->data, &mesh->vertices[i]),
            .vertices[1] = shader->vertex(shader->data, &mesh->vertices[i + 1]),
            .vertices[2] = shader->vertex(shader->data, &mesh->vertices[i + 2])
        };

        transform_to_screen_space(renderer, &face);
        if (winding_order(&face) <= 0.0f)
        {
            continue;
        }
    
        rasterize(renderer, &face, shader);
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
            if (p.x < renderer->viewport.x || p.x > (renderer->viewport.x + renderer->viewport.w) ||
                p.y < renderer->viewport.y || p.y > (renderer->viewport.y + renderer->viewport.h))
            {
                continue;
            }

            vec3f point = barycentric(&p, face);
            bool point_in_polygon = (point.x >= 0.0f && point.y >= 0.0f && point.z >= 0.0f);
            if (point_in_polygon)
            {
                u32 flipped_y = renderer->drawbuffer->height - 1.0f - p.y;
                u32 buffer_index = p.x + flipped_y * renderer->drawbuffer->width;

                f32 z = face->vertices[0].pos.z * point.x + face->vertices[1].pos.z * point.y + face->vertices[2].pos.z * point.z;
                if (z < renderer->depthbuffer[buffer_index])
                {
                    renderer->depthbuffer[buffer_index] = z;
                    colour_ frag_colour;
                    if (shader->fragment(shader->data, &point, face->vertices, &frag_colour))
                    {
                        renderer->drawbuffer->data[buffer_index] = frag_colour.packed;
                    }
                }
            }
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
        vpos->x = (vpos->x + 1.0f) * (renderer->viewport.w * 0.5f) + renderer->viewport.x;
        vpos->y = (vpos->y + 1.0f) * (renderer->viewport.h * 0.5f) + renderer->viewport.y;
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
    f32 left_most   = fmin(a->x, fmin(b->x, c->x));
    f32 bottom_most = fmin(a->y, fmin(b->y, c->y));
    f32 right_most  = fmax(a->x, fmax(b->x, c->x));
    f32 top_most    = fmax(a->y, fmax(b->y, c->y));

    rect_ bb = 
    {
        .x = (u32)left_most,
        .y = (u32)bottom_most,
        .w = (u32)right_most,
        .h = (u32)top_most
    };
    return bb;
}