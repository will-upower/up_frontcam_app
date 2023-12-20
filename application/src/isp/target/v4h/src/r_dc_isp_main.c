/***********************************************************************************************************************
* Copyright [2022] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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
* File Name    : r_dc_isp_main_.c
* Version      : 0.1.0
* Product Name : ISP Test Application
* Device(s)    : R-Car V4H
* Description  : Play-around with VSPX/TISP/CISP on V4H
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : Version DD.MM.YYYY Description
*         : 0.1.0   17.01.2022 Initial version
***********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

/* drivers include */
#include "rcar-xos/vspx/r_vspx.h"
#include "rcar-xos/tisp/r_tisp.h"
#include "rcar-xos/cisp/r_dc_cisp_rpp.h"
#include "rcar-xos/cisp/r_dc_cisp_rpp_version.h"

#include "rcar-xos/osal/r_osal_api.h"
#include "rcar-xos/osal/r_osal_types.h"

#include "r_dc_isp_utils.h"
#include "r_dc_isp_callbacks.h"
#include "r_dc_isp_setup.h"
#include "r_dc_isp_memories.h"
#include "r_vspx_setup.h"
#include "r_dc_isp_main.h"
#include "common.h"

vspx_memories_t g_vsxp_memories;
isp_out_memories_t g_isp_output_memories;
r_stest_mem_mgr_hdl_t g_stest_mem_hdl;

#if USE_DMA_CONFIG
extern st_cisp_dma_config_data_t  *g_dma_config_data;
#endif

st_vspx_drv_ctl_t vspx_ctl;
st_cisp_drv_ctl_t cisp_ctl;

#define VSPX_CHANNEL (0)

#define PRINT_DRIVER_VERSIONS (1)

#if defined(PRINT_DRIVER_VERSIONS)
#include "rcar-xos/vspx/r_vspx_version.h"
#include "rcar-xos/tisp/r_tisp_version.h"
#include "rcar-xos/cisp/r_dc_cisp_rpp_version.h"
#endif

int channel = 0;
extern void Mem_Flush_CISP_CNF();

double  g_hblank_cycles;
bool    g_dma_config;
extern osal_mq_handle_t   mq_handle;
st_isp_options_t option;

static int Init_Vspx(st_isp_options_t *option);
static int Init_Tisp(st_isp_options_t *option);
static int Init_Cisp(st_isp_options_t *option);

/**********************************************************************************************************************
/* Function Name : Deinitialize_Drivers */
/******************************************************************************************************************//**
 * @brief       Deinitialization of driver
                [Covers: BPAP_FC_V4H_AD016][Covers: BPAP_FC_V4H_AD023][Covers: BPAP_FC_V4H_AD036]
 * @param[in]   channel
 * @param[out]  none
 * @retval      true            success(0)
 * @retval      false           fail(1)
 *********************************************************************************************************************/
int Deinitialize_Drivers(uint8_t channel)
{
    DEBUG_PRINT(" Tearing down...\n");

    e_cisp_drv_return_t cisp_ret = R_CISP_Stop(&cisp_ctl);
    CHECK_PRINT_CISP_ERR("CISP Stop", cisp_ret);
    cisp_ret = R_CISP_Quit(&cisp_ctl);
    CHECK_PRINT_CISP_ERR("CISP Quit", cisp_ret);

    uint16_t tisp_ret = R_TISP_Stop(channel);
    CHECK_PRINT_TISP_ERR("TISP Stop", tisp_ret);
    tisp_ret = R_TISP_Close(channel);
    CHECK_PRINT_TISP_ERR("TISP Close", tisp_ret);
    tisp_ret = R_TISP_DeInit(channel);
    CHECK_PRINT_TISP_ERR("TISP DeInit", tisp_ret);

    e_vspx_drv_return_t vspx_ret = R_VSPX_Stop(&vspx_ctl);
    CHECK_PRINT_VSPX_ERR("VSPX Stop", vspx_ret);
    vspx_ret = R_VSPX_Quit(&vspx_ctl);
    CHECK_PRINT_VSPX_ERR("VSPX Quit", vspx_ret);
    
    return SUCCESS;
}
/**********************************************************************************************************************
 End of function Deinitialize_Drivers
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : Setup_StreamingBridge */
/******************************************************************************************************************//**
 * @brief       Seting up StreamingBridge
                [Covers: BPAP_FC_V4H_AD016][Covers: BPAP_FC_V4H_AD036]
 * @param[in]   st_isp_options_t *option
 * @param[out]  none
 * @retval      true            success(0)
 * @retval      false           fail(1)
 *********************************************************************************************************************/
