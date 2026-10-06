#pragma once

#include "defines.h"
#include "_math.h"

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