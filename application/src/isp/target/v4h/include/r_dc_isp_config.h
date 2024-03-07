#ifndef _R_ISP_EXAMPLE_CONFIG_H_
#define _R_ISP_EXAMPLE_CONFIG_H_        0

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "rcar-xos/cisp/r_dc_cisp_rpp.h"

#define FILE_IMAGE_WIDTH           1296
#define INPUT_IMAGE_WIDTH          1296
#define INPUT_IMAGE_HEIGHT         784


/* ACQ config table*/
st_cisp_acq_config_t acq_pre1_enable_config_table =
{
    .input_window.h_offset                = 0,
    .input_window.v_offset                = 0,

    .input_window.width                   = INPUT_IMAGE_WIDTH,
    .input_window.height                  = INPUT_IMAGE_HEIGHT,

    .output_window.h_offset               = 0,
    .output_window.v_offset               = 0,
    .output_window.width                  = INPUT_IMAGE_WIDTH,
    .output_window.height                 = INPUT_IMAGE_HEIGHT,
    .input_mode                           = CISP_ACQ_MODE_BAYER_BT601,
    .acq_properties.sensor_align_in       = CISP_ACQ_ALIGN_LSB,
    .acq_properties.bayer_pattern         = CISP_ACQ_BAYPAT_GRBG,
    .acq_properties.input_sample_width    = CISP_ACQ_INPUT_WIDTH_16b,
    .acq_module_on                        = true,
};
st_cisp_acq_config_t acq_pre2_disable_config_table =
{
    .input_window.h_offset                = 0,
    .input_window.v_offset                = 0,
    .input_window.width                   = INPUT_IMAGE_WIDTH,
    .input_window.height                  = INPUT_IMAGE_HEIGHT,
    .output_window.h_offset               = 0,
    .output_window.v_offset               = 0,
    .output_window.width                  = INPUT_IMAGE_WIDTH,
    .output_window.height                 = INPUT_IMAGE_HEIGHT,
    .input_mode                           = CISP_ACQ_MODE_BAYER_BT601,
    .acq_properties.sensor_align_in       = CISP_ACQ_ALIGN_LSB,
    .acq_properties.bayer_pattern         = CISP_ACQ_BAYPAT_GRBG,
    .acq_properties.input_sample_width    = CISP_ACQ_INPUT_WIDTH_16b,
    .acq_module_on                        = false,
};
st_cisp_acq_config_t acq_pre1_hdr_config_table =
{
    .input_window.h_offset                = 0,
    .input_window.v_offset                = 0,
    .input_window.width                   = INPUT_IMAGE_WIDTH,
    .input_window.height                  = INPUT_IMAGE_HEIGHT,
    .output_window.h_offset               = 0,
    .output_window.v_offset               = 0,
    .output_window.width                  = INPUT_IMAGE_WIDTH,
    .output_window.height                 = INPUT_IMAGE_HEIGHT,
    .input_mode                           = CISP_ACQ_MODE_BAYER_BT601,
    .acq_properties.sensor_align_in       = CISP_ACQ_ALIGN_LSB,
    .acq_properties.bayer_pattern         = CISP_ACQ_BAYPAT_RGGB,
    .acq_properties.input_sample_width    = CISP_ACQ_INPUT_WIDTH_12b,
    .acq_module_on                        = true,
};
st_cisp_acq_config_t acq_pre2_hdr_config_table =
{
    .input_window.h_offset                = 0,
    .input_window.v_offset                = 0,
    .input_window.width                   = INPUT_IMAGE_WIDTH,
    .input_window.height                  = INPUT_IMAGE_HEIGHT,
    .output_window.h_offset               = 0,
    .output_window.v_offset               = 0,
    .output_window.width                  = INPUT_IMAGE_WIDTH,
    .output_window.height                 = INPUT_IMAGE_HEIGHT,
    .input_mode                           = CISP_ACQ_MODE_BAYER_BT601,
    .acq_properties.sensor_align_in       = CISP_ACQ_ALIGN_LSB,
    .acq_properties.bayer_pattern         = CISP_ACQ_BAYPAT_RGGB,
    .acq_properties.input_sample_width    = CISP_ACQ_INPUT_WIDTH_12b,
    .acq_module_on                        = true,
};

/* OUTIF config table*/
st_cisp_outif_config_t outif_config_table =
{
    .num_of_frames = 0,
};
st_cisp_outif_config_t outif_config_continous_cpu_table =
{
    .num_of_frames = 1,
};

/* RMAP config table*/
st_cisp_rmap_config_t rmap_config_table =
{
    .mode               = CISP_RMAP_MODE_BYPASS_PRE1,
    .long_bits          = CISP_RMAP_LONG_EXP_12BIT,
    .rmap_shadow_config.h_size                        = 0,
    .rmap_shadow_config.long_short_blending.step_size = 0,
    .rmap_shadow_config.long_short_blending.th_max    = 0,
    .rmap_shadow_config.long_short_blending.th_min    = 0,
    .rmap_shadow_config.rmap_factor_short             = 0,
    .wb_long.gain_b     = 0,
    .wb_long.gain_r     = 0,
    .wb_long.threshold  = 0,
    .wb_short.gain_b    = 0,
    .wb_short.gain_r    = 0,
    .wb_short.threshold = 0,
};
st_cisp_rmap_config_t rmap_hdr_config_table =
{
    .mode               = CISP_RMAP_MODE_FUSE,
    .long_bits          = CISP_RMAP_LONG_EXP_12BIT,
    .rmap_shadow_config.h_size                        = INPUT_IMAGE_WIDTH,
    .rmap_shadow_config.long_short_blending.step_size = 0x174,
    .rmap_shadow_config.long_short_blending.th_max    = 0xF00,
    .rmap_shadow_config.long_short_blending.th_min    = 0x400,
    .rmap_shadow_config.rmap_factor_short             = 0x008,
    .wb_long.gain_b     = 0x3FF,
    .wb_long.gain_r     = 0x3FF,
    .wb_long.threshold  = 0xFFF,
    .wb_short.gain_b    = 0x3FF,
    .wb_short.gain_r    = 0x3FF,
    .wb_short.threshold = 0xFFF,
};

/* GAMMAIN config table*/
st_cisp_gammain_config_t gammain_pre1_enable_config_table =
{
    .gamma_in_module_on = true,
    .curve[0] =
    {
        .x = 0,
        .red = 0,
        .green = 0,
        .blue = 0,
    },
    .curve[1] =
    {
        .x = 15,
        .red = 1048576,
        .green = 1048576,
        .blue = 1048576,
    },
    .curve[2] =
    {
        .x = 14,
        .red = 6815744,
        .green = 6815744,
        .blue = 6815744,
    },
    .curve[3] =
    {
        .x = 14,
        .red = 16777215,
        .green = 16777215,
        .blue = 16777215,
    },
};
st_cisp_gammain_config_t gammain_pre1_disable_config_table =
{
    .gamma_in_module_on = false,
    .curve[0] =
    {
        .x = 0,
        .red = 0,
        .green = 0,
        .blue = 0,
    },
    .curve[1] =
    {
        .x = 15,
        .red = 1048576,
        .green = 1048576,
        .blue = 1048576,
    },
    .curve[2] =
    {
        .x = 14,
        .red = 6815744,
        .green = 6815744,
        .blue = 6815744,
    },
    .curve[3] =
    {
        .x = 14,
        .red = 16777215,
        .green = 16777215,
        .blue = 16777215,
    },
};
st_cisp_gammain_config_t gammain_pre2_disable_config_table =
{
    .gamma_in_module_on   = false,
    .curve[0] =
    {
        .x = 4,
        .red = 0,
        .green = 0,
        .blue = 0,
    },
    .curve[1] =
    {
        .x = 4,
        .red = 0,
        .green = 0,
        .blue = 0,
    },
    .curve[2] =
    {
        .x = 4,
        .red = 0,
        .green = 0,
        .blue = 0,
    },
};

/* GAMMAOUT config table*/
st_cisp_gammaout_config_t gammaout_hvout_enable_config_table =
{
    /* BT709 gamma, 12bit out*/
    .y_curve[0]          = 0,
    .y_curve[1]          = 1151,
    .y_curve[2]          = 2162,
    .y_curve[3]          = 2920,
    .y_curve[4]          = 3547,
    .y_curve[5]          = 4095,
    .y_curve[6]          = 4095,
    .y_curve[7]          = 4095,
    .y_curve[8]          = 4095,
    .y_curve[9]          = 4095,
    .y_curve[10]         = 4095,
    .y_curve[11]         = 4095,
    .y_curve[12]         = 4095,
    .y_curve[13]         = 4095,
    .y_curve[14]         = 4095,
    .y_curve[15]         = 4095,
    .y_curve[16]         = 4095,
    .gamma_out_module_on = true,
};
st_cisp_gammaout_config_t gammaout_hvout_disable_config_table =
{
    /* BT709 gamma, 12bit out*/
    .y_curve[0]          = 0,
    .y_curve[1]          = 1151,
    .y_curve[2]          = 2162,
    .y_curve[3]          = 2920,
    .y_curve[4]          = 3547,
    .y_curve[5]          = 4095,
    .y_curve[6]          = 4095,
    .y_curve[7]          = 4095,
    .y_curve[8]          = 4095,
    .y_curve[9]          = 4095,
    .y_curve[10]         = 4095,
    .y_curve[11]         = 4095,
    .y_curve[12]         = 4095,
    .y_curve[13]         = 4095,
    .y_curve[14]         = 4095,
    .y_curve[15]         = 4095,
    .y_curve[16]         = 4095,
    .gamma_out_module_on = false,
};
st_cisp_gammaout_config_t gammaout_mvout_enable_config_table =
{
    /* BT709 gamma, 12bit out*/
    .y_curve[0]          = 0,
    .y_curve[1]          = 4718592,
    .y_curve[2]          = 8860805,
    .y_curve[3]          = 11964285,
    .y_curve[4]          = 14536107,
    .y_curve[5]          = 16777215,
    .y_curve[6]          = 16777215,
    .y_curve[7]          = 16777215,
    .y_curve[8]          = 16777215,
    .y_curve[9]          = 16777215,
    .y_curve[10]         = 16777215,
    .y_curve[11]         = 16777215,
    .y_curve[12]         = 16777215,
    .y_curve[13]         = 16777215,
    .y_curve[14]         = 16777215,
    .y_curve[15]         = 16777215,
    .y_curve[16]         = 16777215,
    .gamma_out_module_on = true,
};
st_cisp_gammaout_config_t gammaout_mvout_disable_config_table =
{
    /* BT709 gamma, 12bit out*/
    .y_curve[0]          = 0,
    .y_curve[1]          = 4718592,
    .y_curve[2]          = 8860805,
    .y_curve[3]          = 11964285,
    .y_curve[4]          = 14536107,
    .y_curve[5]          = 16777215,
    .y_curve[6]          = 16777215,
    .y_curve[7]          = 16777215,
    .y_curve[8]          = 16777215,
    .y_curve[9]          = 16777215,
    .y_curve[10]         = 16777215,
    .y_curve[11]         = 16777215,
    .y_curve[12]         = 16777215,
    .y_curve[13]         = 16777215,
    .y_curve[14]         = 16777215,
    .y_curve[15]         = 16777215,
    .y_curve[16]         = 16777215,
    .gamma_out_module_on = false,
};
st_cisp_gammaout_config_t gammaout_hdr_config_table =
{
    /* BT709 gamma, 12bit out*/
    .y_curve[0]          = 0,
    .y_curve[1]          = 0,
    .y_curve[2]          = 0,
    .y_curve[3]          = 0,
    .y_curve[4]          = 0,
    .y_curve[5]          = 0,
    .y_curve[6]          = 0,
    .y_curve[7]          = 0,
    .y_curve[8]          = 0,
    .y_curve[9]          = 0,
    .y_curve[10]         = 0,
    .y_curve[11]         = 0,
    .y_curve[12]         = 0,
    .y_curve[13]         = 0,
    .y_curve[14]         = 0,
    .y_curve[15]         = 0,
    .y_curve[16]         = 0,
    .gamma_out_module_on = false,
};

/* AWB config table*/
st_cisp_awb_config_t awb_pre1_enable_config_table =
{
    .awb_module_on    = true,
    .gain_red         = 4096,
    .gain_green_red   = 4096,
    .gain_blue        = 4096,
    .gain_green_blue  = 4096,
};
st_cisp_awb_config_t awb_pre1_disable_config_table =
{
    .awb_module_on    = false,
    .gain_red         = 4096,
    .gain_green_red   = 4096,
    .gain_blue        = 4096,
    .gain_green_blue  = 4096,
};
st_cisp_awb_config_t awb_pre2_disable_config_table =
{
    .awb_module_on    = false,
    .gain_red         = 4096,
    .gain_green_red   = 4096,
    .gain_blue        = 4096,
    .gain_green_blue  = 4096,
};
st_cisp_awb_config_t awb_post_enable_config_table =
{
    .awb_module_on    = true,
    .gain_red         = 4096,
    .gain_green_red   = 4096,
    .gain_blue        = 4096,
    .gain_green_blue  = 4096,
};
st_cisp_awb_config_t awb_post_disable_config_table =
{
    .awb_module_on    = false,
    .gain_red         = 4096,
    .gain_green_red   = 4096,
    .gain_blue        = 4096,
    .gain_green_blue  = 4096,
};
st_cisp_awb_config_t awb_pre1_hdr_config_table =
{
    .awb_module_on    = false,
    .gain_red         = 4096,
    .gain_green_red   = 4096,
    .gain_blue        = 4096,
    .gain_green_blue  = 4096,
};
st_cisp_awb_config_t awb_pre2_hdr_config_table =
{
    .awb_module_on    = false,
    .gain_red         = 4096,
    .gain_green_red   = 4096,
    .gain_blue        = 4096,
    .gain_green_blue  = 4096,
};
st_cisp_awb_config_t awb_post_hdr_config_table =
{
    .awb_module_on    = false,
    .gain_red         = 4096,
    .gain_green_red   = 4096,
    .gain_blue        = 4096,
    .gain_green_blue  = 4096,
};

/* AWBMEAS config table*/
st_cisp_awbmeas_config_t awbmeas_enable_config_table =
{
    .measure_mode         = CISP_AWBMEAS_MODE_RGB,
    .rgb_params.max_g     = 12582912,
    .rgb_params.max_b     = 8388608,
    .rgb_params.max_r     = 8388608,
    .window.h_offset      = 0,
    .window.v_offset      = 0,
    .window.width         = INPUT_IMAGE_WIDTH,
    .window.height        = INPUT_IMAGE_HEIGHT,
    .nb_of_frames         = 1,

    .yuv_params.ccor_profile.coeff[0] = 2717,
    .yuv_params.ccor_profile.coeff[1] = 1420,
    .yuv_params.ccor_profile.coeff[2] = -41,
    .yuv_params.ccor_profile.coeff[3] = 437,
    .yuv_params.ccor_profile.coeff[4] = 3372,
    .yuv_params.ccor_profile.coeff[5] = 287,
    .yuv_params.ccor_profile.coeff[6] = 4,
    .yuv_params.ccor_profile.coeff[7] = 1411,
    .yuv_params.ccor_profile.coeff[8] = 2681,
};
st_cisp_awbmeas_config_t awbmeas_disable_config_table =
{
    .measure_mode         = CISP_AWBMEAS_MODE_OFF,
    .rgb_params.max_g     = 12582912,
    .rgb_params.max_b     = 8388608,
    .rgb_params.max_r     = 8388608,
    .window.h_offset      = 0,
    .window.v_offset      = 0,
    .window.width         = INPUT_IMAGE_WIDTH,
    .window.height        = INPUT_IMAGE_HEIGHT,
    .nb_of_frames         = 1,

    .yuv_params.ccor_profile.coeff[0] = 2717,
    .yuv_params.ccor_profile.coeff[1] = 1420,
    .yuv_params.ccor_profile.coeff[2] = -41,
    .yuv_params.ccor_profile.coeff[3] = 437,
    .yuv_params.ccor_profile.coeff[4] = 3372,
    .yuv_params.ccor_profile.coeff[5] = 287,
    .yuv_params.ccor_profile.coeff[6] = 4,
    .yuv_params.ccor_profile.coeff[7] = 1411,
    .yuv_params.ccor_profile.coeff[8] = 2681,
};

