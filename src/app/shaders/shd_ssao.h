#pragma once

#include "renderer/renderer.h"
#include "data_structures/darr.h"

typedef struct ssao_data
{
    struct pair
    {
        f32 first, second;
    } pair;

    darr_ *kernel;
    darr_ *ao;
    darr_ *blur;

    f32 radius;
    u32 samples;
    bool do_blur;
} ssao_data_;

void ssao_init(rendertarget_ *rendertarget, ssao_data_ *data);
void ssao_close(ssao_data_ *data);
void ssao_apply(rendertarget_ *rendertarget, ssao_data_ *data);