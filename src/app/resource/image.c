#include "image.h"
#include "renderer/colour.h"

#include <stdlib.h>

void image_free(image_ *image)
{
    free(image->pixels);
}

void bilinear_filter(image_ *image)
{
    for (u32 y = 0; y < image->height; y++)
    {
        for (u32 x = 0; x < image->width; x++)
        {
            u32 xm = clampf(x, 0, x);
            u32 ym = clampf(y, 0, y);
            u32 xa = clampf(x + 1, x, image->width  - 1);
            u32 ya = clampf(y + 1, y, image->height - 1);
            f32 tx = (f32)x / (image->width  - 1);
            f32 ty = (f32)y / (image->height - 1);
            f32 fx = tx * image->width  - x;
            f32 fy = ty * image->height - y;
            vec4f tl = packed_to_rgba(colour(image->pixels[xm + ym * image->width]));
            vec4f tr = packed_to_rgba(colour(image->pixels[xa + ym * image->width]));
            vec4f bl = packed_to_rgba(colour(image->pixels[xm + ya * image->width]));
            vec4f br = packed_to_rgba(colour(image->pixels[xa + ya * image->width]));

            // Interpolate top corners
            f32 t = 1.0f - fx;
            vec4f a = vec4_mul_flt(&tl, t);
            vec4f b = vec4_mul_flt(&tr, fx);
            vec4f txi = vec4_add(&a, &b);

            // Interpolate bottom corners
            a = vec4_mul_flt(&bl, t);
            b = vec4_mul_flt(&br, fx);
            vec4f bxi = vec4_add(&a, &b);

            // Interpolate top and bottom corners together
            t = 1.0f - fy;
            a = vec4_mul_flt(&txi, t);
            b = vec4_mul_flt(&bxi, fy);
            vec4f result = vec4_add(&a, &b);

            image->pixels[x + y * image->width] = rgba_to_packed(&result).packed;
        }
    }
}