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
* File Name    : r_dc_isp_setup.c
* Version      : 0.4.0
* Product Name : ISP Test Application
* Device(s)    : R-Car V4H
* Description  : Configure the ISP drivers
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : Version DD.MM.YYYY Description
*         : 0.1.0   17.01.2022 Initial version
*         : 0.2.0   16.01.2023 Changed processing related to DPCC pre2
*         : 0.3.0   13.02.2023 Removed compiler switch of each module and added enable/disable by option
*         : 0.4.0   13.03.2023 Added option that specify DMA mode, direct writing CPU mode and Display List mode
***********************************************************************************************************************/
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/stdio.h>  /* printf , */
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/string.h> /* memset, ...*/
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/stdlib.h>
#include <math.h>
#include <inttypes.h>

#if ISP_SIL
#include "r_isp_sim_setup.h"
#include "r_isp_sim_sample.h"
#endif

#include "rcar-xos/cisp/r_dc_cisp_rpp.h"
#include "rcar-xos/cisp/r_dc_cisp_rpp_version.h"

#include "rcar-xos/osal/r_osal_api.h"
#include "rcar-xos/osal/r_osal_types.h"

#include "r_dc_isp_config.h"
#include "common.h"
#include "r_dc_isp_utils.h"
#include "r_dc_isp_setup.h"
#if (!ISP_SIL)
#include "r_dc_isp_callbacks.h"
#endif

extern st_cisp_dma_config_data_t  *g_dma_config_data;

extern void Mem_Flush_CISP_CNF();

/* prototype: static function */
static e_cisp_drv_return_t SetupISP_ACQ(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_OUTIF(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_GAMMAIN(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_AWB(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_LTM(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_DPF(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_RGBDENOISE(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_SHRPCNR(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_LTMMEAS(st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width , uint16_t frame_height, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_RMAP(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_AWBMEAS(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_GAMMAOUT(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_EXM(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_FILT(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_HVOUTREGS(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_MVOUTREGS(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_HIST256(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_BLS(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_LSC(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_IS(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_CCOR(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_DPCC(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_CAC(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_HIST(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_RMAPMEAS(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_XYZ2LUV(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
// #endif  /* Default */
#if (ISP_SIL)
static e_cisp_drv_return_t SetupISP_RMAP(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_GAMMAIN(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_AWB(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_LTM(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_DPF(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_RGBDENOISE(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_SHRPCNR(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option);
static e_cisp_drv_return_t SetupISP_LTMMEAS(st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width , uint16_t frame_height, st_isp_options_t *option);
#endif /* SIL */
#if (!ISP_SIL)
/**********************************************************************************************************************/
/* Function Name : Start_ISP */
/******************************************************************************************************************//**
 * @brief       starting ISP
                [Covers: BPAP_FC_V4H_AD016][Covers: BPAP_FC_V4H_AD036]
 * @param[in]   st_cisp_drv_ctl_t* p_isp_ctl, isp_out_memories_t* meminfo, st_isp_options_t *option
 * @param[out]  none
 * @retval      R_ISP_OK
 * @retval      R_ISP_ERROR
 *********************************************************************************************************************/
e_isp_sample_return_t Start_ISP(st_cisp_drv_ctl_t* p_isp_ctl, isp_out_memories_t* meminfo, st_isp_options_t *option)
{
    // Start the execution of the CISP HW
    st_cisp_drv_exec_param_t    exec_param;
    memset(&exec_param, 0, sizeof(st_cisp_drv_exec_param_t));

    exec_param.input_pipelines_cnt                      = 1;
    exec_param.hv_out_on                                = meminfo->num_hv_planes > 0;
    exec_param.mv_out_on                                = meminfo->num_mv_planes > 0;
    exec_param.hist256_out_on                           = option->module_name.hist256;
    exec_param.dpcc_out_on[0]                           = option->module_name.dpcc_pre1;
    exec_param.dpcc_out_on[1]                           = option->module_name.dpcc_pre2;

    if ( option->module_name.hist256 == ENABLED )
    {
        if ( meminfo->hist256_phys_addr != 0 )
        {
            exec_param.hist256_hw_address                       = meminfo->hist256_phys_addr;
        }
        else
        {
            printf("[ERROR] Don't set up correctly physical address for HIST256.\n");
            DeInit_Memories();
            exit(1);
        }
    }
#if (!ISP_SIL)
    exec_param.callback                                 = &CISP_Event_callback;
#endif /* !SIL */

    exec_param.hv_format                                = CISP_HV_FORMAT_YUV422_SEMI_PLANAR;

    DEBUG_PRINT(" %s: hv %d mv %d hist %d dpcc0 %d dpcc1 %d\n",
        __func__, exec_param.hv_out_on, exec_param.mv_out_on,
        exec_param.hist256_out_on, exec_param.dpcc_out_on[0], exec_param.dpcc_out_on[1]  );

    for (uint8_t plane=0; plane<MAX_ISP_PLANES; plane++)
    {
        exec_param.hv_output_buffer[plane].buffer_hw_address    = meminfo->hv_planes[plane].phys_addr;
        exec_param.hv_output_buffer[plane].stride               = meminfo->hv_planes[plane].stride;
    }
    exec_param.mv_format                                = CISP_MV_FORMAT_SRGB_16_PLANAR; /* 16 bpp, 3 planes */

    for (uint8_t plane=0; plane<MAX_ISP_PLANES; plane++)
    {
        exec_param.mv_output_buffer[plane].buffer_hw_address    = meminfo->mv_planes[plane].phys_addr;
        exec_param.mv_output_buffer[plane].stride               = meminfo->mv_planes[plane].stride;
    }
    if ( exec_param.dpcc_out_on[0] == ENABLED)
    {
        if ( meminfo->dppc0_phys_addr != 0 )
        {
            exec_param.dpcc_hw_address[0]                       = meminfo->dppc0_phys_addr;
        }
        else
        {
            printf("[ERROR] Don't set up correctly physical address for DPCC Pre1.\n");
            DeInit_Memories();
            exit(1);
        }
    }

    if ( exec_param.dpcc_out_on[1] == ENABLED)
    {
        if ( meminfo->dppc1_phys_addr != 0 )
        {
            exec_param.dpcc_hw_address[1]                       = meminfo->dppc1_phys_addr;
        }
        else
        {
            printf("[ERROR] Don't set up correctly physical address for DPCC Pre2.\n");
            DeInit_Memories();
            exit(1);
        }
    }
    e_cisp_drv_return_t cisp_ret = R_CISP_Execute(p_isp_ctl, exec_param);
    CHECK_PRINT_CISP_ERR("CISP Execute", cisp_ret);
    if (cisp_ret == CISP_DRV_RET_OK)
    {
        DEBUG_PRINT(" CISP Started\n");
    }

    return (cisp_ret == CISP_DRV_RET_OK) ? R_ISP_OK : R_ISP_ERROR;
}
/**********************************************************************************************************************
 End of function Start_ISP
 *********************************************************************************************************************/
#endif  /* !ISP_SIL */

#if (!ISP_SIL)
/**********************************************************************************************************************/
/* Function Name : Setup_ISP */
/******************************************************************************************************************//**
 * @brief       Setup the entire ISP processing chain
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width , uint16_t frame_height, st_isp_options_t *option
 * @param[out]  none
 * @retval      u16ret_status
 * @retval      none
 *********************************************************************************************************************/
e_isp_sample_return_t Setup_ISP(st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width , uint16_t frame_height, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP(st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width , uint16_t frame_height)
     */
    e_isp_sample_return_t   ret_status  =   R_ISP_OK;
    e_cisp_drv_return_t     ret         =   CISP_DRV_RET_OK;

    if( option->dma == ENABLED )
    {
        uint8_t unit_index = option->channel;
        R_CISP_InitDmaConfigData(unit_index, g_dma_config_data);
    }

    // configure input aquisition stages of the rpp-pre-processing pipelines
    ret = SetupISP_ACQ(p_isp_ctl, option);
    // configure output cropping stage of the rpp-out-processing pipelines (use same window as input-acq) and enable HV output
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_OUTIF(p_isp_ctl, option);
    }
    // switching data-path
    // bypass radiance mapping using long
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_RMAP(p_isp_ctl, option);
    }
    // configuring color-space conversion improve brightness and contrast here.
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_CCOR(p_isp_ctl, option);
    }
    // applying PWL decompression curve
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_GAMMAIN(p_isp_ctl, option);
    }
    // applying gamma-out curve
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_GAMMAOUT(p_isp_ctl, option);
    }
    // applying white-balance gains
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_AWB(p_isp_ctl, option);
    }
    // awb statistics (RGB statistics)
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_AWBMEAS(p_isp_ctl, option);
    }
    // exm statistics (RGB statistics)
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_EXM(p_isp_ctl, option);
    }
    // configure LTM
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_LTM(p_isp_ctl, option);
    }
    // enable ltm measurement
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_LTMMEAS(p_isp_ctl,frame_width, frame_height, option);
    }
    // demosaicer configuration
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_FILT(p_isp_ctl, option);
    }
    // denoising, sharpening, chroma noise reduction and chroma desaturation
    // apply RGB->YUV conversion (REC.709, full-range) and setup densoing
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_RGBDENOISE(p_isp_ctl, option);
    }
    // apply YUV->RGB conversion (REC.709, full-range) and setup sharpening / chroma enhancement
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_SHRPCNR(p_isp_ctl, option);
    }
    // Configure Human Vision output to one of the supported formats.
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_HVOUTREGS(p_isp_ctl, option);
        if (ret != CISP_DRV_RET_OK)
        {
            ret_status = R_ISP_ERROR;
        }
    }
    // Configure Machine Vision output to one of the supported formats.
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_MVOUTREGS(p_isp_ctl, option);
        if (ret != CISP_DRV_RET_OK)
        {
            ret_status = R_ISP_ERROR;
        }
    }
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_HIST256(p_isp_ctl, option);
    }
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_BLS(p_isp_ctl, option);
    }
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_DPF(p_isp_ctl, option);
    }
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_LSC(p_isp_ctl, option);
    }
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_IS(p_isp_ctl, option);
    }
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_DPCC(p_isp_ctl, option);
    }
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_CAC(p_isp_ctl, option);
    }
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_HIST(p_isp_ctl, option);
    }
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_RMAPMEAS(p_isp_ctl, option);
    }
    if (ret == CISP_DRV_RET_OK)
    {
        ret = SetupISP_XYZ2LUV(p_isp_ctl, option);
    }
    if ( option->dma == ENABLED )
    {
        ret = R_CISP_FinalizeDmaConfigData(g_dma_config_data);
        Mem_Flush_CISP_CNF();
    }
    if (ret != CISP_DRV_RET_OK)
    {
        ret_status = R_ISP_ERROR;
    }

    return ret_status;
}
/**********************************************************************************************************************
 End of function Setup_ISP
 *********************************************************************************************************************/
#else /* SIL */

/**********************************************************************************************************************/
/* Function Name : Setup_ISP */
/******************************************************************************************************************//**
 * @brief       Setup ISP
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width , uint16_t frame_height, st_isp_options_t *option
 * @param[out]  none
 * @retval      u16ret_status
 * @retval      none
 *********************************************************************************************************************/
e_isp_sample_return_t Setup_ISP(st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width , uint16_t frame_height, st_isp_options_t *option)
{
    e_isp_sample_return_t   ret_status  =   R_ISP_OK;
    e_cisp_drv_return_t     ret         =   CISP_DRV_RET_OK;

    /* The following process is a custom test program that simply starts the simulator server for V4H
     * and checks if some registers can be accessed.
     */
    ret = R_CISP_setReg_SIM_Init(p_isp_ctl);
    if (ret == CISP_DRV_RET_OK)
    {
        DEBUG_PRINT("Init finished!\n");
    }
    ret = R_CISP_setReg_SIM_ACQ(p_isp_ctl);
    if (ret == CISP_DRV_RET_OK)
    {
        DEBUG_PRINT("ACQ finished!\n");
    }
    ret = R_CISP_setReg_SIM_RPP_OUT_IS(p_isp_ctl);
    if (ret == CISP_DRV_RET_OK)
    {
        DEBUG_PRINT("RPP_OUT_IS finished!\n");
    }
    ret = SetupISP_RMAP(p_isp_ctl, option);
    if (ret == CISP_DRV_RET_OK)
    {
        DEBUG_PRINT("RMAP finished!\n");
    }
    ret = R_CISP_setReg_SIM_CCOR(p_isp_ctl);
    if (ret == CISP_DRV_RET_OK)
    {
        DEBUG_PRINT("CCOR finished!\n");
    }
    ret = SetupISP_GAMMAIN(p_isp_ctl, option);
    if (ret == CISP_DRV_RET_OK)
    {
        DEBUG_PRINT("GAMMAIN finished!\n");
    }
    ret = R_CISP_setReg_SIM_GAMMAOUT(p_isp_ctl);
    if (ret == CISP_DRV_RET_OK)
    {
        DEBUG_PRINT("GAMMAOUT finished!\n");
    }

    ret = R_CISP_setReg_SIM_CCOR_COEFF(p_isp_ctl);
    if (ret == CISP_DRV_RET_OK)
    {
        DEBUG_PRINT("CCOR_COEFF finished!\n");
    }
    ret = SetupISP_AWB(p_isp_ctl, option);
    if (ret == CISP_DRV_RET_OK) {
        ret = R_CISP_setReg_SIM_AWB(p_isp_ctl);
        if (ret == CISP_DRV_RET_OK)
        {
            DEBUG_PRINT("AWB finished!\n");
        }
    }
    ret = R_CISP_setReg_SIM_AWB_MEAS(p_isp_ctl);
    if (ret == CISP_DRV_RET_OK)
    {
        DEBUG_PRINT("AWB_MEAS finished!\n");
    }
    ret = SetupISP_LTM(p_isp_ctl, option);
    if (ret == CISP_DRV_RET_OK) {
        ret = R_CISP_setReg_SIM_LTM(p_isp_ctl);
        if (ret == CISP_DRV_RET_OK)
        {
            DEBUG_PRINT("LTM finished!\n");
        }
    }
    ret = SetupISP_LTMMEAS(p_isp_ctl, frame_width, frame_height, option);
    if (ret == CISP_DRV_RET_OK) {
        ret = R_CISP_setReg_SIM_LTM_MEAS(p_isp_ctl);
        if (ret == CISP_DRV_RET_OK)
        {
            DEBUG_PRINT("LTM_MEAS finished!\n");
        }
    }
    ret = SetupISP_DPF(p_isp_ctl, option);
    if (ret == CISP_DRV_RET_OK) {
        ret = R_CISP_setReg_SIM_BD(p_isp_ctl);
        if (ret == CISP_DRV_RET_OK)
        {
            DEBUG_PRINT("BD finished!\n");
        }
    }
    ret = SetupISP_RGBDENOISE(p_isp_ctl, option);
    if (ret == CISP_DRV_RET_OK) {
        ret = R_CISP_setReg_SIM_BD_RGB(p_isp_ctl);
        if (ret == CISP_DRV_RET_OK)
        {
            DEBUG_PRINT("BD_RGB finished!\n");
        }
    }
    ret = SetupISP_SHRPCNR(p_isp_ctl, option);
    if (ret == CISP_DRV_RET_OK) {
        ret = R_CISP_setReg_SIM_SHRPCNR(p_isp_ctl);
        if (ret == CISP_DRV_RET_OK)
        {
            DEBUG_PRINT("SHRPCNR finished!\n");
        }
    }
    R_CISP_setReg_SIM(p_isp_ctl);

    return ret_status;
}
/**********************************************************************************************************************
 End of function Setup_ISP
 *********************************************************************************************************************/
#endif /* !SIL */

/**********************************************************************************************************************/
/* Function Name : ToCispReturnValueString */
/******************************************************************************************************************//**
 * @brief       CISP ReturnValue String
 * @param[in]   e_cisp_drv_return_t ret
 * @param[out]  none
 * @retval      names[i].name
 * @retval      none
 *********************************************************************************************************************/
const char *ToCispReturnValueString(e_cisp_drv_return_t ret)
{
    struct RETURN_VALUE_TO_STRING_TABLE {
        e_cisp_drv_return_t ret;
        const char *name;
    };

    #define ENUM_TO_STRING(a)   { a, #a }
    static const struct RETURN_VALUE_TO_STRING_TABLE names[] =
    {
        ENUM_TO_STRING(CISP_DRV_RET_OK),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_CONF),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_NULL_POINTER),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_UNIT_INDEX),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_WRONG_STATE),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_INTERNAL),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_HW_VERSION),

        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_MUTEX_CREATE),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_MUTEX_DESTROY),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_MUTEX_LOCK),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_MUTEX_UNLOCK),

        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_IO_DEV_OPEN),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_IO_DEV_CLOSE),

        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_REG_ISR),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_UNREG_ISR),

        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_PM_SET),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_PM_RELEASE),

        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_ENABLE_ISR),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_DISABLE_ISR),

        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_IO_READ),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_IO_WRITE),

        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_SET_PM_POL),
        ENUM_TO_STRING(CISP_DRV_RET_ERR_OSAL_GET_PM_POL),
    };

    for (uint32_t i = 0; i < sizeof(names)/sizeof(names[0]); ++i) {
        if (names[i].ret == ret) {
            return names[i].name;
        }
    }

    return "NOT Registered !!";
}
/**********************************************************************************************************************
 End of function ToCispReturnValueString
 *********************************************************************************************************************/