/* EXM config table*/
st_cisp_exm_config_t exm_pre1_enable_config_table =
{
    .measure_mode                 = CISP_EXM_MEASURE_MODE_Y,
    .meas_wind_config.h_offset    = 0,
    .meas_wind_config.v_offset    = 0,
    /**************************************************************************************
     *_LAST_MEAS_LINE  <= last line in active video
     * _LAST_MEAS_LINE > _V_OFFSET + 5*_V_SIZE
     *
     * _LAST_MEAS_LINE sets V_SIZE×5+1.
     * But _LAST_MEAS_LINE must be contained to size of last line in active video.
     * Dividing window of EXM by 5, _LAST_MEAS_LINE may exceed last line in active video.
     * So, the sample settings are divided by 6 to provide a mergin.
     * Please refer to H/W manual for details.
     **************************************************************************************/
    .meas_wind_config.width       = INPUT_IMAGE_WIDTH/5,
    .meas_wind_config.height      = INPUT_IMAGE_HEIGHT/6,
    .channel                      = 4,
};
st_cisp_exm_config_t exm_pre1_disable_config_table =
{
    .measure_mode                 = CISP_EXM_MEASURE_MODE_DISABLE,
    .meas_wind_config.h_offset    = 0,
    .meas_wind_config.v_offset    = 0,
    .meas_wind_config.width       = INPUT_IMAGE_WIDTH/5,
    .meas_wind_config.height      = INPUT_IMAGE_HEIGHT/6,
    .channel                      = 4,
};
st_cisp_exm_config_t exm_pre2_disable_config_table =
{
    .measure_mode                 = CISP_EXM_MEASURE_MODE_DISABLE,
    .meas_wind_config.h_offset    = 0,
    .meas_wind_config.v_offset    = 0,
    .meas_wind_config.width       = 13,
    .meas_wind_config.height      = 0,
    .channel                      = 0,
};
st_cisp_exm_config_t exm_pre1_hdr_config_table =
{
    .measure_mode                 = CISP_EXM_MEASURE_MODE_Y,
    .meas_wind_config.h_offset    = 0x00A,
    .meas_wind_config.v_offset    = 0x00F,
    /**************************************************************************************
     *_LAST_MEAS_LINE  <= last line in active video
     * _LAST_MEAS_LINE > _V_OFFSET + 5*_V_SIZE
     *
     * _LAST_MEAS_LINE sets V_SIZE×5+1.
     * But _LAST_MEAS_LINE must be contained to size of last line in active video.
     * Dividing window of EXM by 5, _LAST_MEAS_LINE may exceed last line in active video.
     * So, the sample settings are divided by 6 to provide a mergin.
     * Please refer to H/W manual for details.
     **************************************************************************************/
    .meas_wind_config.width       = INPUT_IMAGE_WIDTH/5,
    .meas_wind_config.height      = INPUT_IMAGE_HEIGHT/6,
    .channel                      = 0,
};
st_cisp_exm_config_t exm_pre2_hdr_config_table =
{
    .measure_mode                 = CISP_EXM_MEASURE_MODE_Y,
    .meas_wind_config.h_offset    = 0x00A,
    .meas_wind_config.v_offset    = 0x00F,
    .meas_wind_config.width       = INPUT_IMAGE_WIDTH/5,
    .meas_wind_config.height      = INPUT_IMAGE_HEIGHT/6,
    .channel                      = 0,
};

/* LTM config table*/
st_cisp_ltm_config_t ltm_enable_config_table =
{
    .line_width                   = INPUT_IMAGE_WIDTH,
    .ltm_enable                   = true,
    .params.cgain                 = 0x0000199A,
    .params.k_diff_d              = 0x00000523,
    .params.k_diff_d_r            = 0x000031D8,
    .params.k_max_d               = 0x000089D3,
    .params.k_min_d               = 0x000084B0,
    .params.kw                    = 0x00003E7A,
    .params.kw_r                  = 0x00004190,
    .params.l0d                   = 0x00003468,
    .params.l0d_r                 = 0x000138A5,
    .params.l0w                   = 0x0001250D,
    .params.l0w_r                 = 0x000037E8,
    .params.lprch_r               = 0x0D79435E,
    .tone_curve.dx[0]             = 0,
    .tone_curve.dx[1]             = 3,
    .tone_curve.dx[2]             = 3,
    .tone_curve.dx[3]             = 3,
    .tone_curve.dx[4]             = 3,
    .tone_curve.dx[5]             = 5,
    .tone_curve.dx[6]             = 6,
    .tone_curve.dx[7]             = 7,
    .tone_curve.dx[8]             = 8,
    .tone_curve.dx[9]             = 9,
    .tone_curve.dx[10]            = 10,
    .tone_curve.dx[11]            = 10,
    .tone_curve.dx[12]            = 10,
    .tone_curve.dx[13]            = 10,
    .tone_curve.dx[14]            = 10,
    .tone_curve.dx[15]            = 10,
    .tone_curve.dx[16]            = 10,
    .tone_curve.dx[17]            = 10,
    .tone_curve.dx[18]            = 10,
    .tone_curve.dx[19]            = 10,
    .tone_curve.dx[20]            = 10,
    .tone_curve.dx[21]            = 10,
    .tone_curve.dx[22]            = 10,
    .tone_curve.dx[23]            = 10,
    .tone_curve.dx[24]            = 10,
    .tone_curve.dx[25]            = 10,
    .tone_curve.dx[26]            = 10,
    .tone_curve.dx[27]            = 10,
    .tone_curve.dx[28]            = 10,
    .tone_curve.dx[29]            = 10,
    .tone_curve.dx[30]            = 10,
    .tone_curve.dx[31]            = 10,
    .tone_curve.dx[32]            = 10,
    .tone_curve.dx[33]            = 10,
    .tone_curve.dx[34]            = 10,
    .tone_curve.dx[35]            = 10,
    .tone_curve.dx[36]            = 10,
    .tone_curve.dx[37]            = 10,
    .tone_curve.dx[38]            = 10,
    .tone_curve.dx[39]            = 10,
    .tone_curve.dx[40]            = 10,
    .tone_curve.dx[41]            = 10,
    .tone_curve.dx[42]            = 10,
    .tone_curve.dx[43]            = 10,
    .tone_curve.dx[44]            = 10,
    .tone_curve.dx[45]            = 11,
    .tone_curve.dx[46]            = 11,
    .tone_curve.dx[47]            = 13,
    .tone_curve.dx[48]            = 14,
    .tone_curve.y[0]              = 0,
    .tone_curve.y[1]              = 0,
    .tone_curve.y[2]              = 0,
    .tone_curve.y[3]              = 0,
    .tone_curve.y[4]              = 0,
    .tone_curve.y[5]              = 15,
    .tone_curve.y[6]              = 112,
    .tone_curve.y[7]              = 288,
    .tone_curve.y[8]              = 588,
    .tone_curve.y[9]              = 1059,
    .tone_curve.y[10]             = 1745,
    .tone_curve.y[11]             = 2267,
    .tone_curve.y[12]             = 2700,
    .tone_curve.y[13]             = 3079,
    .tone_curve.y[14]             = 3420,
    .tone_curve.y[15]             = 3733,
    .tone_curve.y[16]             = 4026,
    .tone_curve.y[17]             = 4095,
    .tone_curve.y[18]             = 4095,
    .tone_curve.y[19]             = 4095,
    .tone_curve.y[20]             = 4095,
    .tone_curve.y[21]             = 4095,
    .tone_curve.y[22]             = 4095,
    .tone_curve.y[23]             = 4095,
    .tone_curve.y[24]             = 4095,

    .tone_curve.y[25]             = 4095,
    .tone_curve.y[26]             = 4095,
    .tone_curve.y[27]             = 4095,
    .tone_curve.y[28]             = 4095,
    .tone_curve.y[29]             = 4095,
    .tone_curve.y[30]             = 4095,
    .tone_curve.y[31]             = 4095,
    .tone_curve.y[32]             = 4095,

    .tone_curve.y[33]             = 4095,
    .tone_curve.y[34]             = 4095,
    .tone_curve.y[35]             = 4095,
    .tone_curve.y[36]             = 4095,
    .tone_curve.y[37]             = 4095,
    .tone_curve.y[38]             = 4095,
    .tone_curve.y[39]             = 4095,
    .tone_curve.y[40]             = 4095,
    .tone_curve.y[41]             = 4095,
    .tone_curve.y[42]             = 4095,
    .tone_curve.y[43]             = 4095,
    .tone_curve.y[44]             = 4095,
    .tone_curve.y[45]             = 4095,
    .tone_curve.y[46]             = 4095,
    .tone_curve.y[47]             = 4095,
    .tone_curve.y[48]             = 4095,
    .weights.blue                 = 0x100,
    .weights.green                = 0x1FF,
    .weights.red                  = 0x100,
};
st_cisp_ltm_config_t ltm_disable_config_table =
{
    .line_width                   = INPUT_IMAGE_WIDTH,
    .ltm_enable                   = false,
    .params.cgain                 = 0x0000199A,
    .params.k_diff_d              = 0x00000523,
    .params.k_diff_d_r            = 0x000031D8,
    .params.k_max_d               = 0x000089D3,
    .params.k_min_d               = 0x000084B0,
    .params.kw                    = 0x00003E7A,
    .params.kw_r                  = 0x00004190,
    .params.l0d                   = 0x00003468,
    .params.l0d_r                 = 0x000138A5,
    .params.l0w                   = 0x0001250D,
    .params.l0w_r                 = 0x000037E8,
    .params.lprch_r               = 0x0D79435E,
    .tone_curve.dx[0]             = 0,
    .tone_curve.dx[1]             = 3,
    .tone_curve.dx[2]             = 3,
    .tone_curve.dx[3]             = 3,
    .tone_curve.dx[4]             = 3,
    .tone_curve.dx[5]             = 5,
    .tone_curve.dx[6]             = 6,
    .tone_curve.dx[7]             = 7,
    .tone_curve.dx[8]             = 8,
    .tone_curve.dx[9]             = 9,
    .tone_curve.dx[10]            = 10,
    .tone_curve.dx[11]            = 10,
    .tone_curve.dx[12]            = 10,
    .tone_curve.dx[13]            = 10,
    .tone_curve.dx[14]            = 10,
    .tone_curve.dx[15]            = 10,
    .tone_curve.dx[16]            = 10,
    .tone_curve.dx[17]            = 10,
    .tone_curve.dx[18]            = 10,
    .tone_curve.dx[19]            = 10,
    .tone_curve.dx[20]            = 10,
    .tone_curve.dx[21]            = 10,
    .tone_curve.dx[22]            = 10,
    .tone_curve.dx[23]            = 10,
    .tone_curve.dx[24]            = 10,
    .tone_curve.dx[25]            = 10,
    .tone_curve.dx[26]            = 10,
    .tone_curve.dx[27]            = 10,
    .tone_curve.dx[28]            = 10,
    .tone_curve.dx[29]            = 10,
    .tone_curve.dx[30]            = 10,
    .tone_curve.dx[31]            = 10,
    .tone_curve.dx[32]            = 10,
    .tone_curve.dx[33]            = 10,
    .tone_curve.dx[34]            = 10,
    .tone_curve.dx[35]            = 10,
    .tone_curve.dx[36]            = 10,
    .tone_curve.dx[37]            = 10,
    .tone_curve.dx[38]            = 10,
    .tone_curve.dx[39]            = 10,
    .tone_curve.dx[40]            = 10,
    .tone_curve.dx[41]            = 10,
    .tone_curve.dx[42]            = 10,
    .tone_curve.dx[43]            = 10,
    .tone_curve.dx[44]            = 10,
    .tone_curve.dx[45]            = 11,
    .tone_curve.dx[46]            = 11,
    .tone_curve.dx[47]            = 13,
    .tone_curve.dx[48]            = 14,
    .tone_curve.y[0]              = 0,
    .tone_curve.y[1]              = 0,
    .tone_curve.y[2]              = 0,
    .tone_curve.y[3]              = 0,
    .tone_curve.y[4]              = 0,
    .tone_curve.y[5]              = 15,
    .tone_curve.y[6]              = 112,
    .tone_curve.y[7]              = 288,
    .tone_curve.y[8]              = 588,
    .tone_curve.y[9]              = 1059,
    .tone_curve.y[10]             = 1745,
    .tone_curve.y[11]             = 2267,
    .tone_curve.y[12]             = 2700,
    .tone_curve.y[13]             = 3079,
    .tone_curve.y[14]             = 3420,
    .tone_curve.y[15]             = 3733,
    .tone_curve.y[16]             = 4026,
    .tone_curve.y[17]             = 4095,
    .tone_curve.y[18]             = 4095,
    .tone_curve.y[19]             = 4095,
    .tone_curve.y[20]             = 4095,
    .tone_curve.y[21]             = 4095,
    .tone_curve.y[22]             = 4095,
    .tone_curve.y[23]             = 4095,
    .tone_curve.y[24]             = 4095,

    .tone_curve.y[25]             = 4095,
    .tone_curve.y[26]             = 4095,
    .tone_curve.y[27]             = 4095,
    .tone_curve.y[28]             = 4095,
    .tone_curve.y[29]             = 4095,
    .tone_curve.y[30]             = 4095,
    .tone_curve.y[31]             = 4095,
    .tone_curve.y[32]             = 4095,

    .tone_curve.y[33]             = 4095,
    .tone_curve.y[34]             = 4095,
    .tone_curve.y[35]             = 4095,
    .tone_curve.y[36]             = 4095,
    .tone_curve.y[37]             = 4095,
    .tone_curve.y[38]             = 4095,
    .tone_curve.y[39]             = 4095,
    .tone_curve.y[40]             = 4095,
    .tone_curve.y[41]             = 4095,
    .tone_curve.y[42]             = 4095,
    .tone_curve.y[43]             = 4095,
    .tone_curve.y[44]             = 4095,
    .tone_curve.y[45]             = 4095,
    .tone_curve.y[46]             = 4095,
    .tone_curve.y[47]             = 4095,
    .tone_curve.y[48]             = 4095,
    .weights.blue                 = 0x100,
    .weights.green                = 0x1FF,
    .weights.red                  = 0x100,
};
st_cisp_ltm_config_t ltm_hdr_config_table =
{
    .line_width                   = INPUT_IMAGE_WIDTH,
    .ltm_enable                   = true,
    .params.cgain                 = 0,
    .params.k_diff_d              = 0x526,
    .params.k_diff_d_r            = 0x31BA,
    .params.k_max_d               = 0xAF77,
    .params.k_min_d               = 0xAA51,
    .params.kw                    = 0x5166,
    .params.kw_r                  = 0x3252,
    .params.l0d                   = 0xA3D,
    .params.l0d_r                 = 0x64000,
    .params.l0w                   = 0x7C1F,
    .params.l0w_r                 = 0x8400,
    .params.lprch_r               = 0xF07C1F08,
    .tone_curve.dx[0]             = 0,
    .tone_curve.dx[1]             = 0,
    .tone_curve.dx[2]             = 0,
    .tone_curve.dx[3]             = 0,
    .tone_curve.dx[4]             = 0,
    .tone_curve.dx[5]             = 0,
    .tone_curve.dx[6]             = 0,
    .tone_curve.dx[7]             = 0,
    .tone_curve.dx[8]             = 0,
    .tone_curve.dx[9]             = 0,
    .tone_curve.dx[10]            = 0,
    .tone_curve.dx[11]            = 0,
    .tone_curve.dx[12]            = 0,
    .tone_curve.dx[13]            = 0,
    .tone_curve.dx[14]            = 0,
    .tone_curve.dx[15]            = 0,
    .tone_curve.dx[16]            = 0,
    .tone_curve.dx[17]            = 0,
    .tone_curve.dx[18]            = 0,
    .tone_curve.dx[19]            = 0,
    .tone_curve.dx[20]            = 0,
    .tone_curve.dx[21]            = 0,
    .tone_curve.dx[22]            = 0,
    .tone_curve.dx[23]            = 0,
    .tone_curve.dx[24]            = 0,
    .tone_curve.dx[25]            = 0,
    .tone_curve.dx[26]            = 0,
    .tone_curve.dx[27]            = 0,
    .tone_curve.dx[28]            = 0,
    .tone_curve.dx[29]            = 1,
    .tone_curve.dx[30]            = 1,
    .tone_curve.dx[31]            = 3,
    .tone_curve.dx[32]            = 3,
    .tone_curve.dx[33]            = 4,
    .tone_curve.dx[34]            = 6,
    .tone_curve.dx[35]            = 6,
    .tone_curve.dx[36]            = 6,
    .tone_curve.dx[37]            = 8,
    .tone_curve.dx[38]            = 8,
    .tone_curve.dx[39]            = 8,
    .tone_curve.dx[40]            = 10,
    .tone_curve.dx[41]            = 10,
    .tone_curve.dx[42]            = 10,
    .tone_curve.dx[43]            = 12,
    .tone_curve.dx[44]            = 12,
    .tone_curve.dx[45]            = 12,
    .tone_curve.dx[46]            = 14,
    .tone_curve.dx[47]            = 14,
    .tone_curve.dx[48]            = 14,
    .tone_curve.y[0]              = 0,
    .tone_curve.y[1]              = 0,
    .tone_curve.y[2]              = 183,
    .tone_curve.y[3]              = 403,
    .tone_curve.y[4]              = 591,
    .tone_curve.y[5]              = 753,
    .tone_curve.y[6]              = 896,
    .tone_curve.y[7]              = 1023,
    .tone_curve.y[8]              = 1137,
    .tone_curve.y[9]              = 1241,
    .tone_curve.y[10]             = 1337,
    .tone_curve.y[11]             = 1424,
    .tone_curve.y[12]             = 1506,
    .tone_curve.y[13]             = 1582,
    .tone_curve.y[14]             = 1653,
    .tone_curve.y[15]             = 1720,
    .tone_curve.y[16]             = 1783,
    .tone_curve.y[17]             = 1843,
    .tone_curve.y[18]             = 1900,
    .tone_curve.y[19]             = 1954,
    .tone_curve.y[20]             = 2005,
    .tone_curve.y[21]             = 2054,
    .tone_curve.y[22]             = 2102,
    .tone_curve.y[23]             = 2147,
    .tone_curve.y[24]             = 2191,
    .tone_curve.y[25]             = 2233,
    .tone_curve.y[26]             = 2273,
    .tone_curve.y[27]             = 2312,
    .tone_curve.y[28]             = 2350,
    .tone_curve.y[29]             = 2422,
    .tone_curve.y[30]             = 2490,
    .tone_curve.y[31]             = 2727,
    .tone_curve.y[32]             = 2926,
    .tone_curve.y[33]             = 3246,
    .tone_curve.y[34]             = 4060,
    .tone_curve.y[35]             = 4095,
    .tone_curve.y[36]             = 4095,
    .tone_curve.y[37]             = 4095,
    .tone_curve.y[38]             = 4095,
    .tone_curve.y[39]             = 4095,
    .tone_curve.y[40]             = 4095,
    .tone_curve.y[41]             = 4095,
    .tone_curve.y[42]             = 4095,
    .tone_curve.y[43]             = 4095,
    .tone_curve.y[44]             = 4095,
    .tone_curve.y[45]             = 4095,
    .tone_curve.y[46]             = 4095,
    .tone_curve.y[47]             = 4095,
    .tone_curve.y[48]             = 4095,
    .weights.blue                 = 0x100,
    .weights.green                = 0x1FF,
    .weights.red                  = 0x100,
};

