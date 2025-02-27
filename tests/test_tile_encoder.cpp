/*
*    Copyright (C) 2016-2019 Grok Image Compression Inc.
*
*    This source code is free software: you can redistribute it and/or  modify
*    it under the terms of the GNU Affero General Public License, version 3,
*    as published by the Free Software Foundation.
*
*    This source code is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*
*    This source code incorporates work covered by the following copyright and
*    permission notice:
*
 * Copyright (c) 2008, Jerome Fimes, Communications & Systemes <jerome.fimes@c-s.fr>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "grk_config.h"
#include "grok.h"
#include "stdlib.h"
#include <stdbool.h>
#include "spdlog/spdlog.h"

/* -------------------------------------------------------------------------- */

/**
sample error debug callback expecting no client object
*/
static void error_callback(const char *msg, void *client_data)
{
    (void)client_data;
    spdlog::error("%s", msg);
}
/**
sample warning debug callback expecting no client object
*/
static void warning_callback(const char *msg, void *client_data)
{
    (void)client_data;
    spdlog::warn("%s", msg);
}
/**
sample debug callback expecting no client object
*/
static void info_callback(const char *msg, void *client_data)
{
    (void)client_data;
    spdlog::info("%s", msg);
}

/* -------------------------------------------------------------------------- */

