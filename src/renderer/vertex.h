#pragma once

#include "renderer/_math.h"
#include "renderer/colour.h"

typedef struct vertex
{
    vec4f pos;
    vec2f uv;
    colour_ colour;
} vertex_;