/* LTMMEAS config table*/
st_cisp_ltmmeas_config_t ltmmeas_enable_config_table =
{
    .ltm_meas_enable              = true,
    .meas_wind_config.h_offset    = 0,
    .meas_wind_config.height      = INPUT_IMAGE_HEIGHT,
    .meas_wind_config.v_offset    = 0,
    .meas_wind_config.width       = INPUT_IMAGE_WIDTH,
    .weights.blue                 = 0x100,
    .weights.green                = 0x1FF,
    .weights.red                  = 0x100,
};
st_cisp_ltmmeas_config_t ltmmeas_disable_config_table =
{
    .ltm_meas_enable              = false,
    .meas_wind_config.h_offset    = 0,
    .meas_wind_config.height      = INPUT_IMAGE_HEIGHT,
    .meas_wind_config.v_offset    = 0,
    .meas_wind_config.width       = INPUT_IMAGE_WIDTH,
    .weights.blue                 = 0x100,
    .weights.green                = 0x1FF,
    .weights.red                  = 0x100,
};

/* FILT config table*/
st_cisp_filt_config_t filt_enable_config_table =
{
    .demosaic_params.demosaic_on               = true,
    .demosaic_params.threshold                 = 0,
    .filt_params.dynamic_params.filt_on        = true,
    .filt_params.dynamic_params.filt_mode      = CISP_FILT_MODE_STATIC,
    .filt_params.fac_mid                       = 0x14,
    .filt_params.dynamic_params.filt_lp_select = 6,
    .filt_params.dynamic_params.chroma_h_mode  = CISP_FILT_CHR_H_MODE_BYPASS,
    .filt_params.dynamic_params.chroma_v_mode  = CISP_FILT_CHR_V_MODE_BYPASS,
};
st_cisp_filt_config_t filt_disable_config_table =
{
    .demosaic_params.demosaic_on               = true,
    .demosaic_params.threshold                 = 0,
    .filt_params.dynamic_params.filt_on        = false,
    .filt_params.dynamic_params.filt_mode      = CISP_FILT_MODE_STATIC,
    .filt_params.fac_mid                       = 0x14,
    .filt_params.dynamic_params.filt_lp_select = 6,
    .filt_params.dynamic_params.chroma_h_mode  = CISP_FILT_CHR_H_MODE_BYPASS,
    .filt_params.dynamic_params.chroma_v_mode  = CISP_FILT_CHR_V_MODE_BYPASS,
};

/* HVOUTREGS config table*/
st_cisp_hvoutregs_config_t hvoutregs_config_table =
{
    .input_select       = CISP_HVOUTREGS_IN_SEL_MAIN,
    .subsampling_method = CISP_HVOUTREGS_422_CO_SITED,
};

/* MVOUTREGS config table*/
st_cisp_mvoutregs_config_t mvoutregs_config_table =
{
    .input_select       = CISP_MVOUTREGS_IN_SEL_DEBAYERED,
    .subsampling_method = CISP_MVOUTREGS_422_CO_SITED,
};
st_cisp_mvoutregs_config_t mvoutregs_hdr_config_table =
{
    .input_select       = CISP_MVOUTREGS_IN_SEL_DENOISED,
    .subsampling_method = CISP_MVOUTREGS_422_CO_SITED,
};

/* HIST256 config table*/
st_cisp_hist256_config_t hist256_enable_config_table =
{
    // 1: HIST256_LOG stage enabled. Log2 mapping active.
    .gamma_enable                                     = 0x00000000,
    // select 1 out of max. 7 input channels
    .hist256_shadow_config.channel                    = CISP_HIST256_CHANNEL_0,
    // Horizontal offset of first window in pixels.
    .hist256_shadow_config.meas_wind_config.h_offset  = 0,
    // Horizontal size in pixels of one sub-window.
    .hist256_shadow_config.meas_wind_config.height    = INPUT_IMAGE_HEIGHT,
    // Vertical offset of first window in pixels.
    .hist256_shadow_config.meas_wind_config.v_offset  = 0,
    // Vertical size in lines of one sub-window.
    .hist256_shadow_config.meas_wind_config.width     = INPUT_IMAGE_WIDTH,
    // 0: disable, no measurements, no end indication
    .hist256_shadow_config.measure_mode               = true,
    // sample offset will be subtracted from input sample prior to shift and histogram evaluation
    .hist256_shadow_config.sample_range.sample_offset = 0,
    // sample multiplication will be executed after offset subtraction and prior to histogram evaluation 2.8 fixed point presentation.
    .hist256_shadow_config.sample_range.sample_scale  = 0x00000100,
    // the dx[0] value is required to be zero. gamma curve sample point definition x-axis (input)
    .curve.dx[0]  = 0,
    .curve.dx[1]  = 0x0C,
    .curve.dx[2]  = 0x0C,
    .curve.dx[3]  = 0x0C,
    .curve.dx[4]  = 0x0C,
    .curve.dx[5]  = 0x0C,
    .curve.dx[6]  = 0x0C,
    .curve.dx[7]  = 0x0C,
    .curve.dx[8]  = 0x0C,
    .curve.dx[9]  = 0x0C,
    .curve.dx[10] = 0x0C,
    .curve.dx[11] = 0x0C,
    .curve.dx[12] = 0x0C,
    .curve.dx[13] = 0x0C,
    .curve.dx[14] = 0x0C,
    .curve.dx[15] = 0x0C,
    // gamma curve point definition y-axis (output)
    .curve.y[0]  = 0,
    .curve.y[1]  = 0,
    .curve.y[2]  = 0,
    .curve.y[3]  = 0,
    .curve.y[4]  = 0,
    .curve.y[5]  = 0,
    .curve.y[6]  = 0,
    .curve.y[7]  = 0,
    .curve.y[8]  = 0,
    .curve.y[9]  = 0,
    .curve.y[10] = 0,
    .curve.y[11] = 0,
    .curve.y[12] = 0,
    .curve.y[13] = 0,
    .curve.y[14] = 0,
    .curve.y[15] = 0,
};
st_cisp_hist256_config_t hist256_disable_config_table =
{
    // 1: HIST256_LOG stage enabled. Log2 mapping active.
    .gamma_enable                                     = 0x00000000,
    // select 1 out of max. 7 input channels
    .hist256_shadow_config.channel                    = CISP_HIST256_CHANNEL_0,
    // Horizontal offset of first window in pixels.
    .hist256_shadow_config.meas_wind_config.h_offset  = 0,
    // Horizontal size in pixels of one sub-window.
    .hist256_shadow_config.meas_wind_config.height    = INPUT_IMAGE_HEIGHT,
    // Vertical offset of first window in pixels.
    .hist256_shadow_config.meas_wind_config.v_offset  = 0,
    // Vertical size in lines of one sub-window.
    .hist256_shadow_config.meas_wind_config.width     = INPUT_IMAGE_WIDTH,
    // 0: disable, no measurements, no end indication
    .hist256_shadow_config.measure_mode               = false,
    // sample offset will be subtracted from input sample prior to shift and histogram evaluation
    .hist256_shadow_config.sample_range.sample_offset = 0,
    // sample multiplication will be executed after offset subtraction and prior to histogram evaluation 2.8 fixed point presentation.
    .hist256_shadow_config.sample_range.sample_scale  = 0x00000100,
    // the dx[0] value is required to be zero. gamma curve sample point definition x-axis (input)
    .curve.dx[0]  = 0,
    .curve.dx[1]  = 0x0C,
    .curve.dx[2]  = 0x0C,
    .curve.dx[3]  = 0x0C,
    .curve.dx[4]  = 0x0C,
    .curve.dx[5]  = 0x0C,
    .curve.dx[6]  = 0x0C,
    .curve.dx[7]  = 0x0C,
    .curve.dx[8]  = 0x0C,
    .curve.dx[9]  = 0x0C,
    .curve.dx[10] = 0x0C,
    .curve.dx[11] = 0x0C,
    .curve.dx[12] = 0x0C,
    .curve.dx[13] = 0x0C,
    .curve.dx[14] = 0x0C,
    .curve.dx[15] = 0x0C,
    // gamma curve point definition y-axis (output)
    .curve.y[0]  = 0,
    .curve.y[1]  = 0,
    .curve.y[2]  = 0,
    .curve.y[3]  = 0,
    .curve.y[4]  = 0,
    .curve.y[5]  = 0,
    .curve.y[6]  = 0,
    .curve.y[7]  = 0,
    .curve.y[8]  = 0,
    .curve.y[9]  = 0,
    .curve.y[10] = 0,
    .curve.y[11] = 0,
    .curve.y[12] = 0,
    .curve.y[13] = 0,
    .curve.y[14] = 0,
    .curve.y[15] = 0,
};

/* BLS config table*/
st_cisp_bls_config_t bls_pre1_enable_config_table =
{
    .bl_meas_mode                              = CISP_BLS_MEAS_MODE_WIND_1,
    .bls_module_on                             = 0,
    .fixed_bl_config.bl_a                      = 0,
    .fixed_bl_config.bl_b                      = 0,
    .fixed_bl_config.bl_c                      = 0,
    .fixed_bl_config.bl_d                      = 0,
    .meas_wind_config.meas_windows[0].h_offset = 0,
    .meas_wind_config.meas_windows[0].height   = 0,
    .meas_wind_config.meas_windows[0].v_offset = 0,
    .meas_wind_config.meas_windows[0].width    = 0,
    .meas_wind_config.meas_windows[1].h_offset = 0,
    .meas_wind_config.meas_windows[1].height   = 0,
    .meas_wind_config.meas_windows[1].v_offset = 0,
    .meas_wind_config.meas_windows[1].width    = 0,
    .meas_wind_config.number_of_windows        = 0,
};
st_cisp_bls_config_t bls_pre1_disable_config_table =
{
    .bl_meas_mode                              = CISP_BLS_MEAS_MODE_OFF,
    .bls_module_on                             = 0,
    .fixed_bl_config.bl_a                      = 0,
    .fixed_bl_config.bl_b                      = 0,
    .fixed_bl_config.bl_c                      = 0,
    .fixed_bl_config.bl_d                      = 0,
    .meas_wind_config.meas_windows[0].h_offset = 0,
    .meas_wind_config.meas_windows[0].height   = 0,
    .meas_wind_config.meas_windows[0].v_offset = 0,
    .meas_wind_config.meas_windows[0].width    = 0,
    .meas_wind_config.meas_windows[1].h_offset = 0,
    .meas_wind_config.meas_windows[1].height   = 0,
    .meas_wind_config.meas_windows[1].v_offset = 0,
    .meas_wind_config.meas_windows[1].width    = 0,
    .meas_wind_config.number_of_windows        = 0,
};
st_cisp_bls_config_t bls_pre2_disable_config_table =
{
    .bl_meas_mode                              = CISP_BLS_MEAS_MODE_OFF,
    .bls_module_on                             = 0,
    .fixed_bl_config.bl_a                      = 0,
    .fixed_bl_config.bl_b                      = 0,
    .fixed_bl_config.bl_c                      = 0,
    .fixed_bl_config.bl_d                      = 0,
    .meas_wind_config.meas_windows[0].h_offset = 0,
    .meas_wind_config.meas_windows[0].height   = 0,
    .meas_wind_config.meas_windows[0].v_offset = 0,
    .meas_wind_config.meas_windows[0].width    = 0,
    .meas_wind_config.meas_windows[1].h_offset = 0,
    .meas_wind_config.meas_windows[1].height   = 0,
    .meas_wind_config.meas_windows[1].v_offset = 0,
    .meas_wind_config.meas_windows[1].width    = 0,
    .meas_wind_config.number_of_windows        = 0,
};