#if (!ISP_SIL)
/**********************************************************************************************************************/
/* Function Name : SetupISP_ACQ */
/******************************************************************************************************************//**
 * @brief       Set up ISP ACQ
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_ACQ(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_ACQ(st_cisp_drv_ctl_t *p_isp_ctl, uint8_t ch_num, , uint16_t frame_width , uint16_t frame_height)
     */
    e_cisp_drv_return_t     ret;
    st_cisp_acq_config_t    isp_acq_config;
    uint8_t                 cnt;
    uint8_t                 pipeline = 0;

    memset(&isp_acq_config, 0, sizeof(st_cisp_acq_config_t));

    for(cnt = 0; cnt < 2; cnt++)
    {
        if(cnt == 0)
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_1;
        }
        else
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_2;
        }

        ret=R_CISP_ACQ_GetConfig(p_isp_ctl, pipeline, &isp_acq_config);
        CHECK_PRINT_CISP_ERR("CISP_ACQ_GetConfig", ret);
        if(CISP_DRV_RET_OK == ret)
        {
            switch ( pipeline )
            {

                case CISP_PIPELINE_PRE_FUSION_1:
                    if ( (option->hdr_fusion == ENABLED) || (option->module_name.dpcc_pre2 == ENABLED))
                    {           /* ACQ HDR fusion */
                        isp_acq_config = acq_pre1_hdr_config_table;
                    }
                    else
                    {           /* Default setting */
                        isp_acq_config = acq_pre1_enable_config_table;
                        switch(g_customize.ISP_RAW_IN_Format)
                        {
                            case 0:
                            //RGGB
                            isp_acq_config.acq_properties.bayer_pattern = CISP_ACQ_BAYPAT_RGGB;
                            break;
                            case 1:
                            //GRBG
                            isp_acq_config.acq_properties.bayer_pattern = CISP_ACQ_BAYPAT_GRBG;    
                            break;
                            case 2:
                            //GBRG
                            isp_acq_config.acq_properties.bayer_pattern = CISP_ACQ_BAYPAT_GBRG; 
                            break; 
                            case 3:
                            //BGGR
                            isp_acq_config.acq_properties.bayer_pattern = CISP_ACQ_BAYPAT_BGGR; 
                            break;
                            default:
                            break;
                        }
                    }
                break;
                case CISP_PIPELINE_PRE_FUSION_2:
                    if ( (option->hdr_fusion == ENABLED) || (option->module_name.dpcc_pre2 == ENABLED) )
                    {           /* ACQ HDR fusion */
                        isp_acq_config = acq_pre2_hdr_config_table;
                    }
                    else
                    {           /* Default setting */
                        isp_acq_config = acq_pre2_disable_config_table;
                    }
                break;
                default:
                break;
            }
            if ( option->dma == ENABLED)
            {
                ret = R_CISP_ACQ_AddToDmaConfigData(g_dma_config_data, pipeline, &isp_acq_config);
            }
            else
            {
                ret = R_CISP_ACQ_SetConfig(p_isp_ctl, pipeline, &isp_acq_config);
            }
            if(CISP_DRV_RET_OK != ret)
            {
                printf("Failed: Set the configuration of the ACQ module contained. Error Code %s(%d)/Pipeline %d",
                       ToCispReturnValueString(ret), (int32_t)ret, pipeline);
            }
        }
        else
        {
            printf("Failed: Get the configuration of the ACQ module contained. Error Code %s(%d)/Pipeline %d",
                   ToCispReturnValueString(ret), (int32_t)ret, pipeline);
        }
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_ACQ
 *********************************************************************************************************************/

