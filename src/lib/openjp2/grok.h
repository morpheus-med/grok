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
 * The copyright in this software is being made available under the 2-clauses
 * BSD License, included below. This software may be subject to other third
 * party and contributor rights, including patent rights, and no such rights
 * are granted under this license.
 *
 * Copyright (c) 2002-2014, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2014, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux
 * Copyright (c) 2003-2014, Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * Copyright (c) 2006-2007, Parvatha Elangovan
 * Copyright (c) 2008, Jerome Fimes, Communications & Systemes <jerome.fimes@c-s.fr>
 * Copyright (c) 2010-2011, Kaori Hagihara
 * Copyright (c) 2011-2012, Centre National d'Etudes Spatiales (CNES), France
 * Copyright (c) 2012, CS Systemes d'Information, France
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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 ==========================================================
 Compiler directives
 ==========================================================
 */

/* deprecated attribute */
#ifdef __GNUC__
#define GRK_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define GRK_DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define GRK_DEPRECATED(func) func
#endif

#if defined(GRK_STATIC) || !defined(_WIN32)
/* http://gcc.gnu.org/wiki/Visibility */
#	if __GNUC__ >= 4
#		if defined(GRK_STATIC) /* static library uses "hidden" */
#			define GRK_API    __attribute__ ((visibility ("hidden")))
#		else
#			define GRK_API    __attribute__ ((visibility ("default")))
#		endif
#		define GRK_LOCAL  __attribute__ ((visibility ("hidden")))
#	else
#		define GRK_API
#		define GRK_LOCAL
#	endif
#	define GRK_CALLCONV
#else
#	define GRK_CALLCONV __stdcall
/*
The following ifdef block is the standard way of creating macros which make exporting
from a DLL simpler. All files within this DLL are compiled with the GRK_EXPORTS
symbol defined on the command line. this symbol should not be defined on any project
that uses this DLL. This way any other project whose source files include this file see
GRK_API functions as being imported from a DLL, whereas this DLL sees symbols
defined with this macro as being exported.
*/
#	if defined(GRK_EXPORTS) || defined(DLL_EXPORT)
#		define GRK_API __declspec(dllexport)
#	else
#		define GRK_API __declspec(dllimport)
#	endif /* GRK_EXPORTS */
#endif /* !GRK_STATIC || !_WIN32 */

#include "grk_config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define GRK_PATH_LEN 4096 /**< Maximum allowed size for filenames */
// note: range for number of decomposition levels is 0-32
// So, accordingly, range for number of resolutions is 1-33
#define GRK_J2K_MAXRLVLS 33					/**< Maximum number of resolution levels authorized */
#define GRK_J2K_MAXBANDS (3*GRK_J2K_MAXRLVLS-2)	/**< Maximum number of sub-bands */

// used by dump
#define GRK_IMG_INFO		1	/**< Basic image information provided to the user */
#define GRK_J2K_MH_INFO		2	/**< Codestream information based only on the main header */
#define GRK_J2K_TH_INFO		4	/**< Tile information based on the current tile header */
#define GRK_J2K_TCH_INFO	8	/**< Tile/Component information of all tiles */
#define GRK_J2K_MH_IND		16	/**< Codestream index based only on the main header */
#define GRK_J2K_TH_IND		32	/**< Tile index based on the current tile */
#define GRK_JP2_INFO		128	/**< JP2 file information */
#define GRK_JP2_IND			256	/**< JP2 file index */

/**
 * JPEG 2000 Profiles, see Table A.10 from 15444-1 (updated in various AMD)
 *
 * These values help choose the RSIZ value for the J2K codestream.
 * The RSIZ value forces various encoding options, as detailed in Table A.10.
 * If GRK_PROFILE_PART2 is chosen, it has to be combined with one or more extensions
 * described below.
 *   Example: rsiz = GRK_PROFILE_PART2 | GRK_EXTENSION_MCT;
 *
 * For broadcast profiles, the GRK_PROFILE value has to be combined with the target
 * level (3-0 LSB, value between 0 and 11):
 *   Example: rsiz = GRK_PROFILE_BC_MULTI | 0x0005; //level equals 5
 *
 * For IMF profiles, the GRK_PROFILE value has to be combined with the target main-level
 * (3-0 LSB, value between 0 and 11) and sub-level (7-4 LSB, value between 0 and 9):
 *   Example: rsiz = GRK_PROFILE_IMF_2K | 0x0040 | 0x0005; // main-level equals 5 and sub-level equals 4
 *
 * *********************************************
 * Broadcast level (3-0 LSB) (15444-1 AMD4,AMD8)
 * *********************************************
 *
 * indicates maximum bit rate and sample rate for a code stream
 *
 * Note: Mbit/s == 10^6 bits/s;  Msamples/s == 10^6 samples/s
 *
 * 0:		no maximum rate
 * 1:		200 Mbits/s, 65  Msamples/s
 * 2:		200 Mbits/s, 130 Msamples/s
 * 3:		200 Mbits/s, 195 Msamples/s
 * 4:		400 Mbits/s, 260 Msamples/s
 * 5:		800Mbits/s,  520 Msamples/s
 * >= 6:	2^(level-6) * 1600 Mbits/s, 2^(level-6) * 1200 Msamples/s
 * 
 * Note: level cannot be greater than 11
 *
 * ****************
 * Broadcast tiling
 * ****************
 *
 * Either single-tile or multi-tile. Multi-tile only permits
 * 1 or 4 tiles per frame, where multiple tiles have identical 
 * sizes, and are configured in either 2x2 or 1x4 layout.
 * 
 *************************************************************
 *
 * ***************************************
 * IMF main-level (3-0) LSB (15444-1 AMD8)
 * ***************************************
 *
 * main-level indicates maximum number of samples per second,
 * as listed above.
 *
 *
 * **************************************
 * IMF sub-level (7-4) LSB (15444-1 AMD8)
 * **************************************
 *
 * sub-level indicates maximum bit rate for a code stream:
 *
 * 0:	no maximum rate
 * >0:	2^sub-level * 100 Mbits/second
 *
 * Note: sub-level cannot be greater than 9, and cannot be larger 
 * then maximum of (main-level -2) and 1.
 *
 * 
 * */
#define GRK_PROFILE_NONE        0x0000 /** no profile, conform to 15444-1 */
#define GRK_PROFILE_0           0x0001 /** Profile 0 as described in 15444-1,Table A.45 */
#define GRK_PROFILE_1           0x0002 /** Profile 1 as described in 15444-1,Table A.45 */
#define GRK_PROFILE_CINEMA_2K   0x0003 /** 2K cinema profile defined in 15444-1 AMD1 */
#define GRK_PROFILE_CINEMA_4K   0x0004 /** 4K cinema profile defined in 15444-1 AMD1 */
#define GRK_PROFILE_CINEMA_S2K  0x0005 /** Scalable 2K cinema profile defined in 15444-1 AMD2 */
#define GRK_PROFILE_CINEMA_S4K  0x0006 /** Scalable 4K cinema profile defined in 15444-1 AMD2 */
#define GRK_PROFILE_CINEMA_LTS  0x0007 /** Long term storage cinema profile defined in 15444-1 AMD2 */
#define GRK_PROFILE_BC_SINGLE   0x0100 /** Single Tile Broadcast profile defined in 15444-1 AMD3 */
#define GRK_PROFILE_BC_MULTI    0x0200 /** Multi Tile Broadcast profile defined in 15444-1 AMD3 */
#define GRK_PROFILE_BC_MULTI_R  0x0300 /** Multi Tile Reversible Broadcast profile defined in 15444-1 AMD3 */
#define GRK_PROFILE_BC_MASK		0x0F0F /** Mask for broadcast profile including main level */
#define GRK_PROFILE_IMF_2K      0x0400 /** 2K Single Tile Lossy IMF profile defined in 15444-1 AMD8 */
#define GRK_PROFILE_IMF_4K      0x0401 /** 4K Single Tile Lossy IMF profile defined in 15444-1 AMD8 */
#define GRK_PROFILE_IMF_8K      0x0402 /** 8K Single Tile Lossy IMF profile defined in 15444-1 AMD8 */
#define GRK_PROFILE_IMF_2K_R    0x0403 /** 2K Single/Multi Tile Reversible IMF profile defined in 15444-1 AMD8 */
#define GRK_PROFILE_IMF_4K_R    0x0800 /** 4K Single/Multi Tile Reversible IMF profile defined in 15444-1 AMD8 */
#define GRK_PROFILE_IMF_8K_R    0x0801  /** 8K Single/Multi Tile Reversible IMF profile defined in 15444-1 AMD8 */
#define GRK_PROFILE_MASK		0xBFFF  /** Mask for profile bits */