/* DPF config table*/
st_cisp_dpf_config_t dpf_pre1_enable_config_table =
{
    .mode.b_filter_on            = false,
    .mode.dpf_enabled            = true,
    .mode.gb_filter_on           = false,
    .mode.gr_filter_on           = false,
    .mode.lsc_gain_process_on    = false,
    .mode.r_filter_on            = false,
    .mode.use_nf_gain            = false,
    .mode.use_nll_log_segment    = false,
    .mode.use_rb_filter_size9x9  = false,
    .nf_gain.blue                = 256,
    .nf_gain.green_blue          = 256,
    .nf_gain.green_red           = 256,
    .nf_gain.red                 = 256,
    .nll_coeff.coeff_g[0]        = 0x3FF,
    .nll_coeff.coeff_g[1]        = 0x3FF,
    .nll_coeff.coeff_g[2]        = 0x3FF,
    .nll_coeff.coeff_g[3]        = 0x3FF,
    .nll_coeff.coeff_g[4]        = 0x3FF,
    .nll_coeff.coeff_g[5]        = 0x3FF,
    .nll_coeff.coeff_g[6]        = 0x3FF,
    .nll_coeff.coeff_g[7]        = 0x3FF,
    .nll_coeff.coeff_g[8]        = 0x3FF,
    .nll_coeff.coeff_g[9]        = 0x3FF,
    .nll_coeff.coeff_g[10]       = 0x3FF,
    .nll_coeff.coeff_g[11]       = 0x3FF,
    .nll_coeff.coeff_g[12]       = 0x3FF,
    .nll_coeff.coeff_g[13]       = 0x3FF,
    .nll_coeff.coeff_g[14]       = 0x3FF,
    .nll_coeff.coeff_g[15]       = 0x3FF,
    .nll_coeff.coeff_g[16]       = 0x3FF,
    .nll_coeff.coeff_rb[0]       = 0x3FF,
    .nll_coeff.coeff_rb[1]       = 0x3FF,
    .nll_coeff.coeff_rb[2]       = 0x3FF,
    .nll_coeff.coeff_rb[3]       = 0x3FF,
    .nll_coeff.coeff_rb[4]       = 0x3FF,
    .nll_coeff.coeff_rb[5]       = 0x3FF,
    .nll_coeff.coeff_rb[6]       = 0x3FF,
    .nll_coeff.coeff_rb[7]       = 0x3FF,
    .nll_coeff.coeff_rb[8]       = 0x3FF,
    .nll_coeff.coeff_rb[9]       = 0x3FF,
    .nll_coeff.coeff_rb[10]      = 0x3FF,
    .nll_coeff.coeff_rb[11]      = 0x3FF,
    .nll_coeff.coeff_rb[12]      = 0x3FF,
    .nll_coeff.coeff_rb[13]      = 0x3FF,
    .nll_coeff.coeff_rb[14]      = 0x3FF,
    .nll_coeff.coeff_rb[15]      = 0x3FF,
    .nll_coeff.coeff_rb[16]      = 0x3FF,

    .spatial_strength.blue       = 64,
    .spatial_strength.green      = 64,
    .spatial_strength.red        = 64,
    .spatial_weight.g[0]         = 0x10,
    .spatial_weight.g[1]         = 0x10,
    .spatial_weight.g[2]         = 0x10,
    .spatial_weight.g[3]         = 0x10,
    .spatial_weight.g[4]         = 0x10,
    .spatial_weight.g[5]         = 0x10,
    .spatial_weight.rb[0]        = 0x10,
    .spatial_weight.rb[1]        = 0x10,
    .spatial_weight.rb[2]        = 0x10,
    .spatial_weight.rb[3]        = 0x10,
    .spatial_weight.rb[4]        = 0x10,
    .spatial_weight.rb[5]        = 0x10,
};
st_cisp_dpf_config_t dpf_pre1_disable_config_table =
{
    .mode.b_filter_on            = false,
    .mode.dpf_enabled            = false,
    .mode.gb_filter_on           = false,
    .mode.gr_filter_on           = false,
    .mode.lsc_gain_process_on    = false,
    .mode.r_filter_on            = false,
    .mode.use_nf_gain            = false,
    .mode.use_nll_log_segment    = false,
    .mode.use_rb_filter_size9x9  = false,
    .nf_gain.blue                = 256,
    .nf_gain.green_blue          = 256,
    .nf_gain.green_red           = 256,
    .nf_gain.red                 = 256,
    .nll_coeff.coeff_g[0]        = 0x3FF,
    .nll_coeff.coeff_g[1]        = 0x3FF,
    .nll_coeff.coeff_g[2]        = 0x3FF,
    .nll_coeff.coeff_g[3]        = 0x3FF,
    .nll_coeff.coeff_g[4]        = 0x3FF,
    .nll_coeff.coeff_g[5]        = 0x3FF,
    .nll_coeff.coeff_g[6]        = 0x3FF,
    .nll_coeff.coeff_g[7]        = 0x3FF,
    .nll_coeff.coeff_g[8]        = 0x3FF,
    .nll_coeff.coeff_g[9]        = 0x3FF,
    .nll_coeff.coeff_g[10]       = 0x3FF,
    .nll_coeff.coeff_g[11]       = 0x3FF,
    .nll_coeff.coeff_g[12]       = 0x3FF,
    .nll_coeff.coeff_g[13]       = 0x3FF,
    .nll_coeff.coeff_g[14]       = 0x3FF,
    .nll_coeff.coeff_g[15]       = 0x3FF,
    .nll_coeff.coeff_g[16]       = 0x3FF,
    .nll_coeff.coeff_rb[0]       = 0x3FF,
    .nll_coeff.coeff_rb[1]       = 0x3FF,
    .nll_coeff.coeff_rb[2]       = 0x3FF,
    .nll_coeff.coeff_rb[3]       = 0x3FF,
    .nll_coeff.coeff_rb[4]       = 0x3FF,
    .nll_coeff.coeff_rb[5]       = 0x3FF,
    .nll_coeff.coeff_rb[6]       = 0x3FF,
    .nll_coeff.coeff_rb[7]       = 0x3FF,
    .nll_coeff.coeff_rb[8]       = 0x3FF,
    .nll_coeff.coeff_rb[9]       = 0x3FF,
    .nll_coeff.coeff_rb[10]      = 0x3FF,
    .nll_coeff.coeff_rb[11]      = 0x3FF,
    .nll_coeff.coeff_rb[12]      = 0x3FF,
    .nll_coeff.coeff_rb[13]      = 0x3FF,
    .nll_coeff.coeff_rb[14]      = 0x3FF,
    .nll_coeff.coeff_rb[15]      = 0x3FF,
    .nll_coeff.coeff_rb[16]      = 0x3FF,

    .spatial_strength.blue       = 64,
    .spatial_strength.green      = 64,
    .spatial_strength.red        = 64,
    .spatial_weight.g[0]         = 0x10,
    .spatial_weight.g[1]         = 0x10,
    .spatial_weight.g[2]         = 0x10,
    .spatial_weight.g[3]         = 0x10,
    .spatial_weight.g[4]         = 0x10,
    .spatial_weight.g[5]         = 0x10,
    .spatial_weight.rb[0]        = 0x10,
    .spatial_weight.rb[1]        = 0x10,
    .spatial_weight.rb[2]        = 0x10,
    .spatial_weight.rb[3]        = 0x10,
    .spatial_weight.rb[4]        = 0x10,
    .spatial_weight.rb[5]        = 0x10,
};
st_cisp_dpf_config_t dpf_pre2_disable_config_table =
{
    .mode.b_filter_on            = true,
    .mode.dpf_enabled            = false,
    .mode.gb_filter_on           = true,
    .mode.gr_filter_on           = true,
    .mode.lsc_gain_process_on    = false,
    .mode.r_filter_on            = true,
    .mode.use_nf_gain            = false,
    .mode.use_nll_log_segment    = false,
    .mode.use_rb_filter_size9x9  = false,
    .nf_gain.blue                = 256,
    .nf_gain.green_blue          = 256,
    .nf_gain.green_red           = 256,
    .nf_gain.red                 = 256,
    .nll_coeff.coeff_g[0]        = 0x3FF,
    .nll_coeff.coeff_g[1]        = 0x3FF,
    .nll_coeff.coeff_g[2]        = 0x3FF,
    .nll_coeff.coeff_g[3]        = 0x3FF,
    .nll_coeff.coeff_g[4]        = 0x3FF,
    .nll_coeff.coeff_g[5]        = 0x3FF,
    .nll_coeff.coeff_g[6]        = 0x3FF,
    .nll_coeff.coeff_g[7]        = 0x3FF,
    .nll_coeff.coeff_g[8]        = 0x3FF,
    .nll_coeff.coeff_g[9]        = 0x3FF,
    .nll_coeff.coeff_g[10]       = 0x3FF,
    .nll_coeff.coeff_g[11]       = 0x3FF,
    .nll_coeff.coeff_g[12]       = 0x3FF,
    .nll_coeff.coeff_g[13]       = 0x3FF,
    .nll_coeff.coeff_g[14]       = 0x3FF,
    .nll_coeff.coeff_g[15]       = 0x3FF,
    .nll_coeff.coeff_g[16]       = 0x3FF,
    .nll_coeff.coeff_rb[0]       = 0x3FF,
    .nll_coeff.coeff_rb[1]       = 0x3FF,
    .nll_coeff.coeff_rb[2]       = 0x3FF,
    .nll_coeff.coeff_rb[3]       = 0x3FF,
    .nll_coeff.coeff_rb[4]       = 0x3FF,
    .nll_coeff.coeff_rb[5]       = 0x3FF,
    .nll_coeff.coeff_rb[6]       = 0x3FF,
    .nll_coeff.coeff_rb[7]       = 0x3FF,
    .nll_coeff.coeff_rb[8]       = 0x3FF,
    .nll_coeff.coeff_rb[9]       = 0x3FF,
    .nll_coeff.coeff_rb[10]      = 0x3FF,
    .nll_coeff.coeff_rb[11]      = 0x3FF,
    .nll_coeff.coeff_rb[12]      = 0x3FF,
    .nll_coeff.coeff_rb[13]      = 0x3FF,
    .nll_coeff.coeff_rb[14]      = 0x3FF,
    .nll_coeff.coeff_rb[15]      = 0x3FF,
    .nll_coeff.coeff_rb[16]      = 0x3FF,

    .spatial_strength.blue       = 64,
    .spatial_strength.green      = 64,
    .spatial_strength.red        = 64,
    .spatial_weight.g[0]         = 0x10,
    .spatial_weight.g[1]         = 0x10,
    .spatial_weight.g[2]         = 0x10,
    .spatial_weight.g[3]         = 0x10,
    .spatial_weight.g[4]         = 0x10,
    .spatial_weight.g[5]         = 0x10,
    .spatial_weight.rb[0]        = 0x10,
    .spatial_weight.rb[1]        = 0x10,
    .spatial_weight.rb[2]        = 0x10,
    .spatial_weight.rb[3]        = 0x10,
    .spatial_weight.rb[4]        = 0x10,
    .spatial_weight.rb[5]        = 0x10,
};

/* LSC config table*/
st_cisp_lsc_config_t lsc_pre1_enable_config_table =
{
    .lsc_on                       = true,
    .next_active_table            = CISP_LSC_ACTIVE_TABLE_1,
};
st_cisp_lsc_config_t lsc_pre1_disable_config_table =
{
    .lsc_on                       = false,
    .next_active_table            = CISP_LSC_ACTIVE_TABLE_1,
};
st_cisp_lsc_config_t lsc_pre2_disable_config_table =
{
    .lsc_on                       = false,
    .next_active_table            = CISP_LSC_ACTIVE_TABLE_0,
};

/* IS config table*/
st_cisp_is_config_t is_hvout_config_table =
{
    .out_cropping_window.h_offset = 0,
    .out_cropping_window.height   = INPUT_IMAGE_HEIGHT,
    .out_cropping_window.v_offset = 0,
    .out_cropping_window.width    = INPUT_IMAGE_WIDTH,
};
st_cisp_is_config_t is_mvout_config_table =
{
    .out_cropping_window.h_offset = 0,
    .out_cropping_window.height   = INPUT_IMAGE_HEIGHT,
    .out_cropping_window.v_offset = 0,
    .out_cropping_window.width    = INPUT_IMAGE_WIDTH,

};

