/***********************************************************************************************************************
* Copyright [2022-2023] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_dc_isp_memories.c
* Version      : 0.4.0
* Product Name : ISP Test Application
* Device(s)    : R-Car V4H
* Description  : utility functions for handling the memory buffers used by the ISP Sample Application
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : Version DD.MM.YYYY Description
*         : 0.1.0   17.01.2022 Initial version
*         : 0.2.0   31.05.2022 Add create bmp file option.
*         : 0.3.0   16.01.2023 Upadated buffer ID and Added app forced exit process
*         : 0.4.0   13.03.2023 Added option that specify DMA mode, direct writing CPU mode and Display List mode
***********************************************************************************************************************/
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/stdio.h>  /* printf , */
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/string.h> /* memset, ...*/
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/stdlib.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>

#include "rcar-xos/osal/r_osal_api.h"
#include "rcar-xos/osal/r_osal_types.h"

#include "rcar-xos/vspx/r_vspx.h"

#include "r_dc_isp_utils.h"
#include "r_dc_isp_memories.h"
#include "r_dc_isp_callbacks.h"
#include "r_dc_isp_setup.h"
#include "r_vspx_setup.h"
#include "common.h"

extern st_cisp_dma_config_data_t *g_dma_config_data;
extern uint32_t g_display_list[512];

#if RAW_IMAGE_CONST_TABLE
#endif

extern osal_memory_manager_handle_t g_handle_osalmmngr;

/*bmp header file information*/
#define FILE_TYPE 0x4D42
#define FILE_SIZE ((INFO_WIDTH * INFO_HEIGHT * 4) + DEFAULT_HEADER_SIZE)
#define DEFAULT_HEADER_SIZE 54
#define INFO_HEADER_SIZE 40
#define INFO_WIDTH INPUT_IMAGE_WIDTH
#define INFO_HEIGHT INPUT_IMAGE_HEIGHT
#define INFO_PLANES 0x01
#define INFO_BIT_COUNT 0x20
#define INFO_SIZE_IMAGE (INFO_WIDTH * INFO_HEIGHT * 4)
#define INFO_BIX_PIXPERMETER 0
#define INFO_COMPRESSION 0  
#define INFO_CLR_USED 0
#define INFO_CIR_IMPORTANT 0
#define FILE_RESERVED 0

#pragma pack(1)
typedef struct tag_bmp_file_header
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
}bmp_file_header;

typedef struct tag_bmp_info_header
{
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPixPerMeter;
    int32_t BiYPixPerMeter;
    uint32_t biClrUsed;
    uint32_t biCirImportant;
}bmp_info_header;
#pragma pack()

r_stest_buffer_t buffers[NUM_BUFFERS];

char* devstr_vspx0 = "vspx_00";
char* devstr_cisp0 = "cisp_00";

#define BYTES_PER_PIXEL(bpp) ((uint8_t)((bpp-1)/8)+1)

#define VSPX_ALIGN (64u)
#define CISP_ALIGN (4096u) // !!! Stride must be multiple of 256 and divisor of 4096, start address must be aligned at 4096

/**********************************************************************************************************************
/* Function Name : Buffer_Copy */
/******************************************************************************************************************//**
 * @brief       Copying isp buffer
                [Covers: BPAP_FC_V4H_AD016][Covers: BPAP_FC_V4H_AD036]
 * @param[in]   vspx_memories_t* vspx_mem, st_isp_options_t *option
 * @param[out]  none
 * @retval      none
 * @retval      none
 *********************************************************************************************************************/
void Buffer_Copy(vspx_memories_t* vspx_mem, st_isp_options_t *option)
{
    uint8_t ret;
    memcpy(buffers[BUF_EXPOSURE0].virt_addr, gp_isp_in, (INPUT_IMAGE_HEIGHT*INPUT_IMAGE_WIDTH * BYTES_PER_PIXEL(INPUT_IMAGE_BPP)));
    ret = R_STEST_MemFlush(&g_handle_osalmmngr, &buffers[BUF_EXPOSURE0]);
    CHECK_RET("MemFlush EXP0");
}
/**********************************************************************************************************************
 End of function Buffer_Copy
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : Init_Memories */
/******************************************************************************************************************//**
 * @brief       Memories initialization
 * @param[in]   st_isp_options_t *option
 * @param[out]  vspx_memories_t* vspx_mem,
*  @param[out]  isp_out_memories_t* isp_mem
 * @retval      true            success(0)
 * @retval      false           fail(1)
 *********************************************************************************************************************/
