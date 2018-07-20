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
