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
* File Name    : vout_wrapper.h
* Version      : 1.0.0
* Description  : wrapper header for vout in linux
*********************************************************************************************************************/
/*********************************************************************************************************************
* History : Version DD.MM.YYYY Description
*         : 0.1.0   27.09.2022 First Release
*         : 1.0.0   06.03.2023 Updated as per Renesas coding guidelines
*********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef _VOUTWRAPPER_H_
#define _VOUTWRAPPER_H_

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "vout_display_linux.h"

/**********************************************************************************************************************
 Function Declarations
*********************************************************************************************************************/
int wr_vout_open (const char * device, const char * module);
struct resources * wr_vout_getresources (struct device * dev);
void wr_vout_close (int fd);
bool wr_vout_set_property (struct device * dev, struct property_arg * p);
void wr_vout_clear_planes (struct device * dev, struct plane_arg * p);
void wr_vout_free_resources (struct resources * res);

int wr_vout_setPlane (struct device * dev, struct plane_arg * p);
int wr_vout_control (int fd, unsigned long request, void * arg);
int wr_vout_create_frameBuffer (int fd, unsigned int fourcc, const uint32_t w, const uint32_t h, uint32_t handles[4], uint32_t pitches[4], uint32_t offsets[4], unsigned int * fb_id, unsigned int flags);
int wr_vout_drmGetCap (int fd, uint64_t capability, uint64_t * value);
int wr_vout_drmModeSetCrtc (int fd, uint32_t crtc_id, uint32_t fb_id, uint32_t x, uint32_t y, uint32_t * connectors, int count, struct pipe_arg * pipe);
int wr_vout_drmModeDirtyFB (int fd, uint32_t bufferid, void * clips, uint32_t num_clips);

#endif