void Init_Memories(vspx_memories_t* vspx_mem, isp_out_memories_t* isp_mem, st_isp_options_t *option)
{
    uint8_t ret;
    // For Input:
    // Need RAW buffers (2 exposures)
    // Need DLs for VSPX
    // Need CISP registers
    // => all consumed by VSPX
    (void)isp_mem;
    memset(vspx_mem, 0x00, sizeof(vspx_memories_t));
	memset(buffers, 0x00, NUM_BUFFERS*sizeof(buffers[0]));

    buffers[BUF_EXPOSURE0].size = INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT * BYTES_PER_PIXEL(INPUT_IMAGE_BPP);
    buffers[BUF_EXPOSURE0].align = VSPX_ALIGN;
    ret = R_STEST_MemAlloc(&g_handle_osalmmngr, &buffers[BUF_EXPOSURE0], devstr_vspx0);
    CHECK_RET("MemAlloc EXP0");

    buffers[BUF_EXPOSURE1].size = INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT * BYTES_PER_PIXEL(INPUT_IMAGE_BPP);
    buffers[BUF_EXPOSURE1].align = VSPX_ALIGN;
    ret = R_STEST_MemAlloc(&g_handle_osalmmngr, &buffers[BUF_EXPOSURE1], devstr_vspx0);
    CHECK_RET("MemAlloc EXP1");

    buffers[BUF_CISPREGS].size = 65536;
    buffers[BUF_CISPREGS].align = VSPX_ALIGN;
    ret = R_STEST_MemAlloc(&g_handle_osalmmngr, &buffers[BUF_CISPREGS], devstr_vspx0);
    CHECK_RET("MemAlloc CISPREGS");

    buffers[BUF_VSPXDL].size = sizeof(g_display_list);
    buffers[BUF_VSPXDL].align = VSPX_ALIGN;
    ret = R_STEST_MemAlloc(&g_handle_osalmmngr, &buffers[BUF_VSPXDL], devstr_vspx0);
    CHECK_RET("MemAlloc VSPXDL");

    if ( (option->hdr_fusion == ENABLED) || (option->module_name.dpcc_pre2 == ENABLED) )
    {
        vspx_mem->num_exposures = 2;
    }
    else
    {
        vspx_mem->num_exposures = 1;
    }
    vspx_mem->exposures[0].height = INPUT_IMAGE_HEIGHT;
    vspx_mem->exposures[0].width = INPUT_IMAGE_WIDTH;
    vspx_mem->exposures[0].stride = INPUT_IMAGE_WIDTH * BYTES_PER_PIXEL(INPUT_IMAGE_BPP);

    vspx_mem->exposures[0].phys_addr = (uint32_t)buffers[BUF_EXPOSURE0].phy_addr;
    vspx_mem->exposures[0].format = INPUT_IMAGE_VSPX_RDFMT;

    vspx_mem->cisp_config.height = 65536/64;
    vspx_mem->cisp_config.stride = VSPX_ALIGN;
    vspx_mem->cisp_config.phys_addr = (uint32_t)buffers[BUF_CISPREGS].phy_addr;

    vspx_mem->dl_storage.height = (uint32_t)buffers[BUF_VSPXDL].size / VSPX_ALIGN;
    vspx_mem->dl_storage.stride = VSPX_ALIGN;
    vspx_mem->dl_storage.phys_addr = (uint32_t)buffers[BUF_VSPXDL].phy_addr;
    vspx_mem->dl_storage.virt_addr = (uint32_t *)buffers[BUF_VSPXDL].virt_addr;

    // For Output:
    memset(isp_mem, 0x00, sizeof(isp_out_memories_t));
    /* NOTE: ISP driver offers planar (multiple planes) and semi-planar modes ONLY */
    uint32_t mv_stride = 4096; //(INPUT_IMAGE_WIDTH*4 aligned to next multiple of 256 which is PoT)
    buffers[BUF_MVOUT0].size = mv_stride * (INPUT_IMAGE_HEIGHT + 1);
    buffers[BUF_MVOUT0].align = CISP_ALIGN;
    ret = R_STEST_MemAlloc(&g_handle_osalmmngr, &buffers[BUF_MVOUT0], devstr_cisp0);
    CHECK_RET("MemAlloc MVOUT0");

    buffers[BUF_MVOUT1].size = mv_stride * (INPUT_IMAGE_HEIGHT + 1);
    buffers[BUF_MVOUT1].align = CISP_ALIGN;
    ret = R_STEST_MemAlloc(&g_handle_osalmmngr, &buffers[BUF_MVOUT1], devstr_cisp0);
    CHECK_RET("MemAlloc MVOUT1");

    buffers[BUF_MVOUT2].size = mv_stride * (INPUT_IMAGE_HEIGHT + 1);
    buffers[BUF_MVOUT2].align = CISP_ALIGN;
    ret = R_STEST_MemAlloc(&g_handle_osalmmngr, &buffers[BUF_MVOUT2], devstr_cisp0);
    CHECK_RET("MemAlloc MVOUT2");

    buffers[BUF_MVOUT3].size = mv_stride * (INPUT_IMAGE_HEIGHT + 1);
    buffers[BUF_MVOUT3].align = CISP_ALIGN;
    ret = R_STEST_MemAlloc(&g_handle_osalmmngr, &buffers[BUF_MVOUT3], devstr_cisp0);
    CHECK_RET("MemAlloc MVOUT3");

    uint32_t hv_stride = 2048*2; //(INPUT_IMAGE_WIDTH) aligned to power of two (>=256)
    buffers[BUF_HVOUT0].size = hv_stride * (INPUT_IMAGE_HEIGHT + 1); /* 1 line overlap by V4H v2.0 H/W specification */
    buffers[BUF_HVOUT0].align = CISP_ALIGN;
    ret = R_STEST_MemAlloc(&g_handle_osalmmngr, &buffers[BUF_HVOUT0], devstr_cisp0);
    CHECK_RET("MemAlloc HVOUT0");

    buffers[BUF_HVOUT1].size = hv_stride * (INPUT_IMAGE_HEIGHT + 1); /* 1 line overlap by V4H v2.0 H/W specification */
    buffers[BUF_HVOUT1].align = CISP_ALIGN;
    ret = R_STEST_MemAlloc(&g_handle_osalmmngr, &buffers[BUF_HVOUT1], devstr_cisp0);
    CHECK_RET("MemAlloc HVOUT1");

    buffers[BUF_HIST256].size = HIST256_BUF_SIZE;
    buffers[BUF_HIST256].align = CISP_ALIGN;
    ret = R_STEST_MemAlloc(&g_handle_osalmmngr, &buffers[BUF_HIST256], devstr_cisp0);
    CHECK_RET("MemAlloc BUF_HIST256");

    buffers[BUF_DPCC0].size = DPCC_BUF_SIZE;
    buffers[BUF_DPCC0].align = CISP_ALIGN;
    ret = R_STEST_MemAlloc(&g_handle_osalmmngr, &buffers[BUF_DPCC0], devstr_cisp0);
    CHECK_RET("MemAlloc BUF_DPCC0");

    buffers[BUF_DPCC1].size = DPCC_BUF_SIZE;
    buffers[BUF_DPCC1].align = CISP_ALIGN;
    ret = R_STEST_MemAlloc(&g_handle_osalmmngr, &buffers[BUF_DPCC1], devstr_cisp0);
    CHECK_RET("MemAlloc BUF_DPCC1");

    isp_mem->num_mv_planes = 3; /* NOTE: SRGB8_PLANAR needs three planes */
    isp_mem->num_hv_planes = 2; /* NOTE: YUV422_SEMI needs two planes */

    for (int i=0; i < isp_mem->num_mv_planes; i++)
    {
        isp_mem->mv_planes[i].stride = mv_stride;
        isp_mem->mv_planes[i].width = INPUT_IMAGE_WIDTH;
        isp_mem->mv_planes[i].height = INPUT_IMAGE_HEIGHT;
        isp_mem->mv_planes[i].phys_addr = (uint32_t)buffers[BUF_MVOUT0+i].phy_addr;
        size_t buf_sz = isp_mem->mv_planes[i].height*mv_stride;
        DEBUG_PRINT(" MV%d  phys 0x%x  stride 0x%x  total %ld\n", i, isp_mem->mv_planes[i].phys_addr, mv_stride, buf_sz);
        memset( buffers[BUF_MVOUT0+i].virt_addr, 0x13, buf_sz);
        ret = R_STEST_MemFlush(&g_handle_osalmmngr, &buffers[BUF_MVOUT0+i]);
        CHECK_RET("MemFlush BUF_MVOUT");
    }

    for (int i=0; i < isp_mem->num_hv_planes; i++)
    {
        isp_mem->hv_planes[i].stride = hv_stride;
        isp_mem->hv_planes[i].width = INPUT_IMAGE_WIDTH;
        isp_mem->hv_planes[i].height = INPUT_IMAGE_HEIGHT;
        isp_mem->hv_planes[i].phys_addr = (uint32_t)buffers[BUF_HVOUT0+i].phy_addr;
        size_t buf_sz = isp_mem->hv_planes[i].height*hv_stride;
        DEBUG_PRINT(" HV%d  phys 0x%x  stride 0x%x  total %ld\n", i, isp_mem->hv_planes[i].phys_addr, hv_stride, buf_sz);
        memset( buffers[BUF_HVOUT0+i].virt_addr, 0x13, buf_sz);
        ret = R_STEST_MemFlush(&g_handle_osalmmngr, &buffers[BUF_HVOUT0+i]);
        CHECK_RET("MemFlush BUF_HVOUT");
    }
    isp_mem->hist256_phys_addr = (uint32_t)buffers[BUF_HIST256].phy_addr;
    isp_mem->hist256_virt_addr = (uint32_t *)buffers[BUF_HIST256].virt_addr;
    isp_mem->dppc0_phys_addr = (uint32_t)buffers[BUF_DPCC0].phy_addr;
    isp_mem->dppc0_virt_addr = (uint32_t *)buffers[BUF_DPCC0].virt_addr;
    isp_mem->dppc1_phys_addr = (uint32_t)buffers[BUF_DPCC1].phy_addr;
    isp_mem->dppc1_virt_addr = (uint32_t *)buffers[BUF_DPCC1].virt_addr;

}
/**********************************************************************************************************************
 End of function Init_Memories
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : Set_Memories */
/******************************************************************************************************************//**
 * @brief       Setting memories
 * @param[in]   dma_config
 * @param[out]  none
 * @retval      none
 * @retval      none
 *********************************************************************************************************************/
