#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "common.h"
#include "opencv.h"

using namespace cv;

int read_png_frames(void * buffer, const char* filename, int expected_buffer_size) {
    // Read the image
    Mat image = imread(filename);
    if (image.empty()) {
        PRINT_ERROR("Could not open or find the image %s.\n", filename);
        return FAILED;
    }

    // Convert the image to a buffer
    cv::Mat flattened_image = image.reshape(1, 1);
    int buffer_size = flattened_image.total() * flattened_image.elemSize();
    if (buffer_size == expected_buffer_size) 
    {
        std::memcpy(buffer, flattened_image.data, buffer_size);
    } 
    else 
    {
        PRINT_ERROR("Buffer size mismatch: expected %d, got %d.\n", expected_buffer_size, buffer_size);
        return FAILED;
    }

    return SUCCESS;
}