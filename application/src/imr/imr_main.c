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
 * File Name    : imr_main.c
 * Version      : 0.1.0
 * Description  :This software package demonstrates how input image texture is
 *               rendered using IMR LX4(V3U) or LX5(V4H) by means of the
 *               bandwidth-optimizing display list generation library .
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 06.03.2023 0.1.0     Alpha Release (Base: R-CAR SDK V2.17 sample)
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

/* IMR Driver, OSAL and DLGen library */
#include "rcar-xos/imr/r_imrdrv_api.h"
#include "rcar-xos/imr/r_imrdrv_type.h"
#include "rcar-xos/imrdlg/r_imrdlg.h"
#include "rcar-xos/osal/r_osal.h"
#include "imr_main.h"
#include "reg_settings.h"

/* Local includes */
#include "common.h"
#include "customize.h"
#include "custom_map.h"
#include "osaldrv_helper.h"                 /*abstracts the OSAL/driver init overhead*/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define MESH_SIZE           (4)
#define MESH_SIZE_LIMIT     (0)
#define IMR_SRC_ALIGN       (64U)
#define IMR_DST_ALIGN       (64U)
#define IMR_DL_ALIGN        (256U)         /* according to driver (real HW differs !)*/
#define IMR_STRIDE_RS       (224)
#define VAL_ZERO            (0)
#define VAL_ONE             (1)
#define BUF_MAX             (18)

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
typedef enum                               /* Enum for pixel format*/ 
{
    PIXFMT_Y12 = 0, 
    PIXFMT_Y8, 
    PIXFMT_UV8, 
    PIXFMT_UV8_422, 
    PIXFMT_YUYV, 
    PIXFMT_UYVY, 
    PIXFMT_Y10, 
    PIXFMT_Y16, 
} pixfmt_e;

enum                                        /* Test pattern to use as input */
{
    INPUT_BLACK, 
    INPUT_WHITE, 
    INPUT_GRAY50, 
    INPUT_HBAR, 
    INPUT_VBAR, 
    INPUT_HVBAR, 
    INPUT_HLINES, 
    INPUT_VLINES, 
    INPUT_HBARSTRIPES, 
    INPUT_VBARSTRIPES, 
    INPUT_CHECKER, 
    INPUT_CHECKER_HVBAR
};

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
e_imrdrv_errorcode_t        ret_imr;
osal_memory_buffer_handle_t buf[BUF_MAX];

/**********************************************************************************************************************
 Static variables and functions 
 *********************************************************************************************************************/
static int    mesh_size                     = MESH_SIZE;
static int    mesh_size_limit               = MESH_SIZE_LIMIT;                             /* use library default */
static        pixfmt_e pixfmt               = PIXFMT_YUYV;
static p_imrdlg_mapping_function_t map_func = NULL;
static void * map_func_data                 = NULL;
static int    inputWidth                    = INVALID;
static int    inputHeight                   = INVALID;
static int    outputWidth                   = INVALID;
static int    outputHeight                  = INVALID;
static int    enable_prefetch               = 1;
static undistort_params_t opencv_coeff      = {VAL_ZERO, VAL_ZERO, VAL_ZERO, VAL_ZERO, VAL_ZERO, VAL_ZERO, VAL_ZERO, 
                                              VAL_ZERO, VAL_ZERO, VAL_ZERO, VAL_ZERO};
static map_params_t map_coeff               = {VAL_ZERO, VAL_ZERO};
static addr_t src_addr_ldc                  = VAL_ZERO;
static addr_t dst_addr_ldc                  = VAL_ZERO; 
static addr_t dl_addr_ldc                   = VAL_ZERO; 
static addr_t src_addr_scaling[IMR_CH_MAX] = {VAL_ZERO};
static addr_t dst_addr_scaling[IMR_CH_MAX] = {VAL_ZERO};
static addr_t dl_addr_scaling               = VAL_ZERO;
static st_imrdrv_attr_param_t      imr_param_ldc;                                            /* Channel 0 params */
static st_imrdrv_attr_param_t      imr_param_scaling_0;
static st_imrdrv_attr_param_t      imr_param_scaling_1;
static st_imrdrv_attr_param_t      imr_param_scaling_2;
static st_imrdrv_attr_param_t      imr_param_scaling_3; 
static st_imrdrv_attr_param_t      imr_param_scaling_4;                                                      
static osal_memory_buffer_handle_t dl_buffer_scaling_0;
static osal_memory_buffer_handle_t dl_buffer_scaling_1;
static osal_memory_buffer_handle_t dl_buffer_scaling_2;
static osal_memory_buffer_handle_t dl_buffer_scaling_3;
static osal_memory_buffer_handle_t dl_buffer_scaling_4;
static osal_memory_buffer_handle_t dl_buffer_ldc;

static int R_IMR_AllocImage (st_imrdrv_data_t * img, osal_memory_buffer_handle_t * buf, addr_t * virt_addr, 
                                            int imr_channel, uint32_t width, uint32_t height, pixfmt_e fmt);
static int R_IMR_AllocDl (st_imrdrv_dl_t * dl, osal_memory_buffer_handle_t * buf, addr_t * virt_addr, int imr_channel, 
                                                                                                 uint32_t alloc_size);
static int R_IMR_TriangleMode (st_imrdrv_triangle_mode_t * triangle_mode);
static int imr_SetupResize_ch0(void);
static int imr_SetupResize_ch1(void);
static int imr_SetupResize_ch2(void);
static int imr_SetupResize_ch3(void);
static int imr_SetupResize_ch4(void);
static int imr_ExecuteResize_ch0(void);
static int imr_ExecuteResize_ch1(void);
static int imr_ExecuteResize_ch2(void);
static int imr_ExecuteResize_ch3(void);
static int imr_ExecuteResize_ch4(void);

/**********************************************************************************************************************
 Function Declarations
*********************************************************************************************************************/
int R_IMR_Init (void);
int R_IMR_SetupLDC (void);
int R_IMR_SetupResize (void);
int R_IMR_ExecuteLDC (void);
int R_IMR_ExecuteResize (void);
int R_IMR_Deinit (void);
int R_IMR_AllocImageLDC (void);
int R_IMR_AllocImageResize (void);

/******************************************************************************************************************//**
/* Function Name : R_IMR_AllocImage */
/******************************************************************************************************************//**
 * @brief       Allocates memory to IMR image
                [Covers: BPAP_FC_V4H_AD011]
 * @param[in]   buf                     ptr to MMGR buffer handle
 * @param[in]   imr_channel             Index of IMR channel (req. since physical address uses AXI ID)
 * @param[in]   width                   Width in pixels
 * @param[in]   height                  Height in pixels
 * @param[in]   fmt                     Pixelfmt
 * @param[in]   virt_addr               Specifies virtual address
 * @param[out]  img                     ptr to IMR driver data structure
 * @retval      true                    success
 * @retval      false                   fail
 *********************************************************************************************************************/
static int R_IMR_AllocImage(st_imrdrv_data_t * img, osal_memory_buffer_handle_t * buf, addr_t * virt_addr, 
                                                    int imr_channel, uint32_t width, uint32_t height, pixfmt_e fmt)
{   
    uintptr_t img_ptr;
    size_t align;
    uint32_t alloc_height;
    uint8_t bytes_per_pixel;
    size_t alloc_size;

    if (!img)
    {
        PRINT_ERROR("Missing img parameter\n");
        return FAILED;
    }

    switch (fmt)
    {
        case PIXFMT_YUYV:                                                       /* YUYV pixel format */
            img->bpp          = IMRDRV_BPP_8;                                   /* Set bytes per pixel */
            img->color_format = IMRDRV_COLOR_FORM_YUYV;                         /* Set color format */
            break;

        default:
            PRINT_ERROR("Unsupported pixel format %d\n", fmt);
            return FAILED;
    }

    img->width  = width;                                                        /* Set image width */ 
    img->height = height;                                                       /* Set image height */

    align             = IMR_SRC_ALIGN;
    alloc_height    = img->height;
    bytes_per_pixel = (img->bpp > IMRDRV_BPP_8) ? ((img->bpp == IMRDRV_BPP_32) ? 4 : 2) : 1;

    if (img->width  == IMR_STRIDE_RS)                                           /* Stride in pixels */
    {
        img->stride = IMR_STRIDE_RS;
    }
    else
    {
        img->stride = (((((img->width * bytes_per_pixel) - 1) / (uint32_t)align) + 1) * (uint32_t)align);
    }

    alloc_size        = (size_t)img->stride * (size_t)alloc_height;      /* size allocation of image */
    alloc_size              *= BPP_YUV;                                         /* For YUV */
    DEBUG_PRINT("Alloc_size = [%ld]\n", alloc_size);

    /* allocate buffer via MMGR */
	int res = mmgr_helper_alloc_buffer(buf, imr_channel, (uint32_t)alloc_size, (uint32_t)align, virt_addr, &img_ptr);  
    if (res)
    {
        PRINT_ERROR("Failed to allocate %d bytes for image with size %d x %d fmt %d align %d\n", 
                    (uint32_t)alloc_size, img->width, img->height, (uint32_t)fmt, (uint32_t)align);
        return FAILED;
    }

    img->phys_addr  = (uint32_t)img_ptr;                                        /*updating physical address */
    DEBUG_PRINT("Color Format = %d\n", img->color_format);
    return SUCCESS;
}
/**********************************************************************************************************************
 End of function R_IMR_AllocImage
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : R_IMR_AllocDl */
/******************************************************************************************************************//**
 * @brief       Allocates display list storage.
                [Covers: BPAP_FC_V4H_AD011]
 * @param[in]   virt_addr               Specifies virtual address
 * @param[in]   buf                     Pointer to memory manager buffer handle
 * @param[in]   imr_channel             Index of IMR channel (req. since physical address uses AXI ID)   
 * @param[in]   alloc_size              Specifies the size of memory to be allocated
 * @param[out]  dl                      Pointer to display list data structure 
 * @retval      true                    success
 * @retval      false                   fail
 *********************************************************************************************************************/
