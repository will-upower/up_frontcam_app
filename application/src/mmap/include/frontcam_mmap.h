#include "common.h"

size_t get_buffer_size() {
    size_t size = 0;
    if (YUYV == g_customize.VIN_Capture_Format)                     
    {
        size = g_frame_width * g_frame_height * BPP_YUV; 
    }
    else if (RGB == g_customize.VIN_Capture_Format)                 
    {
        size = g_frame_width * g_frame_height * BPP_RGB; 
    }
    else if (Y10 == g_customize.VIN_Capture_Format)                 
    {
        size = g_frame_width * g_frame_height * BPP_Y10; 
    }
    else                                                            
    {
        size = g_frame_width * g_frame_height * BPP_YUV; 
    }
    return size;
}