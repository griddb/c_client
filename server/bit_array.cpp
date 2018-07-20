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
	@brief Implementation of BitArray
*/
#include "bit_array.h"
#include "gs_error_common.h"
#include <iomanip>

BitArray::BitArray(uint64_t capacity)
	: data_(NULL), bitNum_(0), capacity_(1), reservedUnitNum_(0) {
	try {
		size_t unitNum = static_cast<size_t>(unitNth(capacity) + 1);
		data_ = UTIL_NEW uint64_t[unitNum];

		memset(data_, 0, sizeof(uint64_t) * unitNum);
		reservedUnitNum_ = unitNum;
		bitNum_ = 0;
		if (capacity > 0) {
			capacity_ = capacity;
		}
	}
	catch (std::exception &e) {
		GS_COMMON_RETHROW_USER_ERROR(e,
			"Initialization failed. (capacity="
				<< capacity << ")");
	}
}

BitArray::~BitArray() {
	delete[] data_;
	data_ = NULL;
	bitNum_ = 0;
	capacity_ = 1;
	reservedUnitNum_ = 0;
}

void BitArray::realloc(uint64_t newSize) {
	assert(newSize > 0 && capacity_ > 0);
	try {
		if (newSize >= capacity_) {
			uint64_t times = newSize / capacity_ + 1;
			uint64_t newCapacity = capacity_ * times;
			uint64_t newUnitNum = unitNth(newCapacity) + 1;
			uint64_t *oldData = data_;
			uint64_t *newData =
				UTIL_NEW uint64_t[static_cast<size_t>(newUnitNum)];
			memset(
				newData, 0, static_cast<size_t>(sizeof(uint64_t) * newUnitNum));
			if (bitNum_ > 0) {
				assert(data_);
				memcpy(newData, data_,
					static_cast<size_t>(
						   sizeof(uint64_t) * (unitNth(bitNum_) + 1)));
			}
			delete[] oldData;
			data_ = newData;
			capacity_ = newCapacity;
			reservedUnitNum_ = newUnitNum;
		}
	}
	catch (std::exception &e) {
		GS_COMMON_RETHROW_USER_ERROR(
			e, "Realloc failed. (newSize="
				   << newSize << ", oldCapacity=" << capacity_
				   << ")");
	}
}

void BitArray::set(uint64_t pos, bool value) {

	uint64_t bitListSize = pos + 1;  

	if (bitListSize >= capacity_) {
		realloc(bitListSize);
	}
	if (value) {
		data_[unitNth(pos)] |= 1ULL << (unitOffset(pos));
	}
	else {
		data_[unitNth(pos)] &= ~(1ULL << (unitOffset(pos)));
	}
	if (pos >= bitNum_) {
		bitNum_ = pos + 1;
	}
}

uint64_t BitArray::append(bool value) {
	uint64_t newPos = bitNum_;
	set(newPos, value);  
	return newPos;
}

void BitArray::reserve(uint64_t capacity) {
	if (capacity > capacity_) {
		realloc(capacity);
	}
}

void BitArray::putAll(const uint8_t *buf, uint64_t bitNum) {
	try {
		size_t byteSize = static_cast<size_t>((bitNum + CHAR_BIT - 1) / CHAR_BIT);
		if (bitNum > 0) {
			reserve(bitNum + 1);
			memset(data_, 0, static_cast<size_t>(
					sizeof(uint64_t) * reservedUnitNum_));
			memcpy(data_, buf, byteSize);
		}
		else {
			byteSize = 0;
		}
		bitNum_ = bitNum;
	}
	catch (std::exception &e) {
		GS_COMMON_RETHROW_USER_ERROR(e, "");
	}
}

