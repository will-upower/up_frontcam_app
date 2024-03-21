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
* File Name    : vout_wrapper.c
* Version      : 1.0.0
* Description  : wrapper for vout in linux
*********************************************************************************************************************/
/*********************************************************************************************************************
* History : Version DD.MM.YYYY Description
*         : 0.1.0   27.09.2022 First Release
*         : 1.0.0   06.03.2023 Updated as per Renesas coding guidelines
*********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdbool.h>
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/stdio.h>
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/stdlib.h>
#include <stdint.h>
#include "vout_wrapper.h"

/******************************************************************************************************************//**
/* Function Name : wr_vout_open */
/******************************************************************************************************************//**
 * @brief       drm device open
                [Covers: BPAP_FC_V4H_AD022]
 * @param[in]   device
 * @param[in]   module
 * @param[out]  none                 
 * @retval      ret
 *********************************************************************************************************************/
int wr_vout_open(const char * device, const char * module)
{
    int ret = INVALID;
    ret = R_Vout_open(device, module);                                 /* Opens device */

    return (ret);
}
/**********************************************************************************************************************
 End of function wr_vout_open
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : wr_vout_getresources */
/******************************************************************************************************************//**
 * @brief       drm get resources
                [Covers: BPAP_FC_V4H_AD020]
 * @param[in]   dev
 * @param[out]  none                 
 * @retval      res
 *********************************************************************************************************************/
struct resources * wr_vout_getresources(struct device * dev)
{
    struct resources * res;

    res = R_Vout_GetResources(dev);                                 /* DRM get resources */

    return (res);
}
/**********************************************************************************************************************
 End of function wr_vout_getresources
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : wr_vout_close */
/******************************************************************************************************************//**
 * @brief       drm device close
 * @param[in]   fd
 * @param[out]  none                 
 * @retval      none
 *********************************************************************************************************************/
void wr_vout_close(int fd)
{
    R_Vout_Close(fd);                                                  /* Closes device */

}
/**********************************************************************************************************************
 End of function wr_vout_close
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : wr_vout_set_property */
/******************************************************************************************************************//**
 * @brief       drm set property
 * @param[in]   dev
 * @param[in]   p
 * @param[out]  none                 
 * @retval      ret
 *********************************************************************************************************************/
bool wr_vout_set_property(struct device * dev, struct property_arg * p)
{
    bool ret = INVALID;
    ret = R_Vout_Set_Property(dev, p);                                 /* DRM set property */

    return (ret);
}
/**********************************************************************************************************************
 End of function wr_vout_set_property
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : wr_vout_clear_planes */
/******************************************************************************************************************//**
 * @brief       drm clear planes
                [Covers: BPAP_FC_V4H_AD020]
 * @param[in]   dev
 * @param[in]   p
 * @param[out]  none                 
 * @retval      none
 *********************************************************************************************************************/
void wr_vout_clear_planes(struct device * dev, struct plane_arg * p)
{
    R_Vout_Clear_Planes(dev, p);                                       /* Clear planes */

}
/**********************************************************************************************************************
 End of function wr_vout_clear_planes
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : wr_vout_free_resources */
/******************************************************************************************************************//**
 * @brief       drm free resources
 * @param[in]   res
 * @param[out]  none                 
 * @retval      none
 *********************************************************************************************************************/
void wr_vout_free_resources(struct resources * res)
{
    R_Vout_Free_Resources(res);                                       /* Free resources */

}
/**********************************************************************************************************************
 End of function wr_vout_free_resources
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : wr_vout_setPlane */
/******************************************************************************************************************//**
 * @brief       drm set plane
 * @param[in]   dev
 * @param[in]   p
 * @param[out]  none                 
 * @retval      ret
 *********************************************************************************************************************/
