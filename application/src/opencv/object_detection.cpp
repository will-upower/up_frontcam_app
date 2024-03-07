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

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <cstdio>
#include <sys/stat.h>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <array>
#include <iostream>
#include <cassert>
#include <vector>
#include <fstream>
#include <algorithm>

#include "object_detection.h"
#include "common.h"

using namespace std;

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define OBJ_WIDTH               (320)
#define OBJ_HEIGHT              (320)
#define CLS_START               (5)
#define MAX                     (6)
#define LAST_COL_SIZE           (7)
#define TH_NMS                  (0.5)

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
typedef struct
{
    float x, y, w, h;
} Box;

/**********************************************************************************************************************
 Global variables and functions
 *********************************************************************************************************************/
std::vector<std::vector<float>> obj_det_outbuff( OBJ_DET_OUT_COLOUMN , std::vector<float> (OBJ_DET_OUT_ROW)); 

extern "C" void obj_det (float objDet_output[OBJ_DET_OUT_COLOUMN][OBJ_DET_OUT_ROW]);
static void filter_boxes_nms (std::vector<std::vector <float>> &det, int nBoxes, float th_nms);
static float box_intersection (Box a, Box b);
static float box_iou (Box a, Box b);
static float box_union (Box a, Box b);
static float overlap (float x1, float w1, float x2, float w2);
static void filter_boxes_nms (std::vector<std::vector <float>> &det, int nBoxes, float th_nms);

/**********************************************************************************************************************
/* Function Name :  obj_det */
/******************************************************************************************************************//**
 * @brief       Object detection post process steps
 * @param[in]   objDet_output    
 * @param[out]  none
 * @retval      none
 ***********************************************************************************************************************/
void obj_det(float objDet_output[OBJ_DET_OUT_COLOUMN][OBJ_DET_OUT_ROW])
{
    int data_len    = 0;
    bool break_flag = false;
    obj_det_outbuff.resize(OBJ_DET_OUT_COLOUMN, vector<float> (OBJ_DET_OUT_ROW, -1));
    
    for (auto i = 0; i < OBJ_DET_OUT_COLOUMN; i++) 
    {   
        for (auto j = 0; j < OBJ_DET_OUT_ROW; j++) 
        {
            obj_det_outbuff[i][j] = objDet_output[i][j];
            data_len++; 

            if (OBJ_DET_OUTPUT_LEN <= data_len)
            {
                break_flag = true;
                break;
            }
        }
    
        if(break_flag)
        {
            break_flag = false;
            break;
        }
    }

    // Define a lambda function to check if an element is greater than zero.
    auto greater_than_zero = [](float value) { return value > 0; };

    // Create a vector to store the indices of elements to be erased.
    std::vector<int> elements_to_remove;

    for (int i = 0; i < obj_det_outbuff.size(); i++)
    {
        int   flag = 0;
        float cls  = 0;
        float max  = obj_det_outbuff[i][5];

        // Use std::max_element to find the maximum element and its index efficiently.
        auto max_element_index = std::max_element(obj_det_outbuff[i].begin() + CLS_START, obj_det_outbuff[i].end());

        if (max_element_index != obj_det_outbuff[i].end() && *max_element_index > max)
        {
            max = *max_element_index;
            cls = std::distance(obj_det_outbuff[i].begin() + CLS_START, max_element_index);
        }

        // Count the number of elements greater than zero using std::count_if.
        flag = std::count_if(obj_det_outbuff[i].begin() + CLS_START, obj_det_outbuff[i].end(), greater_than_zero);

        obj_det_outbuff[i][6] = max;
        obj_det_outbuff[i][5] = cls;

        if (flag == 0)
        {
            elements_to_remove.push_back(i);
        }
    }

    // Erase the elements from the vector in reverse order to avoid invalidating other indices.
    for (int i = elements_to_remove.size() - 1; i >= 0; i--)
    {
        obj_det_outbuff.erase(obj_det_outbuff.begin() + elements_to_remove[i]);
    }

    for (int i = 0; i < obj_det_outbuff.size(); i++)
    {
        if (obj_det_outbuff[i].size() > MAX)
        {
            for (int j = LAST_COL_SIZE; j < obj_det_outbuff[i].size(); j++)
            {
                obj_det_outbuff[i].erase(obj_det_outbuff[i].begin() + j);
                --j;
            }
        }
    }

    filter_boxes_nms(obj_det_outbuff, obj_det_outbuff.size(), TH_NMS);

    for (int i = 0; i < obj_det_outbuff.size(); i++)
    {      
        if(obj_det_outbuff[i][6] == 0)
        {
            obj_det_outbuff.erase(obj_det_outbuff.begin() + i);
            --i;
        }
    }

    for (int i = 0; i < obj_det_outbuff.size(); i++)
    {     
        int k = obj_det_outbuff[i][5];
           
        float x  = obj_det_outbuff[i][0] - obj_det_outbuff[i][2] / 2;
        float y  = obj_det_outbuff[i][1] - obj_det_outbuff[i][3] / 2;
        float x1 = obj_det_outbuff[i][0] + obj_det_outbuff[i][2] / 2;
        float y1 = obj_det_outbuff[i][1] + obj_det_outbuff[i][3] / 2; 

        float gain_2 = float(g_customize.OBJ_DET_Height) / float(g_customize.Frame_Height);
        float gain_1 = float(g_customize.OBJ_DET_Width) / float(g_customize.Frame_Width);
        float pad0   = (float(g_customize.OBJ_DET_Width) - float(g_customize.Frame_Width) * gain_1) / 2;
        float pad1   = (float(g_customize.OBJ_DET_Height) - float(g_customize.Frame_Height) * gain_2) / 2;

        /* copy inference result to global struct */
        _bbox_cor[i].cls     = obj_det_outbuff[i][5];
        _bbox_cor[i].start.x = x;
        _bbox_cor[i].start.y = y;
        _bbox_cor[i].end.x   = x1;
        _bbox_cor[i].end.y   = y1;    
    }

    detObjCount = obj_det_outbuff.size();
}
/**********************************************************************************************************************
 * End of function obj_det function
 ********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name :  overlap */
