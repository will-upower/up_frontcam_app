#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "common.h"
#include "screen_snip_out.h"
#include "screen_snip/screen_snip_out.h"

XImage *screen_image;
static Display *display_handle;
static int screen_handle;
static Window X11_window_handle;
static unsigned int capture_width, capture_height;

void print_XImage_info(XImage *image)
{
    printf("width:          %d\n", screen_image->width);
    printf("height:         %d\n", screen_image->height);
    printf("red_mask:       0x%x\n", screen_image->red_mask);
    printf("blue_mask:      0x%x\n", screen_image->blue_mask);
    printf("green_mask:     0x%x\n", screen_image->green_mask);
    printf("bitmap_pad:     %d\n", screen_image->bitmap_pad);
    printf("bytes_per_line: %d\n", screen_image->bytes_per_line);
    printf("format:         0x%x\n", screen_image->format);
    printf("byte_order:     0x%x\n", screen_image->byte_order);
    printf("bits_per_pixel: %d\n", screen_image->bits_per_pixel);
}

int screen_capture_init()
{
    display_handle = XOpenDisplay(NULL);
    if (display_handle == NULL) {
        PRINT_ERROR("Cannot open display\n");
        return FAILED;
    }

    capture_width = g_frame_width;
    capture_height = g_frame_height;

    screen_handle = DefaultScreen(display_handle);
    X11_window_handle = RootWindow(display_handle, screen_handle);

    return SUCCESS;
}

void screen_capture_deinit()
{
    XCloseDisplay(display_handle);
}

// This doesn't work. XImage data seems to go away when pointer is returned out of a function
XImage *screen_capture_begin()
{
    int x = 10, y = 10;

    screen_image = XGetImage(display_handle, X11_window_handle, x, y, capture_width, capture_height, AllPlanes, ZPixmap);

    if (screen_image == NULL) {
        PRINT_ERROR("Cannot capture screen\n");
        return FAILED;
    }

    return screen_image;
}

// Must call every frame after processing image
int screen_capture_end()
{
    if (screen_image != NULL) {
        XDestroyImage(screen_image);
    }
    else {
        PRINT_ERROR("Did not capture screen\n");
        return FAILED;
    }

    return SUCCESS;
}

void save_frame_as_bmp(const char *filename, unsigned char *frame_data, int width, int height) {
    printf("Writing to bmp\n");
    FILE *fp;
    unsigned int file_size = width * height * 3 + 54; // 3 bytes per pixel + 54 BMP header size

    unsigned char file_size_1 = file_size & 0xFF;
    unsigned char file_size_2 = (file_size >> 8) & 0xFF;
    unsigned char file_size_3 = (file_size >> 16) & 0xFF;
    unsigned char file_size_4 = (file_size >> 24) & 0xFF;

    unsigned char bmp_header[54] = {
        0x42, 0x4D,           // BMP signature
        file_size_1, file_size_2, file_size_3, file_size_4,  // File size in bytes
        0x00, 0x00, 0x00, 0x00,  // Reserved
        0x36, 0x00, 0x00, 0x00,  // Image data offset
        0x28, 0x00, 0x00, 0x00,  // Header size
        0x00, 0x05, 0x00, 0x00,  // Image width
        0x20, 0x03, 0x00, 0x00,  // Image height
        0x01, 0x00,           // Number of color planes
        0x18, 0x00,           // Bits per pixel (24 bits)
        0x00, 0x00, 0x00, 0x00,  // Compression method
        0x00, 0x00, 0x00, 0x00,  // Image size (uncompressed)
        0x00, 0x00, 0x00, 0x00,  // Horizontal resolution (pixels per meter)
        0x00, 0x00, 0x00, 0x00,  // Vertical resolution (pixels per meter)
        0x00, 0x00, 0x00, 0x00,  // Colors in color table
        0x00, 0x00, 0x00, 0x00,  // Important color count
    };

    // Update the BMP header with the image dimensions
    bmp_header[18] = (width & 0xFF);
    bmp_header[19] = (width >> 8) & 0xFF;
    bmp_header[22] = (height & 0xFF);
    bmp_header[23] = (height >> 8) & 0xFF;

    fp = fopen(filename, "wb");
    if (!fp) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fwrite(bmp_header, sizeof(bmp_header), 1, fp);
    fwrite(frame_data, width * height * 3, 1, fp);

    fclose(fp);
    printf("bitmap writing complete\n");
}

Conv_XImage2BGR(unsigned char *bgr_out, XImage *image, int width, int height)
{
    for (int i = 0; i < width * height; i++) {
        int out_index = 3 * i;
        int in_index  = 4 * i;

        bgr_out[out_index + 0] = image->data[in_index + 2];
        bgr_out[out_index + 1] = image->data[in_index + 1];
        bgr_out[out_index + 2] = image->data[in_index + 0];
    }
}
