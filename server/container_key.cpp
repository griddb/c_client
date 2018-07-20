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
	@brief Implementation of FullContainerKey
*/
#include "container_key.h"

#include "gs_error_common.h"

class ValueProcessor {
public:
	/*!
		@brief Convert to Lower case
	*/
	static void convertLowerCase(char8_t const *p, size_t size, char8_t *out) {
		char c;
		for (size_t i = 0; i < size; i++) {
			c = *(p + i);
			if ((c >= 'A') && (c <= 'Z')) {
				*(out + i) = c + 32;
			}
			else {
				*(out + i) = c;
			}
		}
	}
	/*!
		@brief get Encode Size(1 or 4 or 8byte)
	*/
	static inline uint32_t getEncodedVarSize(uint64_t val) {
		if (val < VAR_SIZE_1BYTE_THRESHOLD) {
			return 1;
		}
		else if (val < VAR_SIZE_4BYTE_THRESHOLD) {
			return 4;
		}
		else {
			return 8;
		}
	}

	/*!
		@brief get Encode Size
	*/
	static inline uint8_t encode1ByteVarSize(uint8_t val) {
			return ((val << 1) | 0x01);
	}

	/*!
		@brief get Encode Size
	*/
	static inline uint32_t encode4ByteVarSize(uint32_t val) {
			return (val << 2);
	}

	/*!
		@brief get Encode Size
	*/
	static inline uint64_t encode8ByteVarSize(uint64_t val) {
		return (val << 2) | 0x02;
	}

	/*!
		@brief Decode variable size (1byte)
	*/
	static inline uint32_t decode1ByteVarSize(uint8_t val) {
		assert(val != 0);
		return val >> 1;
	}
	/*!
		@brief Decode variable size (4byte)
	*/
	static inline uint32_t decode4ByteVarSize(uint32_t val) {
		assert(val != 0);
		return val >> 2;
	}
	/*!
		@brief Decode variable size (8byte)
	*/
	static inline uint64_t decode8ByteVarSize(uint64_t val) {
		assert(val != 0);
		return val >> 2;
	}
	/*!
		@brief Check if variable size is 1 byte
	*/
	static inline bool varSizeIs1Byte(uint8_t val) {
		return ((val & 0x01) == 0x01);
	}
	/*!
		@brief Check if variable size is 4 byte
	*/
	static inline bool varSizeIs4Byte(uint8_t val) {
		return ((val & 0x03) == 0x00);
	}
	/*!
		@brief Check if variable size is 8 byte
	*/
	static inline bool varSizeIs8Byte(uint8_t val) {
		return ((val & 0x03) == 0x02);
	}
	/*!
		@brief Get variable size
	*/
	static inline uint32_t getVarSize(util::ByteStream<util::ArrayInStream> &in) {
		uint64_t currentPos = in.base().position();
		uint8_t byteData;
		in >> byteData;
		if (ValueProcessor::varSizeIs1Byte(byteData)) {
			return ValueProcessor::decode1ByteVarSize(byteData);
		} else if (ValueProcessor::varSizeIs4Byte(byteData)) {
			in.base().position(static_cast<size_t>(currentPos));
			uint32_t rawData;
			in >> rawData;
			return ValueProcessor::decode4ByteVarSize(rawData);
		} else {
			in.base().position(static_cast<size_t>(currentPos));
			uint64_t rawData;
			in >> rawData;
			uint64_t decodedVal = ValueProcessor::decode8ByteVarSize(rawData);
			if (decodedVal > static_cast<uint64_t>(std::numeric_limits<int32_t>::max())) {
				GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_OUT_OF_RANGE,
						"Decoded size = " << decodedVal);
			}
			return static_cast<uint32_t>(decodedVal);
		}
	}
};

const char8_t* FullContainerKey::symbol = "_-./=";
const char8_t* FullContainerKey::forbiddenPattern[] = { NULL };

const DatabaseId FullContainerKey::MAX_ENCODABLE_DBID = static_cast<DatabaseId>(MAX_NODE_AFFINITY_NUMBER);

const size_t FullContainerKey::MAX_NODE_AFFINITY_NUMBER_DIGITS = 19;
const size_t FullContainerKey::MAX_LARGE_CONTAINERID_DIGITS = 19;
const size_t FullContainerKey::MAX_SYSTEM_PART_ID_DIGITS = 19;

const uint8_t FullContainerKey::DBID_EXISTS					= 0x01 << 0;
const uint8_t FullContainerKey::LARGE_CONTAINERID_EXISTS	= 0x01 << 1;
const uint8_t FullContainerKey::NODE_AFFINITY_NUM			= 0x01 << 2;
const uint8_t FullContainerKey::NODE_AFFINITY_STR			= 0x01 << 3;
const uint8_t FullContainerKey::NODE_AFFINITY_EXISTS		= FullContainerKey::NODE_AFFINITY_NUM | FullContainerKey::NODE_AFFINITY_STR;
const uint8_t FullContainerKey::SYSTEM_PART_ID_NUM			= 0x01 << 4;
const uint8_t FullContainerKey::SYSTEM_PART_ID_STR			= 0x01 << 5;
const uint8_t FullContainerKey::SYSTEM_PART_ID_EXISTS		= FullContainerKey::SYSTEM_PART_ID_NUM | FullContainerKey::SYSTEM_PART_ID_STR;

