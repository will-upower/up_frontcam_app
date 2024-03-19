#include <X11/Xlib.h>

extern XImage *screen_image;

void Conv_RGBA2RGB(XImage *image, unsigned char *bgr_data, unsigned long image_height, unsigned long image_width);
int screen_capture_init();
void screen_capture_deinit();
XImage *screen_capture_begin();
int screen_capture_end();
void save_frame_as_bmp(const char *filename, unsigned char *frame_data, int width, int height);
