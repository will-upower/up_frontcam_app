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
* File Name    : r_dc_isp_setup.h
* Version      : 0.2.0
* Product Name : ISP Test Application
* Device(s)    : R-Car V4H
* Description  : Configure the ISP drivers
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : Version DD.MM.YYYY Description
*         : 0.1.0   20.01.2022 Initial version
*         : 0.2.0   13.03.2023 Common Start_ISP() and Setup_ISP() functions in  SIL and default
***********************************************************************************************************************/
#ifndef _R_ISP_SETUP_H_
#define _R_ISP_SETUP_H_       0

#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>

#if (!ISP_SIL)
#include "rcar-xos/vspx/r_vspx.h"
#include "rcar-xos/tisp/r_tisp.h"
#endif
#include "rcar-xos/cisp/r_dc_cisp_rpp.h"

/*OSAL*/
#if defined(RCAR_XOS_HAVE_OSAL)
#include "rcar-xos/osal/r_osal.h"
#include "rcar-xos/osal/r_osal_api.h"
#endif

#include "r_dc_isp_memories.h"
#include "r_dc_isp_main.h"

e_isp_sample_return_t Start_ISP(st_cisp_drv_ctl_t* p_isp_ctl, isp_out_memories_t* meminfo, st_isp_options_t *option);
e_isp_sample_return_t Setup_ISP(st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width , uint16_t frame_height, st_isp_options_t *option);
#endif  /* _R_ISP_SETUP_H_ */
