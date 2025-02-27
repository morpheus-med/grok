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
namespace grk {

class t1;

/**
 Tier-1 coding (coding of code-block coefficients)
 */
class t1_encode: public t1 {
public:
	t1_encode();
	~t1_encode();
	bool allocateBuffers(uint16_t cblkw, uint16_t cblkh);
	void initBuffers(uint16_t w, uint16_t h);
	void preEncode(encodeBlockInfo *block, tcd_tile_t *tile, uint32_t &max);
	double encode_cblk(tcd_cblk_enc_t *cblk, uint8_t orient, uint32_t compno,
			uint32_t level, uint32_t qmfbid, double stepsize,
			uint32_t mode_switch, uint32_t numcomps, const double *mct_norms,
			uint32_t mct_numcomps, uint32_t max, bool doRateControl);
	uint32_t *data;
private:
	mqc_t *mqc;
	/**
	 Encode significant pass
	 */
	void sigpass_step(flag_opt_t *flagsp, uint32_t *datap, uint8_t orient,
			int32_t bpno, int32_t one, int32_t *nmsedec, uint8_t type,
			uint32_t mode_switch);

	/**
	 Encode significant pass
	 */
	void sigpass(int32_t bpno, uint8_t orient, int32_t *nmsedec, uint8_t type,
			uint32_t mode_switch);

	/**
	 Encode refinement pass
	 */
	void refpass_step(flag_opt_t *flagsp, uint32_t *datap, int32_t bpno,
			int32_t one, int32_t *nmsedec, uint8_t type);

	/**
	 Encode refinement pass
	 */
	void refpass(int32_t bpno, int32_t *nmsedec, uint8_t type);

	/**
	 Encode clean-up pass
	 */
	void clnpass_step(flag_opt_t *flagsp, uint32_t *datap, uint8_t orient,
			int32_t bpno, int32_t one, int32_t *nmsedec, uint32_t agg,
			uint32_t runlen, uint32_t y, uint32_t mode_switch);

	/**
	 Encode clean-up pass
	 */
	void clnpass(int32_t bpno, uint8_t orient, int32_t *nmsedec,
			uint32_t mode_switch);

	double getwmsedec(int32_t nmsedec, uint32_t compno, uint32_t level,
			uint8_t orient, int32_t bpno, uint32_t qmfbid, double stepsize,
			uint32_t numcomps, const double *mct_norms, uint32_t mct_numcomps);

	int16_t getnmsedec_sig(uint32_t x, uint32_t bitpos);
	int16_t getnmsedec_ref(uint32_t x, uint32_t bitpos);
};

}

