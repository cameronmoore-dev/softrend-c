#pragma once

#include "typedefs.h"
#include "renderer/_math.h"

typedef struct
{
    union
    {
        u8  comps[4];
        u32 packed;
    };
} colour_;

colour_ rgba_to_packed(vec4f colour);
vec4f packed_to_rgba(colour_ colour);
vec4f srgb_to_linear(colour_ colour);
vec4f linear_to_srgb(colour_ colour);