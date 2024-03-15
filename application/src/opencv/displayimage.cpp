/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only
 * intended for use with Renesas products. No other uses are authorized. This
 * software is owned by Renesas Electronics Corporation and is protected under
 * all applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
 * LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
 * TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
 * ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
 * FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
 * ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software
 * and to discontinue the availability of this software. By using this software,
 * you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2022-2023 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/*********************************************************************************************************************
* File Name    : DisplayImage.cpp
* Version      : 0.1.0
* Description  : DisplayImage Main File
*********************************************************************************************************************/
/*********************************************************************************************************************
* History : Version DD.MM.YYYY Description
*         : 0.1.0   06.03.2023 Alpha Release
*********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "common.h"
#include "opencv.h"
#include "customize.h"
#include "draw_detection.h"

using namespace cv;

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private variables 
 *********************************************************************************************************************/             
#define CLR_BLACK Scalar(0,0,0)
#define CLR_WHITE Scalar(255,255,255)
#define CLR_RED Scalar(255, 0, 0)
#define CLR_YELLOW Scalar(255, 255, 0)
#define CLR_BLUE Scalar(0, 0, 255)
#define CLR_PURPLE Scalar(255, 0, 255)
#define CLR_GRAY Scalar(150, 150,150)
#define CLR_GREEN Scalar(0,255,0)

int   g_graph_scaling = 2;
int   g_graph_start   = 150;
int   g_max_load      = 100;
float cdnnload         = 0;   
int   avgcdnnload      = 0;
road_map g_road_map;

std::vector<int64_t> cpu_load_arr; 
std::vector<int64_t> cdnn_load_arr;
std::vector<int64_t> g_avg_cdnn_load_arr;


/**********************************************************************************************************************
 Private variables
 *********************************************************************************************************************/
static const char *GRAPH_LABEL_CORE_NAMES[CORE_CNT] = {
    /* index is corresponding to e_graph_label_core_types */
    "CPU LOAD",
    #if (CDNN)
    "AVG CDNN LOAD"
    #endif
    };

typedef enum{
    PLN_1 = 1,
    PLN_2,
    PLN_3
}planes;

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
typedef enum
{
    //GRAPH_LABLE_CORE_CNN,
    GRAPH_LABLE_CORE_CPU,
    #if (CDNN)
    GRAPH_LABLE_CORE_CDNN
    #endif
} e_graph_label_core_t;

/*********************************************************************************************************************/
/* Function Name : log_display */
/**********************************************************************************************************************
 * @brief       Display the log of CNN and CPU
                [Covers: BPAP_FC_V4H_AD020][Covers: BPAP_FC_V4H_AD021]
 * @param[in]   none
 * @param[out]  src
 * @retval      true          success(0)
 * @retval      false         fail(1)
***********************************************************************************************************************/
static int log_display(cv::Mat &src)
{
    int font_type = FONT_HERSHEY_PLAIN;
    int i =0;
    int arr_size = 0;
    if  (true == g_customize.CPU_Load_Enable)
    {
        arr_size =  cpu_load_arr.size();
    }
    #if (CDNN)
    if  (true == g_customize.CDNN_Load_Enable)  
    {
        arr_size =  cdnn_load_arr.size();
    }
    #endif

    char log_str[100];
    char timestamp[100];
    for(int i = 0;i<arr_size;i++)
    {
        /*Display Time stamp*/
        struct timespec ts;
        timespec_get(&ts, TIME_UTC);
        time_t seconds = ts.tv_sec;
        struct tm *t = localtime(&seconds);
        sprintf(timestamp,"%04d-%02d-%02d %02d:%02d:%02d.%03d",
        t->tm_year+1900, t->tm_mon+1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec,
        ts.tv_nsec/1000000);
        Point text_point(20,35*(i)+10);
#if (CDNN)
        if ((true == g_customize.CDNN_Load_Enable) && (true == g_customize.CPU_Load_Enable))
        {
            sprintf(log_str,"[%s] CPU : %d%% CDNN : %d%% Average CDNN : %d%%",timestamp,(int)g_cpu_usage ,(int)cdnnload,(int)avgcdnnload);
           
        }
        else if(true == g_customize.CDNN_Load_Enable)
        {
            sprintf(log_str,"[%s] CDNN : %d%% Avg CDNN : %d%% Exec : %0.2fms",timestamp,(int)cdnnload,(int)avgcdnnload,(float)g_exe_time/1000);
        }
        else if(true == g_customize.CPU_Load_Enable)
        {
            sprintf(log_str,"[%s] CPU Load : %d%%",timestamp,(int)g_cpu_usage);
        }

#else
        if  (true == g_customize.CPU_Load_Enable)
        {
            sprintf(log_str,"[%s] CPU Load : %d%%",timestamp,(int)g_cpu_usage);
        }
#endif
        putText(src, log_str,text_point , font_type, 1, CLR_WHITE, 1, LINE_8, 0);

    }
    return 0;
}
/**********************************************************************************************************************
 End of function log_display
 *********************************************************************************************************************/

