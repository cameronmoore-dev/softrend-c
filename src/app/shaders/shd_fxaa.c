#include "shd_fxaa.h"

#include <math.h>

#define QUALITY 3

#if QUALITY == 0
    #define EDGE_THRESHOLD (1.f/4.f)
    #define EDGE_THRESHOLD_MIN (1.f/16.f)

    #define SUBPIXEL 1
    #define SUBPIXEL_TRIM (1.f/4.f)
    #define SUBPIXEL_TRIM_SCALE (1.f/(1.f - SUBPIXEL_TRIM))
    #define SUBPIXEL_CAP (3.f/4.f)

    #define SEARCH_STEPS 8
    #define SEARCH_ACCELERATION 1
    #define SEARCH_THRESHOLD (1.f/4.f)

#elif QUALITY == 1
    #define EDGE_THRESHOLD (1.f/8.f)
    #define EDGE_THRESHOLD_MIN (1.f/16.f)

    #define SUBPIXEL 1
    #define SUBPIXEL_TRIM (1.f/4.f)
    #define SUBPIXEL_TRIM_SCALE (1.f/(1.f - SUBPIXEL_TRIM))
    #define SUBPIXEL_CAP (3.f/4.f)

    #define SEARCH_STEPS 12
    #define SEARCH_ACCELERATION 1
    #define SEARCH_THRESHOLD (1.f/4.f)

#elif QUALITY == 2
    #define EDGE_THRESHOLD (1.f/8.f)
    #define EDGE_THRESHOLD_MIN (1.f/16.f)

    #define SUBPIXEL 1
    #define SUBPIXEL_TRIM (1.f/4.f)
    #define SUBPIXEL_TRIM_SCALE (1.f/(1.f - SUBPIXEL_TRIM))
    #define SUBPIXEL_CAP (7.f/8.f)

    #define SEARCH_STEPS 24
    #define SEARCH_ACCELERATION 1
    #define SEARCH_THRESHOLD (1.f/4.f)

#elif QUALITY == 3
    #define EDGE_THRESHOLD (1.f/12.f)
    #define EDGE_THRESHOLD_MIN (1.f/16.f)

    #define SUBPIXEL 1
    #define SUBPIXEL_TRIM (1.f/8.f)
    #define SUBPIXEL_TRIM_SCALE (1.f/(1.f - SUBPIXEL_TRIM))
    #define SUBPIXEL_CAP (1.f)

    #define SEARCH_STEPS 32
    #define SEARCH_ACCELERATION 2
    #define SEARCH_THRESHOLD (1.f/8.f)
#endif

f32 luma(colour_ colour);
colour_ texture_offset(rendertarget_ *rendertarget, s32 current, s32 offset, u32 size);