const uint64_t FullContainerKey::DEFAULT_UPPER_CASE_BIT_LENGTH = 1024;

KeyConstraint::KeyConstraint() :
	maxTotalLength_(0),
	systemPartAllowed_(false),
	largeContainerIdAllowed_(false) {
}

KeyConstraint::KeyConstraint(uint32_t maxTotalLength,
							 bool systemPartAllowed,
							 bool largeContainerIdAllowed) :
	maxTotalLength_(maxTotalLength),
	systemPartAllowed_(systemPartAllowed),
	largeContainerIdAllowed_(largeContainerIdAllowed) {
}

FullContainerKeyComponents::FullContainerKeyComponents() {
	clear();
}

void FullContainerKeyComponents::clear() {
	dbId_ = UNDEF_DBID;
	baseName_.clear();
	baseNameSize_ = 0;
	affinityString_.clear();
	affinityStringSize_ = 0;
	affinityNumber_ = UNDEF_NODE_AFFINITY_NUMBER;
	largeContainerId_ = UNDEF_LARGE_CONTAINERID;
	systemPart_.clear();
	systemPartSize_ = 0;
	systemPartId_ = UNDEF_SYSTEM_PART_ID;
}

FullContainerKeyComponents&
FullContainerKeyComponents::operator=(const FullContainerKeyComponents &another) {
	if (this == &another) {
		return *this;
	}
	dbId_ = another.dbId_;
	baseName_ = another.baseName_;
	baseNameSize_ = another.baseNameSize_;
	affinityString_ = another.affinityString_;
	affinityStringSize_ = another.affinityStringSize_;
	affinityNumber_ = another.affinityNumber_;
	largeContainerId_ = another.largeContainerId_;
	systemPart_ = another.systemPart_;
	systemPartSize_ = another.systemPartSize_;
	systemPartId_ = another.systemPartId_;
	return *this;
}

uint32_t FullContainerKeyComponents::getStringSize() const {
	return (baseNameSize_ + affinityStringSize_ + systemPartSize_);
}

FullContainerKey::FullContainerKey(const KeyConstraint &constraint) :
	constraint_(constraint), size_(0) {
		bytes_.clear();
}

FullContainerKey::FullContainerKey(
								   const KeyConstraint &constraint,
								   const void *body,
								   size_t size) :
	constraint_(constraint), size_(size) {
	try {
		
		const uint8_t *it = static_cast<const uint8_t*>(body);
		bytes_.assign(it, it + size);
		
		FullContainerKeyComponents components;
		BitArray upperCaseBit(DEFAULT_UPPER_CASE_BIT_LENGTH);

		deserialize(components, upperCaseBit, false);
		validate(components);

	}
	catch (std::exception &e) {
		GS_COMMON_RETHROW_USER_ERROR(e,
			"failed to construct container/table name");
	}
}
	
FullContainerKey::FullContainerKey(
								   const KeyConstraint &constraint,
								   DatabaseId dbId, const char8_t *str, uint32_t length) :
	constraint_(constraint), size_(0) {
	try {
		bytes_.clear();

		FullContainerKeyComponents components;
		BitArray upperCaseBit(DEFAULT_UPPER_CASE_BIT_LENGTH);

		parseAndValidate(dbId, str, length, components, upperCaseBit);
		serialize(components, upperCaseBit);

	}
	catch (std::exception &e) {
		GS_COMMON_RETHROW_USER_ERROR(e,
			"failed to construct container/table name");
	}
}

FullContainerKey::FullContainerKey(
								   const KeyConstraint &constraint,
								   const FullContainerKeyComponents &components) :
	constraint_(constraint), size_(0) {
	try {
		bytes_.clear();
		
		validate(components);

		BitArray upperCaseBit(components.getStringSize());
		setUpperCaseBit(components, upperCaseBit);

		serialize(components, upperCaseBit);

	}
	catch (std::exception &e) {
		GS_COMMON_RETHROW_USER_ERROR(e,
			"failed to construct container/table name");
	}
}

FullContainerKey::~FullContainerKey() {
}

FullContainerKey& FullContainerKey::operator=(const FullContainerKey &another) {
	if (this == &another) {
		return *this;
	}
	constraint_ = another.constraint_;
	bytes_ = another.bytes_;
	size_ = another.size_;
	return *this;
}

FullContainerKeyComponents FullContainerKey::getComponents(
														   bool unNormalized) const {
	FullContainerKeyComponents components;
	BitArray upperCaseBit(DEFAULT_UPPER_CASE_BIT_LENGTH);

	deserialize(components, upperCaseBit, unNormalized);

	return components;
}

