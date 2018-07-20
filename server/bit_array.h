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
	@brief Definition of BitArray
*/
#ifndef BIT_ARRAY_H_
#define BIT_ARRAY_H_

#include "util/code.h"
#include "util/container.h"
#include <iostream>

/*!
	@brief Represents a compact array of bit values
*/
class BitArray {
public:
	static const uint32_t UNIT_BIT_SIZE =
		sizeof(uint64_t) * 8;  
	static const uint64_t UNDEFINED_POS = -1;

	BitArray(uint64_t capacity);
	~BitArray();

	inline bool get(uint64_t pos) const {
		if (pos == UNDEFINED_POS) {
			return false;
		}
		if (pos < bitNum_) {
			return (data_[unitNth(pos)] & (1ULL << unitOffset(pos))) != 0;
		}
		else {
			return false;
		}
	}

	void set(uint64_t pos, bool value);

	uint64_t append(bool value);

	inline uint8_t* data() const {
		return reinterpret_cast<uint8_t*>(data_);
	}

	inline uint64_t byteLength() const {
		return (bitNum_+ CHAR_BIT - 1) / CHAR_BIT;
	}

	inline uint64_t countNumOfBits() {
		uint64_t numOfBits = 0;
		for (uint64_t i = 0; i < bitNum_ / 64 + 1; ++i) {
			uint32_t *addr = reinterpret_cast<uint32_t *>(&data_[i]);
			numOfBits += util::countNumOfBits(*addr);
			addr++;
			numOfBits += util::countNumOfBits(*addr);
		}
		return numOfBits;
	}

	void reserve(uint64_t capacity);

	void clear() {
		for (uint64_t i = 0; i < reservedUnitNum_; ++i) {
			data_[i] = 0;
		}
		bitNum_ = 0;
	};  

	void putAll(const uint8_t *buf, uint64_t num);

private:
	BitArray(const BitArray &bitArray);

	inline uint64_t unitNth(int64_t pos) const {
		return pos / UNIT_BIT_SIZE;
	}
	inline uint64_t unitOffset(int64_t pos) const {
		return pos % UNIT_BIT_SIZE;
	}

	void realloc(uint64_t newSize);

	uint64_t *data_;
	uint64_t bitNum_;
	uint64_t capacity_;
	uint64_t reservedUnitNum_;
};

#endif  
