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
	@brief Implementation of Client Internal Module
*/
#include "client.h"
#include "gs_error_common.h"
#include "sha2.h"
#include <algorithm>
#include <functional>

#if !defined(GS_DLL_INSIDE) && !defined(GS_CLIENT_UNIT_TEST)
#error
#endif


#ifndef GS_CLIENT_CONFIG_FILE_LOGGING_ONLY
#define GS_CLIENT_CONFIG_FILE_LOGGING_ONLY 1
#endif

#ifndef GS_CLIENT_MD5_LIB_ENABLED
#define GS_CLIENT_MD5_LIB_ENABLED 1
#endif

#if GS_CLIENT_MD5_LIB_ENABLED
#include "md5.h"
#endif

#ifndef GS_CLIENT_DISABLE_DEFAULT_FACTORY
#define GS_CLIENT_DISABLE_DEFAULT_FACTORY 0
#endif



#ifndef GS_CLIENT_ENABLE_PROFILER
#define GS_CLIENT_ENABLE_PROFILER 0
#endif

#if GS_CLIENT_ENABLE_PROFILER
static const uint32_t CLIENT_PROFILER_DUMP_INTERVAL = 1000;
#endif

static const bool CLIENT_TRIM_MILLISECONDS = false;

static const bool CLIENT_TIME_SERIES_PROPERTIES_OLD = false;

#ifndef GS_CLIENT_ENABLE_NODE_ADDRESS_CACHE
#define GS_CLIENT_ENABLE_NODE_ADDRESS_CACHE 1
#endif

#define GS_BINDING_ENTRY_INITIALIZER \
	{ NULL, GSType(), 0, 0, GSTypeOption() }


#define GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(returnValue) \
		do { \
			if (!GSGridStoreFactoryTag::isAlive()) { \
				return (returnValue); \
			} \
		} \
		while (false)

#define GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE() \
		GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE( \
				GS_ERROR_CC_RESOURCE_CLOSED)

#define GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(ptr, initValue) \
		do { \
			if (!GSGridStoreFactoryTag::isAlive()) { \
				if ((ptr) != NULL) { \
					*(ptr) = (initValue); \
				} \
				return (GS_ERROR_CC_RESOURCE_CLOSED); \
			} \
		} \
		while (false)

#define GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT2( \
		ptr1, initValue1, ptr2, initValue2) \
		do { \
			if (!GSGridStoreFactoryTag::isAlive()) { \
				if ((ptr1) != NULL) { \
					*(ptr1) = (initValue1); \
				} \
				if ((ptr2) != NULL) { \
					*(ptr2) = (initValue2); \
				} \
				return (GS_ERROR_CC_RESOURCE_CLOSED); \
			} \
		} \
		while (false)

#define GS_CLIENT_CHECK_FACTORY_AND_RETURN() \
		do { \
			if (!GSGridStoreFactoryTag::isAlive()) { \
				return; \
			} \
		} \
		while (false)

#define GS_CLIENT_CHECK_FACTORY_AND_RETURN_INIT(ptr, initValue) \
		do { \
			if (!GSGridStoreFactoryTag::isAlive()) { \
				if ((ptr) != NULL) { \
					*(ptr) = (initValue); \
				} \
				return; \
			} \
		} \
		while (false)

#define GS_CLIENT_CHECK_NOT_NULL(ptr) \
		do { \
			if ((ptr) == NULL) { \
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, ""); \
			} \
		} \
		while (false)


#define GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(resource) \
		GSInterceptor::FunctionInfo( \
				__FUNCTION__, GSResourceHeader::getStaticType(resource))

#define GS_CLIENT_INTERCEPT_API_CALL_PARAM(p) \
		GSInterceptor::Parameter(#p).with(p)

#define GS_CLIENT_INTERCEPT_API_CALL_BASE1( \
		ret, funcInfo, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, ...) \
		GSInterceptorManager::Scope interceptorManagerScope; \
		do { \
			GSInterceptorManager *manager = \
					GSResourceHeader::findInterceptorManager(p1); \
			if (manager == NULL) { \
				break; \
			} \
			if (interceptorManagerScope.set( \
					*manager, \
					(funcInfo.isEmpty() ? \
							GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(p1) : \
							funcInfo), \
					GSInterceptor::ParameterList( \
							GS_CLIENT_INTERCEPT_API_CALL_PARAM(p1), \
							GS_CLIENT_INTERCEPT_API_CALL_PARAM(p2), \
							GS_CLIENT_INTERCEPT_API_CALL_PARAM(p3), \
							GS_CLIENT_INTERCEPT_API_CALL_PARAM(p4), \
							GS_CLIENT_INTERCEPT_API_CALL_PARAM(p5), \
							GS_CLIENT_INTERCEPT_API_CALL_PARAM(p6), \
							GS_CLIENT_INTERCEPT_API_CALL_PARAM(p7), \
							GS_CLIENT_INTERCEPT_API_CALL_PARAM(p8), \
							GS_CLIENT_INTERCEPT_API_CALL_PARAM(p9), \
							GS_CLIENT_INTERCEPT_API_CALL_PARAM(p10)), \
					GSInterceptor::Parameter().with(ret))) { \
				return ret; \
			} \
		} \
		while (false)

#define GS_CLIENT_INTERCEPT_API_CALL_BASE2(tuple) \
		GS_CLIENT_INTERCEPT_API_CALL_BASE1 tuple

#define GS_CLIENT_INTERCEPT_API_CALL_CUSTOM(ret, funcInfo, ...) \
		GS_CLIENT_INTERCEPT_API_CALL_BASE2(( \
				ret, funcInfo, __VA_ARGS__, \
				util::FalseType(), util::FalseType(), util::FalseType(), \
				util::FalseType(), util::FalseType(), util::FalseType(), \
				util::FalseType(), util::FalseType(), util::FalseType()))

#define GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(funcInfo, ...) \
		GSResult ret = GS_ERROR_CC_INTERNAL_ERROR; \
		GS_CLIENT_INTERCEPT_API_CALL_CUSTOM(ret, funcInfo, __VA_ARGS__)

#define GS_CLIENT_INTERCEPT_API_CALL(...) \
		GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC( \
				GSInterceptor::FunctionInfo::emptyInfo(), __VA_ARGS__)


#if GS_CLIENT_ENABLE_PROFILER

#include <iostream>

struct ClientProfiler {
	static ClientProfiler INSTANCE;

	ClientProfiler() {
		stopwatch_.start();
	}

	void profileStatement(Statement::Id statement, uint64_t elapsedNanos) {
		totalNanos_[statement] += elapsedNanos;
		totalCounts_[statement]++;

		if (CLIENT_PROFILER_DUMP_INTERVAL > 0) {
			util::LockGuard<util::Mutex> lock(mutex_);
			if (stopwatch_.elapsedMillis() >= CLIENT_PROFILER_DUMP_INTERVAL) {
				dump();

				std::fill(totalNanos_, totalNanos_ + Statement::STATEMENT_ID_MAX, 0);
				std::fill(totalCounts_, totalCounts_ + Statement::STATEMENT_ID_MAX, 0);
				stopwatch_.reset();
				stopwatch_.start();
			}
		}
	}

	static const GSChar *statementToString(Statement::Id statement) {
		const GSChar *statementNames[] = {
				"CONNECT",
				"DISCONNECT",
				"LOGIN",
				"LOGOUT",
				"GET_PARTITION_ADDRESS",
				"GET_COLLECTION",
				"GET_TIME_SERIES",
				"PUT_COLLECTION",
				"PUT_TIME_SERIES",
				"DROP_COLLECTION",
				"DROP_TIME_SERIES",
				"CREATE_SESSION",
				"CLOSE_SESSION",
				"CREATE_INDEX",
				"DROP_INDEX",
				"CREATE_EVENT_NOTIFICATION",
				"DROP_EVENT_NOTIFICATION",
				"FLUSH_LOG",
				"COMMIT_TRANSACTION",
				"ABORT_TRANSACTION",
				"GET_ROW",
				"QUERY_TQL",
				"QUERY_COLLECTION_GEOMETRY_RELATED",
				"QUERY_COLLECTION_GEOMETRY_WITH_EXCLUSION",
				"PUT_ROW",
				"PUT_MULTIPLE_ROWS",
				"UPDATE_ROW_BY_ID",
				"DELETE_ROW",
				"DELETE_ROW_BY_ID",
				"APPEND_TIME_SERIES_ROW",
				"GET_TIME_SERIES_ROW",
				"GET_TIME_SERIES_ROW_RELATED",
				"INTERPOLATE_TIME_SERIES_ROW",
				"AGGREGATE_TIME_SERIES",
				"QUERY_TIME_SERIES_TQL",
				"QUERY_TIME_SERIES_RANGE",
				"QUERY_TIME_SERIES_SAMPLING",
				"PUT_TIME_SERIES_ROW",
				"PUT_TIME_SERIES_MULTIPLE_ROWS",
				"DELETE_TIME_SERIES_ROW",
				"GET_CONTAINER_PROPERTIES",
				"GET_MULTIPLE_ROWS",
				"GET_TIME_SERIES_MULTIPLE_ROWS",
				"GET_PARTITION_CONTAINER_NAMES",
				"DROP_CONTAINER",
				"CREATE_MULTIPLE_SESSIONS",
				"CLOSE_MULTIPLE_SESSIONS",
				"EXECUTE_MULTIPLE_QUERIES",
				"GET_MULTIPLE_COLLECTION_ROWS",
				"PUT_MULTIPLE_COLLECTION_ROWS",
				"CLOSE_ROW_SET",
				"FETCH_ROW_SET",
				"CREATE_TRIGGER",
				"DROP_TRIGGER"
		};
		assert(statement >= 0);
		assert(statement < sizeof(statementNames) / sizeof(*statementNames));
		return statementNames[statement];
	}

	void dump() {
		for (int32_t i = 0; i < Statement::STATEMENT_ID_MAX; i++) {
			const uint64_t count = totalCounts_[i];
			if (count > 0) {
				const uint64_t nanos = totalNanos_[i];
				std::cout << statementToString(static_cast<Statement::Id>(i)) <<
						": count=" << count << ": avg[ms]=" <<
						(static_cast<double>(nanos) / count / 1000 / 1000) << std::endl;
			}
		}
		std::cout << std::endl;
	}

	util::Atomic<uint64_t> totalNanos_[Statement::STATEMENT_ID_MAX];
	util::Atomic<uint64_t> totalCounts_[Statement::STATEMENT_ID_MAX];
	util::Mutex mutex_;
	util::Stopwatch stopwatch_;
};
ClientProfiler ClientProfiler::INSTANCE;

extern "C" {
GS_DLL_PUBLIC void GS_API_CALL gsExperimentalDumpProfile() {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN();
	ClientProfiler::INSTANCE.dump();
}
}

#endif	


void GSCommonExceptionRegenerator::CustomFilter::filterCause(
		CauseType causeType, const NamedErrorCode *utilCode,
		NamedErrorCode &causeCode, bool &codeAsLiteral,
		const char8_t *&extraMessage) {
	static_cast<void>(extraMessage);

	if (!causeCode.isEmpty() &&
			!(causeType == CAUSE_PLATFORM || causeType == CAUSE_UTILITY)) {
		return;
	}

	if (utilCode == NULL) {
		if (causeType == CAUSE_PLATFORM) {
			causeCode = GS_COMMON_EXCEPTION_NAMED_CODE(
					GS_ERROR_CC_PLATFORM_ERROR);
		}
	}
	else {
		switch (utilCode->getCode()) {
		case util::UtilityException::CODE_NO_MEMORY:
			causeCode = GS_COMMON_EXCEPTION_NAMED_CODE(
					GS_ERROR_CC_ALLOCATION_FAILED);
			break;
		case util::UtilityException::CODE_MEMORY_LIMIT_EXCEEDED:
			causeCode = GS_COMMON_EXCEPTION_NAMED_CODE(
					GS_ERROR_CC_ALLOCATION_FAILED);
			break;
		case util::UtilityException::CODE_SIZE_LIMIT_EXCEEDED:
			causeCode = GS_COMMON_EXCEPTION_NAMED_CODE(
					GS_ERROR_CC_ILLEGAL_PARAMETER);
			break;
		default:
			break;
		}
	}

	if (causeCode.isEmpty()) {
		causeCode = GS_COMMON_EXCEPTION_NAMED_CODE(
				GS_ERROR_CC_INTERNAL_ERROR);
	}
	codeAsLiteral = true;
}


bool ClientUtil::toBool(GSBool value) {
	return !!value;
}

GSBool ClientUtil::toGSBool(bool value) {
	return (value ? GS_TRUE : GS_FALSE);
}

size_t ClientUtil::toSizeValue(int64_t value) {
	if (value < 0 ||
			static_cast<uint64_t>(value) > std::numeric_limits<size_t>::max()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_SIZE_VALUE_OUT_OF_RANGE, "");
	}
	return static_cast<size_t>(value);
}

size_t ClientUtil::toSizeValue(int32_t value) {
	if (value < 0) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_SIZE_VALUE_OUT_OF_RANGE, "");
	}
	return static_cast<size_t>(value);
}

int32_t ClientUtil::sizeValueToInt32(size_t value) {
	if (value > static_cast<uint32_t>(std::numeric_limits<int32_t>::max())) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_SIZE_VALUE_OUT_OF_RANGE, "");
	}
	return static_cast<int32_t>(value);
}

int64_t ClientUtil::sizeValueToInt64(size_t value) {
	if (value > static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_SIZE_VALUE_OUT_OF_RANGE, "");
	}
	return static_cast<int64_t>(value);
}

size_t ClientUtil::copyString(
		const std::string &src, GSChar *dest, size_t destLenWithNull) {
	return copyString(src.c_str(), src.size() + 1, dest, destLenWithNull);
}

size_t ClientUtil::copyString(
		const GSChar *src, size_t srcLenWithNull,
		GSChar *dest, size_t destLenWithNull) throw() {

	if (destLenWithNull >= srcLenWithNull) {
		memcpy(dest, src, srcLenWithNull);
	}
	else if (destLenWithNull > 0) {
		dest[0] = '\0';
	}

	return srcLenWithNull;
}

std::string ClientUtil::normalizeSymbol(
		const GSChar *symbol, const GSChar *typeName) {
	checkSymbol(symbol, typeName);
	return normalizeSymbolUnchecked(symbol);
}

std::string ClientUtil::normalizeSymbolUnchecked(const GSChar *symbol) {
	assert(symbol != NULL);


	std::string result;
	for (const GSChar *i = symbol;; i++) {
		const GSChar ch = *i;
		if (ch == '\0') {
			break;
		}

		result.append(1, static_cast<GSChar>(toASCIILower(ch)));
	}

	return result;
}

void ClientUtil::checkSymbol(const GSChar *symbol, const GSChar *typeName) {
	if (symbol == NULL || *symbol == '\0') {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER,
				"Empty " << typeName);
	}
}

void ClientUtil::checkBasicSymbol(
		const GSChar *symbol, const GSChar *typeName) {
	checkSymbol(symbol, typeName);

	for (const GSChar *i = symbol;; i++) {
		const GSChar ch = *i;
		if (ch == '\0') {
			break;
		}

		if ((ch <= 'Z' && ch >= 'A') || (ch <= 'z' && ch >= 'a') || ch == '_' ||
				(i != symbol && '0' <= ch && ch <= '9')) {
			continue;
		}

		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SYMBOL_CHARACTER, "");
	}
}

int32_t ClientUtil::toASCIILower(int32_t ch) {
	if (ch <= 'Z' && ch >= 'A') {
		return ch - 'A' + 'a';
	}

	return ch;
}

bool ClientUtil::isIPV6Address(const util::SocketAddress &address) {
	return (address.getFamily() == util::SocketAddress::FAMILY_INET6);
}

GSTimeUnit ClientUtil::checkTimeUnit(GSTimeUnit timeUnit) {
	switch (timeUnit) {
	case GS_TIME_UNIT_YEAR:
	case GS_TIME_UNIT_MONTH:
	case GS_TIME_UNIT_DAY:
	case GS_TIME_UNIT_HOUR:
	case GS_TIME_UNIT_MINUTE:
	case GS_TIME_UNIT_SECOND:
	case GS_TIME_UNIT_MILLISECOND:
		return timeUnit;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNKNOWN_TIME_UNIT, "");
	}
}

void ClientUtil::writeVarSize(XArrayByteOutStream &out, size_t val) {
	if (val < VAR_SIZE_1BYTE_THRESHOLD) {
		out << encode1ByteVarSize(static_cast<uint8_t>(val));
	}
	else if (val < VAR_SIZE_4BYTE_THRESHOLD) {
		out << encode4ByteVarSize(static_cast<uint32_t>(val));
	}
	else {
		int32_t intVal = sizeValueToInt32(val);
		out << encode8ByteVarSize(static_cast<uint64_t>(intVal));
	}
}

void ClientUtil::writeVarDataString(XArrayByteOutStream &out, const GSChar* elem) {
	size_t length = strlen(elem);
	writeVarSize(out, length);
	out.writeAll(elem, length);
}

uint32_t ClientUtil::readVarSize(ArrayByteInStream &in) {
	size_t pos = in.base().position();
	uint8_t byte;
	in >> byte;
	if (varSizeIs1Byte(byte)) {
		return decode1ByteVarSize(byte);
	}
	else if (varSizeIs4Byte(byte)) {
		in.base().position(pos);
		uint32_t val;
		in >> val;
		return decode4ByteVarSize(val);
	}
	else {
		in.base().position(pos);
		uint64_t val;
		in >> val;
		uint64_t decodedVal = decode8ByteVarSize(val);
		if (decodedVal > static_cast<uint64_t>(std::numeric_limits<int32_t>::max())) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
					"Decoded size = " << decodedVal);
		}
		return static_cast<uint32_t>(decodedVal);
	}
}


ClientVersion::ClientVersion() throw() :
		majorVersion_(GS_CLIENT_VERSION_MAJOR),
		minorVersion_(GS_CLIENT_VERSION_MINOR) {
}

ClientVersion::ClientVersion(
		int32_t majorVersion, int32_t minorVersion) throw() :
		majorVersion_(majorVersion),
		minorVersion_(minorVersion) {
}

bool ClientVersion::since(
		int32_t majorVersion, int32_t minorVersion) const throw() {
	return since(ClientVersion(majorVersion, minorVersion));
}

bool ClientVersion::since(const ClientVersion &another) const throw() {
	if (majorVersion_ > another.majorVersion_) {
		return true;
	}

	if (majorVersion_ == another.majorVersion_ &&
			minorVersion_ >= another.minorVersion_) {
		return true;
	}

	return false;
}


bool SocketAddressLess::operator()(
		const util::SocketAddress &left,
		const util::SocketAddress &right) const {
	if (left.getFamily() != right.getFamily()) {
		return left.getFamily() < right.getFamily();
	}

	if (left.getFamily() == util::SocketAddress::FAMILY_INET) {
		util::SocketAddress::Inet leftAddr;
		util::SocketAddress::Inet rightAddr;
		uint16_t leftPort;
		uint16_t rightPort;
		left.getIP(&leftAddr, &leftPort);
		right.getIP(&rightAddr, &rightPort);

		if (leftPort != rightPort) {
			return leftPort < rightPort;
		}

		return memcmp(&leftAddr, &rightAddr, sizeof(leftAddr)) < 0;
	}
	else if (left.getFamily() == util::SocketAddress::FAMILY_INET6) {
		util::SocketAddress::Inet6 leftAddr;
		util::SocketAddress::Inet6 rightAddr;
		uint16_t leftPort;
		uint16_t rightPort;
		left.getIP(&leftAddr, &leftPort);
		right.getIP(&rightAddr, &rightPort);

		if (leftPort != rightPort) {
			return leftPort < rightPort;
		}

		return memcmp(&leftAddr, &rightAddr, sizeof(leftAddr)) < 0;
	}
	else {
		return false;
	}
}


bool SocketAddressEqual::operator()(
		const util::SocketAddress &left,
		const util::SocketAddress &right) const {
	if (left.getFamily() != right.getFamily()) {
		return false;
	}

	if (left.getFamily() == util::SocketAddress::FAMILY_INET) {
		util::SocketAddress::Inet leftAddr;
		util::SocketAddress::Inet rightAddr;
		uint16_t leftPort;
		uint16_t rightPort;
		left.getIP(&leftAddr, &leftPort);
		right.getIP(&rightAddr, &rightPort);

		if (leftPort != rightPort) {
			return false;
		}

		return memcmp(&leftAddr, &rightAddr, sizeof(leftAddr)) == 0;
	}
	else if (left.getFamily() == util::SocketAddress::FAMILY_INET6) {
		util::SocketAddress::Inet6 leftAddr;
		util::SocketAddress::Inet6 rightAddr;
		uint16_t leftPort;
		uint16_t rightPort;
		left.getIP(&leftAddr, &leftPort);
		right.getIP(&rightAddr, &rightPort);

		if (leftPort != rightPort) {
			return false;
		}

		return memcmp(&leftAddr, &rightAddr, sizeof(leftAddr)) == 0;
	}
	else {
		return true;
	}
}


template<typename L>
DynamicLockGuard<L>::DynamicLockGuard(L *lockObject) :
		lockObject_(lockObject) {
	if (lockObject_ != NULL) {
		lockObject_->lock();
	}
}

template<typename L>
DynamicLockGuard<L>::~DynamicLockGuard() {
	if (lockObject_ != NULL) {
		lockObject_->unlock();
	}
}

template class DynamicLockGuard<util::Mutex>;


GSTimestamp TimestampUtil::current() {
	return util::DateTime::now(CLIENT_TRIM_MILLISECONDS).getUnixTime();
}

GSTimestamp TimestampUtil::add(
		GSTimestamp timestamp, int32_t amount, GSTimeUnit timeUnit) {
	util::DateTime dateTime = timestamp;

	util::DateTime::FieldType fieldType;
	switch (timeUnit) {
	case GS_TIME_UNIT_YEAR:
		fieldType = util::DateTime::FIELD_YEAR;
		break;
	case GS_TIME_UNIT_MONTH:
		fieldType = util::DateTime::FIELD_MONTH;
		break;
	case GS_TIME_UNIT_DAY:
		fieldType = util::DateTime::FIELD_DAY_OF_MONTH;
		break;
	case GS_TIME_UNIT_HOUR:
		fieldType = util::DateTime::FIELD_HOUR;
		break;
	case GS_TIME_UNIT_MINUTE:
		fieldType = util::DateTime::FIELD_MINUTE;
		break;
	case GS_TIME_UNIT_SECOND:
		fieldType = util::DateTime::FIELD_SECOND;
		break;
	case GS_TIME_UNIT_MILLISECOND:
		fieldType = util::DateTime::FIELD_MILLISECOND;
		break;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNKNOWN_TIME_UNIT, "");
	}

	dateTime.addField(amount, fieldType);
	return dateTime.getUnixTime();
}

size_t TimestampUtil::format(
		GSTimestamp timestamp, GSChar *strBuf, size_t bufSize) {
	util::NormalOStringStream oss;
	util::DateTime(timestamp).format(oss, CLIENT_TRIM_MILLISECONDS, false);

	return ClientUtil::copyString(oss.str(), strBuf, bufSize);
}

bool TimestampUtil::parse(const GSChar *str, GSTimestamp &timestamp) {
	util::DateTime dateTime;
	if (!util::DateTime::parse(str, dateTime, CLIENT_TRIM_MILLISECONDS)) {
		timestamp = -1;
		return false;
	}

	timestamp = dateTime.getUnixTime();
	return true;
}


Properties::Properties(const Properties &properties) :
		propertyMap_(properties.propertyMap_) {
}

Properties::Properties(
		const GSPropertyEntry *properties, const size_t *propertyCount) {
	const GSPropertyEntry *propertiesEnd;
	if (propertyCount == NULL) {
		if (properties == NULL) {
			return;
		}

		propertiesEnd = NULL;
	}
	else {
		if (*propertyCount == 0) {
			return;
		}

		if (properties == NULL ) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}
		propertiesEnd = properties + *propertyCount;
	}

	for (const GSPropertyEntry *i = properties;; i++) {
		if (propertiesEnd != NULL && i == propertiesEnd) {
			break;
		}

		if (i->name == NULL) {
			if (i->value != NULL) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PROPERTY_ENTRY, "");
			}

			if (propertyCount == NULL) {
				break;
			}
			else {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PROPERTY_ENTRY, "");
			}
		}

		if (i->value == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PROPERTY_ENTRY, "");
		}
		propertyMap_.insert(PropertyMap::value_type(i->name, i->value));
	}
}

bool Properties::getInteger(const GSChar *name, int32_t &value) const {
	value = 0;

	std::string strValue;
	if (!getString(name, strValue)) {
		return false;
	}

	value = ClientUtil::parseValue<int32_t>(strValue.c_str());

	return true;
}

bool Properties::getDouble(const GSChar *name, double &value) const {
	value = 0;

	std::string strValue;
	if (!getString(name, strValue)) {
		return false;
	}

	value = ClientUtil::parseValue<double>(strValue.c_str());

	return true;
}

bool Properties::getBool(const GSChar *name, bool &value) const {
	value = false;

	std::string strValue;
	if (!getString(name, strValue)) {
		return false;
	}

	value = ClientUtil::parseValue<bool>(strValue.c_str());
	return true;
}

bool Properties::getString(const GSChar *name, std::string &value) const {
	value.clear();

	const GSChar *found = getString(name);
	if (found == NULL) {
		return false;
	}

	value = found;
	return true;
}

const GSChar* Properties::getString(const GSChar *name) const {
	assert(name != NULL);
	PropertyMap::const_iterator it = propertyMap_.find(name);
	if (it == propertyMap_.end()) {
		return NULL;
	}

	return it->second.c_str();
}

bool Properties::getTimeoutMillis(
		const GSChar *name, int64_t &value, int64_t defaultValue) const {
	value = 0;

	std::string millisecName;
	millisecName = name;
	millisecName += "Millis";

	int32_t secValue;
	bool secValueSpecified = getInteger(name, secValue);
	int32_t millisecValue;
	bool millisecValueSpecified = getInteger(millisecName.c_str(), millisecValue);
	if (secValueSpecified && millisecValueSpecified) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PROPERTY_ENTRY, "");
	}

	if (secValueSpecified) {
		if (secValue == std::numeric_limits<int32_t>::max()) {
			value = std::numeric_limits<int64_t>::max();
		}
		else {
			value = static_cast<int64_t>(secValue) * 1000;
		}
		return true;
	}
	else if (millisecValueSpecified) {
		value = millisecValue;
		return true;
	}
	else {
		value = defaultValue;
		return false;
	}
}

void Properties::checkExclusiveProperties(
		const GSChar *name1, const GSChar *name2, const GSChar *name3) const {
	const GSChar *nameList[] = {
		name1, name2, name3, NULL
	};

	size_t count = 0;
	while (nameList[count] != NULL) {
		count++;
	}

	checkExclusiveProperties(nameList, count);
}

void Properties::checkExclusiveProperties(
		const GSChar *const *nameList, size_t nameCount) const {
	size_t count = 0;
	for (size_t i = 0; i < nameCount; i++) {
		if (propertyMap_.find(nameList[i]) != propertyMap_.end()) {
			count++;
		}
	}

	if (count > 1) {
		std::string message;

		message += "Either one of following properties can be specified (";

		bool found = false;
		for (size_t i = 0; i < nameCount; i++) {
			PropertyMap::const_iterator it = propertyMap_.find(nameList[i]);
			if (it != propertyMap_.end()) {
				if (!found) {
					message += ", ";
					found = true;
				}
				message += it->first;
				message += "=";
				message += it->second;
			}
		}

		message += ")";

		GS_CLIENT_THROW_ERROR(
				GS_ERROR_CC_ILLEGAL_PROPERTY_ENTRY, message);
	}
}

int32_t Properties::timeoutToInt32Seconds(int64_t value) {
	if (value < 0) {
		return -1;
	}
	else if (value >= static_cast<int64_t>(
			std::numeric_limits<int32_t>::max()) * 1000) {
		return std::numeric_limits<int32_t>::max();
	}
	else {
		return static_cast<int32_t>(value / 1000);
	}
}

int32_t Properties::timeoutToInt32Millis(int64_t value) {
	if (value < 0) {
		return -1;
	}
	else if (value >= std::numeric_limits<int32_t>::max()) {
		return std::numeric_limits<int32_t>::max();
	}
	else {
		return static_cast<int32_t>(value);
	}
}


ClientException::ClientException(
		UTIL_EXCEPTION_CONSTRUCTOR_ARGS_LIST) throw() :
		GSCommonException(UTIL_EXCEPTION_CONSTRUCTOR_ARGS_SET) {
}

ClientException::~ClientException() throw() {
}

void ClientException::appendParameter(
		const char8_t *name, const char8_t *value) throw() {
	try {
		parameterList_.push_back(Parameter(name, value));
	}
	catch (...) {
	}
}

size_t ClientException::getParameterCount() const throw() {
	try {
		return parameterList_.size();
	}
	catch (...) {
		return 0;
	}
}

const char8_t* ClientException::getParameterName(size_t index) const throw() {
	if (index >= getParameterCount()) {
		return "";
	}

	return parameterList_[index].first.c_str();
}

const char8_t* ClientException::getParameterValue(size_t index) const throw() {
	if (index >= getParameterCount()) {
		return "";
	}

	return parameterList_[index].second.c_str();
}

void ClientException::inheritParameters(
		const std::exception *causeInHandling) throw() {
	if (causeInHandling == NULL) {
		return;
	}

	try {
		throw;
	}
	catch (ClientException &e) {
		inheritParameters(e);
	}
	catch (...) {
	}
}

void ClientException::inheritParameters(const ClientException &cause) throw() {
	const size_t count = cause.getParameterCount();
	for (size_t i = 0; i < count; i++) {
		appendParameter(cause.getParameterName(i), cause.getParameterValue(i));
	}
}

void ClientException::formatMessageWithParameter(
		std::ostream &os, const util::Exception &e,
		const ClientException &params) {
	bool exists = false;

	if (e.hasMessage()) {
		e.formatMessage(os);
		exists = true;
	}

	const size_t count = params.getParameterCount();
	if (count > 0) {
		if (exists) {
			os << " ";
		}
		os << "(";
		for (size_t i = 0; i < count; i++) {
			if (i > 0) {
				os << ", ";
			}
			os << params.getParameterName(i);
			os << "=";
			os << params.getParameterValue(i);
		}
		os << ")";
	}
}


volatile bool ErrorStack::detailErrorMessageEnabled_ = false;

size_t ErrorStack::getSize() const throw() {
	if (exception_.isEmpty()) {
		return 0;
	}
	return exception_.getMaxDepth() + 1;
}

GSResult ErrorStack::getErrorCode(size_t index) const throw() {
	return exception_.getErrorCode(index);
}

size_t ErrorStack::formatErrorName(
		size_t index, GSChar *strBuf, size_t bufSize) const throw() {
	try {
		util::NormalOStringStream oss;
		exception_.formatErrorCodeName(oss, index);
		return ClientUtil::copyString(oss.str(), strBuf, bufSize);
	}
	catch (...) {
		ClientUtil::copyString("", 1, strBuf, bufSize);
	}
	return 0;
}

size_t ErrorStack::formatErrorDescription(
		size_t index, GSChar *strBuf, size_t bufSize) const throw() {
	try {
		util::NormalOStringStream oss;
		exception_.formatMessage(oss, index);
		return ClientUtil::copyString(oss.str(), strBuf, bufSize);
	}
	catch (...) {
		ClientUtil::copyString("", 1, strBuf, bufSize);
	}
	return 0;
}

size_t ErrorStack::formatErrorMessage(
		size_t index, GSChar *strBuf, size_t bufSize) const throw() {
	try {
		util::NormalOStringStream oss;
		formatErrorMessage(oss, exception_, index, true);
		return ClientUtil::copyString(oss.str(), strBuf, bufSize);
	}
	catch (...) {
		ClientUtil::copyString("", 1, strBuf, bufSize);
	}
	return 0;
}

void ErrorStack::formatErrorMessage(
		std::ostream &os, const util::Exception &e, size_t index,
		bool withCode) {
	const bool codeEnabled = (withCode && e.hasErrorCode(index));
	if (codeEnabled) {
		if (e.hasErrorCodeName(index)) {
			os << "[" <<
					e.getErrorCode(index) << ":" <<
					e.getNamedErrorCode(index).getName() << "]";
		}
		else {
			os << "[Code:" << e.getErrorCode(index) << "]";
		}
	}
	if (codeEnabled && e.hasMessage(index)) {
		os << " ";
	}
	if (e.hasMessage(index)) {
		e.formatMessage(os, index);
	}
}

size_t ErrorStack::formatErrorLocation(
		size_t index, GSChar *strBuf, size_t bufSize,
		bool detail) const throw() {
	try {
		if (!detailErrorMessageEnabled_ && !detail) {
			ClientUtil::copyString("", 1, strBuf, bufSize);
			return 0;
		}

		util::NormalOStringStream oss;
		bool exists = false;
		if (exception_.hasTypeName(index)) {
			exception_.formatTypeName(oss, index);
			exists = true;
		}
		if (exception_.hasFileName(index)) {
			oss << (exists ? " " : "");
			exception_.formatFileName(oss, index);
			exists = true;
		}
		if (exception_.hasFunctionName(index)) {
			oss << (exists ? " " : "");
			exception_.formatFunctionName(oss, index);
			exists = true;
		}
		if (exception_.hasLineNumber(index)) {
			oss << (exists ? " " : "");
			oss << "line=" << exception_.getLineNumber(index);
		}
		return ClientUtil::copyString(oss.str(), strBuf, bufSize);
	}
	catch (...) {
		ClientUtil::copyString("", 1, strBuf, bufSize);
	}
	return 0;
}

size_t ErrorStack::getErrorParameterCount(size_t stackIndex) const throw() {
	if (stackIndex > 0) {
		return 0;
	}
	return exception_.getParameterCount();
}

size_t ErrorStack::formatErrorParameterName(
		size_t stackIndex, size_t parameterIndex,
		GSChar *strBuf, size_t bufSize) const throw() {
	do {
		if (stackIndex > 0) {
			break;
		}
		if (parameterIndex >= exception_.getParameterCount()) {
			break;
		}
		return ClientUtil::copyString(
				exception_.getParameterName(parameterIndex), strBuf, bufSize);
	}
	while (false);
	ClientUtil::copyString("", 1, strBuf, bufSize);
	return 0;
}

size_t ErrorStack::formatErrorParameterValue(
		size_t stackIndex, size_t parameterIndex,
		GSChar *strBuf, size_t bufSize) const throw() {
	do {
		if (stackIndex > 0) {
			break;
		}
		if (parameterIndex >= exception_.getParameterCount()) {
			break;
		}
		return ClientUtil::copyString(
				exception_.getParameterValue(parameterIndex), strBuf, bufSize);
	}
	while (false);
	ClientUtil::copyString("", 1, strBuf, bufSize);
	return 0;
}

void ErrorStack::clear() throw() {
	exception_ = BaseException();
}

GSResult ErrorStack::setFromCurrentException() throw() {
	std::exception e;
	const BaseException base = GS_CLIENT_EXCEPTION_CONVERT(0, e, "");

	size_t startIndex = 0;
	try {
		throw;
	}
	catch (GSCommonException&) {
		startIndex++;
	}
	catch (...) {
	}
	exception_.assign(base, startIndex);
	exception_.inheritParameters(base);

	return getErrorCode(0);
}

ErrorStack ErrorStack::tryRead(
		ArrayByteInStream &in,
		const util::Exception::NamedErrorCode &defaultCode) throw() {
	ErrorStack stack;
	u8string strBuf;
	try {
		GSExceptionCoder().decode(in, stack.exception_, strBuf, defaultCode);
	}
	catch (...) {
	}
	return stack;
}

ClientException& ErrorStack::getException() throw() {
	return exception_;
}

void ErrorStack::setDetailErrorMessageEnabled(bool enabled) {
	detailErrorMessageEnabled_ = enabled;
}


RemoteException::RemoteException(
		UTIL_EXCEPTION_CONSTRUCTOR_ARGS_LIST) throw() :
		ClientException(UTIL_EXCEPTION_CONSTRUCTOR_ARGS_SET) {
}

RemoteException::~RemoteException() throw() {
}


StatementException::StatementException(
		UTIL_EXCEPTION_CONSTRUCTOR_ARGS_LIST) throw() :
		RemoteException(UTIL_EXCEPTION_CONSTRUCTOR_ARGS_SET) {
}

StatementException::~StatementException() throw() {
}


ConnectionException::ConnectionException(
		UTIL_EXCEPTION_CONSTRUCTOR_ARGS_LIST) throw() :
		RemoteException(UTIL_EXCEPTION_CONSTRUCTOR_ARGS_SET) {
}

ConnectionException::~ConnectionException() throw() {
}


WrongNodeException::WrongNodeException(
		UTIL_EXCEPTION_CONSTRUCTOR_ARGS_LIST) throw() :
		ConnectionException(UTIL_EXCEPTION_CONSTRUCTOR_ARGS_SET) {
}

WrongNodeException::~WrongNodeException() throw() {
}


ErrorReasonFormatter::ErrorReasonFormatter(
		const std::exception &exception) : exception_(exception) {
}

std::ostream& ErrorReasonFormatter::format(std::ostream &stream) const {
	util::LocaleUtils::CLocaleScope localeScope(stream);

	std::exception cause;
	const ClientException e = GS_CLIENT_EXCEPTION_CONVERT(0, cause, "");

	const size_t depth = 0;
	ErrorStack::formatErrorMessage(stream, e, depth, false);

	return stream;
}

std::ostream& operator<<(
		std::ostream &stream, const ErrorReasonFormatter &formatter) {
	return formatter.format(stream);
}


const bool RowMapper::ACCEPT_AGGREGATION_RESULT_COLUMN_ID = false;
const bool RowMapper::RESTRICT_KEY_ORDER_FIRST = true;

RowMapper::Cache *RowMapper::cache_ = NULL;

const RowMapper::Config RowMapper::BASIC_CONFIG(false, false, true);

const RowMapper RowMapper::AGGREGATION_RESULT_MAPPER(
		CATEGORY_AGGREGATION_RESULT, NULL, BASIC_CONFIG);

GS_STRUCT_BINDING(GSQueryAnalysisEntry,
		GS_STRUCT_BINDING_ELEMENT(id, GS_TYPE_INTEGER)
		GS_STRUCT_BINDING_ELEMENT(depth, GS_TYPE_INTEGER)
		GS_STRUCT_BINDING_ELEMENT(type, GS_TYPE_STRING)
		GS_STRUCT_BINDING_ELEMENT(valueType, GS_TYPE_STRING)
		GS_STRUCT_BINDING_ELEMENT(value, GS_TYPE_STRING)
		GS_STRUCT_BINDING_ELEMENT(statement, GS_TYPE_STRING));

const RowMapper RowMapper::QUERY_ANALYSIS_MAPPER(
		CATEGORY_COLLECTION, GS_GET_STRUCT_BINDING(GSQueryAnalysisEntry),
		BASIC_CONFIG);

RowMapper::RowMapper(
		RowTypeCategory rowTypeCategory, const GSBinding *binding,
		const Config &config) :
		digest_(0),
		refCount_(0),
		rowTypeCategory_(rowTypeCategory),
		general_(config.anyTypeAllowed_),
		nullableAllowed_(config.nullableAllowed_),
		binding_(checkAndCopyBinding(
				binding, columnIdMap_, rowTypeCategory, config)),
		keyColumnId_(findRowKeyId(&binding_)),
		varColumnCount_(-1), nullsByteSize_(-1), nullsOffset_(-1)
{
	setupAccessInfo();
}

RowMapper::RowMapper(
		const RowMapper &srcMapper, ArrayByteInStream &schemaIn,
		const Config &config, bool columnOrderIgnorable) :
		digest_(0),
		refCount_(0),
		rowTypeCategory_(srcMapper.rowTypeCategory_),
		general_(srcMapper.general_),
		nullableAllowed_(srcMapper.nullableAllowed_),
		binding_(createReorderedBinding(
				srcMapper, columnIdMap_, schemaIn, config,
				columnOrderIgnorable)),
		keyColumnId_(findRowKeyId(&binding_)),
		varColumnCount_(-1), nullsByteSize_(-1), nullsOffset_(-1)
{
	setupAccessInfo();
}

RowMapper::RowMapper(
		size_t digest, RowTypeCategory rowTypeCategory, bool general,
		bool nullableAllowed) :
		digest_(digest),
		refCount_(0),
		rowTypeCategory_(rowTypeCategory),
		general_(general),
		nullableAllowed_(nullableAllowed),
		keyColumnId_(-1),
		varColumnCount_(-1),
		nullsByteSize_(-1),
		nullsOffset_(-1)
{
	binding_.entries = NULL;
	binding_.entryCount = 0;
}

RowMapper::~RowMapper() {
	delete[] binding_.entries;
	binding_.entries = NULL;
}

std::auto_ptr<RowMapper> RowMapper::getInstance(
		ArrayByteInStream &in, const Config &config,
		RowTypeCategory category) {

	RowMapper::VarDataPool varDataPool;
	GSContainerInfo info = GS_CONTAINER_INFO_INITIALIZER;
	std::vector<GSColumnInfo> columnInfoList;
	const bool withContainerType = false;

	GSGridStoreTag::importSchemaProperty(
			in, config, varDataPool, info, columnInfoList, withContainerType);

	const size_t columnCount = columnInfoList.size();
	if (columnCount > 0) {
		info.columnCount = columnCount;
		info.columnInfoList = &columnInfoList[0];
	}

	std::vector<GSBindingEntry> entryList;
	const GSBinding &binding = GSRow::createBinding(
			RowMapper::toInfoRef(&info, ClientVersion()),
			entryList, config.anyTypeAllowed_);

	std::auto_ptr<RowMapper> mapper(
			new RowMapper(category, &binding, config));

	return mapper;
}

void RowMapper::setupAccessInfo() {
	if (static_cast<int32_t>(nullsByteSize_) == -1 && binding_.entries) {
		nullsByteSize_ = ClientUtil::calcNullsByteSize(binding_.entryCount);
		varColumnCount_ = 0;
		for (size_t i = 0; i < binding_.entryCount; i++) {
			const bool arrayUsed = (binding_.entries[i].arraySizeOffset != static_cast<size_t>(-1));
			if (arrayUsed
				|| (binding_.entries[i].elementType == GS_TYPE_STRING)
				|| (binding_.entries[i].elementType == GS_TYPE_BLOB)
				|| (binding_.entries[i].elementType == GS_TYPE_GEOMETRY)
				|| (binding_.entries[i].elementType == ANY_NULL_TYPE)) {
				++varColumnCount_;
			}
		}
		nullsOffset_ = (varColumnCount_ > 0) ? sizeof(uint64_t) : 0;
	}
}

RowMapper::Cache& RowMapper::getDefaultCache() {
	if (cache_ == NULL) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ALLOCATION_FAILED, "");
	}
	return *cache_;
}

const RowMapper& RowMapper::getAggregationResultMapper() {
	return AGGREGATION_RESULT_MAPPER;
}

const RowMapper& RowMapper::getQueryAnalysisMapper() {
	return QUERY_ANALYSIS_MAPPER;
}

void RowMapper::checkSchemaMatched(const RowMapper &mapper) const {
	if (this == &mapper) {
		return;
	}

	if (rowTypeCategory_ == CATEGORY_AGGREGATION_RESULT ||
			mapper.rowTypeCategory_ == CATEGORY_AGGREGATION_RESULT) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	if (binding_.entryCount != mapper.binding_.entryCount) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
	}

	if (hasKey() ^ mapper.hasKey()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
	}

	for (size_t i = 0; i < binding_.entryCount; i++) {
		const GSBindingEntry &thisEntry = binding_.entries[i];
		const GSBindingEntry &entry = mapper.binding_.entries[i];

		if (thisEntry.elementType != entry.elementType) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
		}

		if (isArrayColumn(thisEntry) ^ isArrayColumn(entry)) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
		}

		if (strcmp(thisEntry.columnName, entry.columnName) != 0 &&
				ClientUtil::normalizeSymbolUnchecked(thisEntry.columnName) !=
				ClientUtil::normalizeSymbolUnchecked(entry.columnName)) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
		}
	}
}

bool RowMapper::matches(
		RowTypeCategory rowTypeCategory, const GSBinding *binding,
		bool general, const Config &config) const {
	if (rowTypeCategory_ != rowTypeCategory) {
		return false;
	}

	if (general_ ^ general) {
		return false;
	}

	if (nullableAllowed_ ^ config.nullableAllowed_) {
		return false;
	}

	const size_t entryCount = (binding == NULL ? 0 : binding->entryCount);
	if (binding_.entryCount != entryCount ||
			(entryCount > 0 && binding->entries == NULL)) {
		return false;
	}

	for (size_t i = 0; i < entryCount; i++) {
		const GSBindingEntry &targetEntry = binding->entries[i];
		const GSBindingEntry &entry = binding_.entries[i];
		if (toNonNullable(targetEntry.elementType) != entry.elementType ||
				targetEntry.offset != entry.offset ||
				targetEntry.arraySizeOffset != entry.arraySizeOffset ||
				filterTypeOptions(
						targetEntry,
						config.anyTypeAllowed_,
						config.nullableAllowed_) != entry.options ||
				targetEntry.columnName == NULL) {
			return false;
		}

		if (strcmp(targetEntry.columnName, entry.columnName) != 0) {
			ColumnIdMap::const_iterator it = columnIdMap_.find(
					ClientUtil::normalizeSymbolUnchecked(entry.columnName));
			if (it == columnIdMap_.end() ||
					targetEntry.columnName != it->second.name_) {
				return false;
			}
		}
	}

	return true;
}

bool RowMapper::matches(
		const RowMapper &baseMapper, ArrayByteInStream schemaIn,
		const Config &config) const {
	if (rowTypeCategory_ != baseMapper.rowTypeCategory_) {
		return false;
	}

	if (general_ ^ baseMapper.general_) {
		return false;
	}

	if (nullableAllowed_ ^ baseMapper.nullableAllowed_) {
		return false;
	}

	const size_t entryCount = importColumnCount(schemaIn);
	int32_t keyColumnId = importKeyListBegin(schemaIn, config, entryCount);

	if (entryCount != binding_.entryCount) {
		return false;
	}

	for (size_t i = 0; i < entryCount; i++) {
		GSBindingEntry inEntry;
		std::string columnName;
		importColumnSchema(
				schemaIn, config, inEntry, &columnName, NULL);

		const GSTypeOption mask =
				~static_cast<GSTypeOption>(GS_TYPE_OPTION_KEY);

		const GSBindingEntry &entry = binding_.entries[i];
		if (inEntry.elementType != entry.elementType ||
				isArrayColumn(inEntry) != isArrayColumn(entry) ||
				(inEntry.options & mask) != entry.options ||
				strcmp(inEntry.columnName, entry.columnName) != 0) {
			return false;
		}

		ColumnIdMap::const_iterator it =
				baseMapper.columnIdMap_.find(columnName);
		if (it == baseMapper.columnIdMap_.end()) {
			return false;
		}

		const GSBindingEntry &baseEntry =
				baseMapper.binding_.entries[it->second.id_];
		if (baseEntry.elementType != entry.elementType ||
				baseEntry.offset != entry.offset ||
				baseEntry.arraySizeOffset != entry.arraySizeOffset ||
				(baseEntry.options & mask) != entry.options) {
			return false;
		}
	}

	importKeyListEnd(schemaIn, config, entryCount, keyColumnId);
	if (keyColumnId != keyColumnId_) {
		return false;
	}

	return true;
}

size_t RowMapper::getDigest(
		RowTypeCategory rowTypeCategory, const GSBinding *binding,
		bool general, bool nullableAllowed) {
	size_t result = 1;
	result = 31 * result + rowTypeCategory;
	result = 31 * result + (general ? 1231 : 1237);
	result = 31 * result + (nullableAllowed ? 1231 : 1237);

	const size_t entryCount = (binding == NULL ? 0 : binding->entryCount);
	if (entryCount > 0 && binding->entries == NULL) {
		return 0;
	}

	for (size_t i = 0; i < entryCount; i++) {
		const GSBindingEntry &entry = binding->entries[i];
		result = 31 * result + getDigest(entry);
	}

	return result;
}

size_t RowMapper::getDigest(
		const RowMapper &baseMapper, ArrayByteInStream schemaIn,
		const Config &config) {
	const size_t entryCount = importColumnCount(schemaIn);
	int32_t keyColumnId = importKeyListBegin(schemaIn, config, entryCount);

	if (entryCount != baseMapper.binding_.entryCount) {
		return 0;
	}

	size_t result = 1;
	result = 31 * result + baseMapper.rowTypeCategory_;
	result = 31 * result + (baseMapper.general_ ? 1231 : 1237);
	result = 31 * result + (baseMapper.nullableAllowed_ ? 1231 : 1237);

	for (size_t i = 0; i < entryCount; i++) {
		GSBindingEntry inEntry;
		std::string columnName;
		importColumnSchema(
				schemaIn, config, inEntry, &columnName, NULL);

		ColumnIdMap::const_iterator it =
				baseMapper.columnIdMap_.find(columnName);
		if (it == baseMapper.columnIdMap_.end()) {
			return 0;
		}

		result = 31 * result +
				getDigest(baseMapper.binding_.entries[it->second.id_]);
	}

	importKeyListEnd(schemaIn, config, entryCount, keyColumnId);

	return result;
}

GSContainerType RowMapper::getContainerType() const {
	switch (rowTypeCategory_) {
	case CATEGORY_COLLECTION:
		return GS_CONTAINER_COLLECTION;
		break;
	case CATEGORY_TIME_SERIES:
		return GS_CONTAINER_TIME_SERIES;
		break;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}
}

void RowMapper::getContainerSchema(
		ContainerInfoRef<> &containerInfoRef, VarDataPool &varDataPool) const {
	if (rowTypeCategory_ == CATEGORY_AGGREGATION_RESULT) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
	}

	GSContainerInfo containerInfo = GS_CONTAINER_INFO_INITIALIZER;

	containerInfo.type = GS_CONTAINER_COLLECTION;

	containerInfo.rowKeyAssigned = ClientUtil::toGSBool(hasKey());

	containerInfoRef.set(containerInfo);

	containerInfoRef.createColumnInfoList(binding_.entryCount, varDataPool);
	for (size_t i = 0; i < binding_.entryCount; i++) {
		const GSColumnInfo &columnInfo =
				getColumnSchema(binding_.entries[i], &varDataPool);
		containerInfoRef.setColumnInfo(static_cast<int32_t>(i), columnInfo);
	}
}

GSColumnInfo RowMapper::getColumnSchema(
		const GSBindingEntry &entry, VarDataPool *varDataPool) {
	GSColumnInfo columnInfo = GS_COLUMN_INFO_INITIALIZER;

	if (entry.columnName == NULL || varDataPool == NULL) {
		columnInfo.name = entry.columnName;
	}
	else {
		GSChar *name;

		const size_t nameSize = strlen(entry.columnName) + 1;
		name = static_cast<GSChar*>(varDataPool->allocate(nameSize));
		memcpy(name, entry.columnName, nameSize);

		columnInfo.name = name;
	}

	columnInfo.type = toFullType(entry.elementType, isArrayColumn(entry));
	columnInfo.indexTypeFlags = 0;
	columnInfo.options =
			(entry.options & TypeOptionMask::MASK_GENERAL_SUPPORTED);

	return columnInfo;
}

RowMapper::RowTypeCategory RowMapper::getCategory() const {
	return rowTypeCategory_;
}

bool RowMapper::isGeneral() const {
	return general_;
}

bool RowMapper::isNullableAllowed() const {
	return nullableAllowed_;
}

const GSBinding& RowMapper::getBinding() const {
	return binding_;
}

bool RowMapper::hasKey() const {
	return (keyColumnId_ >= 0);
}

GSType RowMapper::getElementType(int32_t columnId) const {
	const GSBindingEntry &entry = getEntry(columnId);
	return entry.elementType;
}

bool RowMapper::isArray(int32_t columnId) const {
	const GSBindingEntry &entry = getEntry(columnId);
	return isArrayColumn(entry);
}

bool RowMapper::hasAnyTypeColumn() const {
	for (size_t i = 0; i < binding_.entryCount; i++) {
		const GSBindingEntry &entry = binding_.entries[i];
		if (entry.elementType == ANY_NULL_TYPE) {
			return true;
		}
	}
	return false;
}

bool RowMapper::isDefaultValueSpecified() const {
	for (size_t i = 0; i < binding_.entryCount; i++) {
		const GSBindingEntry &entry = binding_.entries[i];
		if ((entry.options & TypeOptionMask::MASK_DEFAULT_VALUE) != 0) {
			return true;
		}
	}
	return false;
}

void RowMapper::exportSchema(
		XArrayByteOutStream &out, const Config &config) const {
	if (rowTypeCategory_ == CATEGORY_AGGREGATION_RESULT) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	exportColumnCount(out, binding_.entryCount);
	exportKeyListBegin(out, config, keyColumnId_);
	for (size_t i = 0; i < binding_.entryCount; i++) {
		exportColumnSchema(out, binding_.entries[i]);
	}
	exportKeyListEnd(out, config, keyColumnId_);
}

size_t RowMapper::importColumnCount(ArrayByteInStream &in) {
	int32_t intCount;
	in >> intCount;
	return ClientUtil::toSizeValue(intCount);
}

void RowMapper::exportColumnCount(
		XArrayByteOutStream &out, size_t columnCount) {
	out << ClientUtil::sizeValueToInt32(columnCount);
}

int32_t RowMapper::importKeyListBegin(
		ArrayByteInStream &in, const Config &config, size_t columnCount) {
	if (!config.keyExtensible_) {
		int32_t columnId;
		in >> columnId;
		if ((columnId != 0 && columnId != -1) ||
				columnId >= static_cast<int32_t>(columnCount)) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
					"Protocol error by illegal index of row key column");
		}
		return columnId;
	}

	return std::numeric_limits<int32_t>::max();
}

void RowMapper::importKeyListEnd(
		ArrayByteInStream &in, const Config &config, size_t columnCount,
		int32_t &keyColumnId) {
	if (config.keyExtensible_) {
		int16_t count;
		in >> count;
		if (!(count == 0 || count == 1)) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
					"Protocol error by illegal row key count");
		}

		keyColumnId = -1;
		for (int16_t i = 0; i < count; i++) {
			int16_t columnId;
			in >> columnId;
			if (columnId != 0 ||
					columnId >= static_cast<int32_t>(columnCount)) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
						"Protocol error by illegal index of row key column");
			}
			if (i == 0) {
				keyColumnId = columnId;
			}
		}
	}

	assert(keyColumnId == 0 || keyColumnId == -1);
}

void RowMapper::exportKeyListBegin(
		XArrayByteOutStream &out, const Config &config,
		const int32_t &keyColumnId) {
	if (!config.keyExtensible_) {
		out << keyColumnId;
	}
}

void RowMapper::exportKeyListEnd(
		XArrayByteOutStream &out, const Config &config,
		const int32_t &keyColumnId) {
	if (config.keyExtensible_) {
		const int16_t count = (keyColumnId < 0 ? 0 : 1);
		out << count;

		for (int16_t i = 0; i < count; i++) {
			const int16_t columnId = static_cast<int16_t>(keyColumnId);
			if (columnId != 0) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
			}
			out << columnId;
		}
	}
}

GSColumnInfo RowMapper::importColumnSchema(
		ArrayByteInStream &in, const Config &config,
		VarDataPool &varDataPool) {
	GSBindingEntry entry;
	importColumnSchema(in, config, entry, NULL, &varDataPool);
	filterNullable(
			entry.options, 0, config.nullableAllowed_, entry.columnName);

	const bool nullable = isOptionNullable(entry.options);
	filterInitialValueNull(entry.options, nullable, entry.columnName);

	if (config.anyTypeAllowed_ && strlen(entry.columnName) == 0) {
		entry.columnName = NULL;
	}

	return getColumnSchema(entry, NULL);
}

void RowMapper::exportColumnSchema(
		XArrayByteOutStream &out, const GSBindingEntry &entry) {
	out << (entry.columnName == NULL ? "" : entry.columnName);
	out << static_cast<int8_t>(entry.elementType);

	int8_t flags = 0;
	if (isArrayColumn(entry)) {
		flags |= COLUMN_FLAG_ARRAY;
	}
	if ((entry.options & GS_TYPE_OPTION_NOT_NULL) != 0) {
		flags |= COLUMN_FLAG_NOT_NULL;
	}
	out << flags;
}

void RowMapper::importColumnSchema(
		ArrayByteInStream &in, const Config &config, GSBindingEntry &entry,
		std::string *nameStr, VarDataPool *varDataPool) {

	if (nameStr != NULL) {
		in >> *nameStr;
		entry.columnName = nameStr->c_str();
	}
	else {
		assert(varDataPool != NULL);
		entry.columnName = decodeString(in, *varDataPool);
	}

	int8_t rawElementType;
	in >> rawElementType;
	entry.elementType = static_cast<GSType>(rawElementType);

	int8_t flags;
	in >> flags;
	entry.arraySizeOffset =
		((flags & COLUMN_FLAG_ARRAY) == 0 ? static_cast<size_t>(-1) : 0);
	entry.options =
			((config.nullableAllowed_ && (flags & COLUMN_FLAG_NOT_NULL) == 0) ?
					GS_TYPE_OPTION_NULLABLE : GS_TYPE_OPTION_NOT_NULL);
}

int32_t RowMapper::resolveColumnId(const GSChar *name) const {
	ColumnIdMap::const_iterator it = columnIdMap_.find(name);
	if (it == columnIdMap_.end()) {
		it = columnIdMap_.find(
				ClientUtil::normalizeSymbol(name, "column name"));
		if (it == columnIdMap_.end()) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNKNOWN_COLUMN_NAME, "");
		}
	}

	return it->second.id_;
}

void RowMapper::encodeKeyGeneral(
		XArrayByteOutStream &out, MappingMode mode,
		const GSType keyType, const GSValue &value) {
	switch (keyType) {
	case GS_TYPE_STRING:
		if (mode == MODE_ROWWISE_SEPARATED_V2) {
			ClientUtil::writeVarDataString(out, value.asString);
		} else {
			out << value.asString;
		}
		break;
	case GS_TYPE_INTEGER:
		out << value.asInteger;
		break;
	case GS_TYPE_LONG:
		out << value.asLong;
		break;
	case GS_TYPE_TIMESTAMP:
		out << value.asTimestamp;
		break;
	default:
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_KEY_TYPE, "");
	}
}

void RowMapper::encodeKeyByObj(
		XArrayByteOutStream &out, MappingMode mode,
		const GSType *keyType, const void *keyObj) const {
	assert(keyObj != NULL);

	const GSBindingEntry &entry = resolveKeyEntry();
	assert(!isArrayColumn(entry));

	if (keyType != NULL && *keyType != entry.elementType) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_KEY_TYPE, "");
	}

	switch (entry.elementType) {
	case GS_TYPE_STRING:
		{
			const GSChar *keyStr = *static_cast<const GSChar* const*>(keyObj);
			if (keyStr == NULL) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
			}
			if (mode == MODE_ROWWISE_SEPARATED_V2) {
				ClientUtil::writeVarDataString(out, keyStr);
			}
			else {
				out << keyStr;
			}
		}
		break;
	case GS_TYPE_INTEGER:
		out << *static_cast<const int32_t*>(keyObj);
		break;
	case GS_TYPE_LONG:
		out << *static_cast<const int64_t*>(keyObj);
		break;
	case GS_TYPE_TIMESTAMP:
		out << *static_cast<const GSTimestamp*>(keyObj);
		break;
	default:
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_KEY_TYPE, "");
	}
}

void RowMapper::encodeKeyByString(
		XArrayByteOutStream &out, MappingMode mode,
		const GSChar *keyString, OutputCursor *cursor) const {
	const GSBindingEntry &entry = resolveKeyEntry();
	assert(!isArrayColumn(entry));

	switch (entry.elementType) {
	case GS_TYPE_STRING:
		if (cursor == NULL) {
			if (mode == MODE_ROWWISE_SEPARATED_V2) {
				ClientUtil::writeVarDataString(out, keyString);
			} else {
				out << keyString;
			}
		}
		else {
			cursor->beginVarData();
			if (mode == MODE_ROWWISE_SEPARATED_V2) {
				ClientUtil::writeVarDataString(out, keyString);
			} else {
				out << keyString;
			}
			cursor->endVarData();
		}
		break;
	case GS_TYPE_INTEGER:
		out << ClientUtil::parseValue<int32_t>(keyString);
		break;
	case GS_TYPE_LONG:
		out << ClientUtil::parseValue<int64_t>(keyString);
		break;
	case GS_TYPE_TIMESTAMP: {
		GSTimestamp timestamp;
		if (!TimestampUtil::parse(keyString, timestamp)) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_VALUE_FORMAT, keyString);
		}
		out << timestamp;
		break;
	}
	default:
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_KEY_TYPE, "");
	}
}

void RowMapper::encode(XArrayByteOutStream &out, MappingMode mode,
		const GSType *keyType, const void *keyObj,
		const void *rowObj) const {
	OutputCursor cursor(out, *this, mode, 1);
	encode(cursor, keyType, keyObj, rowObj);
}

void RowMapper::encode(OutputCursor &cursor,
		const GSType *keyType, const void *keyObj,
		const void *rowObj) const {
	if (rowTypeCategory_ == CATEGORY_AGGREGATION_RESULT) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_ROW_MAPPING, "");
	}
	if (keyObj != NULL && !hasKey()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_KEY_NOT_ACCEPTED, "");
	}

	const RowMapper &codingMapper = resolveCodingMapper(rowObj);
	cursor.beginRow(codingMapper, -1, codingMapper.findNulls(rowObj));
	for (size_t i = 0; i < binding_.entryCount; i++) {
		codingMapper.encodeField(
				cursor, static_cast<int32_t>(i), keyType, keyObj, rowObj);
	}
	cursor.endRow();
}

void RowMapper::encodeWithKeyString(
		XArrayByteOutStream &out, MappingMode mode,
		const GSChar *keyString, const void *rowObj) const {
	if (rowTypeCategory_ == CATEGORY_AGGREGATION_RESULT || !hasKey()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_ROW_MAPPING, "");
	}

	const RowMapper &codingMapper = resolveCodingMapper(rowObj);
	OutputCursor cursor(out, codingMapper, mode, 1);
	cursor.beginRow(codingMapper, -1, codingMapper.findNulls(rowObj));
	for (size_t i = 0; i < binding_.entryCount; i++) {
		if (static_cast<int32_t>(i) == keyColumnId_) {
			cursor.beginField();
			codingMapper.encodeKeyByString(out, mode, keyString, &cursor);
		}
		else {
			codingMapper.encodeField(
					cursor, static_cast<int32_t>(i), NULL, NULL, rowObj);
		}
	}
	cursor.endRow();
}

void RowMapper::decode(ArrayByteInStream &in, MappingMode mode,
		VarDataPool *varDataPool, void *rowObj) const {
	InputCursor cursor(in, *this, mode, 1, false, varDataPool);
	decode(cursor, rowObj);
}

void RowMapper::decode(InputCursor &cursor, void *rowObj) const {
	if (rowTypeCategory_ == CATEGORY_AGGREGATION_RESULT ||
			cursor.base_.mode_ == MODE_AGGREGATED) {
		if (general_) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
		}
		cursor.beginRow(*this, NULL);
		decodeAggregation(cursor, rowObj);
		cursor.endRow();
	}
	else {
		void *pendingVarData = NULL;
		size_t pendingPtrArraySize = 0;

		try {
			const RowMapper &codingMapper = resolveCodingMapper(rowObj);
			cursor.beginRow(codingMapper, codingMapper.findNulls(rowObj));
			for (size_t i = 0; i < binding_.entryCount; i++) {
				codingMapper.decodeField(
						cursor, static_cast<int32_t>(i), rowObj,
						pendingVarData, pendingPtrArraySize);
			}
			cursor.endRow();
		}
		catch (...) {
			if (pendingVarData != NULL) {
				for (; pendingPtrArraySize > 0; pendingPtrArraySize--) {
					void *elemPtr = static_cast<void**>(
							pendingVarData)[pendingPtrArraySize];
					deallocate(cursor.varDataPool_, rowObj, elemPtr);

				}
				deallocate(cursor.varDataPool_, rowObj, pendingVarData);
			}

			if (general_) {
				static_cast<GSRow*>(rowObj)->clear(true);
			}
			throw;
		}
	}
}

void* RowMapper::extractKey(void *rowObj, KeyStorage &keyStorage) const {
	if (!hasKey()) {
		return NULL;
	}

	const GSBindingEntry &entry = resolveKeyEntry();
	assert(!isArrayColumn(entry));

	switch (entry.elementType) {
	case GS_TYPE_STRING:
		keyStorage.strStorage_ = *static_cast<const GSChar *const*>(
				getField(resolveKeyEntry(), rowObj));
		keyStorage.value_.strValue_ = keyStorage.strStorage_.c_str();
		break;
	case GS_TYPE_INTEGER:
		keyStorage.value_.intValue_ = *static_cast<int32_t*>(
				getField(resolveKeyEntry(), rowObj));
		break;
	case GS_TYPE_LONG:
		keyStorage.value_.longValue_ = *static_cast<int64_t*>(
				getField(resolveKeyEntry(), rowObj));
		break;
	case GS_TYPE_TIMESTAMP:
		keyStorage.value_.longValue_ = *static_cast<GSTimestamp*>(
				getField(resolveKeyEntry(), rowObj));
		break;
	default:
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_KEY_TYPE, "");
	}
	return &keyStorage;
}

size_t RowMapper::getGeneralRowSize() const {
	if (!general_) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	assert(binding_.entryCount > 0);
	size_t size = 0;
	for (size_t i = 0; i < binding_.entryCount; i++) {
		const GSBindingEntry &entry = binding_.entries[i];
		size = std::max(size, entry.offset + getFieldObjectMainPartSize(
				entry.elementType, isArrayColumn(entry)));
	}

	return size;
}

void RowMapper::encodeGeometry(
		XArrayByteOutStream &out, const GSChar *text, MappingMode mode) {
	GS_CLIENT_THROW_ERROR(
			GS_ERROR_CC_UNSUPPORTED_OPERATION,
			"GEOMETRY is not a supported type");
}

template<typename Alloc>
const GSChar* RowMapper::findEmptyString(Alloc &alloc) {
	static_cast<void>(alloc);
	return NULL;
}

const GSChar* RowMapper::findEmptyString(VarDataPool &pool) {
	static_cast<void>(pool);
	return "";
}

template<typename Alloc>
const GSChar* RowMapper::decodeString(
		ArrayByteInStream &in, Alloc &alloc) {
	int32_t size;
	in >> size;
	return readStringWithSize(in, ClientUtil::toSizeValue(size), alloc);
}

const GSChar* RowMapper::decodeString(
		ArrayByteInStream &in, VarDataPool *pool, void *rowObj) const {
	if (general_) {
		return decodeString(in, *static_cast<GSRow*>(rowObj));
	}
	else {
		return decodeString(in, *pool);
	}
}

template<typename Alloc>
const GSChar* RowMapper::decodeStringV2(
		ArrayByteInStream &in, Alloc &alloc) {
	return readStringWithSize(in, ClientUtil::readVarSize(in), alloc);
}

const GSChar* RowMapper::decodeStringV2(
		ArrayByteInStream &in, VarDataPool *pool, void *rowObj) const {
	if (general_) {
		return decodeStringV2(in, *static_cast<GSRow*>(rowObj));
	}
	else {
		return decodeStringV2(in, *pool);
	}
}

template<typename Alloc>
const GSChar* RowMapper::readStringWithSize(
		ArrayByteInStream &in, size_t inSize, Alloc &alloc) {
	if (inSize == 0) {
		const GSChar *emptyString = findEmptyString(alloc);
		if (emptyString != NULL) {
			return emptyString;
		}
	}

	GSChar *str = static_cast<GSChar*>(alloc.allocate(inSize + 1));
	try {
		in.readAll(str, inSize);
		str[inSize] = '\0';
		return str;
	}
	catch (...) {
		try {
			alloc.deallocate(str);
		}
		catch (...) {
		}
		throw;
	}
}

template<typename Alloc>
const GSChar* RowMapper::copyString(const GSChar *src, Alloc &alloc) {
	return copyString(src, strlen(src), alloc);
}

template<typename Alloc>
const GSChar* RowMapper::copyString(
		const GSChar *src, size_t srcSize, Alloc &alloc) {
	GSChar *dest = static_cast<GSChar*>(alloc.allocate(srcSize + 1));
	memcpy(dest, src, srcSize);
	dest[srcSize] = '\0';
	return dest;
}

GSType RowMapper::toFullType(GSType type, bool arrayUsed) {
	if (arrayUsed) {
		GSType arrayType;
		switch (toNonNullable(type)) {
		case GS_TYPE_STRING:
			arrayType = GS_TYPE_STRING_ARRAY;
			break;
		case GS_TYPE_BOOL:
			arrayType = GS_TYPE_BOOL_ARRAY;
			break;
		case GS_TYPE_BYTE:
			arrayType = GS_TYPE_BYTE_ARRAY;
			break;
		case GS_TYPE_SHORT:
			arrayType = GS_TYPE_SHORT_ARRAY;
			break;
		case GS_TYPE_INTEGER:
			arrayType = GS_TYPE_INTEGER_ARRAY;
			break;
		case GS_TYPE_LONG:
			arrayType = GS_TYPE_LONG_ARRAY;
			break;
		case GS_TYPE_FLOAT:
			arrayType = GS_TYPE_FLOAT_ARRAY;
			break;
		case GS_TYPE_DOUBLE:
			arrayType = GS_TYPE_DOUBLE_ARRAY;
			break;
		case GS_TYPE_TIMESTAMP:
			arrayType = GS_TYPE_TIMESTAMP_ARRAY;
			break;
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
		}
		return toNullable(arrayType, isTypeNullable(type));
	}
	else {
		return type;
	}
}

GSType RowMapper::toElementType(GSType type, bool &arrayUsed) {
	arrayUsed = true;

	GSType baseType;
	switch (toNonNullable(type)) {
	case GS_TYPE_STRING_ARRAY:
		baseType = GS_TYPE_STRING;
		break;
	case GS_TYPE_BOOL_ARRAY:
		baseType = GS_TYPE_BOOL;
		break;
	case GS_TYPE_BYTE_ARRAY:
		baseType = GS_TYPE_BYTE;
		break;
	case GS_TYPE_SHORT_ARRAY:
		baseType = GS_TYPE_SHORT;
		break;
	case GS_TYPE_INTEGER_ARRAY:
		baseType = GS_TYPE_INTEGER;
		break;
	case GS_TYPE_LONG_ARRAY:
		baseType = GS_TYPE_LONG;
		break;
	case GS_TYPE_FLOAT_ARRAY:
		baseType = GS_TYPE_FLOAT;
		break;
	case GS_TYPE_DOUBLE_ARRAY:
		baseType = GS_TYPE_DOUBLE;
		break;
	case GS_TYPE_TIMESTAMP_ARRAY:
		baseType = GS_TYPE_TIMESTAMP;
		break;
	default:
		arrayUsed = false;
		return type;
	}

	return toNullable(baseType, isTypeNullable(type));
}

bool RowMapper::isArrayColumn(const GSBindingEntry &entry) {
	return (entry.arraySizeOffset != static_cast<size_t>(-1));
}

GSBindingEntry RowMapper::getEntryGeneral(
		const GSBindingEntry &src, const GSType type) {
	GSBindingEntry dest = GS_BINDING_ENTRY_INITIALIZER;

	bool arrayUsed;
	dest.elementType = RowMapper::toElementType(type, arrayUsed);

	const size_t base = src.offset + sizeof(int8_t);
	dest.offset = base + (arrayUsed ? offsetof(GSValue, asArray.elements) : 0);
	dest.arraySizeOffset = (arrayUsed ?
			base + offsetof(GSValue, asArray.length) :
			static_cast<size_t>(-1));

	return dest;
}

size_t RowMapper::getFieldObjectMainPartSize(
		GSType elementType, bool arrayUsed) {
	if (arrayUsed) {
		return sizeof(void*) + sizeof(size_t);
	}
	else {
		switch (toNonNullable(elementType)) {
		case GS_TYPE_STRING:
			return sizeof(GSChar*);
		case GS_TYPE_BOOL:
			return sizeof(GSBool);
		case GS_TYPE_BYTE:
			return sizeof(int8_t);
		case GS_TYPE_SHORT:
			return sizeof(int16_t);
		case GS_TYPE_INTEGER:
			return sizeof(int32_t);
		case GS_TYPE_LONG:
			return sizeof(int64_t);
		case GS_TYPE_FLOAT:
			return sizeof(float);
		case GS_TYPE_DOUBLE:
			return sizeof(double);
		case GS_TYPE_TIMESTAMP:
			return sizeof(GSTimestamp);
		case GS_TYPE_GEOMETRY:
			return sizeof(GSChar*);
		case GS_TYPE_BLOB:
			return sizeof(GSBlob);
		case ANY_NULL_TYPE:
			return (sizeof(int8_t) + sizeof(GSValue));
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
		}
	}
}

template<typename Caller, typename Operator>
void RowMapper::invokeTypedOperation(
		Caller &caller, Operator &op, const GSBindingEntry &entry) {
	if (isArrayColumn(entry)) {
		switch (toNonNullable(entry.elementType)) {
		case GS_TYPE_STRING:
			op(caller, entry, TypeTraits<GS_TYPE_STRING, true>());
			break;
		case GS_TYPE_BOOL:
			op(caller, entry, TypeTraits<GS_TYPE_BOOL, true>());
			break;
		case GS_TYPE_BYTE:
			op(caller, entry, TypeTraits<GS_TYPE_BYTE, true>());
			break;
		case GS_TYPE_SHORT:
			op(caller, entry, TypeTraits<GS_TYPE_SHORT, true>());
			break;
		case GS_TYPE_INTEGER:
			op(caller, entry, TypeTraits<GS_TYPE_INTEGER, true>());
			break;
		case GS_TYPE_LONG:
			op(caller, entry, TypeTraits<GS_TYPE_LONG, true>());
			break;
		case GS_TYPE_FLOAT:
			op(caller, entry, TypeTraits<GS_TYPE_FLOAT, true>());
			break;
		case GS_TYPE_DOUBLE:
			op(caller, entry, TypeTraits<GS_TYPE_DOUBLE, true>());
			break;
		case GS_TYPE_TIMESTAMP:
			op(caller, entry, TypeTraits<GS_TYPE_TIMESTAMP, true>());
			break;
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
		}
	}
	else {
		switch (toNonNullable(entry.elementType)) {
		case GS_TYPE_STRING:
			op(caller, entry, TypeTraits<GS_TYPE_STRING, false>());
			break;
		case GS_TYPE_BOOL:
			op(caller, entry, TypeTraits<GS_TYPE_BOOL, false>());
			break;
		case GS_TYPE_BYTE:
			op(caller, entry, TypeTraits<GS_TYPE_BYTE, false>());
			break;
		case GS_TYPE_SHORT:
			op(caller, entry, TypeTraits<GS_TYPE_SHORT, false>());
			break;
		case GS_TYPE_INTEGER:
			op(caller, entry, TypeTraits<GS_TYPE_INTEGER, false>());
			break;
		case GS_TYPE_LONG:
			op(caller, entry, TypeTraits<GS_TYPE_LONG, false>());
			break;
		case GS_TYPE_FLOAT:
			op(caller, entry, TypeTraits<GS_TYPE_FLOAT, false>());
			break;
		case GS_TYPE_DOUBLE:
			op(caller, entry, TypeTraits<GS_TYPE_DOUBLE, false>());
			break;
		case GS_TYPE_TIMESTAMP:
			op(caller, entry, TypeTraits<GS_TYPE_TIMESTAMP, false>());
			break;
		case GS_TYPE_GEOMETRY:
			op(caller, entry, TypeTraits<GS_TYPE_GEOMETRY, false>());
			break;
		case GS_TYPE_BLOB:
			op(caller, entry, TypeTraits<GS_TYPE_BLOB, false>());
			break;
		case RowMapper::ANY_NULL_TYPE:
			op(caller, entry, TypeTraits<RowMapper::ANY_NULL_TYPE, false>());
			break;
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
		}
	}
}

template<typename Alloc, typename Mask>
GSValue RowMapper::copyValue(const GSValue &src,
		GSType elementType, bool arrayUsed, Alloc *alloc, const Mask&) {
	ValueCopier<Alloc, Mask> copier(alloc, src);

	GSBindingEntry entry = GS_BINDING_ENTRY_INITIALIZER;
	entry.elementType = elementType;
	entry.arraySizeOffset = arrayUsed ? 0 : static_cast<size_t>(-1);

	GSValue dest;
	memset(&dest, 0, sizeof(dest));
	invokeTypedOperation(dest, copier, entry);

	return dest;
}

template<typename Traits, typename Alloc, typename Mask, typename T>
GSValue RowMapper::copyValue(const GSValue &src, const Traits&, Alloc*,
		const Mask&, T) {
	UTIL_STATIC_ASSERT(!Traits::ARRAY_TYPE);
	UTIL_STATIC_ASSERT(Traits::ELEMENT_TYPE != GS_TYPE_STRING);
	UTIL_STATIC_ASSERT(Traits::ELEMENT_TYPE != GS_TYPE_GEOMETRY);
	UTIL_STATIC_ASSERT(Traits::ELEMENT_TYPE != GS_TYPE_BLOB);

	return src;
}

template<typename Traits, typename Alloc, typename Mask, typename E>
GSValue RowMapper::copyValue(const GSValue &src, const Traits&, Alloc *alloc,
		const Mask&, const E*) {
	UTIL_STATIC_ASSERT(!(Traits::ARRAY_TYPE &&
			Traits::ELEMENT_TYPE == GS_TYPE_STRING));

	return StringOrArrayCopier<Traits, Alloc, Mask>()(src, alloc);
}

template<typename Traits, typename Alloc, typename Mask>
GSValue RowMapper::copyValue(const GSValue &src, const Traits&, Alloc *alloc,
		const Mask&, const GSChar *const*) {
	GSValue dest;

	const size_t arraySize = Traits::arraySizeOf(src);
	Traits::arraySizeOf(dest) = arraySize;

	if (arraySize > 0) {
		if (Traits::as(src) == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}
		GSChar **destArray = static_cast<GSChar**>(
				alloc->allocate(sizeof(GSChar*) * arraySize));

		size_t i = 0;
		try {
			for (size_t i = 0; i < arraySize; i++) {
				const GSChar *srcStr = Traits::as(src)[i];
				if (srcStr == NULL) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
				}

				const size_t strSize = strlen(srcStr) + 1;
				destArray[i] = static_cast<GSChar*>(alloc->allocate(strSize));
				memcpy(destArray[i], srcStr, strSize);
			}
		}
		catch (...) {
			for (size_t j = 0; j < i; j++) {
				alloc->deallocate(destArray[i]);
			}
			alloc->deallocate(destArray);
			throw;
		}
		Traits::as(dest) = destArray;
	}
	else {
		Traits::as(dest) = NULL;
	}

	return dest;
}

template<typename Traits, typename Alloc, typename Mask>
GSValue RowMapper::copyValue(const GSValue &src, const Traits&, Alloc *alloc,
		const Mask&, GSBlob) {
	GSValue dest;

	const size_t size = src.asBlob.size;
	dest.asBlob.size = size;

	if (size > 0) {
		if (src.asBlob.data == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}

		void *data = alloc->allocate(size);
		memcpy(data, src.asBlob.data, size);
		dest.asBlob.data = data;
	}
	else {
		dest.asBlob.data = NULL;
	}

	return dest;
}

GSContainerType RowMapper::checkContainerType(GSContainerType type) {
	containerTypeToCategory(type);
	return type;
}

RowMapper::RowTypeCategory RowMapper::containerTypeToCategory(
		GSContainerType type) {
	switch (type) {
	case GS_CONTAINER_COLLECTION:
		return RowMapper::CATEGORY_COLLECTION;
	case GS_CONTAINER_TIME_SERIES:
		return RowMapper::CATEGORY_TIME_SERIES;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
	}
}

bool RowMapper::isKeyType(GSType type) {
	switch (type) {
	case GS_TYPE_STRING:
	case GS_TYPE_INTEGER:
	case GS_TYPE_LONG:
	case GS_TYPE_TIMESTAMP:
		return true;
	default:
		return false;
	}
}

bool RowMapper::isAny(GSType type) {
	return type == ANY_NULL_TYPE;
}

bool RowMapper::isNullable(const GSBindingEntry &entry) {
	return (isTypeNullable(entry.elementType) ||
			(entry.options & GS_TYPE_OPTION_NULLABLE) != 0);
}

bool RowMapper::isTypeNullable(GSType type) {
	return (type & NULLABLE_MASK) != 0;
}

GSType RowMapper::toNullable(GSType type, bool nullable) {
	if (type == ANY_NULL_TYPE) {
		return type;
	}

	if (nullable) {
		return (type | NULLABLE_MASK);
	}
	else {
		return (type & ~NULLABLE_MASK);
	}
}

GSType RowMapper::toNonNullable(GSType type) {
	return toNullable(type, false);
}

bool RowMapper::isOptionNullable(GSTypeOption options) {
	return ((options & GS_TYPE_OPTION_NULLABLE) != 0);
}

bool RowMapper::isOptionInitialValueNull(GSTypeOption options) {
	return ((options & GS_TYPE_OPTION_DEFAULT_VALUE_NULL) != 0);
}

GSTypeOption RowMapper::filterTypeOptions(
		const GSBindingEntry &entry,
		bool anyTypeAllowed, bool nullableAllowed) {
	GSTypeOption srcOptions = entry.options;

	if ((srcOptions & ~TypeOptionMask::MASK_BINDING_SUPPORTED) != 0) {
		GS_CLIENT_THROW_ERROR(
				GS_ERROR_CC_UNKNOWN_ELEMENT_TYPE_OPTION, "");
	}

	if (isTypeNullable(entry.elementType)) {
		if (!anyTypeAllowed) {
			GS_CLIENT_THROW_ERROR(
					GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
		}
		if (!isAny(entry.elementType)) {
			srcOptions |= GS_TYPE_OPTION_NULLABLE;
		}
	}

	GSTypeOption destOptions = 0;
	if ((srcOptions & GS_TYPE_OPTION_KEY) != 0) {
		destOptions |= GS_TYPE_OPTION_KEY;
	}

	const GSTypeOption nullableDefault = 0;
	destOptions |= filterNullable(
			srcOptions, nullableDefault, nullableAllowed, entry.columnName);

	const bool nullable = isOptionNullable(destOptions);
	destOptions |=
			filterInitialValueNull(srcOptions, nullable, entry.columnName);

	return destOptions;
}

GSTypeOption RowMapper::filterNullable(
		GSTypeOption options, GSTypeOption nullableDefault,
		bool nullableAllowed, const GSChar *columnName) {
	const GSTypeOption mask = TypeOptionMask::MASK_NULLABLE;
	const GSChar *filteredColumnName =
			(columnName == NULL ? "" : columnName);

	if ((options & mask) == mask || (nullableDefault & mask) == mask) {
		GS_CLIENT_THROW_ERROR(
				GS_ERROR_CC_ILLEGAL_SCHEMA,
				"Both of nullable and not null option specified ("
				"column=" << filteredColumnName << ")");
	}

	if (isOptionNullable(options)) {
		if (!nullableAllowed) {
			GS_CLIENT_THROW_ERROR(
					GS_ERROR_CC_ILLEGAL_SCHEMA,
					"Nullable column is not currently available ("
					"column=" << filteredColumnName << ")");
		}

		if ((options & GS_TYPE_OPTION_KEY) != 0) {
			GS_CLIENT_THROW_ERROR(
					GS_ERROR_CC_ILLEGAL_SCHEMA,
					"Row key cannot be null ("
					"column=" << filteredColumnName << ")");
		}
	}

	bool nullable;
	if ((options & mask) == 0) {
		if ((options & GS_TYPE_OPTION_KEY) != 0) {
			nullable = false;
		}
		else if ((nullableDefault & mask) != 0) {
			nullable = isOptionNullable(nullableDefault);
		}
		else {
			nullable = nullableAllowed;
		}
	}
	else {
		nullable = isOptionNullable(options);
	}

	if (nullable) {
		return GS_TYPE_OPTION_NULLABLE;
	}
	else {
		return GS_TYPE_OPTION_NOT_NULL;
	}
}

GSTypeOption RowMapper::filterInitialValueNull(
		GSTypeOption options, bool nullable, const GSChar *columnName) {
	const GSTypeOption mask = TypeOptionMask::MASK_DEFAULT_VALUE;
	const GSChar *filteredColumnName =
			(columnName == NULL ? "" : columnName);

	if ((options & mask) == mask) {
		GS_CLIENT_THROW_ERROR(
				GS_ERROR_CC_ILLEGAL_SCHEMA,
				"Both of default value null and not null option specified ("
				"column=" << filteredColumnName << ")");
	}

	if (!nullable && (options & GS_TYPE_OPTION_DEFAULT_VALUE_NULL) != 0) {
		GS_CLIENT_THROW_ERROR(
				GS_ERROR_CC_ILLEGAL_SCHEMA,
				"Default value null is not allowed ("
				"column=" << filteredColumnName << ")");
	}

	return (options & mask);
}

const RowMapper::ContainerInfoRef<true> RowMapper::toInfoRef(
		const GSContainerInfo *info, const ClientVersion &version) throw() {
	return ContainerInfoRef<true>(info, version);
}

size_t RowMapper::getDigest(const GSBindingEntry &entry) {
	size_t result = 1;

	result = 31 * result + getDigest(entry.columnName);
	result = 31 * result +
			static_cast<size_t>(toNonNullable(entry.elementType));
	result = 31 * result + entry.offset;
	result = 31 * result + entry.arraySizeOffset;
	result = 31 * result + filterTypeOptions(entry, true, true);

	return result;
}

size_t RowMapper::getDigest(const GSChar *str) {
	if (str == NULL) {
		return 0;
	}

	size_t result = 1;
	for (const GSChar *ch = str; *ch != '\0'; ch++) {
		result = 31 * result + *ch;
	}

	return result;
}

GSBinding RowMapper::checkAndCopyBinding(
		const GSBinding *src, ColumnIdMap &columnIdMap,
		RowTypeCategory rowTypeCategory, const Config &config) {
	columnIdMap.clear();

	size_t entryCount = 0;
	if (rowTypeCategory == CATEGORY_AGGREGATION_RESULT) {
		if (src != NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_ROW_MAPPING, "");
		}
	}
	else if (src == NULL || src->entryCount <= 0 || src->entries == NULL) {
		if (src == NULL || src->entryCount != 0 || !config.anyTypeAllowed_) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_BINDING_ENTRY_NOT_FOUND, "");
		}
	}
	else {
		entryCount = src->entryCount;
	}

	if (rowTypeCategory == CATEGORY_TIME_SERIES && entryCount > 0 &&
			(src->entries[0].elementType != GS_TYPE_TIMESTAMP ||
					isArrayColumn(src->entries[0]))) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_KEY_TYPE, "");
	}

	GSBinding dest;
	dest.entries = (entryCount == 0 ? NULL : new GSBindingEntry[entryCount]);
	dest.entryCount = entryCount;

	try {
		int32_t keyColumnId = -1;
		for (size_t i = 0; i < entryCount; i++) {
			const GSChar *columnName;
			GSBindingEntry &entry = dest.entries[i];
			{
				const GSBindingEntry &srcEntry = src->entries[i];

				columnName = srcEntry.columnName;

				entry.elementType = toNonNullable(srcEntry.elementType);
				entry.offset = srcEntry.offset;
				entry.arraySizeOffset = srcEntry.arraySizeOffset;
				entry.options = filterTypeOptions(
						srcEntry,
						config.anyTypeAllowed_, config.nullableAllowed_);
			}

			bool availableAsKey = false;

			switch (entry.elementType) {
			case GS_TYPE_STRING:
			case GS_TYPE_INTEGER:
			case GS_TYPE_LONG:
			case GS_TYPE_TIMESTAMP:
				availableAsKey = !isArrayColumn(entry) && !isNullable(entry);
				break;
			case GS_TYPE_BOOL:
			case GS_TYPE_BYTE:
			case GS_TYPE_SHORT:
			case GS_TYPE_FLOAT:
			case GS_TYPE_DOUBLE:
				break;
			case GS_TYPE_GEOMETRY:
			case GS_TYPE_BLOB:
				if (isArrayColumn(entry)) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
				}
				break;
			case ANY_NULL_TYPE:
				if (isArrayColumn(entry) || !config.anyTypeAllowed_) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
				}
				break;
			default:
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
			}

			const ColumnIdEntry idEntry = {
				(columnName == NULL ? "" : columnName), static_cast<int32_t>(i)
			};

			if ((entry.options & GS_TYPE_OPTION_KEY) != 0) {
				if (i != 0 && RESTRICT_KEY_ORDER_FIRST) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
				}
				if (keyColumnId >= 0) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MULTIPLE_KEYS_FOUND, "");
				}
				if (!availableAsKey) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_KEY_TYPE, "");
				}
				keyColumnId = idEntry.id_;
			}

			if (columnName != NULL || !config.anyTypeAllowed_) {
				const std::string nameStr =
						ClientUtil::normalizeSymbol(columnName, "column name");
				if (!columnIdMap.insert(ColumnIdMap::value_type(
						nameStr, idEntry)).second) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_COLUMN_NAME_CONFLICTED, "");
				}
			}
		}

		for (size_t i = 0; i < entryCount; i++) {
			const GSBindingEntry &srcEntry = src->entries[i];
			GSBindingEntry &destEntry = dest.entries[i];
			if (srcEntry.columnName == NULL) {
				destEntry.columnName = NULL;
			}
			else {
				const std::string &nameStr =
						ClientUtil::normalizeSymbolUnchecked(
								srcEntry.columnName);
				destEntry.columnName =
						columnIdMap.find(nameStr)->second.name_.c_str();
			}
		}
	}
	catch (...) {
		delete[] dest.entries;
		columnIdMap.clear();
		throw;
	}
	return dest;
}

GSBinding RowMapper::createReorderedBinding(
		const RowMapper &baseMapper, ColumnIdMap &columnIdMap,
		ArrayByteInStream &in, const Config &config,
		bool columnOrderIgnorable) {
	columnIdMap.clear();

	if (baseMapper.rowTypeCategory_ == CATEGORY_AGGREGATION_RESULT) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_ROW_MAPPING, "");
	}

	const GSBinding &srcBinding = baseMapper.binding_;

	const size_t entryCount = importColumnCount(in);
	if (entryCount != srcBinding.entryCount) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
	}

	util::NormalXArray<int32_t> newColumnIdList;
	newColumnIdList.assign(srcBinding.entryCount, -1);

	GSBinding dest;
	dest.entries = new GSBindingEntry[srcBinding.entryCount];
	dest.entryCount = srcBinding.entryCount;

	try {
		int32_t keyColumnId = importKeyListBegin(in, config, entryCount);

		for (size_t i = 0; i < srcBinding.entryCount; i++) {
			ColumnIdEntry idEntry = {
				std::string(), static_cast<int32_t>(i)
			};

			GSBindingEntry entry;
			importColumnSchema(in, config, entry, &idEntry.name_, NULL);

			const std::string &columnName = idEntry.name_;
			const std::string &normalizedName =
					ClientUtil::normalizeSymbolUnchecked(columnName.c_str());

			ColumnIdMap::const_iterator it =
					baseMapper.columnIdMap_.find(normalizedName);
			if (it == baseMapper.columnIdMap_.end()) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
			}
			const int32_t orgColumnId = it->second.id_;
			if (newColumnIdList[orgColumnId] != -1) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
			}
			if (!columnOrderIgnorable && orgColumnId != idEntry.id_) {
				GS_CLIENT_THROW_ERROR(
						GS_ERROR_CC_ILLEGAL_SCHEMA, "Inconsistent order");
			}

			newColumnIdList[orgColumnId] = idEntry.id_;
			columnIdMap.insert(
					ColumnIdMap::value_type(normalizedName, idEntry));

			if (srcBinding.entries[orgColumnId].elementType !=
					entry.elementType) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
			}
			if (isArrayColumn(srcBinding.entries[orgColumnId]) ^
					isArrayColumn(entry)) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
			}
			if (isNullable(srcBinding.entries[orgColumnId]) ^
					isNullable(entry)) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
			}
		}

		importKeyListEnd(in, config, entryCount, keyColumnId);

		if (baseMapper.rowTypeCategory_ == CATEGORY_TIME_SERIES &&
				newColumnIdList[0] != 0) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
		}

		for (size_t i = 0; i < srcBinding.entryCount; i++) {
			const int32_t newIndex = newColumnIdList[i];
			assert(newIndex >= 0 &&
					static_cast<size_t>(newIndex) < newColumnIdList.size());

			const GSBindingEntry &srcEntry = srcBinding.entries[i];
			const std::string &normalizedName =
					ClientUtil::normalizeSymbolUnchecked(srcEntry.columnName);

			GSBindingEntry &destEntry = dest.entries[newIndex];
			destEntry.columnName =
					columnIdMap.find(normalizedName)->second.name_.c_str();
			destEntry.elementType = srcEntry.elementType;
			destEntry.offset = srcEntry.offset;
			destEntry.arraySizeOffset = srcEntry.arraySizeOffset;
			destEntry.options = srcEntry.options;
		}

		if ((keyColumnId >= 0 && baseMapper.keyColumnId_ < 0) ||
				(baseMapper.keyColumnId_ >= 0 &&
				keyColumnId != newColumnIdList[baseMapper.keyColumnId_])) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
		}
	}
	catch (...) {
		delete[] dest.entries;
		columnIdMap.clear();
		throw;
	}

	return dest;
}

template<GSType T, typename M>
const GSChar* RowMapper::maskNullString(const GSChar *src) {
	return src;
}

template<>
const GSChar* RowMapper::maskNullString<GS_TYPE_STRING, util::TrueType>(
		const GSChar *src) {
	return (src == NULL ? "" : src);
}

template<>
const GSChar* RowMapper::maskNullString<GS_TYPE_GEOMETRY, util::TrueType>(
		const GSChar *src) {
	return (src == NULL ? "POINT(EMPTY)" : src);
}

int32_t RowMapper::findRowKeyId(const GSBinding *binding) {
	if (binding != NULL) {
		for (size_t i = 0; i < binding->entryCount; i++) {
			if (binding->entries[i].options & GS_TYPE_OPTION_KEY) {
				return static_cast<int32_t>(i);
			}
		}
	}
	return -1;
}

void* RowMapper::getField(const GSBindingEntry &entry, void *rowObj) {
	return static_cast<uint8_t*>(rowObj) + entry.offset;
}

const void* RowMapper::getField(
		const GSBindingEntry &entry, const void *rowObj) {
	return static_cast<const uint8_t*>(rowObj) + entry.offset;
}

void* RowMapper::getArrayFieldSizeAddr(
		const GSBindingEntry &entry, void *rowObj) {
	return static_cast<uint8_t*>(rowObj) + entry.arraySizeOffset;
}

size_t RowMapper::getArrayFieldSize(
		const GSBindingEntry &entry, const void *rowObj) {
	return *reinterpret_cast<const size_t*>(
			static_cast<const uint8_t*>(rowObj) + entry.arraySizeOffset);
}

const RowMapper& RowMapper::resolveCodingMapper(const void *rowObj) const {
	if (!general_) {
		return *this;
	}

	const RowMapper &mapper = GSRow::resolve(rowObj).getRowMapper();

	if (&mapper == this) {
		return *this;
	}

	if (binding_.entryCount != mapper.binding_.entryCount) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
	}

	if (hasKey() ^ mapper.hasKey()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
	}

	for (size_t i = 0; i < binding_.entryCount; i++) {
		const GSBindingEntry &entry = binding_.entries[i];
		const GSBindingEntry &anotherEntry = mapper.binding_.entries[i];

		if (entry.elementType != anotherEntry.elementType) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
		}

		if (isArrayColumn(entry) ^ isArrayColumn(anotherEntry)) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
		}
	}

	return mapper;
}

const GSBindingEntry& RowMapper::resolveKeyEntry() const {
	if (keyColumnId_ < 0) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_KEY_NOT_FOUND, "");
	}

	return binding_.entries[keyColumnId_];
}

const GSBindingEntry& RowMapper::getEntry(int32_t columnId) const {
	if (columnId < 0 || static_cast<size_t>(columnId) >= binding_.entryCount) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	return binding_.entries[columnId];
}

bool RowMapper::isNull(const void *rowObj, int32_t columnId) const {
	if (!general_) {
		return false;
	}

	return static_cast<const GSRow*>(rowObj)->isNull(columnId);
}

void RowMapper::setNull(void *rowObj, int32_t columnId, bool nullValue) const {
	if (!general_) {
		return;
	}

	static_cast<GSRow*>(rowObj)->setNull(columnId, nullValue);
}

uint8_t* RowMapper::findNulls(void *rowObj) const {
	if (!general_) {
		return NULL;
	}

	return static_cast<GSRow*>(rowObj)->getNulls();
}

const uint8_t* RowMapper::findNulls(const void *rowObj) const {
	if (!general_) {
		return NULL;
	}

	return static_cast<const GSRow*>(rowObj)->getNulls();
}

size_t RowMapper::getFixedRowPartSize(bool rowIdIncluded, MappingMode mode) const {
	size_t size = (rowIdIncluded ? sizeof(int64_t) : 0);
	for (size_t i = 0; i < binding_.entryCount; i++) {
		size += getFixedFieldPartSize(static_cast<int32_t>(i), mode);
	}
	if (mode == MODE_ROWWISE_SEPARATED_V2) {
		assert(nullsByteSize_ > 0);
		size += nullsByteSize_;
		size += nullsOffset_;
	}
	return size;
}

size_t RowMapper::getFixedFieldPartSize(int32_t columnId, MappingMode mode) const {
	assert(columnId >= 0 &&
			static_cast<size_t>(columnId) < binding_.entryCount);
	const GSBindingEntry &entry = binding_.entries[columnId];

	size_t varDataHeadSize = sizeof(int64_t);
	if (mode == MODE_ROWWISE_SEPARATED_V2) {
		varDataHeadSize = 0;
	}
	if (isArrayColumn(entry)) {
		return varDataHeadSize;
	}
	else {
		UTIL_STATIC_ASSERT(sizeof(float) == sizeof(int32_t));
		UTIL_STATIC_ASSERT(sizeof(double) == sizeof(int64_t));
		switch (toNonNullable(entry.elementType)) {
		case GS_TYPE_STRING:
			return varDataHeadSize;
		case GS_TYPE_BOOL:
			return sizeof(int8_t);
		case GS_TYPE_BYTE:
			return sizeof(int8_t);
		case GS_TYPE_SHORT:
			return sizeof(int16_t);
		case GS_TYPE_INTEGER:
			return sizeof(int32_t);
		case GS_TYPE_LONG:
			return sizeof(int64_t);
		case GS_TYPE_FLOAT:
			return sizeof(float);
		case GS_TYPE_DOUBLE:
			return sizeof(double);
		case GS_TYPE_TIMESTAMP:
			return sizeof(GSTimestamp);
		case GS_TYPE_GEOMETRY:
			return varDataHeadSize;
		case GS_TYPE_BLOB:
			return varDataHeadSize;
		case ANY_NULL_TYPE:
			return (sizeof(int8_t) + sizeof(int64_t));
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
		}
	}
}

void RowMapper::decodeAggregation(
		InputCursor &cursor, void *rowObj) const {
	if (cursor.base_.mode_ != MODE_AGGREGATED || cursor.base_.rowIdIncluded_) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_ROW_MAPPING, "");
	}

	ArrayByteInStream &in = cursor.in_;

	int32_t columnId = -1;
	if (ACCEPT_AGGREGATION_RESULT_COLUMN_ID) {
		in >> columnId;
	}

	int8_t type;
	in >> type;

	size_t size;
	switch (type) {
	case GS_TYPE_BYTE:
		size = sizeof(int8_t);
		break;
	case GS_TYPE_SHORT:
		size = sizeof(int16_t);
		break;
	case GS_TYPE_INTEGER:
		size = sizeof(int32_t);
		break;
	case GS_TYPE_LONG:
		size = sizeof(int64_t);
		break;
	case GS_TYPE_FLOAT:
		size = sizeof(float);
		break;
	case GS_TYPE_DOUBLE:
		size = sizeof(double);
		break;
	case GS_TYPE_TIMESTAMP:
		size = sizeof(GSTimestamp);
		break;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
	}

	union {
		int8_t asInt8;
		int16_t asInt16;
		int32_t asInt32;
		int64_t asInt64;
		float asFloat;
		double asDouble;
		GSTimestamp asTimestamp;
	} orgValue;
	assert(size <= sizeof(orgValue));
	in.readAll(&orgValue, size);

	if (rowTypeCategory_ == CATEGORY_AGGREGATION_RESULT) {
		GSAggregationResult &result =
				*static_cast<GSAggregationResult*>(rowObj);

		switch (type) {
		case GS_TYPE_BYTE:
			result.resultType_ = GS_TYPE_LONG;
			result.value_.longOrTimestampValue_ = orgValue.asInt8;
			break;
		case GS_TYPE_SHORT:
			result.resultType_ = GS_TYPE_LONG;
			result.value_.longOrTimestampValue_ = orgValue.asInt16;
			break;
		case GS_TYPE_INTEGER:
			result.resultType_ = GS_TYPE_LONG;
			result.value_.longOrTimestampValue_ = orgValue.asInt32;
			break;
		case GS_TYPE_LONG:
			result.resultType_ = GS_TYPE_LONG;
			result.value_.longOrTimestampValue_ = orgValue.asInt64;
			break;
		case GS_TYPE_FLOAT:
			result.resultType_ = GS_TYPE_DOUBLE;
			result.value_.doubleValue_ = orgValue.asFloat;
			break;
		case GS_TYPE_DOUBLE:
			result.resultType_ = GS_TYPE_DOUBLE;
			result.value_.doubleValue_ = orgValue.asDouble;
			break;
		case GS_TYPE_TIMESTAMP:
			result.resultType_ = GS_TYPE_TIMESTAMP;
			result.value_.longOrTimestampValue_ = orgValue.asTimestamp;
			break;
		default:
			assert(false);
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
		}
	}
	else {
		if (!ACCEPT_AGGREGATION_RESULT_COLUMN_ID || columnId < 0 ||
				columnId >= static_cast<int32_t>(binding_.entryCount)) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_ROW_MAPPING, "");
		}

		GSBindingEntry &entry = binding_.entries[columnId];
		assert(size <= sizeof(orgValue));

		if (type == entry.elementType) {
			memcpy(getField(entry, rowObj), &orgValue, size);
		}
		else {
			switch (type) {
			case GS_TYPE_LONG:
			case GS_TYPE_DOUBLE:
				break;
			default:
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_ROW_MAPPING, "");
			}

			void *fieldObj = getField(entry, rowObj);
			const int64_t *longValue = (type == GS_TYPE_LONG ?
					reinterpret_cast<int64_t*>(&orgValue) : NULL);
			const double *doubleValue = (type == GS_TYPE_DOUBLE ?
					reinterpret_cast<double*>(&orgValue) : NULL);

			switch (entry.elementType) {
			case GS_TYPE_BYTE:
				*static_cast<int8_t*>(fieldObj) = (longValue == NULL ?
						static_cast<int8_t>(*doubleValue) :
						static_cast<int8_t>(*longValue));
				break;
			case GS_TYPE_SHORT:
				*static_cast<int16_t*>(fieldObj) = (longValue == NULL ?
						static_cast<int16_t>(*doubleValue) :
						static_cast<int16_t>(*longValue));
				break;
			case GS_TYPE_INTEGER:
				*static_cast<int32_t*>(fieldObj) = (longValue == NULL ?
						static_cast<int32_t>(*doubleValue) :
						static_cast<int32_t>(*longValue));
				break;
			case GS_TYPE_LONG:
				*static_cast<int64_t*>(fieldObj) = (longValue == NULL ?
						static_cast<int64_t>(*doubleValue) : *longValue);
				break;
			case GS_TYPE_FLOAT:
				*static_cast<float*>(fieldObj) = (longValue == NULL ?
						static_cast<float>(*doubleValue) :
						static_cast<float>(*longValue));
				break;
			case GS_TYPE_DOUBLE:
				*static_cast<double*>(fieldObj) = (longValue == NULL ?
						*doubleValue : static_cast<double>(*longValue));
				break;
			case GS_TYPE_TIMESTAMP:
				*static_cast<GSTimestamp*>(fieldObj) = (longValue == NULL ?
						static_cast<GSTimestamp>(*doubleValue) :
						static_cast<GSTimestamp>(*longValue));
				break;
			default:
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_ROW_MAPPING, "");
			}
		}
	}
}

void RowMapper::putArraySizeInfo(
		XArrayByteOutStream &out, MappingMode mode,
		size_t elementSize, size_t elementCount) {
	if (MODE_ROWWISE_SEPARATED_V2 == mode) {
		ClientUtil::writeVarSize(out,
				elementSize * elementCount +
				ClientUtil::getEncodedVarSize(elementCount));
		ClientUtil::writeVarSize(out, elementCount);
	}
	else {
		out << static_cast<int32_t>(0);		
		out << ClientUtil::sizeValueToInt32(elementCount);
	}
}

void RowMapper::encodeField(OutputCursor &cursor, int32_t columnId,
		const GSType *keyType, const void *keyObj,
		const void *rowObj, const GSBindingEntry *generalEntry) const {
	assert(cursor.base_.mapper_ == this);
	assert(columnId >= 0 &&
			static_cast<size_t>(columnId) < binding_.entryCount);
	const GSBindingEntry &entry = (generalEntry == NULL) ?
			binding_.entries[columnId] : *generalEntry;

	cursor.beginField();
	XArrayByteOutStream &out = cursor.out_;
	const MappingMode mode = cursor.base_.mode_;

	if (isArrayColumn(entry)) {
		if (columnId == keyColumnId_ && keyObj != NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
		}

		cursor.beginVarData();
		const size_t topPos = out.base().position();
		const size_t elementCount = getArrayFieldSize(entry, rowObj);

		const void *const arrayValue =
				*static_cast<const void* const*>(getField(entry, rowObj));
		if (arrayValue == NULL && elementCount > 0) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_ROW_FIELD, "");
		}
		switch (toNonNullable(entry.elementType)) {
		case GS_TYPE_STRING:
			if (MODE_ROWWISE_SEPARATED_V2 == mode) {
				size_t totalSize = ClientUtil::getEncodedVarSize(elementCount);
				for (size_t i = 0; i < elementCount; i++) {
					const GSChar* const elem =
							static_cast<const GSChar* const*>(arrayValue)[i];
					if (arrayValue == NULL) {
						GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_ROW_FIELD, "");
					}
					size_t length = strlen(elem);
					totalSize += ClientUtil::getEncodedVarSize(length) + length;
				}
				ClientUtil::writeVarSize(out, totalSize);
				ClientUtil::writeVarSize(out, elementCount);
				for (size_t i = 0; i < elementCount; i++) {
					const GSChar* const elem =
							static_cast<const GSChar* const*>(arrayValue)[i];
					ClientUtil::writeVarDataString(out, elem);
				}
			}
			else {
				out << static_cast<int32_t>(0);		
				out << ClientUtil::sizeValueToInt32(elementCount);
				for (size_t i = 0; i < elementCount; i++) {
					const GSChar* const elem =
							static_cast<const GSChar* const*>(arrayValue)[i];
					if (arrayValue == NULL) {
						GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_ROW_FIELD, "");
					}
					out << elem;
				}
			}
			break;
		case GS_TYPE_BOOL:
			putArraySizeInfo(out, mode, sizeof(GSBool), elementCount);
			out.writeAll(arrayValue, sizeof(GSBool) * elementCount);
			break;
		case GS_TYPE_BYTE:
			putArraySizeInfo(out, mode, sizeof(int8_t), elementCount);
			out.writeAll(arrayValue, sizeof(int8_t) * elementCount);
			break;
		case GS_TYPE_SHORT:
			putArraySizeInfo(out, mode, sizeof(int16_t), elementCount);
			out.writeAll(arrayValue, sizeof(int16_t) * elementCount);
			break;
		case GS_TYPE_INTEGER:
			putArraySizeInfo(out, mode, sizeof(int32_t), elementCount);
			out.writeAll(arrayValue, sizeof(int32_t) * elementCount);
			break;
		case GS_TYPE_LONG:
			putArraySizeInfo(out, mode, sizeof(int64_t), elementCount);
			out.writeAll(arrayValue, sizeof(int64_t) * elementCount);
			break;
		case GS_TYPE_FLOAT:
			putArraySizeInfo(out, mode, sizeof(float), elementCount);
			out.writeAll(arrayValue, sizeof(float) * elementCount);
			break;
		case GS_TYPE_DOUBLE:
			putArraySizeInfo(out, mode, sizeof(double), elementCount);
			out.writeAll(arrayValue, sizeof(double) * elementCount);
			break;
		case GS_TYPE_TIMESTAMP:
			putArraySizeInfo(out, mode, sizeof(GSTimestamp), elementCount);
			out.writeAll(arrayValue, sizeof(GSTimestamp) * elementCount);
			break;
		case GS_TYPE_GEOMETRY:
		case GS_TYPE_BLOB:
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
		}
		if (MODE_ROWWISE_SEPARATED_V2 != mode) {
			const size_t endPos = out.base().position();
			out.base().position(topPos);
			out << ClientUtil::sizeValueToInt32(endPos - (topPos + sizeof(int32_t)));
			out.base().position(endPos);
		}
		cursor.endVarData();
	}
	else {
		const void *fieldValue;
		if (keyObj != NULL && columnId == keyColumnId_) {
			if (keyType != NULL && *keyType != entry.elementType) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_KEY_TYPE, "");
			}
			fieldValue = keyObj;
		}
		else {
			fieldValue = getField(entry, rowObj);
		}

		switch (toNonNullable(entry.elementType)) {
		case GS_TYPE_STRING: {
			const GSChar *strValue =
					*static_cast<const GSChar* const*>(fieldValue);
			if (strValue == NULL) {
				if (general_) {
					strValue = maskNullString<
							GS_TYPE_STRING, util::TrueType>(NULL);
				}
				else {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_ROW_FIELD, "");
				}
			}
			cursor.beginVarData();
			if (MODE_ROWWISE_SEPARATED_V2 == mode) {
				ClientUtil::writeVarDataString(out, strValue);
			} else {
				out << strValue;
			}
			cursor.endVarData();
			break;
		}
		case GS_TYPE_BOOL:
			out.writeAll(fieldValue, sizeof(GSBool));
			break;
		case GS_TYPE_BYTE:
			out.writeAll(fieldValue, sizeof(int8_t));
			break;
		case GS_TYPE_SHORT:
			out.writeAll(fieldValue, sizeof(int16_t));
			break;
		case GS_TYPE_INTEGER:
			out.writeAll(fieldValue, sizeof(int32_t));
			break;
		case GS_TYPE_LONG:
			out.writeAll(fieldValue, sizeof(int64_t));
			break;
		case GS_TYPE_FLOAT:
			out.writeAll(fieldValue, sizeof(float));
			break;
		case GS_TYPE_DOUBLE:
			out.writeAll(fieldValue, sizeof(double));
			break;
		case GS_TYPE_TIMESTAMP:
			out.writeAll(fieldValue, sizeof(GSTimestamp));
			break;
		case GS_TYPE_GEOMETRY: {
			const GSChar *strValue =
					*static_cast<const GSChar* const*>(fieldValue);
			if (strValue == NULL) {
				if (general_) {
					strValue = maskNullString<
							GS_TYPE_GEOMETRY, util::TrueType>(NULL);
				}
				else {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_ROW_FIELD, "");
				}
			}
			cursor.beginVarData();
			encodeGeometry(out, strValue, cursor.base_.mode_);
			cursor.endVarData();
			break;
		}
		case GS_TYPE_BLOB: {
			cursor.beginVarData();
			const GSBlob &blob = *static_cast<const GSBlob*>(fieldValue);
			if (MODE_ROWWISE_SEPARATED_V2 == mode) {
				ClientUtil::writeVarSize(out, blob.size);
			} else {
				out << ClientUtil::sizeValueToInt32(blob.size);
			}
			if (blob.size > 0) {
				if (blob.data == NULL) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_ROW_FIELD, "");
				}
				out.writeAll(blob.data, blob.size);
			}
			cursor.endVarData();
			break;
		}
		case ANY_NULL_TYPE: {
			const GSType actualType = *static_cast<const int8_t*>(fieldValue);
			out << static_cast<int8_t>(actualType);
			if (actualType == ANY_NULL_TYPE) {
				out << static_cast<int64_t>(0);
			}
			else {
				const GSBindingEntry &subEntry =
						getEntryGeneral(entry, actualType);

				const size_t lastPos = out.base().position();
				encodeField(
						cursor, columnId, keyType, keyObj, rowObj, &subEntry);

				const size_t fixedGap =
						lastPos + sizeof(int64_t) - out.base().position();
				for (size_t r = fixedGap; r > 0; r--) {
					const int8_t nullByte = 0;
					out << nullByte;
				}
			}
			break;
		}
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
		}
	}
}

void RowMapper::decodeField(
		InputCursor &cursor, int32_t columnId, void *rowObj,
		void *&pendingVarData, size_t &pendingPtrArraySize,
		const GSBindingEntry *generalEntry) const {
	assert(cursor.base_.mapper_ == this);
	assert(columnId >= 0 &&
		static_cast<size_t>(columnId) < binding_.entryCount);
	const GSBindingEntry &entry = (generalEntry == NULL) ?
			binding_.entries[columnId] : *generalEntry;

	const MappingMode mode = cursor.base_.mode_;

	cursor.beginField();
	ArrayByteInStream &in = cursor.in_;
	VarDataPool *pool = cursor.varDataPool_;

	if (isArrayColumn(entry)) {
		if (pool == NULL) {
			assert(false);
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
		}

		clearFieldGeneral(entry, rowObj);
		cursor.beginVarData();

		int32_t arrayByteSize;
		int32_t elementCountInt32;
		if (MODE_ROWWISE_SEPARATED_V2 == mode) {
			arrayByteSize = ClientUtil::readVarSize(in);
			elementCountInt32 = ClientUtil::readVarSize(in);
		} else {
			in >> arrayByteSize;
			in >> elementCountInt32;
		}
		const size_t elementCount = ClientUtil::toSizeValue(elementCountInt32);

		void *&arrayValue = pendingVarData;

		if (elementCount > 0) {
			switch (toNonNullable(entry.elementType)) {
			case GS_TYPE_STRING:
				arrayValue = allocate(
						pool, rowObj, sizeof(GSChar*) * elementCount);
				for (size_t i = 0; i < elementCount; i++) {
					if (MODE_ROWWISE_SEPARATED_V2 == mode) {
						static_cast<const GSChar**>(arrayValue)[i] =
								decodeStringV2(in, pool, rowObj);
					}
					else {
						static_cast<const GSChar**>(arrayValue)[i] =
								decodeString(in, pool, rowObj);
					}
					pendingPtrArraySize++;
				}
				break;
			case GS_TYPE_BOOL:
				arrayValue =
						allocate(pool, rowObj, sizeof(GSBool) * elementCount);
				in.readAll(arrayValue, sizeof(GSBool) * elementCount);
				break;
			case GS_TYPE_BYTE:
				arrayValue =
						allocate(pool, rowObj, sizeof(int8_t) * elementCount);
				in.readAll(arrayValue, sizeof(int8_t) * elementCount);
				break;
			case GS_TYPE_SHORT:
				arrayValue =
						allocate(pool, rowObj, sizeof(int16_t) * elementCount);
				in.readAll(arrayValue, sizeof(int16_t) * elementCount);
				break;
			case GS_TYPE_INTEGER:
				arrayValue =
						allocate(pool, rowObj, sizeof(int32_t) * elementCount);
				in.readAll(arrayValue, sizeof(int32_t) * elementCount);
				break;
			case GS_TYPE_LONG:
				arrayValue =
						allocate(pool, rowObj, sizeof(int64_t) * elementCount);
				in.readAll(arrayValue, sizeof(int64_t) * elementCount);
				break;
			case GS_TYPE_FLOAT:
				arrayValue =
						allocate(pool, rowObj, sizeof(float) * elementCount);
				in.readAll(arrayValue, sizeof(float) * elementCount);
				break;
			case GS_TYPE_DOUBLE:
				arrayValue =
						allocate(pool, rowObj, sizeof(double) * elementCount);
				in.readAll(arrayValue, sizeof(double) * elementCount);
				break;
			case GS_TYPE_TIMESTAMP:
				arrayValue =
						allocate(pool, rowObj, sizeof(GSTimestamp) * elementCount);
				in.readAll(arrayValue, sizeof(GSTimestamp) * elementCount);
				break;
			case GS_TYPE_GEOMETRY:
			case GS_TYPE_BLOB:
			default:
				assert(false);
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
			}
		}
		else {
			arrayValue = NULL;
		}
		cursor.endVarData();

		ClientUtil::setNonAlignedValue(
			getArrayFieldSizeAddr(entry, rowObj), elementCount);
		ClientUtil::setNonAlignedValue(getField(entry, rowObj), arrayValue);

		pendingVarData = NULL;
		pendingPtrArraySize = 0;
	}
	else {
		void *fieldValue = getField(entry, rowObj);
		switch (toNonNullable(entry.elementType)) {
		case GS_TYPE_STRING: {
			if (pool == NULL) {
				assert(false);
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
			}
			clearField<GS_TYPE_STRING, false>(entry, rowObj);
			cursor.beginVarData();
			if (MODE_ROWWISE_SEPARATED_V2 == mode) {
				ClientUtil::setNonAlignedValue(
					fieldValue, decodeStringV2(in, pool, rowObj));
			} else {
				ClientUtil::setNonAlignedValue(
					fieldValue, decodeString(in, pool, rowObj));
			}
			cursor.endVarData();
			break;
		}
		case GS_TYPE_BOOL:
			in.readAll(fieldValue, sizeof(GSBool));
			break;
		case GS_TYPE_BYTE:
			in.readAll(fieldValue, sizeof(int8_t));
			break;
		case GS_TYPE_SHORT:
			in.readAll(fieldValue, sizeof(int16_t));
			break;
		case GS_TYPE_INTEGER:
			in.readAll(fieldValue, sizeof(int32_t));
			break;
		case GS_TYPE_LONG:
			in.readAll(fieldValue, sizeof(int64_t));
			break;
		case GS_TYPE_FLOAT:
			in.readAll(fieldValue, sizeof(float));
			break;
		case GS_TYPE_DOUBLE:
			in.readAll(fieldValue, sizeof(double));
			break;
		case GS_TYPE_TIMESTAMP:
			in.readAll(fieldValue, sizeof(GSTimestamp));
			break;
		case GS_TYPE_GEOMETRY: {
			if (pool == NULL) {
				assert(false);
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
			}

			clearField<GS_TYPE_GEOMETRY, false>(entry, rowObj);
			cursor.beginVarData();

			size_t dataSize;
			if (MODE_ROWWISE_SEPARATED_V2 == mode) {
				dataSize = ClientUtil::readVarSize(in);
			} else {
				int32_t dataSizeInt32;
				in >> dataSizeInt32;
				dataSize = ClientUtil::toSizeValue(dataSizeInt32);
			}

			GS_CLIENT_THROW_ERROR(
					GS_ERROR_CC_UNSUPPORTED_OPERATION,
					"GEOMETRY is not a supported type");

			cursor.endVarData();
			break;
		}
		case GS_TYPE_BLOB: {
			if (pool == NULL) {
				assert(false);
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
			}

			clearField<GS_TYPE_BLOB, false>(entry, rowObj);
			cursor.beginVarData();

			GSBlob blob;
			if (MODE_ROWWISE_SEPARATED_V2 == mode) {
				blob.size = ClientUtil::readVarSize(in);
			} else {
				int32_t blobSizeInt32;
				in >> blobSizeInt32;
				blob.size = ClientUtil::toSizeValue(blobSizeInt32);
			}

			pendingVarData = (blob.size == 0 ?
					NULL : allocate(pool, rowObj, blob.size));
			in.readAll(pendingVarData, blob.size);
			blob.data = pendingVarData;
			ClientUtil::setNonAlignedValue(fieldValue, blob);
			pendingVarData = NULL;

			cursor.endVarData();
			break;
		}
		case ANY_NULL_TYPE: {
			clearFieldGeneral(entry, rowObj);

			int8_t type;
			in >> type;

			GSType actualType = static_cast<GSType>(type);
			if (actualType == ANY_NULL_TYPE) {
				int64_t dummy;
				in >> dummy;
			}
			else {
				const GSBindingEntry &subEntry =
						getEntryGeneral(entry, actualType);

				const size_t lastPos = in.base().position();
				decodeField(cursor, columnId, rowObj,
						pendingVarData, pendingPtrArraySize, &subEntry);

				const size_t fixedGap =
						lastPos + sizeof(int64_t) - in.base().position();
				if (fixedGap > sizeof(int64_t)) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
				}
				for (size_t r = fixedGap; r > 0; r--) {
					int8_t nullByte;
					in >> nullByte;
				}
			}

			*static_cast<int8_t*>(fieldValue) = static_cast<int8_t>(type);
			break;
		}
		default:
			assert(false);
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
		}
	}
}

template<GSType ElemType, bool ArrayType>
void RowMapper::clearField(const GSBindingEntry &entry, void *rowObj) const {
	if (!general_) {
		return;
	}

	GSRow::FieldClearer clearer;
	typedef TypeTraits<ElemType, ArrayType> Traits;
	clearer(*static_cast<GSRow*>(rowObj), entry, Traits());
}

void RowMapper::clearFieldGeneral(
		const GSBindingEntry &entry, void *rowObj) const {
	if (!general_) {
		return;
	}

	GSRow::FieldClearer clearer;
	invokeTypedOperation(*static_cast<GSRow*>(rowObj), clearer, entry);
}

void* RowMapper::allocate(VarDataPool *pool, void *rowObj, size_t size) const {
	if (general_) {
		assert(rowObj != NULL);
		return static_cast<GSRow*>(rowObj)->allocate(size);
	}
	else {
		assert(pool != NULL);
		return pool->allocate(size);
	}
}

void RowMapper::deallocate(VarDataPool *pool, void *rowObj, void *ptr) const {
	if (general_) {
		assert(rowObj != NULL);
		return static_cast<GSRow*>(rowObj)->deallocate(ptr);
	}
	else {
		assert(pool != NULL);
		return pool->deallocate(ptr);
	}
}

template<typename Alloc, typename Mask>
RowMapper::ValueCopier<Alloc, Mask>::ValueCopier(
		Alloc *alloc, const GSValue &src) :
		alloc_(alloc),
		src_(src) {
}

template<typename Alloc, typename Mask>
template<typename Traits>
void RowMapper::ValueCopier<Alloc, Mask>::operator()(
		GSValue &dest, const GSBindingEntry &entry, const Traits&) {
	static_cast<void>(entry);
	dest = copyValue(src_, Traits(), alloc_, Mask(), Traits::as(src_));
}

template<typename Traits, typename Alloc, typename Mask>
struct RowMapper::StringOrArrayCopier<Traits, Alloc, Mask, true> {
	GSValue operator()(const GSValue &src, Alloc *alloc) {
		GSValue dest;

		const GSChar *srcStr =
				maskNullString<Traits::ELEMENT_TYPE, Mask>(Traits::as(src));
		if (srcStr == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}
		const size_t size = strlen(srcStr) + 1;

		GSChar *destStr = static_cast<GSChar*>(alloc->allocate(size));
		memcpy(destStr, srcStr, size);
		Traits::as(dest) = destStr;

		return dest;
	}
};

template<typename Traits, typename Alloc, typename Mask>
struct RowMapper::StringOrArrayCopier<Traits, Alloc, Mask, false> {
	GSValue operator()(const GSValue &src, Alloc *alloc) {
		GSValue dest;

		const size_t size = Traits::arraySizeOf(src);
		Traits::arraySizeOf(dest) = size;

		if (size > 0) {
			const void *srcArray = Traits::as(src);
			if (srcArray == NULL) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
			}

			const size_t dataSize = sizeof(typename
					TypeTraits<Traits::ELEMENT_TYPE, false>::Object) * size;
			void *destArray = alloc->allocate(dataSize);
			memcpy(destArray, srcArray, dataSize);
			Traits::as(dest) = static_cast<typename Traits::Object>(destArray);
		}
		else {
			Traits::as(dest) = NULL;
		}

		return dest;
	}
};


size_t RowMapper::Initializer::counter_ = 0;

RowMapper::Initializer::Initializer() throw() {
	if (counter_++ == 0) {
		try {
			cache_ = new Cache();
		}
		catch (...) {
		}
	}
}

RowMapper::Initializer::~Initializer() {
	if (--counter_ == 0) {
		delete cache_;
		cache_ = NULL;
	}
}






RowMapper::BaseCursor::BaseCursor(
		const RowMapper &mapper, MappingMode mode, int32_t rowCount,
		bool rowIdIncluded, size_t topPos) :
		mapper_(&mapper),
		mode_(mode),
		rowCount_(rowCount),
		rowIdIncluded_(rowIdIncluded),
		rowIndex_(-1),
		fieldIndex_(-1),
		topPos_(topPos),
		varDataTop_(getVarDataTop(
				mapper, mode, rowCount, rowIdIncluded, topPos)),
		varDataLast_(varDataTop_),
		partialVarDataOffset_(0),
		varDataSeparated_(varDataTop_ != static_cast<size_t>(-1)),
		pendingPos_(static_cast<size_t>(-1)),
		lastRowID_(-1), varDataOffset_(0) {
}

void RowMapper::BaseCursor::reset() {
	rowIndex_ = -1;
	fieldIndex_ = -1;
	varDataLast_ = varDataTop_;
	pendingPos_ = static_cast<size_t>(-1);
	lastRowID_ = -1;
	varDataOffset_ = 0;
	partialVarDataOffset_ = 0;
}

template<typename StreamBase>
void RowMapper::BaseCursor::beginField(StreamBase &s) {
	if (mode_ == MODE_COLUMNWISE_SEPARATED) {
		fieldIndex_++;

		size_t pos;
		if (fieldIndex_ == 0) {
			pos = topPos_ + mapper_->getFixedFieldPartSize(0, mode_) *
					static_cast<size_t>(rowIndex_);
		}
		else {
			pos = s.position() +
					mapper_->getFixedFieldPartSize(fieldIndex_ - 1, mode_) *
						static_cast<size_t>(rowCount_ - rowIndex_ - 1) +
					mapper_->getFixedFieldPartSize(fieldIndex_, mode_) *
						static_cast<size_t>(rowIndex_);
		}
		s.position(pos);
	}
}

size_t RowMapper::BaseCursor::getVarDataTop(
		const RowMapper &mapper, MappingMode mode, int32_t rowCount,
		bool rowIdIncluded, size_t topPos) {
	switch (mode) {
	case MODE_ROWWISE_SEPARATED:
	case MODE_ROWWISE_SEPARATED_V2:
		return topPos + mapper.getFixedRowPartSize(rowIdIncluded, mode) *
				ClientUtil::sizeValueToInt32(rowCount);
	case MODE_COLUMNWISE_SEPARATED:
		if (rowIdIncluded) {
			assert(false);
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
		}
		return topPos + mapper.getFixedRowPartSize(false, mode) *
				ClientUtil::sizeValueToInt32(rowCount);
	case MODE_NORMAL:
	case MODE_AGGREGATED:
	default:
		return static_cast<size_t>(-1);
	}
}


RowMapper::Config::Config(
		bool anyTypeAllowed, bool nullableAllowed, bool keyExtensible) :
		anyTypeAllowed_(anyTypeAllowed),
		nullableAllowed_(nullableAllowed),
		keyExtensible_(keyExtensible) {
}


RowMapper::InputCursor::InputCursor(
		ArrayByteInStream &in, const RowMapper &mapper,
		MappingMode mode, int32_t rowCount, bool rowIdIncluded,
		VarDataPool *varDataPool) :
		base_(mapper, mode, rowCount, rowIdIncluded, in.base().position()),
		in_(in),
		varDataPool_(varDataPool),
		varDataBaseOffset_(0) {
}

void RowMapper::InputCursor::setVarDataBaseOffset(int64_t varDataBaseOffset) {
	varDataBaseOffset_ = varDataBaseOffset;
}

RowMapper::MappingMode RowMapper::InputCursor::getMode() const {
	return base_.mode_;
}

int32_t RowMapper::InputCursor::getRowCount() const {
	return base_.rowCount_;
}

int64_t RowMapper::InputCursor::getLastRowID() const {
	return base_.lastRowID_;
}

bool RowMapper::InputCursor::isRowIdIncluded() const {
	return base_.rowIdIncluded_;
}

bool RowMapper::InputCursor::hasNext() const {
	return (base_.rowIndex_ + 1 < base_.rowCount_);
}

bool RowMapper::InputCursor::isInRange() const {
	return (0 <= base_.rowIndex_ && base_.rowIndex_ < base_.rowCount_);
}

int32_t RowMapper::InputCursor::getRowIndex() const {
	return base_.rowIndex_;
}

void RowMapper::InputCursor::reset() {
	base_.reset();
	in_.base().clear();
}

void RowMapper::InputCursor::getRowFixedPartAll(
		GSRowSetType resultType, const util::NormalXArray<uint8_t> &resultData,
		const uint8_t *&data, size_t &size) const {
	data = resultData.data() + base_.topPos_;
	size = (resultType == GS_ROW_SET_AGGREGATION_RESULT) ?
			sizeof(int8_t) + sizeof(GSTimestamp) :
			base_.varDataTop_ - base_.topPos_;
}

void RowMapper::InputCursor::getRowVariablePartAll(
		GSRowSetType resultType, const util::NormalXArray<uint8_t> &resultData,
		const uint8_t *&data, size_t &size) const {
	if (resultType == GS_ROW_SET_AGGREGATION_RESULT) {
		data = NULL;
		size = 0;
	}
	else {
		data = resultData.data() + base_.varDataTop_;
		size = resultData.size() - base_.varDataTop_;
	}
}

void RowMapper::InputCursor::beginRow(
		const RowMapper &mapper, uint8_t *nulls) {
	base_.mapper_ = &mapper;
	base_.rowIndex_++;
	base_.fieldIndex_ = -1;
	if (base_.rowIdIncluded_) {
		in_ >> base_.lastRowID_;
	}
	if (base_.mode_ == RowMapper::MODE_ROWWISE_SEPARATED_V2) {
		if (base_.mapper_->getVarColumnCount() > 0) {
			in_ >> base_.varDataOffset_;
			if (base_.rowIndex_ == 0) {
				base_.partialVarDataOffset_ = base_.varDataOffset_; 
			}
			base_.varDataLast_ = base_.varDataTop_ + base_.varDataOffset_ - base_.partialVarDataOffset_;
			const size_t savePos = in_.base().position();
			in_.base().position(base_.varDataLast_);
			const uint32_t elemCount = ClientUtil::readVarSize(in_);
			base_.varDataLast_ += ClientUtil::getEncodedVarSize(elemCount);
			in_.base().position(savePos);
		}
		if (nulls == NULL) {
			in_.base().position(in_.base().position() + mapper.nullsByteSize_);
		}
		else {
			in_.readAll(nulls, mapper.nullsByteSize_);
		}
	} else {
		base_.varDataOffset_ = -1;
	}
}

void RowMapper::InputCursor::endRow() {
	if (base_.varDataSeparated_ && base_.rowIndex_ + 1 >= base_.rowCount_) {
		in_.base().position(base_.varDataLast_);
	}
	base_.mapper_ = NULL;
}

void RowMapper::InputCursor::beginField() {
	base_.beginField(in_.base());
}

void RowMapper::InputCursor::beginVarData() {
	if (base_.varDataSeparated_) {
		if (base_.mode_ == RowMapper::MODE_ROWWISE_SEPARATED_V2) {
			base_.pendingPos_ = in_.base().position();
			in_.base().position(base_.varDataLast_);
		} else {
			int64_t offset;
			in_ >> offset;
			base_.pendingPos_ = in_.base().position();
			in_.base().position(base_.varDataTop_ +
								ClientUtil::toSizeValue(offset - varDataBaseOffset_));
		}
	}
}

void RowMapper::InputCursor::endVarData() {
	if (base_.varDataSeparated_) {
		base_.varDataLast_ = in_.base().position();
		in_.base().position(base_.pendingPos_);
		base_.pendingPos_ = 0;
	}
}


RowMapper::OutputCursor::OutputCursor(
		XArrayByteOutStream &out, const RowMapper &mapper,
		MappingMode mode, int32_t rowCount, bool rowIdIncluded) :
		base_(mapper, mode, rowCount, rowIdIncluded, out.base().position()),
		out_(out) {
}

void RowMapper::OutputCursor::reset() {
	base_.reset();
	out_.base().clear();
}

void RowMapper::OutputCursor::beginRow(
		const RowMapper &mapper, int64_t rowID, const uint8_t *nulls) {
	base_.mapper_ = &mapper;
	base_.rowIndex_++;
	base_.fieldIndex_ = -1;
	if (base_.rowIdIncluded_ && rowID != -1) {
		out_ << rowID;
	}
	if (base_.mode_ == RowMapper::MODE_ROWWISE_SEPARATED_V2) {
		if (base_.mapper_->getVarColumnCount() > 0) {
			const size_t offset = base_.varDataLast_ - base_.varDataTop_;
			out_ << static_cast<int64_t>(offset);
			const size_t savePos = out_.base().position();
			out_.base().position(base_.varDataLast_);
			ClientUtil::writeVarSize(out_, base_.mapper_->getVarColumnCount());
			base_.varDataLast_ = out_.base().position();
			out_.base().position(savePos);
		}
		if (nulls == NULL) {
			for (uint32_t i = 0; i < mapper.nullsByteSize_; ++i) {
				int8_t nullByte = 0;
				out_ << nullByte;
			}
		}
		else {
			out_.writeAll(nulls, mapper.nullsByteSize_);
		}
	} else {
	}
}

void RowMapper::OutputCursor::endRow() {
	if (base_.varDataSeparated_ && base_.rowIndex_ + 1 >= base_.rowCount_) {
		out_.base().position(base_.varDataLast_);
	}
	base_.mapper_ = NULL;
}

void RowMapper::OutputCursor::beginField() {
	base_.beginField(out_.base());
}

void RowMapper::OutputCursor::beginVarData() {
	if (base_.varDataSeparated_) {
		if (base_.mode_ == RowMapper::MODE_ROWWISE_SEPARATED_V2) {
			base_.pendingPos_ = out_.base().position();
			out_.base().position(base_.varDataLast_);
		} else {
			const size_t offset = base_.varDataLast_ - base_.varDataTop_;
			out_ << static_cast<int64_t>(offset);
			base_.pendingPos_ = out_.base().position();
			out_.base().position(base_.varDataLast_);
		}
	}
}

void RowMapper::OutputCursor::endVarData() {
	if (base_.varDataSeparated_) {
		base_.varDataLast_ = out_.base().position();
		out_.base().position(base_.pendingPos_);
		base_.pendingPos_ = 0;
	}
}


const size_t RowMapper::VarDataPool::NORMAL_BLOCK_SIZE =
		std::max<size_t>(sizeof(BlockHead) * 2, 1U << 13);

RowMapper::VarDataPool::VarDataPool() :
		block_(NULL), tail_(NULL), end_(NULL) {
}

RowMapper::VarDataPool::~VarDataPool() {
	clear();
	delete[] reinterpret_cast<uint8_t*>(block_);
}

void* RowMapper::VarDataPool::allocate(size_t size) {
	assert(size > 0);
	size = (size + (sizeof(AlignmentUnit) - 1)) & ~(sizeof(AlignmentUnit) - 1);

	if (tail_ + size > end_) {
		const size_t newBodySize =
				std::max<size_t>(NORMAL_BLOCK_SIZE - sizeof(BlockHead), size);
		void *newBlockPtr = new uint8_t[newBodySize + sizeof(BlockHead)];

		BlockHead *blockHead = static_cast<BlockHead*>(newBlockPtr);
		blockHead->prev_ = block_;
		blockHead->bodySize_ = newBodySize;

		uint8_t *const ptr =
				static_cast<uint8_t*>(newBlockPtr) + sizeof(BlockHead);
		block_ = blockHead;
		tail_ = ptr + size;
		end_ = ptr + newBodySize;
		return ptr;
	}

	void *const ptr = tail_;
	tail_ += size;
	return ptr;
}

void RowMapper::VarDataPool::deallocate(void*) {
}

void RowMapper::VarDataPool::clear() {
	while (block_ != NULL) {
		if (block_->prev_ == NULL &&
				block_->bodySize_ + sizeof(BlockHead) == NORMAL_BLOCK_SIZE) {
			break;
		}

		uint8_t *orgBlock = reinterpret_cast<uint8_t*>(block_);
		block_ = block_->prev_;
		delete[] orgBlock;
	}

	if (block_ == NULL) {
		tail_ = NULL;
		end_ = NULL;
	}
	else {
		tail_ = reinterpret_cast<uint8_t*>(block_) + sizeof(BlockHead);
		end_ = tail_ + block_->bodySize_;
	}
}

size_t RowMapper::VarDataPool::getUsageSize() {
	size_t size = 0;
	if (block_ != NULL) {
		size += static_cast<size_t>(tail_ - (end_ - block_->bodySize_));
		for (BlockHead *block; (block = block_->prev_) != NULL;) {
			size += block_->bodySize_;
		}
	}
	return size;
}


RowMapper::Cache::Cache() {
}

const RowMapper* RowMapper::Cache::resolve(
		RowTypeCategory rowTypeCategory, const GSBinding *binding,
		bool general, const Config &config) {
	const size_t digest = getDigest(
			rowTypeCategory, binding, general, config.nullableAllowed_);
	util::LockGuard<util::Mutex> lock(mutex_);

	std::pair<EntryMapIterator, EntryMapIterator> range =
			entryMap_.equal_range(digest);
	for (EntryMapIterator it = range.first; it != range.second; ++it) {
		RowMapper *mapper = it->second;
		if (mapper->matches(rowTypeCategory, binding, general, config)) {
			mapper->refCount_++;
			return mapper;
		}
	}

	std::auto_ptr<RowMapper> mapper(new RowMapper(
			digest, rowTypeCategory, general, config.nullableAllowed_));
	mapper->binding_ = checkAndCopyBinding(
			binding, mapper->columnIdMap_, rowTypeCategory, config);
	mapper->keyColumnId_ = findRowKeyId(&mapper->binding_);
	mapper->setupAccessInfo(); 
	mapper->refCount_++;
	entryMap_.insert(std::make_pair(digest, mapper.get()));
	return mapper.release();
}

const RowMapper* RowMapper::Cache::resolve(
		const RowMapper &baseMapper, ArrayByteInStream &schemaIn,
		const Config &config, bool columnOrderIgnorable) {
	const size_t digest = getDigest(baseMapper, schemaIn, config);
	util::LockGuard<util::Mutex> lock(mutex_);

	std::pair<EntryMapIterator, EntryMapIterator> range =
			entryMap_.equal_range(digest);
	for (EntryMapIterator it = range.first; it != range.second; ++it) {
		RowMapper *mapper = it->second;
		if (mapper->matches(baseMapper, schemaIn, config)) {
			mapper->refCount_++;
			return mapper;
		}
	}

	std::auto_ptr<RowMapper> mapper(new RowMapper(
			digest, baseMapper.rowTypeCategory_, baseMapper.general_,
			baseMapper.nullableAllowed_));
	mapper->binding_ = createReorderedBinding(
			baseMapper, mapper->columnIdMap_, schemaIn,
			config, columnOrderIgnorable);
	mapper->keyColumnId_ = findRowKeyId(&mapper->binding_);
	mapper->setupAccessInfo(); 
	mapper->refCount_++;

	entryMap_.insert(std::make_pair(digest, mapper.get()));
	return mapper.release();
}

const RowMapper* RowMapper::Cache::resolve(
		const RowMapper::ContainerInfoRef<true> &infoRef,
		const Config &config) {
	std::vector<GSBindingEntry> entryList;
	const GSBinding &binding =
			GSRow::createBinding(infoRef, entryList, config.anyTypeAllowed_);

	return resolve(
			containerTypeToCategory(infoRef.getType()), &binding, true,
			config);
}

const RowMapper* RowMapper::Cache::duplicate(const RowMapper &mapper) {
	const size_t digest = mapper.digest_;
	util::LockGuard<util::Mutex> lock(mutex_);

	std::pair<EntryMapIterator, EntryMapIterator> range =
			entryMap_.equal_range(digest);
	for (EntryMapIterator it = range.first; it != range.second; ++it) {
		if (it->second == &mapper) {
			it->second->refCount_++;
			return it->second;
		}
	}

	GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
}

void RowMapper::Cache::release(const RowMapper **mapper) {
	assert(mapper != NULL);
	assert(*mapper != NULL);

	util::LockGuard<util::Mutex> lock(mutex_);

	std::pair<EntryMapIterator, EntryMapIterator> range =
			entryMap_.equal_range((*mapper)->digest_);
	EntryMapIterator targetIt = entryMap_.end();
	for (EntryMapIterator it = range.first; it != range.second; ++it) {
		if (it->second == *mapper) {
			targetIt = it;
			break;
		}
	}

	if (targetIt == entryMap_.end()) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	RowMapper *targetMapper = targetIt->second;
	if (targetMapper->refCount_ == 0) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	if (--targetMapper->refCount_ == 0) {
		entryMap_.erase(targetIt);
		delete targetMapper;
	}

	*mapper = NULL;
}


RowMapper::Reference::Reference(Cache &cache, const RowMapper *mapper) :
		cache_(&cache), mapper_(mapper) {
}

RowMapper::Reference::Reference(std::auto_ptr<RowMapper> mapper) :
		cache_(NULL),
		mapper_(mapper.release()) {
}

RowMapper::Reference::~Reference() {
	reset();
}

RowMapper::Reference::Reference(Reference &another) :
		cache_(another.cache_),
		mapper_(another.mapper_) {
	another.cache_ = NULL;
	another.mapper_ = NULL;
}

RowMapper::Reference& RowMapper::Reference::operator=(
		Reference &another) {
	if (&another == this) {
		return *this;
	}

	reset();

	cache_ = another.cache_;
	mapper_ = another.mapper_;

	another.cache_ = NULL;
	another.mapper_ = NULL;

	return *this;
}

const RowMapper* RowMapper::Reference::operator->() const {
	assert(mapper_ != NULL);
	return mapper_;
}

const RowMapper& RowMapper::Reference::operator*() const {
	assert(mapper_ != NULL);
	return *mapper_;
}

const RowMapper* RowMapper::Reference::get() const {
	return mapper_;
}

const RowMapper* RowMapper::Reference::release() {
	const RowMapper *mapper = mapper_;
	cache_ = NULL;
	mapper_ = NULL;

	return mapper;
}

RowMapper::Cache* RowMapper::Reference::getCache() const {
	return cache_;
}

void RowMapper::Reference::reset() throw() try {
	if (cache_ != NULL) {
		cache_->release(&mapper_);
		cache_ = NULL;
	}
	else {
		delete const_cast<RowMapper*>(mapper_);
	}
	mapper_ = NULL;
}
catch (...) {
}


template<bool Const>
RowMapper::ContainerInfoRef<Const>::ContainerInfoRef(
		RefType ref, const ClientVersion &version) throw() :
		ref_(ref),
		version_(version),
		columnInfoList_(NULL) {
}

template<bool Const>
RowMapper::ContainerInfoRef<Const>::ContainerInfoRef(
		const ContainerInfoRef &another) throw() :
		ref_(another.ref_),
		version_(another.version_),
		columnInfoList_(NULL) {
}

template<bool Const>
RowMapper::ContainerInfoRef<Const>&
RowMapper::ContainerInfoRef<Const>::operator=(
		const ContainerInfoRef &another) throw() {
	if (this != &another) {
		ref_ = another.ref_;
		version_ = another.version_;
		columnInfoList_ = NULL;
	}

	return *this;
}

template<bool Const>
void RowMapper::ContainerInfoRef<Const>::clear(
		RefType ref, const ClientVersion &version) throw() {
	ContainerInfoRef infoRef(ref, version);
	infoRef.clear();
}

template<>
void RowMapper::ContainerInfoRef<false>::clear() throw() {
	if (ref_ == NULL) {
		return;
	}

	const GSContainerInfo initialInfo = GS_CONTAINER_INFO_INITIALIZER;
	set(initialInfo);

	ref_->columnCount = 0;
	ref_->columnInfoList = NULL;
}

template<bool Const>
void RowMapper::ContainerInfoRef<Const>::clear() throw() {
	assert(false);
}

template<bool Const>
bool RowMapper::ContainerInfoRef<Const>::hasRef() const {
	return (ref_ != NULL);
}

template<bool Const>
RowMapper::ContainerInfoRef<true>
RowMapper::ContainerInfoRef<Const>::toConst() const {
	return ContainerInfoRef<true>(ref_, version_);
}

template<bool Const>
void RowMapper::ContainerInfoRef<Const>::get(
		GSContainerInfo &info) const {
	assert(ref_ != NULL);

	const GSContainerInfo initialInfo = GS_CONTAINER_INFO_INITIALIZER;
	info = initialInfo;

	info.name = ref_->name;
	info.type = ref_->type;
	info.rowKeyAssigned = ref_->rowKeyAssigned;

	if (version_.since(1, 5)) {
		info.columnOrderIgnorable = ref_->columnOrderIgnorable;
		info.timeSeriesProperties = getTimeSeriesProperties();
		info.triggerInfoCount = ref_->triggerInfoCount;
		info.triggerInfoList = ref_->triggerInfoList;
	}

	if (version_.since(2, 1)) {
		info.dataAffinity = ref_->dataAffinity;
	}

	if (version_.since(3, 5)) {
		info.indexInfoCount = ref_->indexInfoCount;
		info.indexInfoList = ref_->indexInfoList;
	}
}

template<>
void RowMapper::ContainerInfoRef<false>::set(const GSContainerInfo &info) {
	assert(ref_ != NULL);
	assert(info.columnCount == 0 && info.columnInfoList == NULL);

	ref_->name = info.name;
	ref_->type = info.type;
	ref_->rowKeyAssigned = info.rowKeyAssigned;

	if (version_.since(1, 5)) {
		ref_->columnOrderIgnorable = info.columnOrderIgnorable;
		ref_->timeSeriesProperties = info.timeSeriesProperties;
		ref_->triggerInfoCount = info.triggerInfoCount;
		ref_->triggerInfoList = info.triggerInfoList;
	}

	if (version_.since(2, 1)) {
		ref_->dataAffinity = info.dataAffinity;
	}

	if (version_.since(3, 5)) {
		ref_->indexInfoCount = info.indexInfoCount;
		ref_->indexInfoList = info.indexInfoList;
	}
}

template<bool Const>
void RowMapper::ContainerInfoRef<Const>::set(const GSContainerInfo &info) {
	assert(false);
	static_cast<void>(info);
}

template<bool Const>
GSContainerType RowMapper::ContainerInfoRef<Const>::getType() const {
	assert(ref_ != NULL);
	return ref_->type;
}

template<bool Const>
bool RowMapper::ContainerInfoRef<Const>::isRowKeyAssigned() const {
	assert(ref_ != NULL);
	return ClientUtil::toBool(ref_->rowKeyAssigned);
}

template<bool Const>
size_t RowMapper::ContainerInfoRef<Const>::getColumnCount() const {
	assert(ref_ != NULL);
	return ref_->columnCount;
}

template<bool Const>
GSColumnInfo RowMapper::ContainerInfoRef<Const>::getColumnInfo(
		int32_t column) const {
	assert(ref_ != NULL);

	const GSColumnInfo *columnInfoRef = static_cast<const GSColumnInfo*>(
			getColumnInfoRef<true>(*this, ref_->columnInfoList, column));

	GSColumnInfo columnInfo = GS_COLUMN_INFO_INITIALIZER;
	columnInfo.name = columnInfoRef->name;
	columnInfo.type = columnInfoRef->type;

	if (version_.since(1, 5)) {
		columnInfo.indexTypeFlags = columnInfoRef->indexTypeFlags;
	}

	if (version_.since(3, 5)) {
		columnInfo.options = columnInfoRef->options;
	}

	return columnInfo;
}

template<>
void RowMapper::ContainerInfoRef<false>::setColumnInfo(
		int32_t column, const GSColumnInfo &columnInfo) {
	assert(ref_ != NULL);
	assert(columnInfoList_ != NULL);

	GSColumnInfo *columnInfoRef = static_cast<GSColumnInfo*>(
			getColumnInfoRef<false>(*this, columnInfoList_, column));

	columnInfoRef->name = columnInfo.name;
	columnInfoRef->type = columnInfo.type;

	if (version_.since(1, 5)) {
		columnInfoRef->indexTypeFlags = columnInfo.indexTypeFlags;
	}

	if (version_.since(3, 5)) {
		columnInfoRef->options = columnInfo.options;
	}
}

template<bool Const>
void RowMapper::ContainerInfoRef<Const>::setColumnInfo(
		int32_t column, const GSColumnInfo &columnInfo) {
	assert(false);
	static_cast<void>(column);
	static_cast<void>(columnInfo);
}

template<>
void RowMapper::ContainerInfoRef<false>::createColumnInfoList(
		size_t columnCount, VarDataPool &pool) {
	assert(ref_ != NULL);

	if (columnCount == 0) {
		columnInfoList_ = NULL;
	}
	else {
		columnInfoList_ = pool.allocate(getColumnInfoSize() * columnCount);
	}

	ref_->columnInfoList = static_cast<GSColumnInfo*>(columnInfoList_);
	ref_->columnCount = columnCount;
}

template<bool Const>
void RowMapper::ContainerInfoRef<Const>::createColumnInfoList(
		size_t columnCount, VarDataPool &pool) {
	assert(false);
	static_cast<void>(columnCount);
	static_cast<void>(pool);
}

template<bool Const>
const void* RowMapper::ContainerInfoRef<Const>::getRawColumnInfoList() const {
	assert(ref_ != NULL);
	return ref_->columnInfoList;
}

template<bool Const>
template<bool RefConst>
typename util::Conditional<RefConst, const void*, void*>::Type
RowMapper::ContainerInfoRef<Const>::getColumnInfoRef(
		const ContainerInfoRef &infoRef,
		typename util::Conditional<
				RefConst, const void*, void*>::Type columnInfoList,
		int32_t column) {
	assert(column >= 0);
	assert(static_cast<size_t>(column) < infoRef.getColumnCount());

	typedef typename util::Conditional<
			RefConst, const uint8_t*, uint8_t*>::Type BytesType;

	return static_cast<BytesType>(columnInfoList) +
			infoRef.getColumnInfoSize() * column;
}

template<bool Const>
size_t RowMapper::ContainerInfoRef<Const>::getColumnInfoSize() const {
	if (version_.since(3, 5)) {
		return sizeof(ColumnInfoTraits<2>::Type);
	}
	if (version_.since(1, 5)) {
		return sizeof(ColumnInfoTraits<1>::Type);
	}
	else {
		return sizeof(ColumnInfoTraits<0>::Type);
	}
}

template<bool Const>
const GSTimeSeriesProperties*
RowMapper::ContainerInfoRef<Const>::getTimeSeriesProperties() const {
	const GSTimeSeriesProperties *src = ref_->timeSeriesProperties;
	if (src == NULL || version_.since(2, 0)) {
		return src;
	}
	const GSTimeSeriesProperties initialTSProps =
			GS_TIME_SERIES_PROPERTIES_INITIALIZER;
	tsProps_ = initialTSProps;
	tsProps_.rowExpirationTime = src->rowExpirationTime;
	tsProps_.rowExpirationTimeUnit = src->rowExpirationTimeUnit;
	tsProps_.compressionWindowSize = src->compressionWindowSize;
	tsProps_.compressionWindowSizeUnit = src->compressionWindowSizeUnit;
	tsProps_.compressionMethod = src->compressionMethod;
	tsProps_.compressionListSize = src->compressionListSize;
	tsProps_.compressionList = src->compressionList;

	return &tsProps_;
}

template class RowMapper::ContainerInfoRef<true>;
template class RowMapper::ContainerInfoRef<false>;


ContainerKey::ContainerKey() :
		caseSensitive_(false),
		compatible_(false) {
}

ContainerKey ContainerKey::toCaseSensitive(bool caseSensitive) const {
	ContainerKey key = *this;
	key.caseSensitive_ = caseSensitive;
	return key;
}

bool operator<(const ContainerKey &key1, const ContainerKey &key2) {
	return ContainerKeyConverter(false, true).compare(key1, key2) < 0;
}

bool operator==(const ContainerKey &key1, const ContainerKey &key2) {
	return ContainerKeyConverter(false, true).compare(key1, key2) == 0;
}

std::ostream& operator<<(std::ostream &out, const ContainerKey &key) {
	out << ContainerKeyConverter(false, true).format(key);
	return out;
}


const int64_t ContainerKeyConverter::PUBLIC_DATABASE_ID =
		ContainerKeyUtils::PUBLIC_DATABASE_ID;

int64_t ContainerKeyConverter::getPublicDatabaseId() {
	return PUBLIC_DATABASE_ID;
}

ContainerKeyConverter::ContainerKeyConverter(
		bool compatible, bool internalMode) :
		compatible_(compatible),
		internalMode_(internalMode) {
}

bool ContainerKeyConverter::isCompatible() const {
	return compatible_;
}

ContainerKey ContainerKeyConverter::get(
		ArrayByteInStream &in, bool caseSensitive) const {
	if (compatible_) {
		std::string str;
		in >> str;
		return parse(str.c_str(), caseSensitive);
	}
	else {
		const size_t size = ClientUtil::readVarSize(in);
		ContainerKey key;
		if (size > 0) {
			key.bytes_.resize(size);
			in.readAll(&key.bytes_[0], size);
		}

		Components components;
		decompose(key, components);
		components.databaseId_ = PUBLIC_DATABASE_ID;

		return compose(components, caseSensitive);
	}
}

void ContainerKeyConverter::put(
		XArrayByteOutStream &out,
		const ContainerKey &key, int64_t databaseId) const {
	ContainerKey outKey;

	if (key.compatible_) {
		outKey = key;

		out << ClientUtil::sizeValueToInt32(outKey.bytes_.size());
	}
	else {
		Components components;
		decompose(key, components);
		components.databaseId_ = databaseId;
		outKey = compose(components, false);

		ClientUtil::writeVarSize(out, outKey.bytes_.size());
	}

	out.writeAll(
			ContainerKeyUtils::getBytesAddress(outKey.bytes_),
			outKey.bytes_.size());
}

ContainerKey ContainerKeyConverter::compose(
		const Components &components, bool caseSensitive) const {
	if (compatible_) {
		if (components.largeId_ != -1 ||
				components.affinityNum_ != -1 ||
				components.affinityStr_.get() != NULL ||
				components.systemNum_ != -1 ||
				components.systemStr_.get() != NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_VALUE_FORMAT, "");
		}
		return parse(components.base_.c_str(), caseSensitive);
	}
	else {
		ContainerKey key;
		key.caseSensitive_ = caseSensitive;
		ContainerKeyUtils::compose(components, key.bytes_);
		return key;
	}
}

void ContainerKeyConverter::decompose(
		const ContainerKey &key, Components &components) const {
	if (key.compatible_) {
		components.base_ = format(key);
	}
	else {
		ContainerKeyUtils::decompose(key.bytes_, components);
	}
}

std::string ContainerKeyConverter::format(const ContainerKey &key) const {
	util::NormalOStringStream oss;
	format(oss, key);
	return oss.str();
}

std::ostream& ContainerKeyConverter::format(
		std::ostream &os, const ContainerKey &key) const {
	if (key.compatible_) {
		const void *addr = ContainerKeyUtils::getBytesAddress(key.bytes_);
		std::string str(static_cast<const GSChar*>(addr), key.bytes_.size());
		os << str;
	}
	else {
		ContainerKeyUtils::format(os, key.bytes_);
	}
	return os;
}

ContainerKey ContainerKeyConverter::parse(
		const char8_t *str, bool caseSensitive) const {
	ContainerKey key;

	key.caseSensitive_ = caseSensitive;
	key.compatible_ = compatible_;

	if (key.compatible_) {
		ClientUtil::checkBasicSymbol(str, "container name");

		util::NormalXArray<uint8_t> buf;
		XArrayByteOutStream out((NormalXArrayOutStream(buf)));
		out.writeAll(str, strlen(str));
		key.bytes_.assign(buf.begin(), buf.end());
	}
	else {
		if (!ContainerKeyUtils::parse(str, key.bytes_, internalMode_)) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_VALUE_FORMAT, "");
		}
	}

	return key;
}

int32_t ContainerKeyConverter::compare(
		const ContainerKey &key1, const ContainerKey &key2) const {
	int32_t diff = (key1.compatible_ ? 0 : 1) - (key2.compatible_ ? 0 : 1);
	if (diff != 0) {
		return diff;
	}

	const bool caseSensitive = (key1.caseSensitive_ || key2.caseSensitive_);
	if (key1.compatible_) {
		std::vector<uint8_t>::const_iterator it1 = key1.bytes_.begin();
		std::vector<uint8_t>::const_iterator it2 = key2.bytes_.begin();
		std::vector<uint8_t>::const_iterator end1 = key1.bytes_.end();
		std::vector<uint8_t>::const_iterator end2 = key2.bytes_.end();
		for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
			diff = (caseSensitive ?
					*it1 - *it2 :
					ClientUtil::toASCIILower(*it1) -
							ClientUtil::toASCIILower(*it2));
			if (diff != 0) {
				return diff;
			}
		}
		const ptrdiff_t ptrDiff = (end1 - it1) - (end2 - it2);
		return (ptrDiff < 0 ? -1 : (ptrDiff > 0 ? 1 : 0));
	}

	return ContainerKeyUtils::compare(key1.bytes_, key2.bytes_, caseSensitive);
}


const int32_t NodeConnection::EE_MAGIC_NUMBER = 65021048;

const int32_t NodeConnection::DEFAULT_PROTOCOL_VERSION = 15;

const int32_t NodeConnection::STATEMENT_TYPE_NUMBER_V2_OFFSET = 100;

const char8_t NodeConnection::ERROR_PARAM_ADDRESS[] = "address";
const char8_t NodeConnection::ERROR_PARAM_PARTITION[] = "partition";

volatile int32_t NodeConnection::protocolVersion_ = DEFAULT_PROTOCOL_VERSION;
bool NodeConnection::tcpNoDelayEnabled_ = true;

NodeConnection::NodeConnection(
		const util::SocketAddress &address, const Config &config) :
		address_(address),
		statementTimeoutMillis_(config.statementTimeoutEnabled_ ?
				config.statementTimeoutMillis_ :
				std::numeric_limits<int64_t>::max()),
		heartbeatTimeoutMillis_(config.heartbeatTimeoutMillis_),
		alternativeVersion_(config.alternativeVersion_),
		alternativeVersionEnabled_(config.alternativeVersionEnabled_),
		authMode_(Auth::Challenge::getDefaultMode()),
		remoteProtocolVersion_(0),
		statementId_(0),
		heartbeatReceiveCount_(0),
		responseUnacceptable_(false),
		authenticated_(false),
		ownerMode_(false),
		transactionTimeoutSecs_(-1) {
	if (address_.getFamily() != util::SocketAddress::FAMILY_INET &&
			address_.getFamily() != util::SocketAddress::FAMILY_INET6) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR,
				"Invalid address (address=" << address_ << ")");
	}

	socket_.open(address_.getFamily(), util::Socket::TYPE_STREAM);
	if (tcpNoDelayEnabled_) {
		socket_.setNoDelay(true);
	}
	if (config.connectTimeoutMillis_ >= 0) {
		socket_.setBlockingMode(false);

		if (!socket_.connect(address_)) {
			util::IOPollSelect selector;
			selector.add(&socket_, util::IOPollEvent::TYPE_WRITE);

			uint32_t connectTimeoutMillis = static_cast<uint32_t>(
					Properties::timeoutToInt32Millis(config.connectTimeoutMillis_));
			if (connectTimeoutMillis == 0) {
				connectTimeoutMillis = std::numeric_limits<uint32_t>::max();
			}
			if (!selector.dispatch(connectTimeoutMillis)) {
				GS_CLIENT_THROW_CONNECTION(
						GS_ERROR_CC_CONNECTION_TIMEOUT,
						"Failed to connect (address=" << address_ << ")");
			}
		}

		socket_.setBlockingMode(true);

		const int32_t timeoutMillis = Properties::timeoutToInt32Millis(
				std::min(statementTimeoutMillis_, heartbeatTimeoutMillis_));
		socket_.setReceiveTimeout(timeoutMillis);
		socket_.setSendTimeout(timeoutMillis);
	}
	else {
		try {
			if (!socket_.connect(address_)) {
				GS_CLIENT_THROW_CONNECTION(
						GS_ERROR_CC_BAD_CONNECTION,
						"Failed to connect (address=" << address_ << ")");
			}
		}
		catch (std::exception &e) {
			GS_CLIENT_RETHROW_CONNECTION(
					e, "Failed to connect (address=" << address_ <<
					", reason=" << ErrorReasonFormatter(e) << ")");
		}
	}
}

NodeConnection::~NodeConnection() {
	try {
		if (!socket_.isClosed()) {
			util::NormalXArray<uint8_t> req;
			util::NormalXArray<uint8_t> resp;
			disconnect(req, resp);
		}
	}
	catch (...) {
	}
}

int32_t NodeConnection::getProtocolVersion() {
	return protocolVersion_;
}

int32_t NodeConnection::getRemoteProtocolVersion() const {
	return remoteProtocolVersion_;
}

bool NodeConnection::isSupportedProtocolVersion(int32_t protocolVersion) {
	switch (protocolVersion) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 8:
	case 13:
	case 14:
		return true;
	default:
		return false;
	}
}

void NodeConnection::setProtocolVersion(int32_t protocolVersion) {
	if (!isSupportedProtocolVersion(protocolVersion)) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
				"Wrong protocol version");
	}
	protocolVersion_ = protocolVersion;
}

const util::SocketAddress& NodeConnection::getRemoteAddress() const {
	return address_;
}

size_t NodeConnection::getEEHeadLength(bool ipv6Enabled) {
	if (ipv6Enabled) {
		return 4 * 4 + 16;
	}
	else {
		return 4 * 5;
	}
}

size_t NodeConnection::getRequestHeadLength(
		bool ipv6Enabled, bool firstStatement) {
	const size_t statementIdSize =
			(isStatementIdLarge(firstStatement) ? 8 : 4);
	return getEEHeadLength(ipv6Enabled) + 4 * 2 + statementIdSize;
}

void NodeConnection::fillRequestHead(
		bool ipv6Enabled, util::NormalXArray<uint8_t> &req,
		bool firstStatement) {
	XArrayByteOutStream out =
			XArrayByteOutStream(NormalXArrayOutStream(req));
	out.base().position(0);

	out << EE_MAGIC_NUMBER;

	if (ipv6Enabled) {
		out << static_cast<int64_t>(0);
		out << static_cast<int64_t>(0);
	}
	else {
		out << static_cast<int32_t>(0);
	}

	out << static_cast<int32_t>(0);

	out << static_cast<int32_t>(-1);

	out << static_cast<int32_t>(0);

	out << static_cast<int32_t>(0);

	out << static_cast<int32_t>(0);

	putStatementId(out, 0, firstStatement);
}

void NodeConnection::putStatementId(
		XArrayByteOutStream &reqOut, int64_t statementId,
		bool firstStatement) {
	if (isStatementIdLarge(firstStatement)) {
		reqOut << statementId;
	}
	else {
		reqOut << static_cast<int32_t>(statementId);
	}
}

bool NodeConnection::isOptionalRequestEnabled() {
	return protocolVersion_ >= 3;
}

bool NodeConnection::isClientIdOnLoginEnabled() {
	return protocolVersion_ >= 13;
}

bool NodeConnection::isDatabaseIdEnabled() {
	return protocolVersion_ >= 14;
}

void NodeConnection::tryPutEmptyOptionalRequest(XArrayByteOutStream &reqOut) {
	if (isOptionalRequestEnabled()) {
		reqOut << static_cast<int32_t>(0);
	}
}

ArrayByteInStream NodeConnection::executeStatement(
		Statement::Id statement, int32_t partitionId, int64_t statementId,
		util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> *resp,
		Heartbeat *heartbeat) {
	return executeStatementDirect(statementToNumber(statement),
				partitionId, statementId, req, resp, heartbeat);
}

ArrayByteInStream NodeConnection::executeStatementDirect(
		int32_t statementTypeNumber, int32_t partitionId, int64_t statementId,
		util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> *resp,
		Heartbeat *heartbeat) {
#if GS_CLIENT_ENABLE_PROFILER
	util::Stopwatch watch;
	watch.start();
#endif
	if (partitionId < 0) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR,
				"Internal error by illegal partition ID ("
				"partitionId=" << partitionId <<
				", address=" << address_ << ")");
	}

	const bool firstStatement =
			(statementTypeNumber == statementToNumber(Statement::CONNECT));
	const bool ipv6Enabled = isIPV6Enabled();
	const size_t eeHeadLength = getEEHeadLength(ipv6Enabled);
	const size_t reqLength = req.size();
	assert(reqLength >= getRequestHeadLength(ipv6Enabled, firstStatement));

	XArrayByteOutStream reqOut =
			XArrayByteOutStream(NormalXArrayOutStream(req));
	reqOut.base().position(eeHeadLength - sizeof(int32_t));
	reqOut << ClientUtil::sizeValueToInt32(reqLength - eeHeadLength);
	reqOut << statementTypeNumber;
	reqOut << partitionId;
	int64_t reqStatementId;
	if (statementId == 0) {
		reqStatementId = ++statementId_;
		if (statementId_ == 0) {
			statementId_++;
		}
	}
	else {
		reqStatementId = statementId;
	}
	putStatementId(reqOut, reqStatementId, firstStatement);
	reqOut.base().position(reqLength);

	try {
		socket_.send(req.data(), reqLength);
	}
	catch (std::exception &e) {
		GS_CLIENT_RETHROW_CONNECTION(
				e, "Failed to send message (address=" << address_ <<
				", reason=" << ErrorReasonFormatter(e) << ")");
	}
	reqOut.base().position(reqLength);

	if (resp == NULL) {
		return ArrayByteInStream(util::ArrayInStream(NULL, 0));
	}

	Heartbeat heartbeatStorage;
	std::auto_ptr< util::NormalXArray<uint8_t> > heartbeatBuf;
	util::NormalXArray<uint8_t> *orgResp;

	resp->resize(eeHeadLength);

	try {
		readFully(resp->data(), eeHeadLength);
		orgResp = NULL;
	}
	catch (ConnectionException&) {
		if (responseUnacceptable_ || heartbeat != NULL || firstStatement) {
			throw;
		}

		heartbeat = &heartbeatStorage;
		heartbeat->orgStatementTypeNumber_ = statementTypeNumber;
		heartbeat->orgStatementId_ = reqStatementId;
		heartbeat->orgStatementFound_ = false;
		heartbeat->orgRespPos_ = 0;
		heartbeat->orgRespSize_ = 0;

		heartbeatBuf.reset(new util::NormalXArray<uint8_t>());

		orgResp = resp;
		resp = processHeartbeat(partitionId, *resp, *heartbeatBuf, *heartbeat);
		if (resp == orgResp) {
			orgResp = NULL;
		}
	}

	ArrayByteInStream respHeadIn = ArrayByteInStream(
			util::ArrayInStream(resp->data(), eeHeadLength));
	int32_t magicNumber;
	respHeadIn >> magicNumber;
	if (magicNumber != EE_MAGIC_NUMBER) {
		GS_CLIENT_THROW_CONNECTION(
				GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by illegal magic number (address=" <<
				address_ << ")");
	}

	respHeadIn.base().position(eeHeadLength - sizeof(int32_t));
	int32_t respBodyLength;
	respHeadIn >> respBodyLength;

	resp->resize(respBodyLength);
	readFully(resp->data(), respBodyLength);
	ArrayByteInStream respBodyIn = ArrayByteInStream(
			util::ArrayInStream(resp->data(), respBodyLength));

	int32_t respStatementTypeNumber;
	{
		int32_t respStatementNumber;
		respBodyIn >> respStatementNumber;
		respStatementTypeNumber = statementToNumber(static_cast<Statement::Id>(
				respStatementNumber - getStatementNumberOffset()));
	}
	bool statementIdMatched;
	if (respStatementTypeNumber != statementTypeNumber) {
		if (heartbeat == NULL || respStatementTypeNumber !=
				heartbeat->orgStatementTypeNumber_) {
			GS_CLIENT_THROW_CONNECTION(
					GS_ERROR_CC_MESSAGE_CORRUPTED,
					"Protocol error by illegal statement type (address=" <<
					address_ << ")");
		}
		heartbeat->orgStatementFound_ = true;

		const bool orgFirstStatement = (heartbeat->orgStatementTypeNumber_ ==
				statementToNumber(Statement::CONNECT));
		statementIdMatched =
				(getStatementId(respBodyIn, orgFirstStatement) ==
						heartbeat->orgStatementId_);
	}
	else {
		statementIdMatched =
				(getStatementId(respBodyIn, firstStatement) == reqStatementId);
	}

	if (!statementIdMatched) {
		GS_CLIENT_THROW_CONNECTION(
				GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by illegal statement ID (address=" <<
				address_ << ")");
	}

#if GS_CLIENT_ENABLE_PROFILER
	ClientProfiler::INSTANCE.profileStatement(statement, watch.elapsedNanos());
#endif

	int8_t statementResult;
	respBodyIn >> statementResult;
	if (statementResult == StatementResult::SUCCESS) {
		if (heartbeat != NULL) {
			if (heartbeat->orgException_.get() != NULL) {
				throw StatementException(*heartbeat->orgException_);
			}

			if (orgResp != NULL) {
				respBodyIn = ArrayByteInStream(util::ArrayInStream(
						orgResp->data(), heartbeat->orgRespSize_));
				respBodyIn.base().position(heartbeat->orgRespPos_);
			}
		}
		return respBodyIn;
	}

	util::Exception::NamedErrorCode defaultCode;
	switch (statementResult) {
	case StatementResult::STATEMENT_ERROR:
		defaultCode = GS_COMMON_EXCEPTION_NAMED_CODE(
				GS_ERROR_CC_BAD_STATEMENT);
		break;
	case StatementResult::DENY:
		defaultCode = GS_COMMON_EXCEPTION_NAMED_CODE(
				GS_ERROR_CC_WRONG_NODE);
		break;
	default:
		defaultCode = GS_COMMON_EXCEPTION_NAMED_CODE(
				GS_ERROR_CC_BAD_CONNECTION);
		break;
	}

	ErrorStack errorStack(ErrorStack::tryRead(respBodyIn, defaultCode));
	ClientException &baseException = errorStack.getException();

	{
		util::NormalOStringStream oss;
		oss << address_;
		baseException.appendParameter(ERROR_PARAM_ADDRESS, oss.str().c_str());
	}
	{
		util::NormalOStringStream oss;
		oss << partitionId;
		baseException.appendParameter(
				ERROR_PARAM_PARTITION, oss.str().c_str());
	}

	try {
		util::NormalOStringStream oss;
		ClientException::formatMessageWithParameter(
				oss, baseException, baseException);

		try {
			throw baseException;
		}
		catch (...) {
			std::exception e;
			switch (statementResult) {
			case StatementResult::STATEMENT_ERROR:
				GS_CLIENT_RETHROW_STATEMENT(e, oss.str());
			case StatementResult::DENY:
				GS_CLIENT_RETHROW_WRONG_NODE(e, "");
			default:
				GS_CLIENT_RETHROW_CONNECTION(e, "");
			}
		}
	}
	catch (std::exception &e) {
		if (heartbeat != NULL && respStatementTypeNumber ==
				statementToNumber(Statement::CONNECT)) {
			GS_CLIENT_RETHROW_CONNECTION(
					e,
					"Connection problem occurred by "
					"invalid heartbeat response ("
					"address=" << address_ <<
					", reason=" << ErrorReasonFormatter(e) << ")");
		}
		throw;
	}
}

void NodeConnection::connect(
		util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp) {
	fillRequestHead(isIPV6Enabled(), req, true);
	XArrayByteOutStream reqOut =
			XArrayByteOutStream(NormalXArrayOutStream(req));

	putConnectRequest(reqOut);
	ArrayByteInStream respIn =
			executeStatement(Statement::CONNECT, 0, 0, req, &resp);
	acceptConnectResponse(respIn);
}

void NodeConnection::disconnect(
		util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp) {
	fillRequestHead(isIPV6Enabled(), req, false);
	XArrayByteOutStream reqOut =
			XArrayByteOutStream(NormalXArrayOutStream(req));
	tryPutEmptyOptionalRequest(reqOut);

	(void) resp;
	executeStatement(Statement::DISCONNECT, 0, 0, req, NULL);
	socket_.close();
}

void NodeConnection::login(
		util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp,
		const LoginInfo &loginInfo, int64_t *databaseId) {

	Auth::Challenge challenge;
	size_t lastInPos;
	if (!loginInternal(req, resp, loginInfo, challenge, lastInPos)) {
		loginInternal(req, resp, loginInfo, challenge, lastInPos);
	}

	int64_t respDatabaseId;
	{
		ArrayByteInStream respIn((
				util::ArrayInStream(resp.data(), resp.size())));
		respIn.base().position(lastInPos);

		if (respIn.base().remaining() > 0) {
			respIn >> respDatabaseId;
		}
		else {
			if (isDatabaseIdEnabled()) {
				GS_CLIENT_THROW_CONNECTION(
						GS_ERROR_CC_MESSAGE_CORRUPTED,
						"Protocol error by lack of database ID");
			}
			respDatabaseId = ContainerKeyConverter::getPublicDatabaseId();
		}
	}

	if (databaseId != NULL) {
		*databaseId = respDatabaseId;
	}
}

bool NodeConnection::loginInternal(
		util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp,
		const LoginInfo &loginInfo, Auth::Challenge &challenge,
		size_t &lastInPos) {

	fillRequestHead(isIPV6Enabled(), req, false);
	XArrayByteOutStream reqOut =
			XArrayByteOutStream(NormalXArrayOutStream(req));

	if (isOptionalRequestEnabled()) {
		OptionalRequest request;
		request.transactionTimeout_ = loginInfo.transactionTimeoutSecs_;
		request.dbName_ = loginInfo.database_;
		if (isClientIdOnLoginEnabled()) {
			request.clientId_ = loginInfo.clientId_;
		}
		request.applicationName_ = loginInfo.applicationName_;
		request.storeMemoryAgingSwapRate_ = loginInfo.storeMemoryAgingSwapRate_;
		request.format(reqOut);
	}

	std::allocator<uint8_t> alloc;
	Auth::String digestStr(alloc);
	Auth::Challenge::build(
			authMode_, challenge, loginInfo.passwordDigest_, &digestStr);

	reqOut << loginInfo.user_;
	reqOut << digestStr;
	reqOut << Properties::timeoutToInt32Seconds(statementTimeoutMillis_);
	reqOut << ClientUtil::toGSBool(loginInfo.ownerMode_);

	reqOut << loginInfo.clusterName_;
	Auth::Challenge::putRequest(reqOut, authMode_, challenge, false);

	ArrayByteInStream respIn =
			executeStatement(Statement::LOGIN, 0, 0, req, &resp);

	const bool authed = Auth::Challenge::getResponse(
			respIn, alloc, authMode_, challenge);
	lastInPos = respIn.base().position();

	if (!authed) {
		return false;
	}

	authenticatedUser_ = loginInfo.user_;
	authenticatedDatabase_ = loginInfo.database_;
	ownerMode_ = loginInfo.ownerMode_;
	transactionTimeoutSecs_ = loginInfo.transactionTimeoutSecs_;
	authenticated_ = true;
	return true;
}

void NodeConnection::logout(
		util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp) {
	fillRequestHead(isIPV6Enabled(), req, false);
	XArrayByteOutStream reqOut =
			XArrayByteOutStream(NormalXArrayOutStream(req));
	tryPutEmptyOptionalRequest(reqOut);

	authenticatedUser_.clear();
	authenticatedDatabase_.clear();
	authenticated_ = false;
	executeStatement(Statement::LOGOUT, 0, 0, req, &resp);
}

void NodeConnection::reuse(
		util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp,
		const LoginInfo &loginInfo, int64_t *databaseId) {
	login(req, resp, loginInfo, databaseId);
}

int64_t NodeConnection::getHeartbeatReceiveCount() const {
	return heartbeatReceiveCount_;
}

std::string NodeConnection::getDigest(const char8_t *password) {
	char digest[SHA256_DIGEST_STRING_LENGTH + 1];
	SHA256_Data(
			reinterpret_cast<const uint8_t*>(password),
			strlen(password), digest);
	digest[SHA256_DIGEST_STRING_LENGTH] = '\0';

	return digest;
}

int32_t NodeConnection::statementToNumber(Statement::Id statement) {
	return statement + getStatementNumberOffset();
}

int32_t NodeConnection::getStatementNumberOffset() {
	if (protocolVersion_ < 2) {
		return 0;
	}

	return STATEMENT_TYPE_NUMBER_V2_OFFSET;
}

bool NodeConnection::isDatabaseEnabled() {
	return getProtocolVersion() >= 6;
}

bool NodeConnection::isIPV6Enabled() const {
	return ClientUtil::isIPV6Address(address_);
}

void NodeConnection::readFully(void *buf, size_t length) {
	size_t r = length;
	try {
		do {
			const int64_t last = socket_.receive(
					static_cast<uint8_t*>(buf) + length - r, r);

			if (last <= 0) {
				if (last == 0) {
					GS_CLIENT_THROW_CONNECTION(
							GS_ERROR_CC_BAD_CONNECTION,
							"Connection unexpectedly terminated (address=" <<
							address_ << ")");
				}
				else {
					if (r < length) {
						responseUnacceptable_ = true;
					}

					GS_CLIENT_THROW_CONNECTION(
							GS_ERROR_CC_CONNECTION_TIMEOUT,
							"Connection timed out on receiving ("
							"receivedSize=" << (length - r) <<
							", totalSize=" << length <<
							", ioResult=" << last <<
							"address=" << address_ << ")");
				}
			}
			r -= static_cast<size_t>(last);
		}
		while (r > 0);
	}
	catch (ConnectionException&) {
		throw;
	}
	catch (std::exception &e) {
		try {
			throw;
		}
		catch (util::PlatformException &e2) {
			const bool timeoutOccurred =
					util::Socket::isTimeoutError(e2.getErrorCode());
			if (!timeoutOccurred || r < length) {
				responseUnacceptable_ = true;
			}

			if (timeoutOccurred) {
				GS_CLIENT_RETHROW_CONNECTION(
						e2,
						"Connection timed out on receiving ("
						"receivedSize=" << (length - r) <<
						", totalSize=" << length <<
						", address=" << address_ <<
						", reason=" << ErrorReasonFormatter(e2) << ")");
			}
		}
		GS_CLIENT_RETHROW_CONNECTION(
				e,
				"Connection problem occurred on receiving ("
				"receivedSize=" << (length - r) <<
				", totalSize=" << length <<
				", address=" << address_ <<
				", reason=" << ErrorReasonFormatter(e) << ")");
	}
}

util::NormalXArray<uint8_t>* NodeConnection::processHeartbeat(
		int32_t partitionId, util::NormalXArray<uint8_t> &resp,
		util::NormalXArray<uint8_t> &heartbeatBuf, Heartbeat &heartbeat) {

	const size_t eeHeadLength = getEEHeadLength(isIPV6Enabled());
	util::Stopwatch watch;
	const int64_t initialMillis =
			std::min(statementTimeoutMillis_, heartbeatTimeoutMillis_);
	int64_t lastHeartbeatId;
	for (;;) {
		const int64_t elapsedMillis = watch.elapsedMillis() + initialMillis;
		if (elapsedMillis >= statementTimeoutMillis_) {
			GS_CLIENT_THROW_CONNECTION(
					GS_ERROR_CC_CONNECTION_TIMEOUT,
					"Connection timed out by statement timeout ("
					"elapsedMillis=" << elapsedMillis <<
					", statementTimeoutMillis=" << statementTimeoutMillis_ <<
					", address=" << address_ << ")");
		}

		while (++statementId_ == 0) {
		}
		lastHeartbeatId = statementId_;

		fillRequestHead(isIPV6Enabled(), heartbeatBuf, true);
		XArrayByteOutStream reqOut =
				XArrayByteOutStream(NormalXArrayOutStream(heartbeatBuf));
		putConnectRequest(reqOut);

		ArrayByteInStream respIn =
				ArrayByteInStream(util::ArrayInStream(NULL, 0));
		try {
			respIn = executeStatement(
					Statement::CONNECT, partitionId, lastHeartbeatId,
					heartbeatBuf, &resp, &heartbeat);
		}
		catch (StatementException &e) {
			heartbeat.orgException_.reset(new StatementException(e));
		}

		heartbeatReceiveCount_++;

		if (heartbeat.orgStatementFound_) {
			heartbeatBuf.resize(eeHeadLength);
			readFully(heartbeatBuf.data(), eeHeadLength);

			heartbeat.orgStatementTypeNumber_ =
					statementToNumber(Statement::CONNECT);
			heartbeat.orgStatementId_ = lastHeartbeatId;
			heartbeat.orgRespPos_ = respIn.base().position();
			heartbeat.orgRespSize_ =
					respIn.base().position() + respIn.base().remaining();

			return &heartbeatBuf;
		}

		resp.resize(eeHeadLength);
		try {
			readFully(resp.data(), eeHeadLength);
			return &resp;
		}
		catch (ConnectionException &e) {
			if (responseUnacceptable_) {
				GS_CLIENT_RETHROW_CONNECTION(
						e,
						"Connection problem occurred after heartbeat (" <<
						"elapsedMillis=" << elapsedMillis <<
						", address=" << address_ <<
						", reason=" << ErrorReasonFormatter(e) << ")");
			}
			continue;
		}
	}
}

void NodeConnection::putConnectRequest(XArrayByteOutStream &reqOut) {
	reqOut.base().position(NodeConnection::getRequestHeadLength(
				isIPV6Enabled(), true));
	reqOut << (alternativeVersionEnabled_ ?
			alternativeVersion_ : getProtocolVersion());
}

void NodeConnection::acceptConnectResponse(ArrayByteInStream &respIn) {
	authMode_ = Auth::Challenge::getMode(respIn);

	if (respIn.base().remaining() > 0) {
		int32_t version;
		respIn >> version;
		if (version == 0 || (remoteProtocolVersion_ != 0 &&
				version != remoteProtocolVersion_)) {
			GS_CLIENT_THROW_CONNECTION(
					GS_ERROR_CC_MESSAGE_CORRUPTED,
					"Protocol error by illegal remote version (version=" <<
					version << ")");
		}
	}
}

bool NodeConnection::isStatementIdLarge(bool firstStatement) {
	return !firstStatement && protocolVersion_ >= 3;
}

int64_t NodeConnection::getStatementId(
		ArrayByteInStream &respIn, bool firstStatement) {
	if (isStatementIdLarge(firstStatement)) {
		int64_t statementId;
		respIn >> statementId;
		return statementId;
	}
	else {
		int32_t statementId;
		respIn >> statementId;
		return statementId;
	}
}


NodeConnection::ClientId::ClientId() :
		sessionId_(0),
		generated_(false) {
	memset(uuid_, 0, sizeof(uuid_t));
}

NodeConnection::ClientId::ClientId(const uuid_t &uuid, int64_t sessionId) :
		sessionId_(sessionId),
		generated_(true) {
	memcpy(uuid_, uuid, sizeof(uuid_t));
}

NodeConnection::ClientId::ClientId(const ClientId &another) :
		sessionId_(another.sessionId_),
		generated_(another.generated_) {
	memcpy(uuid_, another.uuid_, sizeof(uuid_t));
}

NodeConnection::ClientId& NodeConnection::ClientId::operator=(
		const ClientId &another) {
	if (this != &another) {
		memcpy(uuid_, another.uuid_, sizeof(uuid_t));
		sessionId_ = another.sessionId_;
		generated_ = another.generated_;
	}

	return *this;
}

void NodeConnection::ClientId::generate(int64_t sessionId) {
	uuid_generate(uuid_);
	sessionId_ = sessionId;
	generated_ = true;
}

bool NodeConnection::ClientId::isEmpty() const {
	return !generated_;
}

int64_t NodeConnection::ClientId::getSessionId() const {
	assert(generated_);
	return sessionId_;
}

const uuid_t& NodeConnection::ClientId::getUUID() const {
	assert(generated_);
	return uuid_;
}


const int32_t NodeConnection::Config::CONNECT_TIMEOUT_DEFAULT = 10 * 1000;
const int32_t NodeConnection::Config::STATEMENT_TIMEOUT_DEFAULT = 15 * 1000;
const int32_t NodeConnection::Config::HEARTBEAT_TIMEOUT_DEFAULT = 10 * 1000;

NodeConnection::Config::Config() :
		connectTimeoutMillis_(CONNECT_TIMEOUT_DEFAULT),
		statementTimeoutMillis_(STATEMENT_TIMEOUT_DEFAULT),
		heartbeatTimeoutMillis_(HEARTBEAT_TIMEOUT_DEFAULT),
		statementTimeoutEnabled_(false),
		alternativeVersion_(-1),
		alternativeVersionEnabled_(false) {
}

bool NodeConnection::Config::set(const Properties &properties) {
	int64_t connectTimeoutMillis;
	properties.getTimeoutMillis(
			"connectTimeout", connectTimeoutMillis, connectTimeoutMillis_);
	int64_t statementTimeoutMillis;
	properties.getTimeoutMillis(
			"statementTimeout", statementTimeoutMillis, statementTimeoutMillis_);
	int64_t heartbeatTimeoutMillis;
	properties.getTimeoutMillis(
			"heartbeatTimeout", heartbeatTimeoutMillis, heartbeatTimeoutMillis_);
	bool statementTimeoutEnabled;
	if (!properties.getBool(
			"statementTimeoutEnabled", statementTimeoutEnabled)) {
		statementTimeoutEnabled = statementTimeoutEnabled_;
	}

	if (connectTimeoutMillis < 0 || statementTimeoutMillis < 0 ||
			heartbeatTimeoutMillis < 0) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PROPERTY_ENTRY, "");
	}

	if (connectTimeoutMillis != connectTimeoutMillis_ ||
			statementTimeoutMillis != statementTimeoutMillis_ ||
			heartbeatTimeoutMillis != heartbeatTimeoutMillis_ ||
			statementTimeoutEnabled != statementTimeoutEnabled_) {
		connectTimeoutMillis_ = connectTimeoutMillis;
		statementTimeoutMillis_ = statementTimeoutMillis;
		heartbeatTimeoutMillis_ = heartbeatTimeoutMillis;
		statementTimeoutEnabled_ = statementTimeoutEnabled;
		return true;
	}

	return false;
}

void NodeConnection::Config::setAlternativeVersion(int32_t alternativeVersion) {
	alternativeVersion_ = alternativeVersion;
	alternativeVersionEnabled_ = true;
}


NodeConnection::LoginInfo::LoginInfo(
		const GSChar *user, const GSChar *password, bool ownerMode,
		const GSChar *clusterName, const GSChar *dbName,
		int64_t transactionTimeoutMillis, const GSChar *applicationName,
		double storeMemoryAgingSwapRate) :
		user_(user),
		passwordDigest_(Auth::Challenge::makeDigest(alloc_, user, password)),
		database_(dbName),
		ownerMode_(ownerMode),
		clusterName_(clusterName),
		transactionTimeoutSecs_(
				Properties::timeoutToInt32Seconds(transactionTimeoutMillis)),
		applicationName_(applicationName),
		storeMemoryAgingSwapRate_(storeMemoryAgingSwapRate) {
}

void NodeConnection::LoginInfo::setPassword(const GSChar *password) {
	passwordDigest_ =
			Auth::Challenge::makeDigest(alloc_, user_.c_str(), password);
}


NodeConnection::OptionalRequest::OptionalRequest() :
		transactionTimeout_(-1),
		forUpdate_(false),
		containerLockRequired_(false),
		systemMode_(false),
		containerAttribute_(-1),
		rowInsertUpdate_(-1),
		statementTimeout_(-1),
		fetchLimit_(-1),
		fetchSize_(-1),
		fetchBytesSize_(0),
		storeMemoryAgingSwapRate_(-1) {
}

NodeConnection::OptionalRequest::OptionalRequest(
		int32_t statementTimeout, int64_t fetchLimit, int64_t fetchSize,
		const std::string &dbName) :
		transactionTimeout_(-1),
		forUpdate_(false),
		containerLockRequired_(false),
		systemMode_(false),
		dbName_(dbName),
		containerAttribute_(-1),
		rowInsertUpdate_(-1),
		statementTimeout_(statementTimeout),
		fetchLimit_(fetchLimit),
		fetchSize_(fetchSize),
		fetchBytesSize_(0),
		storeMemoryAgingSwapRate_(-1) {
}

void NodeConnection::OptionalRequest::putExt(
		int32_t type, const void *value, size_t valueSize) {
	if (extRequestMap_.get() == NULL) {
		extRequestMap_.reset(new ExtMap);
	}

	const uint8_t *begin = static_cast<const ExtValue::value_type*>(value);
	const uint8_t *end = begin + valueSize;

	(*extRequestMap_)[type].assign(begin, end);
}

void NodeConnection::OptionalRequest::format(
		XArrayByteOutStream &reqOut) const {
	Formatter formatter(reqOut);

	if (transactionTimeout_ != -1) {
		formatter.putType(TYPE_TRANSACTION_TIMEOUT);
		reqOut << transactionTimeout_;
	}

	if (forUpdate_) {
		formatter.putType(FOR_UPDATE);
		reqOut << ClientUtil::toGSBool(forUpdate_);
	}

	if (containerLockRequired_) {
		formatter.putType(CONTAINER_LOCK_REQUIRED);
		reqOut << ClientUtil::toGSBool(containerLockRequired_);
	}

	if (isDatabaseEnabled()) {
		if (systemMode_) {
			formatter.putType(SYSTEM_MODE);
			reqOut << ClientUtil::toGSBool(systemMode_);
		}

		if (!dbName_.empty()) {
			formatter.putType(DB_NAME);
			reqOut << dbName_;
		}

		if (containerAttribute_ != -1) {
			formatter.putType(CONTAINER_ATTRIBUTE);
			reqOut << containerAttribute_;
		}

		if (rowInsertUpdate_ != -1) {
			formatter.putType(ROW_INSERT_UPDATE);
			reqOut << rowInsertUpdate_;
		}
	}

	if (statementTimeout_ != -1) {
		formatter.putType(STATEMENT_TIMEOUT);
		reqOut << statementTimeout_;
	}

	if (fetchLimit_ != -1) {
		formatter.putType(FETCH_LIMIT);
		reqOut << fetchLimit_;
	}

	if (fetchSize_ != -1) {
		formatter.putType(FETCH_SIZE);
		reqOut << fetchSize_;
	}

	if (!clientId_.isEmpty()) {
		formatter.putType(CLIENT_ID);
		reqOut << clientId_.getSessionId();
		reqOut.writeAll(clientId_.getUUID(), sizeof(uuid_t));
	}

	if (fetchBytesSize_ > 0) {
		formatter.putType(FETCH_BYTES_SIZE);
		reqOut << fetchBytesSize_;
	}

	if (!applicationName_.empty()) {
		formatter.putType(APPLICATION_NAME);
		reqOut << applicationName_;
	}

	if (storeMemoryAgingSwapRate_ >= 0) {
		formatter.putType(STORE_MEMORY_AGING_SWAP_RATE);
		reqOut << storeMemoryAgingSwapRate_;
	}

	if (extRequestMap_.get() != NULL) {
		for (ExtMap::const_iterator it = extRequestMap_->begin();
				it != extRequestMap_->end(); ++it) {
			formatter.putType(it->first);

			const ExtValue &value = it->second;
			if (!value.empty()) {
				reqOut.base().writeAll(&value[0], value.size());
			}
		}
	}

	formatter.close();
}


const int32_t NodeConnection::OptionalRequest::Formatter::RANGE_SIZE = 1000;
const int32_t NodeConnection::OptionalRequest::Formatter::RANGE_START_ID =
		RANGE_SIZE * 11;

NodeConnection::OptionalRequest::Formatter::Formatter(
		XArrayByteOutStream &reqOut) :
		reqOut_(reqOut),
		lastRangeId_(0),
		headPos_(std::numeric_limits<size_t>::max()),
		bodyPos_(std::numeric_limits<size_t>::max()),
		rangeHeadPos_(std::numeric_limits<size_t>::max()),
		rangeBodyPos_(std::numeric_limits<size_t>::max()) {
	headPos_ = reqOut_.base().position();
	reqOut_ << ClientUtil::sizeValueToInt32(0);
	bodyPos_ = reqOut_.base().position();
}

void NodeConnection::OptionalRequest::Formatter::putType(int32_t type) {
	const int32_t id = type;
	if (id > RANGE_START_ID) {
		const int32_t rangeId = id / RANGE_SIZE;
		if (rangeId != lastRangeId_) {
			if (lastRangeId_ != 0) {
				putBodySize(reqOut_, rangeHeadPos_, rangeBodyPos_);
			}

			reqOut_ << static_cast<int16_t>(rangeId * RANGE_SIZE);

			rangeHeadPos_ = reqOut_.base().position();
			reqOut_ << ClientUtil::sizeValueToInt32(0);
			rangeBodyPos_ = reqOut_.base().position();

			lastRangeId_ = rangeId;
		}
	}

	reqOut_ << static_cast<int16_t>(id);
}

void NodeConnection::OptionalRequest::Formatter::close() {
	if (lastRangeId_ != 0) {
		putBodySize(reqOut_, rangeHeadPos_, rangeBodyPos_);
	}

	putBodySize(reqOut_, headPos_, bodyPos_);
}

void NodeConnection::OptionalRequest::Formatter::putBodySize(
		XArrayByteOutStream &reqOut, size_t headPos, size_t bodyPos) {
	const size_t endPos = reqOut.base().position();
	reqOut.base().position(headPos);
	reqOut << ClientUtil::sizeValueToInt32(endPos - bodyPos);
	reqOut.base().position(endPos);
}


const size_t NodeConnectionPool::DEFAULT_MAX_SIZE = 16;

NodeConnectionPool::NodeConnectionPool() : maxSize_(DEFAULT_MAX_SIZE) {
}

NodeConnectionPool::~NodeConnectionPool() {
	try {
		clear();
	}
	catch (...) {
	}
}

size_t NodeConnectionPool::getMaxSize() {
	util::LockGuard<util::Mutex> guard(mutex_);
	return maxSize_;
}

void NodeConnectionPool::setMaxSize(size_t maxSize) {
	{
		util::LockGuard<util::Mutex> guard(mutex_);
		adjustSize(maxSize);
		maxSize_ = maxSize;
	}
	closeExceededConnections();
}

void NodeConnectionPool::add(std::auto_ptr<NodeConnection> connection) {
	{
		util::LockGuard<util::Mutex> guard(mutex_);

		addressQueue_.push_back(connection->getRemoteAddress());

		ConnectionMap::iterator it =
				connectionMap_.find(connection->getRemoteAddress());
		if (it == connectionMap_.end()) {
			it = connectionMap_.insert(std::make_pair(
					connection->getRemoteAddress(),
					std::vector<NodeConnection*>())).first;
		}
		std::vector<NodeConnection*> &list = it->second;
		list.push_back(connection.get());
		connection.release();

		adjustSize(maxSize_);
	}
	closeExceededConnections();
}

std::auto_ptr<NodeConnection> NodeConnectionPool::pull(
		const util::SocketAddress &address) {
	util::LockGuard<util::Mutex> guard(mutex_);

	std::binder2nd<SocketAddressEqual> pred =
			std::bind2nd(SocketAddressEqual(), address);

	AddressQueue::iterator addressIt =
			std::find_if(addressQueue_.begin(), addressQueue_.end(), pred);
	if (addressIt == addressQueue_.end()) {
		return std::auto_ptr<NodeConnection>(NULL);
	}
	addressQueue_.erase(addressIt);

	ConnectionMap::iterator connectionIt = connectionMap_.find(address);
	if (connectionIt == connectionMap_.end()) {
		return std::auto_ptr<NodeConnection>(NULL);
	}

	std::vector<NodeConnection*> &list = connectionIt->second;
	if (list.empty()) {
		connectionMap_.erase(connectionIt);
		return std::auto_ptr<NodeConnection>(NULL);
	}

	std::auto_ptr<NodeConnection> connection(list.back());
	list.pop_back();
	if (list.empty()) {
		connectionMap_.erase(connectionIt);
	}

	return connection;
}

std::auto_ptr<NodeConnection> NodeConnectionPool::resolve(
		const util::SocketAddress &address,
		util::NormalXArray<uint8_t> &req,
		util::NormalXArray<uint8_t> &resp,
		const NodeConnection::Config &config,
		const NodeConnection::LoginInfo &loginInfo,
		int64_t *databaseId,
		bool preferCache) {
	std::auto_ptr<NodeConnection> connection;
	if (preferCache) {
		connection = pull(address);
	}

	if (connection.get() == NULL) {
		connection.reset(new NodeConnection(address, config));
		connection->connect(req, resp);
	}

	connection->login(req, resp, loginInfo, databaseId);

	return connection;
}

void NodeConnectionPool::clear() {
	{
		util::LockGuard<util::Mutex> guard(mutex_);

		for (ConnectionMap::iterator i = connectionMap_.begin();
				i != connectionMap_.end(); ++i) {
			std::vector<NodeConnection*> &list = i->second;
			while (!list.empty()) {
				exceededConnectionList_.push_back(list.back());
				list.pop_back();
			}
		}
		connectionMap_.clear();
		addressQueue_.clear();
	}
	closeExceededConnections();
}

void NodeConnectionPool::adjustSize(size_t maxSize) {
	while (addressQueue_.size() > maxSize) {
		const util::SocketAddress address = addressQueue_.front();
		addressQueue_.pop_front();

		ConnectionMap::iterator it = connectionMap_.find(address);
		if (it == connectionMap_.end()) {
			continue;
		}

		std::vector<NodeConnection*> &list = it->second;
		if (list.empty()) {
			connectionMap_.erase(it);
			continue;
		}

		exceededConnectionList_.push_back(list.back());
		list.pop_back();
		if (list.empty()) {
			connectionMap_.erase(it);
		}
	}
}

void NodeConnectionPool::closeExceededConnections() {
	std::vector<NodeConnection*> connectionList;
	{
		util::LockGuard<util::Mutex> guard(mutex_);
		connectionList.swap(exceededConnectionList_);
	}

	if (!connectionList.empty()) {
		do {
			delete connectionList.back();
			connectionList.pop_back();
		}
		while (!connectionList.empty());

		util::LockGuard<util::Mutex> guard(mutex_);
		connectionList.swap(exceededConnectionList_);
	}
}


const bool NodeResolver::IPV6_ENABLED_DEFAULT = false;

const int32_t NodeResolver::NOTIFICATION_RECEIVE_TIMEOUT = 10 * 1000;
const int32_t NodeResolver::PROVIDER_UPDATE_TIMEOUT = 60 * 1000;
const int32_t NodeResolver::NOTIFICATION_STATEMENT_TYPE_V1 = 5000;
const int32_t NodeResolver::NOTIFICATION_STATEMENT_TYPE = 5000;

const int32_t NodeResolver::DEFAULT_NOTIFICATION_PORT = 31999;
const GSChar *const NodeResolver::DEFAULT_NOTIFICATION_ADDRESS =
		"239.0.0.1";
const GSChar *const NodeResolver::DEFAULT_NOTIFICATION_ADDRESS_V6 =
		"ff12::1";
const GSChar *const NodeResolver::DEFAULT_SERVICE_TYPE = "transaction";

NodeResolver::AddressConfig NodeResolver::DEFAULT_ADDRESS_CONFIG;
NodeResolver::DefaultProtocolConfig NodeResolver::DEFAULT_PROTOCOL_CONFIG;

NodeResolver::NodeResolver(NodeConnectionPool &pool, bool passive,
		const util::SocketAddress &address,
		const NodeConnection::Config &connectionConfig,
		const ServiceAddressResolver::Config &sarConfig,
		const std::vector<util::SocketAddress> &memberList,
		const AddressConfig &addressConfig) :
		pool_(pool),
		ipv6Enabled_(address.getFamily() == util::SocketAddress::FAMILY_INET6),
		notificationAddress_(passive ? address : util::SocketAddress()),
		masterAddress_(passive ? util::SocketAddress() : address),
		connectionConfig_(connectionConfig),
		notificationReceiveTimeoutMillis_(NOTIFICATION_RECEIVE_TIMEOUT),
		providerTimeoutMillis_(-1),
		masterCacheCounter_(1),
		connectionTrialCounter_(0),
		connectionFailedPreviously_(false),
		preferableConnectionPoolSize_(pool.getMaxSize()),
		serviceAddressResolver_(alloc_, sarConfig),
		providerCheckInterval_(100, 1),
		lastSelectedMember_(-1),
		alwaysMaster_(addressConfig.alwaysMaster_),
		protocolConfig_(&DEFAULT_PROTOCOL_CONFIG) {
	makeServiceAddressResolver(
			serviceAddressResolver_, sarConfig, memberList, addressConfig);
}

void NodeResolver::setConnectionConfig(
		const NodeConnection::Config &connectionConfig) {
	util::LockGuard<util::Mutex> guard(mutex_);
	connectionConfig_ = connectionConfig;
}

void NodeResolver::setNotificationReceiveTimeoutMillis(int64_t timeout) {
	util::LockGuard<util::Mutex> guard(mutex_);
	notificationReceiveTimeoutMillis_ = timeout;
}

void NodeResolver::setProviderTimeoutMillis(int64_t timeout) {
	util::LockGuard<util::Mutex> guard(mutex_);
	providerTimeoutMillis_ = timeout;
}

void NodeResolver::setPreferableConnectionPoolSize(int32_t size) {
	if (size >= 0 && preferableConnectionPoolSize_ != ClientUtil::toSizeValue(size)) {
		preferableConnectionPoolSize_ = ClientUtil::toSizeValue(size);
		updateConnectionPoolSize();
	}
}

void NodeResolver::setProtocolConfig(ProtocolConfig *protocolConfig) {
	util::LockGuard<util::Mutex> guard(mutex_);
	protocolConfig_ = protocolConfig;
}

void NodeResolver::close() {
	util::LockGuard<util::Mutex> guard(mutex_);
	releaseMasterCache(false);
}

util::SocketAddress NodeResolver::getAddressProperties(
		const Properties &props, bool *passive,
		ServiceAddressResolver::Config &sarConfig,
		std::vector<util::SocketAddress> &memberList,
		const AddressConfig *addressConfig) {
	assert(passive != NULL);
	*passive = false;

	if (addressConfig == NULL) {
		return getAddressProperties(
				props, passive, sarConfig, memberList,
				&DEFAULT_ADDRESS_CONFIG);
	}

	const GSChar *host = props.getString("host");
	*passive = (host == NULL);

	const GSChar *ipProtocol = props.getString("ipProtocol");
	bool ipv6Enabled;
	const bool *ipv6EnabledPtr = NULL;
	if (ipProtocol != NULL) {
		if (strcmp(ipProtocol, "IPV6") == 0) {
			ipv6Enabled = true;
		}
		else if (strcmp(ipProtocol, "IPV4") == 0) {
			ipv6Enabled = false;
		}
		else {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
					"Illegal IP type");
		}
		ipv6EnabledPtr = &ipv6Enabled;
	}

	util::SocketAddress address = getNotificationProperties(
			props, host, ipv6EnabledPtr, sarConfig, memberList,
			*addressConfig);

	const GSChar *portKey = (*passive ? "notificationPort" : "port");
	int32_t port;
	bool portAssigned = props.getInteger(portKey, port);
	if (!portAssigned) {
		if (*passive) {
			port = addressConfig->notificationPort_;
			portAssigned = true;
		}
		else if (!address.isEmpty()) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
					"Port must be specified");
		}
	}

	if (portAssigned && (port < 0 || port > static_cast<int32_t>(
			std::numeric_limits<uint16_t>::max()))) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
				"Port out of range (" << port <<
				", propertyName=" << portKey << ")");
	}

	if (!address.isEmpty()) {
		address.setPort(static_cast<uint16_t>(port));
	}

	return address;
}

util::SocketAddress NodeResolver::getNotificationProperties(
		const Properties &props, const GSChar *host,
		const bool *ipv6Expected,
		ServiceAddressResolver::Config &sarConfig,
		std::vector<util::SocketAddress> &memberList,
		const AddressConfig &addressConfig) {

	props.checkExclusiveProperties(
			"notificationProvider",
			"notificationMember",
			"notificationAddress");
	props.checkExclusiveProperties("notificationProvider", "host");
	props.checkExclusiveProperties("notificationMember", "host");

	const GSChar *const notificationProvider =
			props.getString("notificationProvider");
	const GSChar *const notificationMember =
			props.getString("notificationMember");
	const GSChar *const notificationAddress =
			props.getString("notificationAddress");

	if (notificationProvider != NULL) {
		sarConfig.providerURL_ = notificationProvider;
	}

	if (ipv6Expected != NULL && *ipv6Expected) {
		sarConfig.addressFamily_ = util::SocketAddress::FAMILY_INET6;
	}

	parseNotificationMember(notificationMember, ipv6Expected, memberList);

	if (notificationProvider != NULL || notificationMember != NULL) {
		return util::SocketAddress();
	}

	if (host == NULL) {
		return getNotificationAddress(
				notificationAddress, ipv6Expected, addressConfig,
				"notificationAddress");
	}
	else {
		return getNotificationAddress(
				host, ipv6Expected, addressConfig, "host");
	}
}

util::SocketAddress NodeResolver::getNotificationAddress(
		const GSChar *host, const bool *ipv6Expected,
		const AddressConfig &config, const GSChar *key) {

	if (host == NULL) {
		const GSChar * alternative;
		if (getAddressFamily(ipv6Expected) ==
				util::SocketAddress::FAMILY_INET6) {
			alternative = config.notificationAddressV6_;
		}
		else {
			alternative = config.notificationAddress_;
		}
		return resolveAddress(alternative, ipv6Expected, key);
	}
	else {
		return resolveAddress(host, ipv6Expected, key);
	}
}

void NodeResolver::parseNotificationMember(
		const GSChar *value, const bool *ipv6Expected,
		std::vector<util::SocketAddress> &memberList) {
	if (value == NULL) {
		return;
	}

	const GSChar *end = value + strlen(value);
	if (value == end) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER,
				"Notification member is empty");
	}

	for (const GSChar *it = value;;) {
		const GSChar *delim = strchr(it, ',');
		if (delim == NULL) {
			delim = end;
		}

		if (it == delim) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
					"One or more elements in notification member "
					"are empty (" <<
					"element=" << std::string(it, delim) <<
					", list=" << value << ")");
		}

		const GSChar *addrSep = findLastChar(it, delim, ':');
		const GSChar *v6AddrEnd = findLastChar(it, delim, ']');
		if (addrSep == NULL ||
				(v6AddrEnd != NULL && addrSep < v6AddrEnd)) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
					"Port not found in notification member ("
					"element=" << std::string(it, delim) <<
					", list=" << value << ")");
		}

		int32_t port;
		try {
			port = util::LexicalConverter<int32_t>()(
					std::string(&addrSep[1], delim));
		}
		catch (util::Exception &e) {
			GS_CLIENT_RETHROW_ERROR(
					e,
					"Failed to parse port in notification member ("
					"element=" << std::string(it, delim) <<
					", list=" << value << ")");
		}

		if (port < 0 || port > static_cast<int32_t>(
				std::numeric_limits<uint16_t>::max())) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
					"Illegal port in notification member ("
					"element=" << std::string(it, delim) <<
					", list=" << value << ")");
		}

		util::SocketAddress addr;
		try {
			addr = resolveAddress(
					std::string(it, addrSep).c_str(), ipv6Expected, NULL);
		}
		catch (util::Exception &e) {
			GS_CLIENT_RETHROW_ERROR(
					e,
					"Failed to resolve host in notification member ("
					"element=" << std::string(it, delim) <<
					", list=" << value << ")");
		}

		addr.setPort(static_cast<uint16_t>(port));
		memberList.push_back(addr);

		if (delim == end) {
			break;
		}
		it = &delim[1];
	}
}

util::SocketAddress NodeResolver::resolveAddress(
		const GSChar *host, const bool *ipv6Expected, const GSChar *key) {
	if (strlen(host) == 0) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER,
				"Empty host name or address specified" <<
				(key == NULL ? "" : " (propertyName=") <<
				(key == NULL ? "" : key) <<
				(key == NULL ? "" : ")"));
	}

	const int family = getAddressFamily(ipv6Expected);
	try {
		const uint16_t port = 0;
		return util::SocketAddress(host, port, family);
	}
	catch (util::Exception &e) {
		GS_CLIENT_RETHROW_ERROR(
				e,
				"Address not resolved (value=" << host <<
				(key == NULL ? "" : ", propertyName=") <<
				(key == NULL ? "" : key) << ")");
	}
}

int NodeResolver::getAddressFamily(const bool *ipv6Expected) {
	if ((ipv6Expected == NULL && IPV6_ENABLED_DEFAULT) ||
			(ipv6Expected != NULL && *ipv6Expected)) {
		return util::SocketAddress::FAMILY_INET6;
	}
	else {
		return util::SocketAddress::FAMILY_INET;
	}
}

int32_t NodeResolver::getPartitionCount(ClusterInfo &clusterInfo) {
	if (clusterInfo.getPartitionCount() == NULL) {
		const size_t startTrialCount = connectionTrialCounter_;

		util::LockGuard<util::Mutex> guard(mutex_);
		prepareConnectionAndClusterInfo(clusterInfo, startTrialCount);
		assert(clusterInfo.getPartitionCount() != NULL);

		applyMasterCacheCounter(clusterInfo);
	}

	return *clusterInfo.getPartitionCount();
}

ContainerHashMode::Id NodeResolver::getContainerHashMode(
		ClusterInfo &clusterInfo) {
	if (clusterInfo.getHashMode() == NULL) {
		const size_t startTrialCount = connectionTrialCounter_;

		util::LockGuard<util::Mutex> guard(mutex_);
		prepareConnectionAndClusterInfo(clusterInfo, startTrialCount);
		assert(clusterInfo.getHashMode() != NULL);

		applyMasterCacheCounter(clusterInfo);
	}

	return *clusterInfo.getHashMode();
}

int64_t NodeResolver::getDatabaseId(ClusterInfo &clusterInfo) {
	if (clusterInfo.getDatabaseId() == NULL) {
		const size_t startTrialCount = connectionTrialCounter_;

		util::LockGuard<util::Mutex> guard(mutex_);
		prepareConnectionAndClusterInfo(clusterInfo, startTrialCount);
		assert(clusterInfo.getDatabaseId() != NULL);

		applyMasterCacheCounter(clusterInfo);
	}

	return *clusterInfo.getDatabaseId();
}

void NodeResolver::acceptDatabaseId(
		ClusterInfo &clusterInfo, int64_t databaseId,
		const util::SocketAddress &address) {
	const bool byConnection = true;
	acceptClusterInfo(
			clusterInfo, NULL, NULL, &databaseId, address, byConnection);
}

util::SocketAddress NodeResolver::getMasterAddress(ClusterInfo &clusterInfo) {
	const size_t startTrialCount = connectionTrialCounter_;
	util::LockGuard<util::Mutex> guard(mutex_);

	if (masterAddress_.isEmpty()) {
		prepareConnectionAndClusterInfo(clusterInfo, startTrialCount);
		assert(!masterAddress_.isEmpty());
	}

	applyMasterCacheCounter(clusterInfo);
	return masterAddress_;
}

util::SocketAddress NodeResolver::getNodeAddress(
		ClusterInfo &clusterInfo, int32_t partitionId, bool backupPreferred,
		const util::SocketAddress *preferableHost) {
	const size_t startTrialCount = connectionTrialCounter_;
	util::LockGuard<util::Mutex> guard(mutex_);

	const NodeAddressList *addressList = getNodeAddressList(
			clusterInfo, partitionId, backupPreferred, startTrialCount, false);
	assert(addressList != NULL);

	const int32_t backupCount = static_cast<int32_t>(addressList->size()) - 1;
	int32_t index;
	if (backupPreferred && backupCount > 0) {
		const int32_t ownerCount = 1;
		index = ownerCount + random_.nextInt32(backupCount);

		if (preferableHost != NULL && !preferableHost->isEmpty()) {
			std::string hostName1;
			std::string hostName2;

			if (!(*addressList)[index].isEmpty()) {
				(*addressList)[index].getIP(&hostName1);
				preferableHost->getIP(&hostName2);
			}

			if (hostName1 != hostName2) {
				for (size_t index = static_cast<size_t>(ownerCount);
						index < addressList->size(); index++) {

					if (!(*addressList)[index].isEmpty()) {

						(*addressList)[index].getIP(&hostName1);
						preferableHost->getIP(&hostName2);

						if (hostName1 == hostName2) {
							break;
						}
					}
				}
			}
		}
	}
	else {
		index = 0;
	}

	if ((*addressList)[index].isEmpty()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "Empty address");
	}

	applyMasterCacheCounter(clusterInfo);
	return (*addressList)[index];
}

void NodeResolver::getNodeAddressList(
		ClusterInfo &clusterInfo, int32_t partitionId,
		std::vector<util::SocketAddress> &addressList) {
	addressList.clear();

	const size_t startTrialCount = connectionTrialCounter_;
	util::LockGuard<util::Mutex> guard(mutex_);

	const NodeAddressList *baseAddressList = getNodeAddressList(
			clusterInfo, partitionId, true, startTrialCount, false);
	if (baseAddressList != NULL) {
		addressList = *baseAddressList;
	}

	applyMasterCacheCounter(clusterInfo);
}

void NodeResolver::invalidateMaster(ClusterInfo &clusterInfo) {
	util::LockGuard<util::Mutex> guard(mutex_);
	invalidateMasterInternal(clusterInfo, false);
}

void NodeResolver::makeServiceAddressResolver(
		ServiceAddressResolver &resolver,
		const ServiceAddressResolver::Config &sarConfig,
		const std::vector<util::SocketAddress> &memberList,
		const AddressConfig &addressConfig) {
	if (sarConfig.providerURL_ == NULL && memberList.empty()) {
		return;
	}

	if (sarConfig.providerURL_ != NULL &&
			strlen(sarConfig.providerURL_) == 0) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER,
				"Notification provider is empty");
	}

	resolver.initializeType(0, addressConfig.serviceType_);

	if (sarConfig.providerURL_ == NULL) {
		for (size_t i = 0; i < memberList.size(); i++) {
			resolver.setAddress(i, 0, memberList[i]);
		}
		resolver.validate();
	}
}

void NodeResolver::prepareConnectionAndClusterInfo(
		ClusterInfo &clusterInfo, size_t startTrialCount) {
	try {
		if (connectionFailedPreviously_ &&
				startTrialCount != connectionTrialCounter_) {
			GS_CLIENT_THROW_CONNECTION(
					GS_ERROR_CC_CONNECTION_TIMEOUT,
					"Previously failed in the other thread");
		}

		if (masterAddress_.isEmpty()) {
			connectionTrialCounter_++;
			if (notificationAddress_.isEmpty()) {
				updateNotificationMember();
			}
			else {
				updateMasterInfo(clusterInfo);
			}
		}

		while (!updateConnectionAndClusterInfo(clusterInfo)) {
			connectionTrialCounter_++;
		}

		connectionFailedPreviously_ = false;
	}
	catch (...) {
		bool statementError = false;
		try {
			throw;
		}
		catch (StatementException&) {
			statementError = true;
		}
		catch (...) {
		}

		const bool invalidated = clusterInfo.invalidate();
		if (invalidated || !statementError) {
			connectionFailedPreviously_ = true;
			invalidateMasterInternal(clusterInfo, true);
		}
		throw;
	}
}

void NodeResolver::updateNotificationMember() try {
	const GSChar *url = serviceAddressResolver_.getConfig().providerURL_;
	if (url == NULL) {
		return;
	}

	if (!serviceAddressResolver_.update()) {
		const uint32_t timeoutMillis = (providerTimeoutMillis_ > 0 ?
				static_cast<uint32_t>(std::min(
						providerTimeoutMillis_, static_cast<int64_t>(
								std::numeric_limits<uint32_t>::max()))) :
				PROVIDER_UPDATE_TIMEOUT);

		util::Stopwatch watch;
		watch.start();
		bool updated = false;
		do {
			util::Thread::sleep(providerCheckInterval_.first);
			for (;;) {
				size_t readSize;
				updated = serviceAddressResolver_.checkUpdated(&readSize);
				if (updated || readSize == 0) {
					break;
				}

				const uint32_t elapsedMillis = watch.elapsedMillis();
				if (elapsedMillis > timeoutMillis) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_BAD_CONNECTION,
							"Timeout exceeded while updating "
							"notification member ("
							"providerURL=" << url <<
							", elapsedMillis=" << elapsedMillis <<
							", timeoutMillis=" << timeoutMillis << ")");
				}

				util::Thread::sleep(providerCheckInterval_.second);
			}
		}
		while (!updated);
	}
	serviceAddressResolver_.validate();
}
catch (util::Exception &e) {
	if (serviceAddressResolver_.isAvailable()) {
		return;
	}
	GS_CLIENT_RETHROW_CONNECTION(e, "");
}

bool NodeResolver::updateConnectionAndClusterInfo(ClusterInfo &clusterInfo) {
	std::auto_ptr<NodeConnection> pendingConnection;
	try {
		util::SocketAddress address;
		if (masterAddress_.isEmpty()) {
			const int32_t count = static_cast<int32_t>(
					serviceAddressResolver_.getEntryCount());
			if (count <= 0) {
				const GSChar *url = serviceAddressResolver_.
						getConfig().providerURL_;
				GS_CLIENT_THROW_ERROR(GS_ERROR_SA_ADDRESS_NOT_ASSIGNED,
						"No address found in provider (url=" <<
						(url == NULL ? "" : url) << ")");
			}

			int32_t index = lastSelectedMember_;
			if (index < 0) {
				index = random_.nextInt32(count);
			}
			else if (++index >= count) {
				index = 0;
			}
			lastSelectedMember_ = index;

			address = serviceAddressResolver_.getAddress(
					static_cast<size_t>(index), 0);
			if (alwaysMaster_) {
				masterAddress_ = address;
			}
		}
		else {
			address = masterAddress_;
		}

		const bool masterUnresolved = masterAddress_.isEmpty();

		const bool masterResolvable =
				GridStoreChannel::isMasterResolvableByConnection();
		if (masterUnresolved && !masterResolvable) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
		}

		const NodeConnection::LoginInfo &loginInfo = clusterInfo.loginInfo_;
		int64_t databaseId;
		NodeConnection *connection = masterConnection_.get();
		if (connection == NULL) {
			pendingConnection = pool_.resolve(
					address, req_, resp_, connectionConfig_, loginInfo,
					&databaseId, !connectionFailedPreviously_);
			connection = pendingConnection.get();
		}
		else {
			connection->login(req_, resp_, loginInfo, &databaseId);
		}

		NodeConnection::fillRequestHead(ipv6Enabled_, req_);
		XArrayByteOutStream reqOut =
				XArrayByteOutStream(NormalXArrayOutStream(req_));
		NodeConnection::tryPutEmptyOptionalRequest(reqOut);

		if (masterResolvable) {
			reqOut << ClientUtil::toGSBool(true);
		}

		ArrayByteInStream respIn =
				connection->executeStatementDirect(
						protocolConfig_->getNormalStatementType(
								Statement::GET_PARTITION_ADDRESS),
						0, 0, req_, &resp_);

		int32_t partitionCount;
		respIn >> partitionCount;

		bool masterMatched;
		ContainerHashMode::Id hashMode;
		if (masterResolvable) {
			int8_t ownerCount;
			int8_t backupCount;
			respIn >> ownerCount;
			respIn >> backupCount;
			if (ownerCount != 0 || backupCount != 0 ||
					respIn.base().remaining() == 0) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
						"Protocol error by invalid master location");
			}

			GSBool masterMatchedValue;
			respIn >> masterMatchedValue;
			masterMatched = ClientUtil::toBool(masterMatchedValue);

			hashMode = decodeContainerHashMode(respIn);
			decodeSocketAddress(respIn, masterAddress_);
		}
		else {
			masterMatched = true;
			hashMode = GridStoreChannel::getDefaultContainerHashMode();
		}

		if (masterMatched && pendingConnection.get() != NULL) {
			masterConnection_.reset(pendingConnection.release());
		}

		if (masterConnection_.get() != NULL) {
			acceptClusterInfo(
					clusterInfo, &partitionCount, &hashMode, &databaseId,
					masterAddress_, true);
		}
	}
	catch (...) {
		if (pendingConnection.get() != NULL) {
			pool_.add(pendingConnection);
		}
		throw;
	}

	return (masterConnection_.get() != NULL);
}

void NodeResolver::updateMasterInfo(ClusterInfo &clusterInfo) try {
	util::Socket socket;
	const int family = ipv6Enabled_ ?
			util::SocketAddress::FAMILY_INET6 :
			util::SocketAddress::FAMILY_INET;
	socket.open(family, util::Socket::TYPE_DATAGRAM);
	socket.setReceiveTimeout(
			Properties::timeoutToInt32Millis(notificationReceiveTimeoutMillis_));
	socket.setReuseAddress(true);
	socket.bind(
			util::SocketAddress(NULL, notificationAddress_.getPort(), family));
	socket.joinMulticastGroup(notificationAddress_);

	resp_.resize(4 * 7 + getInetAddressSize() * 2 + 1);
	int64_t receivedSize;
	try {
		receivedSize = socket.receive(resp_.data(), resp_.size());
	}
	catch (std::exception &e) {
		GS_CLIENT_RETHROW_CONNECTION_CODED(GS_ERROR_CC_BAD_CONNECTION, e, "");
	}
	if (receivedSize != static_cast<int64_t>(resp_.size())) {
		if (receivedSize <= 0) {
			GS_CLIENT_THROW_CONNECTION(GS_ERROR_CC_CONNECTION_TIMEOUT,
					"Timeout for receiving multicast packet");
		}
		else {
			GS_CLIENT_THROW_CONNECTION(GS_ERROR_CC_MESSAGE_CORRUPTED,
					"Protocol error by invalid packet length");
		}
	}

	ArrayByteInStream respIn = ArrayByteInStream(
			util::ArrayInStream(resp_.data(), resp_.size()));
	respIn.base().position(4 * 4 + getInetAddressSize());

	int32_t statementType;
	respIn >> statementType;
	if (statementType != protocolConfig_->getNotificationStatementType()) {
		GS_CLIENT_THROW_CONNECTION(GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by illegal statement type");
	}

	util::SocketAddress masterAddress;
	if (ipv6Enabled_) {
		util::SocketAddress::Inet6 inetAddr;
		respIn.readAll(&inetAddr, sizeof(inetAddr));
		int32_t port;
		respIn >> port;
		masterAddress.assign(inetAddr, static_cast<uint16_t>(port));
	}
	else {
		util::SocketAddress::Inet inetAddr;
		respIn.readAll(&inetAddr, sizeof(inetAddr));
		int32_t port;
		respIn >> port;
		masterAddress.assign(inetAddr, static_cast<uint16_t>(port));
	}

	int32_t partitionCount;
	respIn >> partitionCount;
	if (partitionCount <= 0) {
		GS_CLIENT_THROW_CONNECTION(GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by negative partition count");
	}

	const ContainerHashMode::Id hashMode = decodeContainerHashMode(respIn);

	acceptClusterInfo(
			clusterInfo, &partitionCount, &hashMode, NULL,
			notificationAddress_, false);
	masterAddress_ = masterAddress;

	connectionFailedPreviously_ = false;
}
catch (...) {
	connectionFailedPreviously_ = true;
	throw;
}

void NodeResolver::invalidateMasterInternal(
		ClusterInfo &clusterInfo, bool inside) {

	clusterInfo.invalidate();
	if (!clusterInfo.acceptMasterInvalidation(masterCacheCounter_) &&
			!inside) {
		return;
	}

	if (!notificationAddress_.isEmpty() ||
			serviceAddressResolver_.getConfig().providerURL_ != NULL ||
			serviceAddressResolver_.getEntryCount() > 0) {
		masterAddress_.clear();
	}

	try {
		releaseMasterCache(true);
	}
	catch (...) {
	}

	updateConnectionPoolSize();

	while (++masterCacheCounter_ == 0) {
	}
}

void NodeResolver::releaseMasterCache(bool forceClose) {
	nodeAddressMap_.clear();
	cachedAddressSet_.clear();

	if (masterConnection_.get() != NULL) {
		std::auto_ptr<NodeConnection> connection(masterConnection_.release());

		if (forceClose) {
			connection.reset();
		}
		else {
			pool_.add(connection);
		}
	}
}

void NodeResolver::applyMasterCacheCounter(ClusterInfo &clusterInfo) {
	clusterInfo.lastMasterCacheCounter_ = masterCacheCounter_;
}

void NodeResolver::acceptClusterInfo(
		ClusterInfo &clusterInfo, const int32_t *partitionCount,
		const ContainerHashMode::Id *hashMode, const int64_t *databaseId,
		const util::SocketAddress &address, bool byConnection) {
	if (partitionCount != NULL && *partitionCount <= 0) {
		GS_CLIENT_THROW_CONNECTION(
				GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by non positive partition count ("
				"value=" << *partitionCount << ")");
	}

	if (databaseId != NULL && *databaseId < 0) {
		GS_CLIENT_THROW_CONNECTION(
				GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by negative database ID ("
				"value=" << *databaseId << ")");
	}

	for (bool checkOnly = true;; checkOnly = false) {
		acceptClusterInfoEntry(
				clusterInfo, clusterInfo.partitionCount_, "partition count",
				*partitionCount, address, byConnection, checkOnly);
		acceptClusterInfoEntry(
				clusterInfo, clusterInfo.hashMode_, "container hash mode",
				*hashMode, address, byConnection, checkOnly);
		acceptClusterInfoEntry(
				clusterInfo, clusterInfo.databaseId_, "partition count",
				*databaseId, address, byConnection, checkOnly);

		if (!checkOnly) {
			break;
		}
	}
}

template<typename T> void NodeResolver::acceptClusterInfoEntry(
		ClusterInfo &clusterInfo, ClusterInfoEntry<T> &entry,
		const char8_t *name, const T &value,
		const util::SocketAddress &address,
		bool byConnection, bool checkOnly) {
	if (entry.tryAccept(&value, address, byConnection, checkOnly)) {
		return;
	}

	static_cast<void>(name);

	if (static_cast<void*>(&entry) == &clusterInfo.partitionCount_) {
		if (byConnection) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARTITION_COUNT, "");
		}
		else {
			GS_CLIENT_THROW_CONNECTION(
					GS_ERROR_CC_ILLEGAL_PARTITION_COUNT, "");
		}
	}
	else {
		if (byConnection) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_CONFIG, "");
		}
		else {
			GS_CLIENT_THROW_CONNECTION(GS_ERROR_CC_ILLEGAL_CONFIG, "");
		}
	}
}

const NodeResolver::NodeAddressList* NodeResolver::getNodeAddressList(
		ClusterInfo &clusterInfo, int32_t partitionId, bool backupPreferred,
		size_t startTrialCount, bool allowEmpty) {
#if GS_CLIENT_ENABLE_NODE_ADDRESS_CACHE
	NodeAddressMap::const_iterator it = nodeAddressMap_.find(partitionId);
	if (it != nodeAddressMap_.end() && !it->second[0].isEmpty()) {
		return &it->second;
	}
#endif

	if (masterConnection_.get() == NULL ||
			clusterInfo.getPartitionCount() == NULL) {
		prepareConnectionAndClusterInfo(clusterInfo, startTrialCount);
	}

	NodeAddressList *addressList;
	NodeConnection::fillRequestHead(ipv6Enabled_, req_);
	try {
		XArrayByteOutStream reqOut =
				XArrayByteOutStream(NormalXArrayOutStream(req_));
		NodeConnection::tryPutEmptyOptionalRequest(reqOut);
		ArrayByteInStream respIn = masterConnection_->executeStatementDirect(
				protocolConfig_->getNormalStatementType(
						Statement::GET_PARTITION_ADDRESS),
				partitionId, 0, req_, &resp_);

		int32_t partitionCount;
		respIn >> partitionCount;
		acceptClusterInfo(
				clusterInfo, &partitionCount, NULL, NULL, masterAddress_,
				true);

		const size_t sockAddrSize = getInetAddressSize() + sizeof(int32_t);


		int8_t ownerCount;
		respIn >> ownerCount;
		const size_t ownerPosition = respIn.base().position();
		respIn.base().position(ownerPosition +
				sockAddrSize * static_cast<size_t>(ownerCount));

		int8_t backupCount;
		respIn >> backupCount;
		const size_t backupPosition = respIn.base().position();
		respIn.base().position(backupPosition +
				sockAddrSize * static_cast<size_t>(backupCount));

		if (ownerCount < 0 || backupCount < 0) {
			GS_CLIENT_THROW_CONNECTION(
					GS_ERROR_CC_MESSAGE_CORRUPTED,
					"masterAddress=" << masterAddress_);
		}

		if (ownerCount == 0 && (backupCount == 0 || !backupPreferred)) {
			if (allowEmpty) {
				return NULL;
			}

			GS_CLIENT_THROW_CONNECTION(
					GS_ERROR_CC_PARTITION_NOT_FOUND,
					"masterAddress=" << masterAddress_);
		}

		addressList = &nodeAddressMap_[partitionId];
		addressList->clear();
		addressList->resize(static_cast<size_t>(1 + backupCount));

		respIn.base().position(ownerPosition);
		if (ownerCount > 0) {
			decodeSocketAddress(respIn, (*addressList)[0]);
		}

		respIn.base().position(backupPosition);
		for (NodeAddressList::iterator it = addressList->begin() + 1;
				it != addressList->end(); ++it) {
			decodeSocketAddress(respIn, *it);
		}

		updateConnectionPoolSize();

		connectionFailedPreviously_ = false;
	}
	catch (...) {
		connectionFailedPreviously_ = true;
		masterConnection_.reset();
		try {
			nodeAddressMap_.erase(partitionId);
		}
		catch (...) {
		}
		throw;
	}
	return addressList;
}

size_t NodeResolver::getInetAddressSize() const {
	return ipv6Enabled_ ? 16 : 4;
}

ContainerHashMode::Id NodeResolver::decodeContainerHashMode(
		ArrayByteInStream &in) {
	int8_t containerHashMode;
	in >> containerHashMode;

	switch (containerHashMode) {
	case ContainerHashMode::COMPATIBLE1:
	case ContainerHashMode::MD5:
		break;
	default:
		GS_CLIENT_THROW_CONNECTION(GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by illegal hash mode");
	}

	return static_cast<ContainerHashMode::Id>(containerHashMode);
}

void NodeResolver::decodeSocketAddress(
		ArrayByteInStream &in, util::SocketAddress &socketAddress) {
	if (ipv6Enabled_) {
		util::SocketAddress::Inet6 address;
		in.readAll(&address, sizeof(address));

		int32_t port;
		in >> port;

		socketAddress.assign(address, static_cast<uint16_t>(port));
	}
	else {
		util::SocketAddress::Inet address;
		in.readAll(&address, sizeof(address));

		int32_t port;
		in >> port;

		socketAddress.assign(address, static_cast<uint16_t>(port));
	}
	cachedAddressSet_.insert(socketAddress);
}

void NodeResolver::updateConnectionPoolSize() {
	pool_.setMaxSize(
			std::max(preferableConnectionPoolSize_, cachedAddressSet_.size()));
}

const GSChar* NodeResolver::findLastChar(
		const GSChar *begin, const GSChar *end, int32_t ch) {
	assert(begin <= end);
	for (const GSChar *it = end; it != begin;) {
		if (*(--it) == ch) {
			return it;
		}
	}
	return NULL;
}

NodeResolver::ProtocolConfig::~ProtocolConfig() {
}

NodeResolver::DefaultProtocolConfig::~DefaultProtocolConfig() {
}

int32_t NodeResolver::DefaultProtocolConfig::getNotificationStatementType() {
	if (NodeConnection::getProtocolVersion() < 2) {
		return NOTIFICATION_STATEMENT_TYPE_V1;
	}

	return NOTIFICATION_STATEMENT_TYPE;
}

int32_t NodeResolver::DefaultProtocolConfig::getNormalStatementType(
		Statement::Id statement) {
	if (statement == Statement::GET_PARTITION_ADDRESS) {
		return NodeConnection::statementToNumber(statement);
	}
	else {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}
}

NodeResolver::AddressConfig::AddressConfig() :
		notificationPort_(DEFAULT_NOTIFICATION_PORT),
		notificationAddress_(DEFAULT_NOTIFICATION_ADDRESS),
		notificationAddressV6_(DEFAULT_NOTIFICATION_ADDRESS_V6),
		serviceType_(DEFAULT_SERVICE_TYPE),
		alwaysMaster_(false) {
}

template<typename T>
NodeResolver::ClusterInfoEntry<T>::ClusterInfoEntry() :
		value_(T()),
		acceptedByAny_(false),
		acceptedByConnection_(false) {
}

template<typename T>
const T* NodeResolver::ClusterInfoEntry<T>::get() const {
	return (acceptedByAny_ ? &value_ : NULL);
}

template<typename T>
bool NodeResolver::ClusterInfoEntry<T>::tryAccept(
		const T *value, const util::SocketAddress &address,
		bool byConnection, bool checkOnly) {
	if (value == NULL) {
		return true;
	}

	if (!acceptedByAny_) {
		if (!checkOnly) {
			value_ = *value;
			acceptedByAny_ = true;
		}
	}
	else if (value_ != *value) {
		return false;
	}

	if (!checkOnly) {
		acceptedByConnection_ |= byConnection;
		acceptedAddress_ = address;
	}

	return true;
}

template<typename T>
bool NodeResolver::ClusterInfoEntry<T>::invalidate() {
	if (!acceptedByConnection_ && acceptedByAny_) {
		value_ = T();
		acceptedByAny_ = false;
		return true;
	}
	return false;
}

NodeResolver::ClusterInfo::ClusterInfo(
		const NodeConnection::LoginInfo &loginInfo) :
		loginInfo_(loginInfo),
		lastMasterCacheCounter_(0) {
	loginInfo_.ownerMode_ = false;
}

bool NodeResolver::ClusterInfo::invalidate() {
	bool invalidated = false;
	invalidated |= partitionCount_.invalidate();
	invalidated |= hashMode_.invalidate();
	invalidated |= databaseId_.invalidate();
	return invalidated;
}

bool NodeResolver::ClusterInfo::acceptMasterInvalidation(
		uint64_t masterCacheCounter) {
	if (lastMasterCacheCounter_ != masterCacheCounter) {
		return false;
	}
	lastMasterCacheCounter_ = 0;
	return true;
}

const int32_t* NodeResolver::ClusterInfo::getPartitionCount() const {
	return partitionCount_.get();
}

const ContainerHashMode::Id* NodeResolver::ClusterInfo::getHashMode() const {
	return hashMode_.get();
}

const int64_t* NodeResolver::ClusterInfo::getDatabaseId() const {
	return databaseId_.get();
}

void NodeResolver::ClusterInfo::setPartitionCount(
		const int32_t *partitionCount) {
	if (partitionCount != NULL && *partitionCount <= 0) {
		return;
	}

	partitionCount_.tryAccept(
			partitionCount, util::SocketAddress(), true, false);
}


bool GridStoreChannel::v1ProtocolCompatibleSpecified_ = false;
bool GridStoreChannel::v1ProtocolCompatible_ = false;
bool GridStoreChannel::v10ResourceCompatible_ = false;
bool GridStoreChannel::v15TSPropsCompatible_ = false;
bool GridStoreChannel::v15DDLCompatible_ = true;
bool GridStoreChannel::v20AffinityCompatible_ = false;
bool GridStoreChannel::v21StatementIdCompatible_ = false;
bool GridStoreChannel::v40QueryCompatible_ = false;
bool GridStoreChannel::v40ContainerHashCompatible_ = true;
bool GridStoreChannel::v40SchemaCompatible_ = false;

const int32_t GridStoreChannel::FAILOVER_TIMEOUT_DEFAULT = 2 * 60 * 1000;
const int32_t GridStoreChannel::FAILOVER_RETRY_INTERVAL = 1 * 1000;
const size_t GridStoreChannel::INITIAL_BUFFER_SIZE = 256;
const size_t GridStoreChannel::MAPPER_CACHE_SIZE = 32;

GridStoreChannel::GridStoreChannel(
		const Config &config, const Source &source) :
		config_(config),
		statementRetryMode_(0),
		key_(source.key_),
		nodeResolver_(
				pool_, key_.passive_, key_.address_,
				config.connectionConfig_,
				key_.sarConfig_, key_.memberList_,
				NodeResolver::AddressConfig()),
		reqHeadLength_(NodeConnection::getRequestHeadLength(
				ClientUtil::isIPV6Address(key_.address_))),
		interceptorManager_(source.interceptorManager_) {
	nodeResolver_.setNotificationReceiveTimeoutMillis(
			config.notificationReceiveTimeoutMillis_);
	nodeResolver_.setProviderTimeoutMillis(key_.providerTimeoutMillis_);
}

NodeConnectionPool& GridStoreChannel::getConnectionPool() {
	return pool_;
}

GSInterceptorManager* GridStoreChannel::getInterceptorManager() throw() {
	return interceptorManager_;
}

void GridStoreChannel::apply(const Config &config) {
	nodeResolver_.setConnectionConfig(config.connectionConfig_);
	nodeResolver_.setNotificationReceiveTimeoutMillis(
			config.notificationReceiveTimeoutMillis_);
	nodeResolver_.setPreferableConnectionPoolSize(
			config.maxConnectionPoolSize_);

	util::LockGuard<util::Mutex> guard(mutex_);
	config_ = config;
}

int64_t GridStoreChannel::getFailoverTimeoutMillis(const Context &context) {
	if (context.localConfig_.failoverTimeoutMillis_ >= 0) {
		return context.localConfig_.failoverTimeoutMillis_;
	}

	return config_.getFailoverTimeoutMillis(mutex_);
}

void GridStoreChannel::setStatementRetryMode(int32_t statementRetryMode) {
	statementRetryMode_ = statementRetryMode;
}

void GridStoreChannel::setMonitoring(bool monitoring) {
	monitoring_ = monitoring;
}

bool GridStoreChannel::isMonitoring() {
	return monitoring_;
}

void GridStoreChannel::initializeRequestBuffer(
		util::NormalXArray<uint8_t> &req, size_t capacity) {
	req.reserve(capacity);
	NodeConnection::fillRequestHead(
			ClientUtil::isIPV6Address(key_.address_), req);
}

void GridStoreChannel::initializeResponseBuffer(
		util::NormalXArray<uint8_t> &resp, size_t capacity) {
	resp.reserve(capacity);
	resp.clear();
}

XArrayByteOutStream GridStoreChannel::getRequestOutStream(
		util::NormalXArray<uint8_t> &initializedReq) {
	initializedReq.resize(NodeConnection::getRequestHeadLength(
			ClientUtil::isIPV6Address(key_.address_)));
	return XArrayByteOutStream(NormalXArrayOutStream(initializedReq));
}

void GridStoreChannel::clearContext(Context &context, bool doClose) {
	if (doClose) {
		context.closed_ = true;
	}

	context.partitionId_ = -1;
	context.lastConnection_ = Context::ConnectionEntry();

	if (doClose) {
		Context::ConnectionMap &activeConnections = context.activeConnections_;
		while (!activeConnections.empty()) {
			Context::ConnectionMap::iterator it = activeConnections.begin();
			std::auto_ptr<NodeConnection> connection(it->second.second);
			activeConnections.erase(it);
			pool_.add(connection);
		}
	}
}

void GridStoreChannel::applyPartitionId(Context &context, int32_t partitionId) {
	if (context.partitionId_ != partitionId) {
		clearContext(context, false);
	}
	context.partitionId_ = partitionId;
}

void GridStoreChannel::checkActiveConnection(
		const Context &context, int32_t partitionId,
		const ConnectionId &connectionId) {

	const util::SocketAddress address = connectionId.getAddress();

	Context::ConnectionMap::const_iterator it =
			context.activeConnections_.find(address);
	if (it == context.activeConnections_.end() || connectionId.isEmpty() ||
			!connectionId.matches(it->second.first)) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_RECOVERABLE_CONNECTION_PROBLEM, "");
	}
}

ArrayByteInStream GridStoreChannel::executeStatement(
		Context &context, Statement::Id statement, int64_t statementId,
		util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp) {
	if (context.closed_) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_RESOURCE_CLOSED, "");
	}

	const size_t reqLength = req.size();
	util::DateTime failoverStartTime = 0;
	int64_t failoverTrialCount = 0;
	int64_t failoverReconnectedTrial = -1;
	for (bool firstTrial = true;; firstTrial = false) {
		try {
			ArrayByteInStream respIn = ArrayByteInStream(
					util::ArrayInStream(static_cast<const void*>(NULL), 0));

			if (context.resolverExecutor_ == NULL) {
				NodeConnection *lastConnection =
						context.lastConnection_.second;
				if (lastConnection == NULL) {
					updateConnection(context, firstTrial);
					lastConnection = context.lastConnection_.second;
				}
				respIn = lastConnection->executeStatement(
						statement, context.partitionId_, statementId,
						req, &resp);
			}
			else {
				context.resolverExecutor_->execute(
						nodeResolver_, context.clusterInfo_);
			}

			if (failoverTrialCount > 0) {
				context.failoverCount_++;
			}
			return respIn;
		}
		catch (ConnectionException &e) {
			NodeConnection *lastConnection = context.lastConnection_.second;
			if (lastConnection != NULL) {
				if (!firstTrial && context.lastHeartbeatCount_ !=
						lastConnection->getHeartbeatReceiveCount()) {
					failoverStartTime = 0;
					failoverReconnectedTrial = failoverTrialCount;
				}
				Context::ConnectionMap::iterator it =
						context.activeConnections_.find(
								lastConnection->getRemoteAddress());
				context.lastConnection_ = Context::ConnectionEntry();
				if (it != context.activeConnections_.end()) {
					delete lastConnection;
					context.activeConnections_.erase(it);
				}
				else {
					assert(false);
				}
			}

			if (statementRetryMode_ == 0) {
				invalidateMaster(context);
			}
			req.resize(reqLength);

			if (isConnectionDependentStatement(statement)) {
				GS_CLIENT_RETHROW_ERROR_CODED(
						GS_ERROR_CC_RECOVERABLE_CONNECTION_PROBLEM, e, "");
			}

			bool wrongNode = false;
			try {
				throw;
			}
			catch (WrongNodeException&) {
				wrongNode = true;
			}
			catch (ConnectionException&) {
			}

			if (statementRetryMode_ == 0) {
				if (firstTrial && !wrongNode) {
					continue;
				}
			}

			const util::DateTime curTime = util::DateTime::now(false);
			if (failoverStartTime == 0) {
				failoverStartTime = curTime;
			}

			const int64_t failureMillis = curTime.getDifference(
					failoverStartTime, util::DateTime::FIELD_MILLISECOND);
			const int64_t failoverTimeout = getFailoverTimeoutMillis(context);
			if (failureMillis >= failoverTimeout) {
				const GSChar *errorMessage = "";
				std::auto_ptr<std::string> errorMessageStr;
				try {
					util::NormalOStringStream oss;
					if (failoverTimeout > 0) {
						oss << "Failover timed out ("
								"trialCount=" << failoverTrialCount <<
								", reconnectedTrial=" <<
								failoverReconnectedTrial <<
								", failureMillis=" << failureMillis <<
								", reason=" <<
								ErrorReasonFormatter(e) << ")";
					}
					else if (!firstTrial) {
						oss << "Failover timed out ("
								"reason=" <<
								ErrorReasonFormatter(e) << ")";
					}
					errorMessageStr.reset(new std::string(oss.str()));
					errorMessage = errorMessageStr->c_str();
				}
				catch (...) {
				}

				if (wrongNode) {
					GS_CLIENT_RETHROW_WRONG_NODE(e, errorMessage);
				}
				else {
					GS_CLIENT_RETHROW_CONNECTION(e, errorMessage);
				}
			}


			if (statementRetryMode_ == 1 && wrongNode) {
				invalidateMaster(context);
			}

			util::Thread::sleep(Properties::timeoutToInt32Millis(
					config_.getFailoverRetryIntervalMillis(mutex_)));
			failoverTrialCount++;
		}
		catch (StatementException&) {
			if (!firstTrial && failoverTrialCount > 0) {
				context.failoverCount_++;
			}
			throw;
		}
		catch (std::exception &e) {
			GS_CLIENT_RETHROW_ERROR(e, "");
		}
	}
}

void GridStoreChannel::invalidateMaster(Context &context) {
	nodeResolver_.invalidateMaster(context.clusterInfo_);
}

void GridStoreChannel::checkContextAvailable(const Context &context) {
	if (context.closed_) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_RESOURCE_CLOSED, "");
	}
}

int64_t GridStoreChannel::getDatabaseId(Context &context) {
	ResolverExecutor executor(ResolverExecutor::COMMAND_GET_DATABASE_ID);
	executeResolver(context, executor);
	return executor.databaseId_;
}

void GridStoreChannel::getNodeAddressList(
		Context &context, int32_t partitionId,
		std::vector<util::SocketAddress> &addressList) {
	ResolverExecutor executor(ResolverExecutor::COMMAND_GET_ADDRESS_LIST);
	executor.partitionId_ = partitionId;
	executor.addressList_ = &addressList;
	executeResolver(context, executor);
}

int32_t GridStoreChannel::getPartitionCount(Context &context) {
	ResolverExecutor executor(ResolverExecutor::COMMAND_GET_PARTITION_COUNT);
	executeResolver(context, executor);
	return executor.partitionCount_;
}

int32_t GridStoreChannel::resolvePartitionId(
		Context &context, const ContainerKey &containerKey,
		const ContainerKeyConverter &keyConverter) {
	ResolverExecutor executor(ResolverExecutor::COMMAND_RESOLVE_PARTITION_ID);
	executor.containerKey_ = &containerKey;
	executor.keyConverter_ = &keyConverter;
	executeResolver(context, executor);
	return executor.partitionId_;
}

int32_t GridStoreChannel::calculatePartitionId(
		const ContainerKey &containerKey,
		const ContainerKeyConverter &keyConverter,
		ContainerHashMode::Id hashMode, int32_t partitionCount) {
	ContainerKeyConverter::Components components;
	keyConverter.decompose(containerKey, components);

	if (components.affinityNum_ >= 0) {
		return static_cast<int32_t>(components.affinityNum_ % partitionCount);
	}

	if (components.affinityStr_.get() != NULL) {
		const std::string &affinity = ClientUtil::normalizeSymbolUnchecked(
				components.affinityStr_->c_str());
		return calculatePartitionId(affinity.c_str(), hashMode, partitionCount);
	}

	const std::string &base = ClientUtil::normalizeSymbolUnchecked(
			components.base_.c_str());
	return calculatePartitionId(base.c_str(), hashMode, partitionCount);
}

int32_t GridStoreChannel::calculatePartitionId(
		const GSChar *normalizedString, ContainerHashMode::Id hashMode,
		int32_t partitionCount) {
	const size_t size = strlen(normalizedString);
	switch (hashMode) {
	case ContainerHashMode::COMPATIBLE1: {
		const uint32_t crcValue =
				util::CRC32::calculate(normalizedString, size);

		return static_cast<int32_t>(
					crcValue % static_cast<uint32_t>(partitionCount));
	}
	case ContainerHashMode::MD5: {
#if GS_CLIENT_MD5_LIB_ENABLED
		MD5_CTX ctx;
		MD5_init(&ctx);
		MD5_update(&ctx, normalizedString, static_cast<unsigned int>(size));

		const uint8_t *digest = MD5_final(&ctx);
		const size_t size = 4;
		UTIL_STATIC_ASSERT(size <= MD5_DIGEST_SIZE);

		uint32_t hash = 0;
		for (uint32_t i = 0; i < size; i++) {
			hash = (hash << 8) | digest[i];
		}

		return static_cast<int32_t>(
					hash % static_cast<uint32_t>(partitionCount));
#else
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
#endif
	}
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}
}

bool GridStoreChannel::isMasterResolvableByConnection() {
	return NodeConnection::getProtocolVersion() >= 8;
}

ContainerHashMode::Id GridStoreChannel::getDefaultContainerHashMode() {
	if (NodeConnection::getProtocolVersion() < 14 ||
			v40ContainerHashCompatible_) {
		return ContainerHashMode::COMPATIBLE1;
	}

	return ContainerHashMode::MD5;
}

int32_t GridStoreChannel::statementToNumber(Statement::Id statement) {
	return NodeConnection::statementToNumber(statement);
}

bool GridStoreChannel::isRecoverableError(GSResult result) throw() {
	return (result == GS_ERROR_CC_RECOVERABLE_CONNECTION_PROBLEM ||
			result == GS_ERROR_CC_RECOVERABLE_ROW_SET_LOST);
}

void GridStoreChannel::executeResolver(
		Context &context, ResolverExecutor &executor) {
	util::NormalXArray<uint8_t> req;
	util::NormalXArray<uint8_t> resp;

	ResolverExecutorScope scope(context, executor);
	executeStatement(context, Statement::Id(), -1, req, resp);
}

void GridStoreChannel::updateConnection(
		Context &context, bool preferConnectionPool) {
	if (context.lastConnection_.second != NULL) {
		return;
	}

	util::NormalXArray<uint8_t> req;
	util::NormalXArray<uint8_t> resp;
	const util::SocketAddress address = nodeResolver_.getNodeAddress(
			context.clusterInfo_, context.partitionId_,
			!context.loginInfo_.ownerMode_,
			context.getPreferableHost(context.partitionId_));
	if (address.isEmpty()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "Empty address");
	}

	Context::ConnectionMap::iterator it =
			context.activeConnections_.find(address);
	if (it != context.activeConnections_.end()) {
		context.lastConnection_ = it->second;
		return;
	}

	int64_t databaseId;
	std::auto_ptr<NodeConnection> connection = pool_.resolve(
			address, req, resp, config_.getConnectionConfig(mutex_),
			context.loginInfo_, &databaseId, preferConnectionPool);
	nodeResolver_.acceptDatabaseId(
			context.clusterInfo_, databaseId,
			connection->getRemoteAddress());

	context.connectionId_ = context.connectionId_.newId(address);
	Context::ConnectionEntry connectionEntry(
			context.connectionId_, connection.get());

	context.activeConnections_[address] = connectionEntry;
	context.lastConnection_ = connectionEntry;
	context.lastHeartbeatCount_ =
			context.lastConnection_.second->getHeartbeatReceiveCount();
	connection.release();
}

bool GridStoreChannel::isConnectionDependentStatement(
		Statement::Id statement) {
	return (statement == Statement::CLOSE_ROW_SET ||
			statement == Statement::FETCH_ROW_SET);
}

bool GridStoreChannel::isContainerKeyComposed() {
	return (NodeConnection::getProtocolVersion() >= 14);
}


GridStoreChannel::Config::Config() :
		failoverTimeoutMillis_(FAILOVER_TIMEOUT_DEFAULT),
		failoverRetryIntervalMillis_(FAILOVER_RETRY_INTERVAL),
		notificationReceiveTimeoutMillis_(
				NodeResolver::NOTIFICATION_RECEIVE_TIMEOUT),
		maxConnectionPoolSize_(-1) {
}

bool GridStoreChannel::Config::set(Properties properties) {
	int64_t failoverTimeoutMillis;
	properties.getTimeoutMillis(
			"failoverTimeout", failoverTimeoutMillis, failoverTimeoutMillis_);
	int64_t failoverRetryIntervalMillis;
	properties.getTimeoutMillis("failoverRetryInterval",
			failoverRetryIntervalMillis, failoverRetryIntervalMillis_);
	int64_t notificationReceiveTimeoutMillis;
	properties.getTimeoutMillis("notificationReceiveTimeout",
			notificationReceiveTimeoutMillis, notificationReceiveTimeoutMillis_);

	if (failoverTimeoutMillis < 0 ||
			failoverRetryIntervalMillis < 0 ||
			notificationReceiveTimeoutMillis < 0) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PROPERTY_ENTRY,
				"Negative timeout parameter");
	}

	bool updated = connectionConfig_.set(properties);
	if (failoverTimeoutMillis != failoverTimeoutMillis_ ||
			failoverRetryIntervalMillis != failoverRetryIntervalMillis_ ||
			notificationReceiveTimeoutMillis !=
					notificationReceiveTimeoutMillis_) {
		failoverTimeoutMillis_ = failoverTimeoutMillis;
		failoverRetryIntervalMillis_ = failoverRetryIntervalMillis;
		notificationReceiveTimeoutMillis_ = notificationReceiveTimeoutMillis;
		updated = true;
	}

	int32_t maxConnectionPoolSize;
	if (properties.getInteger("maxConnectionPoolSize", maxConnectionPoolSize)) {
		if (maxConnectionPoolSize < 0) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PROPERTY_ENTRY,
					"Negative connection pool size");
		}
		maxConnectionPoolSize_ = maxConnectionPoolSize;
		updated = true;
	}

	return updated;
}

int64_t GridStoreChannel::Config::getFailoverTimeoutMillis(
		util::Mutex &mutex) {
	util::LockGuard<util::Mutex> guard(mutex);
	return failoverTimeoutMillis_;
}

int64_t GridStoreChannel::Config::getFailoverRetryIntervalMillis(
		util::Mutex &mutex) {
	util::LockGuard<util::Mutex> guard(mutex);
	return failoverRetryIntervalMillis_;
}

NodeConnection::Config GridStoreChannel::Config::getConnectionConfig(
		util::Mutex &mutex) {
	util::LockGuard<util::Mutex> guard(mutex);
	return connectionConfig_;
}


GridStoreChannel::Key::Key() : passive_(false) {
}


bool GridStoreChannel::KeyLess::operator()(
		const Key &key1, const Key &key2) const {
	if (key1.passive_ != key2.passive_) {
		return key1.passive_ < key2.passive_;
	}
	else if (!SocketAddressEqual()(key1.address_, key2.address_)) {
		return SocketAddressLess()(key1.address_, key2.address_);
	}
	else if (key1.clusterName_ != key2.clusterName_) {
		return key1.clusterName_ < key2.clusterName_;
	}
	else if (key1.providerURL_ != key2.providerURL_) {
		return key1.providerURL_ < key2.providerURL_;
	}
	else if (key1.providerTimeoutMillis_ != key2.providerTimeoutMillis_) {
		return key1.providerTimeoutMillis_ < key2.providerTimeoutMillis_;
	}
	else if (key1.sarConfig_.addressFamily_ !=
			key2.sarConfig_.addressFamily_) {
		return key1.sarConfig_.addressFamily_ < key2.sarConfig_.addressFamily_;
	}

	{
		const size_t size1 = key1.memberList_.size();
		const size_t size2 = key2.memberList_.size();
		if (size1 < size2) {
			return true;
		}
		else if (size1 > size2) {
			return false;
		}
		else {
			for (size_t i = 0; i < size1; i++) {
				const util::SocketAddress &a1 = key1.memberList_[i];
				const util::SocketAddress &a2 = key2.memberList_[i];
				if (SocketAddressLess()(a1, a2)) {
					return true;
				}
				else if (!SocketAddressEqual()(a1, a2)) {
					return false;
				}
			}
			return false;
		}
	}
}


GridStoreChannel::ConnectionId::ConnectionId() :
		value_(0) {
}

bool GridStoreChannel::ConnectionId::isEmpty() const {
	return (value_ == 0);
}

const util::SocketAddress& GridStoreChannel::ConnectionId::getAddress() const {
	return address_;
}

GridStoreChannel::ConnectionId GridStoreChannel::ConnectionId::newId(
		const util::SocketAddress &address) const {
	ConnectionId next;

	do {
		next.value_ = value_ + 1;
	}
	while (next.isEmpty());

	next.address_ = address;

	return next;
}

bool GridStoreChannel::ConnectionId::matches(
		const ConnectionId &connectionId) const {
	return (value_ == connectionId.value_ &&
			address_ == connectionId.address_);
}


GridStoreChannel::LocalConfig::LocalConfig() :
		failoverTimeoutMillis_(-1),
		transactionTimeoutMillis_(-1),
		fetchBytesSize_(0),
		containerCacheSize_(0) {
}

void GridStoreChannel::LocalConfig::set(const Properties &properties) {

	int64_t failoverTimeoutMillis;
	properties.getTimeoutMillis("failoverTimeout", failoverTimeoutMillis, -1);

	int64_t transactionTimeoutMillis;
	properties.getTimeoutMillis(
			"transactionTimeout", transactionTimeoutMillis, -1);

	int32_t containerCacheSize;
	if (!properties.getInteger("containerCacheSize", containerCacheSize)) {
		containerCacheSize = 0;
	}
	else if (containerCacheSize < 0) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
				"Negative container cache size (size=" <<
				containerCacheSize << ")");
	}

	int32_t fetchBytesSize;
	if (!properties.getInteger("internal.fetchBytesSize", fetchBytesSize)) {
		fetchBytesSize = -1;
	}

	failoverTimeoutMillis_ = failoverTimeoutMillis;
	transactionTimeoutMillis_ = transactionTimeoutMillis;
	containerCacheSize_ = containerCacheSize;
	fetchBytesSize_ = fetchBytesSize;
}


GridStoreChannel::Source::Source() :
		partitionCount_(0),
		loginInfo_("", "", false, "", "", -1, "", -1),
		interceptorManager_(NULL) {
}

GridStoreChannel::Source::~Source() {
}

const GridStoreChannel::Key& GridStoreChannel::Source::getKey() const {
	return key_;
}

NodeResolver::ClusterInfo GridStoreChannel::Source::createClusterInfo() const {
	NodeResolver::ClusterInfo clusterInfo(loginInfo_);
	clusterInfo.setPartitionCount(&partitionCount_);
	return clusterInfo;
}

GridStoreChannel::Context GridStoreChannel::Source::createContext() const {
	return Context(localConfig_, loginInfo_, createClusterInfo());
}

void GridStoreChannel::Source::set(const Properties &properties) {
	bool passive;
	ServiceAddressResolver::Config sarConfig;
	std::vector<util::SocketAddress> memberList;

	const util::SocketAddress address = NodeResolver::getAddressProperties(
			properties, &passive, sarConfig, memberList, NULL);

	std::string providerURL;
	if (sarConfig.providerURL_ != NULL) {
		providerURL = sarConfig.providerURL_;
	}

	int64_t providerTimeoutMillis;
	properties.getTimeoutMillis(
			"notificationProviderTimeout", providerTimeoutMillis, -1);

	std::string clusterName;
	properties.getString("clusterName", clusterName);
	std::string user;
	properties.getString("user", user);
	std::string database;
	properties.getString("database", database);
	std::string password;
	properties.getString("password", password);

	if (!clusterName.empty()) {
		ClientUtil::checkSymbol(clusterName.c_str(), "cluster name");
	}
	if (!database.empty()) {
		ClientUtil::checkSymbol(database.c_str(), "database name");
	}

	std::string applicationName;
	if (properties.getString("applicationName", applicationName)) {
		ClientUtil::checkSymbol(applicationName.c_str(), "application name");
	}

	int32_t partitionCount;
	if (!properties.getInteger("partitionCount", partitionCount)) {
		partitionCount = 0;
	}

	std::string consistency;
	bool backupPreferred;
	if (!properties.getString("consistency", consistency) ||
			consistency == "IMMEDIATE") {
		backupPreferred = false;
	}
	else if (consistency == "EVENTUAL") {
		backupPreferred = true;
	}
	else {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	LocalConfig localConfig;
	localConfig.set(properties);

	key_.passive_ = passive;
	key_.address_ = address;
	key_.clusterName_.swap(clusterName);

	key_.providerURL_.swap(providerURL);
	key_.providerTimeoutMillis_ = providerTimeoutMillis;
	key_.sarConfig_ = ServiceAddressResolver::Config();
	key_.sarConfig_.addressFamily_ = sarConfig.addressFamily_;
	if (sarConfig.providerURL_ != NULL) {
		key_.sarConfig_.providerURL_ = key_.providerURL_.c_str();
	}
	key_.memberList_.swap(memberList);

	partitionCount_ = partitionCount;
	localConfig_ = localConfig;
	loginInfo_ = NodeConnection::LoginInfo(
			user.c_str(),
			password.c_str(),
			!backupPreferred,
			key_.clusterName_.c_str(),
			database.c_str(),
			localConfig.transactionTimeoutMillis_,
			applicationName.c_str(),
			resolveStoreMemoryAgingSwapRate(properties));
}

double GridStoreChannel::Source::resolveStoreMemoryAgingSwapRate(
		const Properties &properties) {
	const char8_t *name = "storeMemoryAgingSwapRate";
	double value;
	if (!properties.getDouble(name, value)) {
		return -1;
	}

	if (!(0 <= value && value <= 1)) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PROPERTY_ENTRY,
				"Property value out of range (name=" << name <<
				", value=" << value << ")");
	}

	return value;
}


GridStoreChannel::Context::~Context() try {
	for (ConnectionMap::iterator it = activeConnections_.begin();
			it != activeConnections_.end(); ++it) {
		delete it->second.second;
	}
}
catch (...) {
}

GridStoreChannel::Context::Context(const Context &another) :
		localConfig_(another.localConfig_),
		partitionId_(-1),
		lastHeartbeatCount_(0),
		loginInfo_(another.loginInfo_),
		clusterInfo_(another.clusterInfo_),
		closed_(false),
		failoverCount_(0),
		lastSessionId_(0),
		resolverExecutor_(NULL),
		keyConverter_(another.keyConverter_) {
}

GridStoreChannel::Context& GridStoreChannel::Context::operator=(
		const Context &another) {
	localConfig_ = another.localConfig_;
	partitionId_ = -1;
	lastHeartbeatCount_ = 0;
	lastConnection_ = ConnectionEntry();
	loginInfo_ = another.loginInfo_;
	clusterInfo_ = another.clusterInfo_;
	closed_ = another.closed_;
	failoverCount_ = 0;
	lastSessionId_ = 0;
	resolverExecutor_ = NULL;
	keyConverter_ = another.keyConverter_;
	return *this;
}

int64_t GridStoreChannel::Context::getTransactionTimeoutMillis() const {
	return localConfig_.transactionTimeoutMillis_;
}

GridStoreChannel::ContextQueryOptions
GridStoreChannel::Context::bindQueryOptions(
		const ContextQueryOptions *source) const {
	return ContextQueryOptions(localConfig_, source);
}

int32_t GridStoreChannel::Context::getFailoverCount() const {
	return failoverCount_;
}

util::NormalXArray<uint8_t>& GridStoreChannel::Context::getRequestBuffer() {
	return req_;
}

util::NormalXArray<uint8_t>& GridStoreChannel::Context::getResponseBuffer() {
	return resp_;
}

const uuid_t& GridStoreChannel::Context::getSessionUUID() const {
	return loginInfo_.clientId_.getUUID();
}

int64_t GridStoreChannel::Context::generateSessionId() {
	int64_t newId = lastSessionId_ + 1;
	while (newId == 0) {
		newId++;
	}
	lastSessionId_ = newId;

	return newId;

}

NodeConnection::ClientId GridStoreChannel::Context::generateClientId() {
	return NodeConnection::ClientId(
			loginInfo_.clientId_.getUUID(), generateSessionId());
}

GridStoreChannel::ConnectionId
GridStoreChannel::Context::getLastConnectionId() const {
	return lastConnection_.first;
}

GridStoreChannel::ContainerCache*
GridStoreChannel::Context::getContainerCache() {
	if (localConfig_.containerCacheSize_ <= 0) {
		return NULL;
	}

	ContainerCache *cache = containerCache_.get();
	if (cache == NULL) {
		containerCache_.reset(
				new ContainerCache(localConfig_.containerCacheSize_));
		return containerCache_.get();
	}

	return cache;
}

void GridStoreChannel::Context::setPreferableHost(
		int32_t partitionId, const util::SocketAddress *host) {
	if (host == NULL) {
		preferableHosts_.erase(partitionId);
	}
	else {
		preferableHosts_[partitionId] = *host;
	}
}

const util::SocketAddress* GridStoreChannel::Context::getPreferableHost(
		int32_t partitionId) const {

	Context::PreferableHostMap::const_iterator it =
			preferableHosts_.find(partitionId);
	if (it == preferableHosts_.end()) {
		return NULL;
	}

	return &it->second;
}

const ContainerKeyConverter&
GridStoreChannel::Context::getKeyConverter() const {
	return keyConverter_;
}

GridStoreChannel::Context::Context(
		const LocalConfig &localConfig,
		const NodeConnection::LoginInfo &loginInfo,
		const NodeResolver::ClusterInfo &clusterInfo) :
		localConfig_(localConfig),
		partitionId_(-1),
		lastHeartbeatCount_(0),
		loginInfo_(loginInfo),
		clusterInfo_(clusterInfo),
		closed_(false),
		failoverCount_(0),
		lastSessionId_(0),
		resolverExecutor_(NULL),
		keyConverter_(!isContainerKeyComposed(), false) {
	loginInfo_.clientId_.generate(0);
}


GridStoreChannel::ContextQueryOptions::ContextQueryOptions(
		const LocalConfig &localConfig, const ContextQueryOptions *source) :
		localConfig_(localConfig), source_(source) {
}

bool GridStoreChannel::ContextQueryOptions::hasOptions() const {
	return localConfig_.fetchBytesSize_ > 0 ||
			(source_ != NULL && source_->hasOptions());
}

void GridStoreChannel::ContextQueryOptions::putOptions(
		NodeConnection::OptionalRequest &optionalRequest) const {

	optionalRequest.fetchBytesSize_ = localConfig_.fetchBytesSize_;

	if (source_ != NULL) {
		source_->putOptions(optionalRequest);
	}
}


GridStoreChannel::LocatedSchema::LocatedSchema() :
		schemaCacheId_(0), mapper_(NULL), containerId_(0), versionId_(0) {
}

const RowMapper* GridStoreChannel::LocatedSchema::getMapper() const {
	return mapper_;
}

int64_t GridStoreChannel::LocatedSchema::getContainerId() const {
	return containerId_;
}

int32_t GridStoreChannel::LocatedSchema::getVersionId() const {
	return versionId_;
}

GridStoreChannel::LocatedSchema::LocatedSchema(
		uint64_t schemaCacheId,
		const RowMapper *mapper, int64_t containerId, int32_t versionId) :
		schemaCacheId_(schemaCacheId),
		mapper_(mapper), containerId_(containerId), versionId_(versionId) {
}


GridStoreChannel::SessionInfo::SessionInfo() :
		sessionCacheId_(0),
		key_(0, 0),
		sessionId_(0),
		lastStatementId_(0) {
}

GridStoreChannel::SessionInfo::SessionInfo(
		uint64_t sessionCacheId, int32_t partitionId, int64_t containerId,
		int64_t sessionId, int64_t lastStatementId) :
		sessionCacheId_(sessionCacheId),
		key_(partitionId, containerId),
		sessionId_(sessionId),
		lastStatementId_(lastStatementId) {
}

int32_t GridStoreChannel::SessionInfo::getPartitionId() const {
	return key_.first;
}

int64_t GridStoreChannel::SessionInfo::getContainerId() const {
	return key_.second;
}

int64_t GridStoreChannel::SessionInfo::getSessionId() const {
	return sessionId_;
}

int64_t GridStoreChannel::SessionInfo::getLastStatementId() const {
	return lastStatementId_;
}


GridStoreChannel::ContainerCache::ContainerCache(int32_t cacheSize) :
		cacheSize_(cacheSize),
		lastSchemaCacheId_(0),
		lastSessionCacheId_(0) {
}

GridStoreChannel::ContainerCache::~ContainerCache() try {
	RowMapper::Cache &mapperCache = RowMapper::getDefaultCache();
	for (SchemaCache::iterator it = schemaCache_.begin();
			it != schemaCache_.end(); ++it) {
		mapperCache.release(&it->second.mapper_);
	}

	schemaNameMap_.clear();
	schemaCache_.clear();
	sessionCache_.clear();
}
catch (...) {
}

const GridStoreChannel::LocatedSchema*
GridStoreChannel::ContainerCache::findSchema(
		const ContainerKey &normalizedContainerKey,
		const GSBinding *binding,
		const GSContainerType *containerType,
		bool general, const RowMapper::Config &config) const {

	SchemaCache::const_iterator it =
			schemaCache_.find(normalizedContainerKey);
	if (it == schemaCache_.end()) {
		return NULL;
	}

	const GridStoreChannel::LocatedSchema *schema = &it->second;
	const RowMapper::RowTypeCategory category = (containerType == NULL ?
			schema->getMapper()->getCategory() :
			RowMapper::containerTypeToCategory(*containerType));

	const RowMapper *mapper = schema->getMapper();
	if (binding == NULL) {
		if (mapper->getCategory() ^ category) {
			return NULL;
		}
		if (mapper->isGeneral() ^ general) {
			return NULL;
		}
		if (mapper->isNullableAllowed() ^ config.nullableAllowed_) {
			return NULL;
		}
	}
	else if (!mapper->matches(category, binding, general, config)) {
		return NULL;
	}

	return schema;
}

void GridStoreChannel::ContainerCache::cacheSchema(
		const ContainerKey &normalizedContainerKey,
		const RowMapper &mapper, int64_t containerId, int32_t versionId) {

	removeSchema(normalizedContainerKey);

	RowMapper::Cache &mapperCache = RowMapper::getDefaultCache();
	const RowMapper *duplicatedMapper = mapperCache.duplicate(mapper);

	while (schemaNameMap_.find(++lastSchemaCacheId_) !=
			schemaNameMap_.end()) {
	}

	const LocatedSchema schema(
			lastSchemaCacheId_, duplicatedMapper, containerId, versionId);

	schemaCache_[normalizedContainerKey] = schema;
	schemaNameMap_[schema.schemaCacheId_] = normalizedContainerKey;

	if (!schemaNameMap_.empty() &&
			schemaNameMap_.size() > static_cast<size_t>(cacheSize_)) {
		if (!removeSchema(schemaNameMap_.begin()->second)) {
			schemaNameMap_.erase(schemaNameMap_.begin());
		}
	}
}

bool GridStoreChannel::ContainerCache::removeSchema(
		const ContainerKey &normalizedContainerKey) {

	SchemaCache::iterator it = schemaCache_.find(normalizedContainerKey);
	if (it == schemaCache_.end()) {
		return false;
	}

	RowMapper::Cache &mapperCache = RowMapper::getDefaultCache();
	mapperCache.release(&it->second.mapper_);

	schemaNameMap_.erase(it->second.schemaCacheId_);
	schemaCache_.erase(it);

	return true;
}

bool GridStoreChannel::ContainerCache::takeSession(
		int32_t partitionId, int64_t containerId, SessionInfo &sessionInfo) {
	SessionCache::iterator it =
			sessionCache_.find(SessionInfo::Key(partitionId, containerId));
	if (it == sessionCache_.end()) {
		return false;
	}

	sessionInfo = it->second;

	sessionKeyMap_.erase(it->second.sessionCacheId_);
	sessionCache_.erase(it);

	return true;
}

bool GridStoreChannel::ContainerCache::cacheSession(
		int32_t partitionId, int64_t containerId,
		int64_t sessionId, int64_t lastStatementId,
		SessionInfo &oldSessionInfo) {
	bool oldSessionFound = takeSession(partitionId, containerId, oldSessionInfo);

	while (sessionKeyMap_.find(++lastSessionCacheId_) != sessionKeyMap_.end()) {
	}

	SessionInfo newSessionInfo(lastSchemaCacheId_,
			partitionId, containerId, sessionId, lastStatementId);
	sessionKeyMap_[lastSchemaCacheId_] = newSessionInfo.key_;
	sessionCache_[newSessionInfo.key_] = newSessionInfo;

	if (!oldSessionFound && !sessionKeyMap_.empty() &&
			sessionKeyMap_.size() > static_cast<size_t>(cacheSize_)) {
		const SessionInfo::Key &key = sessionKeyMap_.begin()->second;
		oldSessionFound |= takeSession(key.first, key.second, oldSessionInfo);
		if (!oldSessionFound) {
			sessionKeyMap_.erase(sessionKeyMap_.begin());
		}
	}

	return oldSessionFound;
}

void GridStoreChannel::ContainerCache::takeAllSessions(
		std::vector<SessionInfo> &sessionList) {
	for (SessionCache::iterator it = sessionCache_.begin();
			it != sessionCache_.end(); ++it) {
		sessionList.push_back(it->second);
	}

	sessionKeyMap_.clear();
	sessionCache_.clear();
}


GridStoreChannel::ResolverExecutor::ResolverExecutor(Command command) :
		command_(command),
		containerKey_(NULL),
		keyConverter_(NULL),
		addressList_(NULL),
		partitionId_(-1),
		partitionCount_(-1),
		databaseId_(-1) {
}

void GridStoreChannel::ResolverExecutor::execute(
		NodeResolver &resolver, NodeResolver::ClusterInfo &clusterInfo) {
	switch (command_) {
	case COMMAND_GET_DATABASE_ID:
		databaseId_ = resolver.getDatabaseId(clusterInfo);
		break;
	case COMMAND_GET_ADDRESS_LIST:
		assert(partitionId_ >= 0);
		assert(addressList_ != NULL);
		resolver.getNodeAddressList(clusterInfo, partitionId_, *addressList_);
		break;
	case COMMAND_GET_PARTITION_COUNT:
		partitionCount_ = resolver.getPartitionCount(clusterInfo);
		break;
	case COMMAND_RESOLVE_PARTITION_ID:
		assert(containerKey_ != NULL);
		assert(keyConverter_ != NULL);
		partitionId_ = GridStoreChannel::calculatePartitionId(
				*containerKey_, *keyConverter_,
				resolver.getContainerHashMode(clusterInfo),
				resolver.getPartitionCount(clusterInfo));
		break;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}
}


GridStoreChannel::ResolverExecutorScope::ResolverExecutorScope(
		Context &context, ResolverExecutor &executor) :
		context_(context) {
	assert(context.resolverExecutor_ == NULL);
	context.resolverExecutor_ = &executor;
}

GridStoreChannel::ResolverExecutorScope::~ResolverExecutorScope() {
	assert(context_.resolverExecutor_ != NULL);
	context_.resolverExecutor_ = NULL;
}


const uint32_t GSResourceHeader::RESOURCE_TYPE_MASK = 0x1ec7e2eb;
const uint32_t GSResourceHeader::FLAG_INTERCEPTABLE =
		static_cast<uint32_t>(1) << (sizeof(uint32_t) * CHAR_BIT - 1);

#ifdef GS_CLIENT_UNIT_TEST
int64_t
GSResourceHeader::resourceCountList_[GSResourceType::TYPE_ID_MAX] = { 0 };
#endif

GSResourceHeader::GSResourceHeader(
		GSResourceType::Id type, void *parentResource,
		GSInterceptorManager *interceptorManager) :
		maskedType_(
				maskType(type, resolveInterceptable(
						type, parentResource, interceptorManager))) {
#ifdef GS_CLIENT_UNIT_TEST
	++resourceCountList_[getType()];
#endif
}

GSResourceHeader::~GSResourceHeader() {
#ifdef GS_CLIENT_UNIT_TEST
	--resourceCountList_[getType()];
#endif
}

bool GSResourceHeader::getType(
		const void *resource, GSResourceType::Id &type) throw() {
	if (resource == NULL) {
		memset(&type, -1, sizeof(type));
		return false;
	}

	type = static_cast<const GSResourceHeader*>(resource)->getType();
	return true;
}

bool GSResourceHeader::checkType(
		const void *resource, GSResourceType::Id type) throw() {
	GSResourceType::Id actualType;
	return (getType(resource, actualType) && actualType == type);
}

template<typename T>
GSResourceType::Id GSResourceHeader::getStaticType(T *resource) throw() {
	static_cast<void>(resource);
	return GSResourceType::Resolver<T>::TYPE;
}

template<typename T>
GSResourceType::Id GSResourceHeader::getStaticType(T **resource) throw() {
	static_cast<void>(resource);
	return GSResourceType::Resolver<T>::TYPE;
}

ErrorStack* GSResourceHeader::findErrorStack(
		void *resource, util::Mutex *&mutex) throw() {
	mutex = NULL;

	GSGridStore *store;
	GSGridStoreFactory *factory;
	findStoreOrFactory(resource, store, factory);

	if (store != NULL) {
		return &store->stack_;
	}
	else if (factory != NULL) {
		mutex = &factory->data_->mutex_;
		return &factory->data_->stack_;
	}
	else {
		return NULL;
	}
}

GSInterceptorManager* GSResourceHeader::findInterceptorManager(
		void *resource) throw() {
	if (resource == NULL) {
		return NULL;
	}

	if (!static_cast<GSResourceHeader*>(resource)->isInterceptable()) {
		return NULL;
	}

	GSGridStore *store;
	GSGridStoreFactory *factory;
	findStoreOrFactory(resource, store, factory);

	if (store != NULL) {
		return store->channel_.getInterceptorManager();
	}
	else if (factory != NULL) {
		return &factory->data_->interceptorManager_;
	}
	else {
		return NULL;
	}
}

void GSResourceHeader::findStoreOrFactory(
		void *resource, GSGridStore *&store,
		GSGridStoreFactory *&factory) throw() {
	store = NULL;
	factory = NULL;

	if (resource == NULL) {
		return;
	}

	const GSResourceType::Id type =
			static_cast<GSResourceHeader*>(resource)->getType();
	switch (type) {
	case GSResourceType::GRID_STORE_FACTORY: {
		GSGridStoreFactory *rawFactory =
				static_cast<GSGridStoreFactory*>(resource);
		if (rawFactory->data_.get() == NULL) {
			return;
		}
		factory = rawFactory;
		break;
	}
	case GSResourceType::GRID_STORE:
		store = static_cast<GSGridStore*>(resource);
		break;
	case GSResourceType::CONTAINER:
		store = static_cast<GSContainer*>(resource)->store_;
		break;
	case GSResourceType::QUERY:
		store = static_cast<GSQuery*>(resource)->container_->store_;
		break;
	case GSResourceType::ROW_SET:
		store = static_cast<GSRowSet*>(resource)->container_->store_;
		break;
	case GSResourceType::AGGREGATION_RESULT:
		store = static_cast<GSAggregationResult*>(
				resource)->container_->store_;
		break;
	case GSResourceType::ROW:
		findStoreOrFactory(
				static_cast<GSRow*>(resource)->parentResource_,
				store, factory);
		break;
	case GSResourceType::ROW_KEY_PREDICATE:
		findStoreOrFactory(
				static_cast<GSRowKeyPredicate*>(resource)->store_,
				store, factory);
		break;
	default:
		break;
	}
}

void GSResourceHeader::clearLastError(void *resource) throw() {
	util::Mutex *mutex;
	ErrorStack *errorStack = findErrorStack(resource, mutex);

	DynamicLockGuard<util::Mutex> guard(mutex);
	if (errorStack != NULL) {
		return errorStack->clear();
	}
}

GSResult GSResourceHeader::setCurrentException(void *resource) throw() {
	util::Mutex *mutex;
	ErrorStack *errorStack = findErrorStack(resource, mutex);

	DynamicLockGuard<util::Mutex> guard(mutex);
	if (errorStack != NULL) {
		return errorStack->setFromCurrentException();
	}

	try {
		throw;
	}
	catch (util::Exception &e) {
		try {
			const GSResult resultCode = e.getErrorCode(0);
			if (resultCode != 0) {
				return resultCode;
			}
		}
		catch (...) {
		}
	}
	catch (std::bad_alloc&) {
		return GS_ERROR_CC_ALLOCATION_FAILED;
	}
	catch (...) {
	}

	return GS_ERROR_CC_INTERNAL_ERROR;
}

#ifdef GS_CLIENT_UNIT_TEST
int64_t GSResourceHeader::getResourceCount(GSResourceType::Id type) {
	return resourceCountList_[type];
}
#endif

bool GSResourceHeader::resolveInterceptable(
		GSResourceType::Id type, void *parentResource,
		GSInterceptorManager *interceptorManager) {
	if (type == GSResourceType::GRID_STORE_FACTORY) {
		return true;
	}

	GSInterceptorManager *manager = interceptorManager;

	if (manager == NULL) {
		manager = findInterceptorManager(parentResource);
	}

	if (manager == NULL) {
		return false;
	}

	return !manager->filterByResourceType(type);
}

GSResourceType::Id GSResourceHeader::getType() const throw() {
	return unmaskType(maskedType_);
}

bool GSResourceHeader::isInterceptable() const throw() {
	return ((maskedType_ & FLAG_INTERCEPTABLE) != 0);
}

uint32_t GSResourceHeader::maskType(
		GSResourceType::Id type, bool interceptable) throw() {
	return ((type ^ RESOURCE_TYPE_MASK) & ~FLAG_INTERCEPTABLE) |
			(interceptable ? FLAG_INTERCEPTABLE : 0u);
}

GSResourceType::Id GSResourceHeader::unmaskType(uint32_t maskedType) throw() {
	return static_cast<GSResourceType::Id>(
			(maskedType ^ RESOURCE_TYPE_MASK) & ~FLAG_INTERCEPTABLE);
}


GSInterceptor::GSInterceptor(GSInterceptorManager &manager) :
		manager_(manager),
		id_(std::numeric_limits<Id>::max()) {
	id_ = manager_.add(*this);
}

GSInterceptor::~GSInterceptor() try {
	manager_.remove(id_);
}
catch (...) {
	assert(false);
}

GSInterceptor::Id GSInterceptor::getId() const {
	return id_;
}

bool GSInterceptor::isActivated(GSResourceType::Id type) {
	static_cast<void>(type);
	return true;
}

bool GSInterceptor::start(
		const FunctionInfo &funcInfo, const ParameterList &args,
		const Parameter &ret) {
	static_cast<void>(funcInfo);
	static_cast<void>(args);
	static_cast<void>(ret);
	return false;
}

void GSInterceptor::finish(
		const FunctionInfo &funcInfo, const ParameterList &args) {
	static_cast<void>(funcInfo);
	static_cast<void>(args);
}


GSInterceptor::FunctionInfo::FunctionInfo(
		const GSChar *rawName, GSResourceType::Id resourceType) :
		rawName_(rawName),
		resourceType_(resourceType) {
}

const GSChar* GSInterceptor::FunctionInfo::getRawName() const {
	return rawName_;
}

GSResourceType::Id GSInterceptor::FunctionInfo::getResourceType() const {
	return resourceType_;
}

bool GSInterceptor::FunctionInfo::matchName(const GSChar *name) const {
	if (strstr(rawName_, name) != rawName_) {
		return false;
	}

	const GSChar *followingPart = rawName_ + strlen(name);
	return (followingPart == findMinorSuffix(rawName_));
}

void GSInterceptor::FunctionInfo::formatName(std::ostream &stream) const {
	const GSChar *nameEnd = findMinorSuffix(rawName_);
	stream.write(rawName_, static_cast<std::streamsize>(nameEnd - rawName_));
}

bool GSInterceptor::FunctionInfo::isEmpty() const {
	const FunctionInfo &info = emptyInfo();
	return rawName_ == info.rawName_ && resourceType_ == info.resourceType_;
}

GSInterceptor::FunctionInfo GSInterceptor::FunctionInfo::emptyInfo() {
	return FunctionInfo(NULL, GSResourceType::TYPE_ID_MAX);
}

const GSChar* GSInterceptor::FunctionInfo::findMinorSuffix(
		const GSChar *rawName) {
	bool found = false;
	bool numebering = false;

	const GSChar *const begin = rawName;
	const GSChar *const end = begin + strlen(begin);
	const GSChar *it = end;
	while (it != begin) {
		--it;
		if (*it == '_' || ('0' <= *it && *it <= '9')) {
			numebering = true;
		}
		else {
			found = (numebering && *it == 'V');
			break;
		}
	}
	return (found ? it : end);
}

std::ostream& operator<<(
		std::ostream &stream, const GSInterceptor::FunctionInfo &funcInfo) {
	funcInfo.formatName(stream);
	return stream;
}


GSInterceptor::Parameter::Parameter(const GSChar *name) :
		name_(name),
		storage_(NULL) {
}

const GSChar* GSInterceptor::Parameter::getName() const {
	return name_;
}

bool GSInterceptor::Parameter::isResource() const {
	return getResourceType() != GSResourceType::TYPE_ID_MAX;
}

GSResourceType::Id GSInterceptor::Parameter::getResourceType() const {
	return typeInfo_.resourceType_;
}

void* GSInterceptor::Parameter::findResource() const {
	if (!isResource()) {
		return NULL;
	}

	return *static_cast<void**>(storage_);
}

bool GSInterceptor::Parameter::isString() const {
	return typeInfo_.forString_;
}

void GSInterceptor::Parameter::setNoString() {
	typeInfo_.forString_ = false;
}

bool GSInterceptor::Parameter::isSize() const {
	return typeInfo_.forSize_;
}

size_t& GSInterceptor::Parameter::getSize() const {
	return get<size_t>();
}

size_t*& GSInterceptor::Parameter::getSizePtr() const {
	return get<size_t*>();
}

void GSInterceptor::Parameter::formatValue(std::ostream &os) const {
	TypeInfo::FormatterFunc func = typeInfo_.formatterFunc_;

	if (typeInfo_.forString_) {
		func = &TypeInfo::stringFormatterFunc;
	}

	if (func == NULL) {
		assert(false);
		return;
	}

	(*func)(os, storage_);
}


GSInterceptor::Parameter::TypeInfo::TypeInfo() :
		resourceType_(GSResourceType::TYPE_ID_MAX),
		checkerFunc_(NULL),
		formatterFunc_(NULL),
		forString_(false),
		forSize_(false) {
}

void GSInterceptor::Parameter::TypeInfo::check(const TypeInfo &another) const {
	if (resourceType_ != another.resourceType_ ||
			checkerFunc_ != another.checkerFunc_) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}
}


template<
		typename P1, typename P2, typename P3, typename P4,
		typename P5, typename P6, typename P7, typename P8,
		typename P9, typename P10>
GSInterceptor::ParameterList::ParameterList(
		const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4,
		const P5 &p5, const P6 &p6, const P7 &p7, const P8 &p8,
		const P9 &p9, const P10 &p10, ...) :
		size_(0) {
	add(p1);
	add(p2);
	add(p3);
	add(p4);
	add(p5);
	add(p6);
	add(p7);
	add(p8);
	add(p9);
	checkTooManyArguments(p10);
}

size_t GSInterceptor::ParameterList::getSize() const {
	return size_;
}

const GSInterceptor::Parameter&
GSInterceptor::ParameterList::at(size_t index) const {
	if (index >= size_) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}
	return baseList_[index];
}

template<typename P>
void GSInterceptor::ParameterList::checkTooManyArguments(const P&) {
	UTIL_STATIC_ASSERT((util::IsSame<P, util::FalseType>::VALUE));
}

void GSInterceptor::ParameterList::add(const Parameter &param) {
	if (size_ >= MAX_PARAMETER_COUNT) {
		assert(false);
		return;
	}

	baseList_[size_] = param;

	if (param.isSize()) {
		Parameter &prevParam = baseList_[size_ - 1];
		if (prevParam.isString()) {
			prevParam.setNoString();
		}
	}

	++size_;
}

void GSInterceptor::ParameterList::add(const util::FalseType&) {
}


GSInterceptorManager::GSInterceptorManager() :
		entryCount_(0),
		mutex_(util::UTIL_MUTEX_RECURSIVE) {
}

bool GSInterceptorManager::start(
		const FunctionInfo &funcInfo, const ParameterList &args,
		const Parameter &ret, InterceptorId &endId) {
	CheckerScope checkerScope;
	util::LockGuard<util::Mutex> guard(mutex_);

	EntryList entryList;
	InterceptorId count;
	getActiveEntryList(guard, funcInfo.getResourceType(), entryList, count);
	for (InterceptorId i = 0; i < count; i++) {
		Entry &entry = entryList[i];
		if (entry.interceptor_->start(funcInfo, args, ret)) {
			endId = i + 1;
			return true;
		}
	}

	endId = count;
	return false;
}

void GSInterceptorManager::finish(
		const FunctionInfo &funcInfo, const ParameterList &args,
		InterceptorId endId) {
	CheckerScope checkerScope;
	util::LockGuard<util::Mutex> guard(mutex_);

	EntryList entryList;
	InterceptorId count;
	getActiveEntryList(guard, funcInfo.getResourceType(), entryList, count);

	for (InterceptorId i = 0; i < count; i++) {
		Entry &entry = entryList[i];
		if (entry.id_ >= endId) {
			break;
		}
		entry.interceptor_->finish(funcInfo, args);
	}
}

bool GSInterceptorManager::filterByResourceType(GSResourceType::Id type) {
	util::LockGuard<util::Mutex> guard(mutex_);

	EntryList entryList;
	InterceptorId count;
	getActiveEntryList(guard, type, entryList, count);

	return (count <= 0);
}

void GSInterceptorManager::activate(InterceptorId id, bool enabled) {
	util::LockGuard<util::Mutex> guard(mutex_);
	if (id >= entryCount_) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	Entry &entry = entryList_[id];
	if (entry.interceptor_ == NULL) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	entry.enabled_ = enabled;
}

GSInterceptorManager::InterceptorId
GSInterceptorManager::add(GSInterceptor &interceptor) {
	util::LockGuard<util::Mutex> guard(mutex_);
	for (InterceptorId i = 0; i < MAX_INTERCEPTOR_COUNT; i++) {
		Entry &entry = entryList_[i];
		if (entry.interceptor_ == NULL) {
			entry = Entry();
			entry.id_ = i;
			entry.interceptor_ = &interceptor;
			entryCount_ = static_cast<InterceptorId>(
					std::max<uint64_t>(entryCount_, i + 1));
			return entry.id_;
		}
	}

	assert(false);
	GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
}

void GSInterceptorManager::remove(InterceptorId id) {
	if (id >= MAX_INTERCEPTOR_COUNT) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	util::LockGuard<util::Mutex> guard(mutex_);
	entryList_[id] = Entry();
}

void GSInterceptorManager::getActiveEntryList(
		util::LockGuard<util::Mutex>&, GSResourceType::Id type,
		EntryList &entryList, InterceptorId &count) {
	count = 0;
	for (InterceptorId i = 0; i < entryCount_; i++) {
		Entry &entry = entryList_[i];
		if (entry.enabled_ && entry.interceptor_ != NULL &&
				(type == GSResourceType::TYPE_ID_MAX ||
				entry.interceptor_->isActivated(type))) {
			entryList[count] = entry;
			count++;
		}
	}
}


GSInterceptorManager::Scope::Scope() throw() {
}

GSInterceptorManager::Scope::~Scope() {
	if (data_.get() != NULL) {
		try {
			data_->manager_.finish(
					data_->funcInfo_, data_->args_, data_->endId_);
		}
		catch (...) {
			setCurrentException(data_->args_);
		}
	}
}

bool GSInterceptorManager::Scope::set(
		GSInterceptorManager &manager, const FunctionInfo &funcInfo,
		const ParameterList &args, const Parameter &ret) throw() {
	if (data_.get() != NULL) {
		assert(false);
		return true;
	}

	try {
		data_.reset(new Data(manager, funcInfo, args));
		return data_->manager_.start(
				data_->funcInfo_, data_->args_, ret, data_->endId_);
	}
	catch (...) {
		setCurrentException(data_->args_);
		return true;
	}
}

void GSInterceptorManager::Scope::setCurrentException(
		const ParameterList &args) throw() try {
	if (args.getSize() <= 0) {
		return;
	}

	void *resource = args.at(0).findResource();
	GSResourceHeader::setCurrentException(resource);
}
catch (...) {
	assert(false);
}


GSInterceptorManager::Scope::Data::Data(
		GSInterceptorManager &manager, const FunctionInfo &funcInfo,
		const ParameterList &args) :
		manager_(manager),
		funcInfo_(funcInfo),
		args_(args),
		endId_(0) {
}


GSInterceptorManager::Entry::Entry() :
		id_(std::numeric_limits<InterceptorId>::max()),
		interceptor_(NULL),
		enabled_(false) {
}


GSInterceptorManager::CheckerScope::CheckerScope() {
	if (counter_ != 0) {
		(*abortByResursiveAccessFunc_)();
	}
	++counter_;
}

GSInterceptorManager::CheckerScope::~CheckerScope() {
	--counter_;
}

void GSInterceptorManager::CheckerScope::abortByResursiveAccess() {
	assert(false);
	abort();
}

void (*const volatile
		GSInterceptorManager::CheckerScope::abortByResursiveAccessFunc_)() =
		&abortByResursiveAccess;

UTIL_THREAD_LOCAL size_t GSInterceptorManager::CheckerScope::counter_ = 0;


GSGridStoreFactory *GSGridStoreFactoryTag::defaultFactory_ = NULL;

GSGridStoreFactoryTag::GSGridStoreFactoryTag() throw() :
		resourceHeader_(GSResourceType::GRID_STORE_FACTORY, NULL, NULL) {
	try {
#if !GS_CLIENT_DISABLE_DEFAULT_FACTORY
		data_.reset(new Data);
#endif
	}
	catch (...) {
	}
}

GSGridStoreFactoryTag::~GSGridStoreFactoryTag() try {
	if (data_.get() == NULL) {
		return;
	}

	for (ChannelMap::iterator it = data_->channelMap_.begin();
			it != data_->channelMap_.end(); ++it) {
		delete it->second;
	}
	data_->channelMap_.clear();
}
catch (...) {
}

bool GSGridStoreFactoryTag::isAlive() throw() {
	return (defaultFactory_ != NULL);
}

GSGridStoreFactory& GSGridStoreFactoryTag::getDefaultFactory() throw() {
	defaultFactory_->prepareConfigFile();
	return *defaultFactory_;
}

void GSGridStoreFactoryTag::close(
		GSGridStoreFactory **factory, bool allRelated) throw() {
	if (factory == NULL) {
		return;
	}

	static_cast<void>(allRelated);
	do {
		if (!GSResourceHeader::checkType(
				*factory, GSResourceType::GRID_STORE_FACTORY)) {
			break;
		}

		if (*factory == defaultFactory_) {
			return;
		}

		delete *factory;
	}
	while (false);
	*factory = NULL;
}

GSGridStore* GSGridStoreFactoryTag::getGridStore(
		const GSPropertyEntry *properties, const size_t *propertyCount) {
	if (data_.get() == NULL) {
		return NULL;
	}
	util::LockGuard<util::Mutex> guard(data_->mutex_);

	Properties propertiesObj(properties, propertyCount);
	data_->configLoader_.applyStoreConfig(propertiesObj);

	GridStoreChannel::Source source;
	source.set(propertiesObj);
	source.interceptorManager_ = &data_->interceptorManager_;
	ChannelMap::iterator it = data_->channelMap_.find(source.getKey());

	GridStoreChannel *channel;
	if (it == data_->channelMap_.end()) {
		std::auto_ptr<GridStoreChannel> channelPtr(
				new GridStoreChannel(data_->channelConfig_, source));
		channel = channelPtr.get();
		channel->setMonitoring(data_->monitoring_);

		data_->channelMap_.insert(std::make_pair(source.getKey(), channel));
		channelPtr.release();
	}
	else {
		channel = it->second;
		channel->apply(data_->channelConfig_);
	}

	return new GSGridStore(*channel, source.createContext());
}

void GSGridStoreFactoryTag::setProperties(
		const GSPropertyEntry *properties, const size_t *propertyCount,
		bool forInitial) {
	if (data_.get() == NULL) {
		return;
	}

	util::LockGuard<util::Mutex> guard(data_->mutex_);
	setPropertiesInternal(guard, forInitial, properties, propertyCount);
}

void GSGridStoreFactoryTag::prepareConfigFile() throw() try {
	if (data_.get() == NULL) {
		return;
	}
	util::LockGuard<util::Mutex> guard(data_->mutex_);
	if (data_->configLoader_.isPrepared()) {
		return;
	}

	const size_t count = 0;
	try {
		setPropertiesInternal(guard, true, NULL, &count);
	}
	catch (...) {
		std::exception e;
		data_->configLoader_.handleConfigError(e);
	}
}
catch (...) {
}

void GSGridStoreFactoryTag::setPropertiesInternal(
		util::LockGuard<util::Mutex> &guard, bool forInitial,
		const GSPropertyEntry *properties, const size_t *propertyCount) {

	Properties propertiesObj(properties, propertyCount);
	data_->configLoader_.applyFactoryConfig(propertiesObj);

	setLoggingProperties(guard, propertiesObj, forInitial);

	bool detailErrorMessageEnabled;
	if (propertiesObj.getBool(
			"detailErrorMessageEnabled", detailErrorMessageEnabled)) {
		ErrorStack::setDetailErrorMessageEnabled(detailErrorMessageEnabled);
	}

	int32_t transactionProtocolVersion;
	if (propertiesObj.getInteger(
			"transactionProtocolVersion", transactionProtocolVersion)) {
		NodeConnection::setProtocolVersion(transactionProtocolVersion);
	}

	bool pathKeyOperationEnabled;
	if (propertiesObj.getBool(
			"pathKeyOperationEnabled", pathKeyOperationEnabled)) {
		GSGridStore::setPathKeyOperationEnabled(pathKeyOperationEnabled);
	}

	if (data_->channelConfig_.set(propertiesObj)) {
		for (ChannelMap::iterator it = data_->channelMap_.begin();
				it != data_->channelMap_.end(); ++it) {
			GridStoreChannel &channel = *it->second;
			channel.apply(data_->channelConfig_);
		}
	}
}

void GSGridStoreFactoryTag::setLoggingProperties(
		util::LockGuard<util::Mutex> &guard, const Properties &properties,
		bool forInitial) {
	static_cast<void>(guard);
	static_cast<void>(properties);
	static_cast<void>(forInitial);
}

void GSGridStoreFactoryTag::setMonitoring(
		util::LockGuard<util::Mutex>&, bool monitoring) {
	data_->monitoring_ = monitoring;
	for (ChannelMap::iterator it = data_->channelMap_.begin();
			it != data_->channelMap_.end(); ++it) {
		it->second->setMonitoring(monitoring);
	}
}


size_t GSGridStoreFactoryTag::Initializer::counter_ = 0;

GSGridStoreFactoryTag::Initializer::Initializer() throw() {
	if (counter_++ == 0) {
		try {
			defaultFactory_ = new GSGridStoreFactory();
		}
		catch (...) {
		}
	}
}

GSGridStoreFactoryTag::Initializer::~Initializer() {
	if (--counter_ == 0) {
		delete defaultFactory_;
	}
}


const char8_t GSGridStoreFactoryTag::ConfigLoader::CONFIG_FILE_NAME[] =
		"gs_client.properties";

GSGridStoreFactoryTag::ConfigLoader::ConfigLoader() :
		configFileEnabled_(false),
		errorOccurred_(false),
		prepared_(false) {
}

bool GSGridStoreFactoryTag::ConfigLoader::isPrepared() const {
	return prepared_;
}

void GSGridStoreFactoryTag::ConfigLoader::applyFactoryConfig(
		Properties &props) {
	prepare();
	applyConfig(factoryProps_.get(), props);
}

void GSGridStoreFactoryTag::ConfigLoader::applyStoreConfig(Properties &props) {
	prepare();
	applyConfig(storeProps_.get(), props);
}

void GSGridStoreFactoryTag::ConfigLoader::handleConfigError(
		std::exception&) throw() {
	if (errorOccurred_) {
		return;
	}

	errorOccurred_ = true;
	try {
		throw;
	}
	catch (...) {
		try {
			initialError_.reset(new ErrorStack());
			initialError_->setFromCurrentException();
		}
		catch (...) {
		}
	}
}

void GSGridStoreFactoryTag::ConfigLoader::prepare() {
	if (errorOccurred_) {
		if (initialError_.get() != NULL && initialError_->getSize() > 0) {
			try {
				throw initialError_->getException();
			}
			catch (...) {
				std::exception e;
				GS_CLIENT_RETHROW_ERROR(e, "");
			}
		}
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_CONFIG, "");
	}

	if (prepared_) {
		return;
	}

	prepared_ = true;
}

void GSGridStoreFactoryTag::ConfigLoader::applyConfig(
		const Properties *src, Properties &dest) {
	if (src == NULL) {
		return;
	}

	for (PropertyMap::const_iterator it = src->propertyMap_.begin();
			it != src->propertyMap_.end(); ++it) {
		dest.propertyMap_.insert(*it);
	}
}

void GSGridStoreFactoryTag::ConfigLoader::acceptFileData(
		std::string &buf, bool eof, uint64_t &lineNumber) {
	const char8_t *const delimList[] = { "\r\n", "\r", "\n", NULL };
	while (!buf.empty()) {
		bool found = false;
		for (const char8_t *const *delimIt = delimList;; ++delimIt) {
			std::string::size_type pos;
			const char8_t *delim;
			if (*delimIt == NULL) {
				pos = (eof ? buf.size() : std::string::npos);
				delim = "";
			}
			else {
				pos = buf.find(*delimIt);
				delim = *delimIt;
			}

			if (pos != std::string::npos) {
				acceptFileLine(buf.data(), pos);
				buf.erase(buf.begin(), buf.begin() + pos + strlen(delim));
				lineNumber++;
				found = true;
				break;
			}
			else if (*delimIt == NULL) {
				break;
			}
		}
		if (!found) {
			break;
		}
	}
}

void GSGridStoreFactoryTag::ConfigLoader::acceptFileLine(
		const char8_t *line, size_t size) {
	const char8_t *keyBegin = NULL;
	const char8_t *sepBegin = NULL;
	const char8_t *sepEnd = NULL;
	const char8_t *sepMid = NULL;
	bool ignorable = false;
	bool escaping = false;

	const char8_t *const end = line + size;
	for (const char8_t *it = line; it != end; ++it) {
		if (*it == '\\') {
			escaping = !escaping;
			continue;
		}

		const uint32_t ch = static_cast<uint32_t>(*it);
		if (escaping) {
			escaping = false;
			if (strchr("rn tf!#=:\t\f\\", *it) != NULL) {
				continue;
			}
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_CONFIG,
					"Illegal escape sequence (char=0x" << std::hex << ch << ")");
		}

		const bool space = (strchr(" \t\f", *it) != NULL);
		if (!space && (ch <= 0x1f || ch >= 0x7f)) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_CONFIG,
					"Illegal character found (char=0x" << std::hex << ch << ")");
		}

		if (it == line && (strchr("!#", *it) != NULL)) {
			ignorable = true;
		}

		if (ignorable || sepEnd != NULL) {
			continue;
		}

		const bool sep = (strchr("=:", *it) != NULL);
		if (keyBegin == NULL) {
			if (space) {
				continue;
			}
			keyBegin = it;
		}

		if (sepBegin == NULL) {
			if (!space && !sep) {
				continue;
			}
			sepBegin = it;
		}

		if (sepMid == NULL) {
			if (sep) {
				sepMid = it;
				continue;
			}
		}

		if (sep || !space) {
			sepEnd = it;
		}
	}

	if (ignorable || keyBegin == NULL) {
		return;
	}

	if (escaping) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_CONFIG,
				"Illegal escape sequence with line separator");
	}

	if (sepBegin == NULL) {
		sepBegin = end;
	}
	if (sepEnd == NULL) {
		sepEnd = end;
	}

	const std::string name(keyBegin, sepBegin);
	const std::string value(sepEnd, end);
	acceptProperty(unescape(name).c_str(), unescape(value).c_str());
}

void GSGridStoreFactoryTag::ConfigLoader::acceptProperty(
		const char8_t *name, const char8_t *value) {
	typedef std::pair<const char8_t*, UTIL_UNIQUE_PTR<Properties>*> Entry;
	const Entry endEntry = Entry();
	const Entry entryList[] = {
		Entry("factory.", &factoryProps_),
		Entry("store.", &storeProps_),
		endEntry
	};

	for (const Entry *it = entryList; *it != endEntry; ++it) {
		const char8_t *prefix = it->first;
		UTIL_UNIQUE_PTR<Properties> &props = *it->second;
		if (strstr(name, prefix) == name) {
			const std::string subName(name + strlen(prefix));
#if GS_CLIENT_CONFIG_FILE_LOGGING_ONLY
			if (&props != &factoryProps_ || subName.find("trace.") != 0) {
				return;
			}
#endif

			if (props.get() == NULL) {
				props.reset(new Properties(NULL, NULL));
			}
			props->propertyMap_[subName] = value;
			return;
		}
	}

	GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_CONFIG,
			"Illegal property name (name=" << name << ")");
}

std::string GSGridStoreFactoryTag::ConfigLoader::unescape(
		const std::string &src) {
	const char8_t escChars[] = "rntf";
	const char8_t outChars[] = "\r\n\t\f";
	UTIL_STATIC_ASSERT(sizeof(escChars) == sizeof(outChars));

	bool escaping = false;
	std::string dest;
	for (std::string::const_iterator it = src.begin(); it != src.end(); ++it) {
		if (escaping) {
			const char8_t *ret = strchr(escChars, *it);
			if (ret != NULL) {
				dest.push_back(outChars[ret - escChars]);
				continue;
			}
			escaping = false;
		}
		else if (*it == '\\') {
			escaping = true;
			continue;
		}
		dest.push_back(*it);
	}

	return dest;
}


GSGridStoreFactoryTag::Data::Data() :
		monitoring_(false) {
	setUpInterceptors();
}

void GSGridStoreFactoryTag::Data::setUpInterceptors() {
}


const GSChar *const GSGridStoreTag::CONTEXT_CONTROLLER_NAME =
		"##internal.contextController";
bool GSGridStoreTag::pathKeyOperationEnabled_ = false;

RowMapper::Config GSGridStoreTag::DEFAULT_MAPPER_CONFIG(false, true, true);
RowMapper::Config GSGridStoreTag::COMPATIBLE_MAPPER_CONFIG_14(
		false, true, false);
RowMapper::Config GSGridStoreTag::COMPATIBLE_MAPPER_CONFIG_13(
		false, false, false);

GSGridStoreTag::GSGridStoreTag(
		GridStoreChannel &channel, GridStoreChannel::Context context) :
		resourceHeader_(
				GSResourceType::GRID_STORE, NULL,
				channel.getInterceptorManager()),
		referenceCount_(1),
		channel_(channel),
		context_(context),
		req_(context_.getRequestBuffer()),
		resp_(context_.getResponseBuffer()){
	channel_.initializeRequestBuffer(req_);
	channel_.initializeResponseBuffer(resp_);
}

GSGridStoreTag::~GSGridStoreTag() {
}

void GSGridStoreTag::setPathKeyOperationEnabled(bool enabled) {
	pathKeyOperationEnabled_ = enabled;
}

void GSGridStoreTag::close(GSGridStore **store, bool allRelated) throw() {
	if (store == NULL) {
		return;
	}

	do {
		if (!GSResourceHeader::checkType(*store, GSResourceType::GRID_STORE)) {
			break;
		}

		size_t &referenceCount = (**store).referenceCount_;
		try {
			ContainerCache *cache = (**store).context_.getContainerCache();
			if (cache != NULL) {
				std::vector<GridStoreChannel::SessionInfo> sessionList;
				cache->takeAllSessions(sessionList);
				if (!sessionList.empty()) {
					GSContainer::closeAllSessions(
							(**store).channel_, (**store).context_,
							&sessionList[0], sessionList.size());
				}
			}
		}
		catch (...) {
		}

		try {
			ResourceSet &resourceSet = (**store).activeResources_;

			for (ResourceSet::iterator it = resourceSet.begin();
					it != resourceSet.end(); ++it) {
				void *resource = *it;

				GSResourceType::Id type;
				if (!GSResourceHeader::getType(resource, type)) {
					assert(false);
				}
				if (type == GSResourceType::CONTAINER) {
					static_cast<GSContainer*>(resource)->closeRemote();
				}
			}

			if (allRelated && !resourceSet.empty()) {
				size_t orgSize;
				do {
					orgSize = resourceSet.size();
					void *resource = *(--resourceSet.end());

					GSResourceType::Id type;
					if (!GSResourceHeader::getType(resource, type)) {
						assert(false);
					}

					assert(referenceCount > 1);
					if (type == GSResourceType::CONTAINER) {
						GSContainer *container =
								static_cast<GSContainer*>(resource);
						GSContainer::close(&container, true);
					}
					else if (type == GSResourceType::ROW) {
						GSRow *row = static_cast<GSRow*>(resource);
						GSRow::close(&row);
					}
					else if (type == GSResourceType::ROW_KEY_PREDICATE) {
						GSRowKeyPredicate *predicate =
								static_cast<GSRowKeyPredicate*>(resource);
						GSRowKeyPredicate::close(&predicate);
					}
					else if (type == GSResourceType::PARTITION_CONTROLLER) {
						GSPartitionController *controller =
								static_cast<GSPartitionController*>(resource);
						GSPartitionController::close(&controller);
					}
					else {
						assert(false);
					}
					assert(referenceCount > 0);
				}
				while (!resourceSet.empty() && orgSize != resourceSet.size());
				assert(resourceSet.empty());
			}
		}
		catch (...) {
		}

		try {
			(**store).channel_.clearContext((**store).context_, true);
		}
		catch (...) {
		}

		assert(referenceCount > 0);
		if (--referenceCount == 0) {
			delete *store;
		}
	}
	while (false);
	*store = NULL;
}

void GSGridStoreTag::createReference(void *resource) {
	GSResourceType::Id type;
	if (!GSResourceHeader::getType(resource, type)) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	switch (type) {
	case GSResourceType::CONTAINER:
	case GSResourceType::ROW:
	case GSResourceType::ROW_KEY_PREDICATE:
	case GSResourceType::PARTITION_CONTROLLER:
		break;
	case GSResourceType::GRID_STORE_FACTORY:
	case GSResourceType::GRID_STORE:
	case GSResourceType::QUERY:
	case GSResourceType::ROW_SET:
	case GSResourceType::AGGREGATION_RESULT:
	default:
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	const ContainerKey *containerKey = NULL;
	if (pathKeyOperationEnabled_) {
		if (type == GSResourceType::CONTAINER) {
			GSContainer &container = *static_cast<GSContainer*>(resource);
			containerKey = container.getNormalizedContainerKey();
			if (containerKey != NULL) {
				containerMap_.insert(
						std::make_pair(*containerKey, &container));
			}
		}
	}

	try {
		activeResources_.insert(resource);
	}
	catch (...) {
		if (containerKey != NULL) {
			containerMap_.erase(*containerKey);
		}
		throw;
	}
	referenceCount_++;
}

void GSGridStoreTag::removeReference(GSGridStore *&store, void *resource) {
	if (store == NULL) {
		return;
	}

	try {
		{
			ResourceSet &resourceSet = store->activeResources_;
			ResourceSet::iterator it = resourceSet.find(resource);
			if (it != resourceSet.end()) {
				resourceSet.erase(it);
			}
			else {
				assert(false);
			}
		}

		do {
			if (!pathKeyOperationEnabled_) {
				break;
			}

			GSResourceType::Id type;
			if (!GSResourceHeader::getType(resource, type)) {
				assert(false);
			}
			if (type != GSResourceType::CONTAINER) {
				break;
			}
			GSContainer &container = *static_cast<GSContainer*>(resource);

			ContainerMap &containerMap = store->containerMap_;
			std::pair<ContainerMap::iterator, ContainerMap::iterator> range =
					containerMap.equal_range(
							*container.getNormalizedContainerKey());
			bool found = false;
			for (ContainerMap::iterator it = range.first; it != range.second; ++it) {
				if (it->second == &container) {
					containerMap.erase(it);
					found = true;
					break;
				}
			}
			assert(found);
		}
		while (false);
	}
	catch (...) {
	}

	assert(store->referenceCount_ > 0);
	if (--store->referenceCount_ == 0) {
		delete store;
	}
	store = NULL;
}

GridStoreChannel& GSGridStoreTag::getChannel() {
	return channel_;
}

GridStoreChannel::Context& GSGridStoreTag::getContext() {
	return context_;
}

util::NormalXArray<uint8_t>& GSGridStoreTag::getRequestBuffer() {
	return req_;
}

util::NormalXArray<uint8_t>& GSGridStoreTag::getResponseBuffer() {
	return resp_;
}

RowMapper::VarDataPool& GSGridStoreTag::getVarDataPool() {
	return varDataPool_;
}

bool GSGridStoreTag::getContainerInfo(
		const GSChar *name, RowMapper::ContainerInfoRef<> &infoRef,
		const ContainerPropertyKeySet *propKeySet,
		RowMapper::VarDataPool *varDataPool,
		ContainerIdInfo *idInfo) {
	infoRef.clear();

	if (varDataPool == NULL) {
		varDataPool = &varDataPool_;
	}
	varDataPool->clear();

	const ContainerKeyConverter &keyConverter = context_.getKeyConverter();
	const ContainerKey &key = keyConverter.parse(name, false);

	const int32_t partitionId =
			channel_.resolvePartitionId(context_, key, keyConverter);
	channel_.applyPartitionId(context_, partitionId);

	XArrayByteOutStream reqOut = channel_.getRequestOutStream(req_);
	NodeConnection::tryPutEmptyOptionalRequest(reqOut);

	putContainerKey(reqOut, channel_, context_, key, keyConverter);

	if (propKeySet == NULL) {
		if (isIndexDetailEnabled()) {
			propKeySet = &ContainerPropertyKeySet::KEYS_DEFAULT;
		}
		else if (NodeConnection::getProtocolVersion() < 2) {
			propKeySet = &ContainerPropertyKeySet::KEYS_COMPATIBLE_TRIGGER;
		}
		else {
			propKeySet = &ContainerPropertyKeySet::KEYS_COMPATIBLE_INDEX;
		}
	}

	ContainerIdInfo idInfoStorage;
	if (idInfo == NULL) {
		idInfo = &idInfoStorage;
	}

	const uint32_t propertyCount = propKeySet->size();
	reqOut << ClientUtil::sizeValueToInt32(propertyCount);
	ContainerPropertyKey propKey = propKeySet->begin();
	for (uint32_t i = 0; i < propertyCount; i++) {
		assert(propKey != CONTAINER_PROPERTY_KEY_MAX);

		reqOut << static_cast<int8_t>(propKey);
		propKey = propKeySet->next(propKey);
	}

	ArrayByteInStream respIn = channel_.executeStatement(
			context_, Statement::GET_CONTAINER_PROPERTIES, 0, req_, resp_);

	GSBool exists;
	respIn >> exists;
	if (!exists) {
		return false;
	}

	int32_t resultPropertyCount;
	respIn >> resultPropertyCount;
	if (static_cast<size_t>(resultPropertyCount) != propertyCount) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by illegal property count");
	}

	GSContainerInfo info = GS_CONTAINER_INFO_INITIALIZER;
	std::vector<GSColumnInfo> columnInfoList;
	std::vector<GSIndexInfo> indexInfoList;
	const bool withContainerType = true;
	bool indexFlagsSpecified = false;

	for (size_t i = 0; i < propertyCount; i++) {
		int8_t propertyType;
		respIn >> propertyType;

		int32_t propertySize;
		respIn >> propertySize;
		if (propertySize < 0 ||
				static_cast<size_t>(propertySize) > respIn.base().remaining()) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
					"Protocol error by illegal property size");
		}

		const size_t curEnd =
				respIn.base().position() + static_cast<size_t>(propertySize);

		switch (propertyType) {
		case CONTAINER_PROPERTY_ID:
			importIdProperty(respIn, *varDataPool, *idInfo, keyConverter);
			info.name = idInfo->remoteName_;
			break;
		case CONTAINER_PROPERTY_SCHEMA:
			importSchemaProperty(
					respIn, getRowMapperConfig(), *varDataPool, info,
					columnInfoList, withContainerType);
			importContainerProperties(
					respIn, *varDataPool, info, columnInfoList);
			break;
		case CONTAINER_PROPERTY_INDEX:
			importIndexProperty(respIn, columnInfoList);
			indexFlagsSpecified = true;
			break;
		case CONTAINER_PROPERTY_EVENT_NOTIFICATION:
			importEventNotificationProperty(respIn, *varDataPool, NULL);
			break;
		case CONTAINER_PROPERTY_TRIGGER:
			importTriggerProperty(respIn, *varDataPool, info, columnInfoList);
			break;
		case CONTAINER_PROPERTY_INDEX_DETAIL:
			importIndexDetailProperty(respIn, *varDataPool, indexInfoList);
			break;
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
					"Protocol error by illegal property type");
		}

		if (respIn.base().position() != curEnd) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
					"Protocol error by illegal property format");
		}
	}

	if (!indexInfoList.empty() && !columnInfoList.empty()) {
		applyIndexInfoList(
				indexInfoList, columnInfoList, indexFlagsSpecified, *varDataPool,
				info);
	}

	infoRef.set(info);

	if (!columnInfoList.empty()) {
		infoRef.createColumnInfoList(columnInfoList.size(), *varDataPool);

		for (std::vector<GSColumnInfo>::iterator it = columnInfoList.begin();
				it != columnInfoList.end(); ++it) {
			infoRef.setColumnInfo(
					static_cast<int32_t>(it - columnInfoList.begin()), *it);
		}
	}

	return true;
}

GSResult GSGridStoreTag::getContainerInfo(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists, const ClientVersion &version,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	RowMapper::ContainerInfoRef<>::clear(info, version);

	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(exists, GS_FALSE);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(funcInfo, store, name, info, exists);

	GSBool existsStorage;
	if (exists == NULL) {
		exists = &existsStorage;
	}
	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(name);
		GS_CLIENT_CHECK_NOT_NULL(info);

		RowMapper::ContainerInfoRef<> infoRef(info, version);
		*exists = store->getContainerInfo(
				name, infoRef, NULL, &store->varDataPool_, NULL);
	}
	catch (...) {
		*exists = GS_FALSE;
		RowMapper::ContainerInfoRef<>::clear(info, version);
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSContainer* GSGridStoreTag::getContainer(
		const GSChar *name, const GSBinding &binding,
		GSContainerType containerType) {
	const ContainerKeyConverter &keyConverter = context_.getKeyConverter();
	const ContainerKey &key = keyConverter.parse(name, false);

	ContainerCache *cache = context_.getContainerCache();
	if (cache != NULL) {
		GSCollection *container = findContainerByCache(
				*cache, key, keyConverter, &binding, &containerType, false);
		if (container != NULL) {
			return container;
		}
	}

	XArrayByteOutStream reqOut = channel_.getRequestOutStream(req_);
	NodeConnection::tryPutEmptyOptionalRequest(reqOut);

	RowMapper::Cache &mapperCache = RowMapper::getDefaultCache();
	const RowMapper::RowTypeCategory category =
			RowMapper::containerTypeToCategory(containerType);
	const RowMapper::Reference orgMapper(
			mapperCache, mapperCache.resolve(
					category, &binding, false, getRowMapperConfig()));
	const int32_t partitionId =
			channel_.resolvePartitionId(context_, key, keyConverter);

	putContainerKey(reqOut, channel_, context_, key, keyConverter);
	tryPutContainerType(reqOut, &containerType);

	const Statement::Id statement = getContainerStatement(
			Statement::GET_CONTAINER, &containerType);

	channel_.applyPartitionId(context_, partitionId);
	ArrayByteInStream respIn = channel_.executeStatement(
			context_, statement, 0, req_, resp_);

	GSBool exists;
	respIn >> exists;
	if (!exists) {
		return NULL;
	}

	int32_t schemaVerId;
	respIn >> schemaVerId;

	int64_t containerId;
	respIn >> containerId;

	bool cached;
	std::auto_ptr<ContainerKey> remoteKey =
			acceptRemoteContainerKey(&respIn, key, keyConverter, cached);

	RowMapper::Reference mapper(
			mapperCache, mapperCache.resolve(
					*orgMapper.get(), respIn, getRowMapperConfig(), true));

	if (cache != NULL) {
		cache->cacheSchema(*remoteKey, *mapper, containerId, schemaVerId);
	}

	return new GSContainer(
			*this, mapper, schemaVerId, partitionId, containerId, remoteKey,
			cached);
}

GSCollection* GSGridStoreTag::getCollection(
		const GSChar *name, const GSBinding &binding) {
	return getContainer(name, binding, GS_CONTAINER_COLLECTION);
}

GSTimeSeries* GSGridStoreTag::getTimeSeries(
		const GSChar *name, const GSBinding &binding) {
	return getContainer(name, binding, GS_CONTAINER_TIME_SERIES);
}

GSContainer* GSGridStoreTag::putContainer(
		const GSChar *name, const GSBinding &binding,
		const RowMapper::ContainerInfoRef<true> &containerInfoRef,
		GSBool modifiable, const GSContainerType *containerType) {
	GSContainerInfo containerInfoStorage;
	GSContainerInfo *containerInfo = NULL;

	if (containerInfoRef.hasRef()) {
		containerInfo = &containerInfoStorage;
		containerInfoRef.get(*containerInfo);
	}

	name = resolveContainerName(name, containerInfo);
	containerType = resolveContainerType(containerType, containerInfo);

	const ContainerKeyConverter &keyConverter = context_.getKeyConverter();
	const ContainerKey &key = keyConverter.parse(name, false);

	ContainerCache *cache = context_.getContainerCache();
	if (cache != NULL && !modifiable && containerInfo == NULL) {
		GSCollection *container = findContainerByCache(
				*cache, key, keyConverter, &binding, containerType, false);
		if (container != NULL) {
			return container;
		}
	}

	XArrayByteOutStream reqOut = channel_.getRequestOutStream(req_);

	RowMapper::Cache &mapperCache = RowMapper::getDefaultCache();
	const RowMapper::RowTypeCategory category =
			RowMapper::containerTypeToCategory(*containerType);
	const RowMapper::Reference orgMapper(
			mapperCache, mapperCache.resolve(
					category, &binding, false, getRowMapperConfig()));

	const ContainerPropertiesOption &propsOption =
			containerPropertiesToOption(*orgMapper);
	tryPutSystemOptionalRequest(reqOut, context_, true, propsOption.get());

	const int32_t partitionId =
			channel_.resolvePartitionId(context_, key, keyConverter);

	putContainerKey(reqOut, channel_, context_, key, keyConverter);
	tryPutContainerType(reqOut, containerType);
	reqOut << modifiable;
	orgMapper.get()->exportSchema(reqOut, getRowMapperConfig());

	if (containerInfo != NULL &&
			(containerInfoRef.getColumnCount() > 0 ||
			containerInfo->rowKeyAssigned ||
			containerInfo->columnOrderIgnorable)) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA,
				"Schema can not be specified on ContainerInfo");
	}
	exportContainerProperties(
			reqOut, *containerType, containerInfo, *orgMapper);

	const Statement::Id statement = getContainerStatement(
			Statement::PUT_CONTAINER, containerType);

	channel_.applyPartitionId(context_, partitionId);
	ArrayByteInStream respIn = channel_.executeStatement(
			context_, statement, 0, req_, resp_);

	int32_t schemaVerId;
	respIn >> schemaVerId;

	int64_t containerId;
	respIn >> containerId;

	bool cached;
	std::auto_ptr<ContainerKey> remoteKey =
			acceptRemoteContainerKey(&respIn, key, keyConverter, cached);

	RowMapper::Reference mapper(
			mapperCache, mapperCache.resolve(
					*orgMapper.get(), respIn, getRowMapperConfig(), true));

	if (cache != NULL) {
		cache->cacheSchema(*remoteKey, *mapper, containerId, schemaVerId);
	}

	return new GSContainer(
			*this, mapper, schemaVerId, partitionId, containerId, remoteKey,
			cached);
}

GSResult GSGridStoreTag::putContainer(
		GSGridStore *store, const GSChar *name, bool nameRequired,
		const GSBinding *binding, const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container,
		const GSContainerType *containerType,
		const ClientVersion &version,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(container, NULL);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(
			funcInfo, store, name, binding, info, modifiable, container, containerType);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		if (nameRequired) {
			GS_CLIENT_CHECK_NOT_NULL(name);
		}
		GS_CLIENT_CHECK_NOT_NULL(binding);
		GS_CLIENT_CHECK_NOT_NULL(container);

		*container = store->putContainer(
				name, *binding, RowMapper::toInfoRef(info, version),
				modifiable, containerType);
	}
	catch (...) {
		if (container != NULL) {
			*container = NULL;
		}
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSContainer* GSGridStoreTag::getContainer(
		const GSChar *name, const GSContainerType *expectedType) {
	if (strcmp(name, CONTEXT_CONTROLLER_NAME) == 0) {
		GSCollection *collection =
				getContextControllerCollection(expectedType);
		if (collection != NULL) {
			return collection;
		}
	}

	const ContainerKeyConverter &keyConverter = context_.getKeyConverter();
	const ContainerKey &key = keyConverter.parse(name, false);

	ContainerCache *cache = context_.getContainerCache();
	if (cache != NULL) {
		GSCollection *container = findContainerByCache(
				*cache, key, keyConverter, NULL, expectedType, true);
		if (container != NULL) {
			return container;
		}
	}

	GSContainerInfo info;
	RowMapper::ContainerInfoRef<> infoRef(&info, ClientVersion());
	ContainerIdInfo idInfo;
	RowMapper::VarDataPool varDataPool;
	if (!getContainerInfo(
			name, infoRef, &ContainerPropertyKeySet::KEYS_FOR_OBJECT,
			&varDataPool, &idInfo)) {
		return NULL;
	}

	if (expectedType != NULL && *expectedType != info.type) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA,
				"Container type unmatched");
	}

	bool cached;
	std::auto_ptr<ContainerKey> remoteKey =
			acceptRemoteContainerKey(NULL, key, keyConverter, cached);

	const int32_t partitionId =
			channel_.resolvePartitionId(context_, key, keyConverter);
	RowMapper::Cache &mapperCache = RowMapper::getDefaultCache();
	RowMapper::Reference mapper(mapperCache, mapperCache.resolve(
			infoRef.toConst(), getRowMapperConfig()));

	if (cache != NULL) {
		cache->cacheSchema(
				*remoteKey, *mapper, idInfo.containerId_, idInfo.versionId_);
	}

	return new GSContainer(
			*this, mapper, idInfo.versionId_,
			partitionId, idInfo.containerId_, remoteKey, cached);
}

GSContainer* GSGridStoreTag::putContainer(
		const GSChar *name,
		const RowMapper::ContainerInfoRef<true> &containerInfoRef,
		GSBool modifiable, const GSContainerType *containerType) {
	GSContainerInfo containerInfo;
	containerInfoRef.get(containerInfo);

	name = resolveContainerName(name, &containerInfo);
	containerType = resolveContainerType(containerType, &containerInfo);

	const ContainerKeyConverter &keyConverter = context_.getKeyConverter();
	const ContainerKey &key = keyConverter.parse(name, false);

	ContainerCache *cache = context_.getContainerCache();
	if (cache != NULL && !modifiable &&
			containerInfo.timeSeriesProperties != NULL) {
		std::vector<GSBindingEntry> entryList;
		const GSBinding &binding =
				GSRow::createBinding(containerInfoRef, entryList);

		GSCollection *container = findContainerByCache(
				*cache, key, keyConverter, &binding, containerType, true);
		if (container != NULL) {
			return container;
		}
	}

	XArrayByteOutStream reqOut = channel_.getRequestOutStream(req_);

	RowMapper::Cache &mapperCache = RowMapper::getDefaultCache();
	const RowMapper::Reference orgMapper(
			mapperCache, mapperCache.resolve(
					containerInfoRef, getRowMapperConfig()));

	const ContainerPropertiesOption &propsOption =
			containerPropertiesToOption(*orgMapper);
	tryPutSystemOptionalRequest(reqOut, context_, true, propsOption.get());

	const int32_t partitionId =
			channel_.resolvePartitionId(context_, key, keyConverter);

	putContainerKey(reqOut, channel_, context_, key, keyConverter);
	tryPutContainerType(reqOut, containerType);
	reqOut << modifiable;
	orgMapper.get()->exportSchema(reqOut, getRowMapperConfig());

	exportContainerProperties(
			reqOut, *containerType, &containerInfo, *orgMapper);

	const Statement::Id statement = getContainerStatement(
			Statement::PUT_CONTAINER, containerType);

	channel_.applyPartitionId(context_, partitionId);
	ArrayByteInStream respIn = channel_.executeStatement(
			context_, statement, 0, req_, resp_);

	int32_t schemaVerId;
	respIn >> schemaVerId;

	int64_t containerId;
	respIn >> containerId;

	bool cached;
	std::auto_ptr<ContainerKey> remoteKey =
			acceptRemoteContainerKey(&respIn, key, keyConverter, cached);

	RowMapper::Reference mapper(
			mapperCache, mapperCache.resolve(
					*orgMapper.get(), respIn, getRowMapperConfig(),
					ClientUtil::toBool(containerInfo.columnOrderIgnorable)));

	if (cache != NULL) {
		cache->cacheSchema(*remoteKey, *mapper, containerId, schemaVerId);
	}

	return new GSContainer(
			*this, mapper, schemaVerId, partitionId, containerId, remoteKey,
			cached);
}

GSResult GSGridStoreTag::putContainer(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info, GSBool modifiable,
		GSContainer **container, const GSContainerType *containerType,
		const ClientVersion &version,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(container, NULL);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(
			funcInfo, store, name, info, modifiable, container, containerType);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(info);
		GS_CLIENT_CHECK_NOT_NULL(container);

		*container = store->putContainer(
				name, RowMapper::toInfoRef(info, version),
				modifiable, containerType);
	}
	catch (...) {
		if (container != NULL) {
			*container = NULL;
		}
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

void GSGridStoreTag::dropContainer(
		const GSChar *name, const GSContainerType *containerType) {
	XArrayByteOutStream reqOut = channel_.getRequestOutStream(req_);

	const ContainerKeyConverter &keyConverter = context_.getKeyConverter();
	const ContainerKey &key = keyConverter.parse(name, false);

	NodeConnection::tryPutEmptyOptionalRequest(reqOut);
	putContainerKey(reqOut, channel_, context_, key, keyConverter);
	tryPutContainerType(reqOut, containerType);

	const Statement::Id statement = getContainerStatement(
			Statement::DROP_CONTAINER, containerType);
	channel_.applyPartitionId(
			context_,
			channel_.resolvePartitionId(context_, key, keyConverter));
	channel_.executeStatement(context_, statement, 0, req_, resp_);

	ContainerCache *cache = context_.getContainerCache();
	if (cache != NULL) {
		cache->removeSchema(key.toCaseSensitive(false));
	}
}

void GSGridStoreTag::dropCollection(const GSChar *name) {
	const GSContainerType containerType = GS_CONTAINER_COLLECTION;
	dropContainer(name, &containerType);
}

void GSGridStoreTag::dropTimeSeries(const GSChar *name) {
	const GSContainerType containerType = GS_CONTAINER_TIME_SERIES;
	dropContainer(name, &containerType);
}

bool GSGridStoreTag::getRow(const GSChar *pathKey, void *rowObj) {
	ContainerKey containerKey;
	std::string rowKeyStr;
	splitPathKey(pathKey, containerKey, rowKeyStr);

	std::auto_ptr<GSContainer> container =
			duplicateContainer(resolveContainer(containerKey));
	const GSChar *rowKeyStrPtr = rowKeyStr.c_str();

	return container->getRow(NULL, &rowKeyStrPtr, rowObj, false, true);
}

bool GSGridStoreTag::putRow(const GSChar *pathKey, const void *rowObj) {
	ContainerKey containerKey;
	std::string rowKeyStr;
	splitPathKey(pathKey, containerKey, rowKeyStr);

	std::auto_ptr<GSContainer> container =
			duplicateContainer(resolveContainer(containerKey));
	const GSChar *rowKeyStrPtr = rowKeyStr.c_str();

	return container->putRow(NULL, &rowKeyStrPtr, rowObj, true);
}

bool GSGridStoreTag::removeRow(const GSChar *pathKey) {
	ContainerKey containerKey;
	std::string rowKeyStr;
	splitPathKey(pathKey, containerKey, rowKeyStr);

	std::auto_ptr<GSContainer> container =
			duplicateContainer(resolveContainer(containerKey));
	const GSChar *rowKeyStrPtr = rowKeyStr.c_str();

	return container->removeRow(NULL, &rowKeyStrPtr, true);
}

void GSGridStoreTag::fetchAll(GSQuery *const *queryList, size_t queryCount) {
	if (queryList == NULL) {
		if (queryCount > 0) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}
	}

	typedef std::map<int32_t, MultiQueryStatement> RequestMap;
	RequestMap requestMap;

	for (size_t i = 0; i < queryCount; i++) {
		GSQuery *query = queryList[i];

		if (query == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}
		MultiQueryStatement::check(*query, *this);

		const int32_t partitionId = query->getContainer()->partitionId_;

		RequestMap::iterator it = requestMap.find(partitionId);
		MultiQueryStatement *multiStatement;
		if (it == requestMap.end()) {
			multiStatement = &requestMap.insert(std::make_pair(
					partitionId, MultiQueryStatement(context_))).first->second;
		}
		else {
			multiStatement = &it->second;
		}
		multiStatement->add(*query);
	}

	for (RequestMap::iterator it =
			requestMap.begin(); it != requestMap.end(); ++it) {
		const int32_t partitionId = it->first;
		MultiQueryStatement &multiStatement = it->second;
		executeStatement(partitionId,
				Statement::EXECUTE_MULTIPLE_QUERIES, multiStatement);
	}
}

void GSGridStoreTag::multiPut(
		const GSContainerRowEntry *entryList, size_t entryCount) {
	if (entryList == NULL) {
		if (entryCount > 0) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}
	}

	typedef std::map<int32_t, MultiPutStatement> RequestMap;
	RequestMap requestMap;

	const ContainerKeyConverter &keyConverter = context_.getKeyConverter();

	for (size_t i = 0; i < entryCount; i++) {
		const GSContainerRowEntry &entry = entryList[i];
		if (entry.containerName == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}

		const ContainerKey &key =
				keyConverter.parse(entry.containerName, false);
		const int32_t partitionId =
				channel_.resolvePartitionId(context_, key, keyConverter);

		RequestMap::iterator it = requestMap.find(partitionId);
		MultiPutStatement *multiStatement;
		if (it == requestMap.end()) {
			multiStatement = &requestMap.insert(std::make_pair(
					partitionId, MultiPutStatement(context_))).first->second;
		}
		else {
			multiStatement = &it->second;
		}
		multiStatement->add(entry, NULL, keyConverter);
	}

	for (RequestMap::iterator it =
			requestMap.begin(); it != requestMap.end(); ++it) {
		const int32_t partitionId = it->first;
		MultiPutStatement &multiStatement = it->second;
		executeStatement(partitionId,
				Statement::PUT_MULTIPLE_CONTAINER_ROWS, multiStatement);
	}
}

void GSGridStoreTag::multiGet(
		const GSRowKeyPredicateEntry *const &predicateList,
		size_t predicateCount,
		const GSContainerRowEntry *&entryList, size_t &entryCount) {
	if (predicateList == NULL) {
		if (predicateCount > 0) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}
	}

	entryList = NULL;
	entryCount = 0;
	varDataPool_.clear();

	const ContainerKeyConverter &keyConverter = context_.getKeyConverter();

	typedef std::map<int32_t, MultiGetRequest> RequestMap;
	RequestMap requestMap;

	for (size_t i = 0; i < predicateCount; i++) {
		const GSRowKeyPredicateEntry &entry = predicateList[i];
		if (entry.containerName == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}

		const ContainerKey &key =
				keyConverter.parse(entry.containerName, false);
		const int32_t partitionId =
				channel_.resolvePartitionId(context_, key, keyConverter);

		MultiGetRequest &request = requestMap[partitionId];
		request.add(entry);
	}

	typedef std::vector<const RowMapper*> MapperList;
	typedef std::vector<GSRow*> RowList;
	typedef std::vector<GSContainerRowEntry> ResultList;

	struct Cleaner {
		~Cleaner() try {
			for (MapperList::iterator it = allMapperList_.begin();
					it != allMapperList_.end(); ++it) {
				try {
					RowMapper::getDefaultCache().release(&(*it));
				}
				catch (...) {
				}
			}
			for (RowList::iterator it = allRowList_.begin();
					it != allRowList_.end(); ++it) {
				try {
					GSRow::close(&(*it));
				}
				catch (...) {
				}
			}
		}
		catch (...) {
		}

		MapperList allMapperList_;
		RowList allRowList_;

	} cleaner;

	typedef std::vector<GSRow*> RowList;
	typedef std::map<ContainerKey, RowList> ContainerRowsMap;
	ContainerRowsMap resultMap;

	for (RequestMap::iterator it =
			requestMap.begin(); it != requestMap.end(); ++it) {
		XArrayByteOutStream reqOut = channel_.getRequestOutStream(req_);

		const int32_t partitionId = it->first;
		MultiGetRequest &request = it->second;
		if (!request.makeRequest(reqOut, channel_, context_)) {
			continue;
		}

		channel_.applyPartitionId(context_, partitionId);
		ArrayByteInStream respIn = channel_.executeStatement(
				context_, Statement::GET_MULTIPLE_CONTAINER_ROWS,
				0, req_, resp_);

		RowMapper::Cache &cache = RowMapper::getDefaultCache();

		RowMapper::VarDataPool schemaVarDataPool;
		std::vector<GSColumnInfo> columnInfoList;
		const bool withContainerType = true;

		MapperList mapperList;
		int32_t headCount;
		respIn >> headCount;
		for (int32_t i = 0; i < headCount; i++) {
			schemaVarDataPool.clear();
			columnInfoList.clear();

			GSContainerInfo containerInfo = GS_CONTAINER_INFO_INITIALIZER;
			importSchemaProperty(
					respIn, getRowMapperConfig(), schemaVarDataPool,
					containerInfo, columnInfoList, withContainerType);
			containerInfo.columnCount = columnInfoList.size();
			containerInfo.columnInfoList = &columnInfoList[0];

			RowMapper::Reference mapperRef(cache, cache.resolve(
					RowMapper::toInfoRef(&containerInfo, ClientVersion()),
					getRowMapperConfig()));
			mapperList.push_back(mapperRef.get());
			cleaner.allMapperList_.push_back(mapperRef.release());
		}

		int32_t bodyCount;
		respIn >> bodyCount;
		if (static_cast<size_t>(bodyCount) > respIn.base().remaining()) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
		}

		for (int32_t i = 0; i < bodyCount; i++) {
			const ContainerKey &containerKey = keyConverter.get(respIn, true);

			int32_t mapperIndex;
			respIn >> mapperIndex;
			if (mapperIndex < 0 ||
					mapperIndex >= ClientUtil::sizeValueToInt32(mapperList.size())) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
			}

			const RowMapper *mapper = mapperList[mapperIndex];

			const bool rowIdIncluded =
					(mapper->getCategory() == RowMapper::CATEGORY_COLLECTION);

			int64_t subRowCount;
			respIn >> subRowCount;
			if (subRowCount < 0 ||
					static_cast<size_t>(subRowCount) >
					respIn.base().remaining()) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
			}

			RowList &rowList = resultMap[containerKey];

			RowMapper::InputCursor cursor(
					respIn, *mapper, GSContainer::getRowMappingMode(),
					static_cast<int32_t>(subRowCount), rowIdIncluded,
					&varDataPool_);

			for (int64_t j = 0; j < subRowCount; j++) {
				RowMapper::Reference mapperRef(
						cache, cache.duplicate(*mapper));

				cleaner.allRowList_.reserve(cleaner.allRowList_.size() + 1);
				GSRow *row = GSRow::create(*this, mapperRef);
				cleaner.allRowList_.push_back(row);

				mapper->decode(cursor, row);

				rowList.push_back(row);
			}
		}
	}

	GSContainerRowEntry *resultEntryList;
	if (resultMap.empty()) {
		resultEntryList = NULL;
	}
	else {
		resultEntryList = static_cast<GSContainerRowEntry*>(
				varDataPool_.allocate(
						sizeof(GSContainerRowEntry) * resultMap.size()));
	}

	GSContainerRowEntry *resultEntryIt = resultEntryList;
	for (ContainerRowsMap::iterator it = resultMap.begin();
			it != resultMap.end(); ++it, ++resultEntryIt) {
		GSContainerRowEntry entry = GS_CONTAINER_ROW_ENTRY_INITIALIZER;

		entry.containerName = RowMapper::copyString(
				keyConverter.format(it->first).c_str(), varDataPool_);

		RowList &srcList = it->second;
		if (!srcList.empty()) {
			void **destList = static_cast<void**>(
					varDataPool_.allocate(sizeof(void*) * srcList.size()));

			void **destIt = destList;
			for (RowList::iterator srcIt = srcList.begin();
					srcIt != srcList.end(); ++srcIt) {
				*(destIt++) = *srcIt;
			}

			entry.rowList = destList;
			entry.rowCount = srcList.size();
		}

		*resultEntryIt = entry;
	}

	entryList = resultEntryList;
	entryCount = resultMap.size();

	cleaner.allRowList_.clear();
}

GSRow* GSGridStoreTag::createRow(
		const RowMapper::ContainerInfoRef<true> &infoRef) {
	channel_.checkContextAvailable(context_);
	RowMapper::Cache &cache = RowMapper::getDefaultCache();
	RowMapper::Reference mapper(
			cache, cache.resolve(infoRef, getRowMapperConfig()));

	return GSRow::create(*this, mapper);
}

GSResult GSGridStoreTag::createRow(
		GSGridStore *store, const GSContainerInfo *info, GSRow **row,
		const ClientVersion &version,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(row, NULL);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(funcInfo, store, info, row);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(info);
		GS_CLIENT_CHECK_NOT_NULL(row);

		*row = store->createRow(RowMapper::toInfoRef(info, version));
	}
	catch (...) {
		if (row != NULL) {
			*row = NULL;
		}
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSRowKeyPredicate* GSGridStoreTag::createRowKeyPredicate(GSType keyType) {
	channel_.checkContextAvailable(context_);
	return new GSRowKeyPredicate(*this, keyType);
}

GSPartitionController* GSGridStoreTag::getPartitionController() {
	channel_.checkContextAvailable(context_);
	return new GSPartitionController(*this);
}

void GSGridStoreTag::importIndexInfo(
		ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
		GSIndexInfo &indexInfo) {
	int32_t infoSizeValue;
	in >> infoSizeValue;
	const size_t infoSize = ClientUtil::toSizeValue(infoSizeValue);
	if (infoSize > in.base().remaining()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by illegal index info size");
	}

	const size_t endPos = in.base().position() + infoSize;

	const GSChar *name = RowMapper::decodeString(in, varDataPool);
	indexInfo.name = (strlen(name) == 0 ? NULL : name);

	indexInfo.column = -1;
	indexInfo.columnName = NULL;

	int32_t columnCount;
	in >> columnCount;
	for (int32_t i = 0; i < columnCount; i++) {
		int32_t column;
		in >> column;
		if (i == 0) {
			indexInfo.column = column;
		}
	}

	int8_t rawType;
	in >> rawType;
	GSIndexTypeFlags typeFlag;
	if (rawType == GS_INDEX_FLAG_DEFAULT) {
		typeFlag = GS_INDEX_FLAG_DEFAULT;
	}
	else {
		typeFlag = 1 << rawType;
		getIndexTypeOrdinal(typeFlag, false);
	}
	indexInfo.type = typeFlag;

	if (endPos > in.base().position()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by illegal index info format");
	}
	in.base().position(endPos);
}

void GSGridStoreTag::exportIndexInfo(
		XArrayByteOutStream &out, const GSIndexInfo &indexInfo) {
	const size_t headPos = out.base().position();
	out << ClientUtil::sizeValueToInt32(0);

	const size_t startPos = out.base().position();

	const GSChar *name = indexInfo.name;
	out << (name == NULL ? "" : name);

	const int32_t column = indexInfo.column;
	const int32_t columnCount = (column < 0 ? 0 : 1);
	out << columnCount;
	if (columnCount > 0) {
		out << column;
	}

	out << static_cast<int8_t>(getIndexTypeOrdinal(indexInfo.type, true));

	const size_t endPos = out.base().position();

	out.base().position(headPos);
	out << ClientUtil::sizeValueToInt32(endPos - startPos);
	out.base().position(endPos);
}

int32_t GSGridStoreTag::getIndexTypeOrdinal(
		GSIndexTypeFlags typeFlag, bool emptyOrDefaultAllowed) {
	switch (typeFlag) {
	case GS_INDEX_FLAG_TREE:
		return util::ILog2<GS_INDEX_FLAG_TREE>::VALUE;
	case GS_INDEX_FLAG_HASH:
		return util::ILog2<GS_INDEX_FLAG_HASH>::VALUE;
	case GS_INDEX_FLAG_SPATIAL:
		return util::ILog2<GS_INDEX_FLAG_SPATIAL>::VALUE;
	case GS_INDEX_FLAG_DEFAULT:
		if (!emptyOrDefaultAllowed) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
					"Default index type specified");
		}
		return GS_INDEX_FLAG_DEFAULT;
	case 0:
		if (!emptyOrDefaultAllowed) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
					"No index type specified");
		}
		return GS_INDEX_FLAG_DEFAULT;
	default:
		if (util::population(static_cast<uint32_t>(typeFlag)) > 1) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
					"Multiple index types specified (flags=" << typeFlag << ")");
		}
		else {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNKNOWN_INDEX_FLAG,
					"Unknown index type specified (flag=" << typeFlag << ")");
		}
	}
}

bool GSGridStoreTag::isContainerStatementUnified() {
	return (NodeConnection::getProtocolVersion() >= 3);
}

bool GSGridStoreTag::isSessionUUIDSummarized() {
	return (NodeConnection::getProtocolVersion() >= 3);
}

bool GSGridStoreTag::isTSDivisionAndAffinityEnabled() {
	return (NodeConnection::getProtocolVersion() >= 3 &&
			!GridStoreChannel::v15TSPropsCompatible_);
}

bool GSGridStoreTag::isClientIdEnabled() {
	return (NodeConnection::getProtocolVersion() >= 13);
}

bool GSGridStoreTag::isIndexDetailEnabled() {
	return (NodeConnection::getProtocolVersion() >= 13);
}

bool GSGridStoreTag::isAttributeVerifiable() {
	return (NodeConnection::getProtocolVersion() >= 13);
}

bool GSGridStoreTag::isNullableColumnAllowed() {
	return isNullableColumnAllowed(NodeConnection::getProtocolVersion());
}

bool GSGridStoreTag::isNullableColumnAllowed(int32_t protocolVersion) {
	return (protocolVersion >= 13);
}

bool GSGridStoreTag::isQueryOptionsExtensible() {
	return (NodeConnection::getProtocolVersion() >= 14 &&
			!GridStoreChannel::v40QueryCompatible_);
}

const RowMapper::Config& GSGridStoreTag::getRowMapperConfig() {
	return getRowMapperConfig(NodeConnection::getProtocolVersion());
}

const RowMapper::Config& GSGridStoreTag::getRowMapperConfig(
		int32_t protocolVersion) {
	if (protocolVersion >= 14 && !GridStoreChannel::v40SchemaCompatible_) {
		return DEFAULT_MAPPER_CONFIG;
	}
	else if (isNullableColumnAllowed(protocolVersion)) {
		return COMPATIBLE_MAPPER_CONFIG_14;
	}
	else {
		return COMPATIBLE_MAPPER_CONFIG_13;
	}
}

GSContainer* GSGridStoreTag::findContainerByCache(
		ContainerCache &cache, const ContainerKey &key,
		const ContainerKeyConverter &keyConverter,
		const GSBinding *binding, const GSContainerType *containerType,
		bool general) {
	const ContainerKey &normalizedKey = key.toCaseSensitive(false);
	const GridStoreChannel::LocatedSchema *schema = cache.findSchema(
			normalizedKey, binding, containerType,
			general, getRowMapperConfig());
	if (schema == NULL) {
		return NULL;
	}

	const int32_t partitionId = channel_.resolvePartitionId(
			context_, normalizedKey, keyConverter);
	RowMapper::Cache &mapperCache = RowMapper::getDefaultCache();
	RowMapper::Reference mapper(
			mapperCache, mapperCache.duplicate(*schema->getMapper()));

	const bool cached = true;
	std::auto_ptr<ContainerKey> normalizedKeyPtr(
			new ContainerKey(normalizedKey));

	return new GSContainer(
			*this, mapper, schema->getVersionId(),
			partitionId, schema->getContainerId(), normalizedKeyPtr, cached);
}

std::auto_ptr<ContainerKey> GSGridStoreTag::acceptRemoteContainerKey(
		ArrayByteInStream *in, const ContainerKey &localKey,
		const ContainerKeyConverter &keyConverter, bool &cached) {
	cached = false;
	const ContainerKey &remoteKey =
			(in == NULL ? localKey : keyConverter.get(*in, false));

	std::auto_ptr<ContainerKey> normalizedRemoteKey(
			new ContainerKey(remoteKey.toCaseSensitive(false)));
	if (in != NULL && !(remoteKey == localKey) &&
			!(*normalizedRemoteKey == localKey.toCaseSensitive(false))) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA,
				"Inconsistent container name ("
				"localName=" << localKey <<
				", remoteName=" << remoteKey << ")");
	}

	if (!pathKeyOperationEnabled_ && context_.getContainerCache() == NULL) {
		if (!channel_.isMonitoring()) {
			return std::auto_ptr<ContainerKey>(NULL);
		}
	}
	else {
		cached = true;
	}

	return normalizedRemoteKey;
}

void GSGridStoreTag::splitPathKey(
		const GSChar *pathKey,
		ContainerKey &containerKey, std::string &rowKeyStr) {
	const ContainerKeyConverter &keyConverter = context_.getKeyConverter();

	std::string containerName;
	splitPathKey(pathKey, containerName, rowKeyStr);

	containerKey = keyConverter.parse(containerName.c_str(), false);
}

void GSGridStoreTag::splitPathKey(
		const GSChar *pathKey,
		std::string &containerName, std::string &rowKeyStr) {
	const std::string buf = pathKey;
	const size_t separatorPos = buf.find('/');
	if (separatorPos == std::string::npos) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	containerName.assign(buf.begin(), buf.begin() + separatorPos);
	rowKeyStr.assign(buf.begin() + separatorPos + 1, buf.end());
}

void GSGridStoreTag::tryPutSystemOptionalRequest(
		XArrayByteOutStream &reqOut, GridStoreChannel::Context &context,
		bool forCreationDDL, const OptionalRequestSource *source) {
	const bool clientIdRequired = forCreationDDL && isClientIdEnabled();

	if (!clientIdRequired && (source == NULL || !source->hasOptions())) {
		NodeConnection::tryPutEmptyOptionalRequest(reqOut);
		return;
	}

	if (!NodeConnection::isOptionalRequestEnabled()) {
		return;
	}

	NodeConnection::OptionalRequest optionalRequest;
	if (clientIdRequired) {
		optionalRequest.clientId_ = context.generateClientId();
	}
	if (source != NULL) {
		source->putOptions(optionalRequest);
	}
	optionalRequest.format(reqOut);
}

GSGridStore::ContainerPropertiesOption
GSGridStoreTag::containerPropertiesToOption(const RowMapper &mapper) {
	ContainerPropertiesOption option;
	if (mapper.isDefaultValueSpecified()) {
		GS_CLIENT_THROW_ERROR(
				GS_ERROR_CC_UNSUPPORTED_OPERATION,
				"Default value can not specified for container "
				"definition in the current version");
	}
	return option;
}

void GSGridStoreTag::exportContainerProperties(
		XArrayByteOutStream &out, const GSContainerType type,
		const GSContainerInfo *info, const RowMapper &mapper) {

	if (isTSDivisionAndAffinityEnabled() &&
			!GridStoreChannel::v20AffinityCompatible_) {
		const GSChar *dataAffinity =
				(info == NULL ? NULL : info->dataAffinity);

		if (dataAffinity == NULL) {
			out << "";
		}
		else {
			ClientUtil::checkSymbol(dataAffinity, "data affinity");
			out << dataAffinity;
		}
	}

	const GSTimeSeriesProperties *tsProps =
			(info == NULL ? NULL : info->timeSeriesProperties);

	if (type != GS_CONTAINER_TIME_SERIES) {
		if (tsProps != NULL) {
			GS_CLIENT_THROW_ERROR(
					GS_ERROR_CC_ILLEGAL_SCHEMA,
					"TimeSeriesProperties used except for TimeSeries");
		}
		return;
	}

	if (CLIENT_TIME_SERIES_PROPERTIES_OLD) {
		out << static_cast<int32_t>(-1);		
		out << static_cast<int8_t>(0);		
		out << static_cast<int32_t>(-1);		
		out << static_cast<int8_t>(0);		
		out << static_cast<int32_t>(0);		
		return;
	}

	if (tsProps == NULL) {
		out << static_cast<GSBool>(GS_FALSE);
	}
	else {
		out << static_cast<GSBool>(GS_TRUE);

		out << tsProps->rowExpirationTime;
		if (tsProps->rowExpirationTimeUnit >= 0) {
			out << static_cast<int8_t>(
					ClientUtil::checkTimeUnit(tsProps->rowExpirationTimeUnit));
		}
		else {
			out << static_cast<int8_t>(GS_TIME_UNIT_DAY);
		}

		if (isTSDivisionAndAffinityEnabled()) {
			const int32_t expirationDivisionCount =
					tsProps->expirationDivisionCount;

			if (expirationDivisionCount == 0) {
				GS_CLIENT_THROW_ERROR(
						GS_ERROR_CC_ILLEGAL_PARAMETER, "Illegal division count");
			}
			out << expirationDivisionCount;

			if (GridStoreChannel::v20AffinityCompatible_) {
				out << "";
			}
		}

		out << tsProps->compressionWindowSize;
		if (tsProps->compressionWindowSize >= 0) {
			out << static_cast<int8_t>(
					ClientUtil::checkTimeUnit(tsProps->compressionWindowSizeUnit));
		}
		else {
			out << static_cast<int8_t>(GS_TIME_UNIT_DAY);
		}

		out << static_cast<int8_t>(
				checkCompressionMethod(tsProps->compressionMethod));
		out << ClientUtil::sizeValueToInt32(tsProps->compressionListSize);
		for (size_t i = 0; i < tsProps->compressionListSize; i++) {
			const GSColumnCompression &entry = tsProps->compressionList[i];
			out << mapper.resolveColumnId(entry.columnName);
			out << entry.relative;
			if (entry.relative) {
				out << checkCompressionRate(entry.rate);
				out << entry.span;
			}
			else {
				out << entry.width;
			}
		}
	}
}

void GSGridStoreTag::importContainerProperties(
		ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
		GSContainerInfo &containerInfo,
		const std::vector<GSColumnInfo> &columnInfoList) {
	const GSChar *dataAffinity = NULL;

	if (isTSDivisionAndAffinityEnabled() &&
			!GridStoreChannel::v20AffinityCompatible_) {
		dataAffinity = RowMapper::decodeString(in, varDataPool);
		if (strlen(dataAffinity) == 0) {
			dataAffinity = NULL;
		}
	}

	containerInfo.dataAffinity = dataAffinity;
	containerInfo.timeSeriesProperties = NULL;

	if (containerInfo.type != GS_CONTAINER_TIME_SERIES) {
		return;
	}

	GSBool exists;
	in >> exists;
	if (!exists) {
		return;
	}

	GSTimeSeriesProperties *props = static_cast<GSTimeSeriesProperties*>(
			varDataPool.allocate(sizeof(GSTimeSeriesProperties)));
	try {
		const GSTimeSeriesProperties initialProps =
				GS_TIME_SERIES_PROPERTIES_INITIALIZER;
		*props = initialProps;

		{
			int32_t rowExpirationTime;
			in >> rowExpirationTime;
			int8_t timeUnit;
			in >> timeUnit;

			if (rowExpirationTime > 0) {
				props->rowExpirationTime = rowExpirationTime;
				props->rowExpirationTimeUnit = ClientUtil::checkTimeUnit(
						static_cast<GSTimeUnit>(timeUnit));
			}
		}

		if (isTSDivisionAndAffinityEnabled()) {
			in >> props->expirationDivisionCount;

			if (GridStoreChannel::v20AffinityCompatible_) {
				RowMapper::decodeString(in, varDataPool);
			}
		}

		{
			int32_t compressionWindowSize;
			in >> compressionWindowSize;
			int8_t timeUnit;
			in >> timeUnit;

			if (compressionWindowSize > 0) {
				props->compressionWindowSize = compressionWindowSize;
				props->compressionWindowSizeUnit = ClientUtil::checkTimeUnit(
						static_cast<GSTimeUnit>(timeUnit));
			}
		}

		int8_t compressionMethod;
		in >> compressionMethod;
		props->compressionMethod = checkCompressionMethod(
				static_cast<GSCompressionMethod>(compressionMethod));

		int32_t entryCount;
		in >> entryCount;

		if (entryCount > 0 && compressionMethod != GS_COMPRESSION_HI) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
					"Unexpected compression entry found");
		}

		if (entryCount < 0 ||
				static_cast<size_t>(entryCount) > in.base().remaining()) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
		}

		GSColumnCompression *compressionList;
		if (entryCount > 0) {
			compressionList = static_cast<GSColumnCompression*>(
					varDataPool.allocate(
							sizeof(GSColumnCompression) *
							static_cast<size_t>(entryCount)));
		}
		else {
			compressionList = NULL;
		}

		for (int32_t i = 0; i < entryCount; i++) {
			GSColumnCompression compression =
					GS_COLUMN_COMPRESSION_INITIALIZER;

			int32_t columnId;
			in >> columnId;
			if (columnId < 0 ||
					static_cast<size_t>(columnId) >= columnInfoList.size()) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
						"Protocol error by illegal column ID");
			}
			compression.columnName = columnInfoList[columnId].name;

			in >> compression.relative;
			if (compression.relative) {
				in >> compression.rate;
				in >> compression.span;
			}
			else {
				in >> compression.width;
			}

			compressionList[i] = compression;
		}
		props->compressionList = compressionList;
		props->compressionListSize = static_cast<size_t>(entryCount);

		containerInfo.timeSeriesProperties = props;
	}
	catch (util::Exception &e) {
		if (e.getErrorCode() == GS_ERROR_CC_ILLEGAL_PARAMETER) {
			GS_CLIENT_RETHROW_ERROR_CODED(
					GS_ERROR_CC_MESSAGE_CORRUPTED, e, "");
		}

		throw;
	}
}

GSCompressionMethod GSGridStoreTag::checkCompressionMethod(
		GSCompressionMethod compressionMethod) {
	switch (compressionMethod) {
	case GS_COMPRESSION_NO:
	case GS_COMPRESSION_SS:
	case GS_COMPRESSION_HI:
		return compressionMethod;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}
}

double GSGridStoreTag::checkCompressionRate(double rate) {
	if (!(0 <= rate && rate <= 1)) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	return rate;
}

const GSContainer& GSGridStoreTag::resolveContainer(
		const ContainerKey &containerKey) {
	if (!pathKeyOperationEnabled_) {
		GS_CLIENT_THROW_ERROR(
				GS_ERROR_CC_UNSUPPORTED_OPERATION, "Operation restricted");
	}

	ContainerMap::const_iterator it =
			containerMap_.find(containerKey.toCaseSensitive(false));
	if (it == containerMap_.end()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_CONTAINER_NOT_OPENED, "");
	}

	return *it->second;
}

std::auto_ptr<GSContainer> GSGridStoreTag::duplicateContainer(
		const GSContainer &src) {
	RowMapper::Cache &cache = RowMapper::getDefaultCache();
	RowMapper::Reference mapper(cache, cache.duplicate(src.getMapper()));

	const bool cached = true;
	std::auto_ptr<ContainerKey> containerKey(
			new ContainerKey(*src.normalizedContainerKey_));

	std::auto_ptr<GSContainer> dest(new GSContainer(
			*this, mapper, src.getSchemaVersionId(),
			src.getPartitionId(), src.getContainerId(),
			containerKey, cached));

	return dest;
}

Statement::Id GSGridStoreTag::getContainerStatement(
		Statement::Id statement, const GSContainerType *containerType) {
	if (containerType == NULL || isContainerStatementUnified()) {
		return statement;
	}
	else if (*containerType == GS_CONTAINER_TIME_SERIES) {
		switch (statement) {
		case Statement::PUT_CONTAINER:
			return Statement::PUT_TIME_SERIES;
		case Statement::GET_CONTAINER:
			return Statement::GET_TIME_SERIES;
		case Statement::DROP_CONTAINER:
			return Statement::DROP_TIME_SERIES;
		default:
			assert(false);
			GS_CLIENT_THROW_ERROR(0, "");
		}
	}
	else {
		if (statement == Statement::DROP_CONTAINER) {
			return Statement::DROP_COLLECTION;
		}
		else {
			return statement;
		}
	}
}

void GSGridStoreTag::putContainerKey(
		XArrayByteOutStream &reqOut, GridStoreChannel &channel,
		GridStoreChannel::Context &context, const ContainerKey &key,
		const ContainerKeyConverter &keyConverter) {
	keyConverter.put(reqOut, key, channel.getDatabaseId(context));
}

void GSGridStoreTag::tryPutContainerType(
		XArrayByteOutStream &reqOut, const GSContainerType *containerType) {
	if (!isContainerStatementUnified()) {
		return;
	}

	if (containerType == NULL) {
		reqOut << static_cast<uint8_t>(0xff);
	}
	else {
		reqOut << static_cast<int8_t>(*containerType);
	}

}

void GSGridStoreTag::importIdProperty(
		ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
		ContainerIdInfo &idInfo, const ContainerKeyConverter &keyConverter) {
	in >> idInfo.versionId_;
	in >> idInfo.containerId_;
	idInfo.remoteKey_ = keyConverter.get(in, false);
	idInfo.remoteName_ = RowMapper::copyString(
			keyConverter.format(idInfo.remoteKey_).c_str(), varDataPool);
}

void GSGridStoreTag::importSchemaProperty(
		ArrayByteInStream &in, const RowMapper::Config &config,
		RowMapper::VarDataPool &varDataPool, GSContainerInfo &containerInfo,
		std::vector<GSColumnInfo> &columnInfoList, bool withContainerType) {
	columnInfoList.clear();

	if (withContainerType) {
		int8_t containerTypeValue;
		in >> containerTypeValue;
		containerInfo.type = RowMapper::checkContainerType(
				static_cast<GSContainerType>(containerTypeValue));
	}

	const size_t columnCount = RowMapper::importColumnCount(in);
	if (columnCount == 0 && !config.anyTypeAllowed_) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
	}

	int32_t keyColumnId =
			RowMapper::importKeyListBegin(in, config, columnCount);

	for (size_t i = 0; i < columnCount; i++) {
		columnInfoList.push_back(
				RowMapper::importColumnSchema(in, config, varDataPool));
	}

	RowMapper::importKeyListEnd(in, config, columnCount, keyColumnId);
	containerInfo.rowKeyAssigned = ClientUtil::toGSBool(keyColumnId >= 0);
}

void GSGridStoreTag::importIndexProperty(
		ArrayByteInStream &in, std::vector<GSColumnInfo> &columnInfoList) {

	int32_t entryCount;
	in >> entryCount;
	if (entryCount < 0) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
	}

	for (int32_t i = 0; i < entryCount; i++) {
		int32_t columnId;
		in >> columnId;
		if (columnId < 0 ||
				static_cast<size_t>(columnId) >= columnInfoList.size()) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
					"Protocol error by illegal column ID");
		}

		GSIndexTypeFlags &flags = columnInfoList[columnId].indexTypeFlags;

		int8_t indexType;
		in >> indexType;
		switch (indexType) {
		case 0:
			flags |= GS_INDEX_FLAG_TREE;
			break;
		case 1:
			flags |= GS_INDEX_FLAG_HASH;
			break;
		case 2:
			flags |= GS_INDEX_FLAG_SPATIAL;
			break;
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
					"Protocol error by illegal index type");
		}
	}
}

void GSGridStoreTag::importEventNotificationProperty(
		ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
		std::vector<std::string> *eventNotificationInfoList) {
	if (eventNotificationInfoList != NULL) {
		eventNotificationInfoList->clear();
	}

	int32_t entryCount;
	in >> entryCount;
	if (entryCount < 0 ||
			static_cast<size_t>(entryCount) > in.base().remaining()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
	}

	if (entryCount <= 0) {
		return;
	}

	for (int32_t i = 0; i < entryCount; i++) {
		std::string url;
		in >> url;
		if (eventNotificationInfoList != NULL) {
			eventNotificationInfoList->push_back(url);
		}
	}
}

void GSGridStoreTag::importTriggerProperty(
		ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
		GSContainerInfo &containerInfo,
		const std::vector<GSColumnInfo> &columnInfoList) {

	int32_t entryCountValue;
	in >> entryCountValue;
	if (entryCountValue < 0 ||
			static_cast<size_t>(entryCountValue) > in.base().remaining()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
	}
	const size_t entryCount = static_cast<size_t>(entryCountValue);

	if (entryCount <= 0) {
		return;
	}

	GSTriggerInfo *infoList = static_cast<GSTriggerInfo*>(
			varDataPool.allocate(sizeof(GSTriggerInfo) * entryCount));
	for (size_t i = 0; i < entryCount; i++) {
		infoList[i].name = RowMapper::decodeString(in, varDataPool);
		int8_t type;
		in >> type;
		infoList[i].type = type;
		infoList[i].uri = RowMapper::decodeString(in, varDataPool);
		int32_t eventTypeFlags;
		in >> eventTypeFlags;
		infoList[i].eventTypeFlags = eventTypeFlags;
		int32_t columnCount;
		in >> columnCount;
		if (columnCount < 0) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
		} else if (columnCount == 0) {
			infoList[i].columnSet = NULL;
		} else {
			const GSChar **tmp = static_cast<const GSChar**>(
			varDataPool.allocate(sizeof(GSChar*) *
					static_cast<size_t>(columnCount)));
			for (int32_t j = 0; j < columnCount; j++) {
				int32_t columnId;
				in >> columnId;
				if (columnId < 0 ||
						columnId >= static_cast<int32_t>(columnInfoList.size())) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
							"Protocol error by illegal column ID");
				}
				tmp[j] = columnInfoList[columnId].name;
			}
			infoList[i].columnSet = tmp;
		}
		infoList[i].columnCount = columnCount;
		std::string jmsProviderName;
		in >> jmsProviderName;
		infoList[i].jmsDestinationType = RowMapper::decodeString(in, varDataPool);
		infoList[i].jmsDestinationName = RowMapper::decodeString(in, varDataPool);
		infoList[i].user = RowMapper::decodeString(in, varDataPool);
		infoList[i].password = RowMapper::decodeString(in, varDataPool);
	}

	containerInfo.triggerInfoCount = entryCount;
	containerInfo.triggerInfoList = infoList;
}

void GSGridStoreTag::importIndexDetailProperty(
		ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
		std::vector<GSIndexInfo> &indexInfoList) {
	indexInfoList.clear();

	int32_t entryCount;
	in >> entryCount;
	if (entryCount < 0) {
		return;
	}

	for (int32_t i = 0; i < entryCount; i++) {
		GSIndexInfo indexInfo;
		importIndexInfo(in, varDataPool, indexInfo);
		indexInfoList.push_back(indexInfo);
	}
}

void GSGridStoreTag::applyIndexInfoList(
		const std::vector<GSIndexInfo> &indexInfoList,
		const std::vector<GSColumnInfo> &columnInfoList,
		bool indexFlagsSpecified, RowMapper::VarDataPool &varDataPool,
		GSContainerInfo &containerInfo) {

	const size_t infoCount = indexInfoList.size();
	GSIndexInfo *dest = static_cast<GSIndexInfo*>(varDataPool.allocate(
			sizeof(GSIndexInfo) * infoCount));
	GSIndexInfo *destIt = dest;

	for (std::vector<GSIndexInfo>::const_iterator it = indexInfoList.begin();
			it != indexInfoList.end(); ++it, ++destIt) {
		const int32_t column = it->column;
		const GSChar *columnName = NULL;
		if (column >= 0) {
			if (static_cast<size_t>(column) >= columnInfoList.size()) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
						"Protocol error by illegal index column");
			}
			const GSColumnInfo &columnInfo = columnInfoList[column];
			columnName = columnInfo.name;

			if (indexFlagsSpecified &&
					((columnInfo.indexTypeFlags & it->type) == 0)) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
						"Protocol error by inconsistent index type");
			}
		}

		*destIt = *it;
		destIt->columnName = columnName;
	}

	containerInfo.indexInfoCount = infoCount;
	containerInfo.indexInfoList = dest;
}

const GSChar* GSGridStoreTag::resolveContainerName(
		const GSChar *name, const GSContainerInfo *info) {
	const GSChar *another = (info == NULL ? NULL : info->name);

	if (name == NULL) {
		if (another == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER,
					"Container name not specified");
		}
		return another;
	}
	else if (another != NULL &&
			strcmp(name, another) != 0) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
				"Inconsistent container name");
	}

	return name;
}

const GSContainerType* GSGridStoreTag::resolveContainerType(
		const GSContainerType *type, const GSContainerInfo *info) {
	const GSContainerType *another = (info == NULL ? NULL : &info->type);

	if (type == NULL) {
		if (another == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER,
					"Container type not specified");
		}
		return another;
	}
	else if (another != NULL && *another != *type) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA,
				"Inconsistent container type");
	}

	return type;
}

template<typename MultiStatement>
void GSGridStoreTag::executeStatement(
		int32_t partitionId, Statement::Id statement,
		MultiStatement &multiStatement) {
	try {
		for (int32_t trialCount = 0;; trialCount++) {

			for (int sessionTrial = 0;; sessionTrial++) {
				XArrayByteOutStream reqOut =
						channel_.getRequestOutStream(req_);
				if (multiStatement.makeCreateSessionRequest(
						reqOut, channel_, context_)) {

					channel_.applyPartitionId(context_, partitionId);
					try {
						ArrayByteInStream respIn = channel_.executeStatement(
								context_, Statement::CREATE_MULTIPLE_SESSIONS,
								0, req_, resp_);

						multiStatement.acceptCreateSessionResponse(respIn);
					}
					catch (StatementException &e) {
						if (!GSContainer::isNewSessionConflicted(e)) {
							throw;
						}
						else if (sessionTrial >=
								GSContainer::MAX_SESSION_REPAIR_COUNT) {
							GS_CLIENT_RETHROW_ERROR(
									e,
									"Failed to create session ("
									"totalTrialCount=" << trialCount <<
									", sessionTrialCount=" << sessionTrial <<
									")");
						}
						continue;
					}
				}
				break;
			}

			try {
				XArrayByteOutStream reqOut =
						channel_.getRequestOutStream(req_);
				if (!multiStatement.makeMainRequest(reqOut, context_)) {
					break;
				}

				channel_.applyPartitionId(context_, partitionId);
				ArrayByteInStream respIn = channel_.executeStatement(
						context_, statement, 0, req_, resp_);
				multiStatement.acceptMainResponse(
						respIn, context_.getLastConnectionId());
				break;
			}
			catch (StatementException &e) {
				if (!multiStatement.acceptStatementErrorForSession(e)) {
					throw;
				}

				if (trialCount >= GSContainer::MAX_SESSION_REPAIR_COUNT) {
					GS_CLIENT_RETHROW_ERROR(
							e,
							"Failed to repair session (trialCount=" << trialCount << ")");
				}
			}
		}

		XArrayByteOutStream reqOut = channel_.getRequestOutStream(req_);
		if (multiStatement.makeCloseSessionRequest(reqOut)) {

			channel_.applyPartitionId(context_, partitionId);
			channel_.executeStatement(
					context_, Statement::CLOSE_MULTIPLE_SESSIONS,
					0, req_, resp_);

			multiStatement.acceptSessionClosed();
		}
	}
	catch (std::exception&) {
		try {
			XArrayByteOutStream reqOut = channel_.getRequestOutStream(req_);
			if (multiStatement.makeCloseSessionRequest(reqOut)) {

				channel_.applyPartitionId(context_, partitionId);
				channel_.executeStatement(
						context_, Statement::CLOSE_MULTIPLE_SESSIONS,
						0, req_, resp_);

				multiStatement.acceptSessionClosed();
			}
		}
		catch (...) {
		}

		throw;
	}
}

GSCollection* GSGridStoreTag::getContextControllerCollection(
		const GSContainerType *expectedType) {
	if (expectedType != NULL && *expectedType != GS_CONTAINER_COLLECTION) {
		return NULL;
	}

	GSContainerInfo info = GS_CONTAINER_INFO_INITIALIZER;
	GSColumnInfo columnInfoList[] = {
		GS_COLUMN_INFO_INITIALIZER,
		GS_COLUMN_INFO_INITIALIZER
	};

	columnInfoList[0].name = "name";
	columnInfoList[0].type = GS_TYPE_STRING;

	columnInfoList[1].name = "value";
	columnInfoList[1].type = GS_TYPE_STRING;

	info.columnInfoList = columnInfoList;
	info.columnCount = sizeof(columnInfoList) / sizeof(*columnInfoList);
	info.rowKeyAssigned = GS_TRUE;

	RowMapper::Cache &cache = RowMapper::getDefaultCache();
	RowMapper::Reference mapper(cache, cache.resolve(
			RowMapper::toInfoRef(&info, ClientVersion()),
			getRowMapperConfig()));

	const ContainerKeyConverter &keyConverter = context_.getKeyConverter();

	const bool cached = false;
	std::auto_ptr<ContainerKey> key(new ContainerKey(
			keyConverter.parse(CONTEXT_CONTROLLER_NAME, false)));

	return new GSCollection(*this, mapper, -1, -1, -1, key, cached);
}


const GSGridStoreTag::ContainerPropertyKeySet
GSGridStoreTag::ContainerPropertyKeySet::KEYS_DEFAULT =
		all().merge(true, Keys(
				CONTAINER_PROPERTY_ATTRIBUTE));

const GSGridStoreTag::ContainerPropertyKeySet
GSGridStoreTag::ContainerPropertyKeySet::KEYS_COMPATIBLE_TRIGGER =
		all().merge(true, Keys(
				CONTAINER_PROPERTY_TRIGGER,
				CONTAINER_PROPERTY_ATTRIBUTE,
				CONTAINER_PROPERTY_INDEX_DETAIL));

const GSGridStoreTag::ContainerPropertyKeySet
GSGridStoreTag::ContainerPropertyKeySet::KEYS_COMPATIBLE_INDEX =
		all().merge(true, Keys(
				CONTAINER_PROPERTY_ATTRIBUTE,
				CONTAINER_PROPERTY_INDEX_DETAIL));

const GSGridStoreTag::ContainerPropertyKeySet
GSGridStoreTag::ContainerPropertyKeySet::KEYS_FOR_OBJECT =
		Keys(
				CONTAINER_PROPERTY_ID,
				CONTAINER_PROPERTY_SCHEMA);

GSGridStoreTag::ContainerPropertyKeySet::ContainerPropertyKeySet(
		ContainerPropertyKey key1,
		ContainerPropertyKey key2,
		ContainerPropertyKey key3) :
		flags_(
				((1 << key1) | (1 << key2) | (1 << key3)) &
				((1 << CONTAINER_PROPERTY_KEY_MAX) - 1)) {
}

GSGridStoreTag::ContainerPropertyKeySet
GSGridStoreTag::ContainerPropertyKeySet::all() {
	Keys keys;
	keys.flags_ = (1 << CONTAINER_PROPERTY_KEY_MAX) - 1;
	return keys;
}

GSGridStoreTag::ContainerPropertyKeySet
GSGridStoreTag::ContainerPropertyKeySet::merge(
		bool exclusive, const Keys &keys) const {
	Keys merged = *this;
	if (exclusive) {
		merged.flags_ &= ~keys.flags_;
	}
	else {
		merged.flags_ |= keys.flags_;
	}
	return merged;
}

uint32_t GSGridStoreTag::ContainerPropertyKeySet::size() const {
	return util::population(flags_);
}

GSGridStoreTag::ContainerPropertyKey
GSGridStoreTag::ContainerPropertyKeySet::begin() const {
	const ContainerPropertyKey key = static_cast<ContainerPropertyKey>(0);

	if ((flags_ & (1 << key)) != 0) {
		return key;
	}

	return next(key);
}

GSGridStoreTag::ContainerPropertyKey
GSGridStoreTag::ContainerPropertyKeySet::next(
		ContainerPropertyKey lastKey) const {
	uint32_t key = lastKey + 1;
	const uint32_t restFlags = (flags_ & ~((1 << key) - 1));
	for (;; key++) {
		if (restFlags == 0) {
			return CONTAINER_PROPERTY_KEY_MAX;
		}

		if ((restFlags & (1 << key)) != 0) {
			return static_cast<ContainerPropertyKey>(key);
		}
	}
}


GSGridStoreTag::ContainerIdInfo::ContainerIdInfo() :
		versionId_(0),
		containerId_(0),
		remoteName_(NULL) {
}


GSGridStoreTag::MultiQueryStatement::MultiQueryStatement(
		GridStoreChannel::Context &context) :
		updateQueryFound_(false) {
	memcpy(sessionUUID_, context.getSessionUUID(), sizeof(uuid_t));
}

void GSGridStoreTag::MultiQueryStatement::check(
		GSQuery &query, GSGridStore &store) {
	if (query.getContainer()->store_ != &store) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}
}

void GSGridStoreTag::MultiQueryStatement::add(GSQuery &query) {
	updateQueryFound_ |= query.isForUpdate();
	queryList_.push_back(&query);
}

bool GSGridStoreTag::MultiQueryStatement::makeCreateSessionRequest(
		XArrayByteOutStream &req,
		GridStoreChannel &channel, GridStoreChannel::Context &context) {
	if (!updateQueryFound_ || queryList_.empty()) {
		return false;
	}

	optionalQueryList_.clear();

	for (QueryList::iterator it = queryList_.begin();
			it != queryList_.end(); ++it) {
		GSQuery &query = **it;
		if (query.isForUpdate() && query.getContainer()->getSessionIdDirect() == 0) {
			optionalQueryList_.push_back(&query);
		}
	}

	const bool summarized = isSessionUUIDSummarized();
	if (summarized) {
		req.writeAll(sessionUUID_, sizeof(uuid_t));
	}
	NodeConnection::tryPutEmptyOptionalRequest(req);

	const GSBool withId = GS_TRUE;
	req << withId;
	req << ClientUtil::sizeValueToInt32(optionalQueryList_.size());

	for (QueryList::iterator it = optionalQueryList_.begin();
			it != optionalQueryList_.end(); ++it) {
		GSQuery &query = **it;
		GSContainer &container = *query.getContainer();
		req << container.containerId_;

		if (!summarized) {
			req.writeAll(sessionUUID_, sizeof(uuid_t));
		}

		if (GSContainer::isSessionIdGeneratorEnabled()) {
			const int64_t sessionId = context.generateSessionId();
			container.setSessionIdDirect(sessionId, false);
			req << sessionId;
		}
		else {
			GSContainer::putNewSessionProperties(req, channel, context);
		}
	}

	return true;
}

void GSGridStoreTag::MultiQueryStatement::acceptCreateSessionResponse(
		ArrayByteInStream &resp) {
	int32_t sessionCount;
	resp >> sessionCount;
	if (optionalQueryList_.size() != static_cast<size_t>(sessionCount)) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by inconsistent session count");
	}

	if (!GSContainer::isSessionIdGeneratorEnabled()) {
		for (QueryList::iterator it = optionalQueryList_.begin();
				it != optionalQueryList_.end(); ++it) {
			GSQuery &query = **it;
			GSContainer &container = *query.getContainer();

			int64_t containerId;
			resp >> containerId;
			if (containerId != container.containerId_) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
						"Protocol error by inconsistent container ID");
			}

			int64_t sessionId;
			resp >> sessionId;
			if (sessionId == 0) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
						"Protocol error by empty session ID");
			}

			container.setSessionIdDirect(sessionId, true);
		}
	}

	optionalQueryList_.clear();
}

bool GSGridStoreTag::MultiQueryStatement::makeMainRequest(
		XArrayByteOutStream &req, GridStoreChannel::Context &context) {
	if (queryList_.empty()) {
		return false;
	}

	const bool summarized = isSessionUUIDSummarized();
	if (summarized) {
		req.writeAll(sessionUUID_, sizeof(uuid_t));
	}

	const OptionalRequestSource &source = context.bindQueryOptions(NULL);
	tryPutSystemOptionalRequest(req, context, false, &source);

	req << ClientUtil::sizeValueToInt32(queryList_.size());
	for (QueryList::iterator it = queryList_.begin();
			it != queryList_.end(); ++it) {
		GSQuery &query = **it;
		GSContainer &container = *query.getContainer();

		int64_t statementId;
		if (container.autoCommit_ || container.getSessionIdDirect() == 0) {
			statementId = 0;
		}
		else {
			statementId = container.updateStatementIdDirect();
		}

		req << GridStoreChannel::statementToNumber(query.getStatement());
		NodeConnection::putStatementId(req, statementId);
		query.makeRequest(req, summarized);
	}

	return true;
}

void GSGridStoreTag::MultiQueryStatement::acceptMainResponse(
		ArrayByteInStream &resp,
		const GridStoreChannel::ConnectionId &connectionId) {
	int32_t count;
	resp >> count;
	if (queryList_.size() != static_cast<size_t>(count)) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by inconsistent query count");
	}

	for (QueryList::iterator it = queryList_.begin();
			it != queryList_.end(); ++it) {
		GSQuery &query = **it;

		int64_t size;
		resp >> size;
		if (size < 0 || static_cast<size_t>(size) > resp.base().remaining()) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
		}

		const size_t subEndPos =
				resp.base().position() + static_cast<size_t>(size);

		ArrayByteInStream subResp(util::ArrayInStream(
				query.getContainer()->getResponseBuffer().data(), subEndPos));
		subResp.base().position(resp.base().position());

		query.acceptResponse(subResp, connectionId, false);

		resp.base().position(subEndPos);
	}
}

bool GSGridStoreTag::MultiQueryStatement::makeCloseSessionRequest(
		XArrayByteOutStream &req) {
	static_cast<void>(req);
	return false;
}

void GSGridStoreTag::MultiQueryStatement::acceptSessionClosed() {
}

bool GSGridStoreTag::MultiQueryStatement::acceptStatementErrorForSession(
		const StatementException &cause) {
	if (!updateQueryFound_) {
		return false;
	}

	bool initialSessionLost = false;
	for (QueryList::iterator it = queryList_.begin();
			it != queryList_.end(); ++it) {
		GSQuery &query = **it;
		GSContainer &container = *query.getContainer();

		if (GSContainer::isInitialSessionLost(
				Statement::EXECUTE_MULTIPLE_QUERIES,
				container.statementId_,
				container.transactionStarted_, cause)) {
			initialSessionLost = true;
			container.setSessionIdDirect(0, true);
		}
		else {
			container.disableCache();
		}
	}

	return initialSessionLost;
}


GSGridStoreTag::MultiPutStatement::MultiPutStatement(
		GridStoreChannel::Context &context) {
	memcpy(sessionUUID_, context.getSessionUUID(), sizeof(uuid_t));
}

void GSGridStoreTag::MultiPutStatement::add(
		const GSContainerRowEntry &rowEntry, const RowMapper *mapper,
		const ContainerKeyConverter &keyConverter) {
	if (rowEntry.rowCount == 0) {
		return;
	}

	if (rowEntry.rowList == NULL) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
	}

	const ContainerKey normalizedKey =
			keyConverter.parse(rowEntry.containerName, false);
	SubEntryMap::iterator entryIt = subEntryMap_.find(normalizedKey);

	for (size_t i = 0; i < rowEntry.rowCount; i++) {
		void *row = rowEntry.rowList[i];
		if (row == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}

		if (mapper == NULL) {
			if (entryIt == subEntryMap_.end()) {
				mapper = &GSRow::resolve(row).getRowMapper();
			}
			else {
				mapper = &entryIt->second.rowList_[0]->getRowMapper();
			}
		}
		else {
			mapper->checkSchemaMatched(GSRow::resolve(row).getRowMapper());
		}
	}

	if (entryIt == subEntryMap_.end()) {
		SubEntry &entry = subEntryMap_[normalizedKey];
		for (size_t i = 0; i < rowEntry.rowCount; i++) {
			entry.rowList_.push_back(&GSRow::resolve(rowEntry.rowList[i]));
		}
		containerKeyList_.push_back(normalizedKey);

		MapperList::iterator mapperIt =
				std::find(mapperList_.begin(), mapperList_.end(), mapper);
		if (mapperIt == mapperList_.end()) {
			entry.mapperIndex_ = static_cast<int32_t>(mapperList_.size());
			mapperList_.push_back(mapper);
		}
		else {
			entry.mapperIndex_ =
					static_cast<int32_t>(mapperIt - mapperList_.begin());
		}
	}
	else {
		SubEntry &entry = entryIt->second;
		for (size_t i = 0; i < rowEntry.rowCount; i++) {
			entry.rowList_.push_back(&GSRow::resolve(rowEntry.rowList[i]));
		}
	}
}

bool GSGridStoreTag::MultiPutStatement::makeCreateSessionRequest(
		XArrayByteOutStream &req,
		GridStoreChannel &channel, GridStoreChannel::Context &context) {
	if (containerKeyList_.empty()) {
		return false;
	}

	const bool summarized = isSessionUUIDSummarized();
	if (summarized) {
		req.writeAll(sessionUUID_, sizeof(uuid_t));
	}
	NodeConnection::tryPutEmptyOptionalRequest(req);

	const ContainerKeyConverter &keyConverter = context.getKeyConverter();
	const int64_t databaseId = channel.getDatabaseId(context);

	const GSBool withId = GS_FALSE;
	req << withId;
	req << ClientUtil::sizeValueToInt32(containerKeyList_.size());
	for (ContainerKeyList::iterator it = containerKeyList_.begin();
			it != containerKeyList_.end(); ++it) {
		if (isAttributeVerifiable()) {
			NodeConnection::tryPutEmptyOptionalRequest(req);
		}

		const ContainerKey &containerKey = *it;
		keyConverter.put(req, containerKey, databaseId);

		if (!summarized) {
			req.writeAll(sessionUUID_, sizeof(uuid_t));
		}

		if (GSContainer::isSessionIdGeneratorEnabled()) {
			assert(subEntryMap_.find(containerKey) != subEntryMap_.end());
			SubEntry &entry = subEntryMap_[containerKey];
			entry.sessionId_ = context.generateSessionId();
			req << entry.sessionId_;
		}
		else {
			GSContainer::putNewSessionProperties(req, channel, context);
		}
	}

	return true;
}

void GSGridStoreTag::MultiPutStatement::acceptCreateSessionResponse(
		ArrayByteInStream &resp) {
	int32_t sessionCount;
	resp >> sessionCount;
	if (containerKeyList_.size() != static_cast<size_t>(sessionCount)) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by inconsistent session count");
	}

	for (ContainerKeyList::iterator it = containerKeyList_.begin();
			it != containerKeyList_.end(); ++it) {
		const ContainerKey &containerKey = *it;
		assert(subEntryMap_.find(containerKey) != subEntryMap_.end());

		SubEntry &entry = subEntryMap_[containerKey];
		resp >> entry.containerId_;

		if (!GSContainer::isSessionIdGeneratorEnabled()) {
			resp >> entry.sessionId_;
			if (entry.sessionId_ == 0) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
						"Protocol error by empty session ID");
			}
		}
	}
}

bool GSGridStoreTag::MultiPutStatement::makeMainRequest(
		XArrayByteOutStream &req, GridStoreChannel::Context &context) {
	static_cast<void>(context);

	if (containerKeyList_.empty()) {
		return false;
	}

	const bool summarized = isSessionUUIDSummarized();
	if (summarized) {
		req.writeAll(sessionUUID_, sizeof(uuid_t));
	}
	NodeConnection::tryPutEmptyOptionalRequest(req);

	req << ClientUtil::sizeValueToInt32(mapperList_.size());
	for (MapperList::iterator it = mapperList_.begin();
			it != mapperList_.end(); ++it) {
		const RowMapper &mapper = **it;

		GSContainerType containerType;
		switch (mapper.getCategory()) {
		case RowMapper::CATEGORY_COLLECTION:
			containerType = GS_CONTAINER_COLLECTION;
			break;
		case RowMapper::CATEGORY_TIME_SERIES:
			containerType = GS_CONTAINER_TIME_SERIES;
			break;
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
		}

		req << static_cast<int8_t>(containerType);
		mapper.exportSchema(req, getRowMapperConfig());
	}

	req << ClientUtil::sizeValueToInt32(containerKeyList_.size());
	for (ContainerKeyList::iterator it = containerKeyList_.begin();
			it != containerKeyList_.end(); ++it) {
		const ContainerKey &containerKey = *it;
		assert(subEntryMap_.find(containerKey) != subEntryMap_.end());

		const SubEntry &entry = subEntryMap_[containerKey];

		NodeConnection::putStatementId(req, 1);
		req << entry.containerId_;
		req << entry.sessionId_;
		if (!summarized) {
			req.writeAll(sessionUUID_, sizeof(uuid_t));
		}

		if (GSContainer::isSessionIdGeneratorEnabled()) {
			req << static_cast<int8_t>(GSContainer::SESSION_MODE_GET);
			req << static_cast<int8_t>(GSContainer::TRANSACTION_MODE_AUTO);
		}
		NodeConnection::tryPutEmptyOptionalRequest(req);

		req << entry.mapperIndex_;

		int32_t listBytesSize = 0;
		req << listBytesSize;
		const size_t listStartPos = req.base().position();

		req << ClientUtil::sizeValueToInt64(entry.rowList_.size());
		const RowMapper &mapper = *mapperList_[entry.mapperIndex_];
		RowMapper::OutputCursor cursor(
				req, mapper, GSContainer::getRowMappingMode(),
				ClientUtil::sizeValueToInt32(entry.rowList_.size()), false);
		for (std::vector<GSRow*>::const_iterator listIt = entry.rowList_.begin();
				listIt != entry.rowList_.end(); ++listIt) {
			mapper.encode(cursor, NULL, NULL, *listIt);
		}

		const size_t listEndPos = req.base().position();
		req.base().position(listStartPos - sizeof(listBytesSize));
		req << ClientUtil::sizeValueToInt32(listEndPos - listStartPos);
		req.base().position(listEndPos);
	}

	return true;
}

void GSGridStoreTag::MultiPutStatement::acceptMainResponse(
		ArrayByteInStream &resp,
		const GridStoreChannel::ConnectionId &connectionId) {
	static_cast<void>(resp);
	static_cast<void>(connectionId);
}

bool GSGridStoreTag::MultiPutStatement::makeCloseSessionRequest(
		XArrayByteOutStream &req) {
	if (containerKeyList_.empty()) {
		return false;
	}

	const bool summarized = isSessionUUIDSummarized();
	if (summarized) {
		req.writeAll(sessionUUID_, sizeof(uuid_t));
	}
	NodeConnection::tryPutEmptyOptionalRequest(req);

	req << ClientUtil::sizeValueToInt32(containerKeyList_.size());
	for (ContainerKeyList::iterator it = containerKeyList_.begin();
			it != containerKeyList_.end(); ++it) {
		const ContainerKey &containerKey = *it;
		assert(subEntryMap_.find(containerKey) != subEntryMap_.end());

		const SubEntry &entry = subEntryMap_[containerKey];

		NodeConnection::putStatementId(req, 2);
		req << entry.containerId_;
		req << entry.sessionId_;
		if (!summarized) {
			req.writeAll(sessionUUID_, sizeof(uuid_t));
		}
	}

	return true;
}

void GSGridStoreTag::MultiPutStatement::acceptSessionClosed() {
}

bool GSGridStoreTag::MultiPutStatement::acceptStatementErrorForSession(
		const StatementException &cause) {
	return GSContainer::isInitialSessionLost(
			Statement::CREATE_MULTIPLE_SESSIONS, 1, false, cause);
}


GSGridStoreTag::MultiPutStatement::SubEntry::SubEntry() :
		mapperIndex_(0),
		containerId_(0),
		sessionId_(0) {
}


void GSGridStoreTag::MultiGetRequest::add(
		const GSRowKeyPredicateEntry &predicateEntry) {
	if (predicateEntry.predicate == NULL) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
	}

	PredicateList::iterator it = std::find(
			predicateList_.begin(), predicateList_.end(), predicateEntry.predicate);
	if (it == predicateList_.end()) {
		predicateList_.push_back(predicateEntry.predicate);
	}

	entryList_.push_back(predicateEntry);
}

bool GSGridStoreTag::MultiGetRequest::makeRequest(
		XArrayByteOutStream &req,
		GridStoreChannel &channel, GridStoreChannel::Context &context) {
	if (entryList_.empty()) {
		return false;
	}

	if (isSessionUUIDSummarized()) {
		req.writeAll(context.getSessionUUID(), sizeof(uuid_t));
	}
	NodeConnection::tryPutEmptyOptionalRequest(req);

	req << ClientUtil::sizeValueToInt32(predicateList_.size());
	for (PredicateList::iterator it = predicateList_.begin();
			it != predicateList_.end(); ++it) {
		const GSRowKeyPredicate &predicate = **it;
		const GSType keyType = predicate.getKeyType();
		req << static_cast<int8_t>(keyType);

		if (predicate.distinctKeys_.get() == NULL) {
			req << static_cast<int8_t>(PREDICATE_RANGE);

			for (size_t i = 0; i < GSRowKeyPredicate::RANGE_COUNT; i++) {
				const GSRowKeyPredicate::RangeKeyEntry &keyEntry =
						predicate.rangeKeyEntries_[i];
				const bool keySpecified = keyEntry.second;
				req << ClientUtil::toGSBool(keySpecified);

				if (keySpecified) {
					RowMapper::encodeKeyGeneral(
							req, GSContainer::getRowMappingMode(),
							keyType, keyEntry.first);
				}
			}
		}
		else {
			typedef GSRowKeyPredicate::DistinctKeySet DistinctKeySet;
			const DistinctKeySet &keySet = *predicate.distinctKeys_;

			req << static_cast<int8_t>(PREDICATE_DISTINCT);
			req << ClientUtil::sizeValueToInt32(keySet.size());
			for (DistinctKeySet::const_iterator it = keySet.begin();
					it != keySet.end(); ++it) {
				RowMapper::encodeKeyGeneral(
						req, GSContainer::getRowMappingMode(),
						keyType, *it);
			}
		}
	}

	const ContainerKeyConverter &keyConverter = context.getKeyConverter();
	const int64_t databaseId = channel.getDatabaseId(context);

	req << ClientUtil::sizeValueToInt32(entryList_.size());
	for (EntryList::iterator it = entryList_.begin();
			it != entryList_.end(); ++it) {
		const GSRowKeyPredicateEntry &entry = *it;

		PredicateList::iterator predIt = std::find(
				predicateList_.begin(), predicateList_.end(), entry.predicate);
		if (predIt == predicateList_.end()) {
			assert(false);
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
		}

		const int32_t predicateIndex =
				static_cast<int32_t>(predIt - predicateList_.begin());
		assert(predicateList_[predicateIndex] == *predIt);

		if (isAttributeVerifiable()) {
			NodeConnection::tryPutEmptyOptionalRequest(req);
		}

		const ContainerKey &key =
				keyConverter.parse(entry.containerName, false);
		keyConverter.put(req, key, databaseId);
		req << predicateIndex;
	}

	return true;
}

const NodeConnection::OptionalRequestSource*
GSGridStoreTag::ContainerPropertiesOption::get() const {
	return NULL;
}


const bool GSContainerTag::TIME_SERIES_UPDATE_ENABLED = true;
bool GSContainerTag::queryStatementIdPreserved_ = false;

const int32_t GSContainerTag::MAX_SESSION_REPAIR_COUNT = 2;
const int32_t GSContainerTag::SESSION_NOT_FOUND_ERROR_CODE = 110003;
const int32_t GSContainerTag::UUID_UNMATCHED_ERROR_CODE = 110016;
const int32_t GSContainerTag::ROW_SET_NOT_FOUND_ERROR_CODE = 60132;

const int8_t GSContainerTag::ROW_SET_TYPE_PARTIAL_ROWS =
		GS_ROW_SET_QUERY_ANALYSIS + 1;

const Statement::Id GSContainerTag::TIME_SERIES_STATEMENT_UNIFIED_LIST[] = {
		Statement::GET_ROW,
		Statement::QUERY_TQL,
		Statement::PUT_ROW,
		Statement::PUT_MULTIPLE_ROWS,
		Statement::DELETE_ROW,
		Statement::GET_MULTIPLE_ROWS
};

const Statement::Id GSContainerTag::TIME_SERIES_STATEMENT_SEPARATED_LIST[] = {
		Statement::GET_TIME_SERIES_ROW,
		Statement::QUERY_TIME_SERIES_TQL,
		Statement::PUT_TIME_SERIES_ROW,
		Statement::PUT_TIME_SERIES_MULTIPLE_ROWS,
		Statement::DELETE_TIME_SERIES_ROW,
		Statement::GET_TIME_SERIES_MULTIPLE_ROWS
};

const Statement::Id GSContainerTag::FIXED_SESSION_MODE_STATEMENT_LIST[] = {
		Statement::CREATE_SESSION,
		Statement::CLOSE_SESSION,
		Statement::COMMIT_TRANSACTION,
		Statement::ABORT_TRANSACTION
};

GSContainerTag::GSContainerTag(
		GSGridStore &store, RowMapper::Reference mapper,
		int32_t schemaVerId, int32_t partitionId, int64_t containerId,
		std::auto_ptr<ContainerKey> normalizedContainerKey, bool cached) :
		resourceHeader_(GSResourceType::CONTAINER, &store, NULL),
		referenceCount_(1),
		store_(&store),
		mapper_(mapper),
		schemaVerId_(schemaVerId),
		partitionId_(partitionId),
		containerId_(containerId),
		normalizedContainerKey_(normalizedContainerKey),
		sessionId_(0),
		transactionId_(1),
		statementId_(0),
		closed_(false),
		sessionPrepared_(false),
		containerLocked_(false),
		transactionStarted_(false),
		autoCommit_(true),
		cacheDisabled_(!cached) {
	store_->createReference(this);
}

GSContainerTag::~GSContainerTag() {
	GSGridStore::removeReference(store_, this);
}

bool GSContainerTag::isClosed() const {
	return closed_;
}

void GSContainerTag::close(GSContainer **container, bool allRelated) throw() {
	if (container == NULL) {
		return;
	}

	do {
		if (!GSResourceHeader::checkType(
				*container, GSResourceType::CONTAINER)) {
			break;
		}

		(**container).closeRemote();

		size_t &referenceCount = (**container).referenceCount_;
		try {
			ResourceSet &resourceSet = (**container).activeResources_;
			if (allRelated && !resourceSet.empty()) {
				referenceCount++;
				size_t orgSize;
				do {
					orgSize = resourceSet.size();
					void *resource = *(--resourceSet.end());

					GSResourceType::Id type;
					if (!GSResourceHeader::getType(resource, type)) {
						assert(false);
						continue;
					}

					assert(referenceCount > 1);
					switch (type) {
					case GSResourceType::QUERY: {
						GSQuery *query = static_cast<GSQuery*>(resource);
						GSQuery::close(&query);
						break;
					}
					case GSResourceType::ROW_SET: {
						GSRowSet *rowSet = static_cast<GSRowSet*>(resource);
						GSRowSet::close(&rowSet);
						break;
					}
					case GSResourceType::AGGREGATION_RESULT: {
						GSAggregationResult *aggregationResult =
								static_cast<GSAggregationResult*>(resource);
						GSAggregationResult::close(&aggregationResult);
						break;
					}
					case GSResourceType::ROW: {
						GSRow *row = static_cast<GSRow*>(resource);
						GSRow::close(&row);
						break;
					}
					case GSResourceType::GRID_STORE_FACTORY:
					case GSResourceType::GRID_STORE:
					case GSResourceType::CONTAINER:
					case GSResourceType::ROW_KEY_PREDICATE:
					case GSResourceType::PARTITION_CONTROLLER:
					default:
						assert(false);
						break;
					}
					assert(referenceCount > 0);
				}
				while (!resourceSet.empty() && orgSize != resourceSet.size());
				assert(resourceSet.empty());
				if (referenceCount > 1) {
					--referenceCount;
				}
			}
		}
		catch (...) {
		}

		assert(referenceCount > 0);
		if (--referenceCount <= 0) {
			delete *container;
		}
	}
	while(false);

	*container = NULL;
}

void GSContainerTag::createReference(void *resource) {
	GSResourceType::Id type;
	if (!GSResourceHeader::getType(resource, type)) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	switch (type) {
	case GSResourceType::QUERY:
	case GSResourceType::ROW_SET:
	case GSResourceType::AGGREGATION_RESULT:
	case GSResourceType::ROW:
		break;
	case GSResourceType::GRID_STORE_FACTORY:
	case GSResourceType::GRID_STORE:
	case GSResourceType::CONTAINER:
	case GSResourceType::ROW_KEY_PREDICATE:
	case GSResourceType::PARTITION_CONTROLLER:
	default:
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}
	activeResources_.insert(resource);
	referenceCount_++;
}

void GSContainerTag::removeReference(
		GSContainer *&container, void *resource) {
	if (container == NULL) {
		return;
	}

	try {
		ResourceSet &set = container->activeResources_;
		ResourceSet::iterator it = set.find(resource);
		if (it != set.end()) {
			set.erase(it);
		}
		else {
			assert(false);
		}
	}
	catch (...) {
	}

	assert(container->referenceCount_ > 0);
	if (--container->referenceCount_ == 0) {
		delete container;
	}
	container = NULL;
}

const RowMapper& GSContainerTag::getMapper() const {
	return *mapper_;
}

int32_t GSContainerTag::getSchemaVersionId() const {
	return schemaVerId_;
}

int32_t GSContainerTag::getPartitionId() const {
	return partitionId_;
}

int64_t GSContainerTag::getContainerId() const {
	return containerId_;
}

const ContainerKey* GSContainerTag::getContainerKey() const {
	return getNormalizedContainerKey();
}

const ContainerKey* GSContainerTag::getNormalizedContainerKey() const {
	return normalizedContainerKey_.get();
}

GSContainerType GSContainerTag::getType() const {
	checkOpened();
	return mapper_->getContainerType();
}

RowMapper::VarDataPool& GSContainerTag::getVarDataPool() {
	return store_->getVarDataPool();
}

void GSContainerTag::createEventNotification(const GSChar *url) {
	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	NodeConnection::tryPutEmptyOptionalRequest(reqOut);
	reqOut << url;
	executeStatement(Statement::CREATE_EVENT_NOTIFICATION,
			STATEMENT_FAMILY_NONE);
}

void GSContainerTag::dropEventNotification(const GSChar *url) {
	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	NodeConnection::tryPutEmptyOptionalRequest(reqOut);
	reqOut << url;
	executeStatement(Statement::DROP_EVENT_NOTIFICATION,
			STATEMENT_FAMILY_NONE);
}

void GSContainerTag::createTrigger(const GSTriggerInfo *info) {
	const StatementFamily family = (isDDLSessionEnabled() ?
			prepareSession(STATEMENT_FAMILY_POST) : STATEMENT_FAMILY_NONE);

	XArrayByteOutStream reqOut = getRequestOutStream();

	reqOut << getContainerId();
	if (family != STATEMENT_FAMILY_NONE) {
		putSessionInfo(reqOut, sessionId_);
	}
	reqOut << getSchemaVersionId();
	tryPutOptionalRequest(reqOut, false, true, true);

	if (info->name == NULL) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
	}
	reqOut << info->name;
	reqOut << static_cast<int8_t>(info->type);
	if (info->uri == NULL) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
	}
	reqOut << info->uri;
	reqOut << info->eventTypeFlags;
	reqOut << ClientUtil::sizeValueToInt32(info->columnCount);
	for (size_t i = 0; i < info->columnCount; i++) {
		reqOut << mapper_->resolveColumnId(info->columnSet[i]);
	}

	const std::string emptyString("");
	switch (info->type) {
		case GS_TRIGGER_REST:
			{
				reqOut << emptyString;
				reqOut << emptyString;
				reqOut << emptyString;
				reqOut << emptyString;
				reqOut << emptyString;
			}
			break;
		case GS_TRIGGER_JMS:
			{
				reqOut << "activemq";
				if (info->jmsDestinationType == NULL) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
				}
				reqOut << info->jmsDestinationType;
				if (info->jmsDestinationName == NULL) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
				}
				reqOut << info->jmsDestinationName;
				if (info->user == NULL) {
					reqOut << emptyString;
				} else {
					reqOut << info->user;
				}
				if (info->password == NULL) {
					reqOut << emptyString;
				} else {
					reqOut << info->password;
				}
			}
			break;
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	executeStatement(Statement::CREATE_TRIGGER, family);
}

void GSContainerTag::dropTrigger(const GSChar *name) {
	const StatementFamily family = (isDDLSessionEnabled() ?
			prepareSession(STATEMENT_FAMILY_POST) : STATEMENT_FAMILY_NONE);

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	if (family != STATEMENT_FAMILY_NONE) {
		putSessionInfo(reqOut, sessionId_);
	}
	reqOut << getSchemaVersionId();
	tryPutOptionalRequest(reqOut, false, true, false);
	reqOut << name;

	executeStatement(Statement::DROP_TRIGGER, family);
}

void GSContainerTag::createOrDropIndex(
		const GSChar *columnName, GSIndexTypeFlags flags, bool forCreation) {
	GSIndexInfo info = GS_INDEX_INFO_INITIALIZER;
	info.columnName = columnName;

	if (flags == GS_INDEX_FLAG_DEFAULT) {
		createOrDropIndex(info, forCreation);
	}
	else if (flags == 0) {
		GSIndexInfo filteredInfo;
		filterIndexInfo(info, forCreation, filteredInfo);
		commitForDDL();
	}
	else {
		GSIndexTypeFlags curFlag = 1;
		GSIndexTypeFlags restFlags = flags;
		for (; restFlags != 0; curFlag <<= 1) {
			if ((restFlags & curFlag) == 0) {
				continue;
			}

			info.type = curFlag;
			createOrDropIndex(info, forCreation);

			restFlags &= ~curFlag;
		}
	}
}

void GSContainerTag::createOrDropIndex(
		const GSIndexInfo &info, bool forCreation) {
	GSIndexInfo filteredInfo;
	if (!filterIndexInfo(info, forCreation, filteredInfo) && !forCreation) {
		return;
	}

	commitForDDL();

	const StatementFamily family = (isDDLSessionEnabled() ?
			prepareSession(STATEMENT_FAMILY_POST) : STATEMENT_FAMILY_NONE);

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	if (family != STATEMENT_FAMILY_NONE) {
		putSessionInfo(reqOut, sessionId_);
	}
	reqOut << getSchemaVersionId();
	tryPutOptionalRequest(reqOut, false, true, forCreation);

	Statement::Id statement;
	if (GSGridStore::isIndexDetailEnabled()) {
		statement = (forCreation ?
				Statement::CREATE_INDEX_DETAIL : Statement::DROP_INDEX_DETAIL);
		GSGridStore::exportIndexInfo(reqOut, filteredInfo);

		if (!forCreation) {
			const bool anyNameMatches = (filteredInfo.name == NULL);
			const bool anyTypeMatches = (filteredInfo.type == 0);
			reqOut << ClientUtil::toGSBool(anyNameMatches);
			reqOut << ClientUtil::toGSBool(anyTypeMatches);
		}
	}
	else {
		statement = (forCreation ?
				Statement::CREATE_INDEX : Statement::DROP_INDEX);

		if (!forCreation && filteredInfo.column < 0) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER,
					"Column must be specified");
		}
		reqOut << filteredInfo.column;

		if (!forCreation && filteredInfo.type == 0) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER,
					"Type must be specified");
		}
		reqOut << static_cast<int8_t>(GSGridStore::getIndexTypeOrdinal(
				filteredInfo.type, false));
	}

	executeStatement(statement, family);
}

void GSContainerTag::flush() {
	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	NodeConnection::tryPutEmptyOptionalRequest(reqOut);
	executeStatement(Statement::FLUSH_LOG, STATEMENT_FAMILY_NONE);
}

bool GSContainerTag::getRow(
		const GSType *keyType, const void *key,
		void *rowObj, bool forUpdate, bool keyFormatted) {
	if (partitionId_ < 0) {
		return getRowForInternalController(keyType, key, rowObj);
	}

	getVarDataPool().clear();
	if (forUpdate && autoCommit_) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_COMMIT_MODE, "");
	}

	const StatementFamily family = prepareSession(forUpdate ?
			STATEMENT_FAMILY_LOCK : STATEMENT_FAMILY_QUERY);

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	putTransactionInfo(reqOut, family, TRANSACTION_INFO_DEFAULT, &forUpdate);

	if (keyFormatted) {
		mapper_->encodeKeyByString(
				reqOut, getRowMappingMode(),
				*static_cast<const GSChar *const*>(key), NULL);
	}
	else {
		mapper_->encodeKeyByObj(reqOut, getRowMappingMode(), keyType, key);
	}

	const bool timeSeriesMode =
			(mapper_->getCategory() == RowMapper::CATEGORY_TIME_SERIES);
	ArrayByteInStream respIn = executeStatement(Statement::GET_ROW, family);

	GSBool found;
	respIn >> found;
	if (found) {
		RowMapper::InputCursor cursor(
			respIn, *mapper_, getRowMappingMode(), 1,
			!timeSeriesMode, &getVarDataPool());
		mapper_->decode(cursor, rowObj);
	}

	return ClientUtil::toBool(found);
}

bool GSContainerTag::putRow(
		const GSType *keyType, const void *key, const void *rowObj,
		bool keyFormatted) {
	if (partitionId_ < 0) {
		return putRowForInternalController(keyType, key, rowObj);
	}

	const StatementFamily family = prepareSession(STATEMENT_FAMILY_UPDATE);

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	putTransactionInfo(reqOut, family);

	if (keyFormatted) {
		mapper_->encodeWithKeyString(
				reqOut, getRowMappingMode(),
				*static_cast<const GSChar *const*>(key), rowObj);
	}
	else {
		mapper_->encode(
				reqOut, getRowMappingMode(), keyType, key, rowObj);
	}

	ArrayByteInStream respIn = executeStatement(Statement::PUT_ROW, family);

	GSBool exists;
	respIn >> exists;
	return ClientUtil::toBool(exists);
}

bool GSContainerTag::putMultipleRows(
		size_t rowCount, const void *const *rowObjs) {
	if (rowCount == 0) {
		return false;
	}
	else if (rowObjs == NULL) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
	}
	const int32_t rowCountInt32 = ClientUtil::sizeValueToInt32(rowCount);

	const StatementFamily family = prepareSession(STATEMENT_FAMILY_UPDATE);

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	putTransactionInfo(reqOut, family);

	reqOut << static_cast<int64_t>(rowCountInt32);
	RowMapper::OutputCursor cursor(
			reqOut, *mapper_, getRowMappingMode(), rowCountInt32);
	const void *const *rowObjsEnd = rowObjs + rowCount;
	for (const void *const *i = rowObjs; i != rowObjsEnd; ++i) {
		if (*i == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}
		mapper_->encode(cursor, NULL, NULL, *i);
	}

	ArrayByteInStream respIn = executeMultiStepStatement(
			Statement::PUT_MULTIPLE_ROWS, family, rowCountInt32);

	GSBool exists;
	respIn >> exists;
	return ClientUtil::toBool(exists);
}

bool GSContainerTag::removeRow(
		const GSType *keyType, const void *key, bool keyFormatted) {
	const StatementFamily family = prepareSession(STATEMENT_FAMILY_UPDATE);

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	putTransactionInfo(reqOut, family);

	if (keyFormatted) {
		mapper_->encodeKeyByString(
				reqOut, getRowMappingMode(),
				*static_cast<const GSChar *const*>(key), NULL);
	}
	else {
		mapper_->encodeKeyByObj(reqOut, getRowMappingMode(), keyType, key);
	}

	ArrayByteInStream respIn = executeStatement(Statement::DELETE_ROW, family);

	GSBool exists;
	respIn >> exists;
	return ClientUtil::toBool(exists);
}

GSQuery* GSContainerTag::query(const GSChar *queryString) {
	static struct : public QueryFormatter::Typed<Statement::QUERY_TQL> {
		virtual void getString(std::ostream &os, ArrayByteInStream &in) const {
			std::string queryString;
			in >> queryString;
			os << queryString;
		}
	} formatter;

	std::auto_ptr<GSQuery> queryPtr(new GSQuery(*this, formatter));
	queryPtr->getParametersOutStream() << queryString;
	return queryPtr.release();
}

GSRowSet* GSContainerTag::queryAndFetch(
		const QueryParameters &parameters, bool forUpdate) {
	const bool neverCreate = false;
	const StatementFamily family =
			prepareQuerySession(parameters, forUpdate, neverCreate);

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();

	const OptionalRequestSource &source = getContext().bindQueryOptions(NULL);
	putTransactionInfo(
			reqOut, family, TRANSACTION_INFO_DEFAULT, &forUpdate, &source);
	formatQuery(parameters, reqOut);

	ArrayByteInStream respIn =
			executeStatement(parameters.getStatement(), family);

	return acceptQueryResponse(parameters, forUpdate, respIn, true);
}

void GSContainerTag::makeQueryRequest(
		const QueryParameters &parameters, bool forUpdate,
		XArrayByteOutStream &reqOut, bool noUUID) {
	const bool neverCreate = true;
	const StatementFamily family =
			prepareQuerySession(parameters, forUpdate, neverCreate);

	const TransactionInfoType type = (noUUID ?
			TRANSACTION_INFO_NO_UUID : TRANSACTION_INFO_DEFAULT);

	reqOut << getContainerId();
	putTransactionInfo(reqOut, family, type, &forUpdate);
	formatQuery(parameters, reqOut);
}

void GSContainerTag::formatQuery(
		const QueryParameters &parameters, XArrayByteOutStream &reqOut) {
	parameters.putFixed(reqOut);

	const util::NormalXArray<uint8_t> &parameterData =
			parameters.queryData_.data_;
	reqOut.writeAll(parameterData.data(), parameterData.size());
}

GSRowSet* GSContainerTag::acceptQueryResponse(
		const QueryParameters &parameters, bool forUpdate,
		ArrayByteInStream &respIn, bool bufSwapAllowed) {

	const bool timeSeriesMode =
			(mapper_->getCategory() == RowMapper::CATEGORY_TIME_SERIES);

	GSRowSetType rowSetType;
	const RowMapper *rowSetMapper;
	RowMapper::MappingMode mode;
	bool rowIdIncluded;

	PartialFetchStatus fetchStatus;
	PartialExecutionStatus executionStatus =
			PartialExecutionStatus::STATUS_DISABLED;
	ArrayByteInStream rowSetIn((util::ArrayInStream(NULL, 0)));

	if (GSGridStore::isQueryOptionsExtensible()) {
		int8_t rawRowSetType = -1;
		bool partialFound = false;

		int32_t intCount;
		respIn >> intCount;
		const size_t count = ClientUtil::toSizeValue(intCount);
		for (size_t i = 0; i < count; i++) {
			int8_t rawType;
			respIn >> rawType;
			const QueryResultType type = resolveQueryResultType(rawType);

			int32_t intSize;
			respIn >> intSize;
			const size_t size = ClientUtil::toSizeValue(intSize);
			ArrayByteInStream entryIn =
					ClientUtil::subStream(getResponseBuffer(), respIn, size);

			do {
				if (type == RESULT_PARTIAL_FETCH_STATE) {
					if (!partialFound) {
						fetchStatus = PartialFetchStatus::get(entryIn);
						partialFound = true;
						break;
					}
				}
				else if (type == RESULT_PARTIAL_EXECUTION_STATE) {
					if (!partialFound) {
						executionStatus = PartialExecutionStatus::get(entryIn);
						partialFound = true;
						break;
					}
				}
				else {
					if (rawRowSetType < 0) {
						rawRowSetType = rawType;
						rowSetIn = entryIn;
						break;
					}
				}

				GS_CLIENT_THROW_ERROR(
						GS_ERROR_CC_MESSAGE_CORRUPTED,
						"Protocol error by query result type confliction");
			}
			while (false);

			respIn.base().position(respIn.base().position() + size);
		}

		if (rawRowSetType < 0) {
			GS_CLIENT_THROW_ERROR(
					GS_ERROR_CC_MESSAGE_CORRUPTED,
					"Protocol error by no query result type");
		}

		const QueryResultType type = resolveQueryResultType(rawRowSetType);
		rowSetType = resolveRowSetType(type);
		rowSetMapper = &getResultRowMapper(type);
		mode = getResultRowMappingMode(type);
		rowIdIncluded = isResultRowIdIncluded(type);
	}
	else if (isAnyQueryResultTypeEnabled() ||
			parameters.getStatement() == Statement::QUERY_TQL) {
		int8_t rawType;
		respIn >> rawType;
		const QueryResultType type = resolveQueryResultType(rawType);

		rowSetType = resolveRowSetType(type);
		rowSetMapper = &getResultRowMapper(type);
		mode = getResultRowMappingMode(type);
		rowIdIncluded = isResultRowIdIncluded(type);

		if (type == RESULT_PARTIAL_FETCH_STATE) {
			fetchStatus = PartialFetchStatus::get(respIn);
		}

		rowSetIn = respIn;
	}
	else {
		rowSetType = GS_ROW_SET_CONTAINER_ROWS;
		rowSetMapper = mapper_.get();
		mode = getRowMappingMode();
		rowIdIncluded = !timeSeriesMode;

		rowSetIn = respIn;
	}

	if (parameters.isForUpdate(forUpdate)) {
		transactionStarted_ = true;
	}

	const QueryParameters &rowSetParameters = parameters.inherit(
			forUpdate, (autoCommit_ ? 0 : transactionId_),
			transactionStarted_, executionStatus);

	int64_t rowCount;
	rowSetIn >> rowCount;

	GridStoreChannel::ConnectionId connectionId;
	if (fetchStatus.isEnabled()) {
		connectionId = getContext().getLastConnectionId();
	}

	std::auto_ptr<GSRowSet> rowSet;
	try {
		rowSet.reset(new GSRowSet(
				*this, *rowSetMapper, rowCount, getResponseBuffer(),
				rowSetIn, mode, rowIdIncluded, rowSetType,
				rowSetParameters, fetchStatus, connectionId, bufSwapAllowed));
	}
	catch (...) {
		getChannel().initializeResponseBuffer(getResponseBuffer());
		throw;
	}

	if (bufSwapAllowed) {
		getChannel().initializeResponseBuffer(getResponseBuffer());
	}

	return rowSet.release();
}

void GSContainerTag::closeRowSet(
		const PartialFetchStatus &fetchStatus,
		GridStoreChannel::ConnectionId &connectionId) {
	const GridStoreChannel::ConnectionId orgConnectionId = connectionId;
	connectionId = GridStoreChannel::ConnectionId();

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	NodeConnection::tryPutEmptyOptionalRequest(reqOut);

	reqOut << fetchStatus.rowSetId_;
	if (!isRowSetIdHintDisabled()) {
		reqOut << fetchStatus.rowSetIdHint_;
	}

	getChannel().checkActiveConnection(
			getContext(), partitionId_, orgConnectionId);
	executeStatement(Statement::CLOSE_ROW_SET, STATEMENT_FAMILY_NONE);
}

void GSContainerTag::fetchRowSet(
		int64_t remainingCount, const PartialFetchStatus &fetchStatus,
		const QueryParameters &parameters, const RowMapper &resultMapper,
		util::NormalXArray<uint8_t> &resultData,
		ArrayByteInStream &resultInStream,
		RowMapper::InputCursor &cursor,
		GridStoreChannel::ConnectionId &connectionId) {
	const bool neverCreate = true;
	const StatementFamily family =
			prepareQuerySession(parameters, false, neverCreate);

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();

	if (GSGridStore::isQueryOptionsExtensible()) {
		const bool forUpdate = parameters.isForUpdate(false);
		putTransactionInfo(
				reqOut, family, TRANSACTION_INFO_DEFAULT, &forUpdate);
	}
	else {
		reqOut << schemaVerId_;
		tryPutOptionalRequest(reqOut, false, true, false);
	}

	reqOut << fetchStatus.rowSetId_;
	if (!isRowSetIdHintDisabled()) {
		reqOut << fetchStatus.rowSetIdHint_;
	}
	reqOut << (fetchStatus.totalRowCount_ - remainingCount);
	reqOut << parameters.fetchSize_;

	GSBool resultClosed;
	int64_t varDataBaseOffset;
	int64_t resultRowCount;
	size_t topPos;
	try {
		getChannel().checkActiveConnection(getContext(), partitionId_, connectionId);

		ArrayByteInStream respIn =
				executeStatement(Statement::FETCH_ROW_SET, family);
		respIn >> resultClosed;
		respIn >> varDataBaseOffset;
		respIn >> resultRowCount;

		topPos = respIn.base().position();
	}
	catch (util::Exception &e) {
		if (e.getErrorCode() == ROW_SET_NOT_FOUND_ERROR_CODE) {
			connectionId = GridStoreChannel::ConnectionId();
			GS_CLIENT_RETHROW_ERROR_CODED(
					GS_ERROR_CC_RECOVERABLE_ROW_SET_LOST, e, "");
		}
		if (GridStoreChannel::isRecoverableError(e.getErrorCode())) {
			connectionId = GridStoreChannel::ConnectionId();
		}
		throw;
	}

	if (resultClosed) {
		connectionId = GridStoreChannel::ConnectionId();
	}

	const int64_t newRemainingCount = remainingCount - resultRowCount;
	if (newRemainingCount < 0 ||
			(resultClosed && newRemainingCount > 0 &&
					!isPartialRowSetLostAcceptable()) ||
			(!resultClosed && newRemainingCount == 0)) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Unexpected result (resultClosed=" << resultClosed <<
				", resultRowCount=" << resultRowCount <<
				", remainingCount=" << remainingCount << ")");
	}

	resultData.swap(getResponseBuffer());
	resultInStream = ArrayByteInStream(
			util::ArrayInStream(resultData.data(), resultData.size()));
	resultInStream.base().position(topPos);

	const bool rowIdIncluded = cursor.isRowIdIncluded();

	cursor.~InputCursor();
	new(&cursor) RowMapper::InputCursor(
			resultInStream, resultMapper, getRowMappingMode(),
			static_cast<int32_t>(resultRowCount),
			rowIdIncluded, &getVarDataPool());
	cursor.setVarDataBaseOffset(varDataBaseOffset);
}

void GSContainerTag::removeRow(
		const RowMapper &resolvedMapper, int64_t transactionId,
		const bool *transactionStarted, int64_t rowId, const void *key) {
	checkTransactionPreserved(true, transactionId, transactionStarted, true);

	if (&resolvedMapper != mapper_.get()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();

	const StatementFamily family = STATEMENT_FAMILY_UPDATE;
	if (resolvedMapper.getCategory() == RowMapper::CATEGORY_TIME_SERIES) {
		putTransactionInfo(reqOut, family);

		resolvedMapper.encodeKeyByObj(reqOut, getRowMappingMode(), NULL, key);
		executeStatement(Statement::DELETE_ROW, family);
	}
	else {
		putTransactionInfo(reqOut, family, TRANSACTION_INFO_SKIP_COMMIT_MODE);

		reqOut << rowId;
		executeStatement(Statement::DELETE_ROW_BY_ID, family);
	}
}

void GSContainerTag::updateRow(
		const RowMapper &resolvedMapper, int64_t transactionId,
		const bool *transactionStarted, int64_t rowId, const void *key,
		const void *rowObj) {
	if (resolvedMapper.getCategory() == RowMapper::CATEGORY_TIME_SERIES &&
			!TIME_SERIES_UPDATE_ENABLED) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	checkTransactionPreserved(true, transactionId, transactionStarted, true);

	if (&resolvedMapper != mapper_.get()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();

	Statement::Id statement;
	const StatementFamily family = STATEMENT_FAMILY_UPDATE;
	if (resolvedMapper.getCategory() == RowMapper::CATEGORY_TIME_SERIES) {
		putTransactionInfo(reqOut, family);

		statement = Statement::PUT_ROW;
	}
	else {
		putTransactionInfo(reqOut, family, TRANSACTION_INFO_SKIP_COMMIT_MODE);
		reqOut << rowId;

		statement = Statement::UPDATE_ROW_BY_ID;
	}

	resolvedMapper.encode(
			reqOut, getRowMappingMode(), NULL, key, rowObj);
	executeStatement(statement, family);
}

void GSContainerTag::abort() {
	if (autoCommit_) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_COMMIT_MODE,
				"Illegal commit mode");
	}

	if (!transactionStarted_) {
		return;
	}

	if (++transactionId_ == 0) {
		transactionId_ = 1;
	}

	try {
		XArrayByteOutStream reqOut = getRequestOutStream();
		reqOut << getContainerId();
		putSessionInfo(reqOut, sessionId_);
		NodeConnection::tryPutEmptyOptionalRequest(reqOut);

		executeStatement(Statement::ABORT_TRANSACTION, STATEMENT_FAMILY_POST);
		transactionStarted_ = false;
	}
	catch (...) {
		transactionStarted_ = false;
		closeSubResources(true, true);
		throw;
	}
	closeSubResources(true, true);
}

void GSContainerTag::commit() {
	if (autoCommit_) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_COMMIT_MODE,
				"Illegal commit mode");
	}

	if (!transactionStarted_) {
		return;
	}

	if (++transactionId_ == 0) {
		transactionId_ = 1;
	}

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	putSessionInfo(reqOut, sessionId_);
	NodeConnection::tryPutEmptyOptionalRequest(reqOut);

	try {
		executeStatement(Statement::COMMIT_TRANSACTION, STATEMENT_FAMILY_POST);
	}
	catch (...) {
		transactionStarted_ = false;
		throw;
	}
	transactionStarted_ = false;
}

void GSContainerTag::setAutoCommit(bool enabled) {
	checkOpened();
	if (autoCommit_ && !enabled) {
		transactionStarted_ = false;
		autoCommit_ = false;
	}
	else if (!autoCommit_ && enabled) {
		commit();
		autoCommit_ = true;
	}
}

GSQuery* GSContainerTag::queryByGeometry(
		const GSChar *column, const GSChar *geometry,
		GSGeometryOperator geometryOp) {
	static struct : public QueryFormatter::Typed<
			Statement::QUERY_COLLECTION_GEOMETRY_RELATED> {
	} formatter;

	checkOpened();
	if (mapper_->getCategory() != RowMapper::CATEGORY_COLLECTION) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}
	const RowMapper::MappingMode mode = getRowMappingMode();

	std::auto_ptr<GSQuery> queryPtr(new GSQuery(*this, formatter));

	XArrayByteOutStream paramOut = queryPtr->getParametersOutStream();
	paramOut << mapper_->resolveColumnId(column);
	RowMapper::encodeGeometry(paramOut, geometry, mode);

	switch (geometryOp) {
	case GS_GEOMETRY_OPERATOR_INTERSECT:
		break;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNKNOWN_GEOMETRY_OPERATOR, "");
	}
	paramOut << static_cast<int8_t>(geometryOp);

	return queryPtr.release();
}

GSQuery* GSContainerTag::queryByGeometry(const GSChar *column,
		const GSChar *geometryIntersection, const GSChar *geometryDisjoint) {
	static struct : public QueryFormatter::Typed<
			Statement::QUERY_COLLECTION_GEOMETRY_WITH_EXCLUSION> {
	} formatter;

	checkOpened();
	if (mapper_->getCategory() != RowMapper::CATEGORY_COLLECTION) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}
	const RowMapper::MappingMode mode = getRowMappingMode();

	std::auto_ptr<GSQuery> queryPtr(new GSQuery(*this, formatter));

	XArrayByteOutStream paramOut = queryPtr->getParametersOutStream();
	paramOut << mapper_->resolveColumnId(column);
	RowMapper::encodeGeometry(paramOut, geometryIntersection, mode);
	RowMapper::encodeGeometry(paramOut, geometryDisjoint, mode);

	return queryPtr.release();
}

GSAggregationResult* GSContainerTag::aggregateTimeSeries(
		GSTimestamp start, GSTimestamp end,
		const GSChar *column, GSAggregation aggregation) {
	if (mapper_->getCategory() != RowMapper::CATEGORY_TIME_SERIES) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	const StatementFamily family = prepareSession(STATEMENT_FAMILY_QUERY);

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	putTransactionInfo(reqOut, family, TRANSACTION_INFO_SKIP_COMMIT_MODE);

	reqOut << start;
	reqOut << end;
	const int32_t columnId = (
			column == NULL ? -1 : mapper_->resolveColumnId(column));
	reqOut << columnId;

	switch (aggregation) {
	case GS_AGGREGATION_MINIMUM:
	case GS_AGGREGATION_MAXIMUM:
	case GS_AGGREGATION_TOTAL:
	case GS_AGGREGATION_AVERAGE:
	case GS_AGGREGATION_VARIANCE:
	case GS_AGGREGATION_STANDARD_DEVIATION:
	case GS_AGGREGATION_COUNT:
	case GS_AGGREGATION_WEIGHTED_AVERAGE:
		break;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNKNOWN_AGGREGATION, "");
	}
	reqOut << static_cast<int8_t>(aggregation);

	ArrayByteInStream respIn =
			executeStatement(Statement::AGGREGATE_TIME_SERIES, family);

	int8_t exists;
	respIn >> exists;
	if (!exists) {
		return NULL;
	}

	std::auto_ptr<GSAggregationResult> aggregationResult(
			new GSAggregationResult(*this));
	RowMapper::getAggregationResultMapper().decode(
			respIn, RowMapper::MODE_AGGREGATED, NULL, aggregationResult.get());
	return aggregationResult.release();
}

bool GSContainerTag::appendTimeSeriesRow(const void *rowObj) {
	if (mapper_->getCategory() != RowMapper::CATEGORY_TIME_SERIES) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	const StatementFamily family = prepareSession(STATEMENT_FAMILY_POST);

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	putTransactionInfo(reqOut, family);

	GSTimestamp dummyKey = 0;
	mapper_->encode(
			reqOut, getRowMappingMode(), NULL, &dummyKey, rowObj);
	ArrayByteInStream respIn =
			executeStatement(Statement::APPEND_TIME_SERIES_ROW, family);

	GSBool exists;
	respIn >> exists;
	return ClientUtil::toBool(exists);
}

bool GSContainerTag::getTimeSeriesRow(
		GSTimestamp base, GSTimeOperator timeOp, void *rowObj) {
	getVarDataPool().clear();
	if (mapper_->getCategory() != RowMapper::CATEGORY_TIME_SERIES) {
		GS_CLIENT_THROW_ERROR(0, "");
	}

	const StatementFamily family = prepareSession(STATEMENT_FAMILY_QUERY);

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	putTransactionInfo(reqOut, family, TRANSACTION_INFO_SKIP_COMMIT_MODE);

	reqOut << base;

	switch (timeOp) {
	case GS_TIME_OPERATOR_NEXT:
	case GS_TIME_OPERATOR_NEXT_ONLY:
	case GS_TIME_OPERATOR_PREVIOUS:
	case GS_TIME_OPERATOR_PREVIOUS_ONLY:
		break;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNKNOWN_TIME_OPERATOR, "");
	}
	reqOut << static_cast<int8_t>(timeOp);

	ArrayByteInStream respIn =
			executeStatement(Statement::GET_TIME_SERIES_ROW_RELATED, family);

	GSBool found;
	respIn >> found;
	if (found) {
		mapper_->decode(respIn, getRowMappingMode(),
				&getVarDataPool(), rowObj);
	}

	return ClientUtil::toBool(found);
}

bool GSContainerTag::interpolateTimeSeriesRow(
		GSTimestamp base, const GSChar *column, void *rowObj) {
	getVarDataPool().clear();
	if (mapper_->getCategory() != RowMapper::CATEGORY_TIME_SERIES) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	const StatementFamily family = prepareSession(STATEMENT_FAMILY_QUERY);

	XArrayByteOutStream reqOut = getRequestOutStream();
	reqOut << getContainerId();
	putTransactionInfo(reqOut, family, TRANSACTION_INFO_SKIP_COMMIT_MODE);

	reqOut << base;
	reqOut << mapper_->resolveColumnId(column);

	ArrayByteInStream respIn =
			executeStatement(Statement::INTERPOLATE_TIME_SERIES_ROW, family);

	GSBool found;
	respIn >> found;
	if (found) {
		mapper_->decode(respIn, getRowMappingMode(),
				&getVarDataPool(), rowObj);
	}

	return ClientUtil::toBool(found);
}

GSQuery* GSContainerTag::queryByTime(
		const GSTimestamp *start, const GSTimestamp *end,
		GSQueryOrder order) {
	static struct : public QueryFormatter::Typed<
			Statement::QUERY_TIME_SERIES_RANGE> {
	} formatter;

	checkOpened();
	if (mapper_->getCategory() != RowMapper::CATEGORY_TIME_SERIES) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	std::auto_ptr<GSQuery> queryPtr(new GSQuery(*this, formatter));

	XArrayByteOutStream paramOut = queryPtr->getParametersOutStream();
	paramOut << wrapOptionalTimestamp(start);
	paramOut << wrapOptionalTimestamp(end);
	paramOut << static_cast<int8_t>(checkQueryOrder(order));
	return queryPtr.release();
}

GSQuery* GSContainerTag::queryByTime(
		GSTimestamp start, GSTimestamp end,
		const GSChar *const *columnSet,
		size_t columnCount, bool useColumnCount,
		GSInterpolationMode mode,
		int32_t interval, GSTimeUnit intervalUnit) {
	static struct : public QueryFormatter::Typed<
			Statement::QUERY_TIME_SERIES_SAMPLING> {
	} formatter;

	checkOpened();
	if (mapper_->getCategory() != RowMapper::CATEGORY_TIME_SERIES) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	std::auto_ptr<GSQuery> queryPtr(new GSQuery(*this, formatter));

	XArrayByteOutStream paramOut = queryPtr->getParametersOutStream();
	paramOut << start;
	paramOut << end;

	int32_t columnCountInt32 = 0;
	if (useColumnCount) {
		columnCountInt32 = ClientUtil::sizeValueToInt32(columnCount);
		if (columnCountInt32 > 0 && columnSet == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
		}
	}
	else {
		if (columnSet != NULL) {
			for (const GSChar *const *i = columnSet; *i != NULL; i++) {
				columnCountInt32++;
			}
		}
	}
	paramOut << columnCountInt32;

	for (int32_t i = 0; i < columnCountInt32; i++) {
		paramOut << mapper_->resolveColumnId(columnSet[i]);
	}

	paramOut << interval;

	paramOut << static_cast<int8_t>(ClientUtil::checkTimeUnit(intervalUnit));

	paramOut << static_cast<int8_t>(checkInterpolationMode(mode));

	return queryPtr.release();
}

template<typename Traits>
GSResult GSContainerTag::getRowChecked(
		GSContainer *container, const typename Traits::Object *key,
		void *rowObj, GSBool forUpdate, GSBool *exists,
		const GSInterceptor::FunctionInfo &funcInfo) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(exists, GS_FALSE);
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(
			funcInfo, container, key, rowObj, forUpdate, exists);

	GSBool existsStorage;
	if (exists == NULL) {
		exists = &existsStorage;
	}

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(key);
		GS_CLIENT_CHECK_NOT_NULL(rowObj);

		*exists = container->getRow(
				Traits::resolveKeyType(), key,
				rowObj, ClientUtil::toBool(forUpdate), false);
	}
	catch (...) {
		*exists = GS_FALSE;
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

template<typename Traits>
GSResult GSContainerTag::putRowChecked(
		GSContainer *container, const typename Traits::Object *key,
		const void *rowObj, GSBool *exists,
		const GSInterceptor::FunctionInfo &funcInfo) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(exists, GS_FALSE);
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(
			funcInfo, container, key, rowObj, exists);

	GSBool existsStorage;
	if (exists == NULL) {
		exists = &existsStorage;
	}

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(rowObj);

		*exists = container->putRow(
				Traits::resolveKeyType(), key, rowObj, false);
	}
	catch (...) {
		*exists = GS_FALSE;
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

template<typename Traits>
GSResult GSContainerTag::removeRowChecked(
		GSContainer *container, const typename Traits::Object *key,
		GSBool *exists, const GSInterceptor::FunctionInfo &funcInfo) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(exists, GS_FALSE);
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(funcInfo, container, key, exists);

	GSBool existsStorage;
	if (exists == NULL) {
		exists = &existsStorage;
	}

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(key);

		*exists = container->removeRow(Traits::resolveKeyType(), key, false);
	}
	catch (...) {
		*exists = GS_FALSE;
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSRow* GSContainerTag::createRow() {
	checkOpened();
	RowMapper::Cache &cache = RowMapper::getDefaultCache();
	RowMapper::Reference mapper(cache, cache.duplicate(*mapper_));

	return GSRow::create(*this, mapper);
}

void GSContainerTag::checkOpened() const {
	if (isClosed()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_RESOURCE_CLOSED, "");
	}
}

GridStoreChannel& GSContainerTag::getChannel() {
	return store_->getChannel();
}

GridStoreChannel::Context& GSContainerTag::getContext() {
	return store_->getContext();
}

util::NormalXArray<uint8_t>& GSContainerTag::getRequestBuffer() {
	return store_->getRequestBuffer();
}

util::NormalXArray<uint8_t>& GSContainerTag::getResponseBuffer() {
	return store_->getResponseBuffer();
}

GSInterpolationMode GSContainerTag::checkInterpolationMode(
		GSInterpolationMode interpolationMode) {
	switch (interpolationMode) {
	case GS_INTERPOLATION_LINEAR_OR_PREVIOUS:
	case GS_INTERPOLATION_EMPTY:
		return interpolationMode;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}
}

GSQueryOrder GSContainerTag::checkQueryOrder(GSQueryOrder order) {
	switch (order) {
	case GS_ORDER_ASCENDING:
	case GS_ORDER_DESCENDING:
		return order;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}
}

ArrayByteInStream GSContainerTag::executeStatement(
		Statement::Id statement, StatementFamily familyForSession) {
	if (closed_) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_CONTAINER_CLOSED, "");
	}

	const bool sessionRequired = (familyForSession != STATEMENT_FAMILY_NONE);
	for (int trialCount = 0;; trialCount++) {
		int64_t statementId;
		if (familyForSession == STATEMENT_FAMILY_NONE) {
			statementId = 0;
		}
		else {
			if (!queryStatementIdPreserved_ ||
					familyForSession != STATEMENT_FAMILY_QUERY) {
				while (++statementId_ == 0) {
				}
			}
			statementId = statementId_;
			if (sessionId_ == 0 || statementId == 0) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
			}
		}

		const size_t requestSize = getRequestBuffer().size();
		try {
			Statement::Id actualStatement;
			if (!GSGridStore::isContainerStatementUnified() &&
					mapper_->getContainerType() ==
							GS_CONTAINER_TIME_SERIES) {
				const Statement::Id *it = ClientUtil::findFromArray(
						TIME_SERIES_STATEMENT_UNIFIED_LIST,
						sizeof(TIME_SERIES_STATEMENT_UNIFIED_LIST),
						statement);
				if (it == NULL) {
					actualStatement = statement;
				}
				else {
					actualStatement = TIME_SERIES_STATEMENT_SEPARATED_LIST[
							it - TIME_SERIES_STATEMENT_UNIFIED_LIST];
				}
			}
			else {
				actualStatement = statement;
			}

			getChannel().applyPartitionId(getContext(), getPartitionId());
			ArrayByteInStream respIn = getChannel().executeStatement(
					getContext(), actualStatement, statementId,
					getRequestBuffer(), getResponseBuffer());

			if (sessionRequired) {
				sessionPrepared_ = true;
			}

			if (!autoCommit_ && sessionRequired) {
				transactionStarted_ = true;
			}

			return respIn;
		}
		catch (StatementException &e) {
			const bool started = transactionStarted_ || containerLocked_;
			if (statement != Statement::CLOSE_SESSION) {
				try {
					closeSession(true);
				}
				catch (...) {
				}
			}

			if (!sessionRequired || !isInitialSessionLost(
					statement, statementId, started, e)) {
				try {
					disableCache();
				}
				catch (...) {
				}
				throw;
			}
			else if (trialCount >= MAX_SESSION_REPAIR_COUNT) {
				GS_CLIENT_RETHROW_ERROR(
						e,
						"Failed to repair session (trialCount=" << trialCount << ")");
			}
		}

		util::NormalXArray<uint8_t> requestData;
		requestData.resize(requestSize);
		memcpy(requestData.data(), getRequestBuffer().data(), requestSize);
		XArrayByteOutStream tmpReqOut = getRequestOutStream();
		const size_t containerIdSize = sizeof(int64_t);
		const size_t sessionIdPos = tmpReqOut.base().position() + containerIdSize;

		createSession();

		getRequestBuffer().resize(requestSize);
		memcpy(getRequestBuffer().data(), requestData.data(), requestSize);
		XArrayByteOutStream reqOut = getRequestOutStream();
		reqOut.base().position(sessionIdPos);
		reqOut << sessionId_;

		if (isSessionIdGeneratorEnabled() && ClientUtil::findFromArray(
				FIXED_SESSION_MODE_STATEMENT_LIST,
				sizeof(FIXED_SESSION_MODE_STATEMENT_LIST),
				statement) == NULL) {
			const size_t uuidSize = sizeof(uuid_t);
			const size_t schemaVerIdSize = sizeof(int32_t);
			reqOut.base().position(
					reqOut.base().position() + uuidSize + schemaVerIdSize);
			reqOut << static_cast<int8_t>(SESSION_MODE_GET);
		}

		reqOut.base().position(requestSize);
	}
}

ArrayByteInStream GSContainerTag::executeMultiStepStatement(
		Statement::Id statement, StatementFamily familyForSession,
		int32_t statementStep) {
	if (statementStep <= 0) {
		assert(false);
		GS_CLIENT_THROW_ERROR(
				GS_ERROR_CC_INTERNAL_ERROR, "Internal error by illegal step");
	}

	struct Stepper {
		~Stepper() {
			try {
				if (container_.sessionPrepared_ &&
						container_.isMultiStepStatementIdEnabled()) {
					container_.statementId_ += statementStep_ - 1;
				}
			}
			catch (...) {
			}
		}
		GSContainer &container_;
		const int32_t statementStep_;
	} stepper = { *this, statementStep };
	(void) stepper;

	return executeStatement(statement, familyForSession);
}

int64_t GSContainerTag::getSessionIdDirect() {
	return sessionId_;
}

void GSContainerTag::setSessionIdDirect(
		int64_t sessionId, bool statusUpdatable) {
	sessionId_ = sessionId;

	if (statusUpdatable) {
		if (sessionId == 0) {
			statementId_ = 0;

			if (++transactionId_ == 0) {
				transactionId_ = 1;
			}

			sessionPrepared_ = false;
			containerLocked_ = false;
			transactionStarted_ = false;
		}
		else {
			sessionPrepared_ = true;
		}
	}
}

int64_t GSContainerTag::updateStatementIdDirect() {
	if (sessionId_ == 0) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}
	while (++statementId_ == 0) {
	}

	return statementId_;
}

bool GSContainerTag::isInitialSessionRetrialEnabled() {
	return (NodeConnection::getProtocolVersion() >= 2);
}

bool GSContainerTag::isNewSessionConflicted(
		const StatementException &cause) {
	if (!isSessionIdGeneratorEnabled()) {
		return false;
	}

	return (cause.getErrorCode() == UUID_UNMATCHED_ERROR_CODE);
}

bool GSContainerTag::isInitialSessionLost(
		Statement::Id statement, int64_t statementId,
		bool transactionStarted,
		const StatementException &cause) {
	if (statement == Statement::CREATE_SESSION ||
			statement == Statement::CLOSE_SESSION) {
		return false;
	}

	const int32_t errorCode = cause.getErrorCode();
	if (errorCode != SESSION_NOT_FOUND_ERROR_CODE &&
			errorCode != UUID_UNMATCHED_ERROR_CODE) {
		return false;
	}

	if (!isInitialSessionRetrialEnabled()) {
		return false;
	}

	if (isSessionIdGeneratorEnabled()) {
		if (transactionStarted) {
			return false;
		}
	}
	else {
		if (statementId != 1) {
			return false;
		}
	}

	return true;
}

void GSContainerTag::putSessionInfo(
		XArrayByteOutStream &reqOut, int64_t sessionId) {
	reqOut << sessionId;
	if (isInitialSessionRetrialEnabled()) {
		reqOut.writeAll(getContext().getSessionUUID(), sizeof(uuid_t));
	}
}

void GSContainerTag::commitForDDL() {
	if (!autoCommit_ && transactionStarted_) {
		commit();
	}
}

XArrayByteOutStream GSContainerTag::getRequestOutStream() {
	return getChannel().getRequestOutStream(getRequestBuffer());
}

GSContainer::StatementFamily GSContainerTag::prepareSession(
		StatementFamily family) {
	bool sessionRequired;
	if (isSessionIdGeneratorEnabled()) {
		bool requiredImmediately;
		switch (family) {
		case STATEMENT_FAMILY_QUERY:
			sessionRequired = (!autoCommit_ && transactionStarted_);
			requiredImmediately = false;
			break;
		case STATEMENT_FAMILY_LOCK:
			if (autoCommit_) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_COMMIT_MODE,
						"Illegal commit mode");
			}
			sessionRequired = true;
			requiredImmediately = false;
			break;
		case STATEMENT_FAMILY_UPDATE:
			sessionRequired = (containerLocked_ ||
					!autoCommit_ || !mapper_->hasKey());
			requiredImmediately = (sessionRequired && !mapper_->hasKey());
			break;
		case STATEMENT_FAMILY_POST:
			sessionRequired = true;
			requiredImmediately = true;
			break;
		default:
			assert(false);
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
		}

		if (!sessionPrepared_) {
			if (requiredImmediately) {
				createSession();
			}
			else if (sessionRequired) {
				sessionId_ = getContext().generateSessionId();
			}
		}
	}
	else {
		sessionRequired =
				(family != STATEMENT_FAMILY_QUERY ||
				(!autoCommit_ && sessionId_ != 0));

		if (sessionRequired && sessionId_ == 0) {
			createSession();
		}
	}

	return (sessionRequired ? family : STATEMENT_FAMILY_NONE);
}

void GSContainerTag::createSession() {
	ContainerCache *cache = getContext().getContainerCache();
	if (cache != NULL) {
		GridStoreChannel::SessionInfo sessionInfo;
		if (cache->takeSession(partitionId_, containerId_, sessionInfo)) {
			setSessionIdDirect(sessionInfo.getSessionId(), true);
			statementId_ = sessionInfo.getLastStatementId();
			return;
		}
	}

	for (int trialCount = 0;; trialCount++) {
		XArrayByteOutStream reqOut = getRequestOutStream();
		reqOut << getContainerId();

		const int64_t generatedSessionId = (isSessionIdGeneratorEnabled() ?
				getContext().generateSessionId() : 0);
		if (generatedSessionId != 0) {
			reqOut << generatedSessionId;
		}
		if (isInitialSessionRetrialEnabled()) {
			reqOut.writeAll(getContext().getSessionUUID(), sizeof(uuid_t));
		}

		NodeConnection::tryPutEmptyOptionalRequest(reqOut);

		if (generatedSessionId == 0) {
			putNewSessionProperties(reqOut, getChannel(), getContext());
		}

		int64_t newSessionId;
		try {
			ArrayByteInStream respIn = executeStatement(
					Statement::CREATE_SESSION, STATEMENT_FAMILY_NONE);
			if (generatedSessionId == 0) {
				respIn >> newSessionId;
				if (newSessionId == 0) {
					GS_CLIENT_THROW_CONNECTION(
							GS_ERROR_CC_MESSAGE_CORRUPTED,
							"Protocol error by empty session ID");
				}
			}
			else {
				newSessionId = generatedSessionId;
			}
		}
		catch (StatementException &e) {
			if (!isNewSessionConflicted(e)) {
				throw;
			}
			else if (trialCount >= MAX_SESSION_REPAIR_COUNT) {
				GS_CLIENT_RETHROW_ERROR(
						e,
						"Failed to create session (trialCount=" << trialCount << ")");
			}
			continue;
		}

		sessionId_ = newSessionId;
		sessionPrepared_ = true;
		break;
	}
}

void GSContainerTag::putTransactionInfo(
		XArrayByteOutStream &reqOut, StatementFamily familyForSession,
		TransactionInfoType type, const bool *forUpdate,
		const OptionalRequestSource *source) {
	const bool sessionRequired = (familyForSession != STATEMENT_FAMILY_NONE);
	if (sessionRequired && sessionId_ == 0) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	reqOut << (sessionRequired ? sessionId_ : 0);

	if (type != TRANSACTION_INFO_NO_UUID &&
			isInitialSessionRetrialEnabled()) {
		reqOut.writeAll(getContext().getSessionUUID(), sizeof(uuid_t));
	}

	const bool generatorEnabled = isSessionIdGeneratorEnabled();
	if (!generatorEnabled) {
		if (forUpdate != NULL) {
			reqOut << ClientUtil::toGSBool(*forUpdate);
		}

		if (type != TRANSACTION_INFO_SKIP_COMMIT_MODE) {
			reqOut << ClientUtil::toGSBool(autoCommit_);
		}
	}

	reqOut << schemaVerId_;

	if (generatorEnabled) {
		if (sessionRequired) {
			reqOut << static_cast<int8_t>(sessionPrepared_ ?
					SESSION_MODE_GET : SESSION_MODE_CREATE);
		}
		else {
			reqOut << static_cast<int8_t>(SESSION_MODE_AUTO);
		}

		if (autoCommit_ || !sessionRequired) {
			reqOut << static_cast<int8_t>(TRANSACTION_MODE_AUTO);
		}
		else {
			reqOut << static_cast<int8_t>(transactionStarted_ ?
					TRANSACTION_MODE_CONTINUE :
					TRANSACTION_MODE_BEGIN);
		}
	}

	tryPutOptionalRequest(
			reqOut, (forUpdate != NULL && *forUpdate), true, false, source);
}

void GSContainerTag::tryPutOptionalRequest(
		XArrayByteOutStream &reqOut,
		bool forUpdate, bool containerLockAware, bool forCreationDDL,
		const OptionalRequestSource *source) {
	if (!NodeConnection::isOptionalRequestEnabled()) {
		return;
	}

	const bool containerLockRequired =
			(containerLockAware && containerLocked_);
	const bool clientIdRequired =
			forCreationDDL && GSGridStore::isClientIdEnabled();

	if (forUpdate || containerLockRequired || clientIdRequired ||
			(source != NULL && source->hasOptions())) {
		NodeConnection::OptionalRequest optionalRequest;
		optionalRequest.forUpdate_ = forUpdate;
		optionalRequest.containerLockRequired_ = containerLockRequired;
		if (clientIdRequired) {
			optionalRequest.clientId_ = getContext().generateClientId();
		}
		if (source != NULL) {
			source->putOptions(optionalRequest);
		}
		optionalRequest.format(reqOut);
	}
	else {
		NodeConnection::tryPutEmptyOptionalRequest(reqOut);
	}
}

bool GSContainerTag::isSessionIdGeneratorEnabled() {
	return (NodeConnection::getProtocolVersion() >= 3);
}

bool GSContainerTag::isDDLSessionEnabled() {
	return (NodeConnection::getProtocolVersion() >= 3 &&
			!GridStoreChannel::v15DDLCompatible_);
}

bool GSContainerTag::isRowSetIdHintDisabled() {
	return (NodeConnection::getProtocolVersion() >= 3);
}

bool GSContainerTag::isPartialRowSetLostAcceptable() {
	return (NodeConnection::getProtocolVersion() >= 3);
}

RowMapper::MappingMode GSContainerTag::getRowMappingMode() {
	if (NodeConnection::getProtocolVersion() >= 3) {
		return RowMapper::MODE_ROWWISE_SEPARATED_V2;
	}
	else {
		return RowMapper::MODE_ROWWISE_SEPARATED;
	}
}

bool GSContainerTag::isMultiStepStatementIdEnabled() {
	return (NodeConnection::getProtocolVersion() >= 5 &&
			!GridStoreChannel::v21StatementIdCompatible_);
}

void GSContainerTag::putNewSessionProperties(
		XArrayByteOutStream &reqOut,
		GridStoreChannel &channel,
		GridStoreChannel::Context &context) {
	reqOut << Properties::timeoutToInt32Seconds(
			channel.getFailoverTimeoutMillis(context));
	reqOut << Properties::timeoutToInt32Seconds(
			context.getTransactionTimeoutMillis());
}

void GSContainerTag::closeSession(bool invalidating) {
	if (sessionId_ == 0) {
		return;
	}

	try {
		ContainerCache *cache = getContext().getContainerCache();
		if (cache == NULL || transactionStarted_ || invalidating) {
			XArrayByteOutStream reqOut = getRequestOutStream();
			reqOut << getContainerId();
			putSessionInfo(reqOut, sessionId_);
			NodeConnection::tryPutEmptyOptionalRequest(reqOut);

			executeStatement(Statement::CLOSE_SESSION, STATEMENT_FAMILY_POST);
		}
		else {
			GridStoreChannel::SessionInfo oldSessionInfo;
			if (cache->cacheSession(partitionId_, containerId_, sessionId_,
					statementId_, oldSessionInfo)) {
				closeSession(getChannel(), getContext(), oldSessionInfo);
			}
		}
	}
	catch (...) {
		setSessionIdDirect(0, true);
		throw;
	}
	setSessionIdDirect(0, true);
}

void GSContainerTag::closeSession(
		GridStoreChannel &channel,
		GridStoreChannel::Context &context,
		const GridStoreChannel::SessionInfo &sessionInfo) {
	XArrayByteOutStream reqOut =
			channel.getRequestOutStream(context.getResponseBuffer());
	reqOut << sessionInfo.getContainerId();

	if (isInitialSessionRetrialEnabled()) {
		reqOut.writeAll(context.getSessionUUID(), sizeof(uuid_t));
	}
	NodeConnection::tryPutEmptyOptionalRequest(reqOut);

	channel.applyPartitionId(context, sessionInfo.getPartitionId());
	channel.executeStatement(context, Statement::CLOSE_SESSION,
			sessionInfo.getLastStatementId() + 1,
			context.getRequestBuffer(), context.getResponseBuffer());
}

void GSContainerTag::closeAllSessions(
		GridStoreChannel &channel,
		GridStoreChannel::Context &context,
		const GridStoreChannel::SessionInfo *sessionInfoList,
		size_t sessionInfoCount) {
	typedef GridStoreChannel::SessionInfo SessionInfo;
	std::vector<SessionInfo> sortedSessionInfos;

	for (size_t i = 0; i < sessionInfoCount; i++) {
		sortedSessionInfos.push_back(sessionInfoList[i]);
	}

	std::sort(sortedSessionInfos.begin(), sortedSessionInfos.end(), SessionSortFunc());

	const bool summarized = GSGridStore::isSessionUUIDSummarized();

	for (size_t start = 0; start < sortedSessionInfos.size();) {
		const int32_t partitionId = sortedSessionInfos[start].getPartitionId();
		size_t end = start;
		while (++end < sortedSessionInfos.size()) {
			const SessionInfo &info = sortedSessionInfos[end];
			if (info.getPartitionId() != partitionId) {
				break;
			}
		}

		try {
			XArrayByteOutStream reqOut =
					channel.getRequestOutStream(context.getRequestBuffer());
			if (summarized) {
				reqOut.writeAll(context.getSessionUUID(), sizeof(uuid_t));
			}
			NodeConnection::tryPutEmptyOptionalRequest(reqOut);

			reqOut << ClientUtil::sizeValueToInt32(end - start);
			for (size_t i = start; i < end; i++) {
				const SessionInfo &info = sortedSessionInfos[i];

				NodeConnection::putStatementId(
						reqOut, info.getLastStatementId() + 1);
				reqOut << info.getContainerId();
				reqOut << info.getSessionId();
				if (!summarized) {
					reqOut.writeAll(context.getSessionUUID(), sizeof(uuid_t));
				}
			}

			channel.applyPartitionId(context, partitionId);
			channel.executeStatement(
					context, Statement::CLOSE_MULTIPLE_SESSIONS,
					0, context.getRequestBuffer(), context.getResponseBuffer());
		}
		catch (...) {
		}
		start = end;
	}
}

void GSContainerTag::closeSubResources(bool silent, bool transactionalOnly) {
	assert(referenceCount_ > 0);

	for (ResourceSet::iterator it = activeResources_.begin();
			it != activeResources_.end(); ++it) {
		GSResourceType::Id type;
		if (!GSResourceHeader::getType(*it, type)) {
			continue;
		}

		try {
			if (type == GSResourceType::QUERY) {
				if (!transactionalOnly) {
					GSQuery *query = static_cast<GSQuery*>(*it);
					query->closeRemote();
				}
			}
			else if (type == GSResourceType::ROW_SET) {
				GSRowSet *rowSet = static_cast<GSRowSet*>(*it);
				rowSet->closeRemote(transactionalOnly);
			}
		}
		catch (...) {
			try {
				closeSubResources(true, transactionalOnly);
			}
			catch (...) {
			}

			if (!silent) {
				throw;
			}
		}
	}
}

void GSContainerTag::closeRemote() throw() {
	if (closed_) {
		return;
	}

	try {
		closeSubResources(false, false);
	}
	catch (...) {
	}

	try {
		closeSession(false);
	}
	catch (...) {
	}

	closed_ = true;
}

void GSContainerTag::disableCache() {
	if (cacheDisabled_) {
		return;
	}

	cacheDisabled_ = true;
	if (normalizedContainerKey_.get() != NULL) {
		ContainerCache *cache = getContext().getContainerCache();
		if (cache != NULL) {
			cache->removeSchema(*normalizedContainerKey_);
		}
	}
}

GSContainer::StatementFamily GSContainerTag::prepareQuerySession(
		const QueryParameters &parameters, bool forUpdate,
		bool neverCreate) {
	const bool forUpdateActual = parameters.isForUpdate(forUpdate);

	if (parameters.transactionIdSpecified_) {
		checkTransactionPreserved(
				forUpdateActual, parameters.initialTransactionId_,
				&parameters.initialTransactionStarted_, true);
	}

	StatementFamily baseFamily;
	if (forUpdateActual) {
		if (parameters.isPartialExecutionConfigured()) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION,
					"Partial execution not supported for update "
					"or on manual commit (forUpdate=" <<
					std::boolalpha << parameters.isForUpdate(forUpdate) <<
					", autoCommit=" << autoCommit_ << ")");
		}

		if (neverCreate && (!sessionPrepared_ || sessionId_ == 0)) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
		}
		baseFamily = STATEMENT_FAMILY_LOCK;
	}
	else {
		baseFamily = STATEMENT_FAMILY_QUERY;
	}

	return prepareSession(baseFamily);
}

GSContainer::QueryResultType GSContainerTag::resolveQueryResultType(
		int8_t rawType) {
	switch (rawType) {
	case RESULT_ROW_SET:
	case RESULT_AGGREGATION:
	case RESULT_QUERY_ANALYSIS:
	case RESULT_PARTIAL_FETCH_STATE:
	case RESULT_PARTIAL_EXECUTION_STATE:
		return static_cast<QueryResultType>(rawType);
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
	}
}

GSRowSetType GSContainerTag::resolveRowSetType(QueryResultType type) {
	switch (type) {
	case RESULT_ROW_SET:
		return GS_ROW_SET_CONTAINER_ROWS;
	case RESULT_AGGREGATION:
		return GS_ROW_SET_AGGREGATION_RESULT;
	case RESULT_QUERY_ANALYSIS:
		return GS_ROW_SET_QUERY_ANALYSIS;
	case RESULT_PARTIAL_FETCH_STATE:
		return GS_ROW_SET_CONTAINER_ROWS;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}
}

const RowMapper& GSContainerTag::getResultRowMapper(
		QueryResultType type) const {
	switch (type) {
	case RESULT_AGGREGATION:
		return RowMapper::getAggregationResultMapper();
	case RESULT_QUERY_ANALYSIS:
		return RowMapper::getQueryAnalysisMapper();
	default:
		return *mapper_.get();
	}
}

RowMapper::MappingMode GSContainerTag::getResultRowMappingMode(
		QueryResultType type) {
	if (type == RESULT_AGGREGATION) {
		return RowMapper::MODE_AGGREGATED;
	}

	return getRowMappingMode();
}

bool GSContainerTag::isResultRowIdIncluded(QueryResultType type) {
	if (mapper_->getCategory() == RowMapper::CATEGORY_TIME_SERIES) {
		return false;
	}
	else {
		switch (type) {
		case RESULT_AGGREGATION:
		case RESULT_QUERY_ANALYSIS:
			return false;
		default:
			return true;
		}
	}
}

void GSContainerTag::checkTransactionPreserved(
		bool forUpdate, int64_t transactionId, const bool *transactionStarted,
		bool updatable) {
	if (forUpdate && (transactionId == 0 || !updatable)) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_NOT_LOCKED,
				"Update option must be turned on");
	}

	if (transactionId == 0) {
		if (!autoCommit_) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_COMMIT_MODE,
					"Illegal operation for partial row set "
					"by auto commit query "
					"because of currently manual commit mode");
		}
	}
	else {
		if (transactionId != transactionId_ ||
				(transactionStarted != NULL &&
						(!(*transactionStarted)) != (!transactionStarted_)) ||
				autoCommit_) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_TRANSACTION_CLOSED,
					"Transaction expired");
		}
	}
}

bool GSContainerTag::filterIndexInfo(
		const GSIndexInfo &info, bool forCreation,
		GSIndexInfo &filteredInfo) const {
	filteredInfo = info;

	if (filteredInfo.name != NULL) {
		ClientUtil::checkSymbol(filteredInfo.name, "index name");
	}

	const int32_t column = filteredInfo.column;
	const GSChar *columnNameById = NULL;
	if (column >= 0) {
		const GSBinding &binding = mapper_->getBinding();
		if (static_cast<size_t>(column) >= binding.entryCount) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
					"Column number out of range (column=" << column << ")");
		}
		columnNameById = binding.entries[column].columnName;
	}

	const GSChar *columnName = filteredInfo.columnName;
	if (columnName == NULL) {
		columnName = columnNameById;
	}
	else {
		const int32_t columnByName = mapper_->resolveColumnId(columnName);
		if (column < 0) {
			filteredInfo.column = columnByName;
		}
		else if (column != columnByName) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
					"Inconsistent column specified ("
					"specifiedNumber=" << column <<
					" (actualName=" << columnNameById << "), "
					"specifiedName=" << columnName <<
					" (actualNumber=" << columnByName << "))");
		}
	}

	if (forCreation && filteredInfo.column < 0) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER,
					"Column must be specified");
	}

	const GSIndexTypeFlags type = filteredInfo.type;
	if (!GSGridStore::isIndexDetailEnabled() &&
			((forCreation && type == 0) || type == GS_INDEX_FLAG_DEFAULT)) {
		const int32_t filteredColumn = filteredInfo.column;
		const GSIndexTypeFlags defaultType = (filteredColumn < 0 ?
				0 : getDefaultIndexTypeFlag(filteredColumn));
		if (defaultType == 0) {
			if (!forCreation) {
				return false;
			}
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_DEFAULT_INDEX, "");
		}

		filteredInfo.type = type;
	}

	return true;
}

GSIndexTypeFlags GSContainerTag::getDefaultIndexTypeFlag(
		int32_t columnId) const {
	if (mapper_->isArray(columnId)) {
		return 0;
	}

	const bool forTimeSeries =
			(mapper_->getCategory() == RowMapper::CATEGORY_TIME_SERIES);
	switch (mapper_->getElementType(columnId)) {
	case GS_TYPE_TIMESTAMP:
		if (columnId == 0 && forTimeSeries) {
			return 0;
		}
		return GS_INDEX_FLAG_TREE;
	case GS_TYPE_GEOMETRY:
		if (forTimeSeries) {
			return 0;
		}
		return GS_INDEX_FLAG_SPATIAL;
	case GS_TYPE_BLOB:
		return 0;
	default:
		return GS_INDEX_FLAG_TREE;
	}
}

GSTimestamp GSContainerTag::wrapOptionalTimestamp(
		const GSTimestamp *timestamp) {
	const GSTimestamp undefTimestamp = std::numeric_limits<GSTimestamp>::max();
	if (timestamp == NULL) {
		return undefTimestamp;
	}

	if (*timestamp == undefTimestamp) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	return *timestamp;
}

bool GSContainerTag::isAnyQueryResultTypeEnabled() {
	return (NodeConnection::getProtocolVersion() >= 2 &&
			(!GridStoreChannel::v1ProtocolCompatibleSpecified_ ||
			!GridStoreChannel::v1ProtocolCompatible_));
}

bool GSContainerTag::getRowForInternalController(
		const GSType *keyType, const void *key, void *rowObj) {
	if (normalizedContainerKey_.get() == NULL) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	if ((keyType != NULL && *keyType != GS_TYPE_STRING) || key == NULL) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	const GSChar *nameString = *static_cast<const GSChar *const*>(key);
	if (nameString == NULL) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	GSRow &row = GSRow::resolve(rowObj);

	std::string valueString;
	if (strcmp("failoverCount", nameString) == 0) {
		valueString = util::LexicalConverter<std::string>()(
				getContext().getFailoverCount());
	}
	else {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	{
		GSValue value;
		value.asString = nameString;
		row.setField(0, value, GS_TYPE_STRING);
	}

	{
		GSValue value;
		value.asString = valueString.c_str();
		row.setField(1, value, GS_TYPE_STRING);
	}

	return true;
}

bool GSContainerTag::putRowForInternalController(
		const GSType *keyType, const void *key, const void *rowObj) {
	if (normalizedContainerKey_.get() == NULL) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	if ((keyType != NULL && *keyType != GS_TYPE_STRING) || key == NULL) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	const GSChar *nameString = *static_cast<const GSChar *const*>(key);
	if (nameString == NULL) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	const GSRow &row = GSRow::resolve(rowObj);

	const GSChar *valueString;
	{
		GSValue value;
		GSType type = GS_TYPE_STRING;
		row.getField(1, value, type, &type, getVarDataPool());
		valueString = value.asString;
	}

	if (strcmp("invalidateMaster", nameString) == 0) {
		const bool value = util::LexicalConverter<bool>()(valueString);
		if (value) {
			getChannel().invalidateMaster(getContext());
		}
		else {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
		}
	}
	else {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	return true;
}


GSContainerTag::PartialFetchStatus::PartialFetchStatus() :
		totalRowCount_(-1),
		rowSetId_(0),
		rowSetIdHint_(0) {
}

GSContainer::PartialFetchStatus GSContainerTag::PartialFetchStatus::get(
		ArrayByteInStream &in) {
	PartialFetchStatus status;

	in >> status.totalRowCount_;
	in >> status.rowSetId_;
	if (isRowSetIdHintDisabled()) {
		status.rowSetIdHint_ = 0;
	}
	else {
		in >> status.rowSetIdHint_;
	}

	return status;
}

bool GSContainerTag::PartialFetchStatus::isEnabled() const {
	return (totalRowCount_ >= 0);
}


const GSContainer::PartialExecutionStatus
GSContainerTag::PartialExecutionStatus::STATUS_DISABLED(false);

const GSContainer::PartialExecutionStatus
GSContainerTag::PartialExecutionStatus::STATUS_ENABLED_INITIAL(true);

GSContainerTag::PartialExecutionStatus::PartialExecutionStatus(bool enabled) :
		enabled_(enabled) {
}

GSContainerTag::PartialExecutionStatus::PartialExecutionStatus(
		const PartialExecutionStatus &another) :
		enabled_(false) {
	*this = another;
}

GSContainer::PartialExecutionStatus&
GSContainerTag::PartialExecutionStatus::operator=(
		const PartialExecutionStatus &another) {
	if (this != &another) {
		if (another.entryMap_.get() == NULL) {
			entryMap_.reset();
		}
		else {
			entryMap_.reset(new EntryMap(*another.entryMap_));
		}
		enabled_ = another.enabled_;
	}
	return *this;
}

GSContainer::PartialExecutionStatus
GSContainerTag::PartialExecutionStatus::get(ArrayByteInStream &in) {
	GSBool enabled;
	in >> enabled;
	PartialExecutionStatus status(ClientUtil::toBool(enabled));

	int32_t intCount;
	in >> intCount;
	const size_t count = ClientUtil::toSizeValue(intCount);
	if (count > 0) {
		status.entryMap_.reset(new EntryMap);
		for (size_t i = 0; i < count; i++) {
			int8_t type;
			in >> type;

			int32_t intSize;
			in >> intSize;
			const size_t size = ClientUtil::toSizeValue(intSize);
			if (size > in.base().remaining()) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
			}

			EntryBytes &bytes = (*status.entryMap_)[type];
			if (size > 0) {
				bytes.resize(size);
				in.base().readAll(&bytes[0], size);
			}
		}
	}

	return status;
}

void GSContainerTag::PartialExecutionStatus::put(
		XArrayByteOutStream &out) const {
	out << ClientUtil::toGSBool(enabled_);

	const size_t count = (entryMap_.get() == NULL ? 0 : entryMap_->size());
	out << ClientUtil::sizeValueToInt32(count);

	if (count == 0) {
		return;
	}

	for (EntryMap::const_iterator it = entryMap_->begin();
			it != entryMap_->end(); ++it) {
		out << static_cast<int8_t>(it->first);

		const EntryBytes &bytes = it->second;
		out << ClientUtil::sizeValueToInt32(bytes.size());
		if (!bytes.empty()) {
			out.base().writeAll(&bytes[0], bytes.size());
		}
	}
}

bool GSContainerTag::PartialExecutionStatus::isEnabled() const {
	return enabled_;
}

const GSContainer::PartialExecutionStatus&
GSContainerTag::PartialExecutionStatus::inherit(
		const PartialExecutionStatus &next) const {
	if (!next.isEnabled()) {
		return STATUS_DISABLED;
	}

	if (!isEnabled() ||
			next.entryMap_.get() == NULL || next.entryMap_->empty()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by unexpected partial execution");
	}

	do {
		if (entryMap_.get() == NULL ||
				entryMap_->size() != next.entryMap_->size()) {
			break;
		}

		bool progressed = false;
		for (EntryMap::const_iterator it = entryMap_->begin();
				it != entryMap_->end(); ++it) {
			const EntryBytes &prevBytes = it->second;

			EntryMap::const_iterator nextIt = next.entryMap_->find(it->first);
			if (nextIt == next.entryMap_->end() ||
					prevBytes != nextIt->second) {
				progressed = true;
				break;
			}
		}

		if (progressed) {
			break;
		}
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Protocol error by no progress on partial execution");
	}
	while (false);

	return next;
}


GSContainer::QueryFormatter::QueryFormatter(Statement::Id statement) :
		statement_(statement) {
}

Statement::Id GSContainer::QueryFormatter::getStatement() const {
	return statement_;
}

void GSContainer::QueryFormatter::getString(
		std::ostream &os, ArrayByteInStream &in) const {
	static_cast<void>(os);
	static_cast<void>(in);
}


GSContainer::QueryData::QueryData() {
}

GSContainer::QueryData::QueryData(const QueryData &another) :
		data_(another.data_.begin(), another.data_.end()) {
}

GSContainer::QueryData& GSContainer::QueryData::operator=(
		const QueryData &another) {
	if (this == &another) {
		return *this;
	}

	data_.assign(another.data_.begin(), another.data_.end());
	return *this;
}


const int64_t GSContainerTag::QueryParameters::DEFAULT_SIZE_OPTION_VALUE =
		std::numeric_limits<int64_t>::max();

GSContainerTag::QueryParameters::QueryParameters(
		const QueryFormatter &formatter) :
		formatter_(&formatter),
		fetchLimit_(DEFAULT_SIZE_OPTION_VALUE),
		fetchSize_(DEFAULT_SIZE_OPTION_VALUE),
		executionStatus_(PartialExecutionStatus::STATUS_DISABLED),
		executionPartial_(false),
		forUpdate_(false),
		transactionIdSpecified_(false),
		initialTransactionStarted_(false),
		initialTransactionId_(0) {
}

Statement::Id GSContainerTag::QueryParameters::getStatement() const {
	return formatter_->getStatement();
}

GSContainer::QueryParameters::StringFormatter
GSContainerTag::QueryParameters::toString() const {
	return StringFormatter(*this);
}

void GSContainerTag::QueryParameters::putFixed(
		XArrayByteOutStream &out) const {
	out << fetchLimit_;
	if (GSQuery::isFetchSizeEnabled()) {
		out << fetchSize_;
	}
	if (GSGridStore::isQueryOptionsExtensible()) {
		executionStatus_.put(out);
	}
}

void GSContainerTag::QueryParameters::setFetchLimit(int64_t fetchLimit) {
	fetchLimit_ = fetchLimit;
}

void GSContainerTag::QueryParameters::setFetchSize(int64_t fetchSize) {
	checkPartialOptions(fetchSize, executionStatus_);
	fetchSize_ = fetchSize;
}

void GSContainerTag::QueryParameters::setPartialExecutionEnabled(
		bool enabled) {
	const PartialExecutionStatus &executionStatus = (enabled ?
			PartialExecutionStatus::STATUS_ENABLED_INITIAL :
			PartialExecutionStatus::STATUS_DISABLED);

	checkPartialOptions(fetchSize_, executionStatus);
	executionStatus_ = executionStatus;
	executionPartial_ = enabled;
}

bool GSContainerTag::QueryParameters::isPartialExecutionConfigured() const {
	return executionPartial_;
}

void GSContainerTag::QueryParameters::checkPartialOptions(
		int64_t fetchSize, const PartialExecutionStatus &executionStatus) {
	if (isPartialFetch(fetchSize) && executionStatus.isEnabled()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
				"Partial fetch and partial execution "
				"cannot be enabled at the same time");
	}
}

bool GSContainerTag::QueryParameters::isPartialFetch(int64_t fetchSize) {
	return fetchSize != DEFAULT_SIZE_OPTION_VALUE;
}

bool GSContainerTag::QueryParameters::isForUpdate(bool forUpdate) const {
	return (forUpdate_ || forUpdate);
}

GSContainerTag::QueryParameters GSContainerTag::QueryParameters::inherit(
		bool forUpdate, int64_t transactionId, bool transactionStarted,
		const PartialExecutionStatus &executionStatus) const {
	QueryParameters dest = *this;

	dest.forUpdate_ = isForUpdate(forUpdate);

	if (transactionIdSpecified_ &&
			(initialTransactionId_ != transactionId ||
			(!initialTransactionStarted_) != (!transactionStarted))) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}
	dest.transactionIdSpecified_ = true;
	dest.initialTransactionId_ = transactionId;
	dest.initialTransactionStarted_ = transactionStarted;

	dest.executionStatus_ = dest.executionStatus_.inherit(executionStatus);

	return dest;
}


GSContainerTag::QueryParameters::StringFormatter::StringFormatter(
		const QueryParameters &parameters) :
		parameters_(parameters) {
}

void GSContainerTag::QueryParameters::StringFormatter::format(
		std::ostream &os) const {
	if (parameters_.formatter_ == NULL) {
		return;
	}

	const util::NormalXArray<uint8_t> &data = parameters_.queryData_.data_;
	ArrayByteInStream in((util::ArrayInStream(data.data(), data.size())));

	parameters_.formatter_->getString(os, in);
}

std::ostream& operator<<(
		std::ostream &os,
		const GSContainerTag::QueryParameters::StringFormatter &formatter) {
	formatter.format(os);
	return os;
}


GSQueryTag::GSQueryTag(
		GSContainer &container, const QueryFormatter &formatter) :
		resourceHeader_(GSResourceType::QUERY, &container, NULL),
		container_(&container),
		parameters_(formatter),
		lastRowSet_(NULL),
		lastRowSetVisible_(false),
		closed_(false) {
	container_->createReference(this);
}

GSQueryTag::~GSQueryTag() {
	GSContainer::removeReference(container_, this);
}

void GSQueryTag::close(GSQuery **query) throw() {
	if (query == NULL) {
		return;
	}

	if (GSResourceHeader::checkType(*query, GSResourceType::QUERY)) {
		(**query).clearLastRowSet();
		delete *query;
	}

	*query = NULL;
}

GSContainer* GSQueryTag::getContainer() {
	return container_;
}

Statement::Id GSQueryTag::getStatement() const {
	return parameters_.getStatement();
}

GSQuery::StringFormatter GSQueryTag::toString() const {
	return parameters_.toString();
}

XArrayByteOutStream GSQueryTag::getParametersOutStream() {
	util::NormalXArray<uint8_t> &data = parameters_.queryData_.data_;
	data.clear();
	return XArrayByteOutStream(NormalXArrayOutStream(data));
}

GSRowSet* GSQueryTag::fetch(bool forUpdate) {
	checkOpened();
	clearLastRowSet();

	lastRowSet_ = container_->queryAndFetch(parameters_, forUpdate);
	lastRowSet_->prepareFollowing();
	lastRowSet_->createReference();
	lastRowSetVisible_ = true;

	return lastRowSet_;
}

GSRowSet* GSQueryTag::getRowSet() {
	checkOpened();

	if (!lastRowSetVisible_) {
		return NULL;
	}

	GSRowSet *lastRowSet = lastRowSet_;
	lastRowSet->prepareFollowing();

	lastRowSet->createReference();
	lastRowSetVisible_ = false;

	return lastRowSet;
}

void GSQueryTag::setFetchOption(
		GSFetchOption option, const void *value, GSType valueType) {
	checkOpened();

	switch (option) {
	case GS_FETCH_LIMIT:
		parameters_.setFetchLimit(
				filterSizedFetchOption(option, value, valueType));
		break;
	case GS_FETCH_SIZE:
		parameters_.setFetchSize(
				filterSizedFetchOption(option, value, valueType));
		break;
	case GS_FETCH_PARTIAL_EXECUTION:
		parameters_.setPartialExecutionEnabled(
				ClientUtil::toBool(*filterFetchOption<GS_TYPE_BOOL>(
						option, value, valueType, true)));
		break;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNKNOWN_FETCH_OPTION, "");
	}
}

bool GSQueryTag::isFetchSizeEnabled() {
	return (NodeConnection::getProtocolVersion() >= 2 &&
			(!GridStoreChannel::v1ProtocolCompatibleSpecified_ ||
			!GridStoreChannel::v1ProtocolCompatible_));
}

void GSQueryTag::setForUpdate(bool forUpdate) {
	parameters_.forUpdate_ = forUpdate;
}

bool GSQueryTag::isForUpdate() const {
	return parameters_.forUpdate_;
}

void GSQueryTag::makeRequest(XArrayByteOutStream &reqOut, bool noUUID) {
	checkOpened();
	clearLastRowSet();

	container_->makeQueryRequest(parameters_, false, reqOut, noUUID);
}

void GSQueryTag::acceptResponse(
		ArrayByteInStream &respIn,
		const GridStoreChannel::ConnectionId &connectionId,
		bool bufSwapAllowed) {
	clearLastRowSet();

	lastRowSet_ =
			container_->acceptQueryResponse(parameters_, false, respIn, false);
	lastRowSetVisible_ = true;
}

void GSQueryTag::closeRemote() throw() {
	closed_ = true;
	clearLastRowSet();
}

void GSQueryTag::checkOpened() {
	if (GridStoreChannel::v10ResourceCompatible_) {
		return;
	}

	if (closed_ || container_->isClosed()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_RESOURCE_CLOSED, "");
	}
}

void GSQueryTag::clearLastRowSet() throw() {
	lastRowSetVisible_ = false;
	GSRowSet::close(&lastRowSet_);
}

int64_t GSQueryTag::filterSizedFetchOption(
		GSFetchOption option, const void *value, GSType valueType) {
	const GSType intType = GS_TYPE_INTEGER;
	const int32_t *intValue =
			filterFetchOption<intType>(option, value, valueType, false);

	int64_t longValue;
	if (intValue == NULL) {
		const GSType longType = GS_TYPE_LONG;
		longValue =
				*filterFetchOption<longType>(option, value, valueType, true);
	}
	else {
		longValue = *intValue;
	}

	if (longValue < 0) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	return longValue;
}

template<GSType T>
const typename RowMapper::ObjectTypeTraits<T, false>::Object*
GSQueryTag::filterFetchOption(
		GSFetchOption option, const void *value, GSType valueType,
		bool force) {
	static_cast<void>(option);

	if (T != valueType) {
		if (!force) {
			return NULL;
		}
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE, "");
	}

	typedef typename RowMapper::ObjectTypeTraits<T, false>::Object Object;
	return static_cast<const Object*>(value);
}


GSRowSetTag::GSRowSetTag(
		GSContainer &container, const RowMapper &mapper,
		int64_t rowCount, util::NormalXArray<uint8_t> &resultData,
		const ArrayByteInStream &resultInStream,
		RowMapper::MappingMode mappingMode, bool rowIdIncluded,
		GSRowSetType type, const QueryParameters &queryParameters,
		const PartialFetchStatus &fetchStatus,
		const GridStoreChannel::ConnectionId &connectionId,
		bool bufSwapAllowed) :
		resourceHeader_(GSResourceType::ROW_SET, &container, NULL),
		referenceCount_(1),
		container_(&container),
		mapper_(mapper),
		resultInStream_(swapAndCreateInput(
				resultData_, resultData, resultInStream, bufSwapAllowed)),
		varDataPool_(container.getVarDataPool()),
		cursor_(
				resultInStream_, mapper_, mappingMode,
				ClientUtil::sizeValueToInt32(ClientUtil::toSizeValue(rowCount)),
				rowIdIncluded, &varDataPool_),
		type_(type),
		lastKey_(NULL),
		totalRowCount_(
				(fetchStatus.isEnabled() ?
						fetchStatus.totalRowCount_ :
						(queryParameters.isPartialExecutionConfigured() ?
								-1 : rowCount))),
		remainingRowCount_(
				(fetchStatus.isEnabled() ?
						fetchStatus.totalRowCount_ - rowCount : 0)),
		queryParameters_(queryParameters),
		fetchStatus_(fetchStatus),
		connectionId_(connectionId),
		previousFound_(false),
		followingLost_(false),
		closed_(false) {
	container_->createReference(this);
}

GSRowSetTag::~GSRowSetTag() {
	GSContainer::removeReference(container_, this);
}

void GSRowSetTag::close(GSRowSet **rowSet) throw() {
	if (rowSet == NULL) {
		return;
	}

	if (GSResourceHeader::checkType(*rowSet, GSResourceType::ROW_SET)) {
		try {
			(*rowSet)->closeRemote(false);
		}
		catch (...) {
		}
		removeReference(*rowSet);
	}
	else {
		*rowSet = NULL;
	}
}

void GSRowSetTag::createReference() throw() {
	assert(referenceCount_ > 0);
	referenceCount_++;
}

void GSRowSetTag::removeReference(GSRowSet *&rowSet) throw() {
	if (rowSet == NULL) {
		return;
	}

	assert(rowSet->referenceCount_ > 0);
	if (--rowSet->referenceCount_ <= 0) {
		delete rowSet;
	}

	rowSet = NULL;
}

GSQuery::StringFormatter GSRowSetTag::toQueryString() const {
	return queryParameters_.toString();
}

int32_t GSRowSetTag::getSize() const {
	return static_cast<int32_t>(totalRowCount_);
}

GSRowSetType GSRowSetTag::getType() const {
	return type_;
}

bool GSRowSetTag::isPartial() const {
	return fetchStatus_.isEnabled();
}

bool GSRowSetTag::hasNext() const {
	return cursor_.hasNext();
}

GSContainer* GSRowSetTag::getContainer() const {
	return container_;
}

int64_t GSRowSetTag::getTransactionId() const {
	assert(queryParameters_.transactionIdSpecified_);
	return queryParameters_.initialTransactionId_;
}

bool GSRowSetTag::isTransactionStarted() const {
	assert(queryParameters_.transactionIdSpecified_);
	return queryParameters_.initialTransactionStarted_;
}

int64_t GSRowSetTag::getRowId() const {
	checkOpened();
	checkInRange();

	if (!cursor_.isRowIdIncluded()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	return cursor_.getLastRowID();
}

const void* GSRowSetTag::getRowKey() const {
	checkOpened();

	if (!cursor_.isInRange()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_NO_SUCH_ELEMENT, "");
	}

	if (lastKey_ == NULL) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	return lastKey_;
}

void GSRowSetTag::closeRemote(bool transactionalOnly) {
	if (closed_) {
		return;
	}

	if (transactionalOnly && queryParameters_.initialTransactionId_ == 0) {
		return;
	}

	closed_ = true;

	util::NormalXArray<uint8_t> emptyData;
	resultData_.swap(emptyData);

	if (isPartial() && !connectionId_.isEmpty()) {
		container_->closeRowSet(fetchStatus_, connectionId_);
	}
}

void GSRowSetTag::next(void *rowObj) {
	checkOpened();

	if (!cursor_.hasNext()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_NO_SUCH_ELEMENT, "");
	}

	if (getType() != GS_ROW_SET_CONTAINER_ROWS) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	try {
		varDataPool_.clear();
		mapper_.decode(cursor_, rowObj);
		lastKey_ = mapper_.extractKey(rowObj, lastKeyStorage_);

		if (!cursor_.hasNext()) {
			prepareFollowingDirect();
			previousFound_ = true;
		}
	}
	catch (...) {
		try {
			closeRemote(false);
		}
		catch (...) {
		}
		throw;
	}
}

void GSRowSetTag::nextAggregation(GSAggregationResult **aggregationResult) {
	checkOpened();

	if (!hasNext()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_NO_SUCH_ELEMENT, "");
	}

	if (getType() != GS_ROW_SET_AGGREGATION_RESULT) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	try {
		std::auto_ptr<GSAggregationResult> result(
				new GSAggregationResult(*container_));
		mapper_.decode(cursor_, result.get());
		*aggregationResult = result.release();
		previousFound_ = true;
	}
	catch (...) {
		try {
			closeRemote(false);
		}
		catch (...) {
		}
		throw;
	}
}

void GSRowSetTag::nextQueryAnalysis(GSQueryAnalysisEntry *queryAnalysis) {
	checkOpened();

	if (!hasNext()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_NO_SUCH_ELEMENT, "");
	}

	if (getType() != GS_ROW_SET_QUERY_ANALYSIS) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
	}

	try {
		varDataPool_.clear();
		mapper_.decode(cursor_, queryAnalysis);
		previousFound_ = true;
	}
	catch (...) {
		try {
			closeRemote(false);
		}
		catch (...) {
		}
		throw;
	}
}

void GSRowSetTag::remove() {
	checkOpened();
	checkInRange();

	const bool &transactionStarted = isTransactionStarted();
	container_->removeRow(
			mapper_, getTransactionId(), &transactionStarted,
			cursor_.getLastRowID(), lastKey_);
}

void GSRowSetTag::update(const void *rowObj) {
	checkOpened();
	checkInRange();

	const bool &transactionStarted = isTransactionStarted();
	container_->updateRow(
			mapper_, getTransactionId(), &transactionStarted,
			cursor_.getLastRowID(), lastKey_, rowObj);
}

void GSRowSetTag::getRowFixedPart(const uint8_t *&data, size_t &size) const {
	cursor_.getRowFixedPartAll(getType(), resultData_, data, size);
}

void GSRowSetTag::getRowVariablePart(
		const uint8_t *&data, size_t &size) const {
	cursor_.getRowVariablePartAll(getType(), resultData_, data, size);
}

void GSRowSetTag::prepareFollowing() {
	checkOpened();

	try {
		prepareFollowingDirect();
	}
	catch (...) {
		try {
			closeRemote(false);
		}
		catch (...) {
		}
		throw;
	}
}

void GSRowSetTag::prepareFollowingDirect() {
	if (!cursor_.hasNext()) {
		if (remainingRowCount_ > 0) {
			fetchFollowing();
		}
		else if (queryParameters_.executionStatus_.isEnabled()) {
			executeFollowing();
		}
	}
}

void GSRowSetTag::fetchFollowing() {
	assert(isPartial());

	do {
		if (followingLost_) {
			break;
		}

		if (connectionId_.isEmpty()) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_RESOURCE_CLOSED, "");
		}

		try {
			container_->fetchRowSet(
					remainingRowCount_, fetchStatus_, queryParameters_,
					mapper_, resultData_, resultInStream_, cursor_,
					connectionId_);
		}
		catch (...) {
			try {
				closeRemote(false);
			}
			catch (...) {
			}
			throw;
		}

		remainingRowCount_ -= cursor_.getRowCount();

		if (remainingRowCount_ > 0 && connectionId_.isEmpty()) {
			followingLost_ = true;
		}

		if (cursor_.hasNext()) {
			return;
		}
	}
	while (false);

	if (followingLost_) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_RECOVERABLE_ROW_SET_LOST,
				"Row set lost by modifications (remaining=" <<
						remainingRowCount_ << ")");
	}
}

void GSRowSetTag::executeFollowing() {
	do {
		std::auto_ptr<GSRowSet> rowSet(
				container_->queryAndFetch(queryParameters_, false));

		resultData_.swap(rowSet->resultData_);
		resultInStream_ = rowSet->resultInStream_;
		new (&cursor_) RowMapper::InputCursor(
				resultInStream_, mapper_,
				rowSet->cursor_.getMode(),
				rowSet->cursor_.getRowCount(),
				rowSet->cursor_.isRowIdIncluded(),
				&varDataPool_);

		queryParameters_ = rowSet->queryParameters_;
	}
	while (!cursor_.hasNext() &&
			queryParameters_.executionStatus_.isEnabled());
}

ArrayByteInStream GSRowSetTag::swapAndCreateInput(
		util::NormalXArray<uint8_t> &destBuf,
		util::NormalXArray<uint8_t> &srcBuf,
		const ArrayByteInStream &srcInStream,
		bool bufSwapAllowed) {
	if (bufSwapAllowed) {
		destBuf.swap(srcBuf);

		const size_t position = srcInStream.base().position();
		const size_t remaining = srcInStream.base().remaining();

		ArrayByteInStream destInStream = ArrayByteInStream(util::ArrayInStream(
				destBuf.data(),
				std::min(destBuf.size(), position + remaining)));
		destInStream.base().position(position);

		return destInStream;
	}
	else {
		const size_t remaining = srcInStream.base().remaining();
		destBuf.resize(remaining);

		ArrayByteInStream in = srcInStream;
		in.readAll(destBuf.data(), destBuf.size());

		return ArrayByteInStream(
				util::ArrayInStream(destBuf.data(), destBuf.size()));
	}
}

void GSRowSetTag::checkOpened() const {
	if (GridStoreChannel::v10ResourceCompatible_) {
		return;
	}

	if (closed_ || container_->isClosed()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_RESOURCE_CLOSED, "");
	}
}

void GSRowSetTag::checkInRange() const {
	if (!cursor_.isInRange() && !previousFound_) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_NO_SUCH_ELEMENT, "");
	}
}


GSAggregationResultTag::GSAggregationResultTag(GSContainer &container) :
		resourceHeader_(GSResourceType::AGGREGATION_RESULT, &container, NULL),
		container_(&container),
		resultType_(static_cast<GSType>(-1)) {
	container_->createReference(this);
}

GSAggregationResultTag::~GSAggregationResultTag() {
	GSContainer::removeReference(container_, this);
}

void GSAggregationResultTag::close(
		GSAggregationResult **aggregationResult) throw() {
	if (aggregationResult == NULL) {
		return;
	}

	if (GSResourceHeader::checkType(
			*aggregationResult, GSResourceType::AGGREGATION_RESULT)) {
		delete *aggregationResult;
	}

	*aggregationResult = NULL;
}

std::auto_ptr<GSAggregationResult>
GSAggregationResultTag::newStandaloneInstance() {
	return std::auto_ptr<GSAggregationResult>(new GSAggregationResult());
}

bool GSAggregationResultTag::getValue(void *value, GSType valueType) {
	switch (resultType_) {
	case GS_TYPE_LONG:
		switch (valueType) {
		case GS_TYPE_LONG:
			*static_cast<int64_t*>(value) = value_.longOrTimestampValue_;
			return true;
		case GS_TYPE_DOUBLE:
			*static_cast<double*>(value) =
					static_cast<double>(value_.longOrTimestampValue_);
			return true;
		default:
			break;
		}
		return false;

	case GS_TYPE_DOUBLE:
		switch (valueType) {
		case GS_TYPE_LONG:
			*static_cast<int64_t*>(value) =
					static_cast<int64_t>(value_.doubleValue_);
			return true;
		case GS_TYPE_DOUBLE:
			*static_cast<double*>(value) = value_.doubleValue_;
			return true;
		default:
			break;
		}
		return false;

	case GS_TYPE_TIMESTAMP:
		switch (valueType) {
		case GS_TYPE_TIMESTAMP:
			*static_cast<GSTimestamp*>(value) = value_.longOrTimestampValue_;
			return true;
		default:
			break;
		}
		return false;

	default:
		break;
	}

	switch (valueType) {
	case GS_TYPE_LONG:
		*static_cast<int64_t*>(value) = 0;
		break;
	case GS_TYPE_DOUBLE:
		*static_cast<double*>(value) = 0;
		break;
	case GS_TYPE_TIMESTAMP:
		*static_cast<GSTimestamp*>(value) = 0;
		break;
	default:
		break;
	}

	return false;
}

template<typename Traits>
GSResult GSAggregationResultTag::getValueTyped(
		GSAggregationResult *aggregationResult,
		typename Traits::Object *value, GSBool *assigned,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	typedef typename Traits::Object Object;
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT2(
			value, Object(), assigned, GS_FALSE);
	GSResourceHeader::clearLastError(aggregationResult);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(
			funcInfo, aggregationResult, value, assigned);

	if (value != NULL) {
		*value = Object();
	}

	if (assigned != NULL) {
		*assigned = GS_FALSE;
	}

	try {
		GS_CLIENT_CHECK_NOT_NULL(aggregationResult);
		GS_CLIENT_CHECK_NOT_NULL(value);

		UTIL_STATIC_ASSERT(!Traits::ARRAY_TYPE);

		if (aggregationResult->getValue(value, Traits::ELEMENT_TYPE) &&
				assigned != NULL) {
			*assigned = GS_TRUE;
		}
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(aggregationResult);
	}
	return GS_RESULT_OK;
}

GSAggregationResultTag::GSAggregationResultTag() :
		resourceHeader_(GSResourceType::AGGREGATION_RESULT, NULL, NULL),
		container_(NULL),
		resultType_(static_cast<GSType>(-1)) {
}


const size_t GSRowTag::DATA_OFFSET = sizeof(GSRow);

GSRow* GSRowTag::create(GSGridStore &store, RowMapper::Reference mapper) {
	return create(&store, mapper);
}

GSRow* GSRowTag::create(GSContainer &container, RowMapper::Reference mapper) {
	return create(&container, mapper);
}

GSRow* GSRowTag::create(RowMapper::VarDataPool &pool, RowMapper::Reference mapper) {
	GSRow *row = create(NULL, mapper);
	row->varDataPool_ = &pool;
	return row;
}

void GSRowTag::close(GSRow **row) throw() {
	if (row == NULL) {
		return;
	}

	if (GSResourceHeader::checkType(*row, GSResourceType::ROW)) {
		(*row)->~GSRowTag();
		delete[] reinterpret_cast<uint8_t*>(*row);
	}

	*row = NULL;
}

const GSRow& GSRowTag::resolve(const void *resource) {
	GSResourceType::Id type;
	if (!GSResourceHeader::getType(resource, type) ||
			type != GSResourceType::ROW) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	return *static_cast<const GSRow*>(resource);
}

GSRow& GSRowTag::resolve(void *resource) {
	GSResourceType::Id type;
	if (!GSResourceHeader::getType(resource, type) ||
			type != GSResourceType::ROW) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}

	return *static_cast<GSRow*>(resource);
}

GSBinding GSRowTag::createBinding(
		const RowMapper::ContainerInfoRef<true> &infoRef,
		std::vector<GSBindingEntry> &entryList, bool anyTypeAllowed) {
	const size_t columnCount = infoRef.getColumnCount();

	entryList.clear();
	entryList.reserve(columnCount);

	size_t lastOffset =
			GSRow::DATA_OFFSET + ClientUtil::calcNullsByteSize(columnCount);

	if (columnCount <= 0 || infoRef.getRawColumnInfoList() == NULL) {
		if (columnCount == 0 && anyTypeAllowed) {
			GSBinding dest;
			dest.entries = NULL;
			dest.entryCount = 0;
			return dest;
		}
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
	}

	for (size_t i = 0; i < columnCount; i++) {
		GSBindingEntry entry;
		const GSColumnInfo &columnInfo =
				infoRef.getColumnInfo(static_cast<int32_t>(i));

		bool arrayUsed;
		const GSType elementType =
				RowMapper::toElementType(columnInfo.type, arrayUsed);

		entry.columnName = columnInfo.name;
		entry.elementType = elementType;

		typedef RowMapper::TypeOptionMask TypeOptionMask;
		if ((columnInfo.options &
				~TypeOptionMask::MASK_GENERAL_SUPPORTED) != 0) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_SCHEMA, "");
		}

		entry.options = columnInfo.options;
		if (i == 0 && infoRef.isRowKeyAssigned()) {
			entry.options |= GS_TYPE_OPTION_KEY;
		}

		if (arrayUsed) {
			entry.offset = lastOffset;
			lastOffset += sizeof(void*);
			entry.arraySizeOffset = lastOffset;
			lastOffset += sizeof(size_t);
		}
		else {
			entry.offset = lastOffset;
			lastOffset += RowMapper::getFieldObjectMainPartSize(
					elementType, arrayUsed);
			entry.arraySizeOffset = static_cast<size_t>(-1);
		}
		entryList.push_back(entry);
	}
	GSBinding binding;
	binding.entries = &entryList[0];
	binding.entryCount = entryList.size();

	return binding;
}

const RowMapper& GSRowTag::getRowMapper() const {
	return *mapper_;
}

void GSRowTag::getContainerSchema(RowMapper::ContainerInfoRef<> &infoRef) {
	RowMapper::VarDataPool &varDataPool = getVarDataPool();
	varDataPool.clear();
	mapper_->getContainerSchema(infoRef, varDataPool);
}

GSResult GSRowTag::getContainerSchema(
		GSRow *row, GSContainerInfo *schemaInfo,
		const ClientVersion &version,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	RowMapper::ContainerInfoRef<>::clear(schemaInfo, version);

	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(row);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(funcInfo, row, schemaInfo);

	try {
		GS_CLIENT_CHECK_NOT_NULL(row);
		GS_CLIENT_CHECK_NOT_NULL(schemaInfo);

		RowMapper::ContainerInfoRef<> infoRef(schemaInfo, version);
		row->getContainerSchema(infoRef);
	}
	catch (...) {
		RowMapper::ContainerInfoRef<>::clear(schemaInfo, version);
		return GSResourceHeader::setCurrentException(row);
	}

	return GS_RESULT_OK;
}

template<typename Traits>
GSResult GSRowTag::getPrimitiveField(
		GSRow *row, int32_t columnId, typename Traits::Object *value,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(
			value, typename Traits::Object());
	GSResourceHeader::clearLastError(row);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(funcInfo, row, columnId, value);

	try {
		GS_CLIENT_CHECK_NOT_NULL(row);
		GS_CLIENT_CHECK_NOT_NULL(value);

		const GSBindingEntry &entry = row->getBindingEntry(columnId);
		const GSType expectedType = RowMapper::toFullType(
				Traits::ELEMENT_TYPE, Traits::ARRAY_TYPE);
		row->checkType(columnId, entry, &expectedType, false);

		GSValue generalValue;
		FieldGetter getter(generalValue, row->getVarDataPool());
		RowMapper::invokeTypedOperation(*row, getter, entry);
		*value = Traits::as(generalValue);
	}
	catch (...) {
		if (value != NULL) {
			*value = typename Traits::Object();
		}
		return GSResourceHeader::setCurrentException(row);
	}

	return GS_RESULT_OK;
}

template<typename Traits>
GSResult GSRowTag::getArrayField(
		GSRow *row, int32_t columnId, typename Traits::Object *value,
		size_t *arraySize,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT2(
			value, typename Traits::Object(), arraySize, size_t());
	GSResourceHeader::clearLastError(row);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(
			funcInfo, row, columnId, value, arraySize);

	try {
		GS_CLIENT_CHECK_NOT_NULL(row);
		GS_CLIENT_CHECK_NOT_NULL(value);
		GS_CLIENT_CHECK_NOT_NULL(arraySize);

		const GSBindingEntry &entry = row->getBindingEntry(columnId);
		const GSType expectedType = RowMapper::toFullType(
				Traits::ELEMENT_TYPE, Traits::ARRAY_TYPE);
		row->checkType(columnId, entry, &expectedType, false);

		GSValue generalValue;
		FieldGetter getter(generalValue, row->getVarDataPool());
		RowMapper::invokeTypedOperation(*row, getter, entry);

		*value = Traits::as(generalValue);
		*arraySize = Traits::arraySizeOf(generalValue);
	}
	catch (...) {
		if (value != NULL) {
			*value = typename Traits::Object();
		}
		if (arraySize != NULL) {
			*arraySize = size_t();
		}
		return GSResourceHeader::setCurrentException(row);
	}

	return GS_RESULT_OK;
}

template<typename Traits>
GSResult GSRowTag::setPrimitiveField(
		GSRow *row, int32_t columnId, typename Traits::Object value,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(row);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(funcInfo, row, columnId, value);

	try {
		GS_CLIENT_CHECK_NOT_NULL(row);

		const GSBindingEntry &entry = row->getBindingEntry(columnId);
		const GSType expectedType = RowMapper::toFullType(
				Traits::ELEMENT_TYPE, Traits::ARRAY_TYPE);
		row->checkType(columnId, entry, &expectedType, true);

		GSValue generalValue;
		Traits::as(generalValue) = value;

		FieldSetter setter(generalValue, expectedType);
		RowMapper::invokeTypedOperation(*row, setter, entry);

		row->setNullDirect(columnId, entry, false, false);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(row);
	}

	return GS_RESULT_OK;
}

template<typename Traits>
GSResult GSRowTag::setArrayField(
		GSRow *row, int32_t columnId, typename Traits::Object value,
		size_t arraySize,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(row);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(
			funcInfo, row, columnId, value, arraySize);

	try {
		GS_CLIENT_CHECK_NOT_NULL(row);

		const GSBindingEntry &entry = row->getBindingEntry(columnId);
		const GSType expectedType = RowMapper::toFullType(
				Traits::ELEMENT_TYPE, Traits::ARRAY_TYPE);
		row->checkType(columnId, entry, &expectedType, true);

		GSValue generalValue;
		Traits::as(generalValue) = value;
		Traits::arraySizeOf(generalValue) = arraySize;

		FieldSetter setter(generalValue, expectedType);
		RowMapper::invokeTypedOperation(*row, setter, entry);

		row->setNullDirect(columnId, entry, false, false);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(row);
	}

	return GS_RESULT_OK;
}

void GSRowTag::getField(int32_t columnId, GSValue &value, GSType &type,
		const GSType *expectedType) {
	getField(columnId, value, type, expectedType, getVarDataPool());
}

void GSRowTag::getField(int32_t columnId, GSValue &value, GSType &type,
		const GSType *expectedType, RowMapper::VarDataPool &pool) const {
	const GSBindingEntry &entry = getBindingEntry(columnId);
	type = checkType(columnId, entry, expectedType, false);

	if (type == RowMapper::ANY_NULL_TYPE) {
		return;
	}

	FieldGetter getter(value, pool);
	RowMapper::invokeTypedOperation(*this, getter, entry);
}

void GSRowTag::setField(
		int32_t columnId, const GSValue &value, GSType expectedType) {
	const GSBindingEntry &entry = getBindingEntry(columnId);
	checkType(columnId, entry, &expectedType, true);

	if (RowMapper::isNullable(entry) && !RowMapper::isAny(entry.elementType)) {
		if (expectedType == RowMapper::ANY_NULL_TYPE) {
			setNullDirect(columnId, entry, true, true);
			return;
		}
		else {
			setNullDirect(columnId, entry, false, false);
		}
	}

	FieldSetter setter(value, expectedType);
	RowMapper::invokeTypedOperation(*this, setter, entry);
}

void GSRowTag::clear(bool silent) {
	FieldClearer clearer;
	const GSBinding &binding = mapper_->getBinding();
	for (size_t i = 0; i < binding.entryCount; i++) {
		const GSBindingEntry &entry = binding.entries[i];
		try {
			RowMapper::invokeTypedOperation(*this, clearer, entry);
		}
		catch (...) {
			if (!silent) {
				try {
					clear(true);
				}
				catch (...) {
				}
				throw;
			}
		}
	}
}

void* GSRowTag::allocate(size_t size) {
	return new int8_t[size];
}

void GSRowTag::deallocate(void *varData) {
	delete[] static_cast<int8_t*>(varData);
}

GSRowTag::GSRowTag(
		void *parentResource, RowMapper::Reference mapper, size_t objectSize) :
		resourceHeader_(GSResourceType::ROW, parentResource, NULL),
		parentResource_(parentResource),
		mapper_(mapper),
		varDataPool_(NULL) {
	if (!mapper_->isGeneral()) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}
	assert(objectSize == mapper_->getGeneralRowSize());
	(void) objectSize;

	const GSBinding &binding = mapper_->getBinding();
	memset(getNulls(), 0, ClientUtil::calcNullsByteSize(binding.entryCount));

	FieldInitializer initializer;
	for (size_t i = 0; i < binding.entryCount; i++) {
		const GSBindingEntry &entry = binding.entries[i];
		RowMapper::invokeTypedOperation(*this, initializer, entry);

		if (RowMapper::isOptionInitialValueNull(entry.options)) {
			setNullDirect(static_cast<int32_t>(i), entry, true, false);
		}
	}

	if (parentResource_ == NULL) {
		return;
	}

	GSResourceType::Id type;
	if (!GSResourceHeader::getType(parentResource_, type)) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	switch (type) {
	case GSResourceType::GRID_STORE:
		static_cast<GSGridStore*>(parentResource_)->createReference(this);
		break;
	case GSResourceType::CONTAINER:
		static_cast<GSContainer*>(parentResource_)->createReference(this);
		break;
	default:
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}
}

GSRowTag::~GSRowTag() {
	try {
		clear(true);
	}
	catch (...) {
	}

	if (parentResource_ == NULL) {
		return;
	}

	GSResourceType::Id type;
	if (!GSResourceHeader::getType(parentResource_, type)) {
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	switch (type) {
	case GSResourceType::GRID_STORE: {
		GSGridStore *store = static_cast<GSGridStore*>(parentResource_);
		GSGridStore::removeReference(store, this);
		break;
	}
	case GSResourceType::CONTAINER: {
		GSContainer *container = static_cast<GSContainer*>(parentResource_);
		GSContainer::removeReference(container, this);
		break;
	}
	default:
		assert(false);
		break;
	}
}

GSRow* GSRow::create(void *parentResource, RowMapper::Reference mapper) {
	const size_t storageSize = mapper->getGeneralRowSize();
	uint8_t *storage = new uint8_t[storageSize];

	try {
		GSRow *row =
				new(storage) GSRow(parentResource, mapper, storageSize);
		assert(static_cast<void*>(row) == static_cast<void*>(storage));
		return row;
	}
	catch (...) {
		delete[] storage;
		throw;
	}
}

const GSBindingEntry& GSRowTag::getBindingEntry(int32_t columnId) const {
	const GSBinding &binding = mapper_->getBinding();
	if (columnId < 0 ||
			binding.entryCount <= static_cast<size_t>(columnId)) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER,
				"Column number out of bounds (" <<
				"columnNumber=" << columnId << ", " <<
				"columnCount=" << binding.entryCount << ")");
	}

	return binding.entries[columnId];
}

RowMapper::VarDataPool& GSRowTag::getVarDataPool() {
	if (parentResource_ == NULL) {
		return *varDataPool_;
	}

	GSResourceType::Id type;
	if (!GSResourceHeader::getType(parentResource_, type)) {
		assert(false);
	}

	switch (type) {
	case GSResourceType::GRID_STORE:
		return static_cast<GSGridStore*>(parentResource_)->getVarDataPool();
	case GSResourceType::CONTAINER:
		return static_cast<GSContainer*>(parentResource_)->getVarDataPool();
	default:
		assert(false);
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}
}

GSType GSRowTag::checkType(
		int32_t columnId, const GSBindingEntry &entry,
		const GSType *expectedType, bool overwriting) const {
	GSType actualType = RowMapper::toFullType(
			entry.elementType, RowMapper::isArrayColumn(entry));

	if (entry.elementType == RowMapper::ANY_NULL_TYPE) {
		actualType = getTypeGeneral(entry);
		if (!overwriting &&
				expectedType != NULL && *expectedType != actualType &&
				*expectedType != RowMapper::ANY_NULL_TYPE) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
		}
	}
	else {
		if (RowMapper::isNullable(entry)) {
			if (isNull(columnId)) {
				actualType = RowMapper::ANY_NULL_TYPE;
			}
			else {
				actualType = RowMapper::toNonNullable(actualType);
			}
		}

		if (expectedType != NULL && *expectedType != actualType) {
			const GSType columnType = RowMapper::toFullType(
					entry.elementType, RowMapper::isArrayColumn(entry));

			if ((RowMapper::isAny(columnType) || *expectedType !=
						RowMapper::toNonNullable(columnType)) &&
					(!RowMapper::isAny(*expectedType) ||
							!RowMapper::isNullable(entry))) {
				GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
			}
		}
	}

	return actualType;
}

GSType GSRowTag::getTypeGeneral(const GSBindingEntry &entry) const {
	return ClientUtil::getNonAlignedValue<int8_t>(data() + entry.offset);
}

GSBindingEntry GSRowTag::getEntryGeneral(const GSBindingEntry &src) const {
	return RowMapper::getEntryGeneral(src, getTypeGeneral(src));
}

bool GSRowTag::isNull(int32_t columnId) const {
	getBindingEntry(columnId);
	return isNullDirect(columnId);
}

void GSRowTag::setNull(int32_t columnId, bool nullValue) {
	const GSType expectedType = RowMapper::ANY_NULL_TYPE;

	const GSBindingEntry &entry = getBindingEntry(columnId);
	checkType(columnId, entry, &expectedType, true);

	setNullDirect(columnId, entry, nullValue, true);
}

bool GSRowTag::isNullDirect(int32_t columnId) const {
	assert((getBindingEntry(columnId), true));

	const uint8_t &nulls = getNulls()[columnId / CHAR_BIT];
	return (nulls & (1U << (columnId % CHAR_BIT))) != 0;
}

void GSRowTag::setNullDirect(
		int32_t columnId, const GSBindingEntry &entry, bool nullValue,
		bool withClear) {
	if (withClear) {
		FieldClearer clearer;
		RowMapper::invokeTypedOperation(*this, clearer, entry);
	}

	uint8_t &nulls = getNulls()[columnId / CHAR_BIT];
	if (nullValue) {
		nulls |= (1U << (columnId % CHAR_BIT));
	}
	else {
		nulls &= ~(1U << (columnId % CHAR_BIT));
	}
}

const uint8_t* GSRowTag::getNulls() const {
	return data() + DATA_OFFSET;
}

uint8_t* GSRowTag::getNulls() {
	return data() + DATA_OFFSET;
}

template<typename Traits, typename T>
void GSRowTag::deallocate(const GSBindingEntry&, T, const Traits&) {
	UTIL_STATIC_ASSERT(!Traits::ARRAY_TYPE);
	UTIL_STATIC_ASSERT(Traits::ELEMENT_TYPE != GS_TYPE_STRING);
	UTIL_STATIC_ASSERT(Traits::ELEMENT_TYPE != GS_TYPE_GEOMETRY);
	UTIL_STATIC_ASSERT(Traits::ELEMENT_TYPE != GS_TYPE_BLOB);
}

template<typename Traits, typename E>
void GSRowTag::deallocate(
		const GSBindingEntry&, const E *value, const Traits&) {
	UTIL_STATIC_ASSERT(!(Traits::ARRAY_TYPE &&
			Traits::ELEMENT_TYPE == GS_TYPE_STRING));

	try {
		deallocate(const_cast<E*>(value));
	}
	catch (...) {
		assert(false);
	}
}

template<typename Traits>
void GSRowTag::deallocate(const GSBindingEntry &entry,
		const GSChar *const *value, const Traits&) {
	const size_t arraySize = ClientUtil::getNonAlignedValue<size_t>(
			data() + entry.arraySizeOffset);
	for (size_t i = 0; i < arraySize; i++) {
		try {
			deallocate(const_cast<GSChar*>(value[i]));
		}
		catch (...) {
			assert(false);
		}
	}

	try {
		deallocate(const_cast<GSChar**>(value));
	}
	catch (...) {
		assert(false);
	}
}

template<typename Traits>
void GSRowTag::deallocate(
		const GSBindingEntry&, const GSBlob &value, const Traits&) {
	try {
		deallocate(const_cast<void*>(value.data));
	}
	catch (...) {
		assert(false);
	}
}

template<>
void GSRowTag::initializeArraySize<true>(const GSBindingEntry &entry) {
	ClientUtil::setNonAlignedValue(data() + entry.arraySizeOffset, size_t());
}

template<>
void GSRowTag::initializeArraySize<false>(const GSBindingEntry&) {
}

uint8_t* GSRowTag::data() {
	return reinterpret_cast<uint8_t*>(this);
}

const uint8_t* GSRowTag::data() const {
	return reinterpret_cast<const uint8_t*>(this);
}

template<typename Traits>
void GSRowTag::FieldClearer::operator()(
		GSRow &row, const GSBindingEntry &entry, const Traits&) {
	try {
		FieldDeallocator()(row, entry, Traits());
	}
	catch (...) {
		FieldInitializer()(row, entry, Traits());
		throw;
	}
	FieldInitializer()(row, entry, Traits());
}

GSRowTag::FieldGetter::FieldGetter(
		GSValue &value, RowMapper::VarDataPool &pool) :
		value_(value), pool_(pool) {
}

template<typename Traits>
void GSRowTag::FieldGetter::operator()(
		const GSRow &row, const GSBindingEntry &entry, const Traits&) {
	value_ = RowMapper::copyValue(DirectFieldGetter<Traits>()(row, entry),
			Traits(), &pool_, util::TrueType(), typename Traits::Object());
}

void GSRowTag::FieldGetter::operator()(
		const GSRow &row, const GSBindingEntry &entry,
		const RowMapper::AnyNullTraits&) {
	if (row.getTypeGeneral(entry) == RowMapper::ANY_NULL_TYPE) {
		return;
	}
	RowMapper::invokeTypedOperation(row, *this, row.getEntryGeneral(entry));
}

GSRowTag::FieldSetter::FieldSetter(const GSValue &value, GSType type) :
		value_(value),
		type_(type),
		deallocated_(false) {
}

template<typename Traits>
void GSRowTag::FieldSetter::operator()(
		GSRow &row, const GSBindingEntry &entry, const Traits&) {
	if (!deallocated_) {
		FieldDeallocator()(row, entry, Traits());
	}
	try {
		DirectFieldSetter<Traits>()(row, entry,
				RowMapper::copyValue(value_, Traits(), &row, util::FalseType(),
						typename Traits::Object()));
	}
	catch (...) {
		try {
			FieldInitializer()(row, entry, Traits());
		}
		catch (...) {
		}
		throw;
	}
}

void GSRowTag::FieldSetter::operator()(
		GSRow &row, const GSBindingEntry &entry,
		const RowMapper::AnyNullTraits&) {

	if (row.getTypeGeneral(entry) != RowMapper::ANY_NULL_TYPE) {
		FieldDeallocator deallocator;
		RowMapper::invokeTypedOperation(
				row, deallocator, row.getEntryGeneral(entry));
	}

	ClientUtil::setNonAlignedValue<int8_t>(
			row.data() + entry.offset, static_cast<int8_t>(type_));
	if (type_ != RowMapper::ANY_NULL_TYPE) {
		deallocated_ = true;
		RowMapper::invokeTypedOperation(
				row, *this, RowMapper::getEntryGeneral(entry, type_));
	}
}

template<typename Traits>
void GSRowTag::FieldDeallocator::operator()(
		GSRow &row, const GSBindingEntry &entry, const Traits&) {
	typedef typename Traits::Object Object;
	Object obj = ClientUtil::getNonAlignedValue<Object>(
			row.data() + entry.offset);
	row.deallocate(entry, obj, Traits());
}

void GSRowTag::FieldDeallocator::operator()(
		GSRow &row, const GSBindingEntry &entry,
		const RowMapper::AnyNullTraits&) {
	if (row.getTypeGeneral(entry) == RowMapper::ANY_NULL_TYPE) {
		return;
	}
	RowMapper::invokeTypedOperation(row, *this, row.getEntryGeneral(entry));
}

template<typename Traits>
void GSRowTag::FieldInitializer::operator()(
		GSRow &row, const GSBindingEntry &entry, const Traits&) {
	assert(entry.offset + row.mapper_->getFieldObjectMainPartSize(
			entry.elementType, RowMapper::isArrayColumn(entry)) <=
			row.mapper_->getGeneralRowSize());

	ClientUtil::setNonAlignedValue(
			row.data() + entry.offset, typename Traits::Object());
	row.initializeArraySize<Traits::ARRAY_TYPE>(entry);
}

void GSRowTag::FieldInitializer::operator()(
		GSRow &row, const GSBindingEntry &entry,
		const RowMapper::AnyNullTraits&) {
	ClientUtil::setNonAlignedValue<int8_t>(row.data() + entry.offset,
			static_cast<int8_t>(RowMapper::ANY_NULL_TYPE));
}

template<typename Traits>
struct GSRowTag::DirectFieldGetter<Traits, true> {
	GSValue operator()(const GSRow &row, const GSBindingEntry &entry) {
		GSValue value;
		Traits::as(value) = ClientUtil::getNonAlignedValue<
				typename Traits::Object>(row.data() + entry.offset);
		Traits::arraySizeOf(value) = ClientUtil::getNonAlignedValue<size_t>(
				row.data() + entry.arraySizeOffset);
		return value;
	}
};

template<typename Traits>
struct GSRowTag::DirectFieldGetter<Traits, false> {
	GSValue operator()(const GSRow &row, const GSBindingEntry &entry) {
		GSValue value;
		Traits::as(value) = ClientUtil::getNonAlignedValue<
				typename Traits::Object>(row.data() + entry.offset);
		return value;
	}
};

template<typename Traits>
struct GSRowTag::DirectFieldSetter<Traits, true> {
	void operator()(GSRow &row, const GSBindingEntry &entry,
			const GSValue &value) {
		assert(entry.offset + row.mapper_->getFieldObjectMainPartSize(
				entry.elementType, RowMapper::isArrayColumn(entry)) <=
				row.mapper_->getGeneralRowSize());

		ClientUtil::setNonAlignedValue<typename Traits::Object>(
				row.data() + entry.offset, Traits::as(value));
		ClientUtil::setNonAlignedValue<size_t>(
				row.data() + entry.arraySizeOffset, Traits::arraySizeOf(value));
	}
};

template<typename Traits>
struct GSRowTag::DirectFieldSetter<Traits, false> {
	void operator()(GSRow &row, const GSBindingEntry &entry,
			const GSValue &value) {
		assert(entry.offset + row.mapper_->getFieldObjectMainPartSize(
				entry.elementType, RowMapper::isArrayColumn(entry)) <=
				row.mapper_->getGeneralRowSize());

		ClientUtil::setNonAlignedValue<typename Traits::Object>(
				row.data() + entry.offset, Traits::as(value));
	}
};


GSRowKeyPredicateTag::GSRowKeyPredicateTag(
		GSGridStore &store, GSType keyType) :
		resourceHeader_(GSResourceType::ROW_KEY_PREDICATE, &store, NULL),
		store_(&store),
		keyType_(keyType) {
	if (!RowMapper::isKeyType(keyType)) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_KEY_TYPE, "");
	}
	store_->createReference(this);
}

GSRowKeyPredicateTag::~GSRowKeyPredicateTag() {
	GSGridStore::removeReference(store_, this);
	try {
		clear();
	}
	catch (...) {
	}
}

void GSRowKeyPredicateTag::close(GSRowKeyPredicate **predicate) throw() {
	if (predicate == NULL) {
		return;
	}

	if (GSResourceHeader::checkType(
			*predicate, GSResourceType::ROW_KEY_PREDICATE)) {
		delete *predicate;
	}

	*predicate = NULL;
}

template<typename Traits, GSRowKeyPredicate::RangeElementType RangeType>
GSResult GSRowKeyPredicateTag::getRangeKey(
		GSRowKeyPredicate *predicate, const typename Traits::Object **key,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(key, NULL);
	GSResourceHeader::clearLastError(predicate);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(funcInfo, predicate, key);

	try {
		GS_CLIENT_CHECK_NOT_NULL(predicate);
		GS_CLIENT_CHECK_NOT_NULL(key);

		UTIL_STATIC_ASSERT(Traits::KEY_TYPE);
		const GSValue *value;
		predicate->getRangeKey<RangeType>(value, Traits::resolveKeyType());
		if (value == NULL) {
			*key = NULL;
		}
		else {
			*key = &Traits::as(*value);
		}
	}
	catch (...) {
		if (key != NULL) {
			*key = NULL;
		}
		return GSResourceHeader::setCurrentException(predicate);
	}

	return GS_RESULT_OK;
}

template<typename Traits, GSRowKeyPredicate::RangeElementType RangeType>
GSResult GSRowKeyPredicateTag::setRangeKey(
		GSRowKeyPredicate *predicate, const typename Traits::Object *key,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(predicate);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(funcInfo, predicate, key);

	try {
		GS_CLIENT_CHECK_NOT_NULL(predicate);

		if (key == NULL) {
			predicate->setRangeKey<RangeType>(NULL, Traits::ELEMENT_TYPE);
		}
		else {
			GSValue value;
			Traits::as(value) = *key;
			predicate->setRangeKey<RangeType>(&value, Traits::ELEMENT_TYPE);
		}
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(predicate);
	}

	return GS_RESULT_OK;
}

template<typename Traits>
GSResult GSRowKeyPredicateTag::getDistinctKeys(
		GSRowKeyPredicate *predicate,
		const typename Traits::Object **keyList, size_t *size,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT2(keyList, NULL, size, 0);
	GSResourceHeader::clearLastError(predicate);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(funcInfo, predicate, keyList, size);

	try {
		GS_CLIENT_CHECK_NOT_NULL(predicate);
		GS_CLIENT_CHECK_NOT_NULL(keyList);
		GS_CLIENT_CHECK_NOT_NULL(size);

		UTIL_STATIC_ASSERT(Traits::KEY_TYPE);
		TypedDistinctKeyReceiver<Traits> receiver;
		predicate->getDistinctKeys(
				*keyList, *size, Traits::resolveKeyType(), receiver);
	}
	catch (...) {
		if (keyList != NULL) {
			*keyList = NULL;
		}
		if (size != NULL) {
			*size = 0;
		}
		return GSResourceHeader::setCurrentException(predicate);
	}

	return GS_RESULT_OK;
}

template<typename Traits>
GSResult GSRowKeyPredicateTag::addDistinctKey(
		GSRowKeyPredicate *predicate, typename Traits::Object key,
		const GSInterceptor::FunctionInfo &funcInfo) throw() {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(predicate);
	GS_CLIENT_INTERCEPT_API_CALL_WITH_FUNC(funcInfo, predicate, key);

	try {
		GS_CLIENT_CHECK_NOT_NULL(predicate);

		GSValue value;
		Traits::as(value) = key;
		predicate->addDistinctKey(value, Traits::ELEMENT_TYPE);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(predicate);
	}

	return GS_RESULT_OK;
}

GSType GSRowKeyPredicateTag::getKeyType() const {
	return keyType_;
}

template<GSRowKeyPredicate::RangeElementType RangeType>
void GSRowKeyPredicateTag::getRangeKey(
		const GSValue *&key, const GSType *expectedType) const {
	RowMapper::VarDataPool &pool = store_->getVarDataPool();
	pool.clear();

	checkKeyType(expectedType);

	const RangeKeyEntry &rangeKeyEntry = rangeKeyEntries_[RangeType];
	if (!rangeKeyEntry.second) {
		key = NULL;
		return;
	}

	GSValue *keyPtr = static_cast<GSValue*>(pool.allocate(sizeof(GSValue)));
	*keyPtr = RowMapper::copyValue(
			rangeKeyEntry.first, keyType_, false, &pool, util::FalseType());
	key = keyPtr;
}

template<GSRowKeyPredicate::RangeElementType RangeType>
void GSRowKeyPredicateTag::setRangeKey(
		const GSValue *key, GSType expectedType) {
	checkKeyType(&expectedType);

	RangeKeyEntry &rangeKeyEntry = rangeKeyEntries_[RangeType];
	clearRangeKeyEntry(rangeKeyEntry);

	if (key == NULL) {
		return;
	}

	rangeKeyEntry.first = RowMapper::copyValue(
			*key, keyType_, false, this, util::FalseType());
	rangeKeyEntry.second = true;
}

template<typename T, typename Receiver>
void GSRowKeyPredicateTag::getDistinctKeys(
		const T *&keyList, size_t &size, const GSType *expectedType,
		Receiver &receiver) const {
	RowMapper::VarDataPool &pool = store_->getVarDataPool();
	pool.clear();

	checkKeyType(expectedType);

	if (distinctKeys_.get() == NULL || distinctKeys_->empty()) {
		keyList = NULL;
		size = 0;
		return;
	}

	const size_t distinctKeyCount = distinctKeys_->size();
	T *distinctKeyList =
			static_cast<T*>(pool.allocate(sizeof(T) * distinctKeyCount));

	T *destIt = distinctKeyList;
	for (DistinctKeySet::iterator it = distinctKeys_->begin();
			it != distinctKeys_->end(); ++it, ++destIt) {
		*destIt = receiver(RowMapper::copyValue(
				*it, keyType_, false, &pool, util::FalseType()));
	}

	assert(static_cast<size_t>(destIt - distinctKeyList) == distinctKeyCount);
	keyList = distinctKeyList;
	size = distinctKeyCount;
}

void GSRowKeyPredicateTag::addDistinctKey(
		const GSValue &key, GSType expectedType) {
	checkKeyType(&expectedType);

	GSValue destKey = RowMapper::copyValue(
			key, keyType_, false, this, util::FalseType());

	try {
		if (distinctKeys_.get() == NULL) {
			distinctKeys_.reset(new DistinctKeySet(KeyLess(keyType_)));
		}
		distinctKeys_->insert(destKey);
	}
	catch (...) {
		try {
			clearKey(destKey);
		}
		catch (...) {
		}
		throw;
	}
}

void* GSRowKeyPredicateTag::allocate(size_t size) {
	return new int8_t[size];
}

void GSRowKeyPredicateTag::deallocate(void *varData) {
	delete[] static_cast<int8_t*>(varData);
}

GSRowKeyPredicateTag::KeyLess::KeyLess(GSType keyType) :
		keyType_(keyType) {
}

bool GSRowKeyPredicateTag::KeyLess::operator()(
		const GSValue &key1, const GSValue &key2) const {
	switch (keyType_) {
	case GS_TYPE_STRING: {
		return strcmp(key1.asString, key2.asString) < 0;
	}
	case GS_TYPE_INTEGER:
		return key1.asInteger < key2.asInteger;
	case GS_TYPE_LONG:
		return key1.asLong < key2.asLong;
	case GS_TYPE_TIMESTAMP:
		return key1.asTimestamp < key2.asTimestamp;
	default:
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}
}

void GSRowKeyPredicateTag::checkKeyType(const GSType *expectedType) const {
	if (expectedType == NULL) {
		return;
	}

	if (*expectedType != keyType_) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}
}

void GSRowKeyPredicateTag::clear() {
	for (size_t i = 0; i < GSRowKeyPredicate::RANGE_COUNT; i++) {
		clearRangeKeyEntry(rangeKeyEntries_[i]);
	}

	if (distinctKeys_.get() != NULL) {
		for (;;) {
			DistinctKeySet::iterator it = distinctKeys_->begin();
			if (it == distinctKeys_->end()) {
				break;
			}

			GSValue value = *it;
			distinctKeys_->erase(it);
			clearKey(value);
		}
		distinctKeys_.reset();
	}
}

void GSRowKeyPredicateTag::clearRangeKeyEntry(RangeKeyEntry &keyEntry) {
	if (keyEntry.second) {
		clearKey(keyEntry.first);
		keyEntry.second = false;
	}
}

void GSRowKeyPredicateTag::clearKey(GSValue &value) {
	if (keyType_ != GS_TYPE_STRING) {
		return;
	}

	deallocate(const_cast<GSChar*>(value.asString));
	value.asString = NULL;
}


GSPartitionControllerTag::GSPartitionControllerTag(GSGridStore &store) :
		resourceHeader_(GSResourceType::PARTITION_CONTROLLER, &store, NULL),
		store_(&store) {
	store_->createReference(this);
}

GSPartitionControllerTag::~GSPartitionControllerTag() {
	GSGridStore::removeReference(store_, this);
}

void GSPartitionControllerTag::close(GSPartitionController **controller) throw() {
	if (controller == NULL) {
		return;
	}

	if (GSResourceHeader::checkType(
			*controller, GSResourceType::PARTITION_CONTROLLER)) {
		delete *controller;
	}

	*controller = NULL;
}

int32_t GSPartitionControllerTag::getPartitionCount() {
	checkOpened();
	return store_->getChannel().getPartitionCount(store_->getContext());
}

int64_t GSPartitionControllerTag::getContainerCount(int32_t partitionIndex) {
	checkOpened();
	checkPartitionIndex(partitionIndex);

	GridStoreChannel &channel = store_->getChannel();
	GridStoreChannel::Context &context = store_->getContext();

	XArrayByteOutStream reqOut = channel.getRequestOutStream(
			context.getRequestBuffer());
	NodeConnection::tryPutEmptyOptionalRequest(reqOut);

	const int64_t start = 0;
	const int64_t limit = 0;

	reqOut << start;
	reqOut << limit;

	channel.applyPartitionId(context, partitionIndex);
	ArrayByteInStream respIn = channel.executeStatement(
			context, Statement::GET_PARTITION_CONTAINER_NAMES,
			0, context.getRequestBuffer(), context.getResponseBuffer());

	int64_t totalCount;
	respIn >> totalCount;
	if (totalCount < 0) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED,
				"Negative result by protocol error");
	}

	return totalCount;
}

void GSPartitionControllerTag::getContainerNames(int32_t partitionIndex,
		int64_t start, const int64_t *limit,
		const GSChar *const *&nameList, size_t &size) {
	checkOpened();
	checkPartitionIndex(partitionIndex);

	RowMapper::VarDataPool &varDataPool = store_->getVarDataPool();
	varDataPool.clear();

	GridStoreChannel &channel = store_->getChannel();
	GridStoreChannel::Context &context = store_->getContext();

	XArrayByteOutStream reqOut = channel.getRequestOutStream(
			context.getRequestBuffer());
	NodeConnection::tryPutEmptyOptionalRequest(reqOut);

	reqOut << start;
	reqOut << (limit == NULL ? std::numeric_limits<int64_t>::max() : *limit);

	channel.applyPartitionId(context, partitionIndex);
	ArrayByteInStream respIn = channel.executeStatement(
			context, Statement::GET_PARTITION_CONTAINER_NAMES,
			0, context.getRequestBuffer(), context.getResponseBuffer());

	const ContainerKeyConverter &keyConverter = context.getKeyConverter();

	int64_t totalCount;
	respIn >> totalCount;

	int32_t entryCount;
	respIn >> entryCount;
	if (entryCount < 0 ||
			static_cast<size_t>(entryCount) > respIn.base().remaining()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
	}

	const GSChar **nameListBase;
	if (entryCount > 0) {
		nameListBase =
				static_cast<const GSChar**>(varDataPool.allocate(
						sizeof(const GSChar*) * entryCount));
		for (int32_t i = 0; i < entryCount; i++) {
			const ContainerKey &key = keyConverter.get(respIn, false);
			nameListBase[i] = RowMapper::copyString(
					keyConverter.format(key).c_str(), varDataPool);
		}
	}
	else {
		nameListBase = NULL;
	}

	nameList = nameListBase;
	size = static_cast<size_t>(entryCount);
}

void GSPartitionControllerTag::getHosts(int32_t partitionIndex,
		const GSChar *const *&addressList, size_t &size) {
	checkOpened();
	checkPartitionIndex(partitionIndex);

	RowMapper::VarDataPool &varDataPool = store_->getVarDataPool();
	varDataPool.clear();

	GridStoreChannel &channel = store_->getChannel();
	GridStoreChannel::Context &context = store_->getContext();

	std::vector<util::SocketAddress> baseAddressList;
	channel.getNodeAddressList(context, partitionIndex, baseAddressList);

	addressList = toAddressList(baseAddressList, size, varDataPool);
}

void GSPartitionControllerTag::getOwnerHost(
		int32_t partitionIndex, const GSChar *&address) {
	checkOpened();
	checkPartitionIndex(partitionIndex);

	RowMapper::VarDataPool &varDataPool = store_->getVarDataPool();
	varDataPool.clear();

	GridStoreChannel &channel = store_->getChannel();
	GridStoreChannel::Context &context = store_->getContext();

	std::vector<util::SocketAddress> baseAddressList;
	channel.getNodeAddressList(context, partitionIndex, baseAddressList);
	while (baseAddressList.size() > 1) {
		baseAddressList.pop_back();
	}

	size_t size;
	const GSChar *const *addressList =
			toAddressList(baseAddressList, size, varDataPool);

	if (size > 0) {
		address = *addressList;
	}
	else {
		address = NULL;
	}
}

void GSPartitionControllerTag::getBackupHosts(int32_t partitionIndex,
		const GSChar *const *&addressList, size_t &size) {
	checkOpened();
	checkPartitionIndex(partitionIndex);

	RowMapper::VarDataPool &varDataPool = store_->getVarDataPool();
	varDataPool.clear();

	GridStoreChannel &channel = store_->getChannel();
	GridStoreChannel::Context &context = store_->getContext();

	std::vector<util::SocketAddress> baseAddressList;
	channel.getNodeAddressList(context, partitionIndex, baseAddressList);
	if (!baseAddressList.empty()) {
		baseAddressList.front().clear();
	}

	addressList = toAddressList(baseAddressList, size, varDataPool);
}

int32_t GSPartitionControllerTag::getPartitionIndexOfContainer(
		const GSChar *containerName) {
	GridStoreChannel &channel = store_->getChannel();
	GridStoreChannel::Context &context = store_->getContext();

	const ContainerKeyConverter &keyConverter = context.getKeyConverter();
	const ContainerKey &containerKey =
			keyConverter.parse(containerName, false);

	channel.checkContextAvailable(context);
	return store_->getChannel().resolvePartitionId(
			context, containerKey, keyConverter);
}

void GSPartitionControllerTag::assignPreferableHost(
		int32_t partitionIndex, const GSChar *host) {
	checkOpened();
	checkPartitionIndex(partitionIndex);

	util::SocketAddress sockAddress;
	util::SocketAddress *sockAddressPtr;
	if (host == NULL) {
		sockAddressPtr = NULL;
	}
	else {
		try {
			sockAddress.assign(host, static_cast<uint16_t>(0));
		}
		catch (std::exception &e) {
			GS_CLIENT_RETHROW_ERROR(e, "");
		}
		sockAddressPtr = &sockAddress;
	}

	GridStoreChannel::Context &context = store_->getContext();
	context.setPreferableHost(partitionIndex, sockAddressPtr);
}

void GSPartitionControllerTag::checkOpened() {
	store_->getChannel().checkContextAvailable(store_->getContext());
}

void GSPartitionControllerTag::checkPartitionIndex(int32_t partitionIndex) {
	if (partitionIndex < 0 ||
			partitionIndex >= store_->getChannel().getPartitionCount(
					store_->getContext())) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
	}
}

const GSChar *const* GSPartitionControllerTag::toAddressList(
		const std::vector<util::SocketAddress> &baseAddressList,
		size_t &resultListSize, RowMapper::VarDataPool &varDataPool) {

	std::set<std::string> hostSet;
	for (size_t i = 0; i < baseAddressList.size(); i++) {
		if (!baseAddressList[i].isEmpty()) {
			std::string host;
			baseAddressList[i].getIP(&host);
			hostSet.insert(host);
		}
	}

	const GSChar **resultList;
	if (hostSet.empty()) {
		resultList = NULL;
	}
	else {
		resultList = static_cast<const GSChar**>(varDataPool.allocate(
				sizeof(const GSChar*) * hostSet.size()));

		const GSChar **destIt = resultList;
		for (std::set<std::string>::iterator it = hostSet.begin();
				it != hostSet.end(); ++it, ++destIt) {
			const std::string &host = *it;
			GSChar *hostStr = static_cast<GSChar*>(
					varDataPool.allocate(host.size() + 1));
			memcpy(hostStr, host.c_str(), host.size() + 1);
			*destIt = hostStr;
		}
	}

	resultListSize = hostSet.size();
	return resultList;
}

#ifdef GS_CLIENT_UNIT_TEST
namespace client_unit_test {
#endif


void GS_API_CALL gsCloseFactory(
		GSGridStoreFactory **factory, GSBool allRelated) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_INIT(factory, NULL);
	GSGridStoreFactory::close(factory, ClientUtil::toBool(allRelated));
}

GSGridStoreFactory* GS_API_CALL gsGetDefaultFactory() {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(NULL);
	return &GSGridStoreFactory::getDefaultFactory();
}

GSResult GS_API_CALL gsCompatibleFunc_GetGridStore1(
		GSGridStoreFactory *factory,
		const GSPropertyEntry *properties, GSGridStore **store) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(store, NULL);
	GSResourceHeader::clearLastError(factory);
	GS_CLIENT_INTERCEPT_API_CALL(factory, properties, store);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);

		if (factory == NULL) {
			factory = gsGetDefaultFactory();
		}
		if (properties == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}
		*store = factory->getGridStore(properties, NULL);
	}
	catch (...) {
		if (store != NULL) {
			*store = NULL;
		}
		return GSResourceHeader::setCurrentException(factory);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetGridStore(
		GSGridStoreFactory *factory,
		const GSPropertyEntry *properties, size_t propertyCount,
		GSGridStore **store) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(store, NULL);
	GSResourceHeader::clearLastError(factory);
	GS_CLIENT_INTERCEPT_API_CALL(factory, properties, propertyCount, store);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);

		if (factory == NULL) {
			factory = gsGetDefaultFactory();
		}
		if (properties == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}
		*store = factory->getGridStore(properties, &propertyCount);
	}
	catch (...) {
		if (store != NULL) {
			*store = NULL;
		}
		return GSResourceHeader::setCurrentException(factory);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsCompatibleFunc_SetFactoryProperties1(
		GSGridStoreFactory *factory, const GSPropertyEntry *properties) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(factory);
	GS_CLIENT_INTERCEPT_API_CALL(factory, properties);

	try {
		if (factory == NULL) {
			factory = gsGetDefaultFactory();
		}
		if (properties == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}
		factory->setProperties(properties, NULL);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(factory);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsSetFactoryProperties(
		GSGridStoreFactory *factory, const GSPropertyEntry *properties,
		size_t propertyCount) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(factory);
	GS_CLIENT_INTERCEPT_API_CALL(factory, properties, propertyCount);

	try {
		if (factory == NULL) {
			factory = gsGetDefaultFactory();
		}
		if (properties == NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_EMPTY_PARAMETER, "");
		}
		factory->setProperties(properties, &propertyCount);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(factory);
	}
	return GS_RESULT_OK;
}


void GS_API_CALL gsCloseGridStore(
		GSGridStore **store, GSBool allRelated) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_INIT(store, NULL);
	GSGridStore::close(store, ClientUtil::toBool(allRelated));
}

GSResult GS_API_CALL gsDropCollection(
		GSGridStore *store, const GSChar *name) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, name);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(name);

		store->dropCollection(name);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsDropTimeSeries(
		GSGridStore *store, const GSChar *name) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, name);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(name);

		store->dropTimeSeries(name);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetRowByPath(
		GSGridStore *store, const GSChar *pathKey, void *rowObj,
		GSBool *exists) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(exists, GS_FALSE);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, pathKey, rowObj, exists);

	GSBool existsStorage;
	if (exists == NULL) {
		exists = &existsStorage;
	}

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(pathKey);
		GS_CLIENT_CHECK_NOT_NULL(rowObj);

		*exists = store->getRow(pathKey, rowObj);
	}
	catch (...) {
		*exists = GS_FALSE;
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetCollection(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, GSCollection **collection) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(collection, NULL);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, name, binding, collection);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(name);
		GS_CLIENT_CHECK_NOT_NULL(binding);
		GS_CLIENT_CHECK_NOT_NULL(collection);

		*collection = store->getCollection(name, *binding);
	}
	catch (...) {
		if (collection != NULL) {
			*collection = NULL;
		}
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetContainerInfo(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists) {
	return GSGridStore::getContainerInfo(
			store, name, info, exists, ClientVersion(1, 0),
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsGetContainerInfoV1_5(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists) {
	return GSGridStore::getContainerInfo(
			store, name, info, exists, ClientVersion(1, 5),
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsGetContainerInfoV2_1(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists) {
	return GSGridStore::getContainerInfo(
			store, name, info, exists, ClientVersion(2, 1),
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsGetContainerInfoV3_3(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists) {
	return GSGridStore::getContainerInfo(
			store, name, info, exists, ClientVersion(3, 5),
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsGetTimeSeries(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, GSTimeSeries **timeSeries) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(timeSeries, NULL);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, name, binding, timeSeries);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(name);
		GS_CLIENT_CHECK_NOT_NULL(binding);
		GS_CLIENT_CHECK_NOT_NULL(timeSeries);

		*timeSeries = store->getTimeSeries(name, *binding);
	}
	catch (...) {
		if (timeSeries != NULL) {
			*timeSeries = NULL;
		}
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsPutRowByPath(
		GSGridStore *store, const GSChar *pathKey, const void *rowObj,
		GSBool *exists) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(exists, GS_FALSE);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, pathKey, rowObj, exists);

	GSBool existsStorage;
	if (exists == NULL) {
		exists = &existsStorage;
	}

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(pathKey);
		GS_CLIENT_CHECK_NOT_NULL(rowObj);

		*exists = store->putRow(pathKey, rowObj);
	}
	catch (...) {
		*exists = GS_FALSE;
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsPutContainer(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container) {
	return GSGridStore::putContainer(
			store, name, true, binding, info, modifiable, container, NULL,
			ClientVersion(1, 0), GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsPutCollection(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, const GSCollectionProperties *properties,
		GSBool modifiable, GSCollection **collection) {
	const GSContainerType containerType = GS_CONTAINER_COLLECTION;
	(void) properties;
	return GSGridStore::putContainer(
			store, name, true, binding, NULL, modifiable, collection,
			&containerType, ClientVersion(1, 0),
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsCompatibleFunc_PutTimeSeries1(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, const GSTimeSeriesProperties *properties,
		GSBool modifiable, GSTimeSeries **timeSeries) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(timeSeries, NULL);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(
			store, name, binding, properties, modifiable, timeSeries);

	try {
		if (properties != NULL) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION,
					"Operation not supported for old properties struct");
		}
		return gsPutTimeSeries(
				store, name, binding, NULL, modifiable, timeSeries);
	}
	catch (...) {
		if (timeSeries != NULL) {
			*timeSeries = NULL;
		}
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsPutTimeSeries(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, const GSTimeSeriesProperties *properties,
		GSBool modifiable, GSTimeSeries **timeSeries) {
	GSContainerInfo info = GS_CONTAINER_INFO_INITIALIZER;
	info.type = GS_CONTAINER_TIME_SERIES;
	info.timeSeriesProperties = properties;

	return GSGridStore::putContainer(
			store, name, true, binding, &info, modifiable, timeSeries,
			NULL, ClientVersion(1, 5),
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsPutTimeSeriesV2_0(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, const GSTimeSeriesProperties *properties,
		GSBool modifiable, GSTimeSeries **timeSeries) {
	GSContainerInfo info = GS_CONTAINER_INFO_INITIALIZER;
	info.type = GS_CONTAINER_TIME_SERIES;
	info.timeSeriesProperties = properties;

	return GSGridStore::putContainer(
			store, name, true, binding, &info, modifiable, timeSeries,
			NULL, ClientVersion(2, 0),
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsRemoveRowByPath(
		GSGridStore *store, const GSChar *pathKey, GSBool *exists) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(exists, GS_FALSE);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, pathKey, exists);

	GSBool existsStorage;
	if (exists == NULL) {
		exists = &existsStorage;
	}

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(pathKey);

		*exists = store->removeRow(pathKey);
	}
	catch (...) {
		*exists = GS_FALSE;
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsDeleteRowByPath(
		GSGridStore *store, const GSChar *pathKey, GSBool *exists) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(exists, GS_FALSE);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, pathKey, exists);

	GSBool existsStorage;
	if (exists == NULL) {
		exists = &existsStorage;
	}

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(pathKey);

		*exists = store->removeRow(pathKey);
	}
	catch (...) {
		*exists = GS_FALSE;
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsPutContainerGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container) {
	return GSGridStore::putContainer(
			store, name, info, modifiable, container, NULL,
			ClientVersion(1, 5), GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsPutContainerGeneralV2_0(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container) {
	return GSGridStore::putContainer(
			store, name, info, modifiable, container, NULL,
			ClientVersion(2, 0), GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsPutContainerGeneralV2_1(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container) {
	return GSGridStore::putContainer(
			store, name, info, modifiable, container, NULL,
			ClientVersion(2, 1), GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsPutContainerGeneralV3_3(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container) {
	return GSGridStore::putContainer(
			store, name, info, modifiable, container, NULL,
			ClientVersion(3, 5), GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsGetContainerGeneral(
		GSGridStore *store, const GSChar *name, GSContainer **container) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(container, NULL);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, name, container);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(name);
		GS_CLIENT_CHECK_NOT_NULL(container);

		*container = store->getContainer(name, NULL);
	}
	catch (...) {
		if (container != NULL) {
			*container = NULL;
		}
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsPutCollectionGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSCollection **collection) {
	const GSContainerType containerType = GS_CONTAINER_COLLECTION;
	return GSGridStore::putContainer(
			store, name, info, modifiable, collection, &containerType,
			ClientVersion(1, 5), GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsPutCollectionGeneralV2_1(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSCollection **collection) {
	const GSContainerType containerType = GS_CONTAINER_COLLECTION;
	return GSGridStore::putContainer(
			store, name, info, modifiable, collection, &containerType,
			ClientVersion(2, 1), GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsPutCollectionGeneralV3_3(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSCollection **collection) {
	const GSContainerType containerType = GS_CONTAINER_COLLECTION;
	return GSGridStore::putContainer(
			store, name, info, modifiable, collection, &containerType,
			ClientVersion(3, 5), GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsGetCollectionGeneral(
		GSGridStore *store, const GSChar *name, GSCollection **collection) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(collection, NULL);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, name, collection);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(name);
		GS_CLIENT_CHECK_NOT_NULL(collection);

		const GSContainerType containerType = GS_CONTAINER_COLLECTION;
		*collection = store->getContainer(name, &containerType);
	}
	catch (...) {
		if (collection != NULL) {
			*collection = NULL;
		}
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsPutTimeSeriesGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries) {
	const GSContainerType containerType = GS_CONTAINER_TIME_SERIES;
	return GSGridStore::putContainer(
			store, name, info, modifiable, timeSeries, &containerType,
			ClientVersion(1, 5), GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsPutTimeSeriesGeneralV2_0(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries) {
	const GSContainerType containerType = GS_CONTAINER_TIME_SERIES;
	return GSGridStore::putContainer(
			store, name, info, modifiable, timeSeries, &containerType,
			ClientVersion(2, 0), GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsPutTimeSeriesGeneralV2_1(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries) {
	const GSContainerType containerType = GS_CONTAINER_TIME_SERIES;
	return GSGridStore::putContainer(
			store, name, info, modifiable, timeSeries, &containerType,
			ClientVersion(2, 1), GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsPutTimeSeriesGeneralV3_3(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries) {
	const GSContainerType containerType = GS_CONTAINER_TIME_SERIES;
	return GSGridStore::putContainer(
			store, name, info, modifiable, timeSeries, &containerType,
			ClientVersion(3, 5), GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsGetTimeSeriesGeneral(
		GSGridStore *store, const GSChar *name, GSTimeSeries **timeSeries) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(timeSeries, NULL);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, name, timeSeries);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(name);
		GS_CLIENT_CHECK_NOT_NULL(timeSeries);

		const GSContainerType containerType = GS_CONTAINER_TIME_SERIES;
		*timeSeries = store->getContainer(name, &containerType);
	}
	catch (...) {
		if (timeSeries != NULL) {
			*timeSeries = NULL;
		}
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsDropContainer(
		GSGridStore *store, const GSChar *name) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, name);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(name);

		store->dropContainer(name, NULL);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsCreateRowByStore(
		GSGridStore *store, const GSContainerInfo *info, GSRow **row) {
	return GSGridStore::createRow(
			store, info, row, ClientVersion(1, 5),
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsCreateRowByStoreV3_3(
		GSGridStore *store, const GSContainerInfo *info, GSRow **row) {
	return GSGridStore::createRow(
			store, info, row, ClientVersion(3, 5),
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(store));
}

GSResult GS_API_CALL gsFetchAll(
		GSGridStore *store, GSQuery *const *queryList, size_t queryCount) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, queryList, queryCount);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);

		store->fetchAll(queryList, queryCount);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsPutMultipleContainerRows(
		GSGridStore *store, const GSContainerRowEntry *entryList,
		size_t entryCount) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, entryList, entryCount);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);

		store->multiPut(entryList, entryCount);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetMultipleContainerRows(
		GSGridStore *store,
		const GSRowKeyPredicateEntry *const *predicateList,
		size_t predicateCount,
		const GSContainerRowEntry **entryList, size_t *entryCount) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT2(
			entryList, NULL, entryCount, 0);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(
			store, predicateList, predicateCount, entryList, entryCount);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(entryList);
		GS_CLIENT_CHECK_NOT_NULL(entryCount);

		store->multiGet(*predicateList, predicateCount, *entryList, *entryCount);
	}
	catch (...) {
		if (entryList != NULL) {
			*entryList = NULL;
		}
		if (entryCount != NULL) {
			*entryCount = 0;
		}
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetPartitionController(
		GSGridStore *store, GSPartitionController **partitionController) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(
			partitionController, NULL);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, partitionController);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(partitionController);

		*partitionController = store->getPartitionController();
	}
	catch (...) {
		if (partitionController != NULL) {
			*partitionController = NULL;
		}
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsCreateRowKeyPredicate(
		GSGridStore *store, GSType keyType, GSRowKeyPredicate **predicate) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(predicate, NULL);
	GSResourceHeader::clearLastError(store);
	GS_CLIENT_INTERCEPT_API_CALL(store, keyType, predicate);

	try {
		GS_CLIENT_CHECK_NOT_NULL(store);
		GS_CLIENT_CHECK_NOT_NULL(predicate);

		*predicate = store->createRowKeyPredicate(keyType);
	}
	catch (...) {
		if (predicate != NULL) {
			*predicate = NULL;
		}
		return GSResourceHeader::setCurrentException(store);
	}
	return GS_RESULT_OK;
}


void GS_API_CALL gsCloseContainer(
		GSContainer **container, GSBool allRelated) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_INIT(container, NULL);
	GSContainer::close(container, ClientUtil::toBool(allRelated));
}

GSResult GS_API_CALL gsCreateEventNotification(
		GSContainer *container, const GSChar *url) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, url);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(url);

		container->createEventNotification(url);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsCreateTrigger(
		GSContainer *container, const GSTriggerInfo *info) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, info);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(info);

		container->createTrigger(info);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsCreateIndex(
		GSContainer *container,
		const GSChar *columnName, GSIndexTypeFlags flags) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, columnName, flags);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(columnName);

		container->createOrDropIndex(columnName, flags, true);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsCreateIndexDetail(
		GSContainer *container, const GSIndexInfo *info) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, info);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(info);

		container->createOrDropIndex(*info, true);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsDropEventNotification(
		GSContainer *container, const GSChar *url) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, url);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(url);

		container->dropEventNotification(url);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsDropTrigger(
		GSContainer *container, const GSChar *name) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, name);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(name);

		container->dropTrigger(name);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsDropIndex(
		GSContainer *container,
		const GSChar *columnName, GSIndexTypeFlags flags) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, columnName, flags);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(columnName);

		container->createOrDropIndex(columnName, flags, false);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsDropIndexDetail(
		GSContainer *container, const GSIndexInfo *info) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, info);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(info);

		container->createOrDropIndex(*info, false);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsFlush(GSContainer *container) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);

		container->flush();
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetRow(
		GSContainer *container, const void *key, void *rowObj, GSBool *exists) {
	return GSContainer::getRowChecked<GSContainer::AnyKeyTraits>(
			container, key, rowObj, GS_FALSE, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsPutRow(
		GSContainer *container, const void *key, const void *rowObj,
		GSBool *exists) {
	return GSContainer::putRowChecked<GSContainer::AnyKeyTraits>(
			container, key, rowObj, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsCompatibleFunc_PutMultipleRows1(
		GSContainer *container, size_t rowCount, const void *const *rowObjs,
		GSBool *exists) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(exists, GS_FALSE);
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, rowCount, rowObjs, exists);

	GSBool existsStorage;
	if (exists == NULL) {
		exists = &existsStorage;
	}

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);

		*exists = container->putMultipleRows(rowCount, rowObjs);
	}
	catch (...) {
		*exists = GS_FALSE;
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsPutMultipleRows(
		GSContainer *container, const void *const *rowObjs, size_t rowCount,
		GSBool *exists) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(exists, GS_FALSE);
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, rowObjs, rowCount, exists);

	GSBool existsStorage;
	if (exists == NULL) {
		exists = &existsStorage;
	}

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);

		*exists = container->putMultipleRows(rowCount, rowObjs);
	}
	catch (...) {
		*exists = GS_FALSE;
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsQuery(GSContainer *container,
		const GSChar *queryString, GSQuery **query) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(query, NULL);
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, queryString, query);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(queryString);
		GS_CLIENT_CHECK_NOT_NULL(query);

		*query = container->query(queryString);
	}
	catch (...) {
		if (query != NULL) {
			*query = NULL;
		}
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsRemoveRow(
		GSContainer *container, const void *key, GSBool *exists) {
	return gsDeleteRow(container, key, exists);
}

GSResult GS_API_CALL gsDeleteRow(
		GSContainer *container, const void *key, GSBool *exists) {
	return GSContainer::removeRowChecked<GSContainer::AnyKeyTraits>(
			container, key, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GS_DLL_PUBLIC GSResult GS_API_CALL gsGetContainerType(
		GSContainer *container, GSContainerType *type) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(
			type, GS_CONTAINER_COLLECTION);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(type);

		*type = container->getType();
	}
	catch (...) {
		if (type != NULL) {
			*type = GS_CONTAINER_COLLECTION;
		}
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsCreateRowByContainer(
		GSContainer *container, GSRow **row) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(row, NULL);
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, row);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(row);

		*row = container->createRow();
	}
	catch (...) {
		if (row != NULL) {
			*row = NULL;
		}
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}


GSResult GS_API_CALL gsAbort(GSContainer *container) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);

		container->abort();
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsCommit(GSContainer *container) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);

		container->commit();
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetRowForUpdate(GSContainer *container,
		const void *key, void *rowObj, GSBool *exists) {
	return GSContainer::getRowChecked<GSContainer::AnyKeyTraits>(
			container, key, rowObj, GS_TRUE, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsSetAutoCommit(
		GSContainer *container, GSBool enabled) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, enabled);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);

		container->setAutoCommit(ClientUtil::toBool(enabled));
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}


GSResult GS_API_CALL gsGetRowByInteger(
		GSContainer *container, int32_t key, void *rowObj,
		GSBool forUpdate, GSBool *exists) {
	typedef RowMapper::TypeTraits<GS_TYPE_INTEGER, false> Traits;
	return GSContainer::getRowChecked<Traits>(
			container, &key, rowObj, forUpdate, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsGetRowByLong(
		GSContainer *container, int64_t key, void *rowObj,
		GSBool forUpdate, GSBool *exists) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, false> Traits;
	return GSContainer::getRowChecked<Traits>(
			container, &key, rowObj, forUpdate, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsGetRowByTimestamp(
		GSContainer *container, GSTimestamp key, void *rowObj,
		GSBool forUpdate, GSBool *exists) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, false> Traits;
	return GSContainer::getRowChecked<Traits>(
			container, &key, rowObj, forUpdate, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsGetRowByString(
		GSContainer *container, const GSChar *key, void *rowObj,
		GSBool forUpdate, GSBool *exists) {
	typedef RowMapper::TypeTraits<GS_TYPE_STRING, false> Traits;
	return GSContainer::getRowChecked<Traits>(
			container, &key, rowObj, forUpdate, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsPutRowByInteger(
		GSContainer *container, int32_t key, const void *rowObj,
		GSBool *exists) {
	typedef RowMapper::TypeTraits<GS_TYPE_INTEGER, false> Traits;
	return GSContainer::putRowChecked<Traits>(
			container, &key, rowObj, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsPutRowByLong(
		GSContainer *container, int64_t key, const void *rowObj,
		GSBool *exists) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, false> Traits;
	return GSContainer::putRowChecked<Traits>(
			container, &key, rowObj, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsPutRowByTimestamp(
		GSContainer *container, GSTimestamp key, const void *rowObj,
		GSBool *exists) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, false> Traits;
	return GSContainer::putRowChecked<Traits>(
			container, &key, rowObj, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsPutRowByString(
		GSContainer *container, const GSChar *key, const void *rowObj,
		GSBool *exists) {
	typedef RowMapper::TypeTraits<GS_TYPE_STRING, false> Traits;
	return GSContainer::putRowChecked<Traits>(
			container, &key, rowObj, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsRemoveRowByInteger(
		GSContainer *container, int32_t key, GSBool *exists) {
	return gsDeleteRowByInteger(
			container, key, exists);
}

GSResult GS_API_CALL gsRemoveRowByLong(
		GSContainer *container, int64_t key, GSBool *exists) {
	return gsDeleteRowByLong(
			container, key, exists);
}

GSResult GS_API_CALL gsRemoveRowByTimestamp(
		GSContainer *container, GSTimestamp key, GSBool *exists) {
	return gsDeleteRowByTimestamp(
			container, key, exists);
}

GSResult GS_API_CALL gsRemoveRowByString(
		GSContainer *container, const GSChar *key, GSBool *exists) {
	return gsDeleteRowByString(
			container, key, exists);
}

GSResult GS_API_CALL gsDeleteRowByInteger(
		GSContainer *container, int32_t key, GSBool *exists) {
	typedef RowMapper::TypeTraits<GS_TYPE_INTEGER, false> Traits;
	return GSContainer::removeRowChecked<Traits>(
			container, &key, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsDeleteRowByLong(
		GSContainer *container, int64_t key, GSBool *exists) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, false> Traits;
	return GSContainer::removeRowChecked<Traits>(
			container, &key, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsDeleteRowByTimestamp(
		GSContainer *container, GSTimestamp key, GSBool *exists) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, false> Traits;
	return GSContainer::removeRowChecked<Traits>(
			container, &key, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}

GSResult GS_API_CALL gsDeleteRowByString(
		GSContainer *container, const GSChar *key, GSBool *exists) {
	typedef RowMapper::TypeTraits<GS_TYPE_STRING, false> Traits;
	return GSContainer::removeRowChecked<Traits>(
			container, &key, exists,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(container));
}


GSResult GS_API_CALL gsQueryByGeometry(
		GSContainer *collection, const GSChar *column, const GSChar *geometry,
		GSGeometryOperator geometryOp, GSQuery **query) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(query, NULL);
	GSResourceHeader::clearLastError(collection);
	GS_CLIENT_INTERCEPT_API_CALL(
			collection, column, geometry, geometryOp, query);

	try {
		GS_CLIENT_CHECK_NOT_NULL(collection);
		GS_CLIENT_CHECK_NOT_NULL(column);
		GS_CLIENT_CHECK_NOT_NULL(geometry);
		GS_CLIENT_CHECK_NOT_NULL(query);

		*query = collection->queryByGeometry(column, geometry, geometryOp);
	}
	catch (...) {
		if (query != NULL) {
			*query = NULL;
		}
		return GSResourceHeader::setCurrentException(collection);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsQueryByGeometryWithExclusion(
		GSContainer *collection, const GSChar *column,
		const GSChar *geometryIntersection, const GSChar *geometryDisjoint,
		GSQuery **query) {
	return gsQueryByGeometryWithDisjointCondition(
			collection, column, geometryIntersection, geometryDisjoint,
			query);
}

GSResult GS_API_CALL gsQueryByGeometryWithDisjointCondition(
		GSContainer *collection, const GSChar *column,
		const GSChar *geometryIntersection, const GSChar *geometryDisjoint,
		GSQuery **query) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(query, NULL);
	GSResourceHeader::clearLastError(collection);
	GS_CLIENT_INTERCEPT_API_CALL(
			collection, column, geometryIntersection, geometryDisjoint, query);

	try {
		GS_CLIENT_CHECK_NOT_NULL(collection);
		GS_CLIENT_CHECK_NOT_NULL(column);
		GS_CLIENT_CHECK_NOT_NULL(geometryIntersection);
		GS_CLIENT_CHECK_NOT_NULL(geometryDisjoint);
		GS_CLIENT_CHECK_NOT_NULL(query);

		*query = collection->queryByGeometry(
				column, geometryIntersection, geometryDisjoint);
	}
	catch (...) {
		if (query != NULL) {
			*query = NULL;
		}
		return GSResourceHeader::setCurrentException(collection);
	}
	return GS_RESULT_OK;
}


GSResult GS_API_CALL gsAggregateTimeSeries(
		GSTimeSeries *timeSeries, GSTimestamp start, GSTimestamp end,
		const GSChar *column, GSAggregation aggregation,
		GSAggregationResult **aggregationResult) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(aggregationResult, NULL);
	GSResourceHeader::clearLastError(timeSeries);
	GS_CLIENT_INTERCEPT_API_CALL(
			timeSeries, start, end, column, aggregation, aggregationResult);

	try {
		GS_CLIENT_CHECK_NOT_NULL(timeSeries);
		GS_CLIENT_CHECK_NOT_NULL(aggregationResult);

		*aggregationResult = timeSeries->aggregateTimeSeries(
				start, end, column, aggregation);
	}
	catch (...) {
		if (aggregationResult != NULL) {
			*aggregationResult = NULL;
		}
		return GSResourceHeader::setCurrentException(timeSeries);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsAppendTimeSeriesRow(
		GSTimeSeries *timeSeries, const void *rowObj, GSBool *exists) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(exists, GS_FALSE);
	GSResourceHeader::clearLastError(timeSeries);
	GS_CLIENT_INTERCEPT_API_CALL(timeSeries, rowObj, exists);

	GSBool existsStorage;
	if (exists == NULL) {
		exists = &existsStorage;
	}

	try {
		GS_CLIENT_CHECK_NOT_NULL(timeSeries);
		GS_CLIENT_CHECK_NOT_NULL(rowObj);

		*exists = timeSeries->appendTimeSeriesRow(rowObj);
	}
	catch (...) {
		*exists = GS_FALSE;
		return GSResourceHeader::setCurrentException(timeSeries);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetRowByBaseTime(
		GSTimeSeries *timeSeries, GSTimestamp base, GSTimeOperator timeOp,
		void *rowObj, GSBool *exists) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(exists, GS_FALSE);
	GSResourceHeader::clearLastError(timeSeries);
	GS_CLIENT_INTERCEPT_API_CALL(timeSeries, base, timeOp, rowObj, exists);

	GSBool existsStorage;
	if (exists == NULL) {
		exists = &existsStorage;
	}

	try {
		GS_CLIENT_CHECK_NOT_NULL(timeSeries);
		GS_CLIENT_CHECK_NOT_NULL(rowObj);

		*exists = timeSeries->getTimeSeriesRow(base, timeOp, rowObj);
	}
	catch (...) {
		*exists = GS_FALSE;
		return GSResourceHeader::setCurrentException(timeSeries);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsInterpolateTimeSeriesRow(
		GSTimeSeries *timeSeries, GSTimestamp base, const GSChar *column,
		void *rowObj, GSBool *exists) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(exists, GS_FALSE);
	GSResourceHeader::clearLastError(timeSeries);
	GS_CLIENT_INTERCEPT_API_CALL(timeSeries, base, column, rowObj, exists);

	GSBool existsStorage;
	if (exists == NULL) {
		exists = &existsStorage;
	}

	try {
		GS_CLIENT_CHECK_NOT_NULL(timeSeries);
		GS_CLIENT_CHECK_NOT_NULL(rowObj);

		*exists = timeSeries->interpolateTimeSeriesRow(base, column, rowObj);
	}
	catch (...) {
		*exists = GS_FALSE;
		return GSResourceHeader::setCurrentException(timeSeries);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsQueryByTimeSeriesRange(
		GSTimeSeries *timeSeries, GSTimestamp start, GSTimestamp end,
		GSQuery **query) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(query, NULL);
	GSResourceHeader::clearLastError(timeSeries);
	GS_CLIENT_INTERCEPT_API_CALL(timeSeries, start, end, query);

	try {
		GS_CLIENT_CHECK_NOT_NULL(timeSeries);
		GS_CLIENT_CHECK_NOT_NULL(query);

		*query = timeSeries->queryByTime(&start, &end, GS_ORDER_ASCENDING);
	}
	catch (...) {
		if (query != NULL) {
			*query = NULL;
		}
		return GSResourceHeader::setCurrentException(timeSeries);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsQueryByTimeSeriesOrderedRange(
		GSTimeSeries *timeSeries,
		const GSTimestamp *start, const GSTimestamp *end,
		GSQueryOrder order, GSQuery **query) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(query, NULL);
	GSResourceHeader::clearLastError(timeSeries);
	GS_CLIENT_INTERCEPT_API_CALL(timeSeries, start, end, order, query);

	try {
		GS_CLIENT_CHECK_NOT_NULL(timeSeries);
		GS_CLIENT_CHECK_NOT_NULL(query);

		*query = timeSeries->queryByTime(start, end, order);
	}
	catch (...) {
		if (query != NULL) {
			*query = NULL;
		}
		return GSResourceHeader::setCurrentException(timeSeries);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsCompatibleFunc_QueryByTimeSeriesSampling1(
		GSTimeSeries *timeSeries, GSTimestamp start, GSTimestamp end,
		const GSChar *const *columnSet, int32_t interval, GSTimeUnit intervalUnit,
		GSQuery **query) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(query, NULL);
	GSResourceHeader::clearLastError(timeSeries);
	GS_CLIENT_INTERCEPT_API_CALL(
			timeSeries, start, end, columnSet, interval, intervalUnit, query);

	try {
		GS_CLIENT_CHECK_NOT_NULL(timeSeries);
		GS_CLIENT_CHECK_NOT_NULL(query);

		const bool useColumnCount = false;
		const GSInterpolationMode mode = GS_INTERPOLATION_LINEAR_OR_PREVIOUS;
		*query = timeSeries->queryByTime(
				start, end, columnSet, 0, useColumnCount,
				mode, interval, intervalUnit);
	}
	catch (...) {
		if (query != NULL) {
			*query = NULL;
		}
		return GSResourceHeader::setCurrentException(timeSeries);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsQueryByTimeSeriesSampling(
		GSTimeSeries *timeSeries, GSTimestamp start, GSTimestamp end,
		const GSChar *const *columnSet, size_t columnCount,
		GSInterpolationMode mode, int32_t interval, GSTimeUnit intervalUnit,
		GSQuery **query) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(query, NULL);
	GSResourceHeader::clearLastError(timeSeries);
	GS_CLIENT_INTERCEPT_API_CALL(
			timeSeries, start, end, columnSet, columnCount, mode, interval, intervalUnit, query);

	try {
		GS_CLIENT_CHECK_NOT_NULL(timeSeries);
		GS_CLIENT_CHECK_NOT_NULL(query);

		const bool useColumnCount = true;
		*query = timeSeries->queryByTime(
				start, end, columnSet, columnCount, useColumnCount,
				mode, interval, intervalUnit);
	}
	catch (...) {
		if (query != NULL) {
			*query = NULL;
		}
		return GSResourceHeader::setCurrentException(timeSeries);
	}
	return GS_RESULT_OK;
}


void GS_API_CALL gsCloseRow(GSRow **row) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_INIT(row, NULL);
	GSRow::close(row);
}

GSResult GS_API_CALL gsGetRowSchema(
		GSRow *row, GSContainerInfo *schemaInfo) {
	return GSRow::getContainerSchema(
			row, schemaInfo, ClientVersion(1, 5),
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowSchemaV2_1(
		GSRow *row, GSContainerInfo *schemaInfo) {
	return GSRow::getContainerSchema(
			row, schemaInfo, ClientVersion(2, 1),
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowSchemaV3_3(
		GSRow *row, GSContainerInfo *schemaInfo) {
	return GSRow::getContainerSchema(
			row, schemaInfo, ClientVersion(3, 5),
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldGeneral(
		GSRow *row, int32_t column, const GSValue *fieldValue,
		GSType type) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(row);
	GS_CLIENT_INTERCEPT_API_CALL(row, column, fieldValue, type);

	try {
		GS_CLIENT_CHECK_NOT_NULL(row);
		GS_CLIENT_CHECK_NOT_NULL(fieldValue);

		row->setField(column, *fieldValue, type);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(row);
	}

	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetRowFieldGeneral(
		GSRow *row, int32_t column, GSValue *fieldValue, GSType *type) {
	if (fieldValue != NULL) {
		memset(fieldValue, 0, sizeof(*fieldValue));
	}

	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(type, GS_TYPE_STRING);
	GSResourceHeader::clearLastError(row);
	GS_CLIENT_INTERCEPT_API_CALL(row, column, fieldValue, type);

	try {
		GS_CLIENT_CHECK_NOT_NULL(row);
		GS_CLIENT_CHECK_NOT_NULL(fieldValue);
		GS_CLIENT_CHECK_NOT_NULL(type);

		row->getField(column, *fieldValue, *type, NULL);
	}
	catch (...) {
		if (fieldValue != NULL) {
			memset(fieldValue, 0, sizeof(*fieldValue));
		}
		if (type != NULL) {
			*type = GS_TYPE_STRING;
		}
		return GSResourceHeader::setCurrentException(row);
	}

	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsSetRowFieldNull(GSRow *row, int32_t column) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(row);
	GS_CLIENT_INTERCEPT_API_CALL(row, column);

	try {
		GS_CLIENT_CHECK_NOT_NULL(row);

		row->setNull(column, true);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(row);
	}

	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetRowFieldNull(
		GSRow *row, int32_t column, GSBool *nullValue) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(nullValue, GS_FALSE);
	GSResourceHeader::clearLastError(row);
	GS_CLIENT_INTERCEPT_API_CALL(row, column, nullValue);

	try {
		GS_CLIENT_CHECK_NOT_NULL(row);
		GS_CLIENT_CHECK_NOT_NULL(nullValue);

		*nullValue = ClientUtil::toGSBool(row->isNull(column));
	}
	catch (...) {
		if (nullValue != NULL) {
			*nullValue = GS_FALSE;
		}
		return GSResourceHeader::setCurrentException(row);
	}

	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsSetRowFieldByString(
		GSRow *row, int32_t column, const GSChar *fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_STRING, false> Traits;
	return GSRow::setPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsString(
		GSRow *row, int32_t column, const GSChar **fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_STRING, false> Traits;
	return GSRow::getPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByBool(
		GSRow *row, int32_t column, GSBool fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_BOOL, false> Traits;
	return GSRow::setPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsBool(
		GSRow *row, int32_t column, GSBool *fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_BOOL, false> Traits;
	return GSRow::getPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByByte(
		GSRow *row, int32_t column, int8_t fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_BYTE, false> Traits;
	return GSRow::setPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsByte(
		GSRow *row, int32_t column, int8_t *fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_BYTE, false> Traits;
	return GSRow::getPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByShort(
		GSRow *row, int32_t column, int16_t fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_SHORT, false> Traits;
	return GSRow::setPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsShort(
		GSRow *row, int32_t column, int16_t *fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_SHORT, false> Traits;
	return GSRow::getPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByInteger(
		GSRow *row, int32_t column, int32_t fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_INTEGER, false> Traits;
	return GSRow::setPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsInteger(
		GSRow *row, int32_t column, int32_t *fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_INTEGER, false> Traits;
	return GSRow::getPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByLong(
		GSRow *row, int32_t column, int64_t fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, false> Traits;
	return GSRow::setPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsLong(
		GSRow *row, int32_t column, int64_t *fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, false> Traits;
	return GSRow::getPrimitiveField<Traits>
			(row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByFloat(
		GSRow *row, int32_t column, float fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_FLOAT, false> Traits;
	return GSRow::setPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsFloat(
		GSRow *row, int32_t column, float *fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_FLOAT, false> Traits;
	return GSRow::getPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByDouble(
		GSRow *row, int32_t column, double fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_DOUBLE, false> Traits;
	return GSRow::setPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsDouble(
		GSRow *row, int32_t column, double *fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_DOUBLE, false> Traits;
	return GSRow::getPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByTimestamp(
		GSRow *row, int32_t column, GSTimestamp fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, false> Traits;
	return GSRow::setPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsTimestamp(
		GSRow *row, int32_t column, GSTimestamp *fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, false> Traits;
	return GSRow::getPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByGeometry(
		GSRow *row, int32_t column, const GSChar *fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_GEOMETRY, false> Traits;
	return GSRow::setPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsGeometry(
		GSRow *row, int32_t column, const GSChar **fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_GEOMETRY, false> Traits;
	return GSRow::getPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByBlob(
		GSRow *row, int32_t column, const GSBlob *fieldValue) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(row);
	GS_CLIENT_INTERCEPT_API_CALL(row, column, fieldValue);

	try {
		GS_CLIENT_CHECK_NOT_NULL(row);
		GS_CLIENT_CHECK_NOT_NULL(fieldValue);

		GSValue generalValue;
		generalValue.asBlob = *fieldValue;

		row->setField(column, generalValue, GS_TYPE_BLOB);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(row);
	}

	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetRowFieldAsBlob(
		GSRow *row, int32_t column, GSBlob *fieldValue) {
	typedef RowMapper::TypeTraits<GS_TYPE_BLOB, false> Traits;
	return GSRow::getPrimitiveField<Traits>(
			row, column, fieldValue,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByStringArray(
		GSRow *row, int32_t column, const GSChar *const *fieldValue,
		size_t size) {
	typedef RowMapper::TypeTraits<GS_TYPE_STRING, true> Traits;
	return GSRow::setArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsStringArray(
		GSRow *row, int32_t column, const GSChar *const **fieldValue,
		size_t *size) {
	typedef RowMapper::TypeTraits<GS_TYPE_STRING, true> Traits;
	return GSRow::getArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByBoolArray(
		GSRow *row, int32_t column, const GSBool *fieldValue,
		size_t size) {
	typedef RowMapper::TypeTraits<GS_TYPE_BOOL, true> Traits;
	return GSRow::setArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsBoolArray(
		GSRow *row, int32_t column, const GSBool **fieldValue,
		size_t *size) {
	typedef RowMapper::TypeTraits<GS_TYPE_BOOL, true> Traits;
	return GSRow::getArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByByteArray(
		GSRow *row, int32_t column, const int8_t *fieldValue,
		size_t size) {
	typedef RowMapper::TypeTraits<GS_TYPE_BYTE, true> Traits;
	return GSRow::setArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsByteArray(
		GSRow *row, int32_t column, const int8_t **fieldValue,
		size_t *size) {
	typedef RowMapper::TypeTraits<GS_TYPE_BYTE, true> Traits;
	return GSRow::getArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByShortArray(
		GSRow *row, int32_t column, const int16_t *fieldValue,
		size_t size) {
	typedef RowMapper::TypeTraits<GS_TYPE_SHORT, true> Traits;
	return GSRow::setArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsShortArray(
		GSRow *row, int32_t column, const int16_t **fieldValue,
		size_t *size) {
	typedef RowMapper::TypeTraits<GS_TYPE_SHORT, true> Traits;
	return GSRow::getArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByIntegerArray(
		GSRow *row, int32_t column, const int32_t *fieldValue,
		size_t size) {
	typedef RowMapper::TypeTraits<GS_TYPE_INTEGER, true> Traits;
	return GSRow::setArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsIntegerArray(
		GSRow *row, int32_t column, const int32_t **fieldValue,
		size_t *size) {
	typedef RowMapper::TypeTraits<GS_TYPE_INTEGER, true> Traits;
	return GSRow::getArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByLongArray(
		GSRow *row, int32_t column, const int64_t *fieldValue,
		size_t size) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, true> Traits;
	return GSRow::setArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsLongArray(
		GSRow *row, int32_t column, const int64_t **fieldValue,
		size_t *size) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, true> Traits;
	return GSRow::getArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByFloatArray(
		GSRow *row, int32_t column, const float *fieldValue,
		size_t size) {
	typedef RowMapper::TypeTraits<GS_TYPE_FLOAT, true> Traits;
	return GSRow::setArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsFloatArray(
		GSRow *row, int32_t column, const float **fieldValue,
		size_t *size) {
	typedef RowMapper::TypeTraits<GS_TYPE_FLOAT, true> Traits;
	return GSRow::getArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByDoubleArray(
		GSRow *row, int32_t column, const double *fieldValue,
		size_t size) {
	typedef RowMapper::TypeTraits<GS_TYPE_DOUBLE, true> Traits;
	return GSRow::setArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsDoubleArray(
		GSRow *row, int32_t column, const double **fieldValue,
		size_t *size) {
	typedef RowMapper::TypeTraits<GS_TYPE_DOUBLE, true> Traits;
	return GSRow::getArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsSetRowFieldByTimestampArray(
		GSRow *row, int32_t column, const GSTimestamp *fieldValue,
		size_t size) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, true> Traits;
	return GSRow::setArrayField<Traits>(
			row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}

GSResult GS_API_CALL gsGetRowFieldAsTimestampArray(
		GSRow *row, int32_t column, const GSTimestamp **fieldValue,
		size_t *size) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, true> Traits;
	return GSRow::getArrayField<Traits>(row, column, fieldValue, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(row));
}



void GS_API_CALL gsCloseQuery(GSQuery **query) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_INIT(query, NULL);
	GSQuery::close(query);
}

GSResult GS_API_CALL gsFetch(
		GSQuery *query, GSBool forUpdate, GSRowSet **rowSet) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(rowSet, NULL);
	GSResourceHeader::clearLastError(query);
	GS_CLIENT_INTERCEPT_API_CALL(query, forUpdate, rowSet);

	try {
		GS_CLIENT_CHECK_NOT_NULL(query);
		GS_CLIENT_CHECK_NOT_NULL(rowSet);

		*rowSet = query->fetch(ClientUtil::toBool(forUpdate));
	}
	catch (...) {
		if (rowSet != NULL) {
			*rowSet = NULL;
		}
		return GSResourceHeader::setCurrentException(query);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsSetFetchOption(
		GSQuery *query, GSFetchOption fetchOption,
		const void *value, GSType valueType) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(query);
	GS_CLIENT_INTERCEPT_API_CALL(query, fetchOption, value, valueType);

	try {
		GS_CLIENT_CHECK_NOT_NULL(query);
		GS_CLIENT_CHECK_NOT_NULL(value);

		query->setFetchOption(fetchOption, value, valueType);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(query);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetRowSet(
		GSQuery *query, GSRowSet **rowSet) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(rowSet, NULL);
	GSResourceHeader::clearLastError(query);
	GS_CLIENT_INTERCEPT_API_CALL(query, rowSet);

	try {
		GS_CLIENT_CHECK_NOT_NULL(query);
		GS_CLIENT_CHECK_NOT_NULL(rowSet);

		*rowSet = query->getRowSet();
	}
	catch (...) {
		if (rowSet != NULL) {
			*rowSet = NULL;
		}
		return GSResourceHeader::setCurrentException(query);
	}
	return GS_RESULT_OK;
}


void GS_API_CALL gsCloseRowSet(GSRowSet **rowSet) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_INIT(rowSet, NULL);
	GSRowSet::close(rowSet);
}

GSResult GS_API_CALL gsDeleteCurrentRow(GSRowSet *rowSet) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(rowSet);
	GS_CLIENT_INTERCEPT_API_CALL(rowSet);

	try {
		GS_CLIENT_CHECK_NOT_NULL(rowSet);

		rowSet->remove();
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(rowSet);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetNextRow(GSRowSet *rowSet, void *rowObj) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(rowSet);
	GS_CLIENT_INTERCEPT_API_CALL(rowSet, rowObj);

	try {
		GS_CLIENT_CHECK_NOT_NULL(rowSet);
		GS_CLIENT_CHECK_NOT_NULL(rowObj);

		rowSet->next(rowObj);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(rowSet);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetNextAggregation(
		GSRowSet *rowSet, GSAggregationResult **aggregationResult) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(aggregationResult, NULL);
	GSResourceHeader::clearLastError(rowSet);
	GS_CLIENT_INTERCEPT_API_CALL(rowSet, aggregationResult);

	try {
		GS_CLIENT_CHECK_NOT_NULL(rowSet);
		GS_CLIENT_CHECK_NOT_NULL(aggregationResult);

		rowSet->nextAggregation(aggregationResult);
	}
	catch (...) {
		if (aggregationResult != NULL) {
			*aggregationResult = NULL;
		}
		return GSResourceHeader::setCurrentException(rowSet);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetNextQueryAnalysis(
		GSRowSet *rowSet, GSQueryAnalysisEntry *queryAnalysis) {
	const GSQueryAnalysisEntry initialQueryAnalysis =
			GS_QUERY_ANALYSIS_ENTRY_INITIALIZER;

	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(
			queryAnalysis, initialQueryAnalysis);
	GSResourceHeader::clearLastError(rowSet);
	GS_CLIENT_INTERCEPT_API_CALL(rowSet, queryAnalysis);

	try {
		GS_CLIENT_CHECK_NOT_NULL(rowSet);
		GS_CLIENT_CHECK_NOT_NULL(queryAnalysis);

		rowSet->nextQueryAnalysis(queryAnalysis);
	}
	catch (...) {
		if (queryAnalysis != NULL) {
			*queryAnalysis = initialQueryAnalysis;
		}
		return GSResourceHeader::setCurrentException(rowSet);
	}
	return GS_RESULT_OK;
}

GSRowSetType GS_API_CALL gsGetRowSetType(GSRowSet *rowSet) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(-1);

	if (rowSet == NULL) {
		return -1;
	}

	try {
		return rowSet->getType();
	}
	catch (...) {
	}
	return GS_ROW_SET_CONTAINER_ROWS;
}

int32_t GS_API_CALL gsGetRowSetSize(GSRowSet *rowSet) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(-1);

	if (rowSet == NULL) {
		return -1;
	}

	try {
		return rowSet->getSize();
	}
	catch (...) {
	}
	return -1;
}

GSBool GS_API_CALL gsHasNextRow(GSRowSet *rowSet) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(GS_FALSE);

	if (rowSet == NULL) {
		return GS_FALSE;
	}

	try {
		return rowSet->hasNext();
	}
	catch (...) {
	}
	return GS_FALSE;
}

GSResult GS_API_CALL gsUpdateCurrentRow(
		GSRowSet *rowSet, const void *rowObj) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(rowSet);
	GS_CLIENT_INTERCEPT_API_CALL(rowSet, rowObj);

	if (rowSet == NULL || rowObj == NULL) {
		return -1;
	}

	try {
		rowSet->update(rowObj);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(rowSet);
	}
	return GS_RESULT_OK;
}


void GS_API_CALL gsCloseAggregationResult(
		GSAggregationResult **aggregationResult) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_INIT(aggregationResult, NULL);
	GSAggregationResult::close(aggregationResult);
}

GSBool GS_API_CALL gsGetAggregationValue(
		GSAggregationResult *aggregationResult, void *value,
		GSType valueType) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(GS_FALSE);

	if (aggregationResult == NULL || value == NULL) {
		return GS_FALSE;
	}

	try {
		return aggregationResult->getValue(value, valueType);
	}
	catch (...) {
	}
	return GS_FALSE;
}

GSResult GS_API_CALL gsGetAggregationValueAsLong(
		GSAggregationResult *aggregationResult, int64_t *value,
		GSBool *assigned) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, false> Traits;
	return GSAggregationResult::getValueTyped<Traits>(
			aggregationResult, value, assigned,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(aggregationResult));
}

GSResult GS_API_CALL gsGetAggregationValueAsDouble(
		GSAggregationResult *aggregationResult, double *value,
		GSBool *assigned) {
	typedef RowMapper::TypeTraits<GS_TYPE_DOUBLE, false> Traits;
	return GSAggregationResult::getValueTyped<Traits>(
			aggregationResult, value, assigned,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(aggregationResult));
}

GSResult GS_API_CALL gsGetAggregationValueAsTimestamp(
		GSAggregationResult *aggregationResult, GSTimestamp *value,
		GSBool *assigned) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, false> Traits;
	return GSAggregationResult::getValueTyped<Traits>(
			aggregationResult, value, assigned,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(aggregationResult));
}


void GS_API_CALL gsCloseRowKeyPredicate(
		GSRowKeyPredicate **predicate) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_INIT(predicate, NULL);
	GSRowKeyPredicate::close(predicate);
}

GSResult GS_API_CALL gsGetPredicateKeyType(
		GSRowKeyPredicate *predicate, GSType *keyType) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(
			keyType, GS_TYPE_STRING);
	GSResourceHeader::clearLastError(predicate);
	GS_CLIENT_INTERCEPT_API_CALL(predicate, keyType);

	try {
		GS_CLIENT_CHECK_NOT_NULL(predicate);
		GS_CLIENT_CHECK_NOT_NULL(keyType);

		*keyType = predicate->getKeyType();
	}
	catch (...) {
		if (keyType != NULL) {
			*keyType = GS_TYPE_STRING;
		}
		return GSResourceHeader::setCurrentException(predicate);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetPredicateStartKeyGeneral(
		GSRowKeyPredicate *predicate, const GSValue **startKey) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(startKey, NULL);
	GSResourceHeader::clearLastError(predicate);
	GS_CLIENT_INTERCEPT_API_CALL(predicate, startKey);

	try {
		GS_CLIENT_CHECK_NOT_NULL(predicate);
		GS_CLIENT_CHECK_NOT_NULL(startKey);

		predicate->getRangeKey<GSRowKeyPredicate::RANGE_START>(
				*startKey, NULL);
	}
	catch (...) {
		if (startKey != NULL) {
			*startKey = NULL;
		}
		return GSResourceHeader::setCurrentException(predicate);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetPredicateStartKeyAsString(
		GSRowKeyPredicate *predicate, const GSChar **startKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_STRING, false> Traits;
	const GSChar *const *startKeyPtr = NULL;
	const GSResult result = GSRowKeyPredicate::getRangeKey<
			Traits, GSRowKeyPredicate::RANGE_START>(
			predicate, &startKeyPtr,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
	if (startKey != NULL) {
		if (startKeyPtr == NULL) {
			*startKey = NULL;
		}
		else {
			*startKey = *startKeyPtr;
		}
	}
	return result;
}

GSResult GS_API_CALL gsGetPredicateStartKeyAsInteger(
		GSRowKeyPredicate *predicate, const int32_t **startKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_INTEGER, false> Traits;
	return GSRowKeyPredicate::getRangeKey<
			Traits, GSRowKeyPredicate::RANGE_START>(
			predicate, startKey,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsGetPredicateStartKeyAsLong(
		GSRowKeyPredicate *predicate, const int64_t **startKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, false> Traits;
	return GSRowKeyPredicate::getRangeKey<
			Traits, GSRowKeyPredicate::RANGE_START>(
			predicate, startKey,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsGetPredicateStartKeyAsTimestamp(
		GSRowKeyPredicate *predicate, const GSTimestamp **startKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, false> Traits;
	return GSRowKeyPredicate::getRangeKey<
			Traits, GSRowKeyPredicate::RANGE_START>(
			predicate, startKey,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsGetPredicateFinishKeyGeneral(
		GSRowKeyPredicate *predicate, const GSValue **finishKey) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(finishKey, NULL);
	GSResourceHeader::clearLastError(predicate);
	GS_CLIENT_INTERCEPT_API_CALL(predicate, finishKey);

	try {
		GS_CLIENT_CHECK_NOT_NULL(predicate);
		GS_CLIENT_CHECK_NOT_NULL(finishKey);

		predicate->getRangeKey<GSRowKeyPredicate::RANGE_FINISH>(
				*finishKey, NULL);
	}
	catch (...) {
		if (finishKey != NULL) {
			*finishKey = NULL;
		}
		return GSResourceHeader::setCurrentException(predicate);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetPredicateFinishKeyAsString(
		GSRowKeyPredicate *predicate, const GSChar **finishKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_STRING, false> Traits;
	const GSChar *const *finishKeyPtr = NULL;
	const GSResult result = GSRowKeyPredicate::getRangeKey<
			Traits, GSRowKeyPredicate::RANGE_FINISH>(
			predicate, &finishKeyPtr,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
	if (finishKey != NULL) {
		if (finishKeyPtr == NULL) {
			*finishKey = NULL;
		}
		else {
			*finishKey = *finishKeyPtr;
		}
	}
	return result;
}

GSResult GS_API_CALL gsGetPredicateFinishKeyAsInteger(
		GSRowKeyPredicate *predicate, const int32_t **finishKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_INTEGER, false> Traits;
	return GSRowKeyPredicate::getRangeKey<
			Traits, GSRowKeyPredicate::RANGE_FINISH>(
			predicate, finishKey,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsGetPredicateFinishKeyAsLong(
		GSRowKeyPredicate *predicate, const int64_t **finishKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, false> Traits;
	return GSRowKeyPredicate::getRangeKey<
			Traits, GSRowKeyPredicate::RANGE_FINISH>(
			predicate, finishKey,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsGetPredicateFinishKeyAsTimestamp(
		GSRowKeyPredicate *predicate, const GSTimestamp **finishKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, false> Traits;
	return GSRowKeyPredicate::getRangeKey<
			Traits, GSRowKeyPredicate::RANGE_FINISH>(
			predicate, finishKey,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsGetPredicateDistinctKeysGeneral(
		GSRowKeyPredicate *predicate, const GSValue **keyList, size_t *size) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT2(keyList, NULL, size, 0);
	GSResourceHeader::clearLastError(predicate);
	GS_CLIENT_INTERCEPT_API_CALL(predicate, keyList, size);

	try {
		GS_CLIENT_CHECK_NOT_NULL(predicate);
		GS_CLIENT_CHECK_NOT_NULL(keyList);
		GS_CLIENT_CHECK_NOT_NULL(size);

		GSRowKeyPredicate::GeneralDistinctKeyReceiver receiver;
		predicate->getDistinctKeys(*keyList, *size, NULL, receiver);
	}
	catch (...) {
		if (keyList != NULL) {
			*keyList = NULL;
		}
		if (size != NULL) {
			*size = 0;
		}
		return GSResourceHeader::setCurrentException(predicate);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetPredicateDistinctKeysAsString(
		GSRowKeyPredicate *predicate,
		const GSChar *const **keyList, size_t *size) {
	typedef RowMapper::TypeTraits<GS_TYPE_STRING, false> Traits;
	return GSRowKeyPredicate::getDistinctKeys<Traits>(
			predicate, keyList, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsGetPredicateDistinctKeysAsInteger(
		GSRowKeyPredicate *predicate, const int32_t **keyList, size_t *size) {
	typedef RowMapper::TypeTraits<GS_TYPE_INTEGER, false> Traits;
	return GSRowKeyPredicate::getDistinctKeys<Traits>(
			predicate, keyList, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsGetPredicateDistinctKeysAsLong(
		GSRowKeyPredicate *predicate, const int64_t **keyList, size_t *size) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, false> Traits;
	return GSRowKeyPredicate::getDistinctKeys<Traits>(
			predicate, keyList, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsGetPredicateDistinctKeysAsTimestamp(
		GSRowKeyPredicate *predicate,
		const GSTimestamp **keyList, size_t *size) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, false> Traits;
	return GSRowKeyPredicate::getDistinctKeys<Traits>(
			predicate, keyList, size,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsSetPredicateStartKeyGeneral(
		GSRowKeyPredicate *predicate, const GSValue *startKey, GSType keyType) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(predicate);
	GS_CLIENT_INTERCEPT_API_CALL(predicate, startKey, keyType);

	try {
		GS_CLIENT_CHECK_NOT_NULL(predicate);

		predicate->setRangeKey<GSRowKeyPredicate::RANGE_START>(
				startKey, keyType);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(predicate);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsSetPredicateStartKeyByString(
		GSRowKeyPredicate *predicate, const GSChar *startKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_STRING, false> Traits;
	const GSChar *const *startKeyPtr = (startKey == NULL ? NULL : &startKey);
	return GSRowKeyPredicate::setRangeKey<
			Traits, GSRowKeyPredicate::RANGE_START>(
			predicate, startKeyPtr,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsSetPredicateStartKeyByInteger(
		GSRowKeyPredicate *predicate, const int32_t *startKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_INTEGER, false> Traits;
	return GSRowKeyPredicate::setRangeKey<
			Traits, GSRowKeyPredicate::RANGE_START>(
			predicate, startKey,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsSetPredicateStartKeyByLong(
		GSRowKeyPredicate *predicate, const int64_t *startKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, false> Traits;
	return GSRowKeyPredicate::setRangeKey<
			Traits, GSRowKeyPredicate::RANGE_START>(
			predicate, startKey,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsSetPredicateStartKeyByTimestamp(
		GSRowKeyPredicate *predicate, const GSTimestamp *startKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, false> Traits;
	return GSRowKeyPredicate::setRangeKey<
			Traits, GSRowKeyPredicate::RANGE_START>(
			predicate, startKey,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsSetPredicateFinishKeyGeneral(
		GSRowKeyPredicate *predicate, const GSValue *finishKey,
		GSType keyType) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(predicate);
	GS_CLIENT_INTERCEPT_API_CALL(predicate, finishKey, keyType);

	try {
		GS_CLIENT_CHECK_NOT_NULL(predicate);

		predicate->setRangeKey<GSRowKeyPredicate::RANGE_FINISH>(
				finishKey, keyType);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(predicate);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsSetPredicateFinishKeyByString(
		GSRowKeyPredicate *predicate, const GSChar *finishKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_STRING, false> Traits;
	const GSChar *const *finishKeyPtr = (finishKey == NULL ? NULL : &finishKey);
	return GSRowKeyPredicate::setRangeKey<
			Traits, GSRowKeyPredicate::RANGE_FINISH>(
			predicate, finishKeyPtr,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsSetPredicateFinishKeyByInteger(
		GSRowKeyPredicate *predicate, const int32_t *finishKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_INTEGER, false> Traits;
	return GSRowKeyPredicate::setRangeKey<
			Traits, GSRowKeyPredicate::RANGE_FINISH>(
			predicate, finishKey,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsSetPredicateFinishKeyByLong(
		GSRowKeyPredicate *predicate, const int64_t *finishKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, false> Traits;
	return GSRowKeyPredicate::setRangeKey<
			Traits, GSRowKeyPredicate::RANGE_FINISH>(
			predicate, finishKey,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsSetPredicateFinishKeyByTimestamp(
		GSRowKeyPredicate *predicate, const GSTimestamp *finishKey) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, false> Traits;
	return GSRowKeyPredicate::setRangeKey<
			Traits, GSRowKeyPredicate::RANGE_FINISH>(
			predicate, finishKey,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsAddPredicateKeyGeneral(
		GSRowKeyPredicate *predicate, const GSValue *key, GSType keyType) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(predicate);
	GS_CLIENT_INTERCEPT_API_CALL(predicate, key, keyType);

	try {
		GS_CLIENT_CHECK_NOT_NULL(predicate);
		GS_CLIENT_CHECK_NOT_NULL(key);

		predicate->addDistinctKey(*key, keyType);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(predicate);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsAddPredicateKeyByString(
		GSRowKeyPredicate *predicate, const GSChar *key) {
	typedef RowMapper::TypeTraits<GS_TYPE_STRING, false> Traits;
	return GSRowKeyPredicate::addDistinctKey<Traits>(
			predicate, key,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsAddPredicateKeyByInteger(
		GSRowKeyPredicate *predicate, int32_t key) {
	typedef RowMapper::TypeTraits<GS_TYPE_INTEGER, false> Traits;
	return GSRowKeyPredicate::addDistinctKey<Traits>(
			predicate, key,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsAddPredicateKeyByLong(
		GSRowKeyPredicate *predicate, int64_t key) {
	typedef RowMapper::TypeTraits<GS_TYPE_LONG, false> Traits;
	return GSRowKeyPredicate::addDistinctKey<Traits>(
			predicate, key,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}

GSResult GS_API_CALL gsAddPredicateKeyByTimestamp(
		GSRowKeyPredicate *predicate, GSTimestamp key) {
	typedef RowMapper::TypeTraits<GS_TYPE_TIMESTAMP, false> Traits;
	return GSRowKeyPredicate::addDistinctKey<Traits>(
			predicate, key,
			GS_CLIENT_INTERCEPT_API_CALL_FUNCTION(predicate));
}


void GS_API_CALL gsClosePartitionController(
		GSPartitionController **controller) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_INIT(controller, NULL);
	GSPartitionController::close(controller);
}

GSResult GS_API_CALL gsGetPartitionCount(
		GSPartitionController *controller, int32_t *partitionCount) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(partitionCount, -1);
	GSResourceHeader::clearLastError(controller);
	GS_CLIENT_INTERCEPT_API_CALL(controller, partitionCount);

	try {
		GS_CLIENT_CHECK_NOT_NULL(controller);
		GS_CLIENT_CHECK_NOT_NULL(partitionCount);

		*partitionCount = controller->getPartitionCount();
	}
	catch (...) {
		if (partitionCount != NULL) {
			*partitionCount = -1;
		}
		return GSResourceHeader::setCurrentException(controller);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetPartitionContainerCount(
		GSPartitionController *controller, int32_t partitionIndex,
		int64_t *containerCount) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(containerCount, -1);
	GSResourceHeader::clearLastError(controller);
	GS_CLIENT_INTERCEPT_API_CALL(controller, partitionIndex, containerCount);

	try {
		GS_CLIENT_CHECK_NOT_NULL(controller);
		GS_CLIENT_CHECK_NOT_NULL(containerCount);

		*containerCount = controller->getContainerCount(partitionIndex);
	}
	catch (...) {
		if (containerCount != NULL) {
			*containerCount = -1;
		}
		return GSResourceHeader::setCurrentException(controller);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetPartitionContainerNames(
		GSPartitionController *controller, int32_t partitionIndex,
		int64_t start, const int64_t *limit,
		const GSChar *const **nameList, size_t *size) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT2(
			nameList, NULL, size, 0);
	GSResourceHeader::clearLastError(controller);
	GS_CLIENT_INTERCEPT_API_CALL(
			controller, partitionIndex, start, limit, nameList, size);

	try {
		GS_CLIENT_CHECK_NOT_NULL(controller);
		GS_CLIENT_CHECK_NOT_NULL(nameList);
		GS_CLIENT_CHECK_NOT_NULL(size);

		controller->getContainerNames(partitionIndex, start, limit, *nameList, *size);
	}
	catch (...) {
		if (nameList != NULL) {
			*nameList = NULL;
		}
		if (size != NULL) {
			*size = 0;
		}
		return GSResourceHeader::setCurrentException(controller);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetPartitionHosts(
		GSPartitionController *controller, int32_t partitionIndex,
		const GSChar *const **addressList, size_t *size) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT2(
			addressList, NULL, size, 0);
	GSResourceHeader::clearLastError(controller);
	GS_CLIENT_INTERCEPT_API_CALL(controller, partitionIndex, addressList, size);

	try {
		GS_CLIENT_CHECK_NOT_NULL(controller);
		GS_CLIENT_CHECK_NOT_NULL(addressList);
		GS_CLIENT_CHECK_NOT_NULL(size);

		controller->getHosts(partitionIndex, *addressList, *size);
	}
	catch (...) {
		if (addressList != NULL) {
			*addressList = NULL;
		}
		if (size != NULL) {
			*size = 0;
		}
		return GSResourceHeader::setCurrentException(controller);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetPartitionOwnerHost(
		GSPartitionController *controller, int32_t partitionIndex,
		const GSChar **address) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(address, NULL);
	GSResourceHeader::clearLastError(controller);
	GS_CLIENT_INTERCEPT_API_CALL(controller, partitionIndex, address);

	try {
		GS_CLIENT_CHECK_NOT_NULL(controller);
		GS_CLIENT_CHECK_NOT_NULL(address);

		controller->getOwnerHost(partitionIndex, *address);
	}
	catch (...) {
		if (address != NULL) {
			*address = NULL;
		}
		return GSResourceHeader::setCurrentException(controller);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetPartitionBackupHosts(
		GSPartitionController *controller, int32_t partitionIndex,
		const GSChar *const **addressList, size_t *size) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT2(
			addressList, NULL, size, 0);
	GSResourceHeader::clearLastError(controller);
	GS_CLIENT_INTERCEPT_API_CALL(controller, partitionIndex, addressList, size);

	try {
		GS_CLIENT_CHECK_NOT_NULL(controller);
		GS_CLIENT_CHECK_NOT_NULL(addressList);
		GS_CLIENT_CHECK_NOT_NULL(size);

		controller->getBackupHosts(partitionIndex, *addressList, *size);
	}
	catch (...) {
		if (addressList != NULL) {
			*addressList = NULL;
		}
		if (size != NULL) {
			*size = 0;
		}
		return GSResourceHeader::setCurrentException(controller);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsAssignPartitionPreferableHost(
		GSPartitionController *controller, int32_t partitionIndex,
		const GSChar *host) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(controller);
	GS_CLIENT_INTERCEPT_API_CALL(controller, partitionIndex, host);

	try {
		GS_CLIENT_CHECK_NOT_NULL(controller);

		controller->assignPreferableHost(partitionIndex, host);
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(controller);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsGetPartitionIndexOfContainer(
		GSPartitionController *controller, const GSChar *containerName,
		int32_t *partitionIndex) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE_INIT(partitionIndex, -1);
	GSResourceHeader::clearLastError(controller);
	GS_CLIENT_INTERCEPT_API_CALL(controller, containerName, partitionIndex);

	try {
		GS_CLIENT_CHECK_NOT_NULL(controller);
		GS_CLIENT_CHECK_NOT_NULL(containerName);
		GS_CLIENT_CHECK_NOT_NULL(partitionIndex);

		*partitionIndex = controller->getPartitionIndexOfContainer(containerName);
	}
	catch (...) {
		if (partitionIndex != NULL) {
			*partitionIndex = -1;
		}
		return GSResourceHeader::setCurrentException(controller);
	}
	return GS_RESULT_OK;
}


GSTimestamp GS_API_CALL gsCurrentTime() {
	try {
		return TimestampUtil::current();
	}
	catch (...) {
	}
	return -1;
}

GSTimestamp GS_API_CALL gsAddTime(
		GSTimestamp timestamp, int32_t amount, GSTimeUnit timeUnit) {
	try {
		return TimestampUtil::add(timestamp, amount, timeUnit);
	}
	catch (...) {
	}
	return -1;
}

size_t GS_API_CALL gsFormatTime(
		GSTimestamp timestamp, GSChar *strBuf, size_t bufSize) {
	try {
		return TimestampUtil::format(timestamp, strBuf, bufSize);
	}
	catch (...) {
	}

	return ClientUtil::copyString("", 1, strBuf, bufSize);
}

GSBool GS_API_CALL gsParseTime(
		const GSChar *str, GSTimestamp *timestamp) {
	try {
		if (str != NULL && timestamp != NULL) {
			return TimestampUtil::parse(str, *timestamp);
		}
	}
	catch (...) {
	}

	if (timestamp != NULL) {
		*timestamp = -1;
	}
	return GS_FALSE;
}


size_t GS_API_CALL gsGetErrorStackSize(void *gsResource) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(0);
	try {
		util::Mutex *mutex;
		ErrorStack *stack = GSResourceHeader::findErrorStack(gsResource, mutex);
		if (stack == NULL) {
			return GS_FALSE;
		}

		DynamicLockGuard<util::Mutex> guard(mutex);
		return stack->getSize();
	}
	catch (...) {
	}
	return 0;
}

GSResult GS_API_CALL gsGetErrorCode(void *gsResource, size_t stackIndex) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	try {
		util::Mutex *mutex;
		ErrorStack *stack = GSResourceHeader::findErrorStack(gsResource, mutex);
		if (stack == NULL) {
			return GS_ERROR_CC_EMPTY_PARAMETER;
		}

		DynamicLockGuard<util::Mutex> guard(mutex);
		return stack->getErrorCode(stackIndex);
	}
	catch (...) {
	}
	return GS_ERROR_CC_INTERNAL_ERROR;
}

size_t GS_API_CALL gsFormatErrorMessage(
		void *gsResource, size_t stackIndex, GSChar *strBuf, size_t bufSize) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(0);
	try {
		util::Mutex *mutex;
		ErrorStack *stack = GSResourceHeader::findErrorStack(gsResource, mutex);
		if (stack == NULL) {
			ClientUtil::copyString("", 1, strBuf, bufSize);
			return 0;
		}

		DynamicLockGuard<util::Mutex> guard(mutex);
		return stack->formatErrorMessage(stackIndex, strBuf, bufSize);
	}
	catch (...) {
	}
	ClientUtil::copyString("", 1, strBuf, bufSize);
	return 0;
}

size_t GS_API_CALL gsFormatErrorLocation(
		void *gsResource, size_t stackIndex, GSChar *strBuf, size_t bufSize) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(0);
	try {
		util::Mutex *mutex;
		ErrorStack *stack = GSResourceHeader::findErrorStack(gsResource, mutex);
		if (stack == NULL) {
			ClientUtil::copyString("", 1, strBuf, bufSize);
			return 0;
		}

		DynamicLockGuard<util::Mutex> guard(mutex);
		return stack->formatErrorLocation(stackIndex, strBuf, bufSize);
	}
	catch (...) {
	}
	ClientUtil::copyString("", 1, strBuf, bufSize);
	return 0;
}

GSBool GS_API_CALL gsIsRecoverableError(GSResult result) {
	return ClientUtil::toGSBool(GridStoreChannel::isRecoverableError(result));
}

GSBool GS_API_CALL gsIsTimeoutError(GSResult result) {
	switch (result) {
	case GS_ERROR_CC_BAD_CONNECTION:
	case GS_ERROR_CC_CONNECTION_TIMEOUT:
	case GS_ERROR_CC_WRONG_NODE:
	case GS_ERROR_CC_MESSAGE_CORRUPTED:
	case GS_ERROR_CC_PARTITION_NOT_FOUND:
	case GS_ERROR_CC_ILLEGAL_PARTITION_COUNT:
		return GS_TRUE;
	default:
		return GS_FALSE;
	}
}

size_t GS_API_CALL gsFormatErrorName(
		void *gsResource, size_t stackIndex, GSChar *strBuf, size_t bufSize) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(0);
	try {
		util::Mutex *mutex;
		ErrorStack *stack = GSResourceHeader::findErrorStack(gsResource, mutex);
		if (stack == NULL) {
			ClientUtil::copyString("", 1, strBuf, bufSize);
			return 0;
		}

		DynamicLockGuard<util::Mutex> guard(mutex);
		return stack->formatErrorName(stackIndex, strBuf, bufSize);
	}
	catch (...) {
	}
	ClientUtil::copyString("", 1, strBuf, bufSize);
	return 0;
}

size_t GS_API_CALL gsFormatErrorDescription(
		void *gsResource, size_t stackIndex, GSChar *strBuf, size_t bufSize) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(0);
	try {
		util::Mutex *mutex;
		ErrorStack *stack = GSResourceHeader::findErrorStack(gsResource, mutex);
		if (stack == NULL) {
			ClientUtil::copyString("", 1, strBuf, bufSize);
			return 0;
		}

		DynamicLockGuard<util::Mutex> guard(mutex);
		return stack->formatErrorDescription(stackIndex, strBuf, bufSize);
	}
	catch (...) {
	}
	ClientUtil::copyString("", 1, strBuf, bufSize);
	return 0;
}

size_t GS_API_CALL gsGetErrorParameterCount(
		void *gsResource, size_t stackIndex) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(0);
	try {
		util::Mutex *mutex;
		ErrorStack *stack = GSResourceHeader::findErrorStack(gsResource, mutex);
		if (stack == NULL) {
			return 0;
		}

		DynamicLockGuard<util::Mutex> guard(mutex);
		return stack->getErrorParameterCount(stackIndex);
	}
	catch (...) {
	}
	return 0;
}

size_t GS_API_CALL gsFormatErrorParameterName(
		void *gsResource, size_t stackIndex, size_t parameterIndex,
		GSChar *strBuf, size_t bufSize) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(0);
	try {
		util::Mutex *mutex;
		ErrorStack *stack = GSResourceHeader::findErrorStack(gsResource, mutex);
		if (stack == NULL) {
			ClientUtil::copyString("", 1, strBuf, bufSize);
			return 0;
		}

		DynamicLockGuard<util::Mutex> guard(mutex);
		return stack->formatErrorParameterName(
				stackIndex, parameterIndex, strBuf, bufSize);
	}
	catch (...) {
	}
	ClientUtil::copyString("", 1, strBuf, bufSize);
	return 0;
}

size_t GS_API_CALL gsFormatErrorParameterValue(
		void *gsResource, size_t stackIndex, size_t parameterIndex,
		GSChar *strBuf, size_t bufSize) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_VALUE(0);
	try {
		util::Mutex *mutex;
		ErrorStack *stack = GSResourceHeader::findErrorStack(gsResource, mutex);
		if (stack == NULL) {
			ClientUtil::copyString("", 1, strBuf, bufSize);
			return 0;
		}

		DynamicLockGuard<util::Mutex> guard(mutex);
		return stack->formatErrorParameterValue(
				stackIndex, parameterIndex, strBuf, bufSize);
	}
	catch (...) {
	}
	ClientUtil::copyString("", 1, strBuf, bufSize);
	return 0;
}


#if GS_EXPERIMENTAL_TOOL_ENABLED
GSResult GS_API_CALL gsExperimentalGetRowIdForUpdate(
		GSRowSet *rowSet, GSExperimentalRowId *rowId) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(rowSet);
	GS_CLIENT_INTERCEPT_API_CALL(rowSet, rowId);

	try {
		GS_CLIENT_CHECK_NOT_NULL(rowSet);
		GS_CLIENT_CHECK_NOT_NULL(rowId);

		GSContainer *container = rowSet->getContainer();

		rowId->internal.container = container;
		rowId->internal.transactionId = rowSet->getTransactionId();
		if (rowId->internal.transactionId == 0) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_NOT_LOCKED, "");
		}

		switch (container->getMapper().getCategory()) {
		case RowMapper::CATEGORY_COLLECTION:
			rowId->internal.baseId = rowSet->getRowId();
			break;
		case RowMapper::CATEGORY_TIME_SERIES:
			{
				const GSBinding &binding =
						container->getMapper().getBinding();
				const GSBindingEntry *bindingEntry = NULL;
				for (size_t i = 0; binding.entryCount; i++) {
					if ((binding.entries[i].options &
							GS_TYPE_OPTION_KEY) != 0) {
						bindingEntry = &binding.entries[i];
						break;
					}
				}
				if (bindingEntry == NULL ||
						bindingEntry->elementType != GS_TYPE_TIMESTAMP) {
					GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
				}
			}
			rowId->internal.baseId = *static_cast<const GSTimestamp*>(
					rowSet->getRowKey());
			break;
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
		}
	}
	catch (...) {
		if (rowId != NULL) {
			rowId->internal.container = NULL;
			rowId->internal.transactionId = 0;
			rowId->internal.baseId = 0;
		}
		return GSResourceHeader::setCurrentException(rowSet);
	}

	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsExperimentalUpdateRowById(
		GSContainer *container, const GSExperimentalRowId *rowId,
		const void *rowObj) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, rowId, rowObj);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(rowId);
		GS_CLIENT_CHECK_NOT_NULL(rowObj);

		if (rowId->internal.container != container) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
		}

		switch (container->getMapper().getCategory()) {
		case RowMapper::CATEGORY_COLLECTION:
			container->updateRow(
					container->getMapper(), rowId->internal.transactionId,
					NULL, rowId->internal.baseId, NULL, rowObj);
			break;
		case RowMapper::CATEGORY_TIME_SERIES:
			container->updateRow(
					container->getMapper(), rowId->internal.transactionId,
					NULL, 0,
					&static_cast<const GSTimestamp&>(rowId->internal.baseId),
					rowObj);
			break;
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
		}
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

GSResult GS_API_CALL gsExperimentalDeleteRowById(
		GSContainer *container, const GSExperimentalRowId *rowId) {
	GS_CLIENT_CHECK_FACTORY_AND_RETURN_CODE();
	GSResourceHeader::clearLastError(container);
	GS_CLIENT_INTERCEPT_API_CALL(container, rowId);

	try {
		GS_CLIENT_CHECK_NOT_NULL(container);
		GS_CLIENT_CHECK_NOT_NULL(rowId);

		if (rowId->internal.container != container) {
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_PARAMETER, "");
		}

		switch (container->getMapper().getCategory()) {
		case RowMapper::CATEGORY_COLLECTION:
			container->removeRow(
					container->getMapper(), rowId->internal.transactionId,
					NULL, rowId->internal.baseId, NULL);
			break;
		case RowMapper::CATEGORY_TIME_SERIES:
			container->removeRow(
					container->getMapper(), rowId->internal.transactionId,
					NULL, 0,
					&static_cast<const GSTimestamp&>(rowId->internal.baseId));
			break;
		default:
			GS_CLIENT_THROW_ERROR(GS_ERROR_CC_UNSUPPORTED_OPERATION, "");
		}
	}
	catch (...) {
		return GSResourceHeader::setCurrentException(container);
	}
	return GS_RESULT_OK;
}

#endif	

#ifdef GS_CLIENT_UNIT_TEST
} 


GSResult GS_API_CALL gsGetContainerInfoV3_3(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists) {
	return -1;
}

GSResult GS_API_CALL gsPutTimeSeriesV2_0(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, const GSTimeSeriesProperties *properties,
		GSBool modifiable, GSTimeSeries **timeSeries) {
	return -1;
}

GSResult GS_API_CALL gsPutContainerGeneralV3_3(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container) {
	return -1;
}

GSResult GS_API_CALL gsPutCollectionGeneralV3_3(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container) {
	return -1;
}

GSResult GS_API_CALL gsPutTimeSeriesGeneralV3_3(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries) {
	return -1;
}

GSResult GS_API_CALL gsCreateRowByStoreV3_3(
		GSGridStore *store, const GSContainerInfo *info, GSRow **row) {
	return -1;
}

GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowSchemaV3_3(
		GSRow *row, GSContainerInfo *schemaInfo) {
	return -1;
}

#endif	