/* CCOR config table*/
st_cisp_ccor_config_t ccor_hvout_config_table =
{
    .chroma_range     = CISP_CCOR_RANGE_FULL,   // RPP_OUT_CCOR_RANGE
    .luma_range       = CISP_CCOR_RANGE_FULL,   // RPP_OUT_CCOR_RANGE

    .profile.coeff[0] = 871,                    // RPP_OUT_CCOR_COEFF__0
    .profile.coeff[1] = 2929,                   // RPP_OUT_CCOR_COEFF__1
    .profile.coeff[2] = 296,                    // RPP_OUT_CCOR_COEFF__2
    .profile.coeff[3] = -469,                   // RPP_OUT_CCOR_COEFF__3
    .profile.coeff[4] = -1579,                  // RPP_OUT_CCOR_COEFF__4
    .profile.coeff[5] = 2048,                   // RPP_OUT_CCOR_COEFF__5
    .profile.coeff[6] = 2048,                   // RPP_OUT_CCOR_COEFF__6
    .profile.coeff[7] = -1860,                  // RPP_OUT_CCOR_COEFF__7
    .profile.coeff[8] = -188,                   // RPP_OUT_CCOR_COEFF__8

    .profile.off[0]   = 0,                      // RPP_OUT_CCOR_OFFSET_R
    .profile.off[1]   = 2048,                   // RPP_OUT_CCOR_OFFSET_G
    .profile.off[2]   = 2048,                   // RPP_OUT_CCOR_OFFSET_B
};
st_cisp_ccor_config_t ccor_mvout_config_table =
{
    .chroma_range     = CISP_CCOR_RANGE_FULL,   // RPP_MV_OUT_CCOR_RANGE
    .luma_range       = CISP_CCOR_RANGE_FULL,   // RPP_MV_OUT_CCOR_RANGE

    .profile.coeff[0] = 4096,                   // RPP_MV_OUT_CCOR_COEFF__0
    .profile.coeff[1] = 0,                      // RPP_MV_OUT_CCOR_COEFF__1
    .profile.coeff[2] = 0,                      // RPP_MV_OUT_CCOR_COEFF__2
    .profile.coeff[3] = 0,                      // RPP_MV_OUT_CCOR_COEFF__3
    .profile.coeff[4] = 4096,                   // RPP_MV_OUT_CCOR_COEFF__4
    .profile.coeff[5] = 0,                      // RPP_MV_OUT_CCOR_COEFF__5
    .profile.coeff[6] = 0,                      // RPP_MV_OUT_CCOR_COEFF__6
    .profile.coeff[7] = 0,                      // RPP_MV_OUT_CCOR_COEFF__7
    .profile.coeff[8] = 4096,                   // RPP_MV_OUT_CCOR_COEFF__8

    .profile.off[0]   = 0,                      // RPP_MV_OUT_CCOR_OFFSET_R
    .profile.off[1]   = 0,                      // RPP_MV_OUT_CCOR_OFFSET_G
    .profile.off[2]   = 0,                      // RPP_MV_OUT_CCOR_OFFSET_B
};
st_cisp_ccor_config_t ccor_post_config_table =
{
    .chroma_range     = CISP_CCOR_RANGE_FULL,   // RPP_MAIN_POST_CCOR_RANGE
    .luma_range       = CISP_CCOR_RANGE_FULL,   // RPP_MAIN_POST_CCOR_RANGE

    .profile.coeff[0] = 6653,                   // RPP_MAIN_POST_CCOR_COEFF__0
    .profile.coeff[1] = -2977,                  // RPP_MAIN_POST_CCOR_COEFF__1
    .profile.coeff[2] = 420,                    // RPP_MAIN_POST_CCOR_COEFF__2
    .profile.coeff[3] = -902,                   // RPP_MAIN_POST_CCOR_COEFF__3
    .profile.coeff[4] = 5613,                   // RPP_MAIN_POST_CCOR_COEFF__4
    .profile.coeff[5] = -615,                   // RPP_MAIN_POST_CCOR_COEFF__5
    .profile.coeff[6] = 465,                    // RPP_MAIN_POST_CCOR_COEFF__6
    .profile.coeff[7] = -2950,                  // RPP_MAIN_POST_CCOR_COEFF__7
    .profile.coeff[8] = 6581,                   // RPP_MAIN_POST_CCOR_COEFF__8

    .profile.off[0]   = 0,                      // RPP_MAIN_POST_CCOR_OFFSET_R
    .profile.off[1]   = 0,                      // RPP_MAIN_POST_CCOR_OFFSET_G
    .profile.off[2]   = 0,                      // RPP_MAIN_POST_CCOR_OFFSET_B
};

/* RGBDENOISE config table*/
st_cisp_rgbdenoise_config_t rgbdenoise_enagle_config_table =
{
    .rgbdenoise_enable = true,  // RPP_MAIN_POST_RGBDENOISE_HW_BYPASS
    .params.shift_c    = 0,     // RPP_MAIN_POST_RGBDENOISE_SPNR_CTRL
    .params.shift_y    = 0,     // RPP_MAIN_POST_RGBDENOISE_SPNR_CTRL

    .params.if_cc[0]  = 15,     // RPP_MAIN_POST_RGBDENOISE_SPNR_CHROMA_IF_COEF_00_07
    .params.if_cy[0]  = 15,     // RPP_MAIN_POST_RGBDENOISE_SPNR_LUMA_IF_COEF_00_07

    .params.sf_cc[0]  = 15,     // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cc[1]  = 0,      // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cc[2]  = 0,      // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cc[3]  = 0,      // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cy[0]  = 15,     // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cy[1]  = 0,      // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cy[2]  = 0,      // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cy[3]  = 0,      // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3

    .profile.coeff[0] = 871,    // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_0
    .profile.coeff[1] = 2929,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_1
    .profile.coeff[2] = 296,    // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_2
    .profile.coeff[3] = -469,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_3
    .profile.coeff[4] = -1579,  // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_4
    .profile.coeff[5] = 2048,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_5
    .profile.coeff[6] = 2048,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_6
    .profile.coeff[7] = -1860,  // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_7
    .profile.coeff[8] = -188,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_8

    .profile.off[0]   = 0,      // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_OFFSET_R
    .profile.off[1]   = 2048,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_OFFSET_G
    .profile.off[2]   = 2048,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_OFFSET_B
};
st_cisp_rgbdenoise_config_t rgbdenoise_disable_config_table =
{
    .rgbdenoise_enable = false, // RPP_MAIN_POST_RGBDENOISE_HW_BYPASS
    .params.shift_c    = 0,     // RPP_MAIN_POST_RGBDENOISE_SPNR_CTRL
    .params.shift_y    = 0,     // RPP_MAIN_POST_RGBDENOISE_SPNR_CTRL

    .params.if_cc[0]  = 15,     // RPP_MAIN_POST_RGBDENOISE_SPNR_CHROMA_IF_COEF_00_07
    .params.if_cy[0]  = 15,     // RPP_MAIN_POST_RGBDENOISE_SPNR_LUMA_IF_COEF_00_07

    .params.sf_cc[0]  = 15,     // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cc[1]  = 0,      // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cc[2]  = 0,      // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cc[3]  = 0,      // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cy[0]  = 15,     // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cy[1]  = 0,      // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cy[2]  = 0,      // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3
    .params.sf_cy[3]  = 0,      // RPP_MAIN_POST_RGBDENOISE_SPNR_SPATIAL_COEF_0_3

    .profile.coeff[0] = 871,    // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_0
    .profile.coeff[1] = 2929,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_1
    .profile.coeff[2] = 296,    // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_2
    .profile.coeff[3] = -469,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_3
    .profile.coeff[4] = -1579,  // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_4
    .profile.coeff[5] = 2048,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_5
    .profile.coeff[6] = 2048,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_6
    .profile.coeff[7] = -1860,  // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_7
    .profile.coeff[8] = -188,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_COEFF_8

    .profile.off[0]   = 0,      // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_OFFSET_R
    .profile.off[1]   = 2048,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_OFFSET_G
    .profile.off[2]   = 2048,   // RPP_MAIN_POST_RGBDENOISE_RGB2YUV_CCOR_OFFSET_B
};

