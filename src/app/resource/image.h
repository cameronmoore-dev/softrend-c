#pragma once

#include "typedefs.h"
#include "renderer/_math.h"

#define REPEAT          0
#define MIRRORED_REPEAT 1
#define CLAMP_TO_EDGE   2
#define CLAMP_TO_BORDER 3

typedef struct
{
    u32 *pixels;
    u32 width;
    u32 height;
    u32 bpp;
    u32 wrap_type;
    vec2f offset;
    vec2f scale;
} image_;

image_ image_load(const char *path);
void image_free(image_ *image);