/**********************************************************************************************************************/
/* Function Name : SetupISP_OUTIF */
/******************************************************************************************************************//**
 * @brief       Setup ISP outif
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_OUTIF(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    e_cisp_drv_return_t     ret;
    st_cisp_outif_config_t  isp_outif_config;
    uint8_t                 cnt;
    uint8_t                 pipeline = 0;

    memset(&isp_outif_config, 0, sizeof(st_cisp_outif_config_t));

    for(cnt = 0; cnt < 2; cnt++)
    {
        if(cnt == 0)
        {
            pipeline = CISP_PIPELINE_HV_OUT;
        }
        else
        {
            pipeline = CISP_PIPELINE_MV_OUT;
        }
        ret = R_CISP_OUTIF_GetConfig (p_isp_ctl, pipeline, &isp_outif_config);
        if(ret == CISP_DRV_RET_OK)
        {
            if ( option->cpu == ENABLED && option->continous_frame == ENABLED )
            {
                isp_outif_config = outif_config_continous_cpu_table;
            }
            else
            {
                isp_outif_config = outif_config_table;
            }

            if ( option->dma == ENABLED )
            {
                ret = R_CISP_OUTIF_AddToDmaConfigData(g_dma_config_data, pipeline, &isp_outif_config);
            }
            else
            {
                ret = R_CISP_OUTIF_SetConfig (p_isp_ctl, pipeline, &isp_outif_config);
            }
            if(ret != CISP_DRV_RET_OK)
            {
                printf("Failed: Set the configuration of the OUTIF module. Error Code %s(%d)/Pipeline %d\n",
                       ToCispReturnValueString(ret), (int32_t)ret, pipeline);
            }
        }
        else
        {
            printf("Failed: Get the configuration of the OUTIF module. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_OUTIF
 *********************************************************************************************************************/
#endif /* !SIL */

/**********************************************************************************************************************/
/* Function Name : SetupISP_RMAP */
/******************************************************************************************************************//**
 * @brief       Setup ISP RMAP
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_RMAP(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    e_cisp_drv_return_t    ret;
    st_cisp_rmap_config_t  isp_rmap_config;
    memset(&isp_rmap_config, 0, sizeof(st_cisp_rmap_config_t));

    ret = R_CISP_RMAP_GetConfig (p_isp_ctl, &isp_rmap_config);
    if(ret == CISP_DRV_RET_OK)
    {
        if ( (option->hdr_fusion == ENABLED) || (option->module_name.dpcc_pre2 == ENABLED) )
        {           /* RMAP HDR fusion*/
            isp_rmap_config = rmap_hdr_config_table;
        }
        else
        {           /* RMAP config*/
            isp_rmap_config = rmap_config_table;
        }
#if (!ISP_SIL)
        if ( option->dma == ENABLED )
        {
            ret = R_CISP_RMAP_AddToDmaConfigData(g_dma_config_data, &isp_rmap_config);
        }
        else
        {
            ret = R_CISP_RMAP_SetConfig (p_isp_ctl, &isp_rmap_config);
        }
#else
        ret = R_CISP_RMAP_SetConfig (p_isp_ctl, &isp_rmap_config);
#endif  /* !ISP_SIL*/

        if(ret != CISP_DRV_RET_OK)
        {
            printf("Failed: Set the configuration of the RMAP module. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    else
    {
        printf("Failed: Get the configuration of the RMAP module. Error Code %s(%d)\n",
               ToCispReturnValueString(ret), (int32_t)ret);
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_RMAP
 *********************************************************************************************************************/


/**********************************************************************************************************************/
/* Function Name : SetupISP_GAMMAIN */
/******************************************************************************************************************//**
 * @brief       Setup ISP GAMMAIN
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_GAMMAIN(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_GAMMAIN(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_gammain_config_t    isp_gammain_config;
    uint8_t                     cnt;
    uint8_t                     pipeline = 0;

    memset(&isp_gammain_config, 0, sizeof(st_cisp_gammain_config_t));
    for (uint16_t i = 0; i < CISP_GAMMAIN_CFG_NUM_SAMPLES; i++)
    {
        memset(&isp_gammain_config.curve[i], 0, sizeof(st_cisp_gammain_curve_sample_t));
    }

    for(cnt = 0; cnt < 2; cnt++)
    {
        if(cnt == 0)
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_1;
        }
        else
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_2;
        }

        ret = R_CISP_GAMMAIN_GetConfig (p_isp_ctl, pipeline, &isp_gammain_config);
        if(CISP_DRV_RET_OK == ret)
        {
            switch( pipeline )
            {
                
                case CISP_PIPELINE_PRE_FUSION_1:
                    if ( option->module_name.gammain == ENABLED)
                    {
                        /* GAAMAIN enable*/
                        isp_gammain_config = gammain_pre1_enable_config_table;
                    }
                    else
                    {
                        /* GAMMAIN disable*/
                        isp_gammain_config = gammain_pre1_disable_config_table;
                    }
                break;
                case CISP_PIPELINE_PRE_FUSION_2:
                    isp_gammain_config = gammain_pre2_disable_config_table;
                break;
                default:
                break;
            }
#if (!ISP_SIL)
            if ( option->dma == ENABLED )
            {
                ret = R_CISP_GAMMAIN_AddToDmaConfigData(g_dma_config_data, pipeline, &isp_gammain_config);
            }
            else
            {
                ret = R_CISP_GAMMAIN_SetConfig (p_isp_ctl, pipeline, &isp_gammain_config);
            }
#else
            ret = R_CISP_GAMMAIN_SetConfig (p_isp_ctl, pipeline, &isp_gammain_config);
#endif  /* !ISP_SIL */

            if(CISP_DRV_RET_OK != ret)
            {
                printf("Failed: Set the configuration of the GAMMAIN module contained. Error Code %s(%d)/Pipeline %d\n",
                       ToCispReturnValueString(ret), (int32_t)ret, pipeline);
            }
        }
        else
        {
            printf("Failed: Get the configuration of the GAMMAIN module contained. Error Code %s(%d)/Pipeline %d\n",
                   ToCispReturnValueString(ret), (int32_t)ret, pipeline);
        }
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_GAMMAIN
 *********************************************************************************************************************/

#if (!ISP_SIL)
/**********************************************************************************************************************/
/* Function Name : SetupISP_GAMMAOUT */
/******************************************************************************************************************//**
 * @brief       Setup ISP GAMMAOUT
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_GAMMAOUT(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_GAMMAOUT(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret = CISP_DRV_RET_OK;
    st_cisp_gammaout_config_t   isp_gammaout_config;
    uint8_t                     cnt;
    uint8_t                     pipeline = 0;

    memset(&isp_gammaout_config, 0, sizeof(st_cisp_gammaout_config_t));

    for(cnt = 0; cnt < 2; cnt++)
    {
        if(cnt == 0)
        {
            pipeline = CISP_PIPELINE_HV_OUT;
        }
        else
        {
            pipeline = CISP_PIPELINE_MV_OUT;
        }

        ret = R_CISP_GAMMAOUT_GetConfig (p_isp_ctl, pipeline, &isp_gammaout_config);
        if(CISP_DRV_RET_OK == ret)
        {
            switch( pipeline )
            {
                case CISP_PIPELINE_HV_OUT:
                    if ( (option->hdr_fusion == ENABLED) || (option->module_name.dpcc_pre2 == ENABLED) )
                    {           /* GAMMAOUT HDR fusion*/
                        isp_gammaout_config = gammaout_hdr_config_table;
                    }
                    else if ( (option->module_name.gammaout == ENABLED) && (option->hdr_fusion == DISABLED))
                    {           /* GAMMAOUT enable*/
                        isp_gammaout_config = gammaout_hvout_enable_config_table;
                    }
                    else
                    {           /* GAMMAOUT hv/mvout disable*/
                        isp_gammaout_config = gammaout_hvout_disable_config_table;
                    }
                break;
                case CISP_PIPELINE_MV_OUT:
                    if ( (option->hdr_fusion == ENABLED) || (option->module_name.dpcc_pre2 == ENABLED) )
                    {           /* GAMMAOUT HDR fusion*/
                        isp_gammaout_config = gammaout_hdr_config_table;
                    }
                    else if ( (option->module_name.gammaout == ENABLED) && (option->hdr_fusion == DISABLED))
                    {           /* GAMMAOUT enable*/
                        isp_gammaout_config = gammaout_mvout_enable_config_table;
                    }
                    else
                    {           /* GAMMAOUT hv/mvout disable*/
                        isp_gammaout_config = gammaout_mvout_disable_config_table;
                    }
                break;
                default:
                break;
            }

            if ( option->dma == ENABLED )
            {
                ret = R_CISP_GAMMAOUT_AddToDmaConfigData (g_dma_config_data, pipeline, &isp_gammaout_config);
            }
            else
            {
                ret = R_CISP_GAMMAOUT_SetConfig (p_isp_ctl, pipeline, &isp_gammaout_config);
            }

            if(CISP_DRV_RET_OK != ret)
            {
                printf("Failed: Set the configuration of the GAMMAOUT module contained. Error Code %s(%d)/Pipeline %d\n",
                       ToCispReturnValueString(ret), (int32_t)ret, pipeline);
            }
        }
        else
        {
            printf("Failed: Get the configuration of the GAMMAOUT module contained. Error Code %s(%d)/Pipeline %d\n",
                   ToCispReturnValueString(ret), (int32_t)ret, pipeline);
        }
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_GAMMAOUT
 *********************************************************************************************************************/