#define GRK_PROFILE_PART2						0x8000 /** At least 1 extension defined in 15444-2 (Part-2) */
#define GRK_PROFILE_PART2_EXTENSIONS_MASK       0x3FFF // Mask for Part-2 extension bits
/**
 * JPEG 2000 Part-2 extensions
 * */

#define GRK_EXTENSION_NONE      0x0000 /** No Part-2 extension */
#define GRK_EXTENSION_MCT       0x0100  /** Custom MCT support */

/**
 * JPEG 2000 profile macros
 * */
#define GRK_IS_CINEMA(v)     (((v) >= GRK_PROFILE_CINEMA_2K) && ((v) <= GRK_PROFILE_CINEMA_S4K))
#define GRK_IS_STORAGE(v)    ((v) == GRK_PROFILE_CINEMA_LTS)
#define GRK_IS_BROADCAST(v)  (((v) >= GRK_PROFILE_BC_SINGLE) && ((v) <= ((GRK_PROFILE_BC_MULTI_R) | (0x000b))) && (((v) & (~GRK_PROFILE_BC_MASK)) == 0))
#define GRK_IS_IMF(v)        (((v) >= GRK_PROFILE_IMF_2K) && ((v) <= ((GRK_PROFILE_IMF_8K_R) | (0x009b))))
#define GRK_IS_PART2(v)      ((v) & GRK_PROFILE_PART2)

/**
 * JPEG 2000 codestream and component size limits on cinema profiles
 * */
#define GRK_CINEMA_24_CS     1302083   	/** Maximum codestream length @ 24fps */
#define GRK_CINEMA_48_CS     651041     /** Maximum codestream length @ 48fps */
#define GRK_CINEMA_24_COMP   1041666    /** Maximum size per color component @ 24fps */
#define GRK_CINEMA_48_COMP   520833		/** Maximum size per color component @ 48fps */

/**
 * Progression order
 * */
typedef enum PROG_ORDER {
	GRK_PROG_UNKNOWN = -1, /**< place-holder */
	GRK_LRCP = 0, /**< layer-resolution-component-precinct order */
	GRK_RLCP = 1, /**< resolution-layer-component-precinct order */
	GRK_RPCL = 2, /**< resolution-precinct-component-layer order */
	GRK_PCRL = 3, /**< precinct-component-resolution-layer order */
	GRK_CPRL = 4 /**< component-precinct-resolution-layer order */
} GRK_PROG_ORDER;

/**
 * Supported image color spaces
 */

#define GRK_CUSTOM_CIELAB_SPACE 0x0
#define GRK_DEFAULT_CIELAB_SPACE 0x44454600  //'DEF'
#define GRK_CIE_DAY   ((((uint32_t) 'C')<<24) + (((uint32_t) 'T')<<16))
#define GRK_CIE_D50   ((uint32_t) 0x00443530)
#define GRK_CIE_D65   ((uint32_t) 0x00443635)
#define GRK_CIE_D75   ((uint32_t) 0x00443735)
#define GRK_CIE_SA   ((uint32_t) 0x00005341)
#define GRK_CIE_SC   ((uint32_t) 0x00005343)
#define GRK_CIE_F2   ((uint32_t) 0x00004632)
#define GRK_CIE_F7   ((uint32_t) 0x00004637)
#define GRK_CIE_F11   ((uint32_t) 0x00463131)

typedef enum COLOR_SPACE {
	GRK_CLRSPC_UNKNOWN = 0, /**< not supported by the library */
	GRK_CLRSPC_UNSPECIFIED = 1, /**< not specified in the codestream */
	GRK_CLRSPC_SRGB = 2, /**< sRGB */
	GRK_CLRSPC_GRAY = 3, /**< grayscale */
	GRK_CLRSPC_SYCC = 4, /**< YUV */
	GRK_CLRSPC_EYCC = 5, /**< e-YCC */
	GRK_CLRSPC_CMYK = 6, /**< CMYK */
	GRK_CLRSPC_DEFAULT_CIE = 7, /**< default CIE LAB */
	GRK_CLRSPC_CUSTOM_CIE = 8, /**< custom CIE LAB */
	GRK_CLRSPC_ICC = 9 /**< ICC profile */
} GRK_COLOR_SPACE;

/**
 * Supported codec
 */
typedef enum CODEC_FORMAT {
	GRK_CODEC_UNKNOWN = -1, /**< place-holder */
	GRK_CODEC_J2K = 0, /**< JPEG-2000 codestream : read/write */
	GRK_CODEC_JP2 = 2 /**< JP2 file format : read/write */
} GRK_CODEC_FORMAT;

#define  GRK_NUM_COMMENTS_SUPPORTED 256
#define GRK_MAX_COMMENT_LENGTH (UINT16_MAX-2)

/*
 ==========================================================
 logger typedef definitions
 ==========================================================
 */

/**
 * Callback function prototype for events
 * @param msg               Event message
 * @param client_data       Client object where will be return the event message
 * */
typedef void (*grk_msg_callback)(const char *msg, void *client_data);

/*
 ==========================================================
 codec typedef definitions
 ==========================================================
 */

/**
 * Progression order changes
 *
 */
typedef struct grk_poc {
	/** Resolution num start, Component num start, given by POC */
	uint32_t resno0, compno0;
	/** Layer num end,Resolution num end, Component num end, given by POC */
	uint32_t layno1, resno1, compno1;
	/** Layer num start,Precinct num start, Precinct num end */
	uint32_t layno0, precno0, precno1;
	/** Progression order enum*/
	GRK_PROG_ORDER prg1, prg;
	/** Progression order string*/
	char progorder[5];
	/** Tile number */
	uint32_t tile;
	/** Start and end values for Tile width and height*/
	uint32_t tx0, tx1, ty0, ty1;
	/** Start value, initialised in pi_initialise_encode*/
	uint32_t layS, resS, compS, prcS;
	/** End value, initialised in pi_initialise_encode */
	uint32_t layE, resE, compE, prcE;
	/** Start and end values of Tile width and height, initialised in pi_initialise_encode*/
	uint32_t txS, txE, tyS, tyE, dx, dy;
	/** Temporary values for Tile parts, initialised in pi_create_encode */
	uint32_t lay_t, res_t, comp_t, prc_t, tx0_t, ty0_t;
} grk_poc_t;

/**@name RAW component encoding parameters */
/*@{*/
typedef struct raw_comp_cparameters {
	/** subsampling in X direction */
	uint32_t dx;
	/** subsampling in Y direction */
	uint32_t dy;
	/*@}*/
} raw_comp_cparameters_t;

/**@name RAW image encoding parameters */
/*@{*/
typedef struct raw_cparameters {
	/** width of the raw image */
	uint32_t width;
	/** height of the raw image */
	uint32_t height;
	/** number of components of the raw image */
	uint32_t numcomps;
	/** bit depth of the raw image */
	uint32_t prec;
	/** signed/unsigned raw image */
	bool sgnd;
	/** raw components parameters */
	raw_comp_cparameters_t *comps;
	/*@}*/
} raw_cparameters_t;

/**
 * Compression parameters
 * */
