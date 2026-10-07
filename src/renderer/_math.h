#pragma once

#include "typedefs.h"

#define PI 3.14159265359f
#define E  2.71828f

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

mat4 mat4_identity();
mat4 perspective_matrix(f32 fov, f32 aspect, f32 z_near, f32 z_far);
mat4 orthographic_matrix(f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far);
void lookat_matrix(mat4 *mat, vec3f *pos, vec3f *forward, vec3f *up);
void matrix_rotate(mat4 *mat, f32 angle, vec3f *axis);

vec2f vec2_mul(vec2f a, vec2f b);
vec3f vec3_mul(vec3f *a, vec3f *b);
vec4f vec4_mul(vec4f *a, vec4f *b);
vec4f mat4_mul_vec4(mat4 *mat, vec4f *vec);
mat4  mat4_mul(mat4 *a, mat4 *b);

f32 minf(f32 a, f32 b);
f32 maxf(f32 a, f32 b);
f32 lerp(f32 a, f32 b, f32 t);
f32 degToRad(f32 degrees);
f32 dot(vec3f *a, vec3f *b);
f32 length(vec3f *vec);
vec3f normalize(vec3f *vec);
vec3f cross(vec3f *a, vec3f *b);

vec3f vec2_to_vec3(vec2f vec, f32 z);
vec4f vec2_to_vec4(vec2f vec, f32 z, f32 w);
vec4f vec3_to_vec4(vec3f *vec, f32 w);