int Setup_StreamingBridge(st_isp_options_t *option)
{
    /* Setup streaming bridge (TISP) */
    uint16_t tisp_ret           = TISP_RET_E_OK;

    st_tisp_streaming_bridge_cfg_t  bridge_cfg;
    memset(&bridge_cfg, 0, sizeof(bridge_cfg));
    tisp_ret = R_TISP_GetStreamingBridgeConfig(option->channel, &bridge_cfg);
    CHECK_PRINT_TISP_ERR("TISP_GetStreamingBridgeConfig", tisp_ret);

    if (tisp_ret == TISP_RET_E_OK)
    {
        if ( option->dma == ENABLED )
        {
            Setup_StreamingDmaConfig(&bridge_cfg, ENABLED);
        }
        else
        {
            Setup_StreamingDmaConfig(&bridge_cfg, DISABLED);
        }
        tisp_ret = R_TISP_SetStreamingBridgeConfig(option->channel, &bridge_cfg);
        CHECK_PRINT_TISP_ERR("TISP_SetStreamingBridgeConfig", tisp_ret);
        if (tisp_ret == TISP_RET_E_OK)
        {
            DEBUG_PRINT(" Streaming bridge setup done\n");
        }
    }
    return SUCCESS;
}
/**********************************************************************************************************************
 End of function Setup_StreamingBridge
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : Process_Frame */
/******************************************************************************************************************//**
 * @brief       Processig  frame
                [Covers: BPAP_FC_V4H_AD016][Covers: BPAP_FC_V4H_AD036]
 * @param[in]   st_isp_options_t *option
 * @param[out]  none
 * @retval      true            success(0)
 * @retval      false           fail(1)
 *********************************************************************************************************************/
int Process_Frame(st_isp_options_t *option)
{
    /* Setup streaming bridge */
    Setup_StreamingBridge(option);

    /* Start ISP */
    Start_ISP(&cisp_ctl, &g_isp_output_memories, option);

    /* Trigger processing */
    VSPX_Setup(&vspx_ctl, &g_vsxp_memories, option);


    /* Handle callbacks */
    Handle_ISPCallbacks(&cisp_ctl, &g_isp_output_memories, option);

    /* Stop VSPX */
    e_vspx_drv_return_t vspx_ret = R_VSPX_StopExecution(&vspx_ctl, false);
    CHECK_PRINT_VSPX_ERR("R_VSPX_StopExecution", vspx_ret);

    /* Stop CISP */
    e_cisp_drv_return_t cisp_ret = R_CISP_StopExecution(&cisp_ctl);
    CHECK_PRINT_CISP_ERR("CISP StopExecution", cisp_ret);

    DEBUG_PRINT("*** Stopped VSPX and CISP ***\n");


    e_osal_return_t osal_ret = R_OSAL_MqReset(mq_handle);
    if(osal_ret != OSAL_RETURN_OK)
    {
        printf ("Warning!! R_OSAL_MqDelete: return val:%d \n", osal_ret);
        return FAILED;
    }
    else
    {
        DEBUG_PRINT ("SUCCESS!! R_OSAL_MqDelete: return val:%d \n", osal_ret);
    }

    Save_Memories(&g_isp_output_memories);

    return SUCCESS;
}
/**********************************************************************************************************************
 End of function Process_Frame
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : R_ISP_Initialize */
/******************************************************************************************************************//**
 * @brief       ISP initialization
                [Covers: BPAP_FC_V4H_AD016][Covers: BPAP_FC_V4H_AD036][Covers: BPAP_FC_V4H_AD048]
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success(0)
 * @retval      false           fail(1)
 *********************************************************************************************************************/