typedef struct grk_cparameters {
	/** size of tile: tile_size_on = false (not in argument) or = true (in argument) */
	bool tile_size_on;
	/** XTOsiz */
	uint32_t cp_tx0;
	/** YTOsiz */
	uint32_t cp_ty0;
	/** XTsiz */
	uint32_t cp_tdx;
	/** YTsiz */
	uint32_t cp_tdy;
	/** allocation by rate/distortion */
	uint32_t cp_disto_alloc;
	/** allocation by fixed_quality */
	uint32_t cp_fixed_quality;
	/** comment for coding */
	char *cp_comment[GRK_NUM_COMMENTS_SUPPORTED];
	uint16_t cp_comment_len[GRK_NUM_COMMENTS_SUPPORTED];
	bool cp_is_binary_comment[GRK_NUM_COMMENTS_SUPPORTED];
	size_t cp_num_comments;
	/** csty : coding style */
	uint32_t csty;
	/** progression order (default GRK_LRCP) */
	GRK_PROG_ORDER prog_order;
	/** progression order changes */
	grk_poc_t POC[32];
	/** number of progression order changes (POC), default to 0 */
	uint32_t numpocs;
	/** number of layers */
	uint32_t tcp_numlayers;
	/** rates of layers, expressed as compression ratios. They might be subsequently limited by the max_cs_size field */
	double tcp_rates[100];
	/** different psnr for successive layers */
	double tcp_distoratio[100];
	/** number of resolutions */
	uint32_t numresolution;
	/** initial code block width, default to 64 */
	uint32_t cblockw_init;
	/** initial code block height, default to 64 */
	uint32_t cblockh_init;
	/** mode switch */
	uint32_t mode;
	/** 1 : use the irreversible DWT 9-7, 0 : use lossless compression (default) */
	uint32_t irreversible;
	/** region of interest: affected component in [0..3]; -1 indicates no ROI */
	int32_t roi_compno;
	/** region of interest: upshift value */
	uint32_t roi_shift;
	/* number of precinct size specifications */
	uint32_t res_spec;
	/** initial precinct width */
	uint32_t prcw_init[GRK_J2K_MAXRLVLS];
	/** initial precinct height */
	uint32_t prch_init[GRK_J2K_MAXRLVLS];
	/**@name command line encoder parameters (not used inside the library) */
	/*@{*/
	/** input file name */
	char infile[GRK_PATH_LEN];
	/** output file name */
	char outfile[GRK_PATH_LEN];
	/** subimage encoding: origin image offset in x direction */
	uint32_t image_offset_x0;
	/** subimage encoding: origin image offset in y direction */
	uint32_t image_offset_y0;
	/** subsampling value for dx */
	uint32_t subsampling_dx;
	/** subsampling value for dy */
	uint32_t subsampling_dy;
	/** input file format*/
	uint32_t decod_format;
	/** output file format*/
	uint32_t cod_format;
	raw_cparameters_t raw_cp;
	/**
	 * Maximum size (in bytes) for each component.
	 * If == 0, component size limitation is not considered
	 * */
	uint32_t max_comp_size;
	/** Tile part generation*/
	uint8_t tp_on;
	/** Flag for Tile part generation*/
	uint8_t tp_flag;
	/** MCT (multiple component transform) */
	uint8_t tcp_mct;
	/** Naive implementation of MCT restricted to a single reversible array based
	 encoding without offset concerning all the components. */
	void *mct_data;
	/**
	 * Maximum size (in bytes) for the whole codestream.
	 * If == 0, codestream size limitation is not considered
	 * If it does not comply with tcp_rates, max_cs_size prevails
	 * and a warning is issued.
	 * */
	uint64_t max_cs_size;
	/** RSIZ value
	 To be used to combine GRK_PROFILE_*, GRK_EXTENSION_* and (sub)levels values. */
	uint16_t rsiz;

	// set to true if input file stores capture resolution
	bool write_capture_resolution_from_file;
	double capture_resolution_from_file[2];

	bool write_capture_resolution;
	double capture_resolution[2];

	bool write_display_resolution;
	double display_resolution[2];

	uint32_t rateControlAlgorithm; // 0: bisect with all truncation points,  1: bisect with only feasible truncation points
	uint32_t numThreads;
	int32_t deviceId;
	uint32_t duration; //seconds
	uint32_t kernelBuildOptions;
	uint32_t repeats;
	bool verbose;
} grk_cparameters_t;

/**
 Channel description: channel index, type, association
 */
typedef struct jp2_cdef_info {
	uint16_t cn;
	uint16_t typ;
	uint16_t asoc;
} jp2_cdef_info_t;

/**
 Channel descriptions and number of descriptions
 */
typedef struct jp2_cdef {
	jp2_cdef_info_t *info;
	uint16_t n;
} jp2_cdef_t;

/**
 Component mappings: channel index, mapping type, palette index
 */
typedef struct jp2_cmap_comp {
	uint16_t cmp;
	uint8_t mtyp, pcol;
} jp2_cmap_comp_t;

/**
 Palette data: table entries, palette columns
 */
typedef struct jp2_pclr {
	uint32_t *entries;
	uint8_t *channel_sign;
	uint8_t *channel_size;
	jp2_cmap_comp_t *cmap;
	uint16_t nr_entries;
	uint8_t nr_channels;
} jp2_pclr_t;

/**
 Struct for ICC profile, palette, component mapping, channel description
 */
typedef struct jp2_color {
	uint8_t *icc_profile_buf;
	uint32_t icc_profile_len;
	jp2_cdef_t *jp2_cdef;
	jp2_pclr_t *jp2_pclr;
	uint8_t jp2_has_colour_specification_box;
} jp2_color_t;

typedef struct grk_header_info {
	/** initial code block width, default to 64 */
	uint32_t cblockw_init;
	/** initial code block height, default to 64 */
	uint32_t cblockh_init;
	/** 1 : use the irreversible DWT 9-7, 0 : use lossless compression (default) */
	uint32_t irreversible;
	/** multi-component transform identifier */
	uint32_t mct;
	/** RSIZ value
	 To be used to combine GRK_PROFILE_*, GRK_EXTENSION_* and (sub)levels values. */
	uint16_t rsiz;
	/** number of resolutions */
	uint32_t numresolutions;
	// coding style can be specified in main header COD segment,
	// tile header COD segment, and tile component COC segment.
	// !!! Assume that coding style does not vary across tile components !!!
	uint32_t csty;
	// mode switch is specified in main header COD segment, and can
	// be overridden in a tile header. !!! Assume that mode does
	// not vary across tiles !!!
	uint32_t mode_switch;
	/** initial precinct width */
	uint32_t prcw_init[GRK_J2K_MAXRLVLS];
	/** initial precinct height */
	uint32_t prch_init[GRK_J2K_MAXRLVLS];
	/** XTOsiz */
	uint32_t cp_tx0;
	/** YTOsiz */
	uint32_t cp_ty0;
	/** XTsiz */
	uint32_t cp_tdx;
	/** YTsiz */
	uint32_t cp_tdy;
	/** number of tiles in width */
	uint32_t cp_tw;
	/** number of tiles in height */
	uint32_t cp_th;
	/** number of layers */
	uint32_t tcp_numlayers;
	/*
	 Colour space enumeration:
	 12	CMYK
	 14  CIE LAB
	 16	sRGB
	 17	Grayscale
	 18	SYCC
	 19	EYCC
	 */
	uint32_t enumcs;
	// icc profile information etc.
	// note: the contents of this struct will remain valid
	// until the codec is destroyed
	jp2_color_t color;
	// note: xml_data will remain valid
	// until codec is destroyed
	uint8_t *xml_data;
	size_t xml_data_len;
	size_t num_comments;
	char *comment[GRK_NUM_COMMENTS_SUPPORTED];
	uint16_t comment_len[GRK_NUM_COMMENTS_SUPPORTED];
	bool isBinaryComment[GRK_NUM_COMMENTS_SUPPORTED];

	bool has_capture_resolution;
	double capture_resolution[2];

	bool has_display_resolution;
	double display_resolution[2];

} grk_header_info_t;

/**
 * Decompression parameters
 * */
typedef struct grk_dparameters {
	/**
	 Set the number of highest resolution levels to be discarded.
	 The image resolution is effectively divided by 2 to the power of the number of discarded levels.
	 The reduce factor is limited by the smallest total number of decomposition levels among tiles.
	 if != 0, then original dimension divided by 2^(reduce);
	 if == 0 or not used, image is decoded to the full resolution
	 */
	uint32_t cp_reduce;
	/**
	 Set the maximum number of quality layers to decode.
	 If there are less quality layers than the specified number, all the quality layers are decoded.
	 if != 0, then only the first "layer" layers are decoded;
	 if == 0 or not used, all the quality layers are decoded
	 */
	uint32_t cp_layer;
	/**@name command line decoder parameters (not used inside the library) */
	/*@{*/
	/** input file name */
	char infile[GRK_PATH_LEN];
	/** output file name */
	char outfile[GRK_PATH_LEN];
	/** input file format*/
	uint32_t decod_format;
	/** output file format*/
	uint32_t cod_format;
	/** Decoding area left boundary */
	uint32_t DA_x0;
	/** Decoding area right boundary */
	uint32_t DA_x1;
	/** Decoding area up boundary */
	uint32_t DA_y0;
	/** Decoding area bottom boundary */
	uint32_t DA_y1;
	/** Verbose mode */
	bool m_verbose;
	/** tile number of the decoded tile*/
	uint32_t tile_index;
	/** Nb of tile to decode */
	uint32_t nb_tile_to_decode;
	uint32_t flags;
	uint32_t numThreads;
} grk_dparameters_t;

