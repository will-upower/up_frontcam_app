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
/**********************************************************************************************************************
 * File Name    : r_dc_isp_main.c
 * Version      : 0.1.0
 * Description  : Header file for ISP main file
 ********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 24.04.2023 0.1.0     0.1.0 Release (Base: R-CAR SDK V3.12 sample)
 *********************************************************************************************************************/
#ifndef _R_ISP_MAIN_H_
#define _R_ISP_MAIN_H_

typedef enum {
    R_ISP_OK,
    R_ISP_ERROR
}e_isp_sample_return_t;

#define OSAL_MUTEX_CISP_00         0x6000u
#define OSAL_THREAD_CISP_00        0x6000u
#define OSAL_MUTEX_CISP_01         0x6001u
#define OSAL_THREAD_CISP_01        0x6001u
#define OSAL_MUTEX_TISP_00         0x6002u
#define OSAL_MUTEX_TISP_01         0x6003u
#define OSAL_MUTEX_VSPX_00         0x6008u
#define OSAL_MUTEX_VSPX_01         0x6009u

#define RAW_IMAGE_CONST_TABLE      1

// Test image 1296x784 16bpp
#define FILE_IMAGE_WIDTH           1296
#define INPUT_IMAGE_WIDTH          1296
#define INPUT_IMAGE_HEIGHT         784

#define DPCC_BUF_SIZE              (2048 * 4) /* modify it if need */
#define HIST256_BUF_SIZE           (512 * 16) /* modify it if need */

#define INPUT_IMAGE_BPP            12
#define INPUT_IMAGE_VSPX_RDFMT     VSPX_DRV_RDFMT_RAW_2B
#define RAW_IMAGE_FNAME_EXP0       "template_bayer_image0.bin" /* this file name need to modify by user */
#define RAW_IMAGE_FNAME_EXP1       "template_bayer_image1.bin" /* this file name need to modify by user */

#define ENABLED                    true
#define DISABLED                   false

#endif  /* _R_ISP_MAIN_H_ */