void fxaa_apply(rendertarget_ *rendertarget)
{
    u32 size  = rendertarget->w * rendertarget->h;
    s32 width = rendertarget->w;

    for (u32 i = 0; i < size; i++)
    {
        colour_ n = texture_offset(rendertarget, i, -width, size);
        colour_ w = texture_offset(rendertarget, i, -1, size);
        colour_ m = texture_offset(rendertarget, i, 0, size);
        colour_ e = texture_offset(rendertarget, i, 1, size);
        colour_ s = texture_offset(rendertarget, i, width, size);

        f32 luma_n = luma(n);
        f32 luma_w = luma(w);
        f32 luma_m = luma(m);
        f32 luma_e = luma(e);
        f32 luma_s = luma(s);

        f32 range_min = fminf(luma_m, fminf(fminf(luma_n, luma_w), fminf(luma_s, luma_e)));
        f32 range_max = fmaxf(luma_m, fmaxf(fmaxf(luma_n, luma_w), fmaxf(luma_s, luma_e)));
        f32 range = range_max - range_min;

        if (range < fmaxf(EDGE_THRESHOLD_MIN, range_max * EDGE_THRESHOLD))
        {
            rendertarget->colourbuffer[i] = m.packed;
            continue;
        }

        // Low-pass filter blend / Sub-Pixel Aliasing Test
        f32 luma_l  = (luma_n + luma_w + luma_e + luma_s) * 0.25f;
        f32 range_l = fabsf(luma_l - luma_m);
        f32 blend_l = fmaxf(0.0f, (range_l / range) - SUBPIXEL_TRIM) * SUBPIXEL_TRIM_SCALE;
        blend_l = fminf(SUBPIXEL_CAP, blend_l);

        // Box filter around current pixel
        colour_ nw = texture_offset(rendertarget, i, -width - 1, size);
        colour_ ne = texture_offset(rendertarget, i, -width + 1, size);
        colour_ sw = texture_offset(rendertarget, i,  width - 1, size);
        colour_ se = texture_offset(rendertarget, i,  width + 1, size);

        f32 luma_nw = luma(nw);
        f32 luma_ne = luma(ne);
        f32 luma_sw = luma(sw);
        f32 luma_se = luma(se);

        // Vertical/Horizontal Edge Test
        f32 edge_vertical = 
            fabsf(0.25f * luma_nw) + (-0.5f * luma_n) + (0.25f * luma_ne) + 
            fabsf(0.5f  * luma_w)  + (-1.0f * luma_m) + (0.5f  * luma_e)  + 
            fabsf(0.25f * luma_sw) + (-0.5f * luma_s) + (0.25f * luma_se);

        f32 edge_horizontal = 
            fabsf(0.25f * luma_nw) + (-0.5f * luma_w) + (0.25f * luma_sw) + 
            fabsf(0.5f  * luma_n)  + (-1.0f * luma_m) + (0.5f  * luma_s)  + 
            fabsf(0.25f * luma_ne) + (-0.5f * luma_e) + (0.25f * luma_se);

        bool horizontal_span = edge_vertical >= edge_horizontal;

        f32 threshold = range * SEARCH_THRESHOLD;
        f32 luma_neg = luma_m;
        f32 luma_pos = luma_m;
        f32 offset_neg = 0;
        f32 offset_pos = 0;
        for (u32 i = 0; i < SEARCH_STEPS; i++)
        {
            offset_neg -= (horizontal_span) ? width : 1.0f;
            offset_pos += (horizontal_span) ? width : 1.0f;
            f32 luma_a = luma(texture_offset(rendertarget, i, offset_neg, size));
            f32 luma_b = luma(texture_offset(rendertarget, i, offset_pos, size));

            if (fabsf(luma_a - luma_m) >= threshold && 
                offset_neg == 0)
            {
                luma_neg = luma_a;
            }

            if (fabsf(luma_b - luma_m) >= threshold && 
                offset_pos == 0)
            {
                luma_pos = luma_b;
            }
        }

        colour_ left  = texture_offset(rendertarget, i, ((horizontal_span) ? -width : -1), size);
        colour_ right = texture_offset(rendertarget, i, ((horizontal_span) ?  width :  1), size);
        f32 luma_dir_diff = luma_neg - luma_pos;
        colour_ target_colour = (luma_dir_diff > 0) 
            ? right 
            : left;

        colour_ blended = 
        {
            .comps[0] = m.comps[0] * (1.0f - blend_l) + target_colour.comps[0] * blend_l,
            .comps[1] = m.comps[1] * (1.0f - blend_l) + target_colour.comps[1] * blend_l,
            .comps[2] = m.comps[2] * (1.0f - blend_l) + target_colour.comps[2] * blend_l,
            .comps[3] = 255
        };

        rendertarget->colourbuffer[i] = blended.packed;
    }
}

f32 luma(colour_ colour)
{
    return ((f32)colour.comps[1] / 255.0f) * (0.587f/0.299f) * ((f32)colour.comps[2] / 255.0f);
}

colour_ texture_offset(rendertarget_ *rendertarget, s32 current, s32 offset, u32 size)
{
    u32 index = clampf(current + offset, 0, size - 1);
    colour_ result = 
    {
        .packed = rendertarget->colourbuffer[index]
    };

    return result;
}