static int R_IMR_AllocDl(st_imrdrv_dl_t * dl, osal_memory_buffer_handle_t * buf, addr_t* virt_addr, int imr_channel, 
                                                                                                 uint32_t alloc_size)
{
    uintptr_t dl_ptr;

    if (!dl)
    {
        PRINT_ERROR("Missing img parameter\n");
        return FAILED;
    }

     /* allocate buffer via MMGR */
	int res = mmgr_helper_alloc_buffer(buf, imr_channel, alloc_size, IMR_DL_ALIGN, virt_addr, &dl_ptr);        
    
    if ((res) || (dl_ptr > 0xFFFFFFFF) || ((*virt_addr) == 0))
    {
        PRINT_ERROR("Failed to allocate %d bytes for dl\n", alloc_size);
        return FAILED;
    }

    dl->phys_addr = (uint32_t)dl_ptr;                            /* updating physical address of displaylist */
    
    return SUCCESS;
}
/**********************************************************************************************************************
 End of function R_IMR_AllocDl
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : R_IMR_TriangleMode */
/******************************************************************************************************************//**
 * @brief       Intitialize IMR triangle mode
 * @param[in]   none                    
 * @param[out]  triangle_mode               Structure containing triangle mode data.
 * @retval      true                        success
 * @retval      false                       fail
 *********************************************************************************************************************/
