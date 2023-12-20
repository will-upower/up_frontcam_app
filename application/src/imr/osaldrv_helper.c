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
 * File Name    : osaldrv_helper.c
 * Version      : 0.1.0
 * Description  : OSAL Modules initialisation and execution.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 06.03.2023 0.1.0     Alpha Release (Base: R-CAR SDK V2.17 sample)
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>

#include "rcar-xos/imr/r_imrdrv_api.h"
#include "rcar-xos/imrdlg/r_imrdlg.h"
#include "rcar-xos/osal/r_osal.h"
#include "rcar-xos/osal/r_osal_memory_impl.h"
#include "rcar-xos/imr/r_imrdrv_type.h"
#include "osaldrv_helper.h"
#include "imr_main.h"
#include "common.h"
#include "customize.h"


/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define OSAL_MUTEX_WAIT (1000U)  /* msec */
#define TIMEOUT         (30000U) /* msec */
#define MQ_NUM          (16)
#define IMR_RESOURCE_ID 0x1000
/**********************************************************************************************************************
 Global variable declarations 
 *********************************************************************************************************************/
/* OSAL */
osal_mq_handle_t    g_mq_handle_imr_0;
osal_mq_handle_t    g_mq_handle_imr_1;
osal_mq_handle_t    g_mq_handle_imr_2;
osal_mq_handle_t    g_mq_handle_imr_3;
osal_mq_handle_t    g_mq_handle_imr_4;
osal_mq_id_t        g_mq_imr_id;

/* Memory Manager */
osal_memory_manager_handle_t g_handle_osalmmngr;

st_osal_mmngr_config_t       g_osal_mmngr_config;
osal_axi_bus_id_t s_imp_dev_axi_bus_id = OSAL_AXI_BUS_ID_INVALID;

/**********************************************************************************************************************
 Structure declaration
 *********************************************************************************************************************/
typedef struct
{
    e_imrdrv_channelno_t channel_no;
    osal_mq_handle_t     osal_mq;
} st_cb_arg_t;

typedef struct
{
    uint32_t details_code;
} st_imrdrv_mq_data_t;

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static int32_t imrdrv_callback_func (const e_imrdrv_errorcode_t ret_code, const uint32_t details_code, 
                                                                                void * const p_callback_args);
/* IMR Driver */
static uintptr_t             ctrldata[MAX_MODULE][IMRDRV_SIZE_WORKAREA / sizeof(uintptr_t)];
static st_imrdrv_initdata_t  initdata[MAX_MODULE_NUM];
static imrdrv_ctrl_handle_t  ctrhandle[MAX_MODULE_NUM];
static st_imrdrv_os_config_t os_config[MAX_MODULE_NUM];
static osal_axi_bus_id_t     axi_id[MAX_MODULE_NUM];
static st_cb_arg_t           g_cb_arg[MAX_MODULE_NUM];
static bool                  callback_flag;


static int init_imrdrv_ch0(void);
static int init_imrdrv_ch1(void);
static int init_imrdrv_ch2(void);
static int init_imrdrv_ch3(void);
static int init_imrdrv_ch4(void);

/******************************************************************************************************************//**
/* Function Name : get_imrdrv_ctrlhandles */
/******************************************************************************************************************//**
 * @brief       IMR driver control handle for the channel number passed
 * @param[in]   channel     channel number      
 * @param[out]  none
 * @retval      imrdrv_ctrl_handle_t   imr driver control handle for the passed channel number
 *********************************************************************************************************************/
imrdrv_ctrl_handle_t get_imrdrv_ctrlhandles(int channel)
{
    uint32_t channel_idx = convert_channel_to_index(channel);               /* channel to index conversion */
    return ctrhandle[channel_idx];
}

/**********************************************************************************************************************
/* Function Name : init_osal */
/**********************************************************************************************************************
 * @brief       Inits OSAL layer and creates synchronization objects
                [Covers: BPAP_FC_V4H_AD034]
 * @param[in]   none
 * @param[out]  none
 * @retval      0                    success
 * @retval      1                    fail
 *********************************************************************************************************************/
