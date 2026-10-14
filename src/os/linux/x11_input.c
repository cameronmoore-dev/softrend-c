#include "../input.h"

#include <X11/keysym.h>

key get_platform_key(u32 keycode);

bool is_key_pressed(input_ *input, key key)
{
    bool pressed = input->inputs[key];
    input->inputs[key] = false;

    return pressed;
}

bool is_key_held(input_ *input, key key)
{
    return input->inputs[key];
}

void _set_key_pressed(input_ *input, u32 keycode)
{
    key key = get_platform_key(keycode);
    if (key != INVALID)
    {
        input->inputs[key] = true;
    }
}

void _set_key_released(input_ *input, u32 keycode)
{
    key key = get_platform_key(keycode);
    if (key != INVALID)
    {
        input->inputs[key] = false;
    }
}


key get_platform_key(u32 keycode)
{
    key key = INVALID;
    switch (keycode)
    {
        case XK_1: key = NUM1; break;
        case XK_2: key = NUM2; break;
        case XK_3: key = NUM3; break;
        case XK_4: key = NUM4; break;
        case XK_5: key = NUM5; break;
        case XK_6: key = NUM6; break;
        case XK_q: key = KEY_Q; break;
        case XK_w: key = KEY_W; break;
        case XK_e: key = KEY_E; break;
        case XK_a: key = KEY_A; break;
        case XK_s: key = KEY_S; break;
        case XK_d: key = KEY_D; break;
        case XK_Escape: key = ESC; break;

        default: break;
    }

    return key;
}