{
    .batch_size         = 1,
    .inttensor_size     = 0x000f5000,
    .dsp_dtcm_udbs      = 0x00000000,
    .dsp_cdnn_udbs      = 0x00000000,
    .dsp_rdl_udbs       = 0x00000000,
    .input_image_num    = 1,
    .input_info =
    {
        {
            .input_name          = "./fc_v4h2/poseest/images_t.bin",
            .input_ch            = 3,
            .input_width         = 224,
            .input_height        = 224,
            .input_ch_stride     = 50176,
            .input_mem_stride    = 224,
            .inputtensor_size    = 200704,
            .bytepp              = 1,
            .orig_input_addr     = 0xc0000000,
            .cdnn_buffer         = {0},
            .orig_input_ch_stride= 50176,
            .BufferName          = "images",
        },
    }
},