int R_ISP_Initialize() 
{
    e_isp_sample_return_t ret_status = R_ISP_OK;
    uint8_t osal_ret;
    memset(&option, 0, sizeof(st_isp_options_t));
    option.dma = ENABLED;
    option.dl  = DISABLED;
    g_hblank_cycles = 0.8;
    g_dma_config = ENABLED;

    st_module_config_t default_module_setting =
    {                                      /* Default Setiings     */
        .gammain     = DISABLED,   /* GAMMAIN is disabled                */
        .gammaout    = ENABLED,    /* GAMMAOUT is enabled                */
        .awb         = ENABLED,    /* AWB is enabled                     */
        .awbmeas     = DISABLED,   /* AWBMEAS is disabled                */
        .exm         = DISABLED,   /* EXM is disabled                    */
        .ltm         = DISABLED,   /* LTM is disabled                    */
        .ltmmeas     = DISABLED,   /* LTMMEAS is disabled                */
        .filt        = ENABLED,    /* FILT is enabled                    */
        .hist256     = DISABLED,   /* HIST256 is disabled                */
        .bls         = DISABLED,   /* BLS is disabled                    */
        .dpf         = DISABLED,   /* DPF is disabled                    */
        .lsc         = DISABLED,   /* LSC is disabled                    */
        .rgbdenoise  = DISABLED,   /* RGBDENOISE is disabled             */
        .dpcc_pre1   = DISABLED,   /* DPCC Pre1 is disabled              */
        .dpcc_pre2   = DISABLED,   /* DPCC pre2 is disabled              */
        .cac         = DISABLED,   /* CAC is disabled                    */
        .shrpcnr     = ENABLED,    /* SHRPCNR is enabled                 */
        .hist        = ENABLED,    /* HIST is enabled                    */
        .rmapmeas    = DISABLED,   /* RMAPMEAS is disabled               */
        .hist_dump   = DISABLED,   /* To dump HIST to memory is disabled */
    };
    option.module_name = default_module_setting;
    
    option.channel = g_customize.ISP_Channel;
  
    memset(&cisp_ctl, 0x00u, sizeof(st_cisp_drv_ctl_t));
    memset(&vspx_ctl, 0x00u, sizeof(st_vspx_drv_ctl_t));

    DEBUG_PRINT("R_ISP_Initialize\n\n");

    Init_Memories(&g_vsxp_memories, &g_isp_output_memories, &option);

    osal_ret = Init_Vspx(&option);
    if(FAILED == osal_ret)
    {
        return FAILED;
    }

    osal_ret = Init_Tisp(&option);
    if(FAILED == osal_ret)
    {
        return FAILED;
    }

    osal_ret = Init_Cisp(&option);
    if(FAILED == osal_ret)
    {
        return FAILED;
    }

    /*   should wait 18.750ms after CPG reset */
    R_OSAL_ThreadSleepForTimePeriod((osal_milli_sec_t)20);

    /* Setup ISP Core pipeline */
    /* NOTE: depending on USE_DMA_CONFIG (example.h) this either programms registers or collects in g_dma_config_data */ 

    if ( ret_status == R_ISP_OK )
    {
        Set_Memories(option.dma);

        /* NOTE: with USE_DMA_CONFIG programs VSPX to read from dma_config_data */
        uint16_t tisp_ret = R_TISP_Start(option.channel);
        CHECK_PRINT_TISP_ERR("TISP_Start", tisp_ret);
 
        Setup_Callbacks(option.channel, &cisp_ctl, &vspx_ctl, &option);
        DEBUG_PRINT("Single frame start\n");
        ret_status = Setup_ISP(&cisp_ctl, INPUT_IMAGE_WIDTH, INPUT_IMAGE_HEIGHT, &option);
    }    
    
    return SUCCESS;
}
/**********************************************************************************************************************
 End of function R_ISP_Initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : Init_Cisp */
/******************************************************************************************************************//**
 * @brief       CISp initialization
 * @param[in]   st_isp_options_t *option
 * @param[out]  none
 * @retval      true            success(0)
 * @retval      false           fail(1)
 *********************************************************************************************************************/