void FullContainerKey::toString(util::BasicString<char8_t> &str) const {
	try {
		FullContainerKeyComponents components;
		BitArray upperCaseBit(DEFAULT_UPPER_CASE_BIT_LENGTH);

		deserialize(components, upperCaseBit, true);

		str.clear();
		if (components.baseNameSize_ > 0) {
			str.append(&components.baseName_[0], components.baseNameSize_);
		}

		if (components.systemPartId_ != UNDEF_SYSTEM_PART_ID) {
			util::NormalOStringStream oss;
			oss << components.systemPartId_;
			str.append("#");
			str.append(oss.str().c_str());
		}
		else if (!components.systemPart_.empty() && components.systemPartSize_ > 0) {
			str.append("#");
			str.append(&components.systemPart_[0], components.systemPartSize_);
		}

		if (components.largeContainerId_ != UNDEF_LARGE_CONTAINERID) {
			util::NormalOStringStream oss;
			oss << components.largeContainerId_;
			str.append("@");
			str.append(oss.str().c_str());
		}

		if (components.affinityNumber_ != UNDEF_NODE_AFFINITY_NUMBER) {
			util::NormalOStringStream oss;
			oss << components.affinityNumber_;
			str.append("@");
			str.append(oss.str().c_str());
		}
		else if (!components.affinityString_.empty() && components.affinityStringSize_ > 0) {
			str.append("@");
			str.append(&components.affinityString_[0], components.affinityStringSize_);
		}

	}
	catch (std::exception &e) {
		UTIL_RETHROW(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID, e,
			"failed to get container/table name");
	}
}

void FullContainerKey::toBinary(const void *&body, size_t &size) const {
	body = &bytes_[0];
	size = size_;
}

bool FullContainerKey::isEmpty() const {
	return bytes_.empty();
}

int32_t FullContainerKey::compareTo(
									const FullContainerKey &key, bool caseSensitive) const {
	try {
		if (isEmpty()) {
			return (key.isEmpty() ? 0 : 1);
		}
		else if (key.isEmpty()) {
			return -1;
		}

		const FullContainerKeyComponents components1 = getComponents(caseSensitive);
		BitArray upperCaseBit1(components1.getStringSize());
		setUpperCaseBit(components1, upperCaseBit1);

		const FullContainerKeyComponents components2 = key.getComponents(caseSensitive);
		BitArray upperCaseBit2(components2.getStringSize());
		setUpperCaseBit(components2, upperCaseBit2);

		if (components1.dbId_ > components2.dbId_) {
			return 1;
		}
		else if (components1.dbId_ < components2.dbId_) {
			return -1;
		}


		uint64_t startPos1 = 0;
		uint64_t startPos2= 0;

		int32_t result = compareNormalizedString(
			&components1.baseName_[0], components1.baseNameSize_,
			upperCaseBit1, startPos1,
			&components2.baseName_[0], components2.baseNameSize_,
			upperCaseBit2, startPos2,
			caseSensitive);
		startPos1 += components1.baseNameSize_;
		startPos2 += components2.baseNameSize_;

		if (result != 0) {
			return result;
		}
		else if (components1.largeContainerId_ != UNDEF_LARGE_CONTAINERID) {
			if (components2.largeContainerId_ != UNDEF_LARGE_CONTAINERID) {
				result = static_cast<int32_t>(components1.largeContainerId_ - components2.largeContainerId_);
			}
			else {
				result = 1;
			}
		}

		if (result != 0) {
			return result;
		}
		else if (components1.affinityNumber_ != UNDEF_NODE_AFFINITY_NUMBER) {
			if (components2.affinityNumber_ != UNDEF_NODE_AFFINITY_NUMBER) {
				result = static_cast<int32_t>(components1.affinityNumber_ - components2.affinityNumber_);
			}
			else {
				result = 1;
			}
		}
		else if (components1.affinityStringSize_ > 0) {
			if (components2.affinityNumber_ != UNDEF_NODE_AFFINITY_NUMBER) {
				return -1;
			}
			else if (components2.affinityStringSize_ > 0) {
				result = compareNormalizedString(
					&components1.affinityString_[0], components1.affinityStringSize_,
					upperCaseBit1, startPos1,
					&components2.affinityString_[0], components2.affinityStringSize_,
					upperCaseBit2, startPos2,
					caseSensitive);
				startPos1 += components1.affinityStringSize_;
				startPos2 += components2.affinityStringSize_;
			}
			else {
				result = 1;
			}
		}
		else {
			if (components2.affinityNumber_ != UNDEF_NODE_AFFINITY_NUMBER
				|| components2.affinityStringSize_ > 0) {
				result = -1;
			}
			else {
				result = 0;
			}
		}

		if (result != 0) {
			return result;
		}
		else if (components1.systemPartId_ != UNDEF_SYSTEM_PART_ID) {
			if (components2.systemPartId_ != UNDEF_SYSTEM_PART_ID) {
				result = static_cast<int32_t>(components1.systemPartId_ - components2.systemPartId_);
			}
			else {
				result = 1;
			}
		}
		else if (components1.systemPartSize_ > 0 ) {
			if (components2.systemPartId_ != UNDEF_SYSTEM_PART_ID) {
				result = -1;
			}
			else if (components2.systemPartSize_ > 0) {
				result = compareNormalizedString(
					&components1.systemPart_[0], components1.systemPartSize_,
					upperCaseBit1, startPos1,
					&components2.systemPart_[0], components2.systemPartSize_,
					upperCaseBit2, startPos2,
					caseSensitive);
				startPos1 += components1.systemPartSize_;
				startPos2 += components2.systemPartSize_;
			}
			else {
				result = 1;
			}
		}
		else {
			if (components2.systemPartId_ != UNDEF_NODE_AFFINITY_NUMBER
				|| components2.systemPartSize_ > 0) {
				result = -1;
			}
			else {
				result = 0;
			}
		}

		return result;

	}
	catch (std::exception &e) {
		UTIL_RETHROW(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID, e,
			"failed to compare container/table name");
	}
}