#endif /* !SIL */

/**********************************************************************************************************************/
/* Function Name : SetupISP_AWB */
/******************************************************************************************************************//**
 * @brief       Setup ISP AWB
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_AWB(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_WAB(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_awb_config_t        isp_awb_config;
    uint8_t                     cnt;
    uint8_t                     pipeline = 0;

    memset(&isp_awb_config, 0, sizeof(st_cisp_awb_config_t));

    for(cnt = 0; cnt < 3; cnt++)
    {
        if(cnt == 0)
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_1;
        }
        else if(cnt == 1)
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_2;
        }
        else
        {
            pipeline = CISP_PIPELINE_POST_FUSION;
        }

        ret = R_CISP_AWB_GetConfig (p_isp_ctl, pipeline, &isp_awb_config);
        if (CISP_DRV_RET_OK == ret)
        {
            switch (pipeline)
            {
                case CISP_PIPELINE_PRE_FUSION_1:
                    if ( (option->hdr_fusion == ENABLED) || (option->module_name.dpcc_pre2 == ENABLED) )
                    {           /* AWB HDR fusion*/
                        isp_awb_config = awb_pre1_hdr_config_table;
                    }
                    else if ( (option->module_name.awb == ENABLED) && (option->hdr_fusion == DISABLED) )
                    {           /* AWB enable*/
                        isp_awb_config = awb_pre1_enable_config_table;
                    }
                    else
                    {           /* AWB disable*/
                        isp_awb_config = awb_pre1_disable_config_table;
                    }
                break;

                case CISP_PIPELINE_PRE_FUSION_2:
                    if ( (option->hdr_fusion == ENABLED) || (option->module_name.dpcc_pre2 == ENABLED) )
                    {           /* AWB HDR fusion*/
                        isp_awb_config = awb_pre2_hdr_config_table;
                    }
                    else
                    {           /* AWB disable*/
                        isp_awb_config = awb_pre2_disable_config_table;
                    }
                break;

                case CISP_PIPELINE_POST_FUSION:
                    if ( (option->hdr_fusion == ENABLED) || (option->module_name.dpcc_pre2 == ENABLED) )
                    {           /* AWB HDR fusion*/
                        isp_awb_config = awb_post_hdr_config_table;
                    }
                    else if ( (option->module_name.awb == ENABLED) && (option->hdr_fusion == DISABLED) )
                    {           /* AWB enable*/
                        isp_awb_config = awb_post_enable_config_table;
                    }
                    else
                    {           /* AWB disable*/
                        isp_awb_config = awb_post_disable_config_table;
                    }
                break;
                default:
                break;
            }
#if (!ISP_SIL)
            if ( option->dma == ENABLED )
            {
                ret = R_CISP_AWB_AddToDmaConfigData(g_dma_config_data, pipeline, &isp_awb_config);
            }
            else
            {
                ret = R_CISP_AWB_SetConfig (p_isp_ctl, pipeline, &isp_awb_config);
            }
#else
            ret = R_CISP_AWB_SetConfig (p_isp_ctl, pipeline, &isp_awb_config);
#endif

            if(CISP_DRV_RET_OK != ret)
            {
                printf("Failed: Set the configuration of the AWB module contained. Error Code %s(%d)/Pipeline %d\n",
                       ToCispReturnValueString(ret), (int32_t)ret, pipeline);
            }
        }
        else
        {
            printf("Failed: Get the configuration of the AWB module contained. Error Code %s(%d)/Pipeline %d\n",
                   ToCispReturnValueString(ret), (int32_t)ret, pipeline);
        }
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_AWB
 *********************************************************************************************************************/

#if (!ISP_SIL)
/**********************************************************************************************************************/
/* Function Name : SetupISP_AWBMEAS */
/******************************************************************************************************************//**
 * @brief       Setup ISP AWBMEAS
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_AWBMEAS(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_AWBMEAS(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_awbmeas_config_t    isp_awbmeas_config;

    memset(&isp_awbmeas_config, 0, sizeof(st_cisp_awbmeas_config_t));

    ret = R_CISP_AWBMEAS_GetConfig (p_isp_ctl, &isp_awbmeas_config);
    if(CISP_DRV_RET_OK == ret)
    {
        if ( option->module_name.awbmeas == ENABLED )
        {
            isp_awbmeas_config = awbmeas_enable_config_table;
        }
        else
        {
            isp_awbmeas_config = awbmeas_disable_config_table;
        }

        if ( option->dma == ENABLED )
        {
            ret = R_CISP_AWBMEAS_AddToDmaConfigData(g_dma_config_data, &isp_awbmeas_config);
        }
        else
        {
            ret = R_CISP_AWBMEAS_SetConfig (p_isp_ctl, &isp_awbmeas_config);
        }

        if(CISP_DRV_RET_OK != ret)
        {
            printf("Failed: Set the configuration of the AWBMEAS module contained. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    else
    {
        printf("Failed: Get the configuration of the AWBMEAS module contained. Error Code %s(%d)\n",
               ToCispReturnValueString(ret), (int32_t)ret);
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_AWBMEAS
 *********************************************************************************************************************/

/**********************************************************************************************************************/
/* Function Name : SetupISP_EXM */
/******************************************************************************************************************//**
 * @brief       Setup ISP EXM
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/ 
static e_cisp_drv_return_t SetupISP_EXM(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_EXM(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_exm_config_t        isp_exm_config;
    uint8_t                     cnt;
    uint8_t                     pipeline = 0;

    memset(&isp_exm_config, 0, sizeof(st_cisp_exm_config_t));

    for(cnt = 0; cnt < 2; cnt++)
    {
        if(cnt == 0)
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_1;
        }
        else
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_2;
        }

        ret = R_CISP_EXM_GetConfig (p_isp_ctl, pipeline, &isp_exm_config);
        if(CISP_DRV_RET_OK == ret)
        {
            switch( pipeline )
            {
                case CISP_PIPELINE_PRE_FUSION_1:
                    if ( option->module_name.exm == ENABLED )
                    {           /* EXM enable*/
                        isp_exm_config = exm_pre1_enable_config_table;
                    }
                    else
                    {           /* EXM disable*/
                        isp_exm_config = exm_pre1_disable_config_table;
                    }
                break;

                case CISP_PIPELINE_PRE_FUSION_2:
                        isp_exm_config = exm_pre2_disable_config_table;
                break;

                default:
                break;
            }

            if ( option->dma == ENABLED )
            {
                ret = R_CISP_EXM_AddToDmaConfigData(g_dma_config_data, pipeline, &isp_exm_config);
            }
            else
            {
                ret = R_CISP_EXM_SetConfig (p_isp_ctl, pipeline, &isp_exm_config);
            }

            if(CISP_DRV_RET_OK != ret)
            {
                printf("Failed: Set the configuration of the EXM module contained. Error Code %s(%d)/Pipeline %d\n",
                       ToCispReturnValueString(ret), (int32_t)ret, pipeline);
            }
        }
        else
        {
            printf("Failed: Get the configuration of the EXM module contained. Error Code %s(%d)/Pipeline %d\n",
                   ToCispReturnValueString(ret), (int32_t)ret, pipeline);
        }
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_EXM
 *********************************************************************************************************************/
#endif /* !SIL */

/**********************************************************************************************************************/
/* Function Name : SetupISP_LTM */
/******************************************************************************************************************//**
 * @brief       Setup ISP LTM
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_LTM(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_LTM(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret = CISP_DRV_RET_OK;
    st_cisp_ltm_config_t        isp_ltm_config;

    memset(&isp_ltm_config, 0, sizeof(st_cisp_ltm_config_t));

    ret = R_CISP_LTM_GetConfig (p_isp_ctl, &isp_ltm_config);
    if(CISP_DRV_RET_OK == ret)
    {
        if ( (option->hdr_fusion == ENABLED) || (option->module_name.dpcc_pre2 == ENABLED) )
        {           /* LTM HDR fusion*/
            isp_ltm_config = ltm_hdr_config_table;
        }
        else if ( (option->hdr_fusion == DISABLED) && (option->module_name.ltm == ENABLED) )
        {           /* LTM enable*/
            isp_ltm_config = ltm_enable_config_table;
        }
        else
        {           /* LTM disable*/
            isp_ltm_config = ltm_disable_config_table;
        }
#if (!ISP_SIL)
        if ( option->dma == ENABLED )
        {
            ret = R_CISP_LTM_AddToDmaConfigData(g_dma_config_data, &isp_ltm_config);
        }
        else
        {
            ret = R_CISP_LTM_SetConfig (p_isp_ctl, &isp_ltm_config);
        }
#else
        ret = R_CISP_LTM_SetConfig (p_isp_ctl, &isp_ltm_config);
