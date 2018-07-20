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
	@brief Definition of FullContainerKey
*/
#ifndef FULL_CONTAINER_KEY_H_
#define FULL_CONTAINER_KEY_H_


#include "util/type.h"
typedef int64_t RowId; /*!< Row ID (unique witihin a container) */
const RowId UNDEF_ROWID = INT64_MAX;
const RowId MAX_ROWID = UNDEF_ROWID - 1;
typedef RowId DatabaseId;
typedef uint64_t ContainerId; /*!< Container ID (unique witihin a partition) */

static const uint32_t VAR_SIZE_1BYTE_THRESHOLD = 128;
static const uint32_t VAR_SIZE_4BYTE_THRESHOLD = UINT32_C(1) << 30;
static const uint64_t VAR_SIZE_8BYTE_THRESHOLD = UINT64_C(1) << 62;
static const DatabaseId UNDEF_DBID = UNDEF_ROWID;
static const DatabaseId GS_PUBLIC_DB_ID = 0;
static const DatabaseId MAX_DBID = MAX_ROWID;
#include "bit_array.h"
#include "util/allocator.h"


class TransactionContext;

typedef uint64_t NodeAffinityNumber;
const NodeAffinityNumber MAX_NODE_AFFINITY_NUMBER = VAR_SIZE_8BYTE_THRESHOLD-1;
const NodeAffinityNumber UNDEF_NODE_AFFINITY_NUMBER = UINT64_MAX;

typedef NodeAffinityNumber LargeContainerId;
const LargeContainerId MAX_LARGE_CONTAINERID = MAX_NODE_AFFINITY_NUMBER;
const LargeContainerId UNDEF_LARGE_CONTAINERID = UNDEF_NODE_AFFINITY_NUMBER;

typedef NodeAffinityNumber SystemPartId;
const SystemPartId MAX_SYSTEM_PART_ID = MAX_NODE_AFFINITY_NUMBER;
const SystemPartId UNDEF_SYSTEM_PART_ID = UNDEF_NODE_AFFINITY_NUMBER;

struct KeyConstraint {
	uint32_t maxTotalLength_;
	bool systemPartAllowed_;
	bool largeContainerIdAllowed_;

	KeyConstraint();
	KeyConstraint(
		uint32_t maxTotalLength,
		bool systemPartAllowed,
		bool largeContainerIdAllowed);
};

struct FullContainerKeyComponents {
	DatabaseId dbId_;

	std::string baseName_;
	uint32_t baseNameSize_;

	std::string affinityString_;
	uint32_t affinityStringSize_;
	NodeAffinityNumber affinityNumber_;

	LargeContainerId largeContainerId_;

	std::string systemPart_;
	uint32_t systemPartSize_;
	SystemPartId systemPartId_;

	FullContainerKeyComponents();
	FullContainerKeyComponents& operator=(const FullContainerKeyComponents &another);

	uint32_t getStringSize() const;

	void clear();
};

class FullContainerKey {
public:
	FullContainerKey(const KeyConstraint &constraint);

	FullContainerKey(
		const KeyConstraint &constraint,
		const void *body, size_t size);
	
	FullContainerKey(
		const KeyConstraint &constraint,
		DatabaseId dbId, const char8_t *str, uint32_t length);
	
	FullContainerKey(
		const KeyConstraint &constraint,
		const FullContainerKeyComponents &components);
	
	virtual ~FullContainerKey();

	FullContainerKey& operator=(const FullContainerKey &another);

	FullContainerKeyComponents getComponents(
		bool unNormalized = true) const;

	void toString(util::BasicString<char8_t> &str) const;
	
	void toBinary(const void *&body, size_t &size) const;

	bool isEmpty() const;

	int32_t compareTo(
		const FullContainerKey &key, bool caseSensitive) const;

protected:
	typedef util::ByteStream< util::NormalOutStream > ContainerKeyOutStream;
	typedef util::ByteStream<util::ArrayInStream> ContainerKeyInStream;

	KeyConstraint constraint_;

	std::vector<uint8_t> bytes_;
	size_t size_;

	static const char8_t *symbol;
	static const char8_t *extendedSymbol;
	static const char8_t *forbiddenPattern[];

	static const DatabaseId MAX_ENCODABLE_DBID;