void FullContainerKey::parseAndValidate(DatabaseId dbId, const char8_t *str, uint32_t length,
										FullContainerKeyComponents &components,
										BitArray &upperCaseBit) const {
	try {
		size_t len = length;
		if (len <= 0) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
				"size of container/table name is zero");
		}
		if (len > constraint_.maxTotalLength_) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
				"size of container/table name exceeds maximum size");
		}

		size_t partCounter = 0;
		size_t sharpCounter = 0;
		size_t atmarkCounter = 0;
		const char8_t *part[4] = { NULL, NULL, NULL, NULL };
		uint32_t partLen[4] = { 0, 0, 0, 0 };

		part[partCounter] = str;

		for (size_t i = 0; i < len; i++) {
			if (str[i] == '#') {
				sharpCounter++;
				if (sharpCounter > 1) {
					GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
						"system part must be exactly one");
				}
				else if (atmarkCounter > 0) {
					GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
						"system part can not be described after node affinity");
				}

				partCounter++;
				if (i + 1 <= len) {
					part[partCounter] = str + (i+1);
				}

			}
			else if (str[i] == '@') {
				atmarkCounter++;
				if (atmarkCounter > 2) {
					GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
						"node affinity or largeId can not be described twice or more");
				}

				partCounter++;
				if (i + 1 <= len) {
					part[partCounter] = str + (i+1);
				}

			}
			else {
				partLen[partCounter]++;
			}
		}

		assert(partCounter == sharpCounter + atmarkCounter);


		components.clear();

		components.dbId_ = dbId;
		validateDbId(components.dbId_);

		components.baseName_ = part[0];
		components.baseNameSize_ = partLen[0];
		const bool hasSystemPart = (sharpCounter == 1);
		const bool hasLargeContainerId = (atmarkCounter > 1);

		if (!constraint_.systemPartAllowed_ && hasSystemPart) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
				"system part is not allowed");
		}
		if (!constraint_.largeContainerIdAllowed_ && hasLargeContainerId) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
				"largeId is not allowed");
		}

		validateBaseContainerName(
			&components.baseName_[0], components.baseNameSize_, hasSystemPart);

		if (sharpCounter == 0) {
			if (atmarkCounter == 1) {
				validateAndSetNodeAffinity(part[1], partLen[1], components);
			}
			else if (atmarkCounter == 2) {
				validateAndSetLargeContainerId(part[1], partLen[1], components);
				validateAndSetNodeAffinity(part[2], partLen[2], components);
			}
		}
		else {
			validateAndSetSystemPart(part[1], partLen[1], components);
			if (atmarkCounter == 1) {
				validateAndSetNodeAffinity(part[2], partLen[2], components);
			}
			else if (atmarkCounter == 2) {
				validateAndSetLargeContainerId(part[2], partLen[2], components);
				validateAndSetNodeAffinity(part[3], partLen[3], components);
			}
		}

		setUpperCaseBit(components, upperCaseBit);

	}
	catch (std::exception &e) {
		UTIL_RETHROW(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID, e,
			"failed to parse and validate container/table name : " << str);
	}
}

void FullContainerKey::validate(const FullContainerKeyComponents &components) const {
	try {
		uint64_t totalLength = 0;

		validateDbId(components.dbId_);

		const bool hasSystemPart =
			(components.systemPartId_ != UNDEF_SYSTEM_PART_ID || components.systemPartSize_ > 0);
		const bool hasLargeContainerId = (components.largeContainerId_ != UNDEF_LARGE_CONTAINERID);

		if (!constraint_.systemPartAllowed_ && hasSystemPart) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
				"system part is not allowed");
		}
		if (!constraint_.largeContainerIdAllowed_ && hasLargeContainerId) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
				"largeId is not allowed");
		}

		validateBaseContainerName(
			&components.baseName_[0], components.baseNameSize_, hasSystemPart);
		totalLength += components.baseNameSize_;

		if (components.largeContainerId_ != UNDEF_LARGE_CONTAINERID) {
			validateLargeContainerId(components.largeContainerId_);
			util::NormalOStringStream oss;
			oss << components.largeContainerId_;
			totalLength += oss.str().length();
			totalLength++;
		}
		if (components.affinityNumber_ != UNDEF_NODE_AFFINITY_NUMBER) {
			validateAffinityNumber(components.affinityNumber_);
			util::NormalOStringStream oss;
			oss << components.affinityNumber_;
			totalLength += oss.str().length();
			totalLength++;
		}
		else if (components.affinityStringSize_ > 0) {
			validateExtendedName(&components.affinityString_[0], components.affinityStringSize_, "node affinity");
			totalLength += components.affinityStringSize_;
			totalLength++;
		}

		if (components.systemPartId_ != UNDEF_SYSTEM_PART_ID) {
			validateSystemPartId(components.systemPartId_);
			util::NormalOStringStream oss;
			oss << components.systemPartId_;
			totalLength += oss.str().length();
			totalLength++;
		}
		else if (components.systemPartSize_ > 0) {
			validateExtendedName(&components.systemPart_[0], components.systemPartSize_, "system part");
			totalLength += components.systemPartSize_;
			totalLength++;
		}

		if (totalLength <= 0) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
				"size of container/table name is zero");
		}
		if (totalLength > constraint_.maxTotalLength_) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
				"size of container/table name exceeds maximum size");
		}

	}
	catch (std::exception &e) {
		UTIL_RETHROW(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID, e,
			"failed to validate container/table name");
	}
}