/*********************************************************************************************************************/
/* Function Name : graph_display */
/**********************************************************************************************************************
 * @brief       Display the graph of CNN and CPU
 * @param[in]   load_arr
 * @param[in]   core_index
 * @param[out]  src
 * @retval      true          success(0)
 * @retval      false         fail(1)
***********************************************************************************************************************/
static int graph_display(cv::Mat &src, int core_index, std::vector<int64_t> load_arr)
{
    int thickness = 1;
    int font_type = FONT_HERSHEY_PLAIN;
    int graph_start_pnt_x = 60;
    int graph_start_pnt_y = g_graph_start*g_graph_scaling;
    int max_load = g_max_load*g_graph_scaling;
    int graph_width = 1270;
    int time_seconds;
    int time_seconds_max = 21;
    char time_seconds_str[20];
    int load_value,i;
    int step;
    Point ps;
    Point pe; 
    Point p_new;

    /*Draw X and Y axis*/
    Point ls_hor(graph_start_pnt_x,graph_start_pnt_y);
    Point le_hor(graph_width,graph_start_pnt_y);
    Point ls_ver(graph_start_pnt_x,graph_start_pnt_y);
    Point le_ver(graph_start_pnt_x, graph_start_pnt_y - (max_load+50));
    thickness = 2;
    arrowedLine(src, ls_hor, le_hor, CLR_BLACK, thickness,LINE_8,0,0.01);
    arrowedLine(src, ls_ver, le_ver, CLR_BLACK, thickness,LINE_8,0,0.1);

    /*Add graph details*/
    Point disp_max(10, graph_start_pnt_y-max_load);
    Point disp_mid(20, graph_start_pnt_y-(max_load/2));
    Point disp_axis(1180, graph_start_pnt_y+30);
    Point disp_core_name(600, (g_graph_start*g_graph_scaling)-(110*g_graph_scaling));
    putText(src, "100%", disp_max, font_type, 1, CLR_BLACK, 2, LINE_8, 0);
    putText(src, "50%", disp_mid, font_type, 1, CLR_BLACK, 2, LINE_8, 0);
    putText(src, "time(secs)", disp_axis, font_type, 1, CLR_BLACK, 2, LINE_8, 0);
    putText(src, GRAPH_LABEL_CORE_NAMES[core_index], disp_core_name, font_type, 1, CLR_BLACK, 2, LINE_8, 0);
    for(int time_seconds = 1; time_seconds<time_seconds_max; time_seconds++)
    {
        Point disp3(graph_start_pnt_x*time_seconds, graph_start_pnt_y+15);
        sprintf(time_seconds_str,"%d",time_seconds-1);
        if(time_seconds == (time_seconds_max-1))
        {
            sprintf(time_seconds_str,"now");
        }
        putText(src, time_seconds_str, disp3, font_type, 1, CLR_BLACK, 2, LINE_8, 0);
    }

    /*Draw threshold lines*/
    Point max_s(graph_start_pnt_x, graph_start_pnt_y-max_load);
    Point max_e(graph_width,graph_start_pnt_y-max_load);
    line(src, max_s, max_e,
    CLR_GRAY,
    1, LINE_8);

    /*Draw mid lines*/
    Point mid_s(graph_start_pnt_x, graph_start_pnt_y-(max_load/2));
    Point mid_e(graph_width,graph_start_pnt_y-(max_load/2));
    line(src, mid_s, mid_e,
    CLR_GRAY,
    1, LINE_8);

    /*Display Graph*/
    for(i = 0;i<load_arr.size();i++)
    {
        load_value = load_arr[i]*g_graph_scaling;
        if(i != 0)
        {
            ps = p_new;
            p_new = Point((graph_start_pnt_x*(i+2)),graph_start_pnt_y-load_value);
            pe = p_new;
            if(ps.y != pe.y)
            {
                Point p3(ps.x, p_new.y);
                line(src, ps, p3,CLR_BLUE,2, LINE_8);
                line(src, p3, pe, CLR_BLUE,2, LINE_8);
            } 
            else
            {
                line(src, ps, pe,CLR_BLUE,2, LINE_8);
            }
            
        } 
        else 
        {
            p_new = Point(graph_start_pnt_x*2,graph_start_pnt_y-load_value);
            Point p0(graph_start_pnt_x, graph_start_pnt_y-load_value);
            line(src, p0, p_new,CLR_BLUE,2, LINE_8);
        }

    }

    return 0;
}
/**********************************************************************************************************************
 End of function graph_display
 *********************************************************************************************************************/