/* DPCC config table*/
st_cisp_dpcc_config_t dpcc_pre1_enable_config_table =
{
    .dpcc_on                                         = true,    // RPP_MAIN_PRE1_DPCC_MODE
    .use_hard_method_set                             = false,   // RPP_MAIN_PRE1_DPCC_SET_USE

    .bpt_config.correct_config.correct_on            = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.correct_config.green                 = CISP_DPCC_INTERPOLATION_MEDIAN_4, // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.correct_config.red_blue              = CISP_DPCC_INTERPOLATION_MEDIAN_4, // RPP_MAIN_PRE1_DPCC_BPT_CTRL

    .bpt_config.detect_config.detect_on              = true,    // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_hard_method_set = true,    // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_method_set_1    = true,    // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_method_set_2    = true,    // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_method_set_3    = true,    // RPP_MAIN_PRE1_DPCC_BPT_CTRL

    .method_sets[0].green.pg_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.lc_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.ro_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.rnd_enabled          = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.rg_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.pg_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.lc_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.ro_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.rnd_enabled       = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.rg_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].set_used                   = true,          // RPP_MAIN_PRE1_DPCC_SET_USE
    .method_sets[0].green.lc_threshold         = 0x08,          // RPP_MAIN_PRE1_DPCC_LINE_THRESH_1
    .method_sets[0].red_blue.lc_threshold      = 0x08,          // RPP_MAIN_PRE1_DPCC_LINE_THRESH_1
    .method_sets[0].green.rnd_threshold        = 0x0A,          // RPP_MAIN_PRE1_DPCC_RND_THRESH_1
    .method_sets[0].red_blue.rnd_threshold     = 0x0A,          // RPP_MAIN_PRE1_DPCC_RND_THRESH_1
    .method_sets[0].green.ro_limit             = 0x01,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[0].red_blue.ro_limit          = 0x01,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[0].green.rnd_offset           = 0x02,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[0].red_blue.rnd_offset        = 0x02,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[0].green.lc_factor            = 0x04,          // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_1
    .method_sets[0].red_blue.lc_factor         = 0x04,          // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_1
    .method_sets[0].green.pg_factor            = 0x08,          // RPP_MAIN_PRE1_DPCC_PG_FAC_1
    .method_sets[0].red_blue.pg_factor         = 0x08,          // RPP_MAIN_PRE1_DPCC_PG_FAC_1
    .method_sets[0].green.rg_factor            = 0x20,          // RPP_MAIN_PRE1_DPCC_RG_FAC_1
    .method_sets[0].red_blue.rg_factor         = 0x20,          // RPP_MAIN_PRE1_DPCC_RG_FAC_1

    .method_sets[1].green.pg_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.lc_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.ro_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.rnd_enabled          = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.rg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.pg_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.lc_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.ro_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.rnd_enabled       = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.rg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].set_used                   = true,          // RPP_MAIN_PRE1_DPCC_SET_USE
    .method_sets[1].green.lc_threshold         = 0x18,          // RPP_MAIN_PRE1_DPCC_LINE_THRESH_2
    .method_sets[1].red_blue.lc_threshold      = 0x20,          // RPP_MAIN_PRE1_DPCC_LINE_THRESH_2
    .method_sets[1].green.rnd_threshold        = 0x08,          // RPP_MAIN_PRE1_DPCC_RND_THRESH_2
    .method_sets[1].red_blue.rnd_threshold     = 0x08,          // RPP_MAIN_PRE1_DPCC_RND_THRESH_2
    .method_sets[1].green.ro_limit             = 0x02,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[1].red_blue.ro_limit          = 0x02,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[1].green.rnd_offset           = 0x02,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[1].red_blue.rnd_offset        = 0x02,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[1].green.lc_factor            = 0x10,          // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_2
    .method_sets[1].red_blue.lc_factor         = 0x18,          // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_2
    .method_sets[1].green.pg_factor            = 0x06,          // RPP_MAIN_PRE1_DPCC_PG_FAC_2
    .method_sets[1].red_blue.pg_factor         = 0x08,          // RPP_MAIN_PRE1_DPCC_PG_FAC_2
    .method_sets[1].green.rg_factor            = 0x08,          // RPP_MAIN_PRE1_DPCC_RG_FAC_2
    .method_sets[1].red_blue.rg_factor         = 0x08,          // RPP_MAIN_PRE1_DPCC_RG_FAC_2

    .method_sets[2].green.pg_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.lc_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.ro_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.rnd_enabled          = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.rg_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.pg_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.lc_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.ro_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.rnd_enabled       = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.rg_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].set_used                   = false,         // RPP_MAIN_PRE1_DPCC_SET_USE
    .method_sets[2].green.lc_threshold         = 0x20,          // RPP_MAIN_PRE1_DPCC_LINE_THRESH_3
    .method_sets[2].red_blue.lc_threshold      = 0x20,          // RPP_MAIN_PRE1_DPCC_LINE_THRESH_3
    .method_sets[2].green.rnd_threshold        = 0x06,          // RPP_MAIN_PRE1_DPCC_RND_THRESH_3
    .method_sets[2].red_blue.rnd_threshold     = 0x08,          // RPP_MAIN_PRE1_DPCC_RND_THRESH_3
    .method_sets[2].green.ro_limit             = 0x01,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[2].red_blue.ro_limit          = 0x02,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[2].green.rnd_offset           = 0x02,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[2].red_blue.rnd_offset        = 0x02,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[2].green.lc_factor            = 0x04,          // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_3
    .method_sets[2].red_blue.lc_factor         = 0x04,          // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_3
    .method_sets[2].green.pg_factor            = 0x0A,          // RPP_MAIN_PRE1_DPCC_PG_FAC_3
    .method_sets[2].red_blue.pg_factor         = 0x0A,          // RPP_MAIN_PRE1_DPCC_PG_FAC_3
    .method_sets[2].green.rg_factor            = 0x04,          // RPP_MAIN_PRE1_DPCC_RG_FAC_3
    .method_sets[2].red_blue.rg_factor         = 0x04,          // RPP_MAIN_PRE1_DPCC_RG_FAC_3

    .output_mode.green    = CISP_DPCC_INTERPOLATION_MEDIAN_5,   // RPP_MAIN_PRE1_DPCC_OUTPUT_MODE
    .output_mode.red_blue = CISP_DPCC_INTERPOLATION_MEDIAN_5,   // RPP_MAIN_PRE1_DPCC_OUTPUT_MODE
};
st_cisp_dpcc_config_t dpcc_pre1_disable_config_table =
{
    .dpcc_on                                         = false,   // RPP_MAIN_PRE1_DPCC_MODE
    .use_hard_method_set                             = false,   // RPP_MAIN_PRE1_DPCC_SET_USE

    .bpt_config.correct_config.correct_on            = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.correct_config.green                 = CISP_DPCC_INTERPOLATION_MEDIAN_4, // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.correct_config.red_blue              = CISP_DPCC_INTERPOLATION_MEDIAN_4, // RPP_MAIN_PRE1_DPCC_BPT_CTRL

    .bpt_config.detect_config.detect_on              = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_hard_method_set = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_method_set_1    = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_method_set_2    = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_method_set_3    = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL

    .method_sets[0].green.pg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.lc_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.ro_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.rnd_enabled          = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.rg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.pg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.lc_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.ro_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.rnd_enabled       = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.rg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].set_used                   = false,         // RPP_MAIN_PRE1_DPCC_SET_USE
    .method_sets[0].green.lc_threshold         = 0,             // RPP_MAIN_PRE1_DPCC_LINE_THRESH_1
    .method_sets[0].red_blue.lc_threshold      = 0,             // RPP_MAIN_PRE1_DPCC_LINE_THRESH_1
    .method_sets[0].green.rnd_threshold        = 0,             // RPP_MAIN_PRE1_DPCC_RND_THRESH_1
    .method_sets[0].red_blue.rnd_threshold     = 0,             // RPP_MAIN_PRE1_DPCC_RND_THRESH_1
    .method_sets[0].green.ro_limit             = 0x03,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[0].red_blue.ro_limit          = 0x03,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[0].green.rnd_offset           = 0x03,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[0].red_blue.rnd_offset        = 0x03,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[0].green.lc_factor            = 0,             // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_1
    .method_sets[0].red_blue.lc_factor         = 0,             // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_1
    .method_sets[0].green.pg_factor            = 0,             // RPP_MAIN_PRE1_DPCC_PG_FAC_1
    .method_sets[0].red_blue.pg_factor         = 0,             // RPP_MAIN_PRE1_DPCC_PG_FAC_1
    .method_sets[0].green.rg_factor            = 0,             // RPP_MAIN_PRE1_DPCC_RG_FAC_1
    .method_sets[0].red_blue.rg_factor         = 0,             // RPP_MAIN_PRE1_DPCC_RG_FAC_1

    .method_sets[1].green.pg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.lc_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.ro_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.rnd_enabled          = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.rg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.pg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.lc_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.ro_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.rnd_enabled       = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.rg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].set_used                   = false,         // RPP_MAIN_PRE1_DPCC_SET_USE
    .method_sets[1].green.lc_threshold         = 0,             // RPP_MAIN_PRE1_DPCC_LINE_THRESH_2
    .method_sets[1].red_blue.lc_threshold      = 0,             // RPP_MAIN_PRE1_DPCC_LINE_THRESH_2
    .method_sets[1].green.rnd_threshold        = 0,             // RPP_MAIN_PRE1_DPCC_RND_THRESH_2
    .method_sets[1].red_blue.rnd_threshold     = 0,             // RPP_MAIN_PRE1_DPCC_RND_THRESH_2
    .method_sets[1].green.ro_limit             = 0x03,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[1].red_blue.ro_limit          = 0x03,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[1].green.rnd_offset           = 0x03,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[1].red_blue.rnd_offset        = 0x03,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[1].green.lc_factor            = 0,             // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_2
    .method_sets[1].red_blue.lc_factor         = 0,             // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_2
    .method_sets[1].green.pg_factor            = 0,             // RPP_MAIN_PRE1_DPCC_PG_FAC_2
    .method_sets[1].red_blue.pg_factor         = 0,             // RPP_MAIN_PRE1_DPCC_PG_FAC_2
    .method_sets[1].green.rg_factor            = 0,             // RPP_MAIN_PRE1_DPCC_RG_FAC_2
    .method_sets[1].red_blue.rg_factor         = 0,             // RPP_MAIN_PRE1_DPCC_RG_FAC_2

    .method_sets[2].green.pg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.lc_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.ro_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.rnd_enabled          = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.rg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.pg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.lc_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.ro_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.rnd_enabled       = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.rg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].set_used                   = false,         // RPP_MAIN_PRE1_DPCC_SET_USE
    .method_sets[2].green.lc_threshold         = 0,             // RPP_MAIN_PRE1_DPCC_LINE_THRESH_3
    .method_sets[2].red_blue.lc_threshold      = 0,             // RPP_MAIN_PRE1_DPCC_LINE_THRESH_3
    .method_sets[2].green.rnd_threshold        = 0,             // RPP_MAIN_PRE1_DPCC_RND_THRESH_3
    .method_sets[2].red_blue.rnd_threshold     = 0,             // RPP_MAIN_PRE1_DPCC_RND_THRESH_3
    .method_sets[2].green.ro_limit             = 0x03,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[2].red_blue.ro_limit          = 0x03,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[2].green.rnd_offset           = 0x03,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[2].red_blue.rnd_offset        = 0x03,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[2].green.lc_factor            = 0,             // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_3
    .method_sets[2].red_blue.lc_factor         = 0,             // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_3
    .method_sets[2].green.pg_factor            = 0,             // RPP_MAIN_PRE1_DPCC_PG_FAC_3
    .method_sets[2].red_blue.pg_factor         = 0,             // RPP_MAIN_PRE1_DPCC_PG_FAC_3
    .method_sets[2].green.rg_factor            = 0,             // RPP_MAIN_PRE1_DPCC_RG_FAC_3
    .method_sets[2].red_blue.rg_factor         = 0,             // RPP_MAIN_PRE1_DPCC_RG_FAC_3

    .output_mode.green    = CISP_DPCC_INTERPOLATION_MEDIAN_4,   // RPP_MAIN_PRE1_DPCC_OUTPUT_MODE
    .output_mode.red_blue = CISP_DPCC_INTERPOLATION_MEDIAN_4,   // RPP_MAIN_PRE1_DPCC_OUTPUT_MODE
};
st_cisp_dpcc_config_t dpcc_pre2_enable_config_table =
{
    .dpcc_on                                         = true,    // RPP_MAIN_PRE1_DPCC_MODE
    .use_hard_method_set                             = false,   // RPP_MAIN_PRE1_DPCC_SET_USE

    .bpt_config.correct_config.correct_on            = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.correct_config.green                 = CISP_DPCC_INTERPOLATION_MEDIAN_4, // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.correct_config.red_blue              = CISP_DPCC_INTERPOLATION_MEDIAN_4, // RPP_MAIN_PRE1_DPCC_BPT_CTRL

    .bpt_config.detect_config.detect_on              = true,    // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_hard_method_set = true,    // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_method_set_1    = true,    // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_method_set_2    = true,    // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_method_set_3    = true,    // RPP_MAIN_PRE1_DPCC_BPT_CTRL

    .method_sets[0].green.pg_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.lc_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.ro_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.rnd_enabled          = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.rg_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.pg_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.lc_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.ro_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.rnd_enabled       = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.rg_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].set_used                   = true,          // RPP_MAIN_PRE1_DPCC_SET_USE
    .method_sets[0].green.lc_threshold         = 0x08,          // RPP_MAIN_PRE1_DPCC_LINE_THRESH_1
    .method_sets[0].red_blue.lc_threshold      = 0x08,          // RPP_MAIN_PRE1_DPCC_LINE_THRESH_1
    .method_sets[0].green.rnd_threshold        = 0x0A,          // RPP_MAIN_PRE1_DPCC_RND_THRESH_1
    .method_sets[0].red_blue.rnd_threshold     = 0x0A,          // RPP_MAIN_PRE1_DPCC_RND_THRESH_1
    .method_sets[0].green.ro_limit             = 0x01,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[0].red_blue.ro_limit          = 0x01,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[0].green.rnd_offset           = 0x02,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[0].red_blue.rnd_offset        = 0x02,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[0].green.lc_factor            = 0x04,          // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_1
    .method_sets[0].red_blue.lc_factor         = 0x04,          // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_1
    .method_sets[0].green.pg_factor            = 0x08,          // RPP_MAIN_PRE1_DPCC_PG_FAC_1
    .method_sets[0].red_blue.pg_factor         = 0x08,          // RPP_MAIN_PRE1_DPCC_PG_FAC_1
    .method_sets[0].green.rg_factor            = 0x20,          // RPP_MAIN_PRE1_DPCC_RG_FAC_1
    .method_sets[0].red_blue.rg_factor         = 0x20,          // RPP_MAIN_PRE1_DPCC_RG_FAC_1

    .method_sets[1].green.pg_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.lc_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.ro_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.rnd_enabled          = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.rg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.pg_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.lc_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.ro_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.rnd_enabled       = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.rg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].set_used                   = true,          // RPP_MAIN_PRE1_DPCC_SET_USE
    .method_sets[1].green.lc_threshold         = 0x18,          // RPP_MAIN_PRE1_DPCC_LINE_THRESH_2
    .method_sets[1].red_blue.lc_threshold      = 0x20,          // RPP_MAIN_PRE1_DPCC_LINE_THRESH_2
    .method_sets[1].green.rnd_threshold        = 0x08,          // RPP_MAIN_PRE1_DPCC_RND_THRESH_2
    .method_sets[1].red_blue.rnd_threshold     = 0x08,          // RPP_MAIN_PRE1_DPCC_RND_THRESH_2
    .method_sets[1].green.ro_limit             = 0x02,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[1].red_blue.ro_limit          = 0x02,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[1].green.rnd_offset           = 0x02,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[1].red_blue.rnd_offset        = 0x02,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[1].green.lc_factor            = 0x10,          // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_2
    .method_sets[1].red_blue.lc_factor         = 0x18,          // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_2
    .method_sets[1].green.pg_factor            = 0x06,          // RPP_MAIN_PRE1_DPCC_PG_FAC_2
    .method_sets[1].red_blue.pg_factor         = 0x08,          // RPP_MAIN_PRE1_DPCC_PG_FAC_2
    .method_sets[1].green.rg_factor            = 0x08,          // RPP_MAIN_PRE1_DPCC_RG_FAC_2
    .method_sets[1].red_blue.rg_factor         = 0x08,          // RPP_MAIN_PRE1_DPCC_RG_FAC_2

    .method_sets[2].green.pg_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.lc_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.ro_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.rnd_enabled          = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.rg_enabled           = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.pg_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.lc_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.ro_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.rnd_enabled       = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.rg_enabled        = true,          // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].set_used                   = false,         // RPP_MAIN_PRE1_DPCC_SET_USE
    .method_sets[2].green.lc_threshold         = 0x20,          // RPP_MAIN_PRE1_DPCC_LINE_THRESH_3
    .method_sets[2].red_blue.lc_threshold      = 0x20,          // RPP_MAIN_PRE1_DPCC_LINE_THRESH_3
    .method_sets[2].green.rnd_threshold        = 0x06,          // RPP_MAIN_PRE1_DPCC_RND_THRESH_3
    .method_sets[2].red_blue.rnd_threshold     = 0x08,          // RPP_MAIN_PRE1_DPCC_RND_THRESH_3
    .method_sets[2].green.ro_limit             = 0x01,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[2].red_blue.ro_limit          = 0x02,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[2].green.rnd_offset           = 0x02,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[2].red_blue.rnd_offset        = 0x02,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[2].green.lc_factor            = 0x04,          // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_3
    .method_sets[2].red_blue.lc_factor         = 0x04,          // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_3
    .method_sets[2].green.pg_factor            = 0x0A,          // RPP_MAIN_PRE1_DPCC_PG_FAC_3
    .method_sets[2].red_blue.pg_factor         = 0x0A,          // RPP_MAIN_PRE1_DPCC_PG_FAC_3
    .method_sets[2].green.rg_factor            = 0x04,          // RPP_MAIN_PRE1_DPCC_RG_FAC_3
    .method_sets[2].red_blue.rg_factor         = 0x04,          // RPP_MAIN_PRE1_DPCC_RG_FAC_3

    .output_mode.green    = CISP_DPCC_INTERPOLATION_MEDIAN_5,   // RPP_MAIN_PRE1_DPCC_OUTPUT_MODE
    .output_mode.red_blue = CISP_DPCC_INTERPOLATION_MEDIAN_5,   // RPP_MAIN_PRE1_DPCC_OUTPUT_MODE
};
st_cisp_dpcc_config_t dpcc_pre2_disable_config_table =
{
    .dpcc_on                                         = false,   // RPP_MAIN_PRE1_DPCC_MODE
    .use_hard_method_set                             = false,   // RPP_MAIN_PRE1_DPCC_SET_USE

    .bpt_config.correct_config.correct_on            = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.correct_config.green                 = CISP_DPCC_INTERPOLATION_MEDIAN_4, // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.correct_config.red_blue              = CISP_DPCC_INTERPOLATION_MEDIAN_4, // RPP_MAIN_PRE1_DPCC_BPT_CTRL

    .bpt_config.detect_config.detect_on              = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_hard_method_set = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_method_set_1    = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_method_set_2    = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL
    .bpt_config.detect_config.report_method_set_3    = false,   // RPP_MAIN_PRE1_DPCC_BPT_CTRL

    .method_sets[0].green.pg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.lc_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.ro_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.rnd_enabled          = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].green.rg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.pg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.lc_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.ro_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.rnd_enabled       = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].red_blue.rg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_1
    .method_sets[0].set_used                   = false,         // RPP_MAIN_PRE1_DPCC_SET_USE
    .method_sets[0].green.lc_threshold         = 0,             // RPP_MAIN_PRE1_DPCC_LINE_THRESH_1
    .method_sets[0].red_blue.lc_threshold      = 0,             // RPP_MAIN_PRE1_DPCC_LINE_THRESH_1
    .method_sets[0].green.rnd_threshold        = 0,             // RPP_MAIN_PRE1_DPCC_RND_THRESH_1
    .method_sets[0].red_blue.rnd_threshold     = 0,             // RPP_MAIN_PRE1_DPCC_RND_THRESH_1
    .method_sets[0].green.ro_limit             = 0x03,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[0].red_blue.ro_limit          = 0x03,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[0].green.rnd_offset           = 0x03,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[0].red_blue.rnd_offset        = 0x03,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[0].green.lc_factor            = 0,             // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_1
    .method_sets[0].red_blue.lc_factor         = 0,             // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_1
    .method_sets[0].green.pg_factor            = 0,             // RPP_MAIN_PRE1_DPCC_PG_FAC_1
    .method_sets[0].red_blue.pg_factor         = 0,             // RPP_MAIN_PRE1_DPCC_PG_FAC_1
    .method_sets[0].green.rg_factor            = 0,             // RPP_MAIN_PRE1_DPCC_RG_FAC_1
    .method_sets[0].red_blue.rg_factor         = 0,             // RPP_MAIN_PRE1_DPCC_RG_FAC_1

    .method_sets[1].green.pg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.lc_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.ro_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.rnd_enabled          = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].green.rg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.pg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.lc_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.ro_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.rnd_enabled       = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].red_blue.rg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_2
    .method_sets[1].set_used                   = false,         // RPP_MAIN_PRE1_DPCC_SET_USE
    .method_sets[1].green.lc_threshold         = 0,             // RPP_MAIN_PRE1_DPCC_LINE_THRESH_2
    .method_sets[1].red_blue.lc_threshold      = 0,             // RPP_MAIN_PRE1_DPCC_LINE_THRESH_2
    .method_sets[1].green.rnd_threshold        = 0,             // RPP_MAIN_PRE1_DPCC_RND_THRESH_2
    .method_sets[1].red_blue.rnd_threshold     = 0,             // RPP_MAIN_PRE1_DPCC_RND_THRESH_2
    .method_sets[1].green.ro_limit             = 0x03,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[1].red_blue.ro_limit          = 0x03,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[1].green.rnd_offset           = 0x03,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[1].red_blue.rnd_offset        = 0x03,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[1].green.lc_factor            = 0,             // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_2
    .method_sets[1].red_blue.lc_factor         = 0,             // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_2
    .method_sets[1].green.pg_factor            = 0,             // RPP_MAIN_PRE1_DPCC_PG_FAC_2
    .method_sets[1].red_blue.pg_factor         = 0,             // RPP_MAIN_PRE1_DPCC_PG_FAC_2
    .method_sets[1].green.rg_factor            = 0,             // RPP_MAIN_PRE1_DPCC_RG_FAC_2
    .method_sets[1].red_blue.rg_factor         = 0,             // RPP_MAIN_PRE1_DPCC_RG_FAC_2

    .method_sets[2].green.pg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.lc_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.ro_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.rnd_enabled          = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].green.rg_enabled           = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.pg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.lc_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.ro_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.rnd_enabled       = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].red_blue.rg_enabled        = false,         // RPP_MAIN_PRE1_DPCC_METHODS_SET_3
    .method_sets[2].set_used                   = false,         // RPP_MAIN_PRE1_DPCC_SET_USE
    .method_sets[2].green.lc_threshold         = 0,             // RPP_MAIN_PRE1_DPCC_LINE_THRESH_3
    .method_sets[2].red_blue.lc_threshold      = 0,             // RPP_MAIN_PRE1_DPCC_LINE_THRESH_3
    .method_sets[2].green.rnd_threshold        = 0,             // RPP_MAIN_PRE1_DPCC_RND_THRESH_3
    .method_sets[2].red_blue.rnd_threshold     = 0,             // RPP_MAIN_PRE1_DPCC_RND_THRESH_3
    .method_sets[2].green.ro_limit             = 0x03,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[2].red_blue.ro_limit          = 0x03,          // RPP_MAIN_PRE1_DPCC_RO_LIMITS
    .method_sets[2].green.rnd_offset           = 0x03,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[2].red_blue.rnd_offset        = 0x03,          // RPP_MAIN_PRE1_DPCC_RND_OFFS
    .method_sets[2].green.lc_factor            = 0,             // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_3
    .method_sets[2].red_blue.lc_factor         = 0,             // RPP_MAIN_PRE1_DPCC_LINE_MAD_FAC_3
    .method_sets[2].green.pg_factor            = 0,             // RPP_MAIN_PRE1_DPCC_PG_FAC_3
    .method_sets[2].red_blue.pg_factor         = 0,             // RPP_MAIN_PRE1_DPCC_PG_FAC_3
    .method_sets[2].green.rg_factor            = 0,             // RPP_MAIN_PRE1_DPCC_RG_FAC_3
    .method_sets[2].red_blue.rg_factor         = 0,             // RPP_MAIN_PRE1_DPCC_RG_FAC_3

    .output_mode.green    = CISP_DPCC_INTERPOLATION_MEDIAN_4,   // RPP_MAIN_PRE1_DPCC_OUTPUT_MODE
    .output_mode.red_blue = CISP_DPCC_INTERPOLATION_MEDIAN_4,   // RPP_MAIN_PRE1_DPCC_OUTPUT_MODE
};