static int R_IMR_TriangleMode(st_imrdrv_triangle_mode_t * triangle_mode)
{

    if (!triangle_mode)
    {
        PRINT_ERROR("Missing img parameter\n");
        return FAILED;
    }

    /* init triangle mode */
    triangle_mode->uvshfval = IMRDRV_MODE_OFF;
    triangle_mode->shfval   = IMRDRV_MODE_OFF;
    triangle_mode->uvshfe   = IMRDRV_MODE_OFF;
    triangle_mode->shfe     = IMRDRV_MODE_OFF;
    triangle_mode->rde      = IMRDRV_MODE_OFF;
    triangle_mode->tfe      = IMRDRV_MODE_OFF;
    triangle_mode->tcm      = IMRDRV_MODE_OFF;
    triangle_mode->autosg   = IMRDRV_MODE_OFF;
    triangle_mode->autodg   = IMRDRV_MODE_OFF;
    triangle_mode->bfe      = IMRDRV_MODE_ON;
    triangle_mode->tme      = IMRDRV_MODE_ON;

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function R_IMR_TriangleMode
/******************************************************************************************************************//**
 
/******************************************************************************************************************//**
/* Function Name : R_IMR_Init */
/******************************************************************************************************************//**
 * @brief       IMR module initialisation
                [Covers: BPAP_FC_V4H_AD011][Covers: BPAP_FC_V4H_AD037][Covers: BPAP_FC_V4H_AD049]
 * @param[in]   none      
 * @param[out]  none
 * @retval      true                    success
 * @retval      false                   fail
 *********************************************************************************************************************/
int R_IMR_Init(void)
{
    int ret = INVALID;
    e_osal_return_t ret_osal;

    inputWidth      = g_frame_width;
    inputHeight     = g_frame_height;
    outputWidth     = g_frame_width;
    outputHeight    = g_frame_height;

    ret = init_osal();                                  /* osal initialisation */

    if (ret) 
    {
        PRINT_ERROR("Failed init_osal \n");
        return FAILED;
    }
    
    ret = init_imrdrv();                                /* imr driver initialization */

    if (ret)
    {
        PRINT_ERROR("Failed init_imrdrv \n");
        return FAILED;
    }

    if (true == g_customize.IMR_LDC)
    {
        ret = R_IMR_AllocImageLDC();                    /* image allocation for IMR LDC */

        if (FAILED == ret)
        {
            g_fcStatus.imr_ldc.status = FAILED;
            PRINT_ERROR("Failed R_IMR_AllocImageLDC \n");
            return FAILED;
        }
        else
        {
            g_fcStatus.imr_ldc.status = SUCCESS;
        }

        set_syncstatus(eIMR_LDC, 0);
    }

    if (true == g_customize.IMR_Resize)
    {
        ret = R_IMR_AllocImageResize();                 /* image allocation for IMR Resize */

        if (FAILED == ret)
        {
            g_fcStatus.imr_rs.status = FAILED;
            PRINT_ERROR("Failed R_IMR_AllocImageResize \n");
            return FAILED;
        }
        else
        {
            g_fcStatus.imr_rs.status = SUCCESS;
        }
        
        if (g_customize.IMR_Ch_0_Enable)
        {
            memset(src_addr_scaling[IMR_CH_0], 0, (g_frame_width * g_frame_height * BPP_YUV));
            memset(gp_imr_rs_buffer_ch0, 0, (g_customize.IMR_Resize_Width_Ch_0 * g_customize.IMR_Resize_Height_Ch_0 * 
			BPP_YUV));
        }
        if (g_customize.IMR_Ch_1_Enable)
        {    
            memset(src_addr_scaling[IMR_CH_1], 0, (g_frame_width * g_frame_height * BPP_YUV));
            memset(gp_imr_rs_buffer_ch1, 0, (g_customize.IMR_Resize_Width_Ch_1 * g_customize.IMR_Resize_Height_Ch_1 * 
			BPP_YUV));
        }
        if (g_customize.IMR_Ch_2_Enable)
        {   
            memset(src_addr_scaling[IMR_CH_2], 0, (g_frame_width * g_frame_height * BPP_YUV));
            memset(gp_imr_rs_buffer_ch2, 0, (g_customize.IMR_Resize_Width_Ch_2 * g_customize.IMR_Resize_Height_Ch_2 * 
			BPP_YUV));
        }
        if (g_customize.IMR_Ch_3_Enable)
        {   
            memset(src_addr_scaling[IMR_CH_3], 0, (g_frame_width * g_frame_height * BPP_YUV));
            memset(gp_imr_rs_buffer_ch3, 0, (g_customize.IMR_Resize_Width_Ch_3 * g_customize.IMR_Resize_Height_Ch_3 * 
			BPP_YUV));
        }
        if (g_customize.IMR_Ch_4_Enable)
        {    
            memset(src_addr_scaling[IMR_CH_4], 0, (g_frame_width * g_frame_height * BPP_YUV));
            memset(gp_imr_rs_buffer_ch4, 0, (g_customize.IMR_Resize_Width_Ch_4 * g_customize.IMR_Resize_Height_Ch_4 * 
			BPP_YUV));
        }
 
        set_syncstatus(eIMR_RS, 0);
    }

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function R_IMR_Init
/******************************************************************************************************************//**
 
/******************************************************************************************************************//**
/* Function Name : R_IMR_AllocImageLDC */
/******************************************************************************************************************//**
 * @brief       Allocate image for IMR LDC
                [Covers: BPAP_FC_V4H_AD017][Covers: BPAP_FC_V4H_AD034]
 * @param[in]   none
 * @param[out]  none
 * @retval      true                    success
 * @retval      false                   fail
 *********************************************************************************************************************/
int R_IMR_AllocImageLDC(void)
{
    int ret = INVALID;

    /* IMR_AllocImage for IMR source data when LDC enabled */
    ret = R_IMR_AllocImage(&imr_param_ldc.src_data, &buf[0], &src_addr_ldc, IMR_CH_0, (uint32_t)inputWidth, 
                        (uint32_t)inputHeight, (uint32_t)pixfmt);
    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_AllocImage LDC ret=%d\n", ret);
        return FAILED;
    }
    
    /* IMR_AllocImage for IMR destination data when LDC enabled */
    ret = R_IMR_AllocImage(&imr_param_ldc.dst_data, &buf[1], &dst_addr_ldc, IMR_CH_0, 
                        (uint32_t)outputWidth, (uint32_t)outputHeight, (uint32_t)pixfmt);
    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_AllocImage LDC ret=%d\n", ret);
        return FAILED;
    }
    
    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function R_IMR_AllocImageLDC
/******************************************************************************************************************//**
 
/******************************************************************************************************************//**
/* Function Name : R_IMR_AllocImageResize */
/******************************************************************************************************************//**
 * @brief       Allocate image for IMR Resize
                [Covers: BPAP_FC_V4H_AD011][Covers: BPAP_FC_V4H_AD017]
 * @param[in]   none
 * @param[out]  none
 * @retval      true                    success
 * @retval      false                   fail
 *********************************************************************************************************************/
int R_IMR_AllocImageResize(void)
{
    int ret = INVALID;

    // ch0
    if (g_customize.IMR_Ch_0_Enable)
    {

        /* IMR_AllocImage for IMR source data when IMR resize enabled */
        ret = R_IMR_AllocImage(&imr_param_scaling_0.src_data, &buf[2], &src_addr_scaling[IMR_CH_0], IMR_CH_0, 
                            (uint32_t)inputWidth, (uint32_t)inputHeight, (uint32_t)pixfmt);
        if (ret)
        {
            PRINT_ERROR("Failed R_IMR_AllocImage Resize ret=%d\n", ret);
            return FAILED;
        }
        
        /* IMR_AllocImage for IMR destination data when IMR resize enabled */
        ret = R_IMR_AllocImage(&imr_param_scaling_0.dst_data, &buf[3], &dst_addr_scaling[IMR_CH_0], IMR_CH_0, 
                            g_customize.IMR_Resize_Width_Ch_0, g_customize.IMR_Resize_Height_Ch_0, (uint32_t)pixfmt);
        if (ret)
        {
            PRINT_ERROR("Failed R_IMR_AllocImage Resize ret=%d\n", ret);
            return FAILED;
        }
    }

    // ch 1
    if (g_customize.IMR_Ch_1_Enable)
    {

        ret = R_IMR_AllocImage(&imr_param_scaling_1.src_data, &buf[6], &src_addr_scaling[IMR_CH_1], IMR_CH_1,
                            (uint32_t)inputWidth, (uint32_t)inputHeight, (uint32_t)pixfmt);
        if (ret)
        {
            PRINT_ERROR("Failed R_IMR_AllocImage Resize ret=%d\n", ret);
            return FAILED;
        }

        ret = R_IMR_AllocImage(&imr_param_scaling_1.dst_data, &buf[7], &dst_addr_scaling[IMR_CH_1], IMR_CH_1,
                            g_customize.IMR_Resize_Width_Ch_1, g_customize.IMR_Resize_Height_Ch_1, (uint32_t)pixfmt);
        if (ret)
        {
            PRINT_ERROR("Failed R_IMR_AllocImage Resize ret=%d\n", ret);
            return FAILED;
        }
    }

    // ch2
    if (g_customize.IMR_Ch_2_Enable)
    {

        ret = R_IMR_AllocImage(&imr_param_scaling_2.src_data, &buf[9], &src_addr_scaling[IMR_CH_2], IMR_CH_2,
                            (uint32_t)inputWidth, (uint32_t)inputHeight, (uint32_t)pixfmt);
        if (ret)
        {
            PRINT_ERROR("Failed R_IMR_AllocImage Resize ret=%d\n", ret);
            return FAILED;
        }

        ret = R_IMR_AllocImage(&imr_param_scaling_2.dst_data, &buf[10], &dst_addr_scaling[IMR_CH_2], IMR_CH_2,
                            g_customize.IMR_Resize_Width_Ch_2, g_customize.IMR_Resize_Height_Ch_2, (uint32_t)pixfmt);
        if (ret)
        {
            PRINT_ERROR("Failed R_IMR_AllocImage Resize ret=%d\n", ret);
            return FAILED;
        }
    }

    // ch3
    if (g_customize.IMR_Ch_3_Enable)
    {

        ret = R_IMR_AllocImage(&imr_param_scaling_3.src_data, &buf[12], &src_addr_scaling[IMR_CH_3], IMR_CH_3,
                            (uint32_t)inputWidth, (uint32_t)inputHeight, (uint32_t)pixfmt);
        if (ret)
        {
            PRINT_ERROR("Failed R_IMR_AllocImage Resize ret=%d\n", ret);
            return FAILED;
        }

        ret = R_IMR_AllocImage(&imr_param_scaling_3.dst_data, &buf[13], &dst_addr_scaling[IMR_CH_3], IMR_CH_3,
                            g_customize.IMR_Resize_Width_Ch_3, g_customize.IMR_Resize_Height_Ch_3, (uint32_t)pixfmt);
        if (ret)
        {
            PRINT_ERROR("Failed R_IMR_AllocImage Resize ret=%d\n", ret);
            return FAILED;
        }
    }

    // ch4
    if (g_customize.IMR_Ch_4_Enable)
    {

        ret = R_IMR_AllocImage(&imr_param_scaling_4.src_data, &buf[15], &src_addr_scaling[IMR_CH_4], IMR_CH_4,
                            (uint32_t)inputWidth, (uint32_t)inputHeight, (uint32_t)pixfmt);
        if (ret)
        {
            PRINT_ERROR("Failed R_IMR_AllocImage Resize ret=%d\n", ret);
            return FAILED;
        }

        ret = R_IMR_AllocImage(&imr_param_scaling_4.dst_data, &buf[16], &dst_addr_scaling[IMR_CH_4], IMR_CH_4,
                            g_customize.IMR_Resize_Width_Ch_4, g_customize.IMR_Resize_Height_Ch_4, (uint32_t)pixfmt);
        if (ret)
        {
            PRINT_ERROR("Failed R_IMR_AllocImage Resize ret=%d\n", ret);
            return FAILED;
        }
    }

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function R_IMR_AllocImageResize
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : R_IMR_SetupLDC */
/******************************************************************************************************************//**
 * @brief       Setup properties and mapping function for IMR
                [Covers: BPAP_FC_V4H_AD017]
 * @param[in]   none
 * @param[out]  none
 * @retval      true                    success
 * @retval      false                   fail
 *********************************************************************************************************************/
int R_IMR_SetupLDC(void)
{
    int ret = INVALID;
    uint32_t stride_size;
    e_imrdlg_result_t        dlres;
    st_imrdlg_display_list_t dlout;
    int res;

    /* Set up map function data */
    opencv_coeff.k1 = g_customize.IMR_LDC_Params_k1;
    opencv_coeff.k2 = g_customize.IMR_LDC_Params_k2;
    opencv_coeff.k3 = g_customize.IMR_LDC_Params_k3;
    opencv_coeff.p1 = g_customize.IMR_LDC_Params_p1;
    opencv_coeff.p2 = g_customize.IMR_LDC_Params_p2;
    opencv_coeff.fx = g_customize.IMR_LDC_Params_fx * (float)inputWidth;
    opencv_coeff.cx = g_customize.IMR_LDC_Params_cx * (float)inputWidth;
    opencv_coeff.fy = g_customize.IMR_LDC_Params_fy * (float)inputHeight;
    opencv_coeff.cy = g_customize.IMR_LDC_Params_cy * (float)inputHeight;
    opencv_coeff.sx = ((float)inputWidth) / ((float)outputWidth);
    opencv_coeff.sy = ((float)inputHeight) / ((float)outputHeight);
    map_func_data   = (void *)&opencv_coeff;

    /* Set up mapping function */
    map_func = CustomMapLDC;

    stride_size                       = (pixfmt == PIXFMT_YUYV) ? BPP_YUV : BPP_Y;
    imr_param_ldc.src_data.stride     = (uint32_t)imr_param_ldc.src_data.stride * stride_size;
    imr_param_ldc.dst_data.stride     = (uint32_t)imr_param_ldc.dst_data.stride * stride_size;
    DEBUG_PRINT("Image stride IMR LDC = %d\n", imr_param_ldc.src_data.stride);

    st_imrdlg_imr_properties_t iprops =                                     /* For IMR display list generation */
    {
        .target_device           = IMRDLG_TARGET_V3H_2_0, 
        .frame_format            = IMRDLG_FMT_YUYV, 
        .input_frame_width       = (uint16_t)imr_param_ldc.src_data.width, 
        .input_frame_height      = (uint16_t)imr_param_ldc.src_data.height, 
        .input_frame_stride      = (uint32_t)imr_param_ldc.src_data.stride, 
        .output_frame_width      = (uint16_t)imr_param_ldc.dst_data.width, 
        .output_frame_height     = (uint16_t)imr_param_ldc.dst_data.height, 
        .output_frame_stride     = (uint32_t)imr_param_ldc.dst_data.stride, 
        .output_frame_rot_stride = VAL_ZERO, 
        .mesh_size               = (uint8_t)mesh_size, 
        .mesh_size_limit         = (uint8_t)mesh_size_limit, 
        .tra_rot_enable          = IMRDLG_RS_DISABLED, 
        .tra_rot_flags           = VAL_ZERO, 
        .dl_gen_flags            = IMRDLG_F_ADD_SYNCTRAP, 
        .wup_slp_bits            = VAL_ONE, 
    };

    /* Setup Display List */
    dlout.dis_list_size  = (1024UL * 1024UL * 64UL) / 4UL;                  /* 64 MB (MAX allowed by library!) */
    dlout.dis_list_pos   = VAL_ZERO;
    res                  = allocate_dl_memory(&dl_buffer_ldc, dlout.dis_list_size, IMR_DL_ALIGN, 
                                        (void **)(&dlout.p_dis_list_address)); /* Allocate DL memory */

    if (res)
    {
        PRINT_ERROR("Failed Allocate DL Temp Memory ret=%d \n", res);
        return FAILED;
    }
    else
    {
        DEBUG_PRINT("[SUCCESS] Allocate DL Temp Memory\n");
    }

    DEBUG_PRINT(" Creating DL ...\n");

    /* Generate IMR display list */
    dlres = R_IMRDLG_GenerateDisplayList(IMRDLG_ALGO_TMG01, &iprops, NULL, map_func, map_func_data, &dlout);
    if (dlres != 0)
    {
        PRINT_ERROR("DL Creation failed with error %d\n", dlres);
        return FAILED;
    }

    DEBUG_PRINT("dloutsize %d\n", dlout.dis_list_size);

    mmgr_helper_flush(buf[0]);                                            /* Input image flush to dram */
    
     /* memset and (flush and) invalidate */
	memset((void *)dst_addr_ldc, 0x00, imr_param_ldc.dst_data.stride * imr_param_ldc.dst_data.height);     

    /* Allocate display list */
	ret = R_IMR_AllocDl(&imr_param_ldc.dl_data, &buf[4], &dl_addr_ldc, IMR_CH_0, 16 * 1024 * 1024); 

    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_AllocDl %d\n", ret);
        return FAILED;
    }

    ret = R_IMR_TriangleMode(&imr_param_ldc.triangle_mode);               /* IMR_TriangleMode for LDC */

    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_TriangleMode %d\n", ret);
        return FAILED;
    }

    if (VAL_ZERO != dlout.dis_list_pos) 
    {
        memcpy((void *)dl_addr_ldc, dlout.p_dis_list_address, dlout.dis_list_pos); /* Copy over DL and flush memory */
    }
    mmgr_helper_flush(buf[4]);

    return SUCCESS;

}
/******************************************************************************************************************//**
/End of function R_IMR_SetupLDC
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : R_IMR_SetupResize */
/******************************************************************************************************************//**
 * @brief       Setup properties and mapping function for IMR
                [Covers: BPAP_FC_V4H_AD011]
 * @param[in]   none
 * @param[out]  none
 * @retval      true          success
 * @retval      false         fail
 *********************************************************************************************************************/