int Init_Cisp(st_isp_options_t *option)
{
    // Check & try to start CISP ...
    DEBUG_PRINT(" Initializing CISP...\n");
#if defined(PRINT_DRIVER_VERSIONS)
    const st_cisp_version_t *cisp_ver = R_CISP_GetVersion();
    //PRINT_VER("CISP", cisp_ver);
#endif
    st_cisp_drv_usr_param_t cisp_cfg = { 
        .mutex_id = OSAL_MUTEX_CISP_00 + option->channel,
        .int_priority = OSAL_INTERRUPT_PRIORITY_TYPE1 /* NOTE: was 10 */
    };
    e_cisp_drv_return_t cisp_ret = R_CISP_Init(&cisp_ctl, option->channel, &cisp_cfg);
    CHECK_PRINT_CISP_ERR("CISP Init", cisp_ret);
    if (cisp_ret == CISP_DRV_RET_OK)
    {
        // Start the CISP HW
        cisp_ret = R_CISP_Start(&cisp_ctl);
        CHECK_PRINT_CISP_ERR("CISP Start", cisp_ret);

        if (cisp_ret != CISP_DRV_RET_OK)
        {
            printf(" CISP START failed\n");
            return FAILED;
        }
    }
    else
    {
      printf("CISP init FAILED");
      return FAILED;
    }

    return SUCCESS;
}
/**********************************************************************************************************************
 End of function Init_Cisp
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : Init_Tisp */
/**********************************************************************************************************************
 * @brief       TISP initialization
 * @param[in]   st_isp_options_t *option
 * @param[out]  none
 * @retval      true            success(0)
 * @retval      false           fail(1)
 *********************************************************************************************************************/
int Init_Tisp(st_isp_options_t *option)
{
    /*Check & try to start TISP*/
    DEBUG_PRINT(" Initializing TISP...\n");
#if defined(PRINT_DRIVER_VERSIONS)
    const st_tisp_version_t *tisp_ver = R_TISP_GetVersion();
    //PRINT_VER("TISP", tisp_ver);
#endif
    st_tisp_drv_usr_param_t tisp_cfg = {
        .mutex_id = OSAL_MUTEX_TISP_00 + option->channel,
        .mutex_timeout_ms = 0,
        .int_priority = OSAL_INTERRUPT_PRIORITY_TYPE1 /* NOTE: was 10 */
    };
    st_tisp_info_t                  tisp_info;
    st_tisp_fault_t                 fault_config;
    e_tisp_input_t                  top_input;

    memset(&tisp_info, 0, sizeof(st_tisp_info_t));
    memset(&fault_config, 0, sizeof(st_tisp_fault_t));

    uint16_t tisp_ret = R_TISP_Init(option->channel, &tisp_cfg);
    CHECK_PRINT_TISP_ERR("TISP Init", tisp_ret);

    if (tisp_ret == TISP_RET_E_OK)
    {
        //TISP Open
        tisp_ret = R_TISP_Open(option->channel);
        CHECK_PRINT_TISP_ERR("TISP Open", tisp_ret);
        if (tisp_ret == TISP_RET_E_OK)
        {
            tisp_ret = R_TISP_GetInfo (option->channel, &tisp_info);
            CHECK_PRINT_TISP_ERR("TISP GetInfo", tisp_ret);
            if (tisp_ret == TISP_RET_E_OK)
            {
                DEBUG_PRINT("  TISP Information\n");
                DEBUG_PRINT("   hw_version   = %x\n", tisp_info.hw_version);
                DEBUG_PRINT("   num_of_units = %d\n", tisp_info.num_of_units);
            }

            tisp_ret = R_TISP_GetInputMode (option->channel, &top_input);
            CHECK_PRINT_TISP_ERR("TISP GetInputMode", tisp_ret);
            if (tisp_ret == TISP_RET_E_OK)
            {
                DEBUG_PRINT("  TISP input mode = %d\n", top_input);
            }

            top_input = ISP_TOP_INPUT_CSI;
            tisp_ret = R_TISP_SetInputMode(option->channel, &top_input);
            CHECK_PRINT_TISP_ERR("TISP SetInputMode", tisp_ret);

            tisp_ret = R_TISP_GetFaultInt (option->channel, &fault_config);
            CHECK_PRINT_TISP_ERR("TISP GetFaultInt", tisp_ret);
            if (tisp_ret == TISP_RET_E_OK)
            {
                DEBUG_PRINT ("  TISP fault notification configuration value: %d\n",fault_config.top_fault[0]);
            }
            else
            {
                printf ("  TISP fault notification configuration value Failed\n");
                return FAILED;
            }
        }
    }
    else
    {
        printf("TISP open FAILED");
        return FAILED;

    }
    return SUCCESS;

}
/**********************************************************************************************************************
 End of function Init_Tisp
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : Init_Vspx */
/******************************************************************************************************************//**
 * @brief       VSPX initialization
 * @param[in]   st_isp_options_t *option
 * @param[out]  none
 * @retval      true            success(0)
 * @retval      false           fail(1)
 *********************************************************************************************************************/