int wr_vout_setPlane(struct device * dev, struct plane_arg * p)
{
    int ret = INVALID;
    ret = R_Vout_SetPlane(dev, p);                                 /* DRM set plane */
    return (ret);
}
/**********************************************************************************************************************
 End of function wr_vout_setPlane
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : wr_vout_control */
/******************************************************************************************************************//**
 * @brief       drm control
 * @param[in]   fd
 * @param[in]   request
 * @param[in]   arg
 * @param[out]  none                 
 * @retval      ret
 *********************************************************************************************************************/
int wr_vout_control(int fd, unsigned long request, void * arg)
{
    int ret = INVALID;
    ret = R_Vout_control(fd, request, arg);                        /* DRM control */

    return (ret);

}
/**********************************************************************************************************************
 End of function wr_vout_control
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : wr_vout_create_frameBuffer */
/******************************************************************************************************************//**
 * @brief       frame buffer creation for Vout
 * @param[in]   fd
 * @param[in]   w           width
 * @param[in]   h           height
 * @param[in]   fourcc      frame buffer pixel format
 * @param[in]   handles     buffer handles
 * @param[in]   pitches     pitch of each plane
 * @param[in]   offsets     offset of each plane
 * @param[in]   fb_id       Id of buffer frame
 * @param[in]   flags
 * @param[out]  none                 
 * @retval      ret
 *********************************************************************************************************************/
int wr_vout_create_frameBuffer(int fd, const uint32_t w, const uint32_t h, unsigned int fourcc, uint32_t handles[4], uint32_t pitches[4], uint32_t offsets[4], unsigned int *fb_id, unsigned int flags)
{
    int ret = INVALID;

    ret = R_Vout_Create_FrameBuffer(fd, w, h, fourcc, handles, pitches, offsets, fb_id, flags); /* DRM mode buffer creation */

    return ret;
}
/**********************************************************************************************************************
 End of function wr_vout_create_frameBuffer
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : wr_vout_drmGetCap */
/******************************************************************************************************************//**
 * @brief       capability of drm
 * @param[in]   fd
 * @param[in]   capability
 * @param[in]   value
 * @param[out]  none                 
 * @retval      ret
 *********************************************************************************************************************/
int wr_vout_drmGetCap(int fd, uint64_t capability, uint64_t *value)
{
    int ret = INVALID;

    ret = R_Vout_drmGetCap(fd, capability, value);

    return ret;
}
/**********************************************************************************************************************
 End of function wr_vout_drmGetCap
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : wr_vout_drmModeSetCrtc */
/******************************************************************************************************************//**
 * @brief       setting crtc mode of vout
 * @param[in]   fd              
 * @param[in]   crtc_id         Id of crtc
 * @param[in]   fb_id           Id of frame buffer
 * @param[in]   x               Position of frame buffer
 * @param[in]   y               Position of frame buffer
 * @param[in]   connectors      
 * @param[in]   count
 * @param[in]   pipe
 * @param[out]  none                 
 * @retval      ret
 *********************************************************************************************************************/
int wr_vout_drmModeSetCrtc(int fd, uint32_t crtc_id, uint32_t fb_id, uint32_t x, uint32_t y, uint32_t * connectors, int count, struct pipe_arg *pipe)
{

    int ret = INVALID;

    ret = R_vout_drmModeSetCrtc(fd, crtc_id, fb_id, x, y, connectors, count, pipe);

    return (ret);
}
/**********************************************************************************************************************
 End of function wr_vout_drmModeSetCrtc
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : wr_vout_drmModeDirtyFB */
/******************************************************************************************************************//**
 * @brief       setting drm mode as Dirty frame buffer
 * @param[in]   fd
 * @param[in]   bufferid
 * @param[in]   clips
 * @param[in]   num_clips
 * @param[out]  none                 
 * @retval      ret
 *********************************************************************************************************************/
int wr_vout_drmModeDirtyFB(int fd, uint32_t bufferid, void * clips, uint32_t num_clips)
{
    int ret = INVALID;
    ret = R_vout_drmModeDirtyFB(fd, bufferid, clips, num_clips);
    return ret;
}
/**********************************************************************************************************************
 End of function wr_vout_drmModeDirtyFB
 *********************************************************************************************************************/
