#include "shd_ssao.h"

#include <stdlib.h>
#include <time.h>
#include <math.h>

f32 real_uniform_distribution(f32 min, f32 max)
{
    f32 dist = (f32)rand() / ((f32)RAND_MAX + 1.0f);
    return min + (max - min) * dist;
}

f32 smoothstep(f32 edge1, f32 edge2, f32 x)
{
    f32 t = clampf((x - edge1) / (edge2 - edge1), 0.0f, 1.0f);
    return t * t * (3.0f - (2.0f * t));
}

f32 gaussian(f32 x, f32 sigma)
{
    return expf(-(x * x) / (2.0f * sigma * sigma));
}

void bilateral_blur(rendertarget_ *rendertarget, ssao_data_ *data)
{
    s32 radius = 4;
    f32 sigma_space = 2.0f;
    f32 sigma_depth = 50.0f;
    
    for (u32 y = 0; y < rendertarget->h; y++)
    {
        for (u32 x = 0; x < rendertarget->w; x++)
        {
            u32 index = x + y * rendertarget->w;
            f32 centre_depth = rendertarget->depthbuffer[index];
            f32 centre_ao = *(f32 *)darr_at(data->ao, index);
            if (centre_depth > 1.0f || centre_ao == 0.0f)
            {
                *(f32 *)darr_at(data->blur, index) = centre_ao;
                continue;
            }

            f32 sum = 0.0f;
            f32 weight_sum = 0.0f;

            // Horizontal Pass
            for (s32 dx = -radius; dx <= radius; dx++)
            {
                s32 sx = (s32)clampf(x + dx, 0, rendertarget->w - 1);
                s32 s_index = sx + y * rendertarget->w;
                f32 sample_ao = *(f32 *)darr_at(data->ao, s_index);
                f32 sample_depth = rendertarget->depthbuffer[s_index];
                if (sample_depth > 1.0f || sample_ao == 0.0f)
                {
                    continue;
                }

                f32 wspace = gaussian((f32)dx, sigma_space);
                f32 depth_diff = fabs(centre_depth - sample_depth);
                f32 wdepth = gaussian(depth_diff * 100.f, sigma_depth);

                f32 weight = wspace * wdepth;
                sum += sample_ao * weight;
                weight_sum += weight;
            }
            *(f32 *)darr_at(data->blur, index) = (weight_sum) ? (sum / weight_sum) : centre_ao;

            // Vertical Pass
            for (s32 dy = -radius; dy <= radius; dy++)
            {
                s32 sx = (s32)clampf(x + dy, 0, rendertarget->w - 1);
                s32 s_index = sx + y * rendertarget->w;
                f32 sample_ao = *(f32 *)darr_at(data->ao, s_index);
                f32 sample_depth = rendertarget->depthbuffer[s_index];
                if (sample_depth > 1.0f || sample_ao == 0.0f)
                {
                    continue;
                }

                f32 wspace = gaussian((f32)dy, sigma_space);
                f32 depth_diff = fabs(centre_depth - sample_depth);
                f32 wdepth = gaussian(depth_diff * 100.f, sigma_depth);

                f32 weight = wspace * wdepth;
                sum += sample_ao * weight;
                weight_sum += weight;
            }
            
            *(f32 *)darr_at(data->ao, index) = (weight_sum) ? (sum / weight_sum) : centre_ao;
        }
    }
}

void ssao_init(rendertarget_ *rendertarget, ssao_data_ *data)
{
    srand(time(NULL));

    data->kernel = darr_create(sizeof(struct pair), data->samples);
    data->ao     = darr_create(sizeof(f32), rendertarget->w * rendertarget->h);
    data->blur   = darr_create(sizeof(f32), rendertarget->w * rendertarget->h);

    for (u32 i = 0; i < data->samples; i++)
    {
        f32 angle = real_uniform_distribution(0.0f, 2.0f * PI);
        f32 r = real_uniform_distribution(0.0f, 1.0f);

        f32 scale = (f32)i / (f32)data->samples;
        r = r * r * data->radius * (0.1f + 0.9f * scale);
        f32 dx = r * cosf(angle);
        f32 dy = r * sinf(angle);

        struct pair pair = 
        {
            .first  = dx,
            .second = dy
        };

        darr_replace(data->kernel, i, (void *)&pair);
    }
}

void ssao_close(ssao_data_ *data)
{
    darr_close(data->kernel);
    darr_close(data->ao);
    darr_close(data->blur);
}

void ssao_apply(rendertarget_ *rendertarget, ssao_data_ *data)
{
    for (u32 i = 0; i < darr_count(data->ao); i++)
    {
        *(f32 *)darr_at(data->ao, i) = 0.0f;
        *(f32 *)darr_at(data->blur, i) = 0.0f;
    }

    for (u32 i = 0; i < rendertarget->w * rendertarget->h; i++)
    {
        f32 z = rendertarget->depthbuffer[i];
        if (z > 1.0f)
        {
            continue;
        }

        f32 vote = 0.0f;
        s32 voters = 0;
        s32 x = i % rendertarget->w;
        s32 y = i / rendertarget->w;
        for (u32 j = 0; j < data->samples; j++)
        {
            struct pair kpair = *(struct pair *)darr_at(data->kernel, j);
            s32 sx = x + (s32)kpair.first;
            s32 sy = y + (s32)kpair.second;
            if (sx < 0.0f || sx >= rendertarget->w ||
                sy < 0.0f || sy >= rendertarget->h)
            {
                continue;
            }

            f32 sample_depth = rendertarget->depthbuffer[sx + sy * rendertarget->w];
            f32 diff = sample_depth - z;
            if (diff > 0.02f * z)
            {
                continue;
            }

            vote += (f32)(diff < 0.0f);
            voters++;
        }

        f32 sstep = smoothstep(0.0f, 1.0f, 1.0f - (vote / voters) * 0.5f);
        darr_replace(data->ao, i, &sstep);
    }

    if (data->do_blur)
        bilateral_blur(rendertarget, data);

    for (u32 i = 0; i < darr_count(data->ao); i++)
    {
        f32 ao = *(f32 *)darr_at(data->ao, i);
        if (ao == 0)
        {
            continue;
        }

        vec3f c = packed_to_rgb(colour(rendertarget->colourbuffer[i]));
        c.x *= ao;
        c.y *= ao;
        c.z *= ao;

        rendertarget->colourbuffer[i] = rgb_to_packed(&c).packed;
    }
}