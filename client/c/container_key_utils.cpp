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

#include "container_key_utils.h"

#include "container_key.h"
#include "util/allocator.h"

#include <cstring>

struct ContainerKeyUtils::Tool {

	static KeyConstraint getConstraint(bool internalMode_) {
		KeyConstraint constraint;
		constraint.maxTotalLength_ = std::numeric_limits<uint32_t>::max();
		constraint.systemPartAllowed_ = internalMode_;
		constraint.largeContainerIdAllowed_ = internalMode_;
		return constraint;
	}

	static void toBinary(
			const FullContainerKey &key, std::vector<uint8_t> &bytes) {
		const void *data;
		size_t size;
		key.toBinary(data, size);

		const uint8_t *it = static_cast<const uint8_t*>(data);
		bytes.assign(it, it + size);
	}
};

//
// ContainerKeyUtils
//

const int64_t ContainerKeyUtils::PUBLIC_DATABASE_ID = 0;

void ContainerKeyUtils::compose(
		const Components &components, std::vector<uint8_t> &bytes) {
	FullContainerKeyComponents dest;
	dest.dbId_ = components.databaseId_;
	dest.baseName_ = components.base_;
	dest.baseNameSize_ = components.base_.size();
	dest.largeContainerId_ = (components.largeId_ < 0 ?
			UNDEF_LARGE_CONTAINERID : components.largeId_);
	dest.affinityNumber_ = (components.affinityNum_ < 0 ?
			UNDEF_NODE_AFFINITY_NUMBER : components.affinityNum_);
	dest.systemPartId_ = (components.systemNum_ < 0 ?
			UNDEF_SYSTEM_PART_ID : components.systemNum_);

	if (components.affinityStr_.get() != NULL) {
		dest.affinityString_ = *components.affinityStr_;
		dest.affinityStringSize_ = components.affinityStr_->size();
	}

	if (components.systemStr_.get() != NULL) {
		dest.systemPart_ = *components.systemStr_;
		dest.systemPartSize_ = components.systemStr_->size();
	}

	const KeyConstraint &constraint = Tool::getConstraint(true);
	const FullContainerKey key(constraint, dest);
	Tool::toBinary(key, bytes);
}

void ContainerKeyUtils::decompose(
		const std::vector<uint8_t> &bytes, Components &components) {

	const KeyConstraint &constraint = Tool::getConstraint(true);
	const FullContainerKey key(
			constraint, getBytesAddress(bytes), bytes.size());

	const FullContainerKeyComponents &src = key.getComponents();

	components.databaseId_ = src.dbId_;
	if (src.baseNameSize_ > 0) {
		components.base_.assign(&src.baseName_[0], src.baseNameSize_);
	}
	components.largeId_ =
			(src.largeContainerId_ == UNDEF_LARGE_CONTAINERID ?
					-1 : src.largeContainerId_);
	components.affinityNum_ =
			(src.affinityNumber_ == UNDEF_NODE_AFFINITY_NUMBER ?
					-1 : src.affinityNumber_);
	components.systemNum_ =
			(src.systemPartId_ == UNDEF_SYSTEM_PART_ID ?
					-1 : src.systemPartId_);

	if (src.affinityStringSize_ > 0) {
		components.affinityStr_.reset(UTIL_NEW std::string(
				&src.affinityString_[0], src.affinityStringSize_));
	}

	if (src.systemPartSize_ > 0) {
		components.systemStr_.reset(UTIL_NEW std::string(
				&src.systemPart_[0], src.systemPartSize_));
	}
}

void ContainerKeyUtils::format(
		std::ostream &os, const std::vector<uint8_t> &bytes) {

	const KeyConstraint &constraint = Tool::getConstraint(true);
	const FullContainerKey key(
			constraint, getBytesAddress(bytes), bytes.size());

	util::BasicString<char8_t> str;
	key.toString(str);
	os << str;
}

bool ContainerKeyUtils::parse(
		const char8_t *str, std::vector<uint8_t> &bytes, bool internalMode) {

	const KeyConstraint &constraint = Tool::getConstraint(internalMode);
	const FullContainerKey key(
			constraint, PUBLIC_DATABASE_ID, str, strlen(str));
	Tool::toBinary(key, bytes);

	const FullContainerKeyComponents &components =
			key.getComponents();

	if (!internalMode && (
			components.largeContainerId_ != UNDEF_LARGE_CONTAINERID ||
			components.systemPartSize_ > 0 ||
			components.systemPartId_ != UNDEF_SYSTEM_PART_ID)) {
		return false;
	}

	return true;
}

int32_t ContainerKeyUtils::compare(
		const std::vector<uint8_t> &bytes1,
		const std::vector<uint8_t> &bytes2, bool caseSensitive) {

	const KeyConstraint &constraint = Tool::getConstraint(true);
	const FullContainerKey key1(
			constraint, getBytesAddress(bytes1), bytes1.size());
	const FullContainerKey key2(
			constraint, getBytesAddress(bytes2), bytes2.size());
	return key1.compareTo(key2, caseSensitive);
}

const void* ContainerKeyUtils::getBytesAddress(
		const std::vector<uint8_t> &bytes) {
	return (bytes.empty() ? NULL : &bytes[0]);
}

ContainerKeyUtils::Components::Components() :
		databaseId_(PUBLIC_DATABASE_ID),
		largeId_(-1),
		affinityNum_(-1),
		systemNum_(-1) {
}