typedef enum grk_prec_mode {
	GRK_PREC_MODE_CLIP, GRK_PREC_MODE_SCALE
} grk_precision_mode;

typedef struct grk_prec {
	uint32_t prec;
	grk_precision_mode mode;
} grk_precision;

#define DECOMPRESS_COMPRESSION_LEVEL_DEFAULT (-65535)

typedef struct grk_decompress_params {
	/** core library parameters */
	grk_dparameters_t core;
	/** input file name */
	char infile[GRK_PATH_LEN];
	/** output file name */
	char outfile[GRK_PATH_LEN];
	/** input file format 0: J2K, 1: JP2*/
	uint32_t decod_format;
	/** output file format 0: PGX, 1: PxM, 2: BMP */
	uint32_t cod_format;
	/** index file name */
	char indexfilename[GRK_PATH_LEN];
	/** Decoding area left boundary */
	uint32_t DA_x0;
	/** Decoding area right boundary */
	uint32_t DA_x1;
	/** Decoding area up boundary */
	uint32_t DA_y0;
	/** Decoding area bottom boundary */
	uint32_t DA_y1;
	/** Verbose mode */
	bool m_verbose;
	/** tile number of the decoded tile*/
	uint32_t tile_index;
	/** Nb of tile to decode */
	uint32_t nb_tile_to_decode;
	grk_precision *precision;
	uint32_t nb_precision;
	/* force output colorspace to RGB */
	bool force_rgb;
	/* upsample components according to their dx/dy values */
	bool upsample;
	/* split output components to different files */
	bool split_pnm;
	/* serialize xml metadata to disk */
	bool serialize_xml;
	uint32_t compression;
	int32_t compressionLevel; // compression "quality". Meaning of "quality" depends on file format we are writing to
	int32_t deviceId;
	uint32_t duration; //seconds
	uint32_t kernelBuildOptions;
	uint32_t repeats;
	bool verbose;
} grk_decompress_parameters;

typedef void *grk_codec_t;

/*
 ==========================================================
 I/O stream typedef definitions
 ==========================================================
 */

/**
 * Stream open flags.
 * */
/** The stream was opened for reading. */
#define GRK_STREAM_READ	true
/** The stream was opened for writing. */
#define GRK_STREAM_WRITE false

/*
 * Callback function prototype for read function
 */
typedef size_t (*grk_stream_read_fn)(void *p_buffer, size_t nb_bytes,
		void *p_user_data);

/*
 * Callback function prototype for zero copy read function
 */
typedef size_t (*grk_stream_zero_copy_read_fn)(void **p_buffer,
		size_t nb_bytes, void *p_user_data);

/*
 * Callback function prototype for write function
 */
typedef size_t (*grk_stream_write_fn)(void *p_buffer, size_t nb_bytes,
		void *p_user_data);

/*
 * Callback function prototype for (absolute) seek function.
 */
typedef bool (*grk_stream_seek_fn)(uint64_t nb_bytes, void *p_user_data);

/*
 * Callback function prototype for free user data function
 */
typedef void (*grk_stream_free_user_data_fn)(void *p_user_data);

/*
 * JPEG2000 Stream.
 */
typedef void *grk_stream_t;

/*
 ==========================================================
 image typedef definitions
 ==========================================================
 */

#define GROK_COMPONENT_TYPE_NON_OPACITY	 			0
#define GROK_COMPONENT_TYPE_OPACITY 				1
#define GROK_COMPONENT_TYPE_PREMULTIPLIED_OPACITY 	2

/**
 * Defines a single image component
 * */
typedef struct grk_image_comp {
	/** XRsiz: horizontal separation of a sample of with component with respect to the reference grid */
	uint32_t dx;
	/** YRsiz: vertical separation of a sample of with component with respect to the reference grid */
	uint32_t dy;
	/** data width */
	uint32_t w;
	/** data height */
	uint32_t h;
	/** x component offset compared to the whole image */
	uint32_t x0;
	/** y component offset compared to the whole image */
	uint32_t y0;
	/** precision */
	uint32_t prec;
	/** signed (1) / unsigned (0) */
	uint32_t sgnd;
	/** number of decoded resolution */
	uint32_t resno_decoded;
	/** number of division by 2 of the output image compared to the original size of image */
	uint32_t decodeScaleFactor;
	/** image component data */
	int32_t *data;
	// if true, then image will manage data, otherwise up to caller
	bool owns_data;
	/** alpha channel: can be one of three values: {GROK_COMPONENT_TYPE_NON_OPACITY, GROK_COMPONENT_TYPE_OPACITY, GROK_COMPONENT_TYPE_PREMULTIPLIED_OPACITY}
	 *
	 * GROK_COMPONENT_TYPE_NON_OPACITY				: this component is not an alpha channel
	 * GROK_COMPONENT_TYPE_PREMULTIPLIED_OPACITY 	: this component is an alpha channel, and the colour channels have been pre-multiplied by alpha
	 * GROK_COMPONENT_TYPE_OPACITY					: this component is an alpha channel, and the colour channels have not been pre-multiplied by alpha
	 * */
	uint16_t alpha;
} grk_image_comp_t;

/**
 * Defines image data and characteristics
 * */
typedef struct grk_image {
	/** XOsiz: horizontal offset from the origin of the reference grid to the left side of the image area */
	uint32_t x0;
	/** YOsiz: vertical offset from the origin of the reference grid to the top side of the image area */
	uint32_t y0;
	/** Xsiz: width of the reference grid */
	uint32_t x1;
	/** Ysiz: height of the reference grid */
	uint32_t y1;
	/** number of components in the image */
	uint32_t numcomps;
	/** color space: sRGB, Greyscale or YUV */
	GRK_COLOR_SPACE color_space;
	/** image components */
	grk_image_comp_t *comps;
	/** 'restricted' ICC profile */
	uint8_t *icc_profile_buf;
	/** size of ICC profile */
	uint32_t icc_profile_len;
	double capture_resolution[2];
	double display_resolution[2];
	uint8_t *iptc_buf;
	size_t iptc_len;
	uint8_t *xmp_buf;
	size_t xmp_len;
} grk_image_t;

/**
 * Component parameters structure used by the grk_image_create function
 * */
typedef struct grk_image_comptparm {
	/** XRsiz: horizontal separation of a sample of with component with respect to the reference grid */
	uint32_t dx;
	/** YRsiz: vertical separation of a sample of with component with respect to the reference grid */
	uint32_t dy;
	/** data width */
	uint32_t w;
	/** data height */
	uint32_t h;
	/** x component offset compared to the whole image */
	uint32_t x0;
	/** y component offset compared to the whole image */
	uint32_t y0;
	/** precision */
	uint32_t prec;
	/** signed (1) / unsigned (0) */
	uint32_t sgnd;
} grk_image_cmptparm_t;

/*
 ==========================================================
 Information about the JPEG 2000 codestream
 ==========================================================
 */

/**
 * Index structure : Information concerning a packet inside tile
 * */
typedef struct grk_packet_info {
	/** packet start position (including SOP marker if it exists) */
	int64_t start_pos;
	/** end of packet header position (including EPH marker if it exists)*/
	int64_t end_ph_pos;
	/** packet end position */
	int64_t end_pos;
	/** packet distortion */
	double disto;
} grk_packet_info_t;

/**
 * Marker structure
 * */
typedef struct grk_marker_info {
	/** marker type */
	uint16_t type;
	/** position in codestream */
	uint64_t pos;
	/** length, marker val included */
	uint32_t len;
} grk_marker_info_t;

/**
 * Index structure : Information concerning tile-parts
 */
typedef struct grk_tp_info {
	/** start position of tile part */
	uint32_t tp_start_pos;
	/** end position of tile part header */
	uint32_t tp_end_header;
	/** end position of tile part */
	uint32_t tp_end_pos;
	/** start packet of tile part */
	uint32_t tp_start_pack;
	/** number of packets of tile part */
	uint32_t tp_numpacks;
} grk_tp_info_t;

/**
 * Index structure : information regarding tiles
 */