int R_IMR_SetupResize(void)
{
    
    int ret = FAILED;

    if (g_customize.IMR_Ch_0_Enable)
    {
        ret = imr_SetupResize_ch0();
        if (SUCCESS != ret)
        {
            PRINT_ERROR("Failed R_IMR_SetupResize for setting up channel 0 %d\n", ret);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_1_Enable)
    {
        ret = imr_SetupResize_ch1();
        if (SUCCESS != ret)
        {
            PRINT_ERROR("Failed R_IMR_SetupResize for setting up channel 1 %d\n", ret);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_2_Enable)
    {
        ret = imr_SetupResize_ch2();
        if (SUCCESS != ret)
        {
            PRINT_ERROR("Failed R_IMR_SetupResize for setting up channel 2 %d\n", ret);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_3_Enable)
    {
        ret = imr_SetupResize_ch3();
        if (SUCCESS != ret)
        {
            PRINT_ERROR("Failed R_IMR_SetupResize for setting up channel 3 %d\n", ret);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_4_Enable)
    {
        ret = imr_SetupResize_ch4();
        if (SUCCESS != ret)
        {
            PRINT_ERROR("Failed R_IMR_SetupResize for setting up channel 4 %d\n", ret);
            return FAILED;
        }
    }

    return SUCCESS;

}
/******************************************************************************************************************//**
/End of function R_IMR_SetupResize
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : imr_SetupResize_ch0 */
/******************************************************************************************************************//**
 * @brief       Setup properties and mapping function for IMR channel 0
 * @param[in]   none
 * @param[out]  none
 * @retval      true          success
 * @retval      false         fail
 *********************************************************************************************************************/
static int imr_SetupResize_ch0(void)
{
    int ret = FAILED;
    uint32_t stride_size;
    e_imrdlg_result_t        dlres;
    st_imrdlg_display_list_t dlout;

    map_coeff.w1 = (float)g_frame_width / (float)g_customize.IMR_Resize_Width_Ch_0;
    map_coeff.w2 = (float)g_frame_height / (float)g_customize.IMR_Resize_Height_Ch_0;
    
    map_func_data                         = (void *)&map_coeff;                         /* Set up mapping function */                                                                  
    map_func                              = CustomMapResize;                            /* Set up mapping function */
    
    stride_size                           = (pixfmt == PIXFMT_YUYV) ? BPP_YUV : BPP_Y;
    imr_param_scaling_0.src_data.stride     = (uint32_t)imr_param_scaling_0.src_data.stride * stride_size;
    imr_param_scaling_0.dst_data.stride     = (uint32_t)imr_param_scaling_0.dst_data.stride * stride_size;
    DEBUG_PRINT("Image stride IMR Scaling = %d\n", imr_param_scaling_0.src_data.stride);

    st_imrdlg_imr_properties_t iprops =                                         /* For IMR display list generation */
    {
        .target_device           = IMRDLG_TARGET_V3H_2_0, 
        .frame_format            = IMRDLG_FMT_YUYV, 
        .input_frame_width       = (uint16_t)imr_param_scaling_0.src_data.width, 
        .input_frame_height      = (uint16_t)imr_param_scaling_0.src_data.height, 
        .input_frame_stride      = (uint32_t)imr_param_scaling_0.src_data.stride, 
        .output_frame_width      = (uint16_t)imr_param_scaling_0.dst_data.width, 
        .output_frame_height     = (uint16_t)imr_param_scaling_0.dst_data.height, 
        .output_frame_stride     = (uint32_t)imr_param_scaling_0.dst_data.stride, 
        .output_frame_rot_stride = 0, 
        .mesh_size               = (uint8_t)mesh_size, 
        .mesh_size_limit         = (uint8_t)mesh_size_limit, 
        .tra_rot_enable          = IMRDLG_RS_DISABLED, 
        .tra_rot_flags           = 0, 
        .dl_gen_flags            = IMRDLG_F_ADD_SYNCTRAP, 
        .wup_slp_bits            = 0, 
    };

    /* Setup DL */
    dlout.dis_list_size = (1024UL * 1024UL * 64UL) / 4UL;                    /* 64 MB (MAX allowed by library!) */
    dlout.dis_list_pos  = 0;

    /* Allocate DL memory*/
	ret = allocate_dl_memory(&dl_buffer_scaling_0, dlout.dis_list_size, IMR_DL_ALIGN,
        (void **)(&dlout.p_dis_list_address)); 

    if (ret)
    {
        PRINT_ERROR("Failed Allocate DL Temp Memory ret=%d \n", ret);
        return FAILED;
    }
    else
    {
        DEBUG_PRINT("[SUCCESS] Allocate DL Temp Memory\n");
    }

    DEBUG_PRINT(" Creating Display List for Resize \n");

    /* Generate display list */
	dlres = R_IMRDLG_GenerateDisplayList(IMRDLG_ALGO_TMG01, &iprops, NULL, map_func, map_func_data, &dlout); 
    DEBUG_PRINT("dlout size %d\n", dlout.dis_list_size);
    if (dlres != 0)
    {
        PRINT_ERROR("Failed R_IMRDLG_GenerateDisplayList for resize %d\n", dlres);
        return FAILED;
    }
    
    mmgr_helper_flush(buf[2]);                                                                                              
    memset((void *)dst_addr_scaling[IMR_CH_0], 0x00,
        imr_param_scaling_0.dst_data.stride * imr_param_scaling_0.dst_data.height);

    /* Allocate display list */
	ret = R_IMR_AllocDl(&imr_param_scaling_0.dl_data, &buf[5], &dl_addr_scaling, IMR_CH_0, 16 * 1024 * 1024); 
    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_AllocDl for resize %d\n", ret);
        return FAILED;
    }

    ret = R_IMR_TriangleMode(&imr_param_scaling_0.triangle_mode);              /* IMR_TriangleMode for resize */
    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_TriangleMode for resize %d\n", ret);
        return FAILED;
    }

    if (VAL_ZERO != dlout.dis_list_pos) 
	{
        /* Copy over DL and flush memory */
		memcpy((void *)dl_addr_scaling, dlout.p_dis_list_address, dlout.dis_list_pos); 
    }

    mmgr_helper_flush(buf[5]);

    return SUCCESS;

}
/******************************************************************************************************************//**
/End of function imr_SetupResize_ch0
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : imr_SetupResize_ch1 */
/******************************************************************************************************************//**
 * @brief       Setup properties and mapping function for IMR channel 1
 * @param[in]   none
 * @param[out]  none
 * @retval      true          success
 * @retval      false         fail
 *********************************************************************************************************************/
static int imr_SetupResize_ch1(void)
{
    int ret = FAILED;
    uint32_t stride_size;
    e_imrdlg_result_t        dlres;
    st_imrdlg_display_list_t dlout_1;

    map_coeff.w1 = (float)g_frame_width / (float)g_customize.IMR_Resize_Width_Ch_1;
    map_coeff.w2 = (float)g_frame_height / (float)g_customize.IMR_Resize_Height_Ch_1;
    
    map_func_data                         = (void *)&map_coeff;
    map_func                              = CustomMapResize; 

    stride_size                           = (pixfmt == PIXFMT_YUYV) ? BPP_YUV : BPP_Y;
    imr_param_scaling_1.src_data.stride   = (uint32_t)imr_param_scaling_1.src_data.stride * stride_size;
    imr_param_scaling_1.dst_data.stride   = (uint32_t)imr_param_scaling_1.dst_data.stride * stride_size;
    DEBUG_PRINT("Image stride IMR Scaling = %d\n", imr_param_scaling_0.src_data.stride);

    st_imrdlg_imr_properties_t iprops =                                /* For IMR display list generation */
    {
        .target_device           = IMRDLG_TARGET_V3H_2_0, 
        .frame_format            = IMRDLG_FMT_YUYV, 
        .input_frame_width       = (uint16_t)imr_param_scaling_1.src_data.width, 
        .input_frame_height      = (uint16_t)imr_param_scaling_1.src_data.height, 
        .input_frame_stride      = (uint32_t)imr_param_scaling_1.src_data.stride, 
        .output_frame_width      = (uint16_t)imr_param_scaling_1.dst_data.width, 
        .output_frame_height     = (uint16_t)imr_param_scaling_1.dst_data.height, 
        .output_frame_stride     = (uint32_t)imr_param_scaling_1.dst_data.stride, 
        .output_frame_rot_stride = 0, 
        .mesh_size               = (uint8_t)mesh_size, 
        .mesh_size_limit         = (uint8_t)mesh_size_limit, 
        .tra_rot_enable          = IMRDLG_RS_DISABLED, 
        .tra_rot_flags           = 0, 
        .dl_gen_flags            = IMRDLG_F_ADD_SYNCTRAP, 
        .wup_slp_bits            = 0, 
    };

    /* Setup DL */
    dlout_1.dis_list_size = (1024UL * 1024UL * 64UL) / 4UL;                   /* 64 MB (MAX allowed by library!) */
    dlout_1.dis_list_pos  = 0;

    ret = allocate_dl_memory(&dl_buffer_scaling_1, dlout_1.dis_list_size, IMR_DL_ALIGN, 
	         (void **)(&dlout_1.p_dis_list_address)); /* Allocate DL memory*/
    if (ret)
    {
        PRINT_ERROR("Failed Allocate DL Temp Memory ret=%d \n", ret);
        return FAILED;
    }
    else
    {
        DEBUG_PRINT("[SUCCESS] Allocate DL Temp Memory\n");
    }

    DEBUG_PRINT(" Creating Display List for Resize \n");

    /* Generate display list */
	dlres = R_IMRDLG_GenerateDisplayList(IMRDLG_ALGO_TMG01, &iprops, NULL, map_func, map_func_data, &dlout_1); 
    DEBUG_PRINT("dlout size %d\n", dlout_1.dis_list_size);

    if (dlres != 0)
    {
        PRINT_ERROR("Failed R_IMRDLG_GenerateDisplayList for resize %d\n", dlres);
        return FAILED;
    }

    mmgr_helper_flush(buf[6]);                                                                                              
    memset((void *)dst_addr_scaling[IMR_CH_1], 0x00,
        imr_param_scaling_1.dst_data.stride * imr_param_scaling_1.dst_data.height);

    /* Allocate display list */
	ret = R_IMR_AllocDl(&imr_param_scaling_1.dl_data, &buf[8], &dl_addr_scaling, IMR_CH_1, 16 * 1024 * 1024); 
    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_AllocDl for resize %d\n", ret);
        return FAILED;
    }

    ret = R_IMR_TriangleMode(&imr_param_scaling_1.triangle_mode);                /* IMR_TriangleMode for resize */
    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_TriangleMode for resize %d\n", ret);
        return FAILED;
    }

    if (VAL_ZERO != dlout_1.dis_list_pos) 
	{
        /* Copy over DL and flush memory */
		memcpy((void *)dl_addr_scaling, dlout_1.p_dis_list_address, dlout_1.dis_list_pos); 
    }

    mmgr_helper_flush(buf[8]);

    return SUCCESS;

}
/******************************************************************************************************************//**
/End of function imr_SetupResize_ch1
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : imr_SetupResize_ch2 */
/******************************************************************************************************************//**
 * @brief       Setup properties and mapping function for IMR channel 2
 * @param[in]   none
 * @param[out]  none
 * @retval      true          success
 * @retval      false         fail
 *********************************************************************************************************************/