#endif  /* !ISP_SIL */

        if(CISP_DRV_RET_OK != ret)
        {
            printf("Failed: Set the configuration of the LIM module contained. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    else
    {
        printf("Failed: Get the configuration of the LIM module contained. Error Code %s(%d)\n",
               ToCispReturnValueString(ret), (int32_t)ret);
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_LTM
 *********************************************************************************************************************/

/**********************************************************************************************************************/
/* Function Name : SetupISP_LTMMEAS */
/******************************************************************************************************************//**
 * @brief       Setup ISP LTMMEAS
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width , uint16_t frame_height, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_LTMMEAS(st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width , uint16_t frame_height,
                                                                                             st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_LIMMEAS(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret = CISP_DRV_RET_OK;
    st_cisp_ltmmeas_config_t    isp_ltmmeas_config;

    memset(&isp_ltmmeas_config, 0, sizeof(st_cisp_ltmmeas_config_t));

    ret = R_CISP_LTMMEAS_GetConfig(p_isp_ctl, &isp_ltmmeas_config);
    if(CISP_DRV_RET_OK == ret)
    {
        if ( option->module_name.ltmmeas == ENABLED )
        {
            isp_ltmmeas_config = ltmmeas_enable_config_table;
        }
        else
        {
            isp_ltmmeas_config = ltmmeas_disable_config_table;
        }

        isp_ltmmeas_config.prc_thresholds.values[0]     = (uint32_t)round(frame_width* frame_height * 0.001);
        isp_ltmmeas_config.prc_thresholds.values[1]     = (uint32_t)round(frame_width* frame_height * 0.25);
        isp_ltmmeas_config.prc_thresholds.values[2]     = (uint32_t)round(frame_width* frame_height * 0.999);
#if (!ISP_SIL)
        if ( option->dma == ENABLED )
        {
            ret = R_CISP_LTMMEAS_AddToDmaConfigData(g_dma_config_data, &isp_ltmmeas_config);
        }
        else
        {
            ret = R_CISP_LTMMEAS_SetConfig(p_isp_ctl, &isp_ltmmeas_config);
        }
#else
        ret = R_CISP_LTMMEAS_SetConfig(p_isp_ctl, &isp_ltmmeas_config);
#endif
        if(CISP_DRV_RET_OK != ret)
        {
            printf("Failed: Set the configuration of the LIMMEAS module contained. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    else
    {
        printf("Failed: Get the configuration of the LIMMEAS module contained. Error Code %s(%d)\n",
               ToCispReturnValueString(ret), (int32_t)ret);
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_LTMMEAS
 *********************************************************************************************************************/


#if (!ISP_SIL)
/**********************************************************************************************************************/
/* Function Name : SetupISP_FILT */
/******************************************************************************************************************//**
 * @brief       Setup ISP FILT
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_FILT(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_FILT(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_filt_config_t       isp_filt_config;

    memset(&isp_filt_config, 0, sizeof(st_cisp_filt_config_t));

    ret = R_CISP_FILT_GetConfig (p_isp_ctl, &isp_filt_config);
    if(CISP_DRV_RET_OK == ret)
    {
        if ( option->module_name.filt == ENABLED )
        {           /* FILT enable*/
            isp_filt_config = filt_enable_config_table;
        }
        else
        {           /* FILT disable*/
            isp_filt_config = filt_disable_config_table;
        }

        if ( option->dma == ENABLED )
        {
            ret = R_CISP_FILT_AddToDmaConfigData(g_dma_config_data, &isp_filt_config);
        }
        else
        {
            ret = R_CISP_FILT_SetConfig (p_isp_ctl, &isp_filt_config);
        }

        if(CISP_DRV_RET_OK != ret)
        {
            printf("Failed: Set the configuration of the FILT module contained. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    else
    {
        printf("Failed: Get the configuration of the FILT module contained. Error Code %s(%d)\n",
               ToCispReturnValueString(ret), (int32_t)ret);
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_FILT
 *********************************************************************************************************************/

/**********************************************************************************************************************/
/* Function Name : SetupISP_HVOUTREGS */
/******************************************************************************************************************//**
 * @brief       Setup ISP HVOUTREGS
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_HVOUTREGS(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_HVOUTREGS(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_hvoutregs_config_t  isp_hvoutregs_config;

    memset(&isp_hvoutregs_config, 0, sizeof(st_cisp_hvoutregs_config_t));

    ret = R_CISP_HVOUTREGS_GetConfig (p_isp_ctl, &isp_hvoutregs_config);
    if(CISP_DRV_RET_OK == ret)
    {
        isp_hvoutregs_config = hvoutregs_config_table;

        if ( option->dma == ENABLED )
        {
            ret = R_CISP_HVOUTREGS_AddToDmaConfigData(g_dma_config_data, &isp_hvoutregs_config);
        }
        else
        {
            ret = R_CISP_HVOUTREGS_SetConfig (p_isp_ctl, &isp_hvoutregs_config);
        }

        if(CISP_DRV_RET_OK != ret)
        {
            printf("Failed: Set the configuration of the HVOUTREGS module contained. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    else
    {
        printf("Failed: Get the configuration of the HVOUTREGS module contained. Error Code %s(%d)\n",
               ToCispReturnValueString(ret), (int32_t)ret);
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_HVOUTREGS
 *********************************************************************************************************************/

/**********************************************************************************************************************/
/* Function Name : SetupISP_MVOUTREGS */
/******************************************************************************************************************//**
 * @brief       Setup ISP MVOUTREGS
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_MVOUTREGS(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_MVOUTREGS(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_mvoutregs_config_t  isp_mvoutregs_config;

    memset(&isp_mvoutregs_config, 0, sizeof(st_cisp_mvoutregs_config_t));

    ret = R_CISP_MVOUTREGS_GetConfig(p_isp_ctl, &isp_mvoutregs_config);
    if(CISP_DRV_RET_OK == ret)
    {
        if ( (option->hdr_fusion == ENABLED) || (option->module_name.dpcc_pre2 == ENABLED) )
        {           /* MVOUTREGS HDR fusion*/
            isp_mvoutregs_config = mvoutregs_hdr_config_table;
        }
        else
        {           /* MVOUTREGS config*/
            isp_mvoutregs_config = mvoutregs_config_table;
        }

        if ( option->dma == ENABLED )
        {
            ret = R_CISP_MVOUTREGS_AddToDmaConfigData(g_dma_config_data, &isp_mvoutregs_config);
        }
        else
        {
            ret = R_CISP_MVOUTREGS_SetConfig (p_isp_ctl, &isp_mvoutregs_config);
        }

        if(CISP_DRV_RET_OK != ret)
        {
            printf("Failed: Set the configuration of the MVOUTREGS module contained. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    else
    {
        printf("Failed: Get the configuration of the MVOUTREGS module contained. Error Code %s(%d)\n",
               ToCispReturnValueString(ret), (int32_t)ret);
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_MVOUTREGS
 *********************************************************************************************************************/

/**********************************************************************************************************************/
/* Function Name : SetupISP_HIST256 */
/******************************************************************************************************************//**
 * @brief       Setup ISP HIST256
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_HIST256(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_HIST256(st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width, uint16_t frame_height)
     */
    e_cisp_drv_return_t       ret;
    st_cisp_hist256_config_t  isp_hist256_config;

    memset(&isp_hist256_config, 0, sizeof(st_cisp_hist256_config_t));

    ret = R_CISP_HIST256_GetConfig(p_isp_ctl, &isp_hist256_config);
    if(ret == CISP_DRV_RET_OK)
    {
        if ( option->module_name.hist256 == ENABLED )
        {           /* HIST256 enable*/
            isp_hist256_config = hist256_enable_config_table;
        }
        else
        {           /* HIST256 disable*/
            isp_hist256_config = hist256_disable_config_table;
        }

        if ( option->dma == ENABLED )
        {
            ret = R_CISP_HIST256_AddToDmaConfigData(g_dma_config_data, &isp_hist256_config);
        }
        else
        {
            ret = R_CISP_HIST256_SetConfig(p_isp_ctl, &isp_hist256_config);
        }

        if(ret != CISP_DRV_RET_OK)
        {
            printf("Failed: Set the configuration of the HIST256 module. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    else
    {
        printf("Failed: Get the configuration of the HIST256 module. Error Code %s(%d)\n",
               ToCispReturnValueString(ret), (int32_t)ret);
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_HIST256
 *********************************************************************************************************************/

/**********************************************************************************************************************/
/* Function Name : SetupISP_BLS */
/******************************************************************************************************************//**
 * @brief       Setup ISP BLS
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_BLS(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_BLS(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_bls_config_t        isp_bls_config;
    uint8_t                     cnt;
    uint8_t                     pipeline = 0;

    memset(&isp_bls_config, 0, sizeof(st_cisp_bls_config_t));

    for(cnt = 0; cnt < 2; cnt++)
    {
        if(cnt == 0)
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_1;
        }
        else
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_2;
        }

        ret = R_CISP_BLS_GetConfig (p_isp_ctl, pipeline, &isp_bls_config);
        if(CISP_DRV_RET_OK == ret)
        {
            switch( pipeline )
            {
                case CISP_PIPELINE_PRE_FUSION_1:
                    if ( option->module_name.bls == ENABLED )
                    {           /* BLS pre1 enable*/
                        isp_bls_config = bls_pre1_enable_config_table;
                    }
                    else
                    {           /* BLS pre1/pre2 desable*/
                        isp_bls_config = bls_pre1_disable_config_table;
                    }
                break;

                case CISP_PIPELINE_PRE_FUSION_2:
                    isp_bls_config = bls_pre2_disable_config_table;
                break;

                default:
                break;
            }

            if ( option->dma == ENABLED )
            {
                ret = R_CISP_BLS_AddToDmaConfigData(g_dma_config_data, pipeline, &isp_bls_config);
            }
            else
            {
                ret = R_CISP_BLS_SetConfig (p_isp_ctl, pipeline, &isp_bls_config);
            }

            if(CISP_DRV_RET_OK != ret)
            {
                printf("Failed: Set the configuration of the BLS module contained. Error Code %s(%d)/Pipeline %d\n",
                       ToCispReturnValueString(ret), (int32_t)ret, pipeline);
            }
        }
        else
        {
            printf("Failed: Get the configuration of the BLS module contained. Error Code %s(%d)/Pipeline %d\n",
                   ToCispReturnValueString(ret), (int32_t)ret, pipeline);
        }
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_BLS
 *********************************************************************************************************************/

#endif /* !ISP_SIL */

/**********************************************************************************************************************/
/* Function Name : SetupISP_DPF */
/******************************************************************************************************************//**
 * @brief       Setup ISP DPF
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_DPF(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_DPF(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_dpf_config_t        isp_dpf_config;
    uint8_t                     cnt;
    uint8_t                     pipeline = 0;

    memset(&isp_dpf_config, 0, sizeof(st_cisp_dpf_config_t));

    for(cnt = 0; cnt < 2; cnt++)
    {
        if(cnt == 0)
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_1;
        }
        else
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_2;
        }

        ret = R_CISP_DPF_GetConfig (p_isp_ctl, pipeline, &isp_dpf_config);
        if(CISP_DRV_RET_OK == ret)
        {
            switch( pipeline )
            {
                case CISP_PIPELINE_PRE_FUSION_1:
                    if ( option->module_name.dpf == ENABLED )
                    {           /* DPF enable*/
                        isp_dpf_config = dpf_pre1_enable_config_table;
                    }
                    else
                    {           /* DPF disable*/
                        isp_dpf_config = dpf_pre1_disable_config_table;
                    }
                break;

                case CISP_PIPELINE_PRE_FUSION_2:
                    isp_dpf_config = dpf_pre2_disable_config_table;
                break;

                default:
                break;
            }
#if (!ISP_SIL)
            if ( option->dma == ENABLED )
            {
                ret = R_CISP_DPF_AddToDmaConfigData(g_dma_config_data, pipeline, &isp_dpf_config);
            }
            else
            {
                ret = R_CISP_DPF_SetConfig (p_isp_ctl, pipeline, &isp_dpf_config);
            }
#else
            ret = R_CISP_DPF_SetConfig (p_isp_ctl, pipeline, &isp_dpf_config);
#endif  /* !ISP_SIL */
            if(CISP_DRV_RET_OK != ret)
            {
                printf("Failed: Set the configuration of the DPF module contained. Error Code %s(%d)/Pipeline %d\n",
                       ToCispReturnValueString(ret), (int32_t)ret, pipeline);
            }
        }
        else
        {
            printf("Failed: Get the configuration of the DPF module contained. Error Code %s(%d)/Pipeline %d\n",
                   ToCispReturnValueString(ret), (int32_t)ret, pipeline);
        }
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_DPF
 *********************************************************************************************************************/

#if (!ISP_SIL)
/**********************************************************************************************************************/
/* Function Name : SetupISP_LSC */
/******************************************************************************************************************//**
 * @brief       Setup ISP LSC
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_LSC(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_LSC(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_lsc_config_t        isp_lsc_config;
    st_cisp_lsc_table_t         isp_table_config;
    uint8_t                     pipeline = 0;
    uint8_t                     cnt;
    uint16_t                    cnt2;
    uint16_t                    table_num;

    memset(&isp_lsc_config, 0, sizeof(st_cisp_lsc_config_t));
    memset(&isp_table_config, 0, sizeof(st_cisp_lsc_table_t));

    for(cnt = 0; cnt < 2; cnt++)
    {
        if(cnt == 0)
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_1;
        }
        else
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_2;
        }

        ret = R_CISP_LSC_GetConfig (p_isp_ctl, pipeline, &isp_lsc_config);
        if(CISP_DRV_RET_OK == ret)
        {
            switch( pipeline )
            {
            case CISP_PIPELINE_PRE_FUSION_1:
                if ( option->module_name.lsc == ENABLED )
                {           /* LSC pre1 enable*/
                    isp_lsc_config = lsc_pre1_enable_config_table;
                }
                else
                {           /* LSC pre1 disable*/
                    isp_lsc_config = lsc_pre1_disable_config_table;
                }

                for(cnt2 = 0; cnt2 < CISP_LSC_CFG_GRID_SIZE; cnt2++)
                {
                    isp_lsc_config.grid_config.x_grad[cnt2] = 0;
                    isp_lsc_config.grid_config.x_size[cnt2] = 16;
                    isp_lsc_config.grid_config.y_grad[cnt2] = 0;
                    isp_lsc_config.grid_config.y_size[cnt2] = 16;
                }

                for(table_num = 0; table_num < CISP_LSC_CFG_NUM_TABLES; table_num++)
                {
                    for(cnt2 = 0; cnt2 < CISP_LSC_CFG_TABLE_SIZE; cnt2++)
                    {
                        isp_lsc_config.ram_tables[table_num].blue.values[cnt2]       = 0x400;
                        isp_lsc_config.ram_tables[table_num].green_blue.values[cnt2] = 0x400;
                        isp_lsc_config.ram_tables[table_num].green_red.values[cnt2]  = 0x400;
                        isp_lsc_config.ram_tables[table_num].red.values[cnt2]        = 0x400;
                    }
                }
                memcpy(&isp_table_config, &isp_lsc_config.ram_tables[1], sizeof(st_cisp_lsc_table_t));
                ret = R_CISP_LSC_SetRamTable (p_isp_ctl, pipeline, CISP_LSC_ACTIVE_TABLE_0, &isp_table_config);
                break;

            case CISP_PIPELINE_PRE_FUSION_2:
                        /* LSC pre2 disable*/
                isp_lsc_config = lsc_pre2_disable_config_table;
                for(cnt2 = 0; cnt2 < CISP_LSC_CFG_GRID_SIZE; cnt2++)
                {
                    isp_lsc_config.grid_config.x_grad[cnt2] = 0;
                    isp_lsc_config.grid_config.x_size[cnt2] = 16;
                    isp_lsc_config.grid_config.y_grad[cnt2] = 0;
                    isp_lsc_config.grid_config.y_size[cnt2] = 16;
                }

                for(table_num = 0; table_num < CISP_LSC_CFG_NUM_TABLES; table_num++)
                {
                    for(cnt2 = 0; cnt2 < CISP_LSC_CFG_TABLE_SIZE; cnt2++)
                    {
                        isp_lsc_config.ram_tables[table_num].blue.values[cnt2]       = 0x400;
                        isp_lsc_config.ram_tables[table_num].green_blue.values[cnt2] = 0x400;
                        isp_lsc_config.ram_tables[table_num].green_red.values[cnt2]  = 0x400;
                        isp_lsc_config.ram_tables[table_num].red.values[cnt2]        = 0x400;
                    }
                }
                memcpy(&isp_table_config, &isp_lsc_config.ram_tables[0], sizeof(st_cisp_lsc_table_t));
                ret = R_CISP_LSC_SetRamTable (p_isp_ctl, pipeline, CISP_LSC_ACTIVE_TABLE_1, &isp_table_config);
                break;
            }
            if(CISP_DRV_RET_OK == ret)
            {
                if ( option->dma == ENABLED )
                {
                    ret = R_CISP_LSC_AddToDmaConfigData(g_dma_config_data, pipeline, &isp_lsc_config);
                }
                else
                {
                    ret = R_CISP_LSC_SetConfig (p_isp_ctl, pipeline, &isp_lsc_config);
                }

                if(CISP_DRV_RET_OK != ret)
                {
                    printf("Failed: Set the configuration of the LSC module contained. Error Code %s(%d)/Pipeline %d\n",
                           ToCispReturnValueString(ret), (int32_t)ret, pipeline);
                }
            }
            else
            {
                printf("Failed: Set the RAM table of the LSC module contained. Error Code %s(%d)/Pipeline %d\n",
                       ToCispReturnValueString(ret), (int32_t)ret, pipeline);
            }
        }
        else
        {
            printf("Failed: Get the configuration of the LSC module contained. Error Code %s(%d)/Pipeline %d\n",
                   ToCispReturnValueString(ret), (int32_t)ret, pipeline);
        }
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_LSC
 *********************************************************************************************************************/

/**********************************************************************************************************************/
/* Function Name : SetupISP_IS */
/******************************************************************************************************************//**
 * @brief       Setup ISP IS
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_IS(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_IS(st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width , uint16_t frame_height)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_is_config_t         isp_is_config;
    uint8_t                     cnt;
    uint8_t                     pipeline = 0;

    memset(&isp_is_config, 0, sizeof(st_cisp_is_config_t));

    for(cnt = 0; cnt < 2; cnt++)
    {
        if(cnt == 0)
        {
            pipeline = CISP_PIPELINE_HV_OUT;
        }
        else
        {
            pipeline = CISP_PIPELINE_MV_OUT;
        }

        ret = R_CISP_IS_GetConfig (p_isp_ctl, pipeline, &isp_is_config);
        if(CISP_DRV_RET_OK == ret)
        {
            switch( pipeline )
            {
                case CISP_PIPELINE_HV_OUT:
                    isp_is_config = is_hvout_config_table;
                    break;
                case CISP_PIPELINE_MV_OUT:
                    isp_is_config = is_mvout_config_table;
                    break;
                default:
                    break;
            }

            if ( option->dma == ENABLED )
            {
                ret = R_CISP_IS_AddToDmaConfigData(g_dma_config_data, pipeline, &isp_is_config);
            }
            else
            {
                ret = R_CISP_IS_SetConfig (p_isp_ctl, pipeline, &isp_is_config);
            }

            if(CISP_DRV_RET_OK != ret)
            {
                printf("Failed: Set the configuration of the IS module contained. Error Code %s(%d)/Pipeline %d\n",
                       ToCispReturnValueString(ret), (int32_t)ret, pipeline);
            }
        }
        else
        {
            printf("Failed: Get the configuration of the IS module contained. Error Code %s(%d)/Pipeline %d\n",
                   ToCispReturnValueString(ret), (int32_t)ret, pipeline);
        }
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_IS
 *********************************************************************************************************************/

/**********************************************************************************************************************/
/* Function Name : SetupISP_CCOR */
/******************************************************************************************************************//**
 * @brief       Setup ISP CCOR
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_CCOR(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_CCOR(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_ccor_config_t       isp_ccor_config;
    st_cisp_ccor_hw_type_t      isp_hw_type;
    uint8_t                     cnt;
    uint8_t                     pipeline = 0;

    memset(&isp_ccor_config, 0, sizeof(st_cisp_ccor_config_t));
    memset(&isp_hw_type, 0, sizeof(st_cisp_ccor_hw_type_t));

    for(cnt = 0; cnt < 3; cnt++)
    {
        if(cnt == 0)
        {
            pipeline = CISP_PIPELINE_HV_OUT;
        }
        else if(cnt == 1)
        {
            pipeline = CISP_PIPELINE_POST_FUSION;
        }
        else
        {
            pipeline = CISP_PIPELINE_MV_OUT;
        }

        ret = R_CISP_CCOR_GetHardwareType (p_isp_ctl, pipeline, &isp_hw_type);
        if(CISP_DRV_RET_OK == ret)
        {
            ret = R_CISP_CCOR_GetConfig (p_isp_ctl, pipeline, &isp_ccor_config);
            if(CISP_DRV_RET_OK == ret)
            {
                switch( pipeline )
                {
                case CISP_PIPELINE_HV_OUT:
                    DEBUG_PRINT("CCOR Harware Type\n");
                    DEBUG_PRINT("  CISP_PIPELINE_HV_OUT\n");
                    DEBUG_PRINT("    ccor_range_available = %d\n", isp_hw_type.ccor_range_available);  // RPP_OUT_CCOR_CONFIG_TYPE
                    DEBUG_PRINT("    ccor_range_available = %d\n", isp_hw_type.use_pre_offsets);       // RPP_OUT_CCOR_CONFIG_TYPE

                    isp_ccor_config = ccor_hvout_config_table;          /* CCOR hvout config*/
                    break;

                case CISP_PIPELINE_POST_FUSION:
                    DEBUG_PRINT("CCOR Harware Type\n");
                    DEBUG_PRINT("  CISP_PIPELINE_POST_FUSION\n");
                    DEBUG_PRINT("    ccor_range_available = %d\n", isp_hw_type.ccor_range_available); // RPP_MAIN_POST_CCOR_CONFIG_TYPE
                    DEBUG_PRINT("    ccor_range_available = %d\n", isp_hw_type.use_pre_offsets);      // RPP_MAIN_POST_CCOR_CONFIG_TYPE

                    isp_ccor_config = ccor_post_config_table;           /* CCOR post config*/
                    break;

                case CISP_PIPELINE_MV_OUT:
                    DEBUG_PRINT("CCOR Harware Type\n");
                    DEBUG_PRINT("  CISP_PIPELINE_MV_OUT\n");
                    DEBUG_PRINT("    ccor_range_available = %d\n", isp_hw_type.ccor_range_available); // RPP_MV_OUT_CCOR_CONFIG_TYPE
                    DEBUG_PRINT("    ccor_range_available = %d\n", isp_hw_type.use_pre_offsets);      // RPP_MV_OUT_CCOR_CONFIG_TYPE

                    isp_ccor_config = ccor_mvout_config_table;          /* CCOR mvout config*/
                    break;
                }

                if ( option->dma == ENABLED )
                {
                    ret = R_CISP_CCOR_AddToDmaConfigData(g_dma_config_data, pipeline, &isp_ccor_config);
                }
                else
                {
                    ret = R_CISP_CCOR_SetConfig (p_isp_ctl, pipeline, &isp_ccor_config);
                }

                if(CISP_DRV_RET_OK != ret)
                {
                    printf("Failed: Set the configuration of the CCOR module contained. Error Code %s(%d)/Pipeline %d\n",
                           ToCispReturnValueString(ret), (int32_t)ret, pipeline);
                }
            }
            else
            {
                printf("Failed: Get the configuration of the CCOR module contained. Error Code %s(%d)/Pipeline %d\n",
                       ToCispReturnValueString(ret), (int32_t)ret, pipeline);
            }
        }
        else
        {
            printf("Failed: Get the Hardware Type of the CCOR module contained. Error Code %s(%d)/Pipeline %d\n",
                   ToCispReturnValueString(ret), (int32_t)ret, pipeline);
        }

    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_CCOR
 *********************************************************************************************************************/
