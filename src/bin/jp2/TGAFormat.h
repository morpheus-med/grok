/*
*    Copyright (C) 2016 Grok Image Compression Inc.
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
 */
#pragma once

#include "ImageFormat.h"

#include "ImageFormat.h"

class TGAFormat {
public:
	virtual ~TGAFormat() {}
	bool encode(grk_image_t* image, const char* filename, int compressionParam, bool verbose);
	grk_image_t* decode(const char* filename, grk_cparameters_t *parameters);
};