int init_osal()
{
    e_osal_return_t     ret_osal;
    st_osal_mq_config_t mq_config;

    mq_config.max_num_msg = MQ_NUM;
    mq_config.msg_size    = sizeof(st_imrdrv_mq_data_t);

    if (g_customize.IMR_Ch_0_Enable)
    {
        ret_osal = R_OSAL_MqCreate(&mq_config, IMR_RESOURCE_ID+6, &g_mq_handle_imr_0);
        if (ret_osal != OSAL_RETURN_OK)
        {
            printf("Failed R_OSAL_MqCreate ret=%d\n", ret_osal);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_1_Enable)
    {
        ret_osal = R_OSAL_MqCreate(&mq_config, IMR_RESOURCE_ID+7, &g_mq_handle_imr_1);
        if (ret_osal != OSAL_RETURN_OK)
        {
            printf("Failed R_OSAL_MqCreate ret=%d\n", ret_osal);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_2_Enable)
    {
        ret_osal = R_OSAL_MqCreate(&mq_config, IMR_RESOURCE_ID+8, &g_mq_handle_imr_2);
        if (ret_osal != OSAL_RETURN_OK)
        {
            printf("Failed R_OSAL_MqCreate ret=%d\n", ret_osal);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_3_Enable)
    {
        ret_osal = R_OSAL_MqCreate(&mq_config, IMR_RESOURCE_ID+9, &g_mq_handle_imr_3);
        if (ret_osal != OSAL_RETURN_OK)
        {
            printf("Failed R_OSAL_MqCreate ret=%d\n", ret_osal);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_4_Enable)
    {
        ret_osal = R_OSAL_MqCreate(&mq_config, IMR_RESOURCE_ID+10, &g_mq_handle_imr_4);
        if (ret_osal != OSAL_RETURN_OK)
        {
            printf("Failed R_OSAL_MqCreate ret=%d\n", ret_osal);
            return FAILED;
        }
    }

    return SUCCESS;
}
/**********************************************************************************************************************
/End of function init_osal
/**********************************************************************************************************************

/**********************************************************************************************************************
/* Function Name : init_mmgr */
/**********************************************************************************************************************
 * @brief       Inits memory manager
 * @param[in]   none
 * @param[out]  none
 * @retval      0                      success
 * @retval      1                      fail
 *********************************************************************************************************************/
int init_mmgr(void)
{
    e_osal_return_t ret_osal;
    ret_osal = R_OSAL_MmngrGetOsalMaxConfig(&g_osal_mmngr_config);          /* Get OSAL memory manager configuration */

    if (ret_osal == OSAL_RETURN_OK) 
    {
        g_osal_mmngr_config.mode                        = OSAL_MMNGR_ALLOC_MODE_FREE_LIST;
        //osal_mmngr_cfg.memory_limit                 = 100000000; //0x6000000; /* 96MB */
        g_osal_mmngr_config.max_allowed_allocations     = 256u;
        g_osal_mmngr_config.max_registered_monitors_cbs = 0u;
        ret_osal                                      = R_OSAL_MmngrOpen(&g_osal_mmngr_config, &g_handle_osalmmngr);
        if (ret_osal != OSAL_RETURN_OK)
        {
            printf("Failed R_OSAL_MmngrOpen ret=%d\n", ret_osal);
            return FAILED;
        }
        else
        {
            s_imp_dev_axi_bus_id = OSAL_AXI_BUS_ID_IPA_MAIN_MEMORY;
            ret_osal             = R_OSAL_IoGetAxiBusIdFromDeviceName(
                            "imp_top_00", &s_imp_dev_axi_bus_id);
            if (OSAL_RETURN_OK != ret_osal) {
                /* cast to int32_t */
                printf("<< [ERROR] R_OSAL_IoGetAxiBusIdFromDeviceName : code "
                       "%d>>\n", 
                       (int32_t)ret_osal);
            }
        }
    }
    else
    {
        printf("Failed R_OSAL_MmngrGetOsalMaxConfig ret=%d\n", ret_osal);
        return FAILED;
    }

    return SUCCESS;
}
/**********************************************************************************************************************
/End of function init_mmgr
/**********************************************************************************************************************

/**********************************************************************************************************************
/* Function Name :mmgr_helper_alloc_buffer */
/**********************************************************************************************************************
 * @brief       Allocates IMR data
 * @param[in]   buf                     Pointer to memory manager buffer handle
 * @param[in]   imr_channel             Index of IMR channel (req. since physical address uses AXI ID)
 * @param[in]   phys_addr               Specifies physical address
 * @param[in]   vmr_addr                pointer to virtual memory adress
 * @param[in]   data_size               Size of the data 
 * @param[in]   align
 * @param[out]  none
 * @retval      0                       success
 * @retval      1                       fail
 *********************************************************************************************************************/
int mmgr_helper_alloc_buffer(osal_memory_buffer_handle_t * buf, int imr_channel, uint32_t data_size, uint32_t align, 
                               addr_t * vmr_addr, uintptr_t * phys_addr)
{
    e_osal_return_t ret_osal;
    ret_osal = R_OSAL_MmngrAlloc(g_handle_osalmmngr, data_size, align, buf);     /* OSAL memory manager allocation */

    if (ret_osal == OSAL_RETURN_OK)
    {
        ret_osal = R_OSAL_MmngrGetCpuPtr(*buf, (void *)vmr_addr);           /* OSAL memory manager get cpu pointer */

        if (ret_osal != OSAL_RETURN_OK)
        {
            printf("Failed R_OSAL_MmngrGetCpuPtr ret=%d\n", ret_osal);
            return FAILED;
        }
    }
    else
    {
        printf("Failed R_OSAL_MmngrAlloc ret=%d\n", ret_osal);
        return FAILED;
    }

    ret_osal = R_OSAL_MmngrGetHwAddr(*buf, axi_id[imr_channel], phys_addr);/* OSAL memory manager get hardware address */

    if (ret_osal != OSAL_RETURN_OK)
    {
        printf("Failed R_OSAL_MmngrGetHwAddr ret=%d\n", ret_osal);
        return FAILED;
    }

    return SUCCESS;
}
/**********************************************************************************************************************
/End of function mmgr_helper_alloc_buffer
/**********************************************************************************************************************

/**********************************************************************************************************************
/* Function Name : allocate_dl_memory */
/**********************************************************************************************************************
 * @brief       dl memory allocation
                [Covers: BPAP_FC_V4H_AD011]
 * @param[in]   mmngr_buf_handle        Pointer to memory manager buffer handle
 * @param[in]   data_size               Size of the data
 * @param[in]   align                   
 * @param[in]   p_vmr_addr              pointer to vmr_addr
 * @param[out]  none
 * @retval      0                       success
 * @retval      1                       fail
 *********************************************************************************************************************/
int allocate_dl_memory(osal_memory_buffer_handle_t * mmngr_buf_handle, size_t data_size, size_t align, void * p_vmr_addr)
{
    e_osal_return_t ret_osal;
	
	/* OSAL memory manager allocation */
    ret_osal = R_OSAL_MmngrAlloc(g_handle_osalmmngr, data_size, align, mmngr_buf_handle); 

    if (ret_osal == OSAL_RETURN_OK)
    {
        ret_osal = R_OSAL_MmngrGetCpuPtr(*mmngr_buf_handle, (void *)p_vmr_addr);        /* Get CPU pointer */

        if (ret_osal != OSAL_RETURN_OK)
        {
            printf("Failed R_OSAL_MmngrGetCpuPtr ret=%d\n", ret_osal);
            return FAILED;
        }
    }
    else
    {
        printf("Failed R_OSAL_MmngrAlloc ret=%d\n", ret_osal);
        return FAILED;
    }

    return SUCCESS;
}
/**********************************************************************************************************************
/End of function allocate_dl_memory
/**********************************************************************************************************************

/**********************************************************************************************************************
/* Function Name : mmgr_helper_flush */
/**********************************************************************************************************************
 * @brief       OSAL MmngrFlush
 * @param[in]   buf                     MMGR buffer handle
 * @param[out]  none
 * @retval      0                       success
 * @retval      1                       fail
 *********************************************************************************************************************/
int mmgr_helper_flush(osal_memory_buffer_handle_t buf)
{
    e_osal_return_t ret_osal;
    size_t          size;

    ret_osal = R_OSAL_MmngrGetSize(buf, &size);                             /* Get buffer size */    

    if (ret_osal != OSAL_RETURN_OK)
    {
        printf("Failed R_OSAL_MmngrGetSize ret=%d\n", ret_osal);
        return FAILED;
    }

    ret_osal = R_OSAL_MmngrFlush(buf, 0, size);                             /* Flush buffer */

    if (ret_osal != OSAL_RETURN_OK)
    {
        printf("Failed R_OSAL_MmngrFlush ret=%d\n", ret_osal);
        return FAILED;
    }
    return SUCCESS;
}
/**********************************************************************************************************************
/End of function mmgr_helper_flush
/**********************************************************************************************************************

/**********************************************************************************************************************
/* Function Name : mmgr_helper_dealloc */
/**********************************************************************************************************************
 * @brief       Deallocate memory
                [Covers: BPAP_FC_V4H_AD011][Covers: BPAP_FC_V4H_AD017]
 * @param[in]   buf                  MMGR buffer handle
 * @param[out]  none
 * @retval      0                    success
 * @retval      1                    fail
 *********************************************************************************************************************/
int mmgr_helper_dealloc(osal_memory_buffer_handle_t buf)
{
    e_osal_return_t ret_osal;
    ret_osal = R_OSAL_MmngrDealloc(g_handle_osalmmngr, buf);                 /* Deallocation of OSAL memory manager */

    if (ret_osal != OSAL_RETURN_OK)
    {
        printf("Failed R_OSAL_MmngrDealloc ret=%d\n", ret_osal);
        return FAILED;
    }

    return SUCCESS;
}
/**********************************************************************************************************************
/End of function mmgr_helper_dealloc
/**********************************************************************************************************************

/**********************************************************************************************************************
/* Function Name : init_imrdrv */
/**********************************************************************************************************************
 * @brief       Inits IMR driver stuff
 * @param[in]   none
 * @param[out]  none
 * @retval      0                       success
 * @retval      1                       fail
 *********************************************************************************************************************/
int init_imrdrv()
{
    
    int ret;

    if (g_customize.IMR_Ch_0_Enable)
    {    
        ret = init_imrdrv_ch0();
        if (SUCCESS != ret)
        {
            printf("Failed IMR driver initialization for channel 0 ret=%d\n", ret);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_1_Enable)
    {    
        ret = init_imrdrv_ch1();
        if (SUCCESS != ret)
        {
            printf("Failed IMR driver initialization for channel 1 ret=%d\n", ret);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_2_Enable)
    {    
        ret = init_imrdrv_ch2();
        if (SUCCESS != ret)
        {
            printf("Failed IMR driver initialization for channel 2 ret=%d\n", ret);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_3_Enable)
    {    
        ret = init_imrdrv_ch3();
        if (SUCCESS != ret)
        {
            printf("Failed IMR driver initialization for channel 3 ret=%d\n", ret);
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_4_Enable)
    {    
        ret = init_imrdrv_ch4();
        if (SUCCESS != ret)
        {
            printf("Failed IMR driver initialization for channel 4 ret=%d\n", ret);
            return FAILED;
        }
    }

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function init_imrdrv
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : init_imrdrv_ch0 */
/******************************************************************************************************************//**
 * @brief       Inits IMR driver stuff channel 0
                [Covers: BPAP_DMS_AD010][Covers: BPAP_DMS_AD032]
 * @param[in]   none      
 * @param[out]  none
 * @retval      0                       success
 * @retval      1                       fail
 *********************************************************************************************************************/
static int init_imrdrv_ch0(void)
{	
    e_osal_return_t      ret_osal;
    e_imrdrv_errorcode_t ret_imr;
    uint32_t             ch_idx;

    /* === Initialize device drivers. for ch 0 === */
    ch_idx                              = convert_channel_to_index(IMR_CH_0);
    initdata[ch_idx].p_work_addr        = ctrldata[ch_idx];
    initdata[ch_idx].work_size          = IMRDRV_SIZE_WORKAREA;
    initdata[ch_idx].channel_no         = channel_no[ch_idx];
    os_config[ch_idx].mutex_id          = mutex_id[ch_idx];
    os_config[ch_idx].mutex_wait_period = OSAL_MUTEX_WAIT;
    os_config[ch_idx].dev_irq_priority  = OSAL_INTERRUPT_PRIORITY_TYPE1;
    g_cb_arg[ch_idx].channel_no         = channel_no[ch_idx];
    g_cb_arg[ch_idx].osal_mq            = g_mq_handle_imr_0;

    ret_imr = R_IMRDRV_Init(&initdata[ch_idx], &os_config[ch_idx], imrdrv_callback_func, (void *)&g_cb_arg[ch_idx], 
	                         &ctrhandle[ch_idx]);

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        printf("Failed R_IMRDRV_Init no=%d, ret=%d\n", channel_no[ch_idx], ret_imr);
        return FAILED;
    }

    printf("Succeed R_IMRDRV_Init no=%d\n", channel_no[ch_idx]);

    
    ch_idx   = convert_channel_to_index(IMR_CH_0);                    /*  Get axi id  (WARNING: V3U specific !) */
   
    ret_osal = R_OSAL_IoGetAxiBusIdFromDeviceName(device_name[ch_idx], &axi_id[IMR_CH_0]);

    if (ret_osal != OSAL_RETURN_OK)
    {
        printf("Failed R_OSAL_IoGetAxiBusIdFromDeviceName ret=%d\n", ret_osal);
    }
    
    ch_idx  = convert_channel_to_index(IMR_CH_0);           /* Setup IMR device driver for channel 0 and 4 */
    ret_imr = R_IMRDRV_Start(ctrhandle[ch_idx]);                                      /* IMR start execute */

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        printf("Failed R_IMRDRV_Start no=%d, ret=%d\n", channel_no[ch_idx], ret_imr);
        return FAILED;
    }

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function init_imrdrv_ch0
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : init_imrdrv_ch1 */
/******************************************************************************************************************//**
 * @brief       Inits IMR driver stuff channel 1
 * @param[in]   none      
 * @param[out]  none
 * @retval      0                       success
 * @retval      1                       fail
 *********************************************************************************************************************/
static int init_imrdrv_ch1(void)
{
    e_osal_return_t      ret_osal;
    e_imrdrv_errorcode_t ret_imr;
    uint32_t             ch_idx;

    ch_idx                              = convert_channel_to_index(IMR_CH_1);
    initdata[ch_idx].p_work_addr        = ctrldata[ch_idx];
    initdata[ch_idx].work_size          = IMRDRV_SIZE_WORKAREA;
    initdata[ch_idx].channel_no         = channel_no[ch_idx];
    os_config[ch_idx].mutex_id          = mutex_id[ch_idx];
    os_config[ch_idx].mutex_wait_period = OSAL_MUTEX_WAIT;
    os_config[ch_idx].dev_irq_priority  = OSAL_INTERRUPT_PRIORITY_TYPE1;
    g_cb_arg[ch_idx].channel_no         = channel_no[ch_idx];
    g_cb_arg[ch_idx].osal_mq            = g_mq_handle_imr_1;

    ret_imr = R_IMRDRV_Init(&initdata[ch_idx], &os_config[ch_idx], imrdrv_callback_func, (void *)&g_cb_arg[ch_idx], 
	                         &ctrhandle[ch_idx]);

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        printf("Failed R_IMRDRV_Init no=%d, ret=%d\n", channel_no[ch_idx], ret_imr);
        return FAILED;
    }

    ch_idx   = convert_channel_to_index(IMR_CH_1);                    /*  Get axi id  (WARNING: V3U specific !) */
   
    ret_osal = R_OSAL_IoGetAxiBusIdFromDeviceName(device_name[ch_idx], &axi_id[IMR_CH_1]);

    if (ret_osal != OSAL_RETURN_OK)
    {
        printf("Failed R_OSAL_IoGetAxiBusIdFromDeviceName ret=%d\n", ret_osal);
    }
    
    ch_idx  = convert_channel_to_index(IMR_CH_1);                    /* Setup IMR device driver for channel 0 and 4 */
    ret_imr = R_IMRDRV_Start(ctrhandle[ch_idx]);                                      /* IMR start execute */

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        printf("Failed R_IMRDRV_Start no=%d, ret=%d\n", channel_no[ch_idx], ret_imr);
        return FAILED;
    }

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function init_imrdrv_ch1
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : init_imrdrv_ch2 */
/******************************************************************************************************************//**
 * @brief       Inits IMR driver stuff channel 2
 * @param[in]   none      
 * @param[out]  none
 * @retval      0                       success
 * @retval      1                       fail
 *********************************************************************************************************************/
static int init_imrdrv_ch2(void)
{
    e_osal_return_t      ret_osal;
    e_imrdrv_errorcode_t ret_imr;
    uint32_t             ch_idx;

    ch_idx                              = convert_channel_to_index(IMR_CH_2);
    initdata[ch_idx].p_work_addr        = ctrldata[ch_idx];
    initdata[ch_idx].work_size          = IMRDRV_SIZE_WORKAREA;
    initdata[ch_idx].channel_no         = channel_no[ch_idx];
    os_config[ch_idx].mutex_id          = mutex_id[ch_idx];
    os_config[ch_idx].mutex_wait_period = OSAL_MUTEX_WAIT;
    os_config[ch_idx].dev_irq_priority  = OSAL_INTERRUPT_PRIORITY_TYPE1;
    g_cb_arg[ch_idx].channel_no         = channel_no[ch_idx];
    g_cb_arg[ch_idx].osal_mq            = g_mq_handle_imr_2;

    ret_imr = R_IMRDRV_Init(&initdata[ch_idx], &os_config[ch_idx], imrdrv_callback_func, (void *)&g_cb_arg[ch_idx], 
	                          &ctrhandle[ch_idx]);

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        printf("Failed R_IMRDRV_Init no=%d, ret=%d\n", channel_no[ch_idx], ret_imr);
        return FAILED;
    }

    ch_idx   = convert_channel_to_index(IMR_CH_2);                    /*  Get axi id  (WARNING: V3U specific !) */
   
    ret_osal = R_OSAL_IoGetAxiBusIdFromDeviceName(device_name[ch_idx], &axi_id[IMR_CH_2]);

    if (ret_osal != OSAL_RETURN_OK)
    {
        printf("Failed R_OSAL_IoGetAxiBusIdFromDeviceName ret=%d\n", ret_osal);
    }
    
    ch_idx  = convert_channel_to_index(IMR_CH_2);                    /* Setup IMR device driver for channel 0 and 4 */
    ret_imr = R_IMRDRV_Start(ctrhandle[ch_idx]);                                      /* IMR start execute */

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        printf("Failed R_IMRDRV_Start no=%d, ret=%d\n", channel_no[ch_idx], ret_imr);
        return FAILED;
    }

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function init_imrdrv_ch2
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : init_imrdrv_ch3 */
/******************************************************************************************************************//**
 * @brief       Inits IMR driver stuff channel 3
                [Covers: BPAP_DMS_AD010][Covers: BPAP_DMS_AD032]
 * @param[in]   none      
 * @param[out]  none
 * @retval      0                       success
 * @retval      1                       fail
 *********************************************************************************************************************/
static int init_imrdrv_ch3(void)
{
    e_osal_return_t      ret_osal;
    e_imrdrv_errorcode_t ret_imr;
    uint32_t             ch_idx;

    ch_idx                              = convert_channel_to_index(IMR_CH_3);
    initdata[ch_idx].p_work_addr        = ctrldata[ch_idx];
    initdata[ch_idx].work_size          = IMRDRV_SIZE_WORKAREA;
    initdata[ch_idx].channel_no         = channel_no[ch_idx];
    os_config[ch_idx].mutex_id          = mutex_id[ch_idx];
    os_config[ch_idx].mutex_wait_period = OSAL_MUTEX_WAIT;
    os_config[ch_idx].dev_irq_priority  = OSAL_INTERRUPT_PRIORITY_TYPE1;
    g_cb_arg[ch_idx].channel_no         = channel_no[ch_idx];
    g_cb_arg[ch_idx].osal_mq            = g_mq_handle_imr_3;

    ret_imr = R_IMRDRV_Init(&initdata[ch_idx], &os_config[ch_idx], imrdrv_callback_func, (void *)&g_cb_arg[ch_idx], 
	                         &ctrhandle[ch_idx]);

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        printf("Failed R_IMRDRV_Init no=%d, ret=%d\n", channel_no[ch_idx], ret_imr);
        return FAILED;
    }

    ch_idx   = convert_channel_to_index(IMR_CH_3);                    /*  Get axi id  (WARNING: V3U specific !) */
   
    ret_osal = R_OSAL_IoGetAxiBusIdFromDeviceName(device_name[ch_idx], &axi_id[IMR_CH_3]);

    if (ret_osal != OSAL_RETURN_OK)
    {
        printf("Failed R_OSAL_IoGetAxiBusIdFromDeviceName ret=%d\n", ret_osal);
    }
    
    ch_idx  = convert_channel_to_index(IMR_CH_3);                    /* Setup IMR device driver for channel 0 and 4 */
    ret_imr = R_IMRDRV_Start(ctrhandle[ch_idx]);                       /* IMR start execute */

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        printf("Failed R_IMRDRV_Start no=%d, ret=%d\n", channel_no[ch_idx], ret_imr);
        return FAILED;
    }

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function init_imrdrv_ch3
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : init_imrdrv_ch4 */
/******************************************************************************************************************//**
 * @brief       Inits IMR driver stuff channel 4
                [Covers: BPAP_DMS_AD010][Covers: BPAP_DMS_AD032]
 * @param[in]   none      
 * @param[out]  none
 * @retval      0                       success
 * @retval      1                       fail
 *********************************************************************************************************************/
static int init_imrdrv_ch4(void)
{
    e_osal_return_t      ret_osal;
    e_imrdrv_errorcode_t ret_imr;
    uint32_t             ch_idx;

    ch_idx                              = convert_channel_to_index(IMR_CH_4);
    initdata[ch_idx].p_work_addr        = ctrldata[ch_idx];
    initdata[ch_idx].work_size          = IMRDRV_SIZE_WORKAREA;
    initdata[ch_idx].channel_no         = channel_no[ch_idx];
    os_config[ch_idx].mutex_id          = mutex_id[ch_idx];
    os_config[ch_idx].mutex_wait_period = OSAL_MUTEX_WAIT;
    os_config[ch_idx].dev_irq_priority  = OSAL_INTERRUPT_PRIORITY_TYPE1;
    g_cb_arg[ch_idx].channel_no         = channel_no[ch_idx];
    g_cb_arg[ch_idx].osal_mq            = g_mq_handle_imr_4;

    ret_imr = R_IMRDRV_Init(&initdata[ch_idx], &os_config[ch_idx], imrdrv_callback_func, (void *)&g_cb_arg[ch_idx], 
	                         &ctrhandle[ch_idx]);

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        printf("Failed R_IMRDRV_Init no=%d, ret=%d\n", channel_no[ch_idx], ret_imr);
        return FAILED;
    }

    ch_idx   = convert_channel_to_index(IMR_CH_4);                    /*  Get axi id  (WARNING: V3U specific !) */
   
    ret_osal = R_OSAL_IoGetAxiBusIdFromDeviceName(device_name[ch_idx], &axi_id[IMR_CH_4]);

    if (ret_osal != OSAL_RETURN_OK)
    {
        printf("Failed R_OSAL_IoGetAxiBusIdFromDeviceName ret=%d\n", ret_osal);
    }
    
    ch_idx  = convert_channel_to_index(IMR_CH_4);                    /* Setup IMR device driver for channel 0 and 4 */
    ret_imr = R_IMRDRV_Start(ctrhandle[ch_idx]);                                      /* IMR start execute */

    if (ret_imr != IMRDRV_ERROR_OK)
    {
        printf("Failed R_IMRDRV_Start no=%d, ret=%d\n", channel_no[ch_idx], ret_imr);
        return FAILED;
    }

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function init_imrdrv_ch4
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : deinit_imrdrv_ind */
/**********************************************************************************************************************
 * @brief       Tears down one IMR driver
                [Covers: BPAP_SV_V4H_AD033][Covers: BPAP_SV_V4H_AD043]
 * @param[in]   uint8_t              channel number
 * @param[out]  none
 * @retval      0                    success
 * @retval      1                    fail
 *********************************************************************************************************************/
static int deinit_imrdrv_ind(uint8_t channel)
{
    uint32_t ch_idx;
    e_imrdrv_errorcode_t ret_imr = 0;

    ch_idx  = convert_channel_to_index(channel);

    ret_imr = R_IMRDRV_Stop(ctrhandle[ch_idx]);                        /* Close IMR device driver */
    if (ret_imr != IMRDRV_ERROR_OK)
    {
        PRINT_ERROR("Failed R_IMRDRV_Stop no=%d, ret=%d\n", channel_no[ch_idx], ret_imr);
        return FAILED;
    }

    DEBUG_PRINT("Succeed R_IMRDRV_Stop no=%d\n", channel_no[ch_idx]);

    ret_imr = R_IMRDRV_Quit(ctrhandle[ch_idx]);                       /* Deinitialize */
    if (ret_imr != IMRDRV_ERROR_OK)
    {
        PRINT_ERROR("Failed R_IMRDRV_Quit no=%d, ret=%d\n", channel_no[ch_idx], ret_imr);
    }
    else
    {
        DEBUG_PRINT("Succeed R_IMRDRV_Quit no=%d\n", channel_no[ch_idx]);
    }
    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function deinit_imrdrv_ind
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : deinit_imrdrv */
/**********************************************************************************************************************
 * @brief       Tears down IMR driver
 * @param[in]   none
 * @param[out]  none
 * @retval      0                    success
 * @retval      1                    fail
 *********************************************************************************************************************/
int deinit_imrdrv()
{
    uint32_t ch_idx;
    e_imrdrv_errorcode_t ret_imr;

    ret_imr = 0;

    if (g_customize.IMR_Ch_0_Enable)
    {     
        if (deinit_imrdrv_ind(IMR_CH_0) != SUCCESS)
        {
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_1_Enable)
    { 
        if (deinit_imrdrv_ind(IMR_CH_1) != SUCCESS)
        {
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_2_Enable)
    { 

        if (deinit_imrdrv_ind(IMR_CH_2) != SUCCESS)
        {
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_3_Enable)
    { 

        if (deinit_imrdrv_ind(IMR_CH_3) != SUCCESS)
        {
            return FAILED;
        }
    }

    if (g_customize.IMR_Ch_4_Enable)
    { 

        if (deinit_imrdrv_ind(IMR_CH_4) != SUCCESS)
        {
            return FAILED;
        }
    }

    return SUCCESS;
}
/**********************************************************************************************************************
/End of function deinit_imrdrv
/**********************************************************************************************************************

/**********************************************************************************************************************
/* Function Name : deinit_mmgr */
/**********************************************************************************************************************
 * @brief       Tears down MMGR
 * @param[in]   none
 * @param[out]  none
 * @retval      ret_osal
 *********************************************************************************************************************/
int deinit_mmgr()
{
    e_osal_return_t ret_osal;
    ret_osal = R_OSAL_MmngrClose(g_handle_osalmmngr);                  /* OSAL Memory manager close */

    if (ret_osal != OSAL_RETURN_OK)
    {
        printf("Failed R_OSAL_MmngrClose ret=%d\n", ret_osal);
        return FAILED;
    }

    return SUCCESS;
}
/**********************************************************************************************************************
/End of function deinit_mmgr
/**********************************************************************************************************************
/**********************************************************************************************************************
/* Function Name : imrdrv_try_lock */
/**********************************************************************************************************************
 * @brief       Try to lock mutex for IMR
 * @param[in]   none
 * @param[out]  none
 * @retval      0                    success
 *********************************************************************************************************************/
int imrdrv_try_lock()
{
    e_osal_return_t ret_osal;

    {
        st_osal_time_t debug_time_stamp;
        (void)R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &debug_time_stamp);
#ifdef DEBUG
         DEBUG_PRINT("FC: WAIT: %d.%09lu\n", debug_time_stamp.tv_sec, debug_time_stamp.tv_nsec);
#endif
    }

    return SUCCESS;
}

/*********************************************************************************************************************
/* Function Name : imrdrv_wait */
/*********************************************************************************************************************
 * @brief       Waits for imrdrv callback to occur
 * @param[in]   none
 * @param[out]  none
 * @retval      SUCCESS
 * @retval      FAILED
 *********************************************************************************************************************/
int imrdrv_wait(void)
{
    e_osal_return_t ret_osal;
    st_imrdrv_mq_data_t  message;
    ret_osal = R_OSAL_MqReceiveForTimePeriod(g_mq_handle_imr_0, 
                                             (osal_milli_sec_t)TIMEOUT, 
                                             &message, 
                                             sizeof(message));
    if (ret_osal != OSAL_RETURN_OK)
    {
        if (ret_osal == OSAL_RETURN_TIME)
        {
            PRINT_ERROR("Failed(TIMEOUT) R_OSAL_MqReceiveForTimePeriod , ret=%d\n", ret_osal);
            return FAILED;
        }
        else
        {
            PRINT_ERROR("Failed R_OSAL_MqReceiveForTimePeriod , ret=%d\n", ret_osal);
            return FAILED;
        }
    }

    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function imrdrv_wait
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : imrdrv_wait_1 */
/******************************************************************************************************************//**
 * @brief       Waits for imrdrv callback to occur
 * @param[in]   none      
 * @param[out]  none
 * @retval      true                    0
 * @retval      false                   1
 * @retval      false                   2
 *********************************************************************************************************************/
int imrdrv_wait_1(void)
{

    e_osal_return_t ret_osal;
    st_imrdrv_mq_data_t  message;
    ret_osal = R_OSAL_MqReceiveForTimePeriod(g_mq_handle_imr_1, 
                                             (osal_milli_sec_t)TIMEOUT, 
                                             &message, 
                                             sizeof(message));
    if (ret_osal != OSAL_RETURN_OK)
    {
        if (ret_osal == OSAL_RETURN_TIME)
        {
            PRINT_ERROR("Failed(TIMEOUT) R_OSAL_MqReceiveForTimePeriod , ret=%d\n", ret_osal);
            return FAILED;
        }
        else
        {
            PRINT_ERROR("Failed R_OSAL_MqReceiveForTimePeriod , ret=%d\n", ret_osal);
            return FAILED;
        }
    }


    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function imrdrv_wait_1
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : imrdrv_wait_2 */
/******************************************************************************************************************//**
 * @brief       Waits for imrdrv callback to occur
 * @param[in]   none      
 * @param[out]  none
 * @retval      true                    0
 * @retval      false                   1
 * @retval      false                   2
 *********************************************************************************************************************/
int imrdrv_wait_2(void)
{

    e_osal_return_t ret_osal;
    st_imrdrv_mq_data_t  message;
    ret_osal = R_OSAL_MqReceiveForTimePeriod(g_mq_handle_imr_2, 
                                             (osal_milli_sec_t)TIMEOUT, 
                                             &message, 
                                             sizeof(message));
    if (ret_osal != OSAL_RETURN_OK)
    {
        if (ret_osal == OSAL_RETURN_TIME)
        {
            PRINT_ERROR("Failed(TIMEOUT) R_OSAL_MqReceiveForTimePeriod , ret=%d\n", ret_osal);
            return FAILED;
        }
        else
        {
            PRINT_ERROR("Failed R_OSAL_MqReceiveForTimePeriod , ret=%d\n", ret_osal);
            return FAILED;
        }
    }


    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function imrdrv_wait_2
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : imrdrv_wait_3 */
/******************************************************************************************************************//**
 * @brief       Waits for imrdrv callback to occur
 * @param[in]   none      
 * @param[out]  none
 * @retval      true                    0
 * @retval      false                   1
 * @retval      false                   2
 *********************************************************************************************************************/
int imrdrv_wait_3(void)
{

    e_osal_return_t ret_osal;
    st_imrdrv_mq_data_t  message;
    ret_osal = R_OSAL_MqReceiveForTimePeriod(g_mq_handle_imr_3, 
                                             (osal_milli_sec_t)TIMEOUT, 
                                             &message, 
                                             sizeof(message));
    if (ret_osal != OSAL_RETURN_OK)
    {
        if (ret_osal == OSAL_RETURN_TIME)
        {
            PRINT_ERROR("Failed(TIMEOUT) R_OSAL_MqReceiveForTimePeriod , ret=%d\n", ret_osal);
            return FAILED;
        }
        else
        {
            PRINT_ERROR("Failed R_OSAL_MqReceiveForTimePeriod , ret=%d\n", ret_osal);
            return FAILED;
        }
    }


    return SUCCESS;
}
/******************************************************************************************************************//**
/End of function imrdrv_wait_3
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : imrdrv_wait_4 */
/******************************************************************************************************************//**
 * @brief       Waits for imrdrv callback to occur
 * @param[in]   none      
 * @param[out]  none
 * @retval      true                    0
 * @retval      false                   1
 * @retval      false                   2
 *********************************************************************************************************************/
int imrdrv_wait_4(void)
{

    e_osal_return_t ret_osal;
    st_imrdrv_mq_data_t  message;
    ret_osal = R_OSAL_MqReceiveForTimePeriod(g_mq_handle_imr_4, 
                                             (osal_milli_sec_t)TIMEOUT, 
                                             &message, 
                                             sizeof(message));
    if (ret_osal != OSAL_RETURN_OK)
    {
        if (ret_osal == OSAL_RETURN_TIME)
        {
            PRINT_ERROR("Failed(TIMEOUT) R_OSAL_MqReceiveForTimePeriod , ret=%d\n", ret_osal);
            return FAILED;
        }
        else
        {
            PRINT_ERROR("Failed R_OSAL_MqReceiveForTimePeriod , ret=%d\n", ret_osal);
            return FAILED;
        }
    }


    return SUCCESS;
}

/******************************************************************************************************************//**
/End of function imrdrv_wait_4
/******************************************************************************************************************//**

/******************************************************************************************************************//**
/* Function Name : imrdrv_callback_func */
/**********************************************************************************************************************
 * @brief       CALLBACK METHOD
 * @param[in]   ret_code                error code for callback function
 * @param[in]   details_code            buffer number
 * @param[in]   p_callback_args         pointer to p_handl_osalcond
 * @param[out]  none
 * @retval      true                    0
 * @retval      false                   1
 *********************************************************************************************************************/
static int32_t imrdrv_callback_func(const e_imrdrv_errorcode_t ret_code, const uint32_t details_code, 
                                      void * const p_callback_args)
{
    e_osal_return_t ret_osal;

    int32_t ret = SUCCESS;
    osal_mq_handle_t    handle_mq;
    st_cb_arg_t        *p_st_cb_arg;
    st_imrdrv_mq_data_t message;

    p_st_cb_arg          = (st_cb_arg_t *)p_callback_args;
    handle_mq            = p_st_cb_arg->osal_mq;
    message.details_code = details_code;

    (void)details_code;
    {
    st_osal_time_t debug_time_stamp;
    (void)R_OSAL_ClockTimeGetTimeStamp(OSAL_CLOCK_TYPE_HIGH_RESOLUTION, &debug_time_stamp);
#ifdef DEBUG
    DEBUG_PRINT("FC: SIG0: %d.%09lu\n", debug_time_stamp.tv_sec, debug_time_stamp.tv_nsec);
#endif
     }

    if (ret_code == IMRDRV_ERROR_OK)
    {

        do
        {
            ret_osal = R_OSAL_MqSendForTimePeriod(handle_mq, 
                                                  (osal_milli_sec_t)TIMEOUT, 
                                                  &message, 
                                                  sizeof(message));

            if (ret_osal == OSAL_RETURN_BUSY)
            {
                /* Do nothing */
            }
            else if (ret_osal != OSAL_RETURN_OK )
            {
                PRINT_ERROR("[RESULT]<FAIL> Failed R_OSAL_MqSendForTimePeriod ret=%d\n", ret_osal);
                ret = FAILED;
            }
        } while (ret_osal == OSAL_RETURN_BUSY);
    }
    else
    {
        DEBUG_PRINT("callback_func received error code =%d\n", ret_code);
    }

    return ret;
}
/*******************************************************************************************************************
/End of function imrdrv_callback_func
/******************************************************************************************************************/
