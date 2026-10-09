#pragma once

#include "typedefs.h"

typedef struct
{
    u32 *pixels;
    u32 width;
    u32 height;
    u32 bpp;
} image_;

image_ image_load(const char *path);
void image_free(image_ *image);