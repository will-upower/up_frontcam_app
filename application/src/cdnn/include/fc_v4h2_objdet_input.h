{
    .batch_size         = 1,
    .inttensor_size     = 0x01568000,
    .dsp_dtcm_udbs      = 0x00000000,
    .dsp_cdnn_udbs      = 0x00000000,
    .dsp_rdl_udbs       = 0x00000000,
    .input_image_num    = 1,
    .input_info =
    {
        {
            .input_name          = "./fc_v4h2/objdet/1_raw_script.bin",
            .input_ch            = 3,
            .input_width         = 320,
            .input_height        = 320,
            .input_ch_stride     = 102400,
            .input_mem_stride    = 320,
            .inputtensor_size    = 409600,
            .bytepp              = 1,
            .orig_input_addr     = 0xc0000000,
            .cdnn_buffer         = {0},
            .orig_input_ch_stride= 102400,
            .BufferName          = "input1",
        },
    }
},