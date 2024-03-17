{
    .batch_size         = 1,
    .inttensor_size     = 0x00208000,
    .dsp_dtcm_udbs      = 0x00000000,
    .dsp_cdnn_udbs      = 0x00000000,
    .dsp_rdl_udbs       = 0x00588000,
    .input_image_num    = 1,
    .input_info =
    {
        {
            .input_name       = "./fc_v4h2/rnet/input_t.bin",
            .input_ch            = 3,
            .input_width         = 512,
            .input_height        = 256,
            .input_ch_stride     = 131072,
            .input_mem_stride    = 512,
            .inputtensor_size    = 393216,
            .bytepp              = 1,
            .orig_input_addr     = 0xc0000000,
            .cdnn_buffer         = {0},
            .orig_input_ch_stride = 131072,
            .BufferName          = "input",
        },
    }
},