/* CAC config table*/
st_cisp_cac_config_t cac_enable_config_table =
{
    .cac_enable        = true,   // RPP_MAIN_POST_CAC_CTRL
    .coeffs.a_blue     = 0,      // RPP_MAIN_POST_CAC_A
    .coeffs.a_red      = 0,      // RPP_MAIN_POST_CAC_A
    .coeffs.b_blue     = 0,      // RPP_MAIN_POST_CAC_B
    .coeffs.b_red      = 0,      // RPP_MAIN_POST_CAC_B
    .coeffs.c_blue     = 0,      // RPP_MAIN_POST_CAC_C
    .coeffs.c_red      = 0,      // RPP_MAIN_POST_CAC_C
    .h_clip            = CISP_CAC_H_CLIP_MODE_FIXED,
    .h_start           = 0x1000, // RPP_MAIN_POST_CAC_COUNT_START // RPP_MAIN_POST_CAC_CTRL
    .v_clip            = CISP_CAC_V_CLIP_MODE_FIXED_2,
    .v_start           = 0x1000, // RPP_MAIN_POST_CAC_COUNT_START // RPP_MAIN_POST_CAC_CTRL
    .norm_params.x_nf  = 0x10,   // RPP_MAIN_POST_CAC_X_NORM
    .norm_params.x_ns  = 0x08,   // RPP_MAIN_POST_CAC_X_NORM
    .norm_params.y_nf  = 0x10,   // RPP_MAIN_POST_CAC_Y_NORM
    .norm_params.y_ns  = 0x08,   // RPP_MAIN_POST_CAC_Y_NORM
};
st_cisp_cac_config_t cac_disable_config_table =
{
    .cac_enable        = false,   // RPP_MAIN_POST_CAC_CTRL
    .coeffs.a_blue     = 0,      // RPP_MAIN_POST_CAC_A
    .coeffs.a_red      = 0,      // RPP_MAIN_POST_CAC_A
    .coeffs.b_blue     = 0,      // RPP_MAIN_POST_CAC_B
    .coeffs.b_red      = 0,      // RPP_MAIN_POST_CAC_B
    .coeffs.c_blue     = 0,      // RPP_MAIN_POST_CAC_C
    .coeffs.c_red      = 0,      // RPP_MAIN_POST_CAC_C
    .h_clip            = CISP_CAC_H_CLIP_MODE_FIXED,
    .h_start           = 0x1000, // RPP_MAIN_POST_CAC_COUNT_START // RPP_MAIN_POST_CAC_CTRL
    .v_clip            = CISP_CAC_V_CLIP_MODE_FIXED_2,
    .v_start           = 0x1000, // RPP_MAIN_POST_CAC_COUNT_START // RPP_MAIN_POST_CAC_CTRL
    .norm_params.x_nf  = 0x10,   // RPP_MAIN_POST_CAC_X_NORM
    .norm_params.x_ns  = 0x08,   // RPP_MAIN_POST_CAC_X_NORM
    .norm_params.y_nf  = 0x10,   // RPP_MAIN_POST_CAC_Y_NORM
    .norm_params.y_ns  = 0x08,   // RPP_MAIN_POST_CAC_Y_NORM
};

/* SHRPCNR config table*/
st_cisp_shrpcnr_config_t shrpcnr_enable_config_table =
{
    .cad_config.cad_enable     = true,                  // RPP_MAIN_POST_SHRPCNR_CTRL
    .cad_config.restore_lvl    = 2,                     // RPP_MAIN_POST_SHRPCNR_CAD_RESTORE_LVL
    .cad_config.umax           = 82,                    // RPP_MAIN_POST_SHRPCNR_CAD_THRESH_U
    .cad_config.umin           = 4000,                  // RPP_MAIN_POST_SHRPCNR_CAD_THRESH_U
    .cad_config.uneg_vmax      = 256,                   // RPP_MAIN_POST_SHRPCNR_CAD_THRESH_V_UNEG
    .cad_config.uneg_vmin      = 4000,                  // RPP_MAIN_POST_SHRPCNR_CAD_THRESH_V_UNEG
    .cad_config.upos_vmax      = 682,                   // RPP_MAIN_POST_SHRPCNR_CAD_THRESH_V_UPOS
    .cad_config.upos_vmin      = 4000,                  // RPP_MAIN_POST_SHRPCNR_CAD_THRESH_V_UPOS

    .cnr_config.cnr_enable     = true,                  // RPP_MAIN_POST_SHRPCNR_CTRL
    .cnr_config.thres_cb       = 150,                   // RPP_MAIN_POST_SHRPCNR_CNR_THRES
    .cnr_config.thres_cr       = 150,                   // RPP_MAIN_POST_SHRPCNR_CNR_THRES

    .desat_config.desat_enable = false,                 // RPP_MAIN_POST_SHRPCNR_CTRL
    .desat_config.div          = 0,                     // RPP_MAIN_POST_SHRPCNR_CRED_SLOPE
    .desat_config.slope        = 0,                     // RPP_MAIN_POST_SHRPCNR_CRED_SLOPE
    .desat_config.threshold    = 0,                     // RPP_MAIN_POST_SHRPCNR_CRED_THRES

    .line_width                = INPUT_IMAGE_WIDTH,     // RPP_MAIN_POST_SHRPCNR_CLB_LINESIZE

    .profile.coeff[0]          = 4096,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_0
    .profile.coeff[1]          = 0,                     // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_1
    .profile.coeff[2]          = 6450,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_2
    .profile.coeff[3]          = 4096,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_3
    .profile.coeff[4]          = -767,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_4
    .profile.coeff[5]          = -1917,                 // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_5
    .profile.coeff[6]          = 4096,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_6
    .profile.coeff[7]          = 7601,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_7
    .profile.coeff[8]          = 0,                     // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_8
    .profile.off[0]            = 0,                     // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_OFFSET_R
    .profile.off[1]            = 2048,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_OFFSET_G
    .profile.off[2]            = 2048,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_OFFSET_B

    .shrp_config.shrp_enable      = true,               // RPP_MAIN_POST_SHRPCNR_CTRL
    .shrp_config.strength         = 8,                  // RPP_MAIN_POST_SHRPCNR_PARAM
    .shrp_config.threshold        = 100,                // RPP_MAIN_POST_SHRPCNR_PARAM
    .shrp_config.filter_coeffs[0] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_1
    .shrp_config.filter_coeffs[1] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_1
    .shrp_config.filter_coeffs[2] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_1
    .shrp_config.filter_coeffs[3] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_1
    .shrp_config.filter_coeffs[4] = 8,                  // RPP_MAIN_POST_SHRPCNR_MAT_1
    .shrp_config.filter_coeffs[5] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_1
    .shrp_config.filter_coeffs[6] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_2
    .shrp_config.filter_coeffs[7] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_2
    .shrp_config.filter_coeffs[8] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_2
};
st_cisp_shrpcnr_config_t shrpcnr_disable_config_table =
{
    .cad_config.cad_enable     = false,                 // RPP_MAIN_POST_SHRPCNR_CTRL
    .cad_config.restore_lvl    = 2,                     // RPP_MAIN_POST_SHRPCNR_CAD_RESTORE_LVL
    .cad_config.umax           = 82,                    // RPP_MAIN_POST_SHRPCNR_CAD_THRESH_U
    .cad_config.umin           = 4000,                  // RPP_MAIN_POST_SHRPCNR_CAD_THRESH_U
    .cad_config.uneg_vmax      = 256,                   // RPP_MAIN_POST_SHRPCNR_CAD_THRESH_V_UNEG
    .cad_config.uneg_vmin      = 4000,                  // RPP_MAIN_POST_SHRPCNR_CAD_THRESH_V_UNEG
    .cad_config.upos_vmax      = 682,                   // RPP_MAIN_POST_SHRPCNR_CAD_THRESH_V_UPOS
    .cad_config.upos_vmin      = 4000,                  // RPP_MAIN_POST_SHRPCNR_CAD_THRESH_V_UPOS

    .cnr_config.cnr_enable     = false,                 // RPP_MAIN_POST_SHRPCNR_CTRL
    .cnr_config.thres_cb       = 150,                   // RPP_MAIN_POST_SHRPCNR_CNR_THRES
    .cnr_config.thres_cr       = 150,                   // RPP_MAIN_POST_SHRPCNR_CNR_THRES

    .desat_config.desat_enable = false,                 // RPP_MAIN_POST_SHRPCNR_CTRL
    .desat_config.div          = 0,                     // RPP_MAIN_POST_SHRPCNR_CRED_SLOPE
    .desat_config.slope        = 0,                     // RPP_MAIN_POST_SHRPCNR_CRED_SLOPE
    .desat_config.threshold    = 0,                     // RPP_MAIN_POST_SHRPCNR_CRED_THRES

    .line_width                = INPUT_IMAGE_WIDTH,     // RPP_MAIN_POST_SHRPCNR_CLB_LINESIZE

    .profile.coeff[0]          = 4096,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_0
    .profile.coeff[1]          = 0,                     // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_1
    .profile.coeff[2]          = 6450,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_2
    .profile.coeff[3]          = 4096,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_3
    .profile.coeff[4]          = -767,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_4
    .profile.coeff[5]          = -1917,                 // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_5
    .profile.coeff[6]          = 4096,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_6
    .profile.coeff[7]          = 7601,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_7
    .profile.coeff[8]          = 0,                     // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_COEFF_8
    .profile.off[0]            = 0,                     // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_OFFSET_R
    .profile.off[1]            = 2048,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_OFFSET_G
    .profile.off[2]            = 2048,                  // RPP_MAIN_POST_SHRPCNR_YUV2RGB_CCOR_OFFSET_B

    .shrp_config.shrp_enable      = false,              // RPP_MAIN_POST_SHRPCNR_CTRL
    .shrp_config.strength         = 8,                  // RPP_MAIN_POST_SHRPCNR_PARAM
    .shrp_config.threshold        = 100,                // RPP_MAIN_POST_SHRPCNR_PARAM
    .shrp_config.filter_coeffs[0] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_1
    .shrp_config.filter_coeffs[1] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_1
    .shrp_config.filter_coeffs[2] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_1
    .shrp_config.filter_coeffs[3] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_1
    .shrp_config.filter_coeffs[4] = 8,                  // RPP_MAIN_POST_SHRPCNR_MAT_1
    .shrp_config.filter_coeffs[5] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_1
    .shrp_config.filter_coeffs[6] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_2
    .shrp_config.filter_coeffs[7] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_2
    .shrp_config.filter_coeffs[8] = -1,                 // RPP_MAIN_POST_SHRPCNR_MAT_2
};