	static const size_t MAX_NODE_AFFINITY_NUMBER_DIGITS;
	static const size_t MAX_LARGE_CONTAINERID_DIGITS;
	static const size_t MAX_SYSTEM_PART_ID_DIGITS;

	static const uint8_t DBID_EXISTS;
	static const uint8_t LARGE_CONTAINERID_EXISTS;
	static const uint8_t NODE_AFFINITY_STR;
	static const uint8_t NODE_AFFINITY_NUM;
	static const uint8_t NODE_AFFINITY_EXISTS;
	static const uint8_t SYSTEM_PART_ID_STR;
	static const uint8_t SYSTEM_PART_ID_NUM;
	static const uint8_t SYSTEM_PART_ID_EXISTS;

	static const uint64_t DEFAULT_UPPER_CASE_BIT_LENGTH;


	void parseAndValidate(
		DatabaseId dbId, const char8_t *str, uint32_t length,
		FullContainerKeyComponents &components, BitArray &upperCaseBit) const;

	void validate(const FullContainerKeyComponents &components) const;

	void setUpperCaseBit(const FullContainerKeyComponents &components,
		BitArray &upperCaseBit) const;

	void serialize(
		const FullContainerKeyComponents &components, const BitArray &upperCaseBit);

	void deserialize(
		FullContainerKeyComponents &components, BitArray &upperCaseBit,
		bool unNormalized) const;

	void clear();


	uint64_t strLengthToBitLength(uint64_t strLength) const;

	uint32_t getEncodedVarSize32(uint32_t val) const;
	uint32_t getEncodedVarSize64(uint64_t val) const;

	void encodeVarInt(ContainerKeyOutStream &out, uint32_t val) const;
	void encodeVarLong(ContainerKeyOutStream &out, uint64_t val) const;
	void encodeString(ContainerKeyOutStream &out, const char8_t *str, uint32_t len) const;
	void encodeBinary(ContainerKeyOutStream &out, const uint8_t *data, uint32_t len) const;

	uint32_t decodeVarInt(ContainerKeyInStream &in) const;
	uint64_t decodeVarLong(ContainerKeyInStream &in) const;

	bool isEncodableVarInt(uint32_t val) const;
	bool isEncodableVarLong(uint64_t val) const;


	static bool isSymbol(char8_t ch);

	void createOriginalString(
		char8_t const *src, uint32_t size, char8_t *dest,
		const BitArray &upperCaseBit, uint64_t startPos) const;

	void validateDbId(DatabaseId dbId) const;
	void validateBaseContainerName(
		const char8_t *baseName, uint32_t baseNameLength, bool systemPartExists) const;
	bool validateExtendedName(
		const char8_t *str, uint32_t len, const char8_t *partName = "part") const;
	void validateNumeric(
		const char8_t *str, uint32_t len, const char8_t *partName = "part") const;
	void validateAffinityNumber(NodeAffinityNumber affinityNumber) const;
	void validateLargeContainerId(LargeContainerId largeContainerId) const;
	void validateSystemPartId(SystemPartId systemPartId) const;

	void validateAndSetNodeAffinity(
		const char8_t *str, uint32_t len, FullContainerKeyComponents &component) const;
	void validateAndSetLargeContainerId(
		const char8_t *str, uint32_t len, FullContainerKeyComponents &component) const;
	void validateAndSetSystemPart(
		const char8_t *str, uint32_t len, FullContainerKeyComponents &component) const;

	NodeAffinityNumber getNodeAffinityNumber(
		const char8_t *affinityString, uint32_t affinityStringLength) const;
	LargeContainerId getLargeContainerId(
		const char8_t *largeContainerIdString, uint32_t largeContainerIdStringLength) const;
	SystemPartId getSystemPartId(
		const char8_t *systemPart, uint32_t systemPartLength) const;

	int32_t compareOriginalString(
		const char8_t *str1, uint32_t str1Length,
		const char8_t *str2, uint32_t str2Length,
		bool caseSensitive) const;
	int32_t compareNormalizedString(
		const char8_t *str1, uint32_t str1Length, const BitArray &upperCaseBit1, uint64_t startPos1,
		const char8_t *str2, uint32_t str2Length, const BitArray &upperCaseBit2, uint64_t startPos2,
		bool caseSensitive) const;

	static void dumpComponent(const FullContainerKeyComponents &component, const char8_t *msg);
};

#endif
