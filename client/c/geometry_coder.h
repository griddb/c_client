/*
   Copyright (c) 2017 TOSHIBA Digital Solutions Corporation

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
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
