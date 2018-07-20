/*
	Copyright (c) 2017 TOSHIBA Digital Solutions Corporation

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as
	published by the Free Software Foundation, either version 3 of the
	License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*!
	@file
	@brief Definition of Geometry encoder/decoder
*/
#ifndef GEOMETRY_CODER_H_
#define GEOMETRY_CODER_H_

#include "util/type.h"


class GeometryCoder {
public:
	class BinaryHandler;
	class TextHandler;

	static void toBinary(
			const char8_t *text, BinaryHandler &handler, uint32_t mode);

	static void toText(const void *data, size_t size, TextHandler &handler);

	static std::ostream& toText(
			std::ostream &out, const void *data, size_t size);
};


class GeometryCoder::BinaryHandler {
public:
	virtual ~BinaryHandler();
	virtual void accept(const void *data, size_t size, uint32_t mode) = 0;
};


class GeometryCoder::TextHandler {
public:
	virtual ~TextHandler();
	virtual void accept(const char8_t *text) = 0;
};

#endif	