void FullContainerKey::setUpperCaseBit(const FullContainerKeyComponents &components,
									   BitArray &upperCaseBit) const {
	upperCaseBit.clear();

	const char8_t *strList[3] = { &components.baseName_[0], &components.affinityString_[0], &components.systemPart_[0] };
	uint32_t lengthList[3] = { components.baseNameSize_, components.affinityStringSize_, components.systemPartSize_ };

	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < lengthList[i]; j++) {
			upperCaseBit.append(isupper(strList[i][j]) != 0);
		}
	}
}

void FullContainerKey::serialize(
								 const FullContainerKeyComponents &components,
								 const BitArray &upperCaseBit) {
	try {
		util::NormalXArray<char8_t> buf;
		buf.resize(static_cast<size_t>(components.getStringSize()), '\0');

		util::NormalXArray<uint8_t> binary;
		util::NormalOutStream arrayOut(binary);
		ContainerKeyOutStream out(arrayOut);

		const size_t flagPos = out.base().position();

		uint8_t flag = 0;
		out << flag;

		if (components.dbId_ != GS_PUBLIC_DB_ID) {
			flag |= DBID_EXISTS;
			out << components.dbId_;
		}

		{
			ValueProcessor::convertLowerCase(
				&components.baseName_[0], components.baseNameSize_, buf.data());
			encodeString(out, buf.data(), components.baseNameSize_);
		}

		if (components.largeContainerId_ != UNDEF_LARGE_CONTAINERID) {
			flag |= LARGE_CONTAINERID_EXISTS;
			encodeVarLong(out, components.largeContainerId_);
		}

		if (components.affinityNumber_ != UNDEF_NODE_AFFINITY_NUMBER) {
			flag |= NODE_AFFINITY_NUM;
			encodeVarLong(out, components.affinityNumber_);
		}
		else if (components.affinityStringSize_ > 0) {
			flag |= NODE_AFFINITY_STR;
			ValueProcessor::convertLowerCase(
				&components.affinityString_[0], components.affinityStringSize_, buf.data());
			encodeString(out, buf.data(), components.affinityStringSize_);
		}

		if (components.systemPartId_ != UNDEF_SYSTEM_PART_ID) {
			flag |= SYSTEM_PART_ID_NUM;
			encodeVarLong(out, components.systemPartId_);
		}
		else if (components.systemPartSize_ > 0) {
			flag |= SYSTEM_PART_ID_STR;
			ValueProcessor::convertLowerCase(
				&components.systemPart_[0], components.systemPartSize_, buf.data());
			encodeString(out, buf.data(), components.systemPartSize_);
		}

		out.writeAll(upperCaseBit.data(), upperCaseBit.byteLength());

		const size_t lastPos = out.base().position();
		out.base().position(flagPos);
		out << flag;
		out.base().position(lastPos);

		if (binary.size() >= static_cast<size_t>(UINT32_MAX)) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
				"size of serialized container/table name exceeds maximum size");
		}

		size_ = binary.size();
		const uint8_t *it = static_cast<const uint8_t*>(binary.data());
		bytes_.assign(it, it + size_);

	}
	catch (std::exception &e) {
		UTIL_RETHROW(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID, e,
			"failed to serialize container/table name");
	}
}