cv::VideoCapture video_file_capture;
/*********************************************************************************************************************/
/* Function Name : open_stored_video */
/**********************************************************************************************************************
 * @brief       Open video. Only use if camera is not available
 * @param[in]   video file path
 * @param[out]  status
 * @retval      true      success
 * @retval      false     fail
***********************************************************************************************************************/
int open_stored_video(char *video_filename)
{
    video_file_capture.open(video_filename);

    if (!video_file_capture.isOpened())
    {
        return FAILED;
    }

    return SUCCESS;
}
/**********************************************************************************************************************
 End of function open_stored_video
 *********************************************************************************************************************/

/*********************************************************************************************************************/
/* Function Name : close_stored_video */
/**********************************************************************************************************************
 * @brief       Close video file. Only close if camera is not available
 * @param[in]   void
 * @param[out]  void
***********************************************************************************************************************/
void close_stored_video()
{
    video_file_capture.release();

    cv::destroyAllWindows();
}
/**********************************************************************************************************************
 End of function close_stored_video
 *********************************************************************************************************************/

/*********************************************************************************************************************/
/* Function Name : f_opencv_execute */
/**********************************************************************************************************************
 * @brief       Display Inference
                [Covers: BPAP_FC_V4H_AD020][Covers: BPAP_FC_V4H_AD021][Covers: BPAP_FC_V4H_AD051]
 * @param[in]   none
 * @param[out]  none
 * @retval      true      success
 * @retval      false     fail
***********************************************************************************************************************/
int f_opencv_execute()
{
    cv::Mat image;
    cv::Mat depth_image;
    cv::Mat opt_img;
    int font_size = 1;
    Scalar font_Color_others;
    font_Color_others = cv::Scalar(200, 255);
    cv::Mat resized_image(FRAME_WIDTH, FRAME_HEIGHT, CV_8UC3);
    cv::Mat multiplane_rsz(MULTIPLANE_SCREEN_WIDTH, MULTIPLANE_SCREEN_HEIGHT , CV_8UC3);
    cv::Mat depth_img_resz(MULTIPLANE_SCREEN_WIDTH, MULTIPLANE_SCREEN_HEIGHT , CV_8UC3);
    cv::Mat opt_img_resz(MULTIPLANE_SCREEN_WIDTH, MULTIPLANE_SCREEN_HEIGHT , CV_8UC3);
    Mat status_display(MULTIPLANE_SCREEN_HEIGHT, MULTIPLANE_SCREEN_WIDTH, CV_8UC3, mul_plane_buffer_4);
    if (g_customize.Video_Read_Enable == false)
    {
        DEBUG_PRINT("OpenCV check : plane_count = %d\n",plane_count);
        if(plane_count == 4)
        {
            memset(mul_plane_buffer_4,255,(MULTIPLANE_SCREEN_WIDTH * MULTIPLANE_SCREEN_HEIGHT * 3));
            cv::putText(status_display, "FC Near Application Status", Point(320,30),2, font_size, CLR_BLACK, 2, true);
        }

        process_yuv(gp_opencv_in, image, PLN_1);
    }
    else
    {
        // video_file_capture >> image;
        video_file_capture.read(image);
        imshow("frontcam_demo", image);
    }

    //Point text_ai_fps_position(20, 110);
    if (0 != g_customize.Proc_Time)
    {
        char fps_text[20];
        char status_text[100];
        if(plane_count == 4)
        {
            /*Display FPS*/
            sprintf(status_text,"FPS :%3u.%1u", g_fps[eVOUT] / 10, g_fps[eVOUT] % 10);
            cv::putText(status_display, status_text, Point(20,70),2, font_size, CLR_BLUE, 2, true);

            /*Display CPU Load*/
            sprintf(status_text,"CPU Load : %0.2f %%",g_cpu_usage);
            cv::putText(status_display, status_text, Point(20,110), 2, font_size, CLR_BLUE, 2, true);

            /*Display model status*/
#if (CDNN)
            if ((true == g_customize.OBJ_DET_Enable) & (true == g_display_od_flag))
            {
                cv::putText(status_display, "Object Detection : ON", Point(20,150), 2, font_size, CLR_GREEN, 2, true);
            }
            else
            {
                cv::putText(status_display, "Object Detection : OFF", Point(20,150), 2, font_size, CLR_GRAY, 2, true);
            }
            if ((true == g_customize.SEM_SEG_Enable) & (true == g_display_ssg_flag))
            {
                cv::putText(status_display, "Semantic Segmentation : ON", Point(20,230), 2, font_size, CLR_GREEN, 2, true);
            }
            else
            {
                cv::putText(status_display, "Semantic Segmentation : OFF", Point(20,230), 2, font_size, CLR_GRAY, 2, true);
            }
#endif
        }
      
    }

    /* Display Graph*/
#if (CDNN)
    if (true == g_customize.CDNN_Load_Enable || true == g_customize.CPU_Load_Enable)
#else
    if (true == g_customize.CPU_Load_Enable) //TODO need to changed the comment on final integration
#endif
    {
        char cdnnloadtext[20];
        char cpuloadtext[20];
        Mat graph(360, 1280, CV_8UC3, gp_graph_buffer);
        Mat log(1080, 640, CV_8UC3, gp_log_buffer);
        memset(gp_log_buffer, 0, (LOG_SCREEN_WIDTH * LOG_SCREEN_HEIGHT * 3));
        int font_type = FONT_HERSHEY_PLAIN;

        float cpuload = g_cpu_usage;

        /*Calculate CDNN Load for V4H*/
        cdnnload = 100 * ((float)g_cnn_load_cnt / ((float)CNN_CYCLE_COUNT_MAX * (float)(g_exe_time / 1000)));
        avgcdnnload = ((avgcdnnload + (int)cdnnload) / 2);
        
        /*Fill cdnn load array*/
        if(cdnn_load_arr.size() >= 30)
        {
            cdnn_load_arr.erase(cdnn_load_arr.begin());
        }
        cdnn_load_arr.push_back((int)cdnnload);
        /*Display Graphs in each second*/
        if(true == g_load_flag)
        {
            float cpuload = g_cpu_usage;

            /*Fill cpu load array*/
            if(cpu_load_arr.size() > 18)
            {
                cpu_load_arr.erase(cpu_load_arr.begin());
            }
            cpu_load_arr.push_back((int)cpuload);

            /*Fill avg cdnn load array*/
            if(g_avg_cdnn_load_arr.size() > 18)
            {
                g_avg_cdnn_load_arr.erase(g_avg_cdnn_load_arr.begin());
            }
            g_avg_cdnn_load_arr.push_back(avgcdnnload);
            memset(gp_graph_buffer, 255, (1280 * 360 * 3));    /*Setting background as white*/   
#if (CDNN)
            if (true == g_customize.CDNN_Load_Enable && true == g_customize.CPU_Load_Enable)
            {
                g_graph_scaling = 1;
                g_graph_start = 150;
                g_max_load = 100;
                graph_display(graph, GRAPH_LABLE_CORE_CDNN, g_avg_cdnn_load_arr);

                g_graph_start = 320;
                graph_display(graph, GRAPH_LABLE_CORE_CPU, cpu_load_arr);
            }
     //       else
      //      {
#endif                

#if (CDNN)
                if (true == g_customize.CDNN_Load_Enable)
                {
                    g_graph_scaling = 2;
                    graph_display(graph, GRAPH_LABLE_CORE_CDNN, g_avg_cdnn_load_arr);
                }
#endif
                if  (true == g_customize.CPU_Load_Enable)
                {
                    g_graph_scaling = 2;
                    graph_display(graph, GRAPH_LABLE_CORE_CPU, cpu_load_arr); //TODO error hence commented 
                }
//           } 
            g_load_flag=false;
        }

        log_display(log);
#if (CDNN)
    if (true == g_customize.CDNN_Load_Enable && true == g_customize.CDNN_Enable)
    {
        sprintf(cdnnloadtext,"CDNN Load : %0.2f %%",cdnnload);
        DEBUG_PRINT("cdnnloadtext [%s] g_exe_time [%d]\n",cdnnloadtext,    g_exe_time);
        Point text_position_cdnn(20, 110);
        cv::putText(image, cdnnloadtext, text_position_cdnn, 2, font_size, font_Color_others, 2, true);
    }
    if (true == g_customize.CDNN_Load_Enable && false == g_customize.CDNN_Enable)
    {
        sprintf(cdnnloadtext,"CDNN Load : 0%% ");
        Point text_position_cdnn(20, 110);
        cv::putText(image, cdnnloadtext, text_position_cdnn,2, font_size, font_Color_others, 2, true);
    }
    if((true == g_customize.CPU_Load_Enable ) && (true == g_customize.CDNN_Load_Enable))
    {
        sprintf(cpuloadtext,"CPU Load : %0.2f %%",cpuload);
        Point text_position_cpu(20, 150);
        cv::putText(image, cpuloadtext, text_position_cpu, 2, font_size, font_Color_others, 2, true);
    }
    else
    {
        if(true == g_customize.CPU_Load_Enable )
        {
            sprintf(cpuloadtext,"CPU Load : %0.2f %%",cpuload);
            Point text_position_cpu(20, 110);
            cv::putText(image, cpuloadtext, text_position_cpu, 2, font_size, font_Color_others, 2, true);
        }
    }
#else
        if (true == g_customize.CPU_Load_Enable )
        {
            sprintf(cpuloadtext,"CPU Load : %0.2f %%",cpuload);
            Point text_position_cpu(20, 110);
            cv::putText(image, cpuloadtext, text_position_cpu, 2, font_size, font_Color_others, 2, true);
        }
#endif
    }
#if 0    
#if(CDNN)
    if((((false == g_customize.CPU_Load_Enable) || (false == g_customize.CDNN_Load_Enable)) && (true == g_customize.DEPTH_EST_Enable || true == g_customize.OPT_FLOW_Enable)) && 
        (true == g_customize.OBJ_DET_Enable || true == g_customize.POSE_EST_Enable || true == g_customize.SEM_SEG_Enable || true == g_customize.TRF_SGN_DET_Enable))
        
        {
            resize(image, multiplane_rsz, Size(960, 540));
            memcpy((void *)mul_plane_buffer_1, (void *)multiplane_rsz.data, 960 * 540 * 3);

            if (true == g_customize.DEPTH_EST_Enable)
            {
            resize(depth_image, depth_img_resz, Size(960, 540));
            memcpy((void *)mul_plane_buffer_2, (void *)depth_img_resz.data, 960 * 540 * 3);
            }
            if (true == g_customize.OPT_FLOW_Enable)
            {
            resize(opt_img, opt_img_resz, Size(960, 540));
            memcpy((void *)mul_plane_buffer_3, (void *)opt_img_resz.data, 960 * 540 * 3);
            }
        }
#endif
#endif
    Point text_fps_position(20, 30);
    Point text_ai_fps_position(20, 70);
    if (0 != g_customize.Proc_Time)
    {
        char fps_text[20];
        sprintf(fps_text, "Display FPS: %3u.%1u", g_fps[0] / 10, g_fps[0] % 10);
        cv::putText(image, fps_text, text_fps_position, 2, font_size, font_Color_others, 2, true);
        sprintf(fps_text,"Inference FPS :%3u.%1u", g_fps[1] / 10, g_fps[1] % 10);
        cv::putText(image, fps_text, text_ai_fps_position, 2, font_size, font_Color_others, 2, true);
    }
    memcpy((void *)gp_opencv_buffer, (void *)image.data, g_frame_width * g_frame_height * g_vout_pix_fmt);
    if (0 == g_customize.VOUT_Enable) {
        cv::Mat bgr_image(g_frame_width, g_frame_height, CV_8UC3);
        cv::cvtColor(image, bgr_image, COLOR_RGB2BGR);
        imshow("frontcam_demo", bgr_image);
        waitKey(1);
    }
    return SUCCESS;
}

int create_opencv_window() {
    namedWindow("frontcam_demo");
    resizeWindow("frontcam_demo", 1280, 720);
    return SUCCESS;
}

void self_destruct_ocv() {
    destroyWindow("frontcam_demo");
    //destroyAllWindows();
}

/**********************************************************************************************************************
 End of function f_opencv_execute
 *********************************************************************************************************************/