void Set_Memories(bool dma_config)
{
    if ( dma_config == ENABLED )
    {
        g_dma_config_data = buffers[BUF_CISPREGS].virt_addr;
        uint8_t ret = R_STEST_MemFlush(&g_handle_osalmmngr, &buffers[BUF_CISPREGS]);
        CHECK_RET("MemFlush BUF_CISPREGS");
    }
    else
    {
        g_dma_config_data = 0;
    }
}
/**********************************************************************************************************************
 End of function Set_Memories
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : Mem_Flush_DPCC0_CNF */
/******************************************************************************************************************//**
 * @brief       Memory flush DPCC0
 * @param[in]   none
 * @param[out]  none
 * @retval      none
 * @retval      none
 *********************************************************************************************************************/
void Mem_Flush_DPCC0_CNF()
{
    uint8_t ret = R_STEST_MemFlush(&g_handle_osalmmngr, &buffers[BUF_DPCC0]);
    CHECK_RET("MemFlush BUF_DPCC0");
}
/**********************************************************************************************************************
 End of function Mem_Flush_DPCC0_CNF
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : Mem_Flush_DPCC1_CNF */
/******************************************************************************************************************//**
 * @brief       Memory flush DPCC1
 * @param[in]   none
 * @param[out]  none
 * @retval      none
 * @retval      none
 *********************************************************************************************************************/
