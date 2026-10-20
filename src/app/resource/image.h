#pragma once

#include "typedefs.h"
#include "renderer/_math.h"

#define WRAPTYPE_REPEAT          0
#define WRAPTYPE_MIRRORED_REPEAT 1
#define WRAPTYPE_CLAMP_TO_EDGE   2
#define WRAPTYPE_CLAMP_TO_BORDER 3

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
void bilinear_filter(image_ *image);