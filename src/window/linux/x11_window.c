#include "window/window.h"

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <libudev.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>

#define MOUSE_BUTTON_LEFT   1
#define MOUSE_BUTTON_MIDDLE 2
#define MOUSE_BUTTON_RIGHT  3
#define SCROLL_WHEEL_UP     4
#define SCROLL_WHEEL_DOWN   5
#define XBUTTON0            8
#define XBUTTON1            9

typedef struct platform_context
{
    Display *display;
    xcb_connection_t *connection;
    xcb_window_t window;
    xcb_screen_t *screen;
    xcb_atom_t wm_deleteWindow;
    struct udev *udevice;
    struct udev_monitor *umonitor;

    xcb_gcontext_t gid;
} platform_context;

void window_on_resize(platform_context *context, sr_window *window, xcb_configure_notify_event_t *cfg);
xcb_atom_t _xcb_intern_atom(xcb_connection_t *connection, const char *atom_name);

platform_context *window_create_platform_context()
{
    return malloc(sizeof(platform_context));
}

sr_window *window_create(platform_context *context, const char *title, u32 width, u32 height)
{
    context->display = XOpenDisplay(NULL);
    XAutoRepeatOff(context->display);

    context->connection = XGetXCBConnection(context->display);
    if (xcb_connection_has_error(context->connection))
    {
        printf("Connection Error!\n");
        return NULL;
    }

    XSetEventQueueOwner(context->display, XCBOwnsEventQueue);

    const struct xcb_setup_t *setup = xcb_get_setup(context->connection);
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    for (s32 i = 0; i > 0; i--)
    {
        xcb_screen_next(&it);
    }

    context->screen = it.data;
    context->window = xcb_generate_id(context->connection);

    u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    u32 event_values = 
        XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | 
        XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | 
        XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION | 
        XCB_EVENT_MASK_STRUCTURE_NOTIFY;
    u32 value_list[] = 
    {
        context->screen->black_pixel, 
        event_values 
    };

    xcb_void_cookie_t window = xcb_create_window(
        context->connection,
        XCB_COPY_FROM_PARENT,
        context->window,
        context->screen->root,
        0, 0,
        width, height,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        context->screen->root_visual,
        event_mask,
        value_list
    );

    xcb_change_property(
        context->connection, 
        XCB_PROP_MODE_REPLACE, 
        context->window, 
        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 
        8, strlen(title), title
    );

    context->wm_deleteWindow = _xcb_intern_atom(context->connection, "WM_DELETE_WINDOW");
    xcb_atom_t proto = _xcb_intern_atom(context->connection, "WM_PROTOCOLS");
    xcb_change_property(
        context->connection,
        XCB_PROP_MODE_REPLACE,
        context->window, proto,
        4, 32, 1,
        &context->wm_deleteWindow
    );

    xcb_map_window(context->connection, context->window);
    xcb_flush(context->connection);

    context->udevice = udev_new();
    context->umonitor = udev_monitor_new_from_netlink(context->udevice, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(context->umonitor, "input", NULL);
    udev_monitor_enable_receiving(context->umonitor);

    sr_window *wnd = malloc(sizeof(sr_window));
    memset(wnd, 0, sizeof(sr_window));
    wnd->backbuffer = malloc(sizeof(sr_backbuffer));
    wnd->title  = title;
    wnd->width  = width;
    wnd->height = height;
    wnd->isOpen = true;

    context->gid = xcb_generate_id(context->connection);
    xcb_create_gc(context->connection, context->gid, context->window, 0, NULL);

    return wnd;
}

void window_swap_buffers(platform_context *context, sr_window *window)
{
    if (window->frontbuffer && 
        window->backbuffer->buffer)
    {
        memcpy(window->frontbuffer, window->backbuffer->buffer, window->backbuffer->width * window->backbuffer->height * sizeof(u32));
        xcb_put_image(
            context->connection, XCB_IMAGE_FORMAT_Z_PIXMAP, 
            context->window, context->gid, 
            window->width, window->height, 
            0, 0, 
            0, context->screen->root_depth, 
            window->width * window->height * sizeof(u32),
            (u8 *)window->frontbuffer
        );
        xcb_flush(context->connection);
    }

    window_pump_messages(context, window);
}

void window_cleanup(platform_context *context, sr_window *window)
{
    XAutoRepeatOn(context->display);
    xcb_destroy_window(context->connection, context->window);
    xcb_free_gc(context->connection, context->gid);
    udev_unref(context->udevice);
    udev_monitor_unref(context->umonitor);

    free(window->backbuffer->buffer);
    free(window->backbuffer);
    free(window);
    free(context);
}

void window_pump_messages(platform_context *context, sr_window *window)
{
    xcb_generic_event_t *event;
    while (event = xcb_poll_for_event(context->connection))
    {
        if (!event)
        {
            break;
        }

        switch (event->response_type & ~0x80)
        {
            case XCB_CLIENT_MESSAGE:
            {
                xcb_client_message_event_t *client = (xcb_client_message_event_t *)event;
                if (client->data.data32[0] == context->wm_deleteWindow)
                {
                    window_close(window);
                }
            } break;

            case XCB_CONFIGURE_NOTIFY:
            {
                xcb_configure_notify_event_t *cfg = (xcb_configure_notify_event_t *)event;
                window_on_resize(context, window, cfg);
            } break;

            case XCB_MOTION_NOTIFY:
            {
            } break;

            default: break;
        }

        free(event);
    }
}

bool window_is_open(sr_window *window)
{
    return window->isOpen;
}

void window_close(sr_window *window)
{
    window->isOpen = false;
}

void window_on_resize(platform_context *context, sr_window *window, xcb_configure_notify_event_t *cfg)
{
    window->width  = cfg->width;
    window->height = cfg->height;

    free(window->frontbuffer);
    free(window->backbuffer->buffer);
    window->frontbuffer = malloc(cfg->width * cfg->height * sizeof(u32));
    window->backbuffer->buffer = malloc(cfg->width * cfg->height * sizeof(u32));
    window->backbuffer->width  = cfg->width;
    window->backbuffer->height = cfg->height;

    memset(window->backbuffer->buffer, 0x111111, cfg->width * cfg->height * sizeof(u32));
}

xcb_atom_t _xcb_intern_atom(xcb_connection_t *connection, const char *atom_name)
{
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 0, strlen(atom_name), atom_name);
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, NULL);
    xcb_atom_t atom = reply->atom;

    free(reply);
    return atom;
}