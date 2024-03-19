#include <X11/Xlib.h>
#include <stdio.h>

#include "common.h"
#include "screen_snip_out.h"

static Display *display;
static int screen;
static Window root;
static XImage *image;
static unsigned int width = 896, height = 504;

int screen_capture_init()
{
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        PRINT_ERROR("Cannot open display\n");
        return FAILED;
    }

    screen = DefaultScreen(display);
    root = RootWindow(display, screen);

    return SUCCESS;
}

void screen_capture_deinit()
{
    XCloseDisplay(display);
}

void *screen_capture_begin()
{
    int x = 10, y = 10;

    image = XGetImage(display, root, x, y, width, height, AllPlanes, ZPixmap);

    if(image == NULL) {
        PRINT_ERROR("Cannot capture screen\n");
        return FAILED;
    }

    return (void*)image;
}

// Must call every frame after processing image
int screen_capture_end()
{
    if (image != NULL) {
        XDestroyImage(image);
    }
    else {
        PRINT_ERROR("Did not capture screen\n");
        return FAILED;
    }

    return SUCCESS;
}