typedef struct grk_tile_info {
	/** value of thresh for each layer by tile cfr. Marcela   */
	double *thresh;
	/** number of tile */
	uint32_t tileno;
	/** start position */
	uint32_t start_pos;
	/** end position of the header */
	uint32_t end_header;
	/** end position */
	uint32_t end_pos;
	/** precinct number for each resolution level (width) */
	uint32_t pw[GRK_J2K_MAXRLVLS];
	/** precinct number for each resolution level (height) */
	uint32_t ph[GRK_J2K_MAXRLVLS];
	/** precinct size (in power of 2), in X for each resolution level */
	uint32_t pdx[GRK_J2K_MAXRLVLS];
	/** precinct size (in power of 2), in Y for each resolution level */
	uint32_t pdy[GRK_J2K_MAXRLVLS];
	/** information concerning packets inside tile */
	grk_packet_info_t *packet;
	int64_t numpix;
	double distotile;
	/** number of markers */
	uint32_t marknum;
	/** list of markers */
	grk_marker_info_t *marker;
	/** actual size of markers array */
	uint32_t maxmarknum;
	/** number of tile parts */
	uint32_t num_tps;
	/** information concerning tile parts */
	grk_tp_info_t *tp;
} grk_tile_info_t;

/**
 * Index structure of the codestream
 */
typedef struct grk_codestream_info {
	/** maximum distortion reduction on the whole image (add for Marcela) */
	double D_max;
	/** packet number */
	uint32_t packno;
	/** writing the packet in the index with t2_encode_packets */
	uint32_t index_write;
	/** image width */
	uint32_t image_w;
	/** image height */
	uint32_t image_h;
	/** progression order */
	GRK_PROG_ORDER prog;
	/** tile size in x */
	uint32_t tile_x;
	/** tile size in y */
	uint32_t tile_y;
	/** */
	uint32_t tile_Ox;
	/** */
	uint32_t tile_Oy;
	/** number of tiles in X */
	uint32_t tw;
	/** number of tiles in Y */
	uint32_t th;
	/** component numbers */
	uint32_t numcomps;
	/** number of layer */
	uint32_t numlayers;
	/** number of decomposition for each component */
	uint32_t *numdecompos;
	/** number of markers */
	uint32_t marknum;
	/** list of markers */
	grk_marker_info_t *marker;
	/** actual size of markers array */
	uint32_t maxmarknum;
	/** main header position */
	uint64_t main_head_start;
	/** main header position */
	uint64_t main_head_end;
	/** codestream's size */
	uint64_t codestream_size;
	/** information regarding tiles inside image */
	grk_tile_info_t *tile;
} grk_codestream_info_t;

/**
 * Tile-component coding parameters information
 */
typedef struct grk_tccp_info {
	/** component index */
	uint32_t compno;
	/** coding style */
	uint32_t csty;
	/** number of resolutions */
	uint32_t numresolutions;
	/** code-blocks width */
	uint32_t cblkw;
	/** code-blocks height */
	uint32_t cblkh;
	/** code block mode */
	uint32_t mode_switch;
	/** discrete wavelet transform identifier */
	uint32_t qmfbid;
	/** quantisation style */
	uint32_t qntsty;
	/** stepsizes used for quantization */
	uint32_t stepsizes_mant[GRK_J2K_MAXBANDS];
	/** stepsizes used for quantization */
	uint32_t stepsizes_expn[GRK_J2K_MAXBANDS];
	/** number of guard bits */
	uint32_t numgbits;
	/** Region Of Interest shift */
	uint32_t roishift;
	/** precinct width */
	uint32_t prcw[GRK_J2K_MAXRLVLS];
	/** precinct height */
	uint32_t prch[GRK_J2K_MAXRLVLS];
} grk_tccp_info_t;

/**
 * Tile coding parameters information
 */
typedef struct grk_tile_v2_info {
	/** number (index) of tile */
	uint32_t tileno;
	/** coding style */
	uint32_t csty;
	/** progression order */
	GRK_PROG_ORDER prg;
	/** number of layers */
	uint32_t numlayers;
	/** multi-component transform identifier */
	uint32_t mct;
	/** information concerning tile component parameters*/
	grk_tccp_info_t *tccp_info;

} grk_tile_info_v2_t;

/**
 * Information structure about the codestream (FIXME should be expand and enhance)
 */
typedef struct grk_codestream_info_v2 {
	/* Tile info */
	/** tile origin in x = XTOsiz */
	uint32_t tx0;
	/** tile origin in y = YTOsiz */
	uint32_t ty0;
	/** tile size in x = XTsiz */
	uint32_t tdx;
	/** tile size in y = YTsiz */
	uint32_t tdy;
	/** number of tiles in X */
	uint32_t tw;
	/** number of tiles in Y */
	uint32_t th;
	/** number of components*/
	uint32_t nbcomps;
	/** Default information regarding tiles inside image */
	grk_tile_info_v2_t m_default_tile_info;
	/** information regarding tiles inside image */
	grk_tile_info_v2_t *tile_info; /* FIXME not used for the moment */
} grk_codestream_info_v2_t;

/**
 * Index structure about a tile part
 */
typedef struct grk_tp_index {
	/** start position */
	int64_t start_pos;
	/** end position of the header */
	int64_t end_header;
	/** end position */
	int64_t end_pos;
} grk_tp_index_t;

/**
 * Index structure about a tile
 */
typedef struct grk_tile_index {
	/** tile index */
	uint32_t tileno;
	/** number of tile parts */
	uint32_t nb_tps;
	/** current nb of tile part (allocated)*/
	uint32_t current_nb_tps;
	/** current tile-part index */
	uint32_t current_tpsno;
	/** information concerning tile parts */
	grk_tp_index_t *tp_index;
	/** number of markers */
	uint32_t marknum;
	/** list of markers */
	grk_marker_info_t *marker;
	/** actual size of markers array */
	uint32_t maxmarknum;
	/** packet number */
	uint32_t nb_packet;
	/** information concerning packets inside tile */
	grk_packet_info_t *packet_index;
} grk_tile_index_t;

/**
 * Index structure of the codestream (FIXME should be expand and enhance)
 */
typedef struct grk_codestream_index {
	/** main header start position (SOC position) */
	uint64_t main_head_start;
	/** main header end position (first SOT position) */
	uint64_t main_head_end;
	/** codestream's size */
	uint64_t codestream_size;
	/** number of markers */
	uint32_t marknum;
	/** list of markers */
	grk_marker_info_t *marker;
	/** actual size of markers array */
	uint32_t maxmarknum;
	uint32_t nb_of_tiles;
	grk_tile_index_t *tile_index; /* FIXME not used for the moment */
} grk_codestream_index_t;
/* -----------------------------------------------------------> */

// version
GRK_API const char* GRK_CALLCONV grk_version(void);
//initialize library
GRK_API bool GRK_CALLCONV grk_initialize(const char *plugin_path,
		uint32_t numthreads);
//deinitialize library
GRK_API void GRK_CALLCONV grk_deinitialize();

/*
 =================================
 image functions definitions
 =================================
 */

/**
 * Create an image
 *
 * @param numcmpts      number of components
 * @param cmptparms     components parameters
 * @param clrspc        image color space
 * @return returns      a new image structure if successful, returns nullptr otherwise
 * */
GRK_API grk_image_t* GRK_CALLCONV grk_image_create(uint32_t numcmpts,
		grk_image_cmptparm_t *cmptparms, GRK_COLOR_SPACE clrspc);

/**
 * Deallocate any resources associated with an image
 *
 * @param image         image to be destroyed
 */
GRK_API void GRK_CALLCONV grk_image_destroy(grk_image_t *image);

/**
 * Creates an image without allocating memory for the image (used in the new version of the library).
 *
 * @param	numcmpts    the number of components
 * @param	cmptparms   the components parameters
 * @param	clrspc      the image color space
 *
 * @return	a new image structure if successful, nullptr otherwise.
 */
GRK_API grk_image_t* GRK_CALLCONV grk_image_tile_create(uint32_t numcmpts,
		grk_image_cmptparm_t *cmptparms, GRK_COLOR_SPACE clrspc);

GRK_API void GRK_CALLCONV grk_image_all_components_data_free(
		grk_image_t *image);

GRK_API void GRK_CALLCONV grk_image_single_component_data_free(
		grk_image_comp_t *image);

GRK_API bool GRK_CALLCONV grk_image_single_component_data_alloc(
		grk_image_comp_t *image);

GRK_API uint8_t* GRK_CALLCONV grk_buffer_new(size_t len);

GRK_API void GRK_CALLCONV grk_buffer_delete(uint8_t *buffer);
/*
 =================================
 stream functions definitions
 =================================
 */