static int imr_SetupResize_ch2(void)
{
    int ret = FAILED;
    uint32_t stride_size;
    e_imrdlg_result_t        dlres;
    st_imrdlg_display_list_t dlout_2;

    map_coeff.w1 = (float)g_frame_width / (float)g_customize.IMR_Resize_Width_Ch_2;
    map_coeff.w2 = (float)g_frame_height / (float)g_customize.IMR_Resize_Height_Ch_2;
    
    map_func_data                         = (void *)&map_coeff; 
    map_func                              = CustomMapResize; 

    stride_size                           = (pixfmt == PIXFMT_YUYV) ? BPP_YUV : BPP_Y;
    imr_param_scaling_2.src_data.stride   = (uint32_t)imr_param_scaling_2.src_data.stride * stride_size;
    imr_param_scaling_2.dst_data.stride   = (uint32_t)imr_param_scaling_2.dst_data.stride * stride_size;
    DEBUG_PRINT("Image stride IMR Scaling = %d\n", imr_param_scaling_0.src_data.stride);

    st_imrdlg_imr_properties_t iprops =                                    /* For IMR display list generation */
    {
        .target_device           = IMRDLG_TARGET_V3H_2_0, 
        .frame_format            = IMRDLG_FMT_YUYV, 
        .input_frame_width       = (uint16_t)imr_param_scaling_2.src_data.width, 
        .input_frame_height      = (uint16_t)imr_param_scaling_2.src_data.height, 
        .input_frame_stride      = (uint32_t)imr_param_scaling_2.src_data.stride, 
        .output_frame_width      = (uint16_t)imr_param_scaling_2.dst_data.width, 
        .output_frame_height     = (uint16_t)imr_param_scaling_2.dst_data.height, 
        .output_frame_stride     = (uint32_t)imr_param_scaling_2.dst_data.stride, 
        .output_frame_rot_stride = 0, 
        .mesh_size               = (uint8_t)mesh_size, 
        .mesh_size_limit         = (uint8_t)mesh_size_limit, 
        .tra_rot_enable          = IMRDLG_RS_DISABLED, 
        .tra_rot_flags           = 0, 
        .dl_gen_flags            = IMRDLG_F_ADD_SYNCTRAP, 
        .wup_slp_bits            = 0, 
    };

    /* Setup DL */
    dlout_2.dis_list_size = (1024UL * 1024UL * 64UL) / 4UL;                /* 64 MB (MAX allowed by library!) */
    dlout_2.dis_list_pos  = 0;

    ret = allocate_dl_memory(&dl_buffer_scaling_2, dlout_2.dis_list_size, IMR_DL_ALIGN, 
	         (void **)(&dlout_2.p_dis_list_address)); /* Allocate DL memory*/
    if (ret)
    {
        PRINT_ERROR("Failed Allocate DL Temp Memory ret=%d \n", ret);
        return FAILED;
    }
    else
    {
        DEBUG_PRINT("[SUCCESS] Allocate DL Temp Memory\n");
    }

    DEBUG_PRINT(" Creating Display List for Resize \n");

    /* Generate display list */
	dlres = R_IMRDLG_GenerateDisplayList(IMRDLG_ALGO_TMG01, &iprops, NULL, map_func, map_func_data, &dlout_2); 
    DEBUG_PRINT("dlout size %d\n", dlout_2.dis_list_size);

    if (dlres != 0)
    {
        PRINT_ERROR("Failed R_IMRDLG_GenerateDisplayList for resize %d\n", dlres);
        return FAILED;
    }

    mmgr_helper_flush(buf[9]);                                                                                              
    memset((void *)dst_addr_scaling[IMR_CH_2], 0x00,
        imr_param_scaling_2.dst_data.stride * imr_param_scaling_2.dst_data.height);

    /* Allocate display list */
	ret = R_IMR_AllocDl(&imr_param_scaling_2.dl_data, &buf[11], &dl_addr_scaling, IMR_CH_2, 16 * 1024 * 1024); 
    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_AllocDl for resize %d\n", ret);
        return FAILED;
    }

    ret = R_IMR_TriangleMode(&imr_param_scaling_2.triangle_mode);              /* IMR_TriangleMode for resize */
    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_TriangleMode for resize %d\n", ret);
        return FAILED;
    }
    if (VAL_ZERO != dlout_2.dis_list_pos) 
	{
		/* Copy over DL and flush memory */
        memcpy((void *)dl_addr_scaling, dlout_2.p_dis_list_address, dlout_2.dis_list_pos);     
    }

    mmgr_helper_flush(buf[11]);

    return SUCCESS;

}
/******************************************************************************************************************//**
/End of function imr_SetupResize_ch2
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : imr_SetupResize_ch3 */
/******************************************************************************************************************//**
 * @brief       Setup properties and mapping function for IMR channel 3
 * @param[in]   none
 * @param[out]  none
 * @retval      true          success
 * @retval      false         fail
 *********************************************************************************************************************/
static int imr_SetupResize_ch3(void)
{
    int ret = FAILED;
    uint32_t stride_size;
    e_imrdlg_result_t        dlres;
    st_imrdlg_display_list_t dlout_3;

    map_coeff.w1 = (float)g_frame_width / (float)g_customize.IMR_Resize_Width_Ch_3;
    map_coeff.w2 = (float)g_frame_height / (float)g_customize.IMR_Resize_Height_Ch_3;
    
    map_func_data                         = (void *)&map_coeff;
    map_func                              = CustomMapResize; 

    stride_size                           = (pixfmt == PIXFMT_YUYV) ? BPP_YUV : BPP_Y;
    imr_param_scaling_3.src_data.stride   = (uint32_t)imr_param_scaling_3.src_data.stride * stride_size;
    imr_param_scaling_3.dst_data.stride   = (uint32_t)imr_param_scaling_3.dst_data.stride * stride_size;
    DEBUG_PRINT("Image stride IMR Scaling = %d\n", imr_param_scaling_3.src_data.stride);

    st_imrdlg_imr_properties_t iprops =                            /* For IMR display list generation */
    {
        .target_device           = IMRDLG_TARGET_V3H_2_0, 
        .frame_format            = IMRDLG_FMT_YUYV, 
        .input_frame_width       = (uint16_t)imr_param_scaling_3.src_data.width, 
        .input_frame_height      = (uint16_t)imr_param_scaling_3.src_data.height, 
        .input_frame_stride      = (uint32_t)imr_param_scaling_3.src_data.stride, 
        .output_frame_width      = (uint16_t)imr_param_scaling_3.dst_data.width, 
        .output_frame_height     = (uint16_t)imr_param_scaling_3.dst_data.height, 
        .output_frame_stride     = (uint32_t)imr_param_scaling_3.dst_data.stride, 
        .output_frame_rot_stride = 0, 
        .mesh_size               = (uint8_t)mesh_size, 
        .mesh_size_limit         = (uint8_t)mesh_size_limit, 
        .tra_rot_enable          = IMRDLG_RS_DISABLED, 
        .tra_rot_flags           = 0, 
        .dl_gen_flags            = IMRDLG_F_ADD_SYNCTRAP, 
        .wup_slp_bits            = 0, 
    };

    /* Setup DL */
    dlout_3.dis_list_size = (1024UL * 1024UL * 64UL) / 4UL;                 /* 64 MB (MAX allowed by library!) */
    dlout_3.dis_list_pos  = 0;

    ret = allocate_dl_memory(&dl_buffer_scaling_3, dlout_3.dis_list_size, IMR_DL_ALIGN, 
	           (void **)(&dlout_3.p_dis_list_address)); /* Allocate DL memory*/
    if (ret)
    {
        PRINT_ERROR("Failed Allocate DL Temp Memory ret=%d \n", ret);
        return FAILED;
    }
    else
    {
        DEBUG_PRINT("[SUCCESS] Allocate DL Temp Memory\n");
    }

    DEBUG_PRINT(" Creating Display List for Resize \n");

    /* Generate display list */
	dlres = R_IMRDLG_GenerateDisplayList(IMRDLG_ALGO_TMG01, &iprops, NULL, map_func, map_func_data, &dlout_3); 
    DEBUG_PRINT("dlout size %d\n", dlout_3.dis_list_size);

    if (dlres != 0)
    {
        PRINT_ERROR("Failed R_IMRDLG_GenerateDisplayList for resize %d\n", dlres);
        return FAILED;
    }

    mmgr_helper_flush(buf[12]);                                                                                              
    memset((void *)dst_addr_scaling[IMR_CH_3], 0x00,
        imr_param_scaling_3.dst_data.stride * imr_param_scaling_3.dst_data.height);

    /* Allocate display list */
	ret = R_IMR_AllocDl(&imr_param_scaling_3.dl_data, &buf[14], &dl_addr_scaling, IMR_CH_3, 16 * 1024 * 1024); 
    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_AllocDl for resize %d\n", ret);
        return FAILED;
    }

    ret = R_IMR_TriangleMode(&imr_param_scaling_3.triangle_mode);     /* IMR_TriangleMode for resize */
    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_TriangleMode for resize %d\n", ret);
        return FAILED;
    }
    if (VAL_ZERO != dlout_3.dis_list_pos) 
	{
		/* Copy over DL and flush memory */
        memcpy((void *)dl_addr_scaling, dlout_3.p_dis_list_address, dlout_3.dis_list_pos);    
    }

    mmgr_helper_flush(buf[14]);

    return SUCCESS;

}
/******************************************************************************************************************//**
/End of function imr_SetupResize_ch3
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : imr_SetupResize_ch4 */
/******************************************************************************************************************//**
 * @brief       Setup properties and mapping function for IMR channel 4
 * @param[in]   none
 * @param[out]  none
 * @retval      true          success
 * @retval      false         fail
 *********************************************************************************************************************/
