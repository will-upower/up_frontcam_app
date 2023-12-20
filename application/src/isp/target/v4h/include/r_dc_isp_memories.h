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
* File Name    : r_dc_isp_memories.h
* Version      : 0.3.0
* Product Name : ISP Test Application
* Device(s)    : R-Car V4H
* Description  : utility functions for handling the memory buffers used by the ISP Sample Application
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : Version DD.MM.YYYY Description
*         : 0.1.0   20.01.2022 Initial version
*         : 0.2.0   16.01.2023 Added Forced_Exit_Process()
*         : 0.3.0   13.03.2023 Added macros of buffer
***********************************************************************************************************************/

#ifndef _R_ISP_MEMORIES_H_
#define _R_ISP_MEMORIES_H_     0

#include "r_dc_isp_utils.h"

#define BUF_EXPOSURE0   (0)
#define BUF_EXPOSURE1   (1)
#define BUF_CISPREGS    (2)
#define BUF_VSPXDL      (3)
#define BUF_MVOUT0      (4)
#define BUF_MVOUT1      (5)
#define BUF_MVOUT2      (6)
#define BUF_MVOUT3      (7)
#define BUF_HVOUT0      (8)
#define BUF_HVOUT1      (9)
#define BUF_HIST256     (10)
#define BUF_DPCC0       (11)
#define BUF_DPCC1       (12)
#define NUM_BUFFERS     (13)

typedef struct img_plane {
    uint32_t *virt_addr;
    uint32_t phys_addr;
    uint32_t stride;       /* in bytes */
    uint32_t width;
    uint32_t height;
    int format;     /* e_vspx_drv_read_pixel_format_t */
} img_plane_t;

#define MAX_ISP_PLANES (4)

typedef struct isp_out_memories {
    uint8_t num_hv_planes;
    uint8_t num_mv_planes;
    uint32_t hist256_phys_addr;
    uint32_t dppc0_phys_addr;
    uint32_t dppc1_phys_addr;
    uint32_t *dppc0_virt_addr;
    uint32_t *dppc1_virt_addr;
    uint32_t *hist256_virt_addr;
    img_plane_t hv_planes[MAX_ISP_PLANES];
    img_plane_t mv_planes[MAX_ISP_PLANES];
} isp_out_memories_t;

#define MAX_EXPOSURES (2)
typedef struct vspx_memories {
    uint8_t num_exposures;
    uint8_t vspx_plane_flags;
    img_plane_t exposures[MAX_EXPOSURES];
    img_plane_t cisp_config;
    img_plane_t dl_storage;
} vspx_memories_t;

void Init_Memories(vspx_memories_t* vspx_mem, isp_out_memories_t* isp_mem, st_isp_options_t *option);
void Buffer_Copy(vspx_memories_t* vspx_mem, st_isp_options_t *option);
void Set_Memories(bool dma_config);
void DeInit_Memories();
void Save_Memories(isp_out_memories_t* isp_mem);
void Forced_Exit_Process();

#endif /* _R_ISP_MEMORIES_H_ */