/**
 * Creates an abstract stream. This function does nothing except allocating memory and initializing the abstract stream.
 *
 * @param	p_is_input		if set to true then the stream will be an input stream, an output stream else.
 *
 * @return	a stream object.
 */
GRK_API grk_stream_t* GRK_CALLCONV grk_stream_default_create(bool p_is_input);

/**
 * Creates an abstract stream. This function does nothing except allocating memory and initializing the abstract stream.
 *
 * @param	p_buffer_size   size of stream's double-buffer
 * @param	p_is_input		if set to true then the stream will be an input stream, an output stream else.
 *
 * @return	a stream object.
 */
GRK_API grk_stream_t* GRK_CALLCONV grk_stream_create(size_t p_buffer_size,
		bool p_is_input);

/**
 * Destroys a stream created by grk_create_stream. This function does NOT close the abstract stream. If needed the user must
 * close its own implementation of the stream.
 *
 * @param	p_stream	the stream to destroy.
 */
GRK_API void GRK_CALLCONV grk_stream_destroy(grk_stream_t *p_stream);

/**
 * Sets the given function to be used as a read function.
 * @param		p_stream	the stream to modify
 * @param		p_function	the function to use a read function.
 */
GRK_API void GRK_CALLCONV grk_stream_set_read_function(grk_stream_t *p_stream,
		grk_stream_read_fn p_function);

/**
 * Sets the given function to be used as a zero copy read function.
 * NOTE: this feature is only available for memory mapped and buffer backed streams, not file streams
 * @param		p_stream	the stream to modify
 * @param		p_function	the function to use a read function.
 */
GRK_API void GRK_CALLCONV grk_stream_set_zero_copy_read_function(
		grk_stream_t *p_stream, grk_stream_zero_copy_read_fn p_function);

/**
 * Sets the given function to be used as a write function.
 * @param		p_stream	the stream to modify
 * @param		p_function	the function to use a write function.
 */
GRK_API void GRK_CALLCONV grk_stream_set_write_function(grk_stream_t *p_stream,
		grk_stream_write_fn p_function);

/**
 * Sets the given function to be used as a seek function, the stream is then seekable.
 * @param		p_stream	the stream to modify
 * @param		p_function	the function to use a skip function.
 */
GRK_API void GRK_CALLCONV grk_stream_set_seek_function(grk_stream_t *p_stream,
		grk_stream_seek_fn p_function);

/**
 * Sets the given data to be used as a user data for the stream.
 * @param		p_stream	the stream to modify
 * @param		p_data		the data to set.
 * @param		p_function	the function to free p_data when grk_stream_destroy() is called.
 */
GRK_API void GRK_CALLCONV grk_stream_set_user_data(grk_stream_t *p_stream,
		void *p_data, grk_stream_free_user_data_fn p_function);

/**
 * Sets the length of the user data for the stream.
 *
 * @param p_stream    the stream to modify
 * @param data_length length of the user_data.
 */
GRK_API void GRK_CALLCONV grk_stream_set_user_data_length(
		grk_stream_t *p_stream, uint64_t data_length);

/**
 * Create a stream from a file identified with its filename with default parameters (helper function)
 * @param fname             the filename of the file to stream
 * @param p_is_read_stream  whether the stream is a read stream (true) or not (false)
 */
GRK_API grk_stream_t* GRK_CALLCONV grk_stream_create_default_file_stream(
		const char *fname, bool p_is_read_stream);

/** Create a stream from a file identified with its filename with a specific buffer size
 * @param fname             the filename of the file to stream
 * @param p_buffer_size     size of the chunk used to stream
 * @param p_is_read_stream  whether the stream is a read stream (true) or not (false)
 */
GRK_API grk_stream_t* GRK_CALLCONV grk_stream_create_file_stream(
		const char *fname, size_t p_buffer_size, bool p_is_read_stream);

/** Create a stream from a buffer
 * @param buf			buffer
 * @param buffer_len    length of buffer
 * @param ownsBuffer	if true, library will delete[] buffer. Otherwise, it is the caller's
 *						responsibility to delete the buffer
 * @param p_is_read_stream  whether the stream is a read stream (true) or not (false)
 */
GRK_API grk_stream_t* GRK_CALLCONV grk_stream_create_buffer_stream(uint8_t *buf,
		size_t buffer_len, bool ownsBuffer, bool p_is_read_stream);

GRK_API size_t GRK_CALLCONV grk_stream_get_write_buffer_stream_length(
		grk_stream_t*);

GRK_API grk_stream_t* GRK_CALLCONV grk_stream_create_mapped_file_read_stream(
		const char *fname);

/*
 ========================================
 logger functions definitions
 ========================================
 */
/**
 * Set the info handler used by Grok.
 * @param p_codec       the codec previously initialise
 * @param p_callback    the callback function which will be used
 * @param p_user_data   client object where will be returned the message
 */
GRK_API bool GRK_CALLCONV grk_set_info_handler(grk_codec_t *p_codec,
		grk_msg_callback p_callback, void *p_user_data);
/**
 * Set the warning handler used by Grok.
 * @param p_codec       the codec previously initialise
 * @param p_callback    the callback function which will be used
 * @param p_user_data   client object where will be returned the message
 */
GRK_API bool GRK_CALLCONV grk_set_warning_handler(grk_codec_t *p_codec,
		grk_msg_callback p_callback, void *p_user_data);
/**
 * Set the error handler used by Grok.
 * @param p_codec       the codec previously initialise
 * @param p_callback    the callback function which will be used
 * @param p_user_data   client object where will be returned the message
 */
GRK_API bool GRK_CALLCONV grk_set_error_handler(grk_codec_t *p_codec,
		grk_msg_callback p_callback, void *p_user_data);

/*
 ===============================
 codec functions definitions
 ===============================
 */

/**
 * Creates a J2K/JP2 decompression structure
 * @param format 		Decoder to select
 *
 * @return a handle to a decompressor if successful, returns nullptr otherwise
 * */
GRK_API grk_codec_t* GRK_CALLCONV grk_create_decompress(
		GRK_CODEC_FORMAT format);

/**
 * Destroy a decompressor handle
 *
 * @param	p_codec			decompressor handle to destroy
 */
GRK_API void GRK_CALLCONV grk_destroy_codec(grk_codec_t *p_codec);

/**
 * End compression
 * @param	p_codec			the JPEG2000 codec
 * @param	p_stream		the JPEG2000 stream.
 */
GRK_API bool GRK_CALLCONV grk_end_decompress(grk_codec_t *p_codec,
		grk_stream_t *p_stream);

/**
 * Set decoding parameters to default values
 * @param parameters Decompression parameters
 */
GRK_API void GRK_CALLCONV grk_set_default_decoder_parameters(
		grk_dparameters_t *parameters);

/**
 * Setup the decoder with decompression parameters provided by the user and with the message handler
 * provided by the user.
 *
 * @param p_codec 		decompressor handler
 * @param parameters 	decompression parameters
 *
 * @return true			if the decoder is correctly set
 */
GRK_API bool GRK_CALLCONV grk_setup_decoder(grk_codec_t *p_codec,
		grk_dparameters_t *parameters);
/**
 * Decodes an image header.
 *
 * @param	p_stream		the jpeg2000 stream.
 * @param	p_codec			the jpeg2000 codec to read.
 * @param	p_image			the image structure initialized with the characteristics of encoded image.
 *
 * @return true				if the main header of the codestream and the JP2 header is correctly read.
 */
GRK_API bool GRK_CALLCONV grk_read_header(grk_stream_t *p_stream,
		grk_codec_t *p_codec, grk_image_t **p_image);

/**
 * Decodes an image header (extended version).
 *
 * @param	p_stream			the jpeg2000 stream.
 * @param	p_codec				the jpeg2000 codec to read.
 * @param	header_info			information read from jpeg 2000 header.
 * @param	p_image				the image structure initialized with the characteristics
 *								of encoded image.
 * @return true				if the main header of the codestream and the JP2 header is correctly read.
 */
GRK_API bool GRK_CALLCONV grk_read_header_ex(grk_stream_t *p_stream,
		grk_codec_t *p_codec, grk_header_info_t *header_info,
		grk_image_t **p_image);

