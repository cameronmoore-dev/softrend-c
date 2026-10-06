#include "colour.h"

colour_ rgba_to_packed(vec4f colour)
{
    colour_ c;
    c.comps[0] = (u8)(colour.x * 255.0f);
    c.comps[1] = (u8)(colour.y * 255.0f);
    c.comps[2] = (u8)(colour.z * 255.0f);
    c.comps[3] = (u8)(colour.w * 255.0f);

    return c;
}

vec4f packed_to_rgba(colour_ colour)
{
    vec4f c;
    c.x = (f32)colour.comps[0] / 255.0f;
    c.y = (f32)colour.comps[1] / 255.0f;
    c.z = (f32)colour.comps[2] / 255.0f;
    c.w = (f32)colour.comps[3] / 255.0f;

    return c;
}