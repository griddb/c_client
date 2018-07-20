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
/*
    Copyright (c) 2008, Yubin Lim(purewell@gmail.com).
    All rights reserved.

    Redistribution and use in source and binary forms, with or without 
    modification, are permitted provided that the following conditions 
    are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the 
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Purewell nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "util/os.h"
#include "util/file.h"
#include "util/code.h"
#include <vector>
#include <cassert>

namespace util {

PlatformException PlatformExceptionBuilder::operator()(
		const char8_t *message,
		const Exception::SourceSymbolChar *fileNameLiteral,
		const Exception::SourceSymbolChar *functionNameLiteral,
		int32_t lineNumber,
		Type type,
		int32_t specialErrorCode) {
	const char8_t *descriptionPtr;
#ifdef _WIN32
	const DWORD lastError = GetLastError();
	const int32_t errorCode = static_cast<int32_t>(lastError);
	const char8_t *name = "GetLastError";

	wchar_t descriptionW[1024];
	u8string descriptionStr;

	if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError,
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			descriptionW,
			sizeof(descriptionW) / sizeof(descriptionW[0]), NULL) == 0) {
		descriptionPtr = "";
	}
	else {
		try {
			CodeConverter(Code::WCHAR_T, Code::UTF8)(
					descriptionW, descriptionStr);
			for (u8string::iterator it = descriptionStr.begin();
					it != descriptionStr.end();) {
				if (*it == '\r' || *it == '\n') {
					it = descriptionStr.erase(it);
				}
				else {
					++it;
				}
			}
			descriptionPtr = descriptionStr.c_str();
		}
		catch (...) {
			descriptionPtr = "";
		}
	}
#else
	const int32_t errorCode =
			(type == TYPE_NORMAL ? errno : specialErrorCode);
	const char8_t *name =
			(type == TYPE_NORMAL ? "errno" : "getaddrinfo");

	char8_t description[1024];
	descriptionPtr = (type == TYPE_NORMAL ?
			strerror_r(errorCode, description, sizeof(description)) :
			gai_strerror(errorCode));
#endif

	return PlatformException(errorCode,
			UTIL_EXCEPTION_CREATE_MESSAGE_CHARS(
					"Platform error (" <<
					(message == NULL ? "" : message) <<
					(message == NULL ? "" : ", ") <<
					name << "=" << errorCode <<
					", description=" << descriptionPtr << ")"),
			fileNameLiteral, functionNameLiteral, lineNumber,
			NULL, "util::PlatformException",
			Exception::STACK_TRACE_TOP);
}


namespace {
const int64_t UNIXTIME_FILETIME_EPOC_DIFF =
		static_cast<int64_t>(10000000) * 60 * 60 * 24 * (365 * 369 + 89);
}

#ifdef _WIN32
int64_t FileLib::getUnixTime(const FILETIME &fileTime) {
	ULARGE_INTEGER value;
	value.LowPart = fileTime.dwLowDateTime;
	value.HighPart = fileTime.dwHighDateTime;
	return (static_cast<int64_t>(value.QuadPart) -
			UNIXTIME_FILETIME_EPOC_DIFF) / 10000;
}

FILETIME FileLib::getFileTime(SYSTEMTIME &systemTime, bool asLocalTimeZone) {
	FILETIME fileTime;

	if (asLocalTimeZone) {
		TIME_ZONE_INFORMATION tzInfo;
		const DWORD tzType = GetTimeZoneInformation(&tzInfo);
		if (tzType == TIME_ZONE_ID_INVALID) {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
		SYSTEMTIME utcSystemTime;
		if (!TzSpecificLocalTimeToSystemTime(
				&tzInfo, &systemTime, &utcSystemTime)) {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
		if (!SystemTimeToFileTime(&utcSystemTime, &fileTime)) {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
	} else {
		if (!SystemTimeToFileTime(&systemTime, &fileTime)) {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
	}

	return fileTime;
}

FILETIME FileLib::getFileTime(int64_t unixTime) {
	if (unixTime < 0) {
		UTIL_THROW_UTIL_ERROR(CODE_INVALID_PARAMETER,
				"Negative unix time");
	}
	const int64_t value = (unixTime * 10000 + UNIXTIME_FILETIME_EPOC_DIFF);
	FILETIME fileTime;
	fileTime.dwLowDateTime = static_cast<DWORD>(value);
	fileTime.dwHighDateTime = static_cast<DWORD>(value >> 32);
	return fileTime;
}

SYSTEMTIME FileLib::getSystemTime(
		const FILETIME &fileTime, bool asLocalTimeZone) {
	SYSTEMTIME utcSystemTime;
	if (!FileTimeToSystemTime(&fileTime, &utcSystemTime)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}

	if (asLocalTimeZone) {
		TIME_ZONE_INFORMATION tzInfo;
		const DWORD tzType = GetTimeZoneInformation(&tzInfo);
		if (tzType == TIME_ZONE_ID_INVALID) {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
		SYSTEMTIME localSystemTime;
		if (!SystemTimeToTzSpecificLocalTime(
				&tzInfo, &utcSystemTime, &localSystemTime)) {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
		return localSystemTime;
	}
	else {
		return utcSystemTime;
	}
}
#else
int64_t FileLib::getUnixTime(
		tm &time, int32_t milliSecond, bool asLocalTimeZone) {
	if (milliSecond < 0 || milliSecond >= 1000) {
		UTIL_THROW_UTIL_ERROR(CODE_INVALID_PARAMETER,
				"Millisecond field out of range");
	}

	time_t unixTimeSec;
	if (asLocalTimeZone) {
		unixTimeSec = mktime(&time);
	}
	else {
		unixTimeSec = timegm(&time);
	}

	if (unixTimeSec == static_cast<time_t>(-1)) {
		UTIL_THROW_UTIL_ERROR(CODE_INVALID_PARAMETER,
				"Illegal time");
	}
	else {
		return static_cast<int64_t>(unixTimeSec) * 1000 +
				static_cast<int32_t>(milliSecond);
	}
}

int64_t FileLib::getUnixTime(const timeval &time) {
	return static_cast<int64_t>(time.tv_sec) * 1000 +
			static_cast<int64_t>(time.tv_usec) / 1000;
}

int64_t FileLib::getUnixTime(const timespec &time) {
	const int64_t nanosPerMilli = 1000 * 1000;
	return static_cast<int64_t>(time.tv_sec) * 1000 +
			static_cast<int64_t>(time.tv_nsec) / nanosPerMilli;
}

int64_t FileLib::getUnixTime(time_t time) {
	return static_cast<int64_t>(time) * 1000;
}

tm FileLib::getTM(int64_t unixTime, bool asLocalTimeZone) {
	if (unixTime < 0) {
		UTIL_THROW_UTIL_ERROR(CODE_INVALID_PARAMETER,
				"Negative unix time");
	}
	time_t unixTimeSec = static_cast<time_t>(unixTime / 1000);
	tm time;
	if (asLocalTimeZone) {
		if (localtime_r(&unixTimeSec, &time) == NULL) {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
	}
	else {
		if (gmtime_r(&unixTimeSec, &time) == NULL) {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
	}
	return time;
}

timeval FileLib::getTimeval(int64_t unixTime) {
	timeval tv;
	tv.tv_sec = static_cast<long>(unixTime / 1000);
	tv.tv_usec = static_cast<long>(unixTime % 1000) * 1000;
	return tv;
}

timespec FileLib::calculateTimeoutSpec(clockid_t clockId, uint32_t timeoutMillis) {
	timespec ts;
	if (0 != clock_gettime(clockId, &ts)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}

	const long nanosPerSec = 1000 * 1000 * 1000;
	const long nanosPerMilli = 1000 * 1000;

	ts.tv_sec += static_cast<time_t>(timeoutMillis / 1000);
	ts.tv_nsec += static_cast<long>(timeoutMillis % 1000) * nanosPerMilli;

	const long sec = ts.tv_nsec / nanosPerSec;
	assert(0 <= sec && sec <= 1);

	ts.tv_sec += static_cast<time_t>(sec);
	ts.tv_nsec -= sec * nanosPerSec;

	return ts;
}
#endif


} 