/* HIST config table*/
st_cisp_hist_config_t hist_pre1_enable_config_table =
{
    .channel                           = CISP_HIST_CHANNEL_0,        // RPP_MAIN_PRE1_HIST_CHANNEL_SEL
    .color_conv_coef.coeff_b           = 13,                         // RPP_MAIN_PRE1_HIST_COEFF_B
    .color_conv_coef.coeff_g           = 64,                         // RPP_MAIN_PRE1_HIST_COEFF_G
    .color_conv_coef.coeff_r           = 33,                         // RPP_MAIN_PRE1_HIST_COEFF_R
    .meas_wind_config.steps.h_step_inc = 65536,                      // RPP_MAIN_PRE1_HIST_SUBSAMPLING
    .meas_wind_config.steps.v_stepsize = 1,                          // RPP_MAIN_PRE1_HIST_SUBSAMPLING

    .meas_wind_config.window.v_offset  = 0,                          // RPP_MAIN_PRE1_HIST_V_OFFS
    .meas_wind_config.window.height    = INPUT_IMAGE_HEIGHT/5,       // RPP_MAIN_PRE1_HIST_V_SIZE
    .meas_wind_config.window.h_offset  = 0,                          // RPP_MAIN_PRE1_HIST_H_OFFS
    .meas_wind_config.window.width     = INPUT_IMAGE_WIDTH/5,        // RPP_MAIN_PRE1_HIST_H_SIZE

    .measure_mode                      = CISP_HIST_MEASURE_MODE_Y,   // RPP_MAIN_PRE1_HIST_MODE
    .sample_range.sample_offset        = 0,                          // RPP_MAIN_PRE1_HIST_SAMPLE_RANGE
    .sample_range.sample_shift         = 0,                          // RPP_MAIN_PRE1_HIST_SAMPLE_RANGE

    .weights[0]  = 0x10,                                             // RPP_MAIN_PRE1_HIST_WEIGHT_00TO30 - 44
    .weights[1]  = 0x10,
    .weights[2]  = 0x10,
    .weights[3]  = 0x10,
    .weights[4]  = 0x10,
    .weights[5]  = 0x10,
    .weights[6]  = 0x10,
    .weights[7]  = 0x10,
    .weights[8]  = 0x10,
    .weights[9]  = 0x10,
    .weights[10] = 0x10,
    .weights[11] = 0x10,
    .weights[12] = 0x10,
    .weights[13] = 0x10,
    .weights[14] = 0x10,
    .weights[15] = 0x10,
    .weights[16] = 0x10,
    .weights[17] = 0x10,
    .weights[18] = 0x10,
    .weights[19] = 0x10,
    .weights[20] = 0x10,
    .weights[21] = 0x10,
    .weights[22] = 0x10,
    .weights[23] = 0x10,
    .weights[24] = 0x10,
};
st_cisp_hist_config_t hist_pre1_disable_config_table =
{
    .channel                           = CISP_HIST_CHANNEL_0,               // RPP_MAIN_PRE1_HIST_CHANNEL_SEL
    .color_conv_coef.coeff_b           = 13,                                // RPP_MAIN_PRE1_HIST_COEFF_B
    .color_conv_coef.coeff_g           = 64,                                // RPP_MAIN_PRE1_HIST_COEFF_G
    .color_conv_coef.coeff_r           = 33,                                // RPP_MAIN_PRE1_HIST_COEFF_R
    .meas_wind_config.steps.h_step_inc = 65536,                             // RPP_MAIN_PRE1_HIST_SUBSAMPLING
    .meas_wind_config.steps.v_stepsize = 1,                                 // RPP_MAIN_PRE1_HIST_SUBSAMPLING

    .meas_wind_config.window.v_offset  = 0,                                 // RPP_MAIN_PRE1_HIST_V_OFFS
    .meas_wind_config.window.height    = INPUT_IMAGE_HEIGHT/5,              // RPP_MAIN_PRE1_HIST_V_SIZE
    .meas_wind_config.window.h_offset  = 0,                                 // RPP_MAIN_PRE1_HIST_H_OFFS
    .meas_wind_config.window.width     = INPUT_IMAGE_WIDTH/5,               // RPP_MAIN_PRE1_HIST_H_SIZE

    .measure_mode                      = CISP_HIST_MEASURE_MODE_DISABLE,    // RPP_MAIN_PRE1_HIST_MODE
    .sample_range.sample_offset        = 0,                                 // RPP_MAIN_PRE1_HIST_SAMPLE_RANGE
    .sample_range.sample_shift         = 0,                                 // RPP_MAIN_PRE1_HIST_SAMPLE_RANGE

    .weights[0]  = 0x10,                                                    // RPP_MAIN_PRE1_HIST_WEIGHT_00TO30 - 44
    .weights[1]  = 0x10,
    .weights[2]  = 0x10,
    .weights[3]  = 0x10,
    .weights[4]  = 0x10,
    .weights[5]  = 0x10,
    .weights[6]  = 0x10,
    .weights[7]  = 0x10,
    .weights[8]  = 0x10,
    .weights[9]  = 0x10,
    .weights[10] = 0x10,
    .weights[11] = 0x10,
    .weights[12] = 0x10,
    .weights[13] = 0x10,
    .weights[14] = 0x10,
    .weights[15] = 0x10,
    .weights[16] = 0x10,
    .weights[17] = 0x10,
    .weights[18] = 0x10,
    .weights[19] = 0x10,
    .weights[20] = 0x10,
    .weights[21] = 0x10,
    .weights[22] = 0x10,
    .weights[23] = 0x10,
    .weights[24] = 0x10,
};
st_cisp_hist_config_t hist_pre2_enable_config_table =
{
    .channel                           = CISP_HIST_CHANNEL_0,               // RPP_MAIN_PRE2_HIST_CHANNEL_SEL
    .color_conv_coef.coeff_b           = 13,                                // RPP_MAIN_PRE2_HIST_COEFF_B
    .color_conv_coef.coeff_g           = 64,                                // RPP_MAIN_PRE2_HIST_COEFF_G
    .color_conv_coef.coeff_r           = 33,                                // RPP_MAIN_PRE2_HIST_COEFF_R
    .meas_wind_config.steps.h_step_inc = 65536,                             // RPP_MAIN_PRE2_HIST_SUBSAMPLING
    .meas_wind_config.steps.v_stepsize = 1,                                 // RPP_MAIN_PRE2_HIST_SUBSAMPLING

    .meas_wind_config.window.v_offset  = 0,                                 // RPP_MAIN_PRE2_HIST_V_OFFS
    .meas_wind_config.window.height    = INPUT_IMAGE_HEIGHT/5,              // RPP_MAIN_PRE2_HIST_V_SIZE
    .meas_wind_config.window.h_offset  = 0,                                 // RPP_MAIN_PRE2_HIST_H_OFFS
    .meas_wind_config.window.width     = INPUT_IMAGE_WIDTH/5,               // RPP_MAIN_PRE2_HIST_H_SIZE
    .measure_mode                      = CISP_HIST_MEASURE_MODE_Y,          // RPP_MAIN_PRE2_HIST_MODE
    .sample_range.sample_offset        = 0,                                 // RPP_MAIN_PRE2_HIST_SAMPLE_RANGE
    .sample_range.sample_shift         = 0,                                 // RPP_MAIN_PRE2_HIST_SAMPLE_RANGE

    .weights[0]  = 0x10,                                                    // RPP_MAIN_PRE2_HIST_WEIGHT_00TO30 - 44
    .weights[1]  = 0x10,
    .weights[2]  = 0x10,
    .weights[3]  = 0x10,
    .weights[4]  = 0x10,
    .weights[5]  = 0x10,
    .weights[6]  = 0x10,
    .weights[7]  = 0x10,
    .weights[8]  = 0x10,
    .weights[9]  = 0x10,
    .weights[10] = 0x10,
    .weights[11] = 0x10,
    .weights[12] = 0x10,
    .weights[13] = 0x10,
    .weights[14] = 0x10,
    .weights[15] = 0x10,
    .weights[16] = 0x10,
    .weights[17] = 0x10,
    .weights[18] = 0x10,
    .weights[19] = 0x10,
    .weights[20] = 0x10,
    .weights[21] = 0x10,
    .weights[22] = 0x10,
    .weights[23] = 0x10,
    .weights[24] = 0x10,
};
st_cisp_hist_config_t hist_pre2_disable_config_table =
{
    .channel                           = CISP_HIST_CHANNEL_0,               // RPP_MAIN_PRE2_HIST_CHANNEL_SEL
    .color_conv_coef.coeff_b           = 13,                                // RPP_MAIN_PRE2_HIST_COEFF_B
    .color_conv_coef.coeff_g           = 64,                                // RPP_MAIN_PRE2_HIST_COEFF_G
    .color_conv_coef.coeff_r           = 33,                                // RPP_MAIN_PRE2_HIST_COEFF_R
    .meas_wind_config.steps.h_step_inc = 65536,                             // RPP_MAIN_PRE2_HIST_SUBSAMPLING
    .meas_wind_config.steps.v_stepsize = 1,                                 // RPP_MAIN_PRE2_HIST_SUBSAMPLING

    .meas_wind_config.window.v_offset  = 0,                                 // RPP_MAIN_PRE2_HIST_V_OFFS
    .meas_wind_config.window.height    = INPUT_IMAGE_HEIGHT/5,              // RPP_MAIN_PRE2_HIST_V_SIZE
    .meas_wind_config.window.h_offset  = 0,                                 // RPP_MAIN_PRE2_HIST_H_OFFS
    .meas_wind_config.window.width     = INPUT_IMAGE_WIDTH/5,               // RPP_MAIN_PRE2_HIST_H_SIZE
    .measure_mode                      = CISP_HIST_MEASURE_MODE_DISABLE,    // RPP_MAIN_PRE2_HIST_MODE
    .sample_range.sample_offset        = 0,                                 // RPP_MAIN_PRE2_HIST_SAMPLE_RANGE
    .sample_range.sample_shift         = 0,                                 // RPP_MAIN_PRE2_HIST_SAMPLE_RANGE

    .weights[0]  = 0x10,                                                    // RPP_MAIN_PRE2_HIST_WEIGHT_00TO30 - 44
    .weights[1]  = 0x10,
    .weights[2]  = 0x10,
    .weights[3]  = 0x10,
    .weights[4]  = 0x10,
    .weights[5]  = 0x10,
    .weights[6]  = 0x10,
    .weights[7]  = 0x10,
    .weights[8]  = 0x10,
    .weights[9]  = 0x10,
    .weights[10] = 0x10,
    .weights[11] = 0x10,
    .weights[12] = 0x10,
    .weights[13] = 0x10,
    .weights[14] = 0x10,
    .weights[15] = 0x10,
    .weights[16] = 0x10,
    .weights[17] = 0x10,
    .weights[18] = 0x10,
    .weights[19] = 0x10,
    .weights[20] = 0x10,
    .weights[21] = 0x10,
    .weights[22] = 0x10,
    .weights[23] = 0x10,
    .weights[24] = 0x10,
};
st_cisp_hist_config_t hist_post_enable_config_table =
{
    .channel                           = CISP_HIST_CHANNEL_0,               // RPP_MAIN_POST_HIST_CHANNEL_SEL
    .color_conv_coef.coeff_b           = 13,                                // RPP_MAIN_POST_HIST_COEFF_B
    .color_conv_coef.coeff_g           = 64,                                // RPP_MAIN_POST_HIST_COEFF_G
    .color_conv_coef.coeff_r           = 33,                                // RPP_MAIN_POST_HIST_COEFF_R
    .meas_wind_config.steps.h_step_inc = 65536,                             // RPP_MAIN_POST_HIST_SUBSAMPLING
    .meas_wind_config.steps.v_stepsize = 1,                                 // RPP_MAIN_POST_HIST_SUBSAMPLING

    .meas_wind_config.window.v_offset  = 0,                                 // RPP_MAIN_POST_HIST_V_OFFS
    .meas_wind_config.window.height    = INPUT_IMAGE_HEIGHT/5,              // RPP_MAIN_POST_HIST_V_SIZE
    .meas_wind_config.window.h_offset  = 0,                                 // RPP_MAIN_POST_HIST_H_OFFS
    .meas_wind_config.window.width     = INPUT_IMAGE_WIDTH/5,               // RPP_MAIN_POST_HIST_H_SIZE

    .measure_mode                      = CISP_HIST_MEASURE_MODE_Y,          // RPP_MAIN_POST_HIST_MODE
    .sample_range.sample_offset        = 0,                                 // RPP_MAIN_POST_HIST_SAMPLE_RANGE
    .sample_range.sample_shift         = 0,                                 // RPP_MAIN_POST_HIST_SAMPLE_RANGE

    .weights[0]  = 0x10,                                                    // RPP_MAIN_POST_HIST_WEIGHT_00TO30 - 44
    .weights[1]  = 0x10,
    .weights[2]  = 0x10,
    .weights[3]  = 0x10,
    .weights[4]  = 0x10,
    .weights[5]  = 0x10,
    .weights[6]  = 0x10,
    .weights[7]  = 0x10,
    .weights[8]  = 0x10,
    .weights[9]  = 0x10,
    .weights[10] = 0x10,
    .weights[11] = 0x10,
    .weights[12] = 0x10,
    .weights[13] = 0x10,
    .weights[14] = 0x10,
    .weights[15] = 0x10,
    .weights[16] = 0x10,
    .weights[17] = 0x10,
    .weights[18] = 0x10,
    .weights[19] = 0x10,
    .weights[20] = 0x10,
    .weights[21] = 0x10,
    .weights[22] = 0x10,
    .weights[23] = 0x10,
    .weights[24] = 0x10,
};
st_cisp_hist_config_t hist_post_disable_config_table =
{
    .channel                           = CISP_HIST_CHANNEL_0,               // RPP_MAIN_POST_HIST_CHANNEL_SEL
    .color_conv_coef.coeff_b           = 13,                                // RPP_MAIN_POST_HIST_COEFF_B
    .color_conv_coef.coeff_g           = 64,                                // RPP_MAIN_POST_HIST_COEFF_G
    .color_conv_coef.coeff_r           = 33,                                // RPP_MAIN_POST_HIST_COEFF_R
    .meas_wind_config.steps.h_step_inc = 65536,                             // RPP_MAIN_POST_HIST_SUBSAMPLING
    .meas_wind_config.steps.v_stepsize = 1,                                 // RPP_MAIN_POST_HIST_SUBSAMPLING

    .meas_wind_config.window.v_offset  = 0,                                 // RPP_MAIN_POST_HIST_V_OFFS
    .meas_wind_config.window.height    = INPUT_IMAGE_HEIGHT/5,              // RPP_MAIN_POST_HIST_V_SIZE
    .meas_wind_config.window.h_offset  = 0,                                 // RPP_MAIN_POST_HIST_H_OFFS
    .meas_wind_config.window.width     = INPUT_IMAGE_WIDTH/5,               // RPP_MAIN_POST_HIST_H_SIZE

    .measure_mode                      = CISP_HIST_MEASURE_MODE_DISABLE,    // RPP_MAIN_POST_HIST_MODE
    .sample_range.sample_offset        = 0,                                 // RPP_MAIN_POST_HIST_SAMPLE_RANGE
    .sample_range.sample_shift         = 0,                                 // RPP_MAIN_POST_HIST_SAMPLE_RANGE

    .weights[0]  = 0x10,                                                    // RPP_MAIN_POST_HIST_WEIGHT_00TO30 - 44
    .weights[1]  = 0x10,
    .weights[2]  = 0x10,
    .weights[3]  = 0x10,
    .weights[4]  = 0x10,
    .weights[5]  = 0x10,
    .weights[6]  = 0x10,
    .weights[7]  = 0x10,
    .weights[8]  = 0x10,
    .weights[9]  = 0x10,
    .weights[10] = 0x10,
    .weights[11] = 0x10,
    .weights[12] = 0x10,
    .weights[13] = 0x10,
    .weights[14] = 0x10,
    .weights[15] = 0x10,
    .weights[16] = 0x10,
    .weights[17] = 0x10,
    .weights[18] = 0x10,
    .weights[19] = 0x10,
    .weights[20] = 0x10,
    .weights[21] = 0x10,
    .weights[22] = 0x10,
    .weights[23] = 0x10,
    .weights[24] = 0x10,
};

/* RMAPMEAS config table*/
st_cisp_rmapmeas_config_t rmapmeas_enable_config_table =
{
    .meas_wind_config.steps.h_step_inc = 65536,                     // RPP_RMAP_MEAS_SUBSAMPLING
    .meas_wind_config.steps.v_stepsize = 1,                         // RPP_RMAP_MEAS_SUBSAMPLING
    .meas_wind_config.window.v_offset  = 0,                         // RPP_RMAP_MEAS_V_OFFS
    .meas_wind_config.window.height    = INPUT_IMAGE_HEIGHT,        // RPP_RMAP_MEAS_V_SIZE
    .meas_wind_config.window.h_offset  = 0,                         // RPP_RMAP_MEAS_H_OFFS
    .meas_wind_config.window.width     = INPUT_IMAGE_WIDTH,         // RPP_RMAP_MEAS_H_SIZE

    .measure_mode                      = CISP_RMAPMEAS_MODE_ENABLE, // RPP_RMAP_MEAS_MODE
    .thresholds.l_max                  = 0x0FFF,                    // RPP_RMAP_MEAS_MAX_THRES_LONG
    .thresholds.s_max                  = 0x0FFF,                    // RPP_RMAP_MEAS_MAX_THRES_SHORT
    .thresholds.s_min                  = 0,                         // RPP_RMAP_MEAS_MIN_THRES_SHORT
};
st_cisp_rmapmeas_config_t rmapmeas_disable_config_table =
{
    .meas_wind_config.steps.h_step_inc = 65536,                     // RPP_RMAP_MEAS_SUBSAMPLING
    .meas_wind_config.steps.v_stepsize = 1,                         // RPP_RMAP_MEAS_SUBSAMPLING
    .meas_wind_config.window.v_offset  = 0,                         // RPP_RMAP_MEAS_V_OFFS
    .meas_wind_config.window.height    = INPUT_IMAGE_HEIGHT,        // RPP_RMAP_MEAS_V_SIZE
    .meas_wind_config.window.h_offset  = 0,                         // RPP_RMAP_MEAS_H_OFFS
    .meas_wind_config.window.width     = INPUT_IMAGE_WIDTH,         // RPP_RMAP_MEAS_H_SIZE

    .measure_mode                      = CISP_RMAPMEAS_MODE_DISABLE,// RPP_RMAP_MEAS_MODE
    .thresholds.l_max                  = 0x0FFF,                    // RPP_RMAP_MEAS_MAX_THRES_LONG
    .thresholds.s_max                  = 0x0FFF,                    // RPP_RMAP_MEAS_MAX_THRES_SHORT
    .thresholds.s_min                  = 0,                         // RPP_RMAP_MEAS_MIN_THRES_SHORT
};

/* XYZ2LUV config table*/
st_cisp_xyz2luv_config_t xyz2luv_config_table =
{
    .chroma_factor = 128, // RPP_MV_OUT_XYZ2LUV_CHROMA_OUT_FAC
    .luma_factor   = 128, // RPP_MV_OUT_XYZ2LUV_LUMA_OUT_FAC
    .u_ref         = 203, // RPP_MV_OUT_XYZ2LUV_U_REF
    .v_ref         = 480, // RPP_MV_OUT_XYZ2LUV_V_REF
};

#endif  /* _R_ISP_EXAMPLE_CONFIG_H_ */