void Mem_Flush_DPCC1_CNF()
{
    uint8_t ret = R_STEST_MemFlush(&g_handle_osalmmngr, &buffers[BUF_DPCC1]);
    CHECK_RET("MemFlush BUF_DPCC1");
}
/**********************************************************************************************************************
 End of function Mem_Flush_DPCC1_CNF
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : Mem_Flush_HIST256_CNF */
/******************************************************************************************************************//**
 * @brief       Memory flush HIST256
 * @param[in]   none
 * @param[out]  none
 * @retval      none
 * @retval      none
 *********************************************************************************************************************/
void Mem_Flush_HIST256_CNF()
{
    uint8_t ret = R_STEST_MemFlush(&g_handle_osalmmngr, &buffers[BUF_HIST256]);
    CHECK_RET("MemFlush BUF_HIST256");
}
/**********************************************************************************************************************
 End of function Mem_Flush_HIST256_CNF
 *********************************************************************************************************************/

/*This function is draft code. This operation is confirmed when default setting only.*/

/**********************************************************************************************************************
/* Function Name : Save_Memories */
/******************************************************************************************************************//**
 * @brief       Save output Memories 
 * @param[in]   isp_out_memories_t* isp_mem
 * @param[out]  none
 * @retval      none
 * @retval      none
 *********************************************************************************************************************/