#endif /* !ISP_SIL */


/**********************************************************************************************************************/
/* Function Name : SetupISP_RGBDENOISE */
/******************************************************************************************************************//**
 * @brief       Setup ISP RGBDENOISE
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_RGBDENOISE(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_RGBDENOISE(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_rgbdenoise_config_t isp_rgbdenoise_config;
    uint8_t                     cnt;

    memset(&isp_rgbdenoise_config, 0, sizeof(st_cisp_rgbdenoise_config_t));

    ret = R_CISP_RGBDENOISE_GetConfig (p_isp_ctl, &isp_rgbdenoise_config);
    if(CISP_DRV_RET_OK == ret)
    {
        if ( option->module_name.rgbdenoise == ENABLED )
        {
            isp_rgbdenoise_config = rgbdenoise_enagle_config_table;
        }
        else
        {
            isp_rgbdenoise_config = rgbdenoise_disable_config_table;
        }
        for(cnt= 1; cnt < CISP_RGBDENOISE_CFG_IF_COEFF_NUM; cnt++)
        {
            isp_rgbdenoise_config.params.if_cc[cnt] = 0; // RPP_MAIN_POST_RGBDENOISE_SPNR_CHROMA_IF_COEF_08_15 - 24_31
            isp_rgbdenoise_config.params.if_cy[cnt] = 0; // RPP_MAIN_POST_RGBDENOISE_SPNR_LUMA_IF_COEF_08_015 - 24_31
        }
#if (!ISP_SIL)
        if ( option->dma == ENABLED )
        {
            ret = R_CISP_RGBDENOISE_AddToDmaConfigData(g_dma_config_data, &isp_rgbdenoise_config);
        }
        else
        {
            ret = R_CISP_RGBDENOISE_SetConfig (p_isp_ctl, &isp_rgbdenoise_config);
        }
#else
        ret = R_CISP_RGBDENOISE_SetConfig (p_isp_ctl, &isp_rgbdenoise_config);
#endif
        if(CISP_DRV_RET_OK != ret)
        {
            printf("Failed: Set the configuration of the RGBDENOISE module contained. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    else
    {
        printf("Failed: Get the configuration of the RGBDENOISE module contained. Error Code %s(%d)\n",
               ToCispReturnValueString(ret), (int32_t)ret);
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_RGBDENOISE
 *********************************************************************************************************************/

