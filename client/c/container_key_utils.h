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
#ifndef CONTAINER_KEY_UTILS_H_
#define CONTAINER_KEY_UTILS_H_

#include "util/type.h"
#include <vector>
#include <string>
#include <memory>

struct ContainerKeyUtils {
	struct Components;
	struct Tool;

	static const int64_t PUBLIC_DATABASE_ID;

	static void compose(
			const Components &components, std::vector<uint8_t> &bytes);
	static void decompose(
			const std::vector<uint8_t> &bytes, Components &components);

	static void format(std::ostream &os, const std::vector<uint8_t> &bytes);
	static bool parse(
			const char8_t *str, std::vector<uint8_t> &bytes,
			bool internalMode);

	static int32_t compare(
			const std::vector<uint8_t> &bytes1,
			const std::vector<uint8_t> &bytes2, bool caseSensitive);

	static const void* getBytesAddress(const std::vector<uint8_t> &bytes);
};

struct ContainerKeyUtils::Components {
	Components();

	int64_t databaseId_;
	std::string base_;
	int64_t largeId_;
	UTIL_UNIQUE_PTR<std::string> affinityStr_;
	int64_t affinityNum_;
	UTIL_UNIQUE_PTR<std::string> systemStr_;
	int64_t systemNum_;
};

#endif	