void FullContainerKey::deserialize(
								   FullContainerKeyComponents &components,
								   BitArray &upperCaseBit,
								   bool unNormalized) const {
	try {
		if (isEmpty()) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
				"container/table name is empty");
		}

		components.clear();
		upperCaseBit.clear();

		ContainerKeyInStream in(util::ArrayInStream(&bytes_[0], size_));

		const char8_t *normalizedStr[3] = { NULL, NULL, NULL };

		uint8_t flag;
		in >> flag;

		if (flag & DBID_EXISTS) {
			in >> components.dbId_;
		}
		else {
			components.dbId_ = GS_PUBLIC_DB_ID;
		}

		components.baseNameSize_ = decodeVarInt(in);
		normalizedStr[0] = reinterpret_cast<const char8_t*>(&bytes_[0] + in.base().position());
		in.base().position(in.base().position() + components.baseNameSize_);

		if (flag & LARGE_CONTAINERID_EXISTS) {
			components.largeContainerId_ = decodeVarLong(in);
		}

		if (flag & NODE_AFFINITY_NUM) {
			components.affinityNumber_ = decodeVarLong(in);
		}
		else if (flag & NODE_AFFINITY_STR) {
			components.affinityStringSize_ = decodeVarInt(in);
			normalizedStr[1] = reinterpret_cast<const char8_t*>(&bytes_[0] + in.base().position());
			in.base().position(in.base().position() + components.affinityStringSize_);
		}

		if (flag & SYSTEM_PART_ID_NUM) {
			components.systemPartId_ = decodeVarLong(in);
		}
		else if (flag & SYSTEM_PART_ID_STR) {
			components.systemPartSize_ = decodeVarInt(in);
			normalizedStr[2] = reinterpret_cast<const char8_t*>(&bytes_[0] + in.base().position());
			in.base().position(in.base().position() + components.systemPartSize_);
		}

		const uint64_t strLength = components.baseNameSize_
			+ components.affinityStringSize_
			+ components.systemPartSize_;

		if (in.base().remaining() != strLengthToBitLength(strLength)) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
				"size of container/table name is invalid");
		}
		else {
			upperCaseBit.reserve(strLength);
			upperCaseBit.putAll(&bytes_[0] + in.base().position(), strLength);
			in.base().position(in.base().position() + in.base().remaining());
		}

		if (in.base().position() != size_) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
				"size of container/table name is invalid");
		}


		if (unNormalized) {
			util::NormalXArray<char8_t> buf;
			buf.resize(static_cast<size_t>(strLength), '\0');
			uint64_t startPos = 0;

			createOriginalString(normalizedStr[0], components.baseNameSize_,
				buf.data()+startPos, upperCaseBit, startPos);
			components.baseName_.append(buf.data()+startPos, components.baseNameSize_);
			startPos += components.baseNameSize_;

			if (components.affinityStringSize_ > 0) {
				createOriginalString(normalizedStr[1], components.affinityStringSize_,
					buf.data()+startPos, upperCaseBit, startPos);
				components.affinityString_.append(buf.data()+startPos, components.affinityStringSize_);
				startPos += components.affinityStringSize_;
			}

			if (components.systemPartSize_ > 0) {
				createOriginalString(normalizedStr[2], components.systemPartSize_,
					buf.data()+startPos, upperCaseBit, startPos);
				components.systemPart_.append(buf.data()+startPos, components.systemPartSize_);
				startPos += components.systemPartSize_;
			}
		}
		else {
			components.baseName_.append(normalizedStr[0], components.baseNameSize_);
			if (components.affinityStringSize_ > 0) {
				components.affinityString_.append(normalizedStr[1], components.affinityStringSize_);
			}
			if (components.systemPartSize_ > 0) {
				components.systemPart_.append(normalizedStr[2], components.systemPartSize_);
			}
		}

	}
	catch (std::exception &e) {
		UTIL_RETHROW(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID, e,
			"failed to deserialize container/table name");
	}
}

void FullContainerKey::clear() {
	bytes_.clear();
	size_ = 0;
}

uint64_t FullContainerKey::strLengthToBitLength(uint64_t strLength) const {
	return (strLength / (sizeof(uint8_t) * CHAR_BIT)
			+ (strLength % (sizeof(uint8_t) * CHAR_BIT) > 0 ? 1 : 0));
}

uint32_t FullContainerKey::getEncodedVarSize32(uint32_t val) const {
	return ValueProcessor::getEncodedVarSize(val);
}

uint32_t FullContainerKey::getEncodedVarSize64(uint64_t val) const {
	return ValueProcessor::getEncodedVarSize(val);
}

void FullContainerKey::encodeVarInt(ContainerKeyOutStream &out,
									uint32_t val) const {
	if (val > VAR_SIZE_4BYTE_THRESHOLD) {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
			"too large value to encode : " << val);
	}
	switch (getEncodedVarSize32(val)) {
		case 1:
			out << ValueProcessor::encode1ByteVarSize(static_cast<uint8_t>(val));
			break;
		case 4:
			out << ValueProcessor::encode4ByteVarSize(val);
			break;
		default:
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
				"value is not encodable : " << val);
	}
}

void FullContainerKey::encodeVarLong(ContainerKeyOutStream &out,
									 uint64_t val) const {
	if (val > VAR_SIZE_8BYTE_THRESHOLD) {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
			"too large value to encode : " << val);
	}
	switch (getEncodedVarSize64(val)) {
		case 1:
			out << ValueProcessor::encode1ByteVarSize(static_cast<uint8_t>(val));
			break;
		case 4:
			out << ValueProcessor::encode4ByteVarSize(static_cast<uint32_t>(val));
			break;
		case 8:
			out << ValueProcessor::encode8ByteVarSize(val);
			break;
		default:
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
				"value is not encodable : " << val);
	}
}

void FullContainerKey::encodeString(ContainerKeyOutStream &out,
									const char8_t *str, uint32_t len) const {
	encodeVarInt(out, len);
	out.writeAll(str, len);
}

void FullContainerKey::encodeBinary(ContainerKeyOutStream &out,
								 const uint8_t *data, uint32_t len) const {
	encodeVarInt(out, len);
	out.writeAll(data, len);
}

uint32_t FullContainerKey::decodeVarInt(ContainerKeyInStream &in) const {
	return ValueProcessor::getVarSize(in);
}

