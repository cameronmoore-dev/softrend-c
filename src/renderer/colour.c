#include "colour.h"

s32 clamp(s32 channel)
{
    if      (channel < 0)   return 0;
    else if (channel > 255) return 255;
    else                    return channel;
}

colour_ colour(u32 colour)
{
    colour_ result;
    result.packed = colour;

    return result;
}

colour_ rgb_to_packed(vec3f *colour)
{
    colour_ c;
    c.comps[0] = clamp((colour->x * 255.0f));
    c.comps[1] = clamp((colour->y * 255.0f));
    c.comps[2] = clamp((colour->z * 255.0f));

    return c;
}

vec3f packed_to_rgb(colour_ colour)
{
    vec3f c;
    c.x = (f32)colour.comps[0] / 255.0f;
    c.y = (f32)colour.comps[1] / 255.0f;
    c.z = (f32)colour.comps[2] / 255.0f;

    return c;
}

colour_ rgba_to_packed(vec4f *colour)
{
    colour_ c;
    c.comps[0] = clamp((colour->x * 255.0f));
    c.comps[1] = clamp((colour->y * 255.0f));
    c.comps[2] = clamp((colour->z * 255.0f));
    c.comps[3] = clamp((colour->w * 255.0f));
    
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

vec4f srgb_to_linear(colour_ colour)
{
    return (vec4f){0,0,0,0};
}

vec4f linear_to_srgb(colour_ colour)
{
    return (vec4f){0,0,0,0};
}