/******************************************************************************************************************//**
 * @brief       Function to compute the overlapped data  between coordinate x with size w
 * @param[in]   x1 = 1-dimensional coordinate of first line    
 * @param[in]   w1 = size of fist line
 * @param[in]   x2 = 1-dimensional coordinate of second line    
 * @param[in]   w2 = size of second line
 * @param[out]  none
 * @retval      overlapped line size
 ***********************************************************************************************************************/
static float overlap(float x1, float w1, float x2, float w2)
{
    float l1    = x1 - w1 / 2;
    float l2    = x2 - w2 / 2;
    float left  = l1 > l2 ? l1 : l2;
    float r1    = x1 + w1 / 2;
    float r2    = x2 + w2 / 2;
    float right = r1 < r2 ? r1 : r2;
    return right - left;
}
/**********************************************************************************************************************
 * End of function overlap function
 ********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name :  filter_boxes_nms */
/******************************************************************************************************************//**
 * @brief       Apply nms to get rid of overlapped rectangles.
 * @param[in]   nBoxes = number of detections stored in det   
 * @param[in]   th_nms = threshold for nms
 * @param[out]  det= detected rectangles
 * @retval      overlapped line size
 ***********************************************************************************************************************/
static void filter_boxes_nms(std::vector<std::vector <float>> &det, int nBoxes, float th_nms)
{
    int count = nBoxes;
    float b_intersection;
    for (int i = 0; i < count; i++)
    {
        Box a = {det[i][0], det[i][1], det[i][2], det[i][3]};         
        for (int j = 0; j < count; j++)
        {
            if (i == j)
            {
                continue;
            }
            if (det[i][5] != det[j][5])
            {
                continue;
            }
            Box b = {det[j][0], det[j][1], det[j][2], det[j][3]};
            b_intersection = box_intersection(a, b);
            if((box_iou(a, b) > th_nms) || (b_intersection >= a.h * a.w - 1) || (b_intersection >= b.h * b.w - 1))
            {
                if (det[i][6] > det[j][6])
                {
                    det[j][6] = 0;
                }
                else
                {
                    det[i][6] = 0;
                }
            }
        }
    }
}
/**********************************************************************************************************************
 * End of function filter_boxes_nms function
 ********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name :  box_intersection */
/******************************************************************************************************************//**
 * @brief       Function to compute the area of intersection of Box a and b
 * @param[in]   a = Box 1  
 * @param[in]   b = Box 2
 * @param[out]  none
 * @retval      area of intersection
 ***********************************************************************************************************************/
static float box_intersection(Box a, Box b)
{
    float w = overlap(a.x, a.w, b.x, b.w);
    float h = overlap(a.y, a.h, b.y, b.h);
    if(w < 0 || h < 0)
    {
        return 0;
    }
    float area = w * h;
    return area;
}
/**********************************************************************************************************************
 * End of function box_intersection function
 ********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name :  box_union */
/******************************************************************************************************************//**
 * @brief       Function to compute the area of union of Box a and b
 * @param[in]   a = Box 1  
 * @param[in]   b = Box 2
 * @param[out]  none
 * @retval      area of union
 ***********************************************************************************************************************/
static float box_union(Box a, Box b)
{
    float i = box_intersection(a, b);
    float u = a.w * a.h + b.w * b.h - i;
    return u;
}
/**********************************************************************************************************************
 * End of function box_union function
 ********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name :  box_iou */
/******************************************************************************************************************//**
 * @brief       Function to compute the Intersection over Union (IoU) of Box a and b
 * @param[in]   a = Box 1  
 * @param[in]   b = Box 2
 * @param[out]  none
 * @retval      IoU
 ***********************************************************************************************************************/
static float box_iou(Box a, Box b)
{
    return box_intersection(a, b) / box_union(a, b);
}
/**********************************************************************************************************************
 * End of function box_iou function
 ********************************************************************************************************************/

 /**********************************************************************************************************************
 * End of file
 ********************************************************************************************************************/
