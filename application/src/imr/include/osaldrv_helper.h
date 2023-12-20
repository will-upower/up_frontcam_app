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
 * File Name    : osaldrv_helper.h
 * Version      : 0.1.0
 * Description  : Helper functions (inofficial) to simplify OSAL/MMGR/IMR Driver usage

*********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 06.03.2023 0.1.0     Alpha Release (Base: R-CAR SDK V2.17 sample)
 *********************************************************************************************************************/
#ifndef OSALDRV_HELPER_H
#define OSALDRV_HELPER_H

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include "rcar-xos/imr/r_imrdrv_type.h"
#include "imr_main.h"

/**********************************************************************************************************************
 Global Constants
 *********************************************************************************************************************/
/* IMR channels */
#define IMR_CH_0                (0)
#define IMR_CH_1                (1)
#define IMR_CH_2                (2)
#define IMR_CH_3                (3)
#define IMR_CH_4                (4)
#define IMR_CH_MAX              (5)

/**********************************************************************************************************************
 Global variable declarations
 *********************************************************************************************************************/
/* OSAL */
osal_mq_handle_t    g_mq_handle_imr_0;
osal_mq_id_t        g_mq_imr_id;
osal_mq_handle_t    g_mq_handle_imr_1;
osal_mq_handle_t    g_mq_handle_imr_2;
osal_mq_handle_t    g_mq_handle_imr_3;
osal_mq_handle_t    g_mq_handle_imr_4;

/**********************************************************************************************************************
 Function Declarations
*********************************************************************************************************************/
int init_osal ();
int init_mmgr ();
int init_imrdrv ();
int deinit_imrdrv ();
int deinit_mmgr ();
int mmgr_helper_alloc_buffer (osal_memory_buffer_handle_t * buf, int imr_channel, uint32_t data_size, uint32_t align, 
                                                                    addr_t * vmr_addr, uintptr_t * phys_addr);
int mmgr_helper_flush (osal_memory_buffer_handle_t buf);
int mmgr_helper_dealloc (osal_memory_buffer_handle_t buf);
int allocate_dl_memory (osal_memory_buffer_handle_t * mmngr_buf_handle, size_t data_size, size_t align, void * p_vmr_addr);

/* Returns pointer to list of IMR channel control handles (array with MAX_MODULE elements),
 which is created by init_imrdrv() and needed to communicate with the IMR driver for a specific channel. */

imrdrv_ctrl_handle_t get_imrdrv_ctrlhandles (int channel);

/* This function Won't work when running DLs on multiple IMRs (we may have a design issue here)*/
 int imrdrv_wait ();
 int imrdrv_wait_1 ();
 int imrdrv_wait_2 ();
 int imrdrv_wait_3 ();
 int imrdrv_wait_4 ();

#endif // OSALDRV_HELPER_H