int Init_Vspx(st_isp_options_t *option) 
{
    /*Check & try to start VSPX ...*/
    DEBUG_PRINT(" Initializing VSPX...\n");
#if defined(PRINT_DRIVER_VERSIONS)
    const st_vspx_version_t *vspx_ver = R_VSPX_GetVersion();
    //PRINT_VER("VSPX", vspx_ver);
#endif
    st_vspx_drv_usr_param_t vspx_usr_param = { 
        .mutex_id = OSAL_MUTEX_VSPX_00 + option->channel,
        .int_priority = OSAL_INTERRUPT_PRIORITY_TYPE1, /* NOTE: was 10 */
        .mutex_timeout_ms = 100
    };
    e_vspx_drv_return_t vspx_ret = R_VSPX_Init(&vspx_ctl, option->channel, &vspx_usr_param);
    CHECK_PRINT_VSPX_ERR("VSPX Init", vspx_ret);

    if (vspx_ret == VSPX_DRV_RET_OK)
    {
        vspx_ret = R_VSPX_Start(&vspx_ctl);
        CHECK_PRINT_VSPX_ERR("VSPX Start", vspx_ret);
        if (vspx_ret == VSPX_DRV_RET_OK)
        {
            DEBUG_PRINT(" VSPX Started\n");     
        }
        else
        {
            return FAILED;
        }

    }
    return SUCCESS;
}
/**********************************************************************************************************************
 End of function Init_Vspx
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : R_ISP_Execute */
/******************************************************************************************************************//**
 * @brief       ISP Execution
                [Covers: BPAP_FC_V4H_AD016][Covers: BPAP_FC_V4H_AD036][Covers: BPAP_FC_V4H_AD048]
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success(0)
 * @retval      false           fail(1)
 *********************************************************************************************************************/
int R_ISP_Execute()
{
    uint8_t osal_ret;
    Buffer_Copy(&g_vsxp_memories, &option);
    DEBUG_PRINT("INSIDE R_ISP_Execute\n");
    osal_ret = Process_Frame(&option);
    if(SUCCESS == osal_ret)
    {
        DEBUG_PRINT(" ISP EXECUTED\n");
    }
    else
    {
        return FAILED;
    }

    return SUCCESS;
}
/**********************************************************************************************************************
 End of function R_ISP_Execute
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : R_ISP_DeInitialize */
/******************************************************************************************************************//**
 * @brief       ISP deinitialization
                [Covers: BPAP_FC_V4H_AD016][Covers: BPAP_FC_V4H_AD036][Covers: BPAP_FC_V4H_AD048]
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success(0)
 * @retval      false           fail(1)
 *********************************************************************************************************************/
int R_ISP_DeInitialize() 
{
    uint8_t osal_ret;
    DeInit_Memories();
    osal_ret = Deinitialize_Drivers(option.channel);
    if(SUCCESS == osal_ret)
    {
        DEBUG_PRINT(" Deinitialized Drivers\n");
    }
    else
    {
        return FAILED;
    }
    return SUCCESS;
}
/**********************************************************************************************************************
 End of function R_ISP_DeInitialize
 *********************************************************************************************************************/