uint64_t FullContainerKey::decodeVarLong(ContainerKeyInStream &in) const {
	uint64_t currentPos = in.base().position();
	uint8_t byteData;
	in >> byteData;
	if (ValueProcessor::varSizeIs1Byte(byteData)) {
		return ValueProcessor::decode1ByteVarSize(byteData);
	} else if (ValueProcessor::varSizeIs4Byte(byteData)) {
		in.base().position(static_cast<size_t>(currentPos));
		uint32_t rawData;
		in >> rawData;
		return ValueProcessor::decode4ByteVarSize(rawData);
	} else if (ValueProcessor::varSizeIs8Byte(byteData)) {
		in.base().position(static_cast<size_t>(currentPos));
		uint64_t rawData;
		in >> rawData;
		return ValueProcessor::decode8ByteVarSize(rawData);
	} else {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
			"size of encoded value is invalid");
	}
}

bool FullContainerKey::isEncodableVarInt(uint32_t val) const {
	return (val < VAR_SIZE_4BYTE_THRESHOLD);
}

bool FullContainerKey::isEncodableVarLong(uint64_t val) const {
	return (val < VAR_SIZE_8BYTE_THRESHOLD);
}

bool FullContainerKey::isSymbol(char8_t ch) {
	bool match = false;
	for (size_t i = 0; i < strlen(symbol) && !match; i++) {
		match = (ch == symbol[i]);
	}
	return match;
}

void FullContainerKey::createOriginalString(char8_t const *src, uint32_t size, char8_t *dest,
											const BitArray &upperCaseBit, uint64_t startPos) const {
	for (uint32_t i = 0; i < size; i++) {
		const char8_t c = *(src + i);
		if (upperCaseBit.get(startPos + i)) {
			*(dest + i) = c - 32;
		}
		else {
			*(dest + i) = c;
		}
	}
}

void FullContainerKey::validateDbId(DatabaseId dbId) const {
	if (dbId < 0 || dbId > MAX_DBID) {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
			"invalid database id : " << dbId);
	}
}

void FullContainerKey::validateBaseContainerName(const char8_t *baseName,
												 uint32_t baseNameLength,
												 bool systemPartExists) const {
	const char8_t *str = baseName;
	const uint32_t len = baseNameLength;
	if ((str == NULL || len <= 0) && !systemPartExists) {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
			"size of base name is zero");
	}
	else if (len > constraint_.maxTotalLength_) {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
			"size of base name exceeds maximum size : " << str);
	}

	for (size_t i = 0; forbiddenPattern[i] != NULL; i++) {
		if (compareOriginalString(baseName, baseNameLength,
				forbiddenPattern[i], static_cast<uint32_t>(strlen(forbiddenPattern[i])),
				false) == 0) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
				"base name contains forbidden patterns : " << forbiddenPattern[i]);
		}
	}
	
	for (uint32_t i = 0; i < len; i++) {
		const unsigned char ch = static_cast<unsigned char>(str[i]);
		if (!isalpha(ch) && !isdigit(ch) && !isSymbol(ch)) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
				"base name contains forbidden characters : " << ch);
		}
	}
}

bool FullContainerKey::validateExtendedName(const char8_t *str, uint32_t len,
											const char8_t *partName) const {
	bool allNum = true;

	if (str == NULL || len <= 0) {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
			"size of " << partName << " string is zero : " << str);
	}
	if (len > constraint_.maxTotalLength_) {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
			"size of " << partName << " string exceeds maximum size : " << str);
	}

	for (uint32_t i = 0; i < len; i++) {
		const unsigned char ch = static_cast<unsigned char>(str[i]);
		if (isdigit(ch)) {
			allNum &= true;
		}
		else if (isalpha(ch) || isSymbol(ch)) {
			allNum &= false;
		}
		else {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
				partName << "contains forbidden characters : " << ch);
		}
	}

	if (allNum) {
		if (len > 1 && str[0] == '0') {
			allNum = false;
		}
		else if (len > MAX_NODE_AFFINITY_NUMBER_DIGITS) {
			allNum = false;
		}
	}

	return allNum;
}

void FullContainerKey::validateNumeric(const char8_t *str, uint32_t len,
									   const char8_t *partName) const {
	if (str == NULL || len <= 0) {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
			"size of " << partName << " string is zero");
	}
	if (len > constraint_.maxTotalLength_) {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
			"size of " << partName << " string exceeds maximum size");
	}

	for (uint32_t i = 0; i < len; i++) {
		const unsigned char ch = static_cast<unsigned char>(str[i]);
		if (!isdigit(ch)) {
			GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
				partName << "contains forbidden characters : " << ch);
		}
	}

	if (len > 1 && str[0] == '0') {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
			partName << " is not numeric");
	}
	else if (len > MAX_LARGE_CONTAINERID_DIGITS) {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_CM_LIMITS_EXCEEDED,
			"number of digits of " << partName << " exceeds maximum size");
	}
}

void FullContainerKey::validateAffinityNumber(NodeAffinityNumber affinityNumber) const {
	if (!isEncodableVarLong(affinityNumber)) {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
			"invalid node affinity : " << affinityNumber);
	}
}

void FullContainerKey::validateLargeContainerId(LargeContainerId largeContainerId) const {
	if (!isEncodableVarLong(largeContainerId)) {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
			"invalid largeId : " << largeContainerId);
	}
}

void FullContainerKey::validateSystemPartId(SystemPartId systemPartId) const {
	if (!isEncodableVarLong(systemPartId)) {
		GS_COMMON_THROW_USER_ERROR(GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
			"invalid system internal id : " << systemPartId);
	}
}