/**
 * Sets the given area to be decoded. This function should be called right after grk_read_header and before any tile header reading.
 *
 * @param	p_codec			the jpeg2000 codec.
 * @param	p_image         the decoded image previously set by grk_read_header
 * @param	start_x		the left position of the rectangle to decode (in image coordinates).
 * @param	end_x			the right position of the rectangle to decode (in image coordinates).
 * @param	start_y		the up position of the rectangle to decode (in image coordinates).
 * @param	end_y			the bottom position of the rectangle to decode (in image coordinates).
 *
 * @return	true			if the area could be set.
 */
GRK_API bool GRK_CALLCONV grk_set_decode_area(grk_codec_t *p_codec,
		grk_image_t *p_image, uint32_t start_x, uint32_t start_y,
		uint32_t end_x, uint32_t end_y);

////////////////////////////////////////////////
// Structs to pass data between grok and plugin
/////////////////////////////////////////////////

typedef struct grok_plugin_pass {
	double distortionDecrease; //distortion decrease up to and including this pass
	size_t rate;    // rate up to and including this pass
	size_t length;	//stream length for this pass
} grok_plugin_pass_t;

typedef struct grok_plugin_code_block {
	/////////////////////////
	// debug info
	uint32_t x0, y0, x1, y1;
	unsigned int *contextStream;
	///////////////////////////
	size_t numPix;
	unsigned char *compressedData;
	size_t compressedDataLength;
	size_t numBitPlanes;
	size_t numPasses;
	grok_plugin_pass_t passes[67];
	unsigned int sortedIndex;
} grok_plugin_code_block_t;

typedef struct grok_plugin_precinct {
	size_t numBlocks;
	grok_plugin_code_block_t **blocks;
} grok_plugin_precinct_t;

typedef struct grok_plugin_band {
	size_t orient;
	size_t numPrecincts;
	grok_plugin_precinct_t **precincts;
	float stepsize;
} grok_plugin_band_t;

typedef struct grok_plugin_resolution {
	size_t level;
	size_t numBands;
	grok_plugin_band_t **bands;
} grok_plugin_resolution_t;

typedef struct grok_plugin_tile_component {
	size_t numResolutions;
	grok_plugin_resolution_t **resolutions;
} grok_plugin_tile_component_t;

#define GROK_DECODE_HEADER	(1 << 0)
#define GROK_DECODE_T2		(1 << 1)
#define GROK_DECODE_T1		(1 << 2)
#define GROK_DECODE_POST_T1	(1 << 3)
#define GROK_PLUGIN_DECODE_CLEAN  (1 << 4)
#define GROK_DECODE_ALL		(GROK_PLUGIN_DECODE_CLEAN | GROK_DECODE_HEADER | GROK_DECODE_T2 | GROK_DECODE_T1 | GROK_DECODE_POST_T1)

typedef struct grok_plugin_tile {
	uint32_t decode_flags;
	size_t numComponents;
	grok_plugin_tile_component_t **tileComponents;
} grok_plugin_tile_t;

/**
 * Decode an image from a JPEG-2000 codestream
 *
 * @param p_decompressor 	decompressor handle
 * @param tile			 	tile struct from plugin
 * @param p_stream			Input buffer stream
 * @param p_image 			the decoded image
 * @return 					true if success, otherwise false
 * */
GRK_API bool GRK_CALLCONV grk_decode(grk_codec_t *p_decompressor,
		grok_plugin_tile_t *tile, grk_stream_t *p_stream, grk_image_t *p_image);

/**
 * Get the decoded tile from the codec
 *
 * @param	p_codec			the jpeg2000 codec.
 * @param	p_stream		input stream
 * @param	p_image			output image
 * @param	tile_index		index of the tile which will be decode
 *
 * @return					true if success, otherwise false
 */
GRK_API bool GRK_CALLCONV grk_get_decoded_tile(grk_codec_t *p_codec,
		grk_stream_t *p_stream, grk_image_t *p_image, uint32_t tile_index);

/**
 * Set the resolution factor of the decoded image
 * @param	p_codec			the jpeg2000 codec.
 * @param	res_factor		resolution factor to set
 *
 * @return					true if success, otherwise false
 */
GRK_API bool GRK_CALLCONV grk_set_decoded_resolution_factor(
		grk_codec_t *p_codec, uint32_t res_factor);

/**
 * Writes a tile with the given data.
 *
 * @param	p_codec		        the jpeg2000 codec.
 * @param	tile_index		the index of the tile to write. At the moment, the tiles must be written from 0 to n-1 in sequence.
 * @param	p_data				pointer to the data to write. Data is arranged in sequence, data_comp0, then data_comp1, then ... NO INTERLEAVING should be set.
 * @param	data_size			this value os used to make sure the data being written is correct. The size must be equal to the sum for each component of
 *                              tile_width * tile_height * component_size. component_size can be 1,2 or 4 bytes, depending on the precision of the given component.
 * @param	p_stream			the stream to write data to.
 *
 * @return	true if the data could be written.
 */
GRK_API bool GRK_CALLCONV grk_write_tile(grk_codec_t *p_codec,
		uint32_t tile_index, uint8_t *p_data, uint64_t data_size,
		grk_stream_t *p_stream);

/**
 * Reads a tile header. This function is compulsory and allows one to know the size of the tile that will be decoded.
 * The user may need to refer to the image got by grk_read_header to understand the size being taken by the tile.
 *
 * @param	p_codec			the jpeg2000 codec.
 * @param	tile_index	pointer to a value that will hold the index of the tile being decoded, in case of success.
 * @param	data_size		pointer to a value that will hold the maximum size of the decoded data, in case of success. In case
 *							of truncated codestreams, the actual number of bytes decoded may be lower. The computation of the size is the same
 *							as depicted in grk_write_tile.
 * @param	p_tile_x0		pointer to a value that will hold the x0 pos of the tile (in the image).
 * @param	p_tile_y0		pointer to a value that will hold the y0 pos of the tile (in the image).
 * @param	p_tile_x1		pointer to a value that will hold the x1 pos of the tile (in the image).
 * @param	p_tile_y1		pointer to a value that will hold the y1 pos of the tile (in the image).
 * @param	p_nb_comps		pointer to a value that will hold the number of components in the tile.
 * @param	p_should_go_on	pointer to a boolean that will hold the fact that the decoding should go on. In case the
 *							codestream is over at the time of the call, the value will be set to false. The user should then stop
 *							the decoding.
 * @return	true			if the tile header could be decoded. In case the decoding should end, the returned value is still true.
 *							returning false may be the result of a shortage of memory or an internal error.
 */
GRK_API bool GRK_CALLCONV grk_read_tile_header(grk_codec_t *p_codec,
		grk_stream_t *p_stream, uint32_t *tile_index, uint64_t *data_size,
		uint32_t *p_tile_x0, uint32_t *p_tile_y0, uint32_t *p_tile_x1,
		uint32_t *p_tile_y1, uint32_t *p_nb_comps, bool *p_should_go_on);

/**
 * Reads a tile data. This function is compulsory and allows one to decode tile data. grk_read_tile_header should be called before.
 * The user may need to refer to the image got by grk_read_header to understand the size being taken by the tile.
 *
 * @param	p_codec			the jpeg2000 codec.
 * @param	tile_index	the index of the tile being decoded, this should be the value set by grk_read_tile_header.
 * @param	p_data			pointer to a memory block that will hold the decoded data.
 * @param	data_size		size of p_data. data_size should be bigger or equal to the value set by grk_read_tile_header.
 * @param	p_stream		the stream to decode.
 *
 * @return	true			if the data could be decoded.
 */
GRK_API bool GRK_CALLCONV grk_decode_tile_data(grk_codec_t *p_codec,
		uint32_t tile_index, uint8_t *p_data, uint64_t data_size,
		grk_stream_t *p_stream);

/* COMPRESSION FUNCTIONS*/

/**
 * Creates a J2K/JP2 compression structure
 * @param 	format 		Coder to select
 * @return 				Returns a handle to a compressor if successful, returns nullptr otherwise
 */
GRK_API grk_codec_t* GRK_CALLCONV grk_create_compress(GRK_CODEC_FORMAT format);

/**
 Set encoding parameters to default values, that means :

 Lossless
 1 tile
 Size of precinct : 2^15 x 2^15 (means 1 precinct)
 Size of code-block : 64 x 64
 Number of resolutions: 6
 No SOP marker in the codestream
 No EPH marker in the codestream
 No sub-sampling in x or y direction
 No mode switch activated
 Progression order: LRCP
 No index file
 No ROI upshifted
 No offset of the origin of the image
 No offset of the origin of the tiles
 Reversible DWT 5-3
 @param parameters Compression parameters
 */
