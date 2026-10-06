#pragma once

#include "defines.h"

typedef struct
{
    s32 x, y;
} vec2i;

typedef struct
{
    f32 x, y;
} vec2f;

typedef struct
{
    f32 x, y, z;
} vec3f;

typedef struct
{
    f32 x, y, z, w;
} vec4f;

typedef struct
{
    f32 m[3][3];
} mat3;

typedef struct
{
    f32 m[4][4];
} mat4;

typedef struct
{
    u32 x, y;
    u32 w, h;
} rect_;

vec2f vec2_mul(vec2f a, vec2f b);
vec3f vec3_mul(vec3f *a, vec3f *b);
vec4f vec4_mul(vec4f *a, vec4f *b);
mat4  mat4_mul(mat4 *a, mat4 *b);

f32 minf(f32 a, f32 b);
f32 maxf(f32 a, f32 b);