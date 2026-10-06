#include "_math.h"

vec2f vec2_mul(vec2f a, vec2f b)
{
    vec2f v;
    v.x = a.x * b.x;
    v.y = a.y * b.y;

    return v;
}

vec3f vec3_mul(vec3f *a, vec3f *b)
{
    vec3f v;
    v.x = a->x * b->x;
    v.y = a->y * b->y;
    v.z = a->z * b->z;

    return v;
}

vec4f vec4_mul(vec4f *a, vec4f *b)
{
    vec4f v;
    v.x = a->x * b->x;
    v.y = a->y * b->y;
    v.z = a->z * b->z;
    v.w = a->w * b->w;

    return v;
}

mat4  mat4_mul(mat4 *a, mat4 *b)
{

}

f32 minf(f32 a, f32 b)
{
    return (a < b) ? a : b;
}

f32 maxf(f32 a, f32 b)
{
    return (a > b) ? a : b;
}