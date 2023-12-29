{
    .batch_size         = 1,
    .inttensor_size     = 0x00e00000,
    .dsp_dtcm_udbs      = 0x00000000,
    .dsp_cdnn_udbs      = 0x00000000,
    .dsp_rdl_udbs       = 0x00000000,
    .input_image_num    = 1,
    .input_info =
    {
        {
            .input_name          = "./fc_v4h2/semseg/4.bin",
            .input_ch            = 3,
            .input_width         = 256,
            .input_height        = 256,
            .input_ch_stride     = 65536,
            .input_mem_stride    = 256,
            .inputtensor_size    = 262144,
            .bytepp              = 1,
            .orig_input_addr     = 0xc0000000,
            .cdnn_buffer         = {0},
            .orig_input_ch_stride= 65536,
            .BufferName          = "image_input",
        },
    }
},