static int imr_SetupResize_ch4(void)
{
    int ret = FAILED;
    uint32_t stride_size;
    e_imrdlg_result_t        dlres;
    st_imrdlg_display_list_t dlout_4;

    map_coeff.w1 = (float)g_frame_width / (float)g_customize.IMR_Resize_Width_Ch_4;
    map_coeff.w2 = (float)g_frame_height / (float)g_customize.IMR_Resize_Height_Ch_4;
    
    map_func_data                         = (void *)&map_coeff; 
    map_func                              = CustomMapResize; 

    stride_size                           = (pixfmt == PIXFMT_YUYV) ? BPP_YUV : BPP_Y;
    imr_param_scaling_4.src_data.stride   = (uint32_t)imr_param_scaling_4.src_data.stride * stride_size;
    imr_param_scaling_4.dst_data.stride   = (uint32_t)imr_param_scaling_4.dst_data.stride * stride_size;
    DEBUG_PRINT("Image stride IMR Scaling = %d\n", imr_param_scaling_4.src_data.stride);

    st_imrdlg_imr_properties_t iprops =                 /* For IMR display list generation */
    {
        .target_device           = IMRDLG_TARGET_V3H_2_0, 
        .frame_format            = IMRDLG_FMT_YUYV, 
        .input_frame_width       = (uint16_t)imr_param_scaling_4.src_data.width, 
        .input_frame_height      = (uint16_t)imr_param_scaling_4.src_data.height, 
        .input_frame_stride      = (uint32_t)imr_param_scaling_4.src_data.stride, 
        .output_frame_width      = (uint16_t)imr_param_scaling_4.dst_data.width, 
        .output_frame_height     = (uint16_t)imr_param_scaling_4.dst_data.height, 
        .output_frame_stride     = (uint32_t)imr_param_scaling_4.dst_data.stride, 
        .output_frame_rot_stride = 0, 
        .mesh_size               = (uint8_t)mesh_size, 
        .mesh_size_limit         = (uint8_t)mesh_size_limit, 
        .tra_rot_enable          = IMRDLG_RS_DISABLED, 
        .tra_rot_flags           = 0, 
        .dl_gen_flags            = IMRDLG_F_ADD_SYNCTRAP, 
        .wup_slp_bits            = 0, 
    };

    /* Setup DL */
    dlout_4.dis_list_size = (1024UL * 1024UL * 64UL) / 4UL;                  /* 64 MB (MAX allowed by library!) */
    dlout_4.dis_list_pos  = 0;

    ret = allocate_dl_memory(&dl_buffer_scaling_4, dlout_4.dis_list_size, IMR_DL_ALIGN, 
	          (void **)(&dlout_4.p_dis_list_address)); /* Allocate DL memory*/
    if (ret)
    {
        PRINT_ERROR("Failed Allocate DL Temp Memory ret=%d \n", ret);
        return FAILED;
    }
    else
    {
        DEBUG_PRINT("[SUCCESS] Allocate DL Temp Memory\n");
    }

    DEBUG_PRINT(" Creating Display List for Resize \n");

    /* Generate display list */
    dlres = R_IMRDLG_GenerateDisplayList(IMRDLG_ALGO_TMG01, &iprops, NULL, map_func, map_func_data, &dlout_4); 
    DEBUG_PRINT("dlout size %d\n", dlout_4.dis_list_size);

    if (dlres != 0)
    {
        PRINT_ERROR("Failed R_IMRDLG_GenerateDisplayList for resize %d\n", dlres);
        return FAILED;
    }

    mmgr_helper_flush(buf[15]);                                                                                              
    memset((void *)dst_addr_scaling[IMR_CH_4], 0x00,
        imr_param_scaling_4.dst_data.stride * imr_param_scaling_4.dst_data.height);

    /* Allocate display list */
	ret = R_IMR_AllocDl(&imr_param_scaling_4.dl_data, &buf[17], &dl_addr_scaling, IMR_CH_4, 16 * 1024 * 1024); 
    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_AllocDl for resize %d\n", ret);
        return FAILED;
    }

    ret = R_IMR_TriangleMode(&imr_param_scaling_4.triangle_mode);           /* IMR_TriangleMode for resize */
    if (ret)
    {
        PRINT_ERROR("Failed R_IMR_TriangleMode for resize %d\n", ret);
        return FAILED;
    }
    if (VAL_ZERO != dlout_4.dis_list_pos) 
	{
		 /* Copy over DL and flush memory */
        memcpy((void *)dl_addr_scaling, dlout_4.p_dis_list_address, dlout_4.dis_list_pos);    
    }

    mmgr_helper_flush(buf[17]);

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function imr_SetupResize_ch4
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : R_IMR_ExecuteLDC */
/******************************************************************************************************************//**
 * @brief       IMR LDC Execution
                [Covers: BPAP_FC_V4H_AD017]
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
 *********************************************************************************************************************/
int R_IMR_ExecuteLDC(void)
{
    int ret = INVALID;

    DEBUG_PRINT("running IMR-LDC \n");
    R_FC_SyncStart(eIMR_LDC, gp_mtx_handle_imr_ldc, gp_imr_ldc_cond_handle, 0);

    memcpy((void *)src_addr_ldc, (void *)gp_imr_ldc_in, g_frame_width * g_frame_height * BPP_YUV); /* Get source data */
    
    R_FC_SyncEnd(eIMR_LDC, gp_mtx_handle_imr_ldc, gp_imr_ldc_cond_handle, 0);
    
    imrdrv_ctrl_handle_t imr_handles_ch0 = get_imrdrv_ctrlhandles(IMR_CH_0); /* control handle for imr driver */
    
    ret_imr = R_IMRDRV_AttrSetParam(imr_handles_ch0, &imr_param_ldc);
    
    if (ret_imr != IMRDRV_ERROR_OK)
    {
        PRINT_ERROR("Failed R_IMRDRV_AttrSetParam for resize ch=%d ret=%d\n", IMR_CH_0, ret_imr);
        return FAILED;
    }

    DEBUG_PRINT("Succeed R_IMRDRV_AttrSetParam ch=%d\n", IMR_CH_0);

    /* ---- Setting Sync ------*/
    st_imrdrv_attr_cache_mode_t cache_mode_param;

    if (enable_prefetch == 1) 
    {
        cache_mode_param.cache_mode = IMRDRV_CACHE_MODE_NON_BLOCKING;
    }
    else 
    {
        cache_mode_param.cache_mode = IMRDRV_CACHE_MODE_NORMAL;
    }

    cache_mode_param.double_cache_mode = IMRDRV_DOUBLE_CACHE_MODE_SINGLE;
    
    ret_imr = R_IMRDRV_AttrSetCacheMode(imr_handles_ch0, &cache_mode_param);       /* Set IMR attribute cache mode */
    
    if (ret_imr != IMRDRV_ERROR_OK)
    {
        PRINT_ERROR("Failed R_IMRDRV_AttrSetCacheMode LDC ret=%d\n", ret_imr);

        if (ret != OSAL_RETURN_OK) 
        {
            PRINT_ERROR("Failed R_IMRDRV_AttrSetCacheMode LDC ret=%d\n", ret);
        }
        return FAILED;
    }

    ret = imrdrv_try_lock();

    if (ret)
    {
        PRINT_ERROR("***ERROR: imr_exectute LDC failed with %d\n", ret);
        return FAILED;
    }
    ret_imr =  R_IMRDRV_Execute(imr_handles_ch0);                       /* Execution lens distortion correction */
    {
         st_osal_time_t debug_time_stamp;
         (void)R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &debug_time_stamp);
#ifdef DEBUG
         DEBUG_PRINT("DMS: IMR2: %d.%09lu\n", debug_time_stamp.tv_sec, 
                                         debug_time_stamp.tv_nsec);
#endif
     }

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        PRINT_ERROR("Failed R_IMRDRV_Execute LDC ret=%d\n", ret_imr);
        return FAILED;
    }

    DEBUG_PRINT("Succeed R_IMRDRV_Execute LDC ch = %d\n", IMR_CH_0);

    ret = imrdrv_wait();                                                               /* Wait for completion */

    if (ret)
    {
        PRINT_ERROR("***ERROR: imr_exectute LDC failed with %d\n", ret);
        return FAILED;
    }

    R_FC_SyncStart(eIMR_LDC, &g_mtx_handle_imrldc, &g_imr_ldc_cond_handle, 1);              /* sync start of dms */

    /* Get the data after lens distortion correction */
	memcpy((void *)gp_imr_ldc_buffer, (void *)dst_addr_ldc, g_frame_width * g_frame_height * 2); 

    R_FC_SyncEnd(eIMR_LDC, &g_mtx_handle_imrldc, &g_imr_ldc_cond_handle, 1);                /* sync end of dms */

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function R_IMR_ExecuteLDC
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : R_IMR_GetLDCBuffer */
/******************************************************************************************************************//**
 * @brief       Get LDC IMR buffer for resize
                [Covers: BPAP_SV_V4H_AD033][Covers: BPAP_SV_V4H_AD043]
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
 *********************************************************************************************************************/
