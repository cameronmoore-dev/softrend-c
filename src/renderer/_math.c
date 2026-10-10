#include "_math.h"

#include <math.h>

#define array_count(x) (sizeof(x) / sizeof((x)[0]))

vec3f vec3_mul_flt(vec3f *v, f32 f)
{
    vec3f result = 
    {
        .x = v->x * f,
        .y = v->y * f,
        .z = v->z * f
    };
    
    return result;
}

vec3f vec3_sub(vec3f *a, vec3f *b)
{
    vec3f result = 
    {
        .x = a->x - b->x,
        .y = a->y - b->y,
        .z = a->z - b->z,
    };

    return result;
}

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

vec4f mat4_mul_vec4(mat4 *mat, vec4f *vec)
{
    vec4f result = 
    {
        .x = mat->m[0][0] * vec->x + mat->m[0][1] * vec->y + mat->m[0][2] * vec->z + mat->m[0][3] * vec->w,
        .y = mat->m[1][0] * vec->x + mat->m[1][1] * vec->y + mat->m[1][2] * vec->z + mat->m[1][3] * vec->w,
        .z = mat->m[2][0] * vec->x + mat->m[2][1] * vec->y + mat->m[2][2] * vec->z + mat->m[2][3] * vec->w,
        .w = mat->m[3][0] * vec->x + mat->m[3][1] * vec->y + mat->m[3][2] * vec->z + mat->m[3][3] * vec->w
    };
    
    return result;
}

mat4 mat4_mul(const mat4 *a, const mat4 *b)
{
    u32 a_rows = 4;
    u32 a_cols = 4;
    u32 b_cols = 4;

    mat4 result = {};
    for (u32 row = 0; row < a_rows; row++)
    {
        for (u32 col = 0; col < b_cols; col++)
        {
            for (u32 e = 0; e < a_cols; e++)
            {
                result.m[row][col] += a->m[row][e] * b->m[e][col];
            }
        }
    }

    return result;
}

mat4 mat4_identity()
{
    mat4 result = 
    {
        .m[0][0] = 1.0f,
        .m[1][1] = 1.0f,
        .m[2][2] = 1.0f,
        .m[3][3] = 1.0f
    };

    return result;
}

mat4 perspective_matrix(f32 fov, f32 aspect, f32 z_near, f32 z_far)
{
    f32 f = 1.0f / tanf(fov * 0.5f);
    mat4 result = 
    {
        .m[0][0] = f / aspect,
        .m[1][1] = f,
        .m[2][2] = -(z_far + z_near) / (z_far - z_near),
        .m[2][3] = -(2.0f * z_far * z_near) / (z_far - z_near),
        .m[3][2] = -1.0f
    };

    return result;
}

mat4 orthographic_matrix(f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far)
{
    mat4 result = 
    {
        .m[0][0] = 2.0f / (left - right),
        .m[0][3] = -(right + left) / (right - left),
        .m[1][1] = 2.0f / (top - bottom),
        .m[1][3] = -(top + bottom) / (top - bottom),
        .m[2][2] = 2.0f / (z_far - z_near),
        .m[2][3] = -(z_far + z_near) / (z_far - z_near),
        .m[3][3] = 1.0f
    };

    return result;
}

void lookat_matrix(mat4 *mat, vec3f *pos, vec3f *forward, vec3f *up)
{
    vec3f fdir = 
    { 
        .x = pos->x - forward->x, 
        .y = pos->y - forward->y, 
        .z = pos->z - forward->z 
    };
    
    // Local Forward
    vec3f f = normalize(&fdir);

    vec3f c = cross(&f, up);

    // Local Side
    vec3f s = normalize(&c);
    
    // Local Up
    vec3f u = cross(&s, &f);

    mat->m[0][0] = s.x;
    mat->m[0][1] = s.y;
    mat->m[0][2] = s.z;
    mat->m[0][3] = -dot(&s, pos);

    mat->m[1][0] = u.x;
    mat->m[1][1] = u.y;
    mat->m[1][2] = u.z;
    mat->m[1][3] = -dot(&u, pos);

    mat->m[2][0] = -f.x;
    mat->m[2][1] = -f.y;
    mat->m[2][2] = -f.z;
    mat->m[2][3] = dot(&f, pos);

    mat->m[3][3] = 1.f;
}

mat4 matrix_rotate(f32 angle, vec3f *axis)
{
    f32 rad = degToRad(angle);
    vec3f ax = normalize(axis);

    f32 c = cosf(rad);
    f32 s = sinf(rad);
    f32 t = 1.0 - c;

    mat4 result = mat4_identity();
    result.m[0][0] = t * ax.x * ax.x + c;
    result.m[0][1] = t * ax.x * ax.y - s * ax.z;
    result.m[0][2] = t * ax.x * ax.z + s * ax.y;
    result.m[1][0] = t * ax.x * ax.y + s * ax.z;
    result.m[1][1] = t * ax.y * ax.y + c;
    result.m[1][2] = t * ax.y * ax.z - s * ax.x;
    result.m[2][0] = t * ax.x * ax.z - s * ax.y;
    result.m[2][1] = t * ax.y * ax.z + s * ax.x;
    result.m[2][2] = t * ax.z * ax.z + c;

    return result;
}

f32 lerp(f32 a, f32 b, f32 t)
{
    return a + t * (b - a);
}

f32 degToRad(f32 degrees)
{
    return degrees * (PI / 180.0f);
}

f32 dot(vec3f *a, vec3f *b)
{
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

f32 length(vec3f *vec)
{
    return sqrtf((vec->x * vec->x) + (vec->y * vec->y) + (vec->z * vec->z));
}

vec3f normalize(vec3f *vec)
{
    f32 magnitude = length(vec);
    return (vec3f){ (vec->x / magnitude), (vec->y / magnitude), (vec->z / magnitude) };
}

vec3f cross(vec3f *a, vec3f *b)
{
    vec3f result = 
    {
        .x = a->y * b->z - a->z * b->y,
        .y = a->z * b->x - a->x * b->z,
        .z = a->x * b->y - a->y * b->x
    };

    return result;
}

f32 clampf(f32 value, f32 min, f32 max)
{
    if (value < min)        return min;
    else if (value > max)   return max;
    else                    return value;
}

vec3f vec2_to_vec3(vec2f vec, f32 z)
{
    return (vec3f){ vec.x, vec.y, z };
}

vec4f vec2_to_vec4(vec2f vec, f32 z, f32 w)
{
    return (vec4f){ vec.x, vec.y, z, w };
}

vec4f vec3_to_vec4(vec3f *vec, f32 w)
{
    return (vec4f){ vec->x, vec->y, vec->z, w };
}