#include <X11/Xlib.h>

extern XImage *screen_image;

void print_XImage_info(XImage *image);
int screen_capture_init();
void screen_capture_deinit();
XImage *screen_capture_begin();
int screen_capture_end();
void save_frame_as_bmp(const char *filename, unsigned char *frame_data, int width, int height);
Conv_XImage2BGR(unsigned char *bgr_out, XImage *screen_image, int width, int height);
void find_window_by_pid(Display* display, Window window, Atom pidAtom, unsigned long pid, Window* outWindow);