void R_IMR_GetLDCBuffer(void)
{
    if (g_customize.IMR_Ch_0_Enable)
    {
        memcpy((void *)src_addr_scaling[IMR_CH_0], (void *)gp_imr_res_in, g_frame_width * g_frame_height * 2); /* Get source data when LDC is enabled */
    }
    if (g_customize.IMR_Ch_1_Enable)
    {
        memcpy((void *)src_addr_scaling[IMR_CH_1], (void *)gp_imr_res_in, g_frame_width * g_frame_height * 2);
    }
    if (g_customize.IMR_Ch_2_Enable)
    {
        memcpy((void *)src_addr_scaling[IMR_CH_2], (void *)gp_imr_res_in, g_frame_width * g_frame_height * 2);
    }
    if (g_customize.IMR_Ch_3_Enable)
    {
        memcpy((void *)src_addr_scaling[IMR_CH_3], (void *)gp_imr_res_in, g_frame_width * g_frame_height * 2);
    }
    if (g_customize.IMR_Ch_4_Enable)
    {
        memcpy((void *)src_addr_scaling[IMR_CH_4], (void *)gp_imr_res_in, g_frame_width * g_frame_height * 2);
    }
}

/******************************************************************************************************************//**
/End of function R_IMR_GetLDCBuffer
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : R_IMR_GetBuffer */
/******************************************************************************************************************//**
 * @brief       Get IMR Buffer for resize
                [Covers: BPAP_SV_V4H_AD033][Covers: BPAP_SV_V4H_AD043]
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
 *********************************************************************************************************************/
void R_IMR_GetBuffer(void)
{
        R_FC_SyncStart(eIMR_RS, gp_mtx_handle_imr_rs, gp_imr_rs_cond_handle, 0);
        
        if (g_customize.IMR_Ch_0_Enable)
        {
            /* Get source data to be resized */
            memcpy((void *)src_addr_scaling[IMR_CH_0], (void *)gp_vin_out_buffer, 
                g_frame_width * g_frame_height * BPP_YUV); 
        }
        if (g_customize.IMR_Ch_1_Enable)
        {    
            memcpy((void *)src_addr_scaling[IMR_CH_1], (void *)gp_vin_out_buffer,
                g_frame_width * g_frame_height * BPP_YUV);
        }
        if (g_customize.IMR_Ch_2_Enable)
        {    
            memcpy((void *)src_addr_scaling[IMR_CH_2], (void *)gp_vin_out_buffer,
                g_frame_width * g_frame_height * BPP_YUV);
        }
        if (g_customize.IMR_Ch_3_Enable)
        {    
            memcpy((void *)src_addr_scaling[IMR_CH_3], (void *)gp_vin_out_buffer,
                g_frame_width * g_frame_height * BPP_YUV);
        }
        if (g_customize.IMR_Ch_4_Enable)
        {    
            memcpy((void *)src_addr_scaling[IMR_CH_4], (void *)gp_vin_out_buffer,
                g_frame_width * g_frame_height * BPP_YUV);
        }

        R_FC_SyncEnd(eIMR_RS, gp_mtx_handle_imr_rs, gp_imr_rs_cond_handle, 0);
}

/******************************************************************************************************************//**
/End of function R_IMR_GetBuffer
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : R_IMR_GetResizedBuffer */
/******************************************************************************************************************//**
 * @brief       Get IMR Resized Buffer
                [Covers: BPAP_SV_V4H_AD033][Covers: BPAP_SV_V4H_AD043]
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
 *********************************************************************************************************************/
void R_IMR_GetResizedBuffer(void)
{
    if (g_customize.IMR_Ch_0_Enable)
    {
        /* Get IMR resized data ch0 */
        memcpy((void *)gp_imr_rs_buffer_ch0, (void *)dst_addr_scaling[IMR_CH_0],
            g_customize.IMR_Resize_Width_Ch_0 * g_customize.IMR_Resize_Height_Ch_0 * BPP_YUV); 
    }
    if (g_customize.IMR_Ch_1_Enable)
    {    
        /* Get IMR resized data ch1 */
        memcpy((void *)gp_imr_rs_buffer_ch1, (void *)dst_addr_scaling[IMR_CH_1],
            g_customize.IMR_Resize_Width_Ch_1 * g_customize.IMR_Resize_Height_Ch_1 * BPP_YUV); 
    }
    if (g_customize.IMR_Ch_2_Enable)
    {    
        /* Get IMR resized data ch2 */
        memcpy((void *)gp_imr_rs_buffer_ch2, (void *)dst_addr_scaling[IMR_CH_2],
            g_customize.IMR_Resize_Width_Ch_2 * g_customize.IMR_Resize_Height_Ch_2 * BPP_YUV); 
    }
    if (g_customize.IMR_Ch_3_Enable)
    {    
        /* Get IMR resized data ch3 */
        memcpy((void *)gp_imr_rs_buffer_ch3, (void *)dst_addr_scaling[IMR_CH_3],
            g_customize.IMR_Resize_Width_Ch_3 * g_customize.IMR_Resize_Height_Ch_3 * BPP_YUV); 
    }
    if (g_customize.IMR_Ch_4_Enable)
    {    
        memcpy((void *)gp_imr_rs_buffer_ch4, (void *)dst_addr_scaling[IMR_CH_4],
            g_customize.IMR_Resize_Width_Ch_4 * g_customize.IMR_Resize_Height_Ch_4 * BPP_YUV);
    }
}

/******************************************************************************************************************//**
/End of function R_IMR_GetResizedBuffer
/******************************************************************************************************************//**
/* Function Name : R_IMR_ExecuteResize */
/******************************************************************************************************************//**
 * @brief       IMR Resize Execution
                [Covers: BPAP_FC_V4H_AD011][Covers: BPAP_FC_V4H_AD017]
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
 *********************************************************************************************************************/
