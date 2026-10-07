#include "renderer.h"
#include "_math.h"

void rasterize(renderer_ *renderer, vertex_ *mesh, shader_ *shader);
void transform_to_screen_space(renderer_ * renderer, vertex_ *mesh);
vec3f barycentric(vec2i *p, vertex_ *mesh);
f32 winding_order(vertex_ *mesh);
rect_ polygon_screen_bounding_box(vertex_ *mesh);

renderer_ renderer_init(backbuffer_ *drawbuffer, u32 clear_colour)
{
    renderer_ r = {};
    r.drawbuffer = drawbuffer;
    r.viewport = (viewport_){ 0, 0, drawbuffer->width, drawbuffer->height };
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
    }
}

void renderer_draw(renderer_ *renderer, vertex_ *mesh, shader_ *shader)
{
    vertex_ v0 = shader->vertex(shader->data, &mesh[0]);
    vertex_ v1 = shader->vertex(shader->data, &mesh[1]);
    vertex_ v2 = shader->vertex(shader->data, &mesh[2]);

    vertex_ face[3] = { v0, v1, v2 };
    transform_to_screen_space(renderer, face);
    if (winding_order(face) <= 0.0f)
    {
        return;
    }

    rasterize(renderer, face, shader);
}

void rasterize(renderer_ *renderer, vertex_ *mesh, shader_ *shader)
{
    // rect_ bounding_box = polygon_screen_bounding_box(mesh);
    viewport_ vp = renderer->viewport;
    vec2i p;
    for (p.y = vp.y; p.y < vp.h; p.y++)
    {
        for (p.x = vp.x; p.x < vp.w; p.x++)
        {
            // Discard if the point is outside of the viewport's bounds
            if (p.x < renderer->viewport.x || p.x > (renderer->viewport.x + renderer->viewport.w) ||
                p.y < renderer->viewport.y || p.y > (renderer->viewport.y + renderer->viewport.h))
            {
                continue;
            }

            vec3f point = barycentric(&p, mesh);
            bool point_in_polygon = (point.x >= 0.0f && point.y >= 0.0f && point.z >= 0.0f);
            if (point_in_polygon)
            {
                // f32 z = mesh[0].pos.z * point.x + mesh[1].pos.z * point.y + mesh[2].pos.z * point.z;
                u32 flipped_y = renderer->drawbuffer->height - 1.0f - p.y;
                u32 frame_buffer_index = p.x + flipped_y * renderer->drawbuffer->width;
                colour_ frag_colour;
                if (shader->fragment(shader->data, &point, mesh, &frag_colour))
                {
                    renderer->drawbuffer->data[frame_buffer_index] = frag_colour.packed;
                }
            }
        }
    }
}

void transform_to_screen_space(renderer_ *renderer, vertex_ *mesh)
{
    for (u32 i = 0; i < 3; i++)
    {
        vec4f *vpos = &mesh[i].pos;

        // Perspective Division
        vpos->x /= vpos->w;
        vpos->y /= vpos->w;
        vpos->z /= vpos->w;

        // To Screen Space
        vpos->x = (vpos->x + 1.0f) * (renderer->viewport.w * 0.5f) + renderer->viewport.x;
        vpos->y = (vpos->y + 1.0f) * (renderer->viewport.h * 0.5f) + renderer->viewport.y;
    }
}

vec3f barycentric(vec2i *p, vertex_ *mesh)
{
    vec4f *a = &mesh[0].pos;
    vec4f *b = &mesh[1].pos;
    vec4f *c = &mesh[2].pos;
    f32 denom = (b->y - c->y) * (a->x - c->x) + (c->x - b->x) * (a->y - c->y);
    f32 wx = ((b->y - c->y) * (p->x - c->x) + (c->x - b->x) * (p->y - c->y)) / denom;
    f32 wy = ((c->y - a->y) * (p->x - c->x) + (a->x - c->x) * (p->y - c->y)) / denom;
    f32 wz = 1.0f - wx - wy;

    return (vec3f){ wx, wy, wz };
}

f32 winding_order(vertex_ *mesh)
{
    vec4f *a = &mesh[0].pos;
    vec4f *b = &mesh[1].pos;
    vec4f *c = &mesh[2].pos;

    return (b->x - a->x) * (c->y - a->y) - (b->y - a->y) * (c->x - a->x);
}

rect_ polygon_screen_bounding_box(vertex_ *mesh)
{
    vec4f *a = &mesh[0].pos;
    vec4f *b = &mesh[1].pos;
    vec4f *c = &mesh[2].pos;
    f32 left_most   = minf(a->x, minf(b->x, c->x));
    f32 bottom_most = minf(a->y, minf(b->y, c->y));
    f32 right_most  = maxf(a->x, maxf(b->x, c->x));
    f32 top_most    = maxf(a->y, maxf(b->y, c->y));

    rect_ bb = 
    {
        .x = (u32)left_most,
        .y = (u32)bottom_most,
        .w = (u32)right_most,
        .h = (u32)top_most
    };
    return bb;
}