/*
Copyright (c) 2016, Jean-Francois Pambrun
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "grok.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define EMSCRIPTEN_KEEPALIVE __attribute__((used))
#define J2K_MAGIC_NUMBER 0x51FF4FFF

//
//  Callbacks
//

static void error_callback(const char *msg, void *client_data) {
    (void)client_data;
    printf("[ERROR] %s", msg);
}
static void warning_callback(const char *msg, void *client_data) {
    (void)client_data;
    // printf("[WARNING] %s", msg);
}
static void info_callback(const char *msg, void *client_data) {
    (void)client_data;
    // printf("[INFO] %s", msg);
}

//
//  API
//
extern "C" int EMSCRIPTEN_KEEPALIVE jp2_decode(
    void* data,
    int data_size,
    void** p_image,
    int* p_image_size,
    int* size_x,
    int* size_y,
    int* size_comp,
    int* prec,
    int* sgnd
) {
    grk_dparameters_t parameters;
    grk_codec_t* l_codec = NULL;
    grk_image_t* image = NULL;
    grk_stream_t *l_stream = NULL;

    printf("[ERROR] In decode");
    grk_initialize(nullptr, 8);
    printf("[ERROR] Done init");

    // detect stream type
    if (((int32_t*)data)[0] == J2K_MAGIC_NUMBER) {
        l_codec = grk_create_decompress(GRK_CODEC_J2K);
    } else {
        l_codec = grk_create_decompress(GRK_CODEC_JP2);
    }
    printf("[ERROR] Created codec");

    grk_set_info_handler(l_codec, info_callback, 00);
    grk_set_warning_handler(l_codec, warning_callback, 00);
    grk_set_error_handler(l_codec, error_callback, 00);

    grk_set_default_decoder_parameters(&parameters);

    // set stream
//    grk_buffer_info_t buffer_info;
//    buffer_info.buf = data;
//    buffer_info.cur = data;
//    buffer_info.len = data_size;
    l_stream = grk_stream_create_buffer_stream((uint8_t*)data, data_size, true, true);
    printf("Created stream");

    // Setup the decoder decoding parameters using user parameters
    if (!grk_setup_decoder(l_codec, &parameters)) {
        printf("[ERROR] grk_decompress: failed to setup the decoder\n");
        grk_stream_destroy(l_stream);
        grk_destroy_codec(l_codec);
        grk_deinitialize();
        return 1;
    }
    printf("Decoder set up");

    // Read the main header of the codestream and if necessary the JP2 boxes
    if (!grk_read_header(l_stream, l_codec, &image)) {
        printf("[ERROR] grk_decompress: failed to read the header\n");
        grk_stream_destroy(l_stream);
        grk_destroy_codec(l_codec);
        grk_image_destroy(image);
        grk_deinitialize();
        return 1;
    }
    printf("read header");

    // decode the image
    if (!grk_decode(l_codec, nullptr, l_stream, image)) {
        printf("[ERROR] grk_decompress: failed to decode tile!\n");
        grk_destroy_codec(l_codec);
        grk_stream_destroy(l_stream);
        grk_image_destroy(image);
        grk_deinitialize();
        return 1;
    }
    printf("decode");

    if (!grk_end_decompress(l_codec, l_stream)) {
        printf("[ERROR] grk_end_decompress failed!\n");
        grk_destroy_codec(l_codec);
        grk_stream_destroy(l_stream);
        grk_image_destroy(image);
        grk_deinitialize();
        return 1;
    }
    printf("end decompress");

    printf("tile %d is decoded!\n\n", parameters.tile_index);
    printf("image X %d\n", image->x1);
    printf("image Y %d\n", image->y1);
    printf("image numcomps %d\n", image->numcomps);

    printf("prec=%d, sgnd=%d\n", image->comps[0].prec, image->comps[0].sgnd);

    *size_x = image->x1;
    *size_y = image->y1;
    *size_comp = image->numcomps;
    *sgnd = image->comps[0].sgnd;
    *prec = image->comps[0].prec;

    *p_image_size = (*size_x) * (*size_y) * (*size_comp) * sizeof(int32_t);
    *p_image = malloc(*p_image_size);

    if (*size_comp == 1) {
        memcpy(*p_image, image->comps[0].data, *p_image_size);
    } else if(*size_comp == 3) {
        for (int i = 0; i < (*size_x) * (*size_y); i++) {
            ((int32_t*)*p_image)[i*3+0] = image->comps[0].data[i];
            ((int32_t*)*p_image)[i*3+1] = image->comps[1].data[i];
            ((int32_t*)*p_image)[i*3+2] = image->comps[2].data[i];
        }
    }

    grk_stream_destroy(l_stream);
    grk_destroy_codec(l_codec);
    grk_image_destroy(image);
    grk_deinitialize();
    return 0;
}