GRK_API void GRK_CALLCONV grk_set_default_encoder_parameters(
		grk_cparameters_t *parameters);

/**
 * Setup the encoder parameters using the current image and using user parameters.
 * @param p_codec 		Compressor handle
 * @param parameters 	Compression parameters
 * @param image 		Input filled image
 */
GRK_API bool GRK_CALLCONV grk_setup_encoder(grk_codec_t *p_codec,
		grk_cparameters_t *parameters, grk_image_t *image);

/**
 * Start to compress the current image.
 * @param p_codec 		Compressor handle
 * @param image 	    Input filled image
 * @param p_stream 		Input stream
 */
GRK_API bool GRK_CALLCONV grk_start_compress(grk_codec_t *p_codec,
		grk_image_t *p_image, grk_stream_t *p_stream);

/**
 * End to compress the current image.
 * @param p_codec 		Compressor handle
 * @param p_stream 		Input stream
 */
GRK_API bool GRK_CALLCONV grk_end_compress(grk_codec_t *p_codec,
		grk_stream_t *p_stream);

/**
 * Encode an image into a JPEG-2000 codestream
 * @param p_codec 		compressor handle
 * @param p_stream 		Output buffer stream
 *
 * @return 				Returns true if successful, returns false otherwise
 */
GRK_API bool GRK_CALLCONV grk_encode(grk_codec_t *p_codec,
		grk_stream_t *p_stream);

/**
 * Encode an image into a JPEG-2000 codestream using plugin
 * @param p_codec 		compressor handle
 * @param tile			plugin tile
 * @param p_stream 		Output buffer stream
 *
 * @return 				Returns true if successful, returns false otherwise
 */
GRK_API bool GRK_CALLCONV grk_encode_with_plugin(grk_codec_t *p_codec,
		grok_plugin_tile_t *tile, grk_stream_t *p_stream);

/*
 ==========================================================
 codec output functions definitions
 ==========================================================
 */
/**
 Destroy Codestream information after compression or decompression
 @param cstr_info Codestream information structure
 */
GRK_API void GRK_CALLCONV grk_destroy_cstr_info(
		grk_codestream_info_v2_t **cstr_info);

/**
 * Dump the codec information into the output stream
 *
 * @param	p_codec			the jpeg2000 codec.
 * @param	info_flag		type of information dump.
 * @param	output_stream	output stream where dump the information get from the codec.
 *
 */
GRK_API void GRK_CALLCONV grk_dump_codec(grk_codec_t *p_codec,
		int32_t info_flag, FILE *output_stream);

/**
 * Get the codestream information from the codec
 *
 * @param	p_codec			the jpeg2000 codec.
 *
 * @return					a pointer to a codestream information structure.
 *
 */
GRK_API grk_codestream_info_v2_t* GRK_CALLCONV grk_get_cstr_info(
		grk_codec_t *p_codec);

/**
 * Get the codestream index from the codec
 *
 * @param	p_codec			the jpeg2000 codec.
 *
 * @return					a pointer to a codestream index structure.
 *
 */
GRK_API grk_codestream_index_t* GRK_CALLCONV grk_get_cstr_index(
		grk_codec_t *p_codec);

GRK_API void GRK_CALLCONV grk_destroy_cstr_index(
		grk_codestream_index_t **p_cstr_index);

/*
 ==========================================================
 MCT functions
 ==========================================================
 */

/**
 * Sets the MCT matrix to use.
 *
 * @param	parameters		the parameters to change.
 * @param	pEncodingMatrix	the encoding matrix.
 * @param	p_dc_shift		the dc shift coefficients to use.
 * @param	pNbComp			the number of components of the image.
 *
 * @return	true if the parameters could be set.
 */
GRK_API bool GRK_CALLCONV grk_set_MCT(grk_cparameters_t *parameters,
		float *pEncodingMatrix, int32_t *p_dc_shift, uint32_t pNbComp);

/*****************
 Plugin Interface
 ******************/

/*
 Plugin management
 */

typedef struct grok_plugin_load_info {
	const char *plugin_path;
} grok_plugin_load_info_t;

GRK_API bool GRK_CALLCONV grok_plugin_load(grok_plugin_load_info_t info);

GRK_API void GRK_CALLCONV grok_plugin_cleanup(void);

// No debug is done on plugin. Production setting.
#define GROK_PLUGIN_STATE_NO_DEBUG			0x0

//For encode debugging, the plugin first performs a T1 encode.
// Then:
// 1. perform host DWT on plugin MCT data, and write to host image
// This way, both plugin and host start from same point (assume MCT is equivalent for both host and plugin)
//2. map plugin DWT data, compare with host DWT, and then write to plugin image
// At this point in the code, the plugin image holds plugin DWT data. And if no warnings are triggered,
// then we can safely say that host and plugin DWT data are identical.
//3. Perform host encode, skipping MCT and DWT (they have already been performed)
//4. during host encode, each context that is formed is compared against context stream from plugin
//5. rate control - synch with plugin code stream, and compare
//6. T2 and store to disk
#define GROK_PLUGIN_STATE_DEBUG				0x1
#define GROK_PLUGIN_STATE_PRE_TR1			0x2
#define GROK_PLUGIN_STATE_DWT_QUANTIZATION	0x4
#define GROK_PLUGIN_STATE_MCT_ONLY			0x8

GRK_API uint32_t GRK_CALLCONV grok_plugin_get_debug_state();

/*
 Plugin encoding
 */

typedef struct grok_plugin_init_info {
	int32_t deviceId;
	bool verbose;
} grok_plugin_init_info_t;

GRK_API bool GRK_CALLCONV grok_plugin_init(grok_plugin_init_info_t initInfo);

typedef struct grok_plugin_encode_user_callback_info {
	const char *input_file_name;
	bool outputFileNameIsRelative;
	const char *output_file_name;
	grk_cparameters_t *encoder_parameters;
	grk_image_t *image;
	grok_plugin_tile_t *tile;
	uint8_t *compressBuffer;
	size_t compressBufferLen;
	unsigned int error_code;
} grok_plugin_encode_user_callback_info_t;

typedef bool (*GROK_PLUGIN_ENCODE_USER_CALLBACK)(
		grok_plugin_encode_user_callback_info_t *info);

GRK_API int32_t GRK_CALLCONV grok_plugin_encode(
		grk_cparameters_t *encode_parameters,
		GROK_PLUGIN_ENCODE_USER_CALLBACK callback);

GRK_API int32_t GRK_CALLCONV grok_plugin_batch_encode(const char *input_dir,
		const char *output_dir, grk_cparameters_t *encode_parameters,
		GROK_PLUGIN_ENCODE_USER_CALLBACK callback);

GRK_API bool GRK_CALLCONV grok_plugin_is_batch_complete(void);

GRK_API void GRK_CALLCONV grok_plugin_stop_batch_encode(void);

/*
 Plugin decoding
 */

typedef int (*GROK_INIT_DECODERS)(grk_header_info_t *header_info,
		grk_image_t *image);

typedef struct grok_plugin_decode_callback_info {
	size_t deviceId;
	GROK_INIT_DECODERS init_decoders_func;
	const char *input_file_name;
	const char *output_file_name;
	// input file format 0: J2K, 1: JP2
	int decod_format;
	// output file format 0: PGX, 1: PxM, 2: BMP etc 
	int cod_format;
	grk_stream_t *l_stream;
	grk_codec_t *l_codec;
	grk_header_info_t header_info;
	grk_decompress_parameters *decoder_parameters;
	grk_image_t *image;
	bool plugin_owns_image;
	grok_plugin_tile_t *tile;
	unsigned int error_code;
	uint32_t decode_flags;
} grok_plugin_decode_callback_info_t;

typedef int32_t (*grok_plugin_decode_callback)(
		grok_plugin_decode_callback_info_t *info);

GRK_API int32_t GRK_CALLCONV grok_plugin_decode(
		grk_decompress_parameters *decode_parameters,
		grok_plugin_decode_callback callback);

GRK_API int32_t GRK_CALLCONV grok_plugin_init_batch_decode(
		const char *input_dir, const char *output_dir,
		grk_decompress_parameters *decode_parameters,
		grok_plugin_decode_callback callback);

GRK_API int32_t GRK_CALLCONV grok_plugin_batch_decode(void);

GRK_API void GRK_CALLCONV grok_plugin_stop_batch_decode(void);

#ifdef __cplusplus
}
#endif