int R_IMR_ExecuteResize(void)
{
    int ret = FAILED;

    struct timeval  mod_starttime;
    struct timeval  mod_endtime;


    if(true == g_customize.IMR_LDC)
    {
        R_IMR_GetLDCBuffer();
    }
    else 
    {           
        R_IMR_GetBuffer();
    }

    DEBUG_PRINT("running IMR-Resize \n");

    if (g_customize.IMR_Ch_0_Enable)
    {
        ret = imr_ExecuteResize_ch0();
        if (SUCCESS != ret)
        {
            PRINT_ERROR("Failed IMR resize execution for channel 0, ret=%d \n", ret);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_1_Enable)
    {
        ret = imr_ExecuteResize_ch1();
        if (SUCCESS != ret)
        {
            PRINT_ERROR("Failed IMR resize execution for channel 1, ret=%d \n", ret);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_2_Enable)
    {   
        ret = imr_ExecuteResize_ch2();
        if (SUCCESS != ret)
        {
            PRINT_ERROR("Failed IMR resize execution for channel 2, ret=%d \n", ret);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_3_Enable)
    {   
        ret = imr_ExecuteResize_ch3();
        if (SUCCESS != ret)
        {
            PRINT_ERROR("Failed IMR resize execution for channel 3, ret=%d \n", ret);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_4_Enable)
    {   
        ret = imr_ExecuteResize_ch4();
        if (SUCCESS != ret)
        {
            PRINT_ERROR("Failed IMR resize execution for channel 4, ret=%d \n", ret);
            return FAILED;
        }
    }

    if (0 != g_customize.Proc_Time)
    {
        gettimeofday(&mod_starttime, NULL);
    }

    R_FC_SyncStart(eIMR_RS, &g_mtx_handle_imrrs_out, &g_imr_rs_cond_handle, 1);
    
    R_IMR_GetResizedBuffer();


    R_FC_SyncEnd(eIMR_RS, &g_mtx_handle_imrrs_out, &g_imr_rs_cond_handle, 1);

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function R_IMR_ExecuteResize
/*********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : imr_ExecuteResize_ch0 */
/******************************************************************************************************************//**
 * @brief       IMR Resize Execution
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
 *********************************************************************************************************************/
static int imr_ExecuteResize_ch0(void)
{
    int ret = INVALID;
    imrdrv_ctrl_handle_t imr_handles_ch = get_imrdrv_ctrlhandles(IMR_CH_0);

    ret_imr = R_IMRDRV_AttrSetParam(imr_handles_ch, &imr_param_scaling_0);     /* Setting parameter for imr driver */
    if (ret_imr != IMRDRV_ERROR_OK)
    {
        PRINT_ERROR("Failed R_IMRDRV_AttrSetParam Resize ch=%d ret=%d\n", IMR_CH_0, ret_imr);
        return FAILED;
    }

    DEBUG_PRINT("Succeed R_IMRDRV_AttrSetParam Resize ch=%d\n", IMR_CH_0);

    ret = imrdrv_try_lock();
    if (ret)
    {
        PRINT_ERROR("***ERROR: imr_exectute LDC failed with %d\n", ret);
        return FAILED;
    }

    ret_imr =  R_IMRDRV_Execute(imr_handles_ch);                                      /* Execute IMR resize */

    st_osal_time_t debug_time_stamp;
    (void)R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &debug_time_stamp);
#ifdef DEBUG
    DEBUG_PRINT("DMS: IMR1: %d.%09lu\n", debug_time_stamp.tv_sec, 
                                         debug_time_stamp.tv_nsec);
#endif

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        PRINT_ERROR("Failed R_IMRDRV_Execute Resize ret=%d\n", ret_imr);
        return FAILED;
    }

    DEBUG_PRINT("Succeed R_IMRDRV_Execute Resize ch = %d\n", IMR_CH_0);

    ret = imrdrv_wait();                                                                /* Wait for completion */
    if (ret)
    {
        PRINT_ERROR("ERROR: imr_exectute Resize failed with %d\n", ret);
        return FAILED;
    }

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function imr_ExecuteResize_ch0
/*********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : imr_ExecuteResize_ch1 */
/******************************************************************************************************************//**
 * @brief       IMR Resize Execution
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
 *********************************************************************************************************************/
static int imr_ExecuteResize_ch1(void)
{
    int ret = INVALID;
    imrdrv_ctrl_handle_t imr_handles_ch1 = get_imrdrv_ctrlhandles(IMR_CH_1);

    ret_imr = R_IMRDRV_AttrSetParam(imr_handles_ch1, &imr_param_scaling_1);   /* Setting parameter for imr driver */

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        PRINT_ERROR("Failed R_IMRDRV_AttrSetParam Resize ch=%d ret=%d\n", IMR_CH_1, ret_imr);
        return FAILED;
    }

    DEBUG_PRINT("Succeed R_IMRDRV_AttrSetParam Resize ch=%d\n", IMR_CH_1);

    ret = imrdrv_try_lock();
    if (ret)
    {
        PRINT_ERROR("***ERROR: imr_exectute LDC failed with %d\n", ret);
        return FAILED;
    }

    ret_imr =  R_IMRDRV_Execute(imr_handles_ch1);                                      /* Execute IMR resize */

    ret = imrdrv_wait_1();                                                            /* Wait for completion */
    if (ret)
    {
        PRINT_ERROR("ERROR: imr_exectute Resize failed with %d\n", ret);
        return FAILED;
    }

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function imr_ExecuteResize_ch1
/*********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : imr_ExecuteResize_ch2 */
/******************************************************************************************************************//**
 * @brief       IMR Resize Execution
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
 *********************************************************************************************************************/
static int imr_ExecuteResize_ch2(void)
{
    int ret = INVALID;
    imrdrv_ctrl_handle_t imr_handles_ch2 = get_imrdrv_ctrlhandles(IMR_CH_2);

    ret_imr = R_IMRDRV_AttrSetParam(imr_handles_ch2, &imr_param_scaling_2);    /* Setting parameter for imr driver */

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        PRINT_ERROR("Failed R_IMRDRV_AttrSetParam Resize ch=%d ret=%d\n", IMR_CH_2, ret_imr);
        return FAILED;
    }

    DEBUG_PRINT("Succeed R_IMRDRV_AttrSetParam Resize ch=%d\n", IMR_CH_2);

    ret = imrdrv_try_lock();
    if (ret)
    {
        PRINT_ERROR("***ERROR: imr_exectute LDC failed with %d\n", ret);
        return FAILED;
    }

    ret_imr =  R_IMRDRV_Execute(imr_handles_ch2);                                      /* Execute IMR resize */

    ret = imrdrv_wait_2();                                                            /* Wait for completion */
    if (ret)
    {
        PRINT_ERROR("ERROR: imr_exectute Resize failed with %d\n", ret);
        return FAILED;
    }

     return SUCCESS;
}
/******************************************************************************************************************//**
/End of function imr_ExecuteResize_ch2
/*********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : imr_ExecuteResize_ch3 */
/******************************************************************************************************************//**
 * @brief       IMR Resize Execution
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
 *********************************************************************************************************************/
static int imr_ExecuteResize_ch3(void)
{
    int ret = INVALID;
    imrdrv_ctrl_handle_t imr_handles_ch3 = get_imrdrv_ctrlhandles(IMR_CH_3);

    ret_imr = R_IMRDRV_AttrSetParam(imr_handles_ch3, &imr_param_scaling_3);   /* Setting parameter for imr driver */

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        PRINT_ERROR("Failed R_IMRDRV_AttrSetParam Resize ch=%d ret=%d\n", IMR_CH_3, ret_imr);
        return FAILED;
    }

    DEBUG_PRINT("Succeed R_IMRDRV_AttrSetParam Resize ch=%d\n", IMR_CH_3);

    ret = imrdrv_try_lock();
    if (ret)
    {
        PRINT_ERROR("***ERROR: imr_exectute LDC failed with %d\n", ret);
        return FAILED;
    }

    ret_imr =  R_IMRDRV_Execute(imr_handles_ch3);                                      /* Execute IMR resize */

    ret = imrdrv_wait_3();                                                            /* Wait for completion */
    if (ret)
    {
        PRINT_ERROR("ERROR: imr_exectute Resize failed with %d\n", ret);
        return FAILED;
    }

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function imr_ExecuteResize_ch3
/*********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : imr_ExecuteResize_ch4 */
/******************************************************************************************************************//**
 * @brief       IMR Resize Execution
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
 *********************************************************************************************************************/
static int imr_ExecuteResize_ch4(void)
{
    int ret = INVALID;
    imrdrv_ctrl_handle_t imr_handles_ch4 = get_imrdrv_ctrlhandles(IMR_CH_4);

    ret_imr = R_IMRDRV_AttrSetParam(imr_handles_ch4, &imr_param_scaling_4);    /* Setting parameter for imr driver */

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        PRINT_ERROR("Failed R_IMRDRV_AttrSetParam Resize ch=%d ret=%d\n", IMR_CH_4, ret_imr);
        return FAILED;
    }

    DEBUG_PRINT("Succeed R_IMRDRV_AttrSetParam Resize ch=%d\n", IMR_CH_4);

    ret = imrdrv_try_lock();
    if (ret)
    {
        PRINT_ERROR("***ERROR: imr_exectute LDC failed with %d\n", ret);
        return FAILED;
    }

    ret_imr =  R_IMRDRV_Execute(imr_handles_ch4);                                      /* Execute IMR resize */

    ret = imrdrv_wait_4();                                                            /* Wait for completion */
    if (ret)
    {
        PRINT_ERROR("ERROR: imr_exectute Resize failed with %d\n", ret);
        return FAILED;
    }

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function imr_ExecuteResize_ch4
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : R_IMR_Deinit */
/******************************************************************************************************************//**
 * @brief       IMR module deinitialisation
                [Covers: BPAP_FC_V4H_AD011][Covers: BPAP_FC_V4H_AD017][Covers: BPAP_FC_V4H_AD037]
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
 *********************************************************************************************************************/
int R_IMR_Deinit(void)
{
    e_osal_return_t ret_osal;
    /* Release DL and Image planes (NOTE: inverse order !) */
    if (SUCCESS == g_fcStatus.imr_ldc.status)                     /* If IMR lens distortion correction is success */
    {
        mmgr_helper_dealloc(buf[0]);
        mmgr_helper_dealloc(buf[1]);
        mmgr_helper_dealloc(buf[4]);
        mmgr_helper_dealloc(dl_buffer_ldc);
    }

    if (SUCCESS == g_fcStatus.imr_rs.status)                      /* If IMR resize is success */
    {
        if (g_customize.IMR_Ch_0_Enable)
        {
            mmgr_helper_dealloc(buf[2]);
            mmgr_helper_dealloc(buf[3]);
            mmgr_helper_dealloc(buf[5]);
            mmgr_helper_dealloc(dl_buffer_scaling_0);

            ret_osal = R_OSAL_MqDelete(g_mq_handle_imr_0);
            if (ret_osal != OSAL_RETURN_OK)
            {
                printf("Failed R_OSAL_MqDelete ret=%d\n", ret_osal);

            }
        }
        if (g_customize.IMR_Ch_1_Enable)
        {
            mmgr_helper_dealloc(buf[6]);
            mmgr_helper_dealloc(buf[7]);
            mmgr_helper_dealloc(buf[8]);
            mmgr_helper_dealloc(dl_buffer_scaling_1);

            ret_osal = R_OSAL_MqDelete(g_mq_handle_imr_1);
            if (ret_osal != OSAL_RETURN_OK)
            {
                printf("Failed R_OSAL_MqDelete ret=%d\n", ret_osal);

            }
        }
        if (g_customize.IMR_Ch_2_Enable)
        {
            mmgr_helper_dealloc(buf[9]);
            mmgr_helper_dealloc(buf[10]);
            mmgr_helper_dealloc(buf[11]);
            mmgr_helper_dealloc(dl_buffer_scaling_2);

            ret_osal = R_OSAL_MqDelete(g_mq_handle_imr_2);
            if (ret_osal != OSAL_RETURN_OK)
            {
                printf("Failed R_OSAL_MqDelete ret=%d\n", ret_osal);

            }
        }
        if (g_customize.IMR_Ch_3_Enable)
        {
            mmgr_helper_dealloc(buf[12]);
            mmgr_helper_dealloc(buf[13]);
            mmgr_helper_dealloc(buf[14]);
            mmgr_helper_dealloc(dl_buffer_scaling_3);

            ret_osal = R_OSAL_MqDelete(g_mq_handle_imr_3);
            if (ret_osal != OSAL_RETURN_OK)
            {
                printf("Failed R_OSAL_MqDelete ret=%d\n", ret_osal);

            }
        }
        if (g_customize.IMR_Ch_4_Enable)
        {
            mmgr_helper_dealloc(buf[15]);
            mmgr_helper_dealloc(buf[16]);
            mmgr_helper_dealloc(buf[17]);
            mmgr_helper_dealloc(dl_buffer_scaling_4);

            ret_osal = R_OSAL_MqDelete(g_mq_handle_imr_4);
            if (ret_osal != OSAL_RETURN_OK)
            {
                printf("Failed R_OSAL_MqDelete ret=%d\n", ret_osal);

            }
        }
    }

    /* shutdown */
    DEBUG_PRINT("Tearing down drivers\n");

    if ((SUCCESS == g_fcStatus.imr_ldc.status) || (SUCCESS == g_fcStatus.imr_rs.status))
    {
        deinit_imrdrv();                                         /* Deinitialize IMR driver */
    }

    DEBUG_PRINT("IMR END\n");
    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function R_IMR_Deinit
/******************************************************************************************************************/