#if (!ISP_SIL)
/**********************************************************************************************************************/
/* Function Name : SetupISP_DPCC */
/******************************************************************************************************************//**
 * @brief       Setup ISP DPCC
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_DPCC(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_DPCC(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_dpcc_config_t isp_dpcc_config;
    uint8_t                     pipeline;

    memset(&isp_dpcc_config, 0, sizeof(st_cisp_dpcc_config_t));

    for(uint8_t cnt = 0; cnt < 2; cnt++)
    {
        if(cnt == 0)
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_1;
        }
        else
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_2;
        }

        ret = R_CISP_DPCC_GetConfig (p_isp_ctl, pipeline, &isp_dpcc_config);
        if(CISP_DRV_RET_OK == ret)
        {
            switch( pipeline )
            {
                case CISP_PIPELINE_PRE_FUSION_1:
                    if ( option->module_name.dpcc_pre1 == ENABLED )
                    {       /* DPCC Pre1 enable */
                        isp_dpcc_config = dpcc_pre1_enable_config_table;
                    }
                    else
                    {       /* DPCC Pre1 disable */
                        isp_dpcc_config = dpcc_pre1_disable_config_table;
                    }
                break;
                case CISP_PIPELINE_PRE_FUSION_2:
                    if ( option->module_name.dpcc_pre2 == ENABLED )
                    {       /* DPCC Pre2 enable */
                        isp_dpcc_config = dpcc_pre2_enable_config_table;
                    }
                    else
                    {       /* DPCC Pre2 disable */
                        isp_dpcc_config = dpcc_pre2_disable_config_table;
                    }
                break;
                default:
                break;
            }

            if ( option->dma == ENABLED )
            {
                ret = R_CISP_DPCC_AddToDmaConfigData(g_dma_config_data, pipeline, &isp_dpcc_config);
            }
            else
            {
                ret = R_CISP_DPCC_SetConfig (p_isp_ctl, pipeline, &isp_dpcc_config);
            }

            if(CISP_DRV_RET_OK != ret)
            {
                printf("Failed: Set the configuration of the DPCC module contained. Error Code %s(%d)/Pipeline %d\n",
                        ToCispReturnValueString(ret), (int32_t)ret, pipeline);
            }
        }
        else
        {
            printf("Failed: Get the configuration of the DPCC module contained. Error Code %s(%d)/Pipeline %d\n",
                   ToCispReturnValueString(ret), (int32_t)ret, pipeline);
        }
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_DPCC
 *********************************************************************************************************************/

