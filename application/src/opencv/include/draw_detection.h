#ifndef __DRAW_DETECTION_H__
#define __DRAW_DETECTION_H__

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "common.h"

using namespace cv;
using namespace std;

#define DEBUG_FLAG 0
#define IMG_WIDTH 1280
#define IMG_HIGHT 800

#define MAX_ROAD_CORD_HEIGHT 720
#define MAX_ROAD_CORD_WIDTH 1280
#define MAX_ROAD_CORD_SIZE (MAX_ROAD_CORD_HEIGHT * MAX_ROAD_CORD_WIDTH)

#define MAX_OBJECT_DETECTION 10
#define MAX_POSE_CORDINATES 17

enum obj_class{
    CLS_PERSON = 0,
    CLS_CAR
};

struct bbox
{
    cordinate start;
    cordinate end;
    float score;
};
struct POS_EST
{
    bbox bb_data;
    cordinate pos[MAX_POSE_CORDINATES];
    uint32_t cls;
};
struct POS
{    
    cordinate cor[MAX_POSE_CORDINATES];
};
struct road_map
{
    unsigned char road[MAX_ROAD_CORD_SIZE];
    int total_det;
    bbox bb_data[MAX_OBJECT_DETECTION];
    POS pos[4];
    uint32_t cls;
};

#define CLR_UNKNOWN Scalar(0,0,0)
#define CLR_RED Scalar(0, 0, 255)
#define CLR_YELLOW Scalar(0, 255, 255)
#define CLR_BLUE Scalar(255, 0, 0)
#define CLR_PURPLE Scalar(255, 0, 255)
#define CLR_GREEN Scalar(0,  255,  0)

int process_yuv(void *inImage, Mat &outimg, int plane);
int process_rgb(void *inImage, Mat &outimg, int plane);

#endif //__DRAW_DETECTION_H__