#define NUM_COMPS_MAX 4
int main (int argc, char *argv[])
{
    grk_cparameters_t l_param;
    grk_codec_t * l_codec=nullptr;
    grk_image_t * l_image = nullptr;
    grk_image_cmptparm_t l_params [NUM_COMPS_MAX];
    grk_stream_t * l_stream = nullptr;
    uint32_t l_nb_tiles=0;
    uint64_t l_data_size=0;
    size_t len=0;
	int rc = 0;

#ifdef USING_MCT
    const float l_mct [] = {
        1 , 0 , 0 ,
        0 , 1 , 0 ,
        0 , 0 , 1
    };

    const int32_t l_offsets [] = {
        128 , 128 , 128
    };
#endif

    grk_image_cmptparm_t * l_current_param_ptr=nullptr;
    uint32_t i;
    uint8_t *l_data=nullptr;

    uint32_t num_comps;
	uint32_t image_width;
	uint32_t image_height;
	uint32_t tile_width;
	uint32_t tile_height;
	uint32_t comp_prec;
	uint32_t irreversible;
    const char* output_file;

    grk_initialize(nullptr,0);

    /* should be test_tile_encoder 3 2000 2000 1000 1000 8 tte1.j2k */
    if( argc == 9 ) {
        num_comps = atoi( argv[1] );
        image_width = atoi( argv[2] );
        image_height = atoi( argv[3] );
        tile_width = atoi( argv[4] );
        tile_height = atoi( argv[5] );
        comp_prec = atoi( argv[6] );
        irreversible = atoi( argv[7] );
        output_file = argv[8];
    } else {
        num_comps = 3;
        image_width = 2000;
        image_height = 2000;
        tile_width = 1000;
        tile_height = 1000;
        comp_prec = 8;
        irreversible = 1;
        output_file = "test.j2k";
    }
    if( num_comps > NUM_COMPS_MAX ) {
		rc = 1;
		goto cleanup;
    }
    l_nb_tiles = (image_width/tile_width) * (image_height/tile_height);
    l_data_size = (uint64_t)tile_width * tile_height * num_comps * (comp_prec/8);
	if (!l_data_size) {
		rc = 1;
		goto cleanup;
	}
    l_data = (uint8_t*) malloc(l_data_size * sizeof(uint8_t));
	if (!l_data) {
		rc = 1;
		goto cleanup;
	}

    fprintf(stdout, "Encoding random values -> keep in mind that this is very hard to compress\n");
    for (i=0; i<l_data_size; ++i)	{
        l_data[i] = (uint8_t)i; /*rand();*/
    }

    grk_set_default_encoder_parameters(&l_param);
    /** you may here add custom encoding parameters */
    /* rate specifications */
    /** number of quality layers in the stream */
    l_param.tcp_numlayers = 1;
    l_param.cp_fixed_quality = 1;
    l_param.tcp_distoratio[0] = 20;
    /* is using others way of calculation */
    /* l_param.cp_disto_alloc = 1 or l_param.cp_fixed_alloc = 1 */
    /* l_param.tcp_rates[0] = ... */


    /* tile definitions parameters */
    /* position of the tile grid aligned with the image */
    l_param.cp_tx0 = 0;
    l_param.cp_ty0 = 0;
    /* tile size, we are using tile based encoding */
    l_param.tile_size_on = true;
    l_param.cp_tdx = tile_width;
    l_param.cp_tdy = tile_height;

    /* use irreversible encoding ?*/
    l_param.irreversible = irreversible;

    /* do not bother with mct, the rsiz is set when calling grk_set_MCT*/
    /*l_param.cp_rsiz = GRK_STD_RSIZ;*/

    /* no cinema */
    /*l_param.cp_cinema = 0;*/

    /* do not bother using SOP or EPH markers, do not use custom size precinct */
    /* number of precincts to specify */
    /* l_param.csty = 0;*/
    /* l_param.res_spec = ... */
    /* l_param.prch_init[i] = .. */
    /* l_param.prcw_init[i] = .. */


    /* do not use progression order changes */
    /*l_param.numpocs = 0;*/
    /* l_param.POC[i].... */

    /* do not restrain the size for a component.*/
    /* l_param.max_comp_size = 0; */

    /** block encoding style for each component, do not use at the moment */
    /** J2K_CCP_CBLKSTY_TERMALL, J2K_CCP_CBLKSTY_LAZY, J2K_CCP_CBLKSTY_VSC, J2K_CCP_CBLKSTY_SEGSYM, J2K_CCP_CBLKSTY_RESET */
    /* l_param.mode = 0;*/

    /** number of resolutions */
    l_param.numresolution = 6;

    /** progression order to use*/
    /** GRK_LRCP, GRK_RLCP, GRK_RPCL, PCRL, CPRL */
    l_param.prog_order = GRK_LRCP;

    /** no "region" of interest, more precisely component */
    /* l_param.roi_compno = -1; */
    /* l_param.roi_shift = 0; */

    /* we are not using multiple tile parts for a tile. */
    /* l_param.tp_on = 0; */
    /* l_param.tp_flag = 0; */

    /* if we are using mct */
#ifdef USING_MCT
    grk_set_MCT(&l_param,l_mct,l_offsets,NUM_COMPS);
#endif


    /* image definition */
    l_current_param_ptr = l_params;
    for (i=0; i<num_comps; ++i) {
        /* do not bother bpp useless */
        /*l_current_param_ptr->bpp = COMP_PREC;*/
        l_current_param_ptr->dx = 1;
        l_current_param_ptr->dy = 1;

        l_current_param_ptr->h = image_height;
        l_current_param_ptr->w = image_width;

        l_current_param_ptr->sgnd = 0;
        l_current_param_ptr->prec = comp_prec;

        l_current_param_ptr->x0 = 0;
        l_current_param_ptr->y0 = 0;

        ++l_current_param_ptr;
    }

    /* should we do j2k or jp2 ?*/
    len = strlen( output_file );
    if( strcmp( output_file + len - 4, ".jp2" ) == 0 ) {
        l_codec = grk_create_compress(GRK_CODEC_JP2);
    } else {
        l_codec = grk_create_compress(GRK_CODEC_J2K);
    }
    if (!l_codec) {
		rc = 1;
		goto cleanup;
    }

    /* catch events using our callbacks and give a local context */
    grk_set_info_handler(l_codec, info_callback,nullptr);
    grk_set_warning_handler(l_codec, warning_callback,nullptr);
    grk_set_error_handler(l_codec, error_callback,nullptr);

    l_image = grk_image_tile_create(num_comps,l_params,GRK_CLRSPC_SRGB);
    if (! l_image) {
		rc = 1;
		goto cleanup;
    }

    l_image->x0 = 0;
    l_image->y0 = 0;
    l_image->x1 = image_width;
    l_image->y1 = image_height;
    l_image->color_space = GRK_CLRSPC_SRGB;

    if (! grk_setup_encoder(l_codec,&l_param,l_image)) {
        spdlog::error("test_tile_encoder: failed to setup the codec!\n");
		rc = 1;
		goto cleanup;
    }

    l_stream = grk_stream_create_default_file_stream(output_file, false);
    if (! l_stream) {
        spdlog::error("test_tile_encoder: failed to create the stream from the output file %s !\n",output_file );
		rc = 1;
		goto cleanup;
    }

    if (! grk_start_compress(l_codec,l_image,l_stream)) {
        spdlog::error("test_tile_encoder: failed to start compress!\n");
		rc = 1;
		goto cleanup;
    }

    for (i=0; i<l_nb_tiles; ++i) {
        if (! grk_write_tile(l_codec,i,l_data,l_data_size,l_stream)) {
            spdlog::error("test_tile_encoder: failed to write the tile %d!\n",i);
			rc = 1;
			goto cleanup;
        }
    }

    if (! grk_end_compress(l_codec,l_stream)) {
        spdlog::error("test_tile_encoder: failed to end compress!\n");
		rc = 1;
		goto cleanup;
    }

cleanup:
	if (l_stream)
		grk_stream_destroy(l_stream);
	if (l_codec)
		grk_destroy_codec(l_codec);
	if (l_image)
		grk_image_destroy(l_image);

	if (l_data)
		free(l_data);

    /* Print profiling*/
    /*PROFPRINT();*/
	grk_deinitialize();

    return rc;
}