void Save_Memories(isp_out_memories_t* isp_mem)
{
    uint32_t x;
    uint32_t y;
    int ret;

    unsigned char * d_buf[2];
    d_buf[0] = (unsigned char *) malloc(g_frame_width * g_frame_height * BYTES_PER_PIXEL(INPUT_IMAGE_BPP));
    d_buf[1] = (unsigned char *) malloc(g_frame_width * g_frame_height * BYTES_PER_PIXEL(INPUT_IMAGE_BPP));

        for (uint32_t i = 0; i < isp_mem->num_hv_planes; i++)
        {
            R_STEST_MemInvalidate(&g_handle_osalmmngr, &buffers[BUF_HVOUT0+i]);

            for (y = 0; y < g_frame_height; y++)
            {
                memcpy(&d_buf[i][y*g_frame_width * BYTES_PER_PIXEL(INPUT_IMAGE_BPP)],(uint8_t*)buffers[BUF_HVOUT0+i].virt_addr + y * (uint32_t)isp_mem->hv_planes[i].stride, g_frame_width  * BYTES_PER_PIXEL(INPUT_IMAGE_BPP));
            }

        }

    R_FC_SyncStart(eVIN, &g_mtx_handle_vin_out, &g_vin_cond_handle, 1);

    y_uv2yuyv_8(gp_isp_buffer,d_buf[1], d_buf[0], g_frame_width ,g_frame_height);

    R_FC_SyncEnd(eVIN, &g_mtx_handle_vin_out, &g_vin_cond_handle, 1);

    free(d_buf[0]);
    free(d_buf[1]);
}
/**********************************************************************************************************************
 End of function Save_Memories
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : DeInit_Memories */
/******************************************************************************************************************//**
 * @brief       DeInit Memories 
 * @param[in]   none
 * @param[out]  none
 * @retval      None
 *********************************************************************************************************************/
void DeInit_Memories()
{
    /* NOTE: Need to de-allocate in inverse order ! */
    for (int i=NUM_BUFFERS-1; i>= 0; i--)
    {
        if (buffers[i].phy_addr) 
        {
            uint8_t ret = R_STEST_MemFree(&g_handle_osalmmngr, &buffers[i]);
            if (ret == R_UTILS_ERROR)
            {
                printf(" Failed to de-allocate buffers[%d]\n",i);
            }
        }
    }
}
/**********************************************************************************************************************
 End of function DeInit_Memories
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : Forced_Exit_Process */
/******************************************************************************************************************//**
 * @brief       Free memory until fopen 
 * @param[in]   none
 * @param[out]  none
 * @retval      None
 *********************************************************************************************************************/
void Forced_Exit_Process()
{
    /* NOTE: Need to de-allocate in inverse order ! */
    /* Free memory until fopen*/
    for (int i = BUF_VSPXDL; i >= 0; i--)
    {
        if (buffers[i].phy_addr)
        {
            uint8_t ret = R_STEST_MemFree(&g_handle_osalmmngr, &buffers[i]);
            if (ret == R_UTILS_ERROR)
            {
                printf(" Failed to de-allocate buffers[%d]\n",i);
            }
        }
    }
    exit(1);
}
/**********************************************************************************************************************
 End of function Forced_Exit_Process
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : Mem_Flush_VSPX_DL */
/******************************************************************************************************************//**
 * @brief       Memory flush VSPX module
 * @param[in]   none
 * @param[out]  none
 * @retval      none
 * @retval      none
 *********************************************************************************************************************/
void Mem_Flush_VSPX_DL()
{
    uint8_t ret = R_STEST_MemFlush(&g_handle_osalmmngr, &buffers[BUF_VSPXDL]);
    CHECK_RET("MemFlush BUF_VSPXDL");
}
/**********************************************************************************************************************
 End of function Mem_Flush_VSPX_DL
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : Mem_Flush_CISP_CNF */
/******************************************************************************************************************//**
 * @brief       Memory Flush CISP module
 * @param[in]   none
 * @param[out]  none
 * @retval      none
 * @retval      none           
 *********************************************************************************************************************/
void Mem_Flush_CISP_CNF()
{
    uint8_t ret = R_STEST_MemFlush(&g_handle_osalmmngr, &buffers[BUF_CISPREGS]);
    CHECK_RET("MemFlush BUF_CISPREGS");
}
/**********************************************************************************************************************
 End of function Mem_Flush_CISP_CNF
 *********************************************************************************************************************/