/**********************************************************************************************************************/
/* Function Name : SetupISP_CAC */
/******************************************************************************************************************//**
 * @brief       Setup ISP CAC
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/

static e_cisp_drv_return_t SetupISP_CAC(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_CAC(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_cac_config_t isp_cac_config;

    memset(&isp_cac_config, 0, sizeof(st_cisp_cac_config_t));

    ret = R_CISP_CAC_GetConfig (p_isp_ctl, &isp_cac_config);
    if(CISP_DRV_RET_OK == ret)
    {   // for HEX file
        if ( option->module_name.cac == ENABLED )
        {
            isp_cac_config = cac_enable_config_table;
        }
        else
        {
            isp_cac_config = cac_disable_config_table;
        }

        if ( option->dma == ENABLED )
        {
            ret = R_CISP_CAC_AddToDmaConfigData(g_dma_config_data, &isp_cac_config);
        }
        else
        {
            ret = R_CISP_CAC_SetConfig (p_isp_ctl, &isp_cac_config);
        }

        if(CISP_DRV_RET_OK != ret)
        {
            printf("Failed: Set the configuration of the CAC module contained. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    else
    {
        printf("Failed: Get the configuration of the CAC module contained. Error Code %s(%d)\n",
               ToCispReturnValueString(ret), (int32_t)ret);
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_CAC
 *********************************************************************************************************************/
#endif /* !ISP_SIL */

/**********************************************************************************************************************/
/* Function Name : SetupISP_SHRPCNR */
/******************************************************************************************************************//**
 * @brief       Setup ISP SHRPCNR
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_SHRPCNR(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_SHRPCNR(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t         ret;
    st_cisp_shrpcnr_config_t isp_shrpcnr_config;

    memset(&isp_shrpcnr_config, 0, sizeof(st_cisp_shrpcnr_config_t));

    ret = R_CISP_SHRPCNR_GetConfig (p_isp_ctl, &isp_shrpcnr_config);
    if(CISP_DRV_RET_OK == ret)
    {  // for Python
        if ( option->module_name.shrpcnr == ENABLED )
        {           /* SHRPCNR enable*/
            isp_shrpcnr_config = shrpcnr_enable_config_table;
        }
        else
        {           /* SHRPCNR disable*/
            isp_shrpcnr_config = shrpcnr_disable_config_table;
        }
#if (!ISP_SIL)
        if ( option->dma == ENABLED )
        {
            ret = R_CISP_SHRPCNR_AddToDmaConfigData(g_dma_config_data, &isp_shrpcnr_config);
        }
        else
        {
            ret = R_CISP_SHRPCNR_SetConfig (p_isp_ctl, &isp_shrpcnr_config);
        }
#else
        ret = R_CISP_SHRPCNR_SetConfig (p_isp_ctl, &isp_shrpcnr_config);
#endif  /* !ISP_SIL */
        if(CISP_DRV_RET_OK != ret)
        {
            printf("Failed: Set the configuration of the SHRPCNR module contained. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    else
    {
        printf("Failed: Get the configuration of the SHRPCNR module contained. Error Code %s(%d)\n",
               ToCispReturnValueString(ret), (int32_t)ret);
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_SHRPCNR
 *********************************************************************************************************************/

#if (!ISP_SIL)
/**********************************************************************************************************************/
/* Function Name : SetupISP_HIST */
/******************************************************************************************************************//**
 * @brief       Setup ISP HIST
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/
static e_cisp_drv_return_t SetupISP_HIST(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_HIST(st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width, uint16_t frame_height)
     */
    e_cisp_drv_return_t    ret;
    st_cisp_hist_config_t  isp_hist_config;
    uint8_t                cnt;
    uint8_t                pipeline = 0;

    memset(&isp_hist_config, 0, sizeof(st_cisp_hist_config_t));

    for(cnt = 0; cnt < 3; cnt++)
    {
        if(cnt == 0)
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_1;
        }
        else if(cnt == 1)
        {
            pipeline = CISP_PIPELINE_PRE_FUSION_2;
        }
        else
        {
            pipeline = CISP_PIPELINE_POST_FUSION;
        }
        ret = R_CISP_HIST_GetConfig(p_isp_ctl, pipeline, &isp_hist_config);
        if(ret == CISP_DRV_RET_OK)
        {
            switch( pipeline )
            {
                case CISP_PIPELINE_PRE_FUSION_1:
                    if ( (option->module_name.hist == ENABLED) || (option->hdr_fusion == ENABLED) )
                    {           /* HIST enable*/
                        isp_hist_config = hist_pre1_enable_config_table;
                    }
                    else
                    {           /* HIST disable*/
                        isp_hist_config = hist_pre1_disable_config_table;
                    }
                break;

                case CISP_PIPELINE_PRE_FUSION_2:
                    if ( option->hdr_fusion == ENABLED )
                    {
                        isp_hist_config = hist_pre2_enable_config_table;
                    }
                    else
                    {
                        isp_hist_config = hist_pre2_disable_config_table;
                    }
                break;

                case CISP_PIPELINE_POST_FUSION:
                    if ( (option->module_name.hist == ENABLED) || (option->hdr_fusion == ENABLED) )
                    {
                        isp_hist_config = hist_post_enable_config_table;
                    }
                    else
                    {
                        isp_hist_config = hist_post_disable_config_table;
                    }
                break;
                
                default:
                break;
            }

            if ( option->dma == ENABLED )
            {
                ret = R_CISP_HIST_AddToDmaConfigData(g_dma_config_data, pipeline, &isp_hist_config);
            }
            else
            {
                ret = R_CISP_HIST_SetConfig(p_isp_ctl, pipeline, &isp_hist_config);
            }

           if(CISP_DRV_RET_OK != ret)
            {
                printf("Failed: Set the configuration of the HIST module contained. Error Code %s(%d)/Pipeline %d\n",
                       ToCispReturnValueString(ret), (int32_t)ret, pipeline);
            }
        }
        else
        {
            printf("Failed: Get the configuration of the HIST module contained. Error Code %s(%d)/Pipeline %d\n",
                   ToCispReturnValueString(ret), (int32_t)ret, pipeline);
        }
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_HIST
 *********************************************************************************************************************/

/**********************************************************************************************************************/
/* Function Name : SetupISP_RMAPMEAS */
/******************************************************************************************************************//**
 * @brief       Setup ISP RMAPMEAS
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/

static e_cisp_drv_return_t SetupISP_RMAPMEAS(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_RMAPMEAS(st_cisp_drv_ctl_t *p_isp_ctl, uint16_t frame_width , uint16_t frame_height)
     */
    e_cisp_drv_return_t       ret;
    st_cisp_rmapmeas_config_t isp_rmapmeas_config;

    memset(&isp_rmapmeas_config, 0, sizeof(st_cisp_rmapmeas_config_t));

    ret = R_CISP_RMAPMEAS_GetConfig (p_isp_ctl, &isp_rmapmeas_config);
    if(CISP_DRV_RET_OK == ret)
    {  // for HEX file
        if( option->module_name.rmapmeas == ENABLED)
        {
            /* RMAPMEAS enable*/
            isp_rmapmeas_config = rmapmeas_enable_config_table;
        }
        else
        {
            /* RMAPMEAS disable*/
            isp_rmapmeas_config = rmapmeas_disable_config_table;
        }

        if ( option->dma == ENABLED )
        {
            ret = R_CISP_RMAPMEAS_AddToDmaConfigData(g_dma_config_data, &isp_rmapmeas_config);
        }
        else
        {
            ret = R_CISP_RMAPMEAS_SetConfig (p_isp_ctl, &isp_rmapmeas_config);
        }

        if(CISP_DRV_RET_OK != ret)
        {
            printf("Failed: Set the configuration of the RMAPMEAS module contained. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    else
    {
        printf("Failed: Get the configuration of the RMAPMEAS module contained. Error Code %s(%d)\n",
               ToCispReturnValueString(ret), (int32_t)ret);
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_RMAPMEAS
 *********************************************************************************************************************/

/**********************************************************************************************************************/
/* Function Name : SetupISP_XYZ2LUV */
/******************************************************************************************************************//**
 * @brief       Setup ISP XYZ2LUV
 * @param[in]   st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option
 * @param[out]  none
 * @retval      e_cisp_drv_return_t ret
 * @retval      none
 *********************************************************************************************************************/

static e_cisp_drv_return_t SetupISP_XYZ2LUV(st_cisp_drv_ctl_t *p_isp_ctl, st_isp_options_t *option)
{
    /*!
     *   @brief uint16_t SetupISP_XYZ2LUV(st_cisp_drv_ctl_t *p_isp_ctl)
     */
    e_cisp_drv_return_t      ret;
    st_cisp_xyz2luv_config_t isp_xyz2luv_config;

    memset(&isp_xyz2luv_config, 0, sizeof(st_cisp_xyz2luv_config_t));

    ret = R_CISP_XYZ2LUV_GetConfig (p_isp_ctl, &isp_xyz2luv_config);
    if(CISP_DRV_RET_OK == ret)
    {  // for Python
        isp_xyz2luv_config = xyz2luv_config_table;

        if ( option->dma == ENABLED )
        {
            ret = R_CISP_XYZ2LUV_AddToDmaConfigData(g_dma_config_data, &isp_xyz2luv_config);
        }
        else
        {
            ret = R_CISP_XYZ2LUV_SetConfig (p_isp_ctl, &isp_xyz2luv_config);
        }

        if(CISP_DRV_RET_OK != ret)
        {
            printf("Failed: Set the configuration of the XYZ2LUV module contained. Error Code %s(%d)\n",
                   ToCispReturnValueString(ret), (int32_t)ret);
        }
    }
    else
    {
        printf("Failed: Get the configuration of the XYZ2LUV module contained. Error Code %s(%d)\n",
               ToCispReturnValueString(ret), (int32_t)ret);
    }

    return ret;
}
/**********************************************************************************************************************
 End of function SetupISP_XYZ2LUV
 *********************************************************************************************************************/
#endif /* !SIL */