void FullContainerKey::validateAndSetNodeAffinity(const char8_t *str, uint32_t len,
												  FullContainerKeyComponents &components) const {
	if (validateExtendedName(str, len, "node affinity")) {
		const NodeAffinityNumber affinityNumber = getNodeAffinityNumber(str, len);
		if (isEncodableVarLong(affinityNumber)) {
			components.affinityNumber_ = affinityNumber;
		}
		else {
			components.affinityString_.append(str, len);
			components.affinityStringSize_ = len;
		}
	}
	else {
		components.affinityString_.append(str, len);
		components.affinityStringSize_ = len;
	}
}

void FullContainerKey::validateAndSetLargeContainerId(const char8_t *str, uint32_t len,
													  FullContainerKeyComponents &components) const {
	validateNumeric(str, len, "largeId");
	components.largeContainerId_ = getLargeContainerId(str, len);
	validateLargeContainerId(components.largeContainerId_);
}

void FullContainerKey::validateAndSetSystemPart(const char8_t *str, uint32_t len,
												FullContainerKeyComponents &components) const {
	if (validateExtendedName(str, len, "system part")) {
		const SystemPartId systemPartId = getSystemPartId(str, len);
		if (isEncodableVarLong(systemPartId)) {
			components.systemPartId_ = systemPartId;
		}
		else {
			components.systemPart_.append(str, len);
			components.systemPartSize_ = len;
		}
	}
	else {
		components.systemPart_.append(str, len);
		components.systemPartSize_ = len;
	}
}

NodeAffinityNumber FullContainerKey::getNodeAffinityNumber(const char8_t *affinityString,
												   uint32_t affinityStringLength) const {
	if (affinityString == NULL || affinityStringLength == 0) {
		return UNDEF_NODE_AFFINITY_NUMBER;
	}
	else {
		char8_t tmp[MAX_NODE_AFFINITY_NUMBER_DIGITS + 1];
		memcpy(tmp, affinityString, affinityStringLength*sizeof(char8_t));
		tmp[affinityStringLength] = '\0';
		util::NormalIStringStream iss(tmp);
		NodeAffinityNumber affinityNumber;
		iss >> affinityNumber;
		return affinityNumber;
	}
}

ContainerId FullContainerKey::getLargeContainerId(const char8_t *largeContainerIdString,
												  uint32_t largeContainerIdStringLength) const {
	if (largeContainerIdString == NULL || largeContainerIdStringLength == 0) {
		return UNDEF_LARGE_CONTAINERID;
	}
	else {
		char8_t tmp[MAX_LARGE_CONTAINERID_DIGITS + 1];
		memcpy(tmp, largeContainerIdString, largeContainerIdStringLength*sizeof(char8_t));
		tmp[largeContainerIdStringLength] = '\0';
		util::NormalIStringStream iss(tmp);
		ContainerId largeContainerId;
		iss >> largeContainerId;
		return largeContainerId;
	}
}

SystemPartId FullContainerKey::getSystemPartId(const char8_t *systemPart,
											   uint32_t systemPartLength) const {
	if (systemPart == NULL || systemPartLength == 0) {
		return UNDEF_SYSTEM_PART_ID;
	}
	else {
		char8_t tmp[MAX_SYSTEM_PART_ID_DIGITS + 1];
		memcpy(tmp, systemPart, systemPartLength*sizeof(char8_t));
		tmp[systemPartLength] = '\0';
		util::NormalIStringStream iss(tmp);
		SystemPartId systemPartId;
		iss >> systemPartId;
		return systemPartId;
	}
}

int32_t FullContainerKey::compareOriginalString(
		const char8_t *str1, uint32_t str1Length,
		const char8_t *str2, uint32_t str2Length,
		bool caseSensitive) const {

	const size_t len = std::min(str1Length, str2Length);

	int32_t result = 0;

	if (caseSensitive) {
		result = strncmp(str1, str2, len);
	}
	else {
		for (size_t i = 0; i < len && result == 0; i++) {
			int ch1 = static_cast<int>(str1[i]);
			int ch2 = static_cast<int>(str2[i]);

			result = ch1 - ch2;

			if (result != 0) {
				if (!caseSensitive && isalpha(ch1) && isalpha(ch2)) {
					ch1 = tolower(ch1);
					ch2 = tolower(ch2);
					result = ch1 - ch2;
				}
			}
		}
	}

	if (result == 0) {
		result = str1Length - str2Length;
	}

	return result;
}

int32_t FullContainerKey::compareNormalizedString(
		const char8_t *str1, uint32_t str1Length, const BitArray &upperCaseBit1, uint64_t startPos1,
		const char8_t *str2, uint32_t str2Length, const BitArray &upperCaseBit2, uint64_t startPos2,
		bool caseSensitive) const {

	const size_t len = std::min(str1Length, str2Length);

	int32_t result = strncmp(str1, str2, len);

	if (caseSensitive) {
		for (uint64_t i = 0; i < len && result == 0; i++) {
			const bool bit1 = upperCaseBit1.get(startPos1+i);
			const bool bit2 = upperCaseBit2.get(startPos2+i);
			if (bit1 != bit2) {
				result = (bit1 ? -1 : 1);
			}
		}
	}

	if (result == 0) {
		result = str1Length - str2Length;
	}

	return result;
}
