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
* File Name    : opencv.h
* Version      : 0.1.0
* Description  : Header File
*********************************************************************************************************************/
/*********************************************************************************************************************
* History : Version DD.MM.YYYY Description
*         : 0.1.0   06.03.2023 Alpha Release
*********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#ifndef OPENCV_H_
#define OPENCV_H_

#include "common.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#define ACT_STR_MAXLEN  (25)
#define MAX_ACT_NO      (10)
#define START_X 60
#define END_X 90
#define PLOT_GAP 20
#define END_Y 150
#define RANGE 100
#define START_Y END_Y-RANGE
#define CORE_CNT 6
#define MUL 2
#define CLR_BLACK Scalar(0,0,0)
#define CLR_WHITE Scalar(255,255,255)
#define CLR_RED Scalar(255, 0, 0)
#define CLR_YELLOW Scalar(255, 255, 0)
#define CLR_BLUE Scalar(0, 0, 255)
#define CLR_PURPLE Scalar(255, 0, 255)
#define CLR_GRAY Scalar(150, 150,150)

#endif