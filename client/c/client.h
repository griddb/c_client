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
	@brief Definition of Client Internal Module
*/
#ifndef CLIENT_H_
#define CLIENT_H_

#ifdef GS_DLL_INSIDE
#define gsGetContainerInfo gsGetContainerInfo_Static
#define gsPutTimeSeries gsPutTimeSeries_Static
#define gsPutContainerGeneral gsPutContainerGeneral_Static
#define gsPutCollectionGeneral gsPutCollectionGeneral_Static
#define gsPutTimeSeriesGeneral gsPutTimeSeriesGeneral_Static
#define gsCreateRowByStore gsCreateRowByStore_Static
#define gsGetRowSchema gsGetRowSchema_Static
#endif 

#ifdef GS_CLIENT_UNIT_TEST
#define gsGetContainerInfoV1_5 gsGetContainerInfoV1_5_Stub
#define gsGetContainerInfoV2_1 gsGetContainerInfoV2_1_Stub
#define gsGetContainerInfoV3_3 gsGetContainerInfoV3_3_Stub
#define gsPutTimeSeriesV2_0 gsPutTimeSeriesV2_0_Stub
#define gsPutContainerGeneralV2_0 gsPutContainerGeneralV2_0_Stub
#define gsPutContainerGeneralV2_1 gsPutContainerGeneralV2_1_Stub
#define gsPutContainerGeneralV3_3 gsPutContainerGeneralV3_3_Stub
#define gsPutCollectionGeneralV2_1 gsPutCollectionGeneralV2_1_Stub
#define gsPutCollectionGeneralV3_3 gsPutCollectionGeneralV3_3_Stub
#define gsPutTimeSeriesGeneralV2_0 gsPutTimeSeriesGeneralV2_0_Stub
#define gsPutTimeSeriesGeneralV2_1 gsPutTimeSeriesGeneralV2_1_Stub
#define gsPutTimeSeriesGeneralV3_3 gsPutTimeSeriesGeneralV3_3_Stub
#define gsCreateRowByStoreV3_3 gsCreateRowByStoreV3_3_Stub
#define gsGetRowSchemaV2_1 gsGetRowSchemaV2_1_Stub
#define gsGetRowSchemaV3_3 gsGetRowSchemaV3_3_Stub
#endif 

#if !defined(GS_DEPRECATION_IGNORABLE) && defined(GS_CLIENT_UNIT_TEST)
#define GS_DEPRECATION_IGNORABLE 1
#endif

#include "util/container.h"
#include "util/net.h"
#include "gridstore.h"
#include "authentication.h"
#include "service_address.h"
#include "container_key_utils.h"
#include "geometry_coder.h"
#include "gs_error_common.h"
#include "uuid/uuid.h"


#ifdef GS_DLL_INSIDE
#undef gsGetContainerInfo
#undef gsPutTimeSeries
#undef gsPutContainerGeneral
#undef gsPutCollectionGeneral
#undef gsPutTimeSeriesGeneral
#undef gsCreateRowByStore
#undef gsGetRowSchema

#ifdef __cplusplus
extern "C" {
#endif

GS_DLL_PUBLIC GSResult GS_API_CALL gsGetContainerInfo(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists);
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetContainerInfoV1_5(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists);
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetContainerInfoV2_1(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists);
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutTimeSeries(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, const GSTimeSeriesProperties *properties,
		GSBool modifiable, GSTimeSeries **timeSeries);
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutContainerGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container);
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutContainerGeneralV2_0(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container);
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutContainerGeneralV2_1(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container);
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutCollectionGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSCollection **collection);
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutCollectionGeneralV2_1(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSCollection **collection);
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutTimeSeriesGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries);
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutTimeSeriesGeneralV2_0(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries);
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutTimeSeriesGeneralV2_1(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries);
GS_DLL_PUBLIC GSResult GS_API_CALL gsCreateRowByStore(
		GSGridStore *store, const GSContainerInfo *info, GSRow **row);
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowSchema(
		GSRow *row, GSContainerInfo *schemaInfo);
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowSchemaV2_1(
		GSRow *row, GSContainerInfo *schemaInfo);

#ifdef __cplusplus
}
#endif

#endif	


typedef util::ByteStream<util::ArrayInStream> ArrayByteInStream;
typedef util::XArrayOutStream< std::allocator<uint8_t> > NormalXArrayOutStream;
typedef util::ByteStream<NormalXArrayOutStream> XArrayByteOutStream;


class ClientUtil {
public:
	static bool toBool(GSBool value);
	static GSBool toGSBool(bool value);
	static size_t toSizeValue(int64_t value);
	static size_t toSizeValue(int32_t value);
	static int32_t sizeValueToInt32(size_t value);
	static int64_t sizeValueToInt64(size_t value);
	template<typename T> static bool parseValue(
			const GSChar *strValue, T &resultValue);
	template<typename T> static T parseValue(const GSChar *strValue);

	static size_t copyString(
			const std::string &src, GSChar *dest, size_t destLenWithNull);
	static size_t copyString(
			const GSChar *src, size_t srcLenWithNull,
			GSChar *dest, size_t destLenWithNull) throw();

	static std::string normalizeSymbol(
			const GSChar *symbol, const GSChar *typeName);
	static std::string normalizeSymbolUnchecked(const GSChar *symbol);
	static void checkSymbol(const GSChar *symbol, const GSChar *typeName);
	static void checkBasicSymbol(const GSChar *symbol, const GSChar *typeName);
	static int32_t toASCIILower(int32_t ch);

	static bool isIPV6Address(const util::SocketAddress &address);
	static GSTimeUnit checkTimeUnit(GSTimeUnit timeUnit);
	template<typename T> static T getNonAlignedValue(const void *ptr);
	template<typename T> static void setNonAlignedValue(
			void *ptr, const T &value);

	template<typename T>
	static const T* findFromArray(
			const T *target, size_t totalByteSize, const T& value);

	template<typename C>
	static ArrayByteInStream subStream(
			const C &buf, const ArrayByteInStream &in, size_t size);


	static const uint32_t VAR_SIZE_1BYTE_THRESHOLD = 128;
	static const uint32_t VAR_SIZE_4BYTE_THRESHOLD = UINT32_C(1) << 30;
	static const uint64_t VAR_SIZE_8BYTE_THRESHOLD = UINT64_C(1) << 62;

	static inline bool varSizeIs1Byte(uint8_t val);
	static inline bool varSizeIs4Byte(uint8_t val);
	static inline bool varSizeIs8Byte(uint8_t val);

	static inline uint32_t decode1ByteVarSize(uint8_t val);
	static inline uint32_t decode4ByteVarSize(uint32_t val);
	static inline uint64_t decode8ByteVarSize(uint64_t val);

	static inline uint8_t encode1ByteVarSize(uint8_t val);
	static inline uint32_t encode4ByteVarSize(uint32_t val);
	static inline uint64_t encode8ByteVarSize(uint64_t val);


	static inline uint32_t getEncodedVarSize(size_t val);

	static void writeVarSize(XArrayByteOutStream &out, size_t val);

	static void writeVarDataString(XArrayByteOutStream &out, const GSChar* elem);

	static uint32_t readVarSize(ArrayByteInStream &in);


	inline static size_t calcNullsByteSize(size_t columnNum);

private:
	ClientUtil();
};

struct ClientVersion {
	ClientVersion() throw();
	ClientVersion(int32_t majorVersion, int32_t minorVersion) throw();

	bool since(int32_t majorVersion, int32_t minorVersion) const throw();
	bool since(const ClientVersion &another) const throw();

	int32_t majorVersion_;
	int32_t minorVersion_;
};


struct SocketAddressLess :
		public std::binary_function<util::SocketAddress, util::SocketAddress, bool> {
	bool operator()(
			const util::SocketAddress &left,
			const util::SocketAddress &right) const;
};


struct SocketAddressEqual :
		public std::binary_function<util::SocketAddress, util::SocketAddress, bool> {
	bool operator()(
			const util::SocketAddress &left,
			const util::SocketAddress &right) const;
};


template<typename L>
class DynamicLockGuard {
public:
	explicit DynamicLockGuard(L *lockObject);
	~DynamicLockGuard();

private:
	DynamicLockGuard(const DynamicLockGuard&);
	DynamicLockGuard& operator=(const DynamicLockGuard&);

	L *lockObject_;
};


class TimestampUtil {
public:
	static GSTimestamp current();
	static GSTimestamp add(
			GSTimestamp timestamp, int32_t amount, GSTimeUnit timeUnit);
	static size_t format(GSTimestamp timestamp, GSChar *strBuf, size_t bufSize);
	static bool parse(const GSChar *str, GSTimestamp &timestamp);

private:
	TimestampUtil();
};


struct Properties {
	Properties(const Properties &properties);
	Properties(const GSPropertyEntry *properties, const size_t *propertyCount);

	bool getInteger(const GSChar *name, int32_t &value) const;
	bool getDouble(const GSChar *name, double &value) const;
	bool getBool(const GSChar *name, bool &value) const;
	bool getString(const GSChar *name, std::string &value) const;
	const GSChar* getString(const GSChar *name) const;

	bool getTimeoutMillis(
			const GSChar *name, int64_t &value, int64_t defaultValue) const;

	void checkExclusiveProperties(
			const GSChar *name1, const GSChar *name2,
			const GSChar *name3 = NULL) const;

	void checkExclusiveProperties(
			const GSChar *const *nameList, size_t nameCount) const;

	static int32_t timeoutToInt32Seconds(int64_t value);
	static int32_t timeoutToInt32Millis(int64_t value);

	typedef std::map<std::string, std::string> PropertyMap;
	PropertyMap propertyMap_;
};


class ClientException : public GSCommonException {
public:
	typedef ClientException ParametersBuilder;

	ClientException(UTIL_EXCEPTION_CONSTRUCTOR_ARGS_DECL) throw();
	virtual ~ClientException() throw();

	void appendParameter(const char8_t *name, const char8_t *value) throw();

	size_t getParameterCount() const throw();
	const char8_t* getParameterName(size_t index) const throw();
	const char8_t* getParameterValue(size_t index) const throw();

	void inheritParameters(const std::exception *causeInHandling) throw();
	void inheritParameters(const ClientException &cause) throw();

	static void formatMessageWithParameter(
			std::ostream &os, const util::Exception &e,
			const ClientException &params);

	template<typename T> static T generateWithParameter(
			const T &base, const std::exception *causeInHandling) throw() {
		T exception = base;
		exception.inheritParameters(causeInHandling);
		return exception;
	}

private:
	typedef std::pair<u8string, u8string> Parameter;
	typedef std::vector<Parameter> ParameterList;

	ParameterList parameterList_;
};

#define GS_CLIENT_EXCEPTION_CONVERT_CUSTOM(type, errorCode, cause, message) \
		ClientException::generateWithParameter( \
				GS_COMMON_EXCEPTION_CONVERT_CUSTOM( \
						type, errorCode, cause, message), &(cause))
#define GS_CLIENT_EXCEPTION_CONVERT(errorCode, cause, message) \
		GS_CLIENT_EXCEPTION_CONVERT_CUSTOM( \
				ClientException, errorCode, cause, message)

#define GS_CLIENT_THROW_CUSTOM_ERROR(type, errorCode, message) \
		throw GS_COMMON_EXCEPTION_CREATE_DETAIL(type, errorCode, NULL, message)
#define GS_CLIENT_RETHROW_CUSTOM_ERROR(type, defaultCode, cause, message) \
		throw GS_CLIENT_EXCEPTION_CONVERT_CUSTOM( \
				type, defaultCode, cause, message)

#define GS_CLIENT_THROW_ERROR(errorCode, message) \
		GS_CLIENT_THROW_CUSTOM_ERROR(ClientException, errorCode, message)
#define GS_CLIENT_RETHROW_ERROR(cause, message) \
		GS_CLIENT_RETHROW_CUSTOM_ERROR(ClientException, 0, cause, message)
#define GS_CLIENT_RETHROW_ERROR_CODED(errorCode, cause, message) \
		GS_CLIENT_RETHROW_CUSTOM_ERROR( \
				ClientException, errorCode, cause, message)


struct ErrorStack {
public:
	size_t getSize() const throw();

	GSResult getErrorCode(size_t index) const throw();
	size_t formatErrorName(
			size_t index, GSChar *strBuf, size_t bufSize) const throw();

	size_t formatErrorDescription(
			size_t index, GSChar *strBuf, size_t bufSize) const throw();
	size_t formatErrorMessage(
			size_t index, GSChar *strBuf, size_t bufSize) const throw();
	static void formatErrorMessage(
			std::ostream &os, const util::Exception &e, size_t index,
			bool withCode);

	size_t formatErrorLocation(
			size_t index, GSChar *strBuf, size_t bufSize,
			bool detail = false) const throw();

	size_t getErrorParameterCount(size_t stackIndex) const throw();
	size_t formatErrorParameterName(
			size_t stackIndex, size_t parameterIndex,
			GSChar *strBuf, size_t bufSize) const throw();
	size_t formatErrorParameterValue(
			size_t stackIndex, size_t parameterIndex,
			GSChar *strBuf, size_t bufSize) const throw();

	void clear() throw();
	GSResult setFromCurrentException() throw();
	static ErrorStack tryRead(
			ArrayByteInStream &in,
			const util::Exception::NamedErrorCode &defaultCode) throw();

	ClientException& getException() throw();

	static void setDetailErrorMessageEnabled(bool enabled);

private:
	typedef ClientException BaseException;

	static volatile bool detailErrorMessageEnabled_;

	BaseException exception_;
};


class RemoteException : public ClientException {
protected:
	RemoteException(UTIL_EXCEPTION_CONSTRUCTOR_ARGS_DECL) throw();
	virtual ~RemoteException() throw();
};


class StatementException : public RemoteException {
public:
	StatementException(UTIL_EXCEPTION_CONSTRUCTOR_ARGS_DECL) throw();
	virtual ~StatementException() throw();
};

#define GS_CLIENT_RETHROW_STATEMENT(cause, message) \
		GS_CLIENT_RETHROW_CUSTOM_ERROR(StatementException, 0, cause, message)


class ConnectionException : public RemoteException {
public:
	ConnectionException(UTIL_EXCEPTION_CONSTRUCTOR_ARGS_DECL) throw();
	virtual ~ConnectionException() throw();
};

#define GS_CLIENT_THROW_CONNECTION(errorCode, message) \
		GS_CLIENT_THROW_CUSTOM_ERROR(ConnectionException, errorCode, message)
#define GS_CLIENT_RETHROW_CONNECTION(cause, message) \
		GS_CLIENT_RETHROW_CUSTOM_ERROR(ConnectionException, 0, cause, message)
#define GS_CLIENT_RETHROW_CONNECTION_CODED(errorCode, cause, message) \
		GS_CLIENT_RETHROW_CUSTOM_ERROR( \
				ConnectionException, errorCode, cause, message)


class WrongNodeException : public ConnectionException {
public:
	WrongNodeException(UTIL_EXCEPTION_CONSTRUCTOR_ARGS_DECL) throw();
	virtual ~WrongNodeException() throw();
};

#define GS_CLIENT_THROW_WRONG_NODE(errorCode, message) \
		GS_CLIENT_THROW_CUSTOM_ERROR(WrongNodeException, errorCode, message)
#define GS_CLIENT_RETHROW_WRONG_NODE(cause, message) \
		GS_CLIENT_RETHROW_CUSTOM_ERROR(WrongNodeException, 0, cause, message)


class ErrorReasonFormatter {
public:
	explicit ErrorReasonFormatter(const std::exception &exception);

	std::ostream& format(std::ostream &stream) const;

private:
	const std::exception &exception_;
};

std::ostream& operator<<(
		std::ostream &stream, const ErrorReasonFormatter &formatter);


enum ClientErrorCode {
	GS_ERROR_CC_INTERNAL_ERROR = 140000,
	GS_ERROR_CC_EMPTY_PARAMETER,
	GS_ERROR_CC_ILLEGAL_PARAMETER,
	GS_ERROR_CC_UNSUPPORTED_OPERATION,
	GS_ERROR_CC_SIZE_VALUE_OUT_OF_RANGE,
	GS_ERROR_CC_ILLEGAL_PROPERTY_ENTRY,
	GS_ERROR_CC_ILLEGAL_VALUE_FORMAT,
	GS_ERROR_CC_ILLEGAL_SYMBOL_CHARACTER,
	GS_ERROR_CC_UNKNOWN_COLUMN_NAME,
	GS_ERROR_CC_UNSUPPORTED_KEY_TYPE,
	GS_ERROR_CC_UNSUPPORTED_FIELD_TYPE,
	GS_ERROR_CC_UNSUPPORTED_ROW_MAPPING,
	GS_ERROR_CC_UNKNOWN_ELEMENT_TYPE_OPTION,
	GS_ERROR_CC_UNKNOWN_GEOMETRY_OPERATOR,
	GS_ERROR_CC_UNKNOWN_AGGREGATION,
	GS_ERROR_CC_UNKNOWN_TIME_OPERATOR,
	GS_ERROR_CC_UNKNOWN_INDEX_FLAG,
	GS_ERROR_CC_UNKNOWN_FETCH_OPTION,
	GS_ERROR_CC_UNKNOWN_TIME_UNIT,
	GS_ERROR_CC_UNSUPPORTED_DEFAULT_INDEX,
	GS_ERROR_CC_BINDING_ENTRY_NOT_FOUND,
	GS_ERROR_CC_MULTIPLE_KEYS_FOUND,
	GS_ERROR_CC_COLUMN_NAME_CONFLICTED,
	GS_ERROR_CC_ILLEGAL_SCHEMA,
	GS_ERROR_CC_KEY_NOT_FOUND,
	GS_ERROR_CC_KEY_NOT_ACCEPTED,
	GS_ERROR_CC_EMPTY_ROW_FIELD,
	GS_ERROR_CC_BAD_STATEMENT,
	GS_ERROR_CC_BAD_CONNECTION,
	GS_ERROR_CC_CONNECTION_TIMEOUT,
	GS_ERROR_CC_WRONG_NODE,
	GS_ERROR_CC_MESSAGE_CORRUPTED,
	GS_ERROR_CC_PARTITION_NOT_FOUND,
	GS_ERROR_CC_ILLEGAL_PARTITION_COUNT,
	GS_ERROR_CC_CONTAINER_NOT_OPENED,
	GS_ERROR_CC_ILLEGAL_COMMIT_MODE,
	GS_ERROR_CC_TRANSACTION_CLOSED,
	GS_ERROR_CC_NO_SUCH_ELEMENT,
	GS_ERROR_CC_CONTAINER_CLOSED,
	GS_ERROR_CC_NOT_LOCKED,
	GS_ERROR_CC_RESOURCE_CLOSED,
	GS_ERROR_CC_ALLOCATION_FAILED,
	GS_ERROR_CC_RECOVERABLE_CONNECTION_PROBLEM,
	GS_ERROR_CC_RECOVERABLE_ROW_SET_LOST,
	GS_ERROR_CC_ILLEGAL_CONFIG,
	GS_ERROR_CC_DATABASE_NOT_EMPTY, 
	GS_ERROR_CC_PLATFORM_ERROR
};



class RowMapper {
	friend class Cache;

public:
	enum RowTypeCategory {
		CATEGORY_COLLECTION,
		CATEGORY_TIME_SERIES,
		CATEGORY_AGGREGATION_RESULT
	};

	enum MappingMode {
		MODE_NORMAL,
		MODE_ROWWISE_SEPARATED,
		MODE_ROWWISE_SEPARATED_V2, 
		MODE_COLUMNWISE_SEPARATED,
		MODE_AGGREGATED
	};

	static const GSType ANY_NULL_TYPE = GS_TYPE_NULL;
	static const GSType NULLABLE_MASK =
			~static_cast<GSType>((1U << (CHAR_BIT - 1)) - 1);

	struct TypeOptionMask {
		static const GSTypeOption MASK_NULLABLE =
				GS_TYPE_OPTION_NULLABLE | GS_TYPE_OPTION_NOT_NULL;
		static const GSTypeOption MASK_DEFAULT_VALUE =
				GS_TYPE_OPTION_DEFAULT_VALUE_NULL |
				GS_TYPE_OPTION_DEFAULT_VALUE_NOT_NULL;

		static const GSTypeOption MASK_GENERAL_SUPPORTED =
				MASK_NULLABLE | MASK_DEFAULT_VALUE;
		static const GSTypeOption MASK_BINDING_SUPPORTED =
				GS_TYPE_OPTION_KEY | MASK_GENERAL_SUPPORTED;
	};

	struct Tool;
	struct KeyStorage;
	struct Config;
	class InputCursor;
	class OutputCursor;
	class VarDataPool;
	class Cache;
	class Reference;

	template<GSType ElemType, bool ArrayType> struct ObjectTypeTraits;
	template<GSType ElemType, bool ArrayType> struct TypeTraits;

	template<bool A>
	struct ObjectTypeTraits<ANY_NULL_TYPE, A> {
		typedef GSValue Object;
		static Object& as(GSValue &v) { return v; }
		static const Object& as(const GSValue &v) { return v; }
	};
	typedef TypeTraits<ANY_NULL_TYPE, false> AnyNullTraits;

	template<int> struct ColumnInfoTraits;
	template<bool Const = false> class ContainerInfoRef;

public:
	RowMapper(
			RowTypeCategory rowTypeCategory, const GSBinding *binding,
			const Config &config);
	RowMapper(
			const RowMapper &srcMapper, ArrayByteInStream &schemaIn,
			const Config &config, bool columnOrderIgnorable);
	~RowMapper();

	static std::auto_ptr<RowMapper> getInstance(
			ArrayByteInStream &in, const Config &config,
			RowTypeCategory category);

	static Cache& getDefaultCache();

	static const RowMapper& getAggregationResultMapper();
	static const RowMapper& getQueryAnalysisMapper();

	void checkSchemaMatched(const RowMapper &mapper) const;

	bool matches(
			RowTypeCategory rowTypeCategory, const GSBinding *binding,
			bool general, const Config &config) const;
	bool matches(
			const RowMapper &baseMapper, ArrayByteInStream schemaIn,
			const Config &config) const;

	static size_t getDigest(
			RowTypeCategory rowTypeCategory, const GSBinding *binding,
			bool general, bool nullableAllowed);
	static size_t getDigest(
			const RowMapper &baseMapper, ArrayByteInStream schemaIn,
			const Config &config);

	GSContainerType getContainerType() const;
	void getContainerSchema(
			ContainerInfoRef<> &containerInfoRef,
			VarDataPool &varDataPool) const;
	static GSColumnInfo getColumnSchema(
			const GSBindingEntry &entry, VarDataPool *varDataPool);

	RowTypeCategory getCategory() const;
	bool isGeneral() const;
	bool isNullableAllowed() const;
	const GSBinding& getBinding() const;
	bool hasKey() const;

	GSType getElementType(int32_t columnId) const;
	bool isArray(int32_t columnId) const;
	bool hasAnyTypeColumn() const;
	bool isDefaultValueSpecified() const;

	static GSBinding importSchema(
			ArrayByteInStream &in, VarDataPool *varDataPool,
			std::vector<GSBindingEntry> *entryList,
			GSContainerType *containerType, bool anyTypeAllowed);
	void exportSchema(XArrayByteOutStream &out, const Config &config) const;

	static size_t importColumnCount(ArrayByteInStream &in);
	static void exportColumnCount(
			XArrayByteOutStream &out, size_t columnCount);

	static int32_t importKeyListBegin(
			ArrayByteInStream &in, const Config &config, size_t columnCount);
	static void importKeyListEnd(
			ArrayByteInStream &in, const Config &config, size_t columnCount,
			int32_t &keyColumnId);

	static void exportKeyListBegin(
			XArrayByteOutStream &out, const Config &config,
			const int32_t &keyColumnId);
	static void exportKeyListEnd(
			XArrayByteOutStream &out, const Config &config,
			const int32_t &keyColumnId);

	static GSColumnInfo importColumnSchema(
			ArrayByteInStream &in, const Config &config,
			VarDataPool &varDataPool);

	static void exportColumnSchema(
			XArrayByteOutStream &out, const GSBindingEntry &entry);
	static void importColumnSchema(
			ArrayByteInStream &in, const Config &config, GSBindingEntry &entry,
			std::string *nameStr, VarDataPool *varDataPool);

	int32_t resolveColumnId(const GSChar *name) const;

	static void encodeKeyGeneral(
			XArrayByteOutStream &out, MappingMode mode,
			const GSType keyType, const GSValue &value);
	void encodeKeyByObj(
			XArrayByteOutStream &out, MappingMode mode,
			const GSType *keyType, const void *keyObj) const;
	void encodeKeyByString(
			XArrayByteOutStream &out, MappingMode mode,
			const GSChar *keyString, OutputCursor *cursor) const;

	void encode(XArrayByteOutStream &out, MappingMode mode,
			const GSType *keyType, const void *keyObj,
			const void *rowObj) const;
	void encode(OutputCursor &cursor,
			const GSType *keyType, const void *keyObj,
			const void *rowObj) const;

	void encodeWithKeyString(XArrayByteOutStream &out, MappingMode mode,
			const GSChar *keyString, const void *rowObj) const;

	void decode(ArrayByteInStream &in, MappingMode mode,
			VarDataPool *varDataPool, void *rowObj) const;
	void decode(InputCursor &cursor, void *rowObj) const;

	void* extractKey(void *rowObj, KeyStorage &keyStorage) const;

	size_t getGeneralRowSize() const;

	static void encodeGeometry(XArrayByteOutStream &out, const char8_t *text, MappingMode mode);

	template<typename Alloc>
	static const GSChar* findEmptyString(Alloc &alloc);
	static const GSChar* findEmptyString(VarDataPool &pool);

	template<typename Alloc>
	static const GSChar* decodeString(ArrayByteInStream &in, Alloc &alloc);
	const GSChar* decodeString(
			ArrayByteInStream &in, VarDataPool *pool, void *rowObj) const;

	template<typename Alloc>
	static const GSChar* decodeStringV2(ArrayByteInStream &in, Alloc &alloc);
	const GSChar* decodeStringV2(
			ArrayByteInStream &in, VarDataPool *pool, void *rowObj) const;

	template<typename Alloc>
	static const GSChar* readStringWithSize(
			ArrayByteInStream &in, size_t inSize, Alloc &alloc);
	template<typename Alloc>
	static const GSChar* copyString(const GSChar *src, Alloc &alloc);
	template<typename Alloc>
	static const GSChar* copyString(
			const GSChar *src, size_t srcSize, Alloc &alloc);

	static GSType toFullType(GSType type, bool arrayUsed);
	static GSType toElementType(GSType type, bool &arrayUsed);

	static bool isArrayColumn(const GSBindingEntry &entry);

	static GSBindingEntry getEntryGeneral(
			const GSBindingEntry &src, const GSType type);

	static size_t getFieldObjectMainPartSize(
			GSType elementType, bool arrayUsed);

	template<typename Caller, typename Operator>
	static void invokeTypedOperation(
			Caller &caller, Operator &op, const GSBindingEntry &entry);

	template<typename Alloc, typename Mask>
	static GSValue copyValue(const GSValue &src,
			GSType elementType, bool arrayUsed, Alloc *alloc, const Mask&);

	template<typename Traits, typename Alloc, typename Mask, typename T>
	static GSValue copyValue(const GSValue &src, const Traits&, Alloc*,
			const Mask&, T);

	template<typename Traits, typename Alloc, typename Mask, typename E>
	static GSValue copyValue(const GSValue &src, const Traits&, Alloc *alloc,
			const Mask&, const E*);

	template<typename Traits, typename Alloc, typename Mask>
	static GSValue copyValue(const GSValue &src, const Traits&, Alloc *alloc,
			const Mask&, const GSChar *const*);

	template<typename Traits, typename Alloc, typename Mask>
	static GSValue copyValue(const GSValue &src, const Traits&, Alloc *alloc,
			const Mask&, GSBlob);

	static GSContainerType checkContainerType(GSContainerType type);
	static RowTypeCategory containerTypeToCategory(GSContainerType type);

	static bool isKeyType(GSType type);

	static bool isAny(GSType type);
	static bool isNullable(const GSBindingEntry &entry);
	static bool isTypeNullable(GSType type);
	static GSType toNullable(GSType type, bool nullable = true);
	static GSType toNonNullable(GSType type);

	static bool isOptionNullable(GSTypeOption options);
	static bool isOptionInitialValueNull(GSTypeOption options);

	static GSTypeOption filterTypeOptions(
			const GSBindingEntry &entry,
			bool anyTypeAllowed, bool nullableAllowed);
	static GSTypeOption filterNullable(
			GSTypeOption options, GSTypeOption nullableDefault,
			bool nullableAllowed, const GSChar *columnName);
	static GSTypeOption filterInitialValueNull(
			GSTypeOption options, bool nullable,
			const GSChar *columnName);

	static const ContainerInfoRef<true> toInfoRef(
			const GSContainerInfo *info, const ClientVersion &version) throw();

	struct Initializer {
	public:
		Initializer() throw();
		~Initializer();
	private:
		static size_t counter_;
	};

	inline uint32_t getVarColumnCount() const {
		return varColumnCount_;
	}
	inline size_t getNullsByteSize() const {
		return nullsByteSize_;
	}
	inline uint32_t getNullsOffset() const {
		return nullsOffset_;
	}
private:
	class GeometryEncoderHandler : public GeometryCoder::BinaryHandler {
	public:
		explicit GeometryEncoderHandler(XArrayByteOutStream &out);
		virtual void accept(const void *data, size_t size, uint32_t mode);

		XArrayByteOutStream &out_;
	};

	class GeometryDecoderHandler : public GeometryCoder::TextHandler {
	public:
		GeometryDecoderHandler(InputCursor &cursor, void *rowObj);
		virtual void accept(const char8_t *text);

		InputCursor &cursor_;
		void *const rowObj_;
		const char8_t *text_;
	};

	struct BaseCursor;

	struct ColumnIdEntry {
		std::string name_;
		int32_t id_;
	};

	enum ColumnFlag {
		COLUMN_FLAG_ARRAY = 1 << 0,
		COLUMN_FLAG_NOT_NULL = 1 << 2
	};

	typedef std::map<std::string, ColumnIdEntry> ColumnIdMap;

	static const bool ACCEPT_AGGREGATION_RESULT_COLUMN_ID;
	static const bool RESTRICT_KEY_ORDER_FIRST;

	static Cache *cache_;
	static const Config BASIC_CONFIG;
	static const RowMapper AGGREGATION_RESULT_MAPPER;
	static const RowMapper QUERY_ANALYSIS_MAPPER;

	RowMapper(
			size_t digest, RowTypeCategory rowTypeCategory, bool general,
			bool nullableAllowed);

	RowMapper(const RowMapper&);
	RowMapper& operator=(const RowMapper&);

	static size_t getDigest(const GSBindingEntry &entry);
	static size_t getDigest(const GSChar *str);

	static GSBinding checkAndCopyBinding(
			const GSBinding *src, ColumnIdMap &entryIndexMap,
			RowTypeCategory rowTypeCategory, const Config &config);
	static GSBinding createReorderedBinding(
			const RowMapper &baseMapper, ColumnIdMap &entryIndexMap,
			ArrayByteInStream &in, const Config &config,
			bool columnOrderIgnorable);

	template<GSType T, typename M>
	static const GSChar *maskNullString(const GSChar *src);

	static int32_t findRowKeyId(const GSBinding *binding);

	static void* getField(const GSBindingEntry &entry, void *rowObj);
	static const void* getField(
			const GSBindingEntry &entry, const void *rowObj);

	static void* getArrayFieldSizeAddr(
			const GSBindingEntry &entry, void *rowObj);
	static size_t getArrayFieldSize(
			const GSBindingEntry &entry, const void *rowObj);

	const RowMapper& resolveCodingMapper(const void *rowObj) const;
	const GSBindingEntry& resolveKeyEntry() const;
	const GSBindingEntry& getEntry(int32_t columnId) const;

	bool isNull(const void *rowObj, int32_t columnId) const;
	void setNull(void *rowObj, int32_t columnId, bool nullValue) const;

	uint8_t* findNulls(void *rowObj) const;
	const uint8_t* findNulls(const void *rowObj) const;

	size_t getFixedRowPartSize(bool rowIdIncluded, MappingMode mode) const;
	size_t getFixedFieldPartSize(int32_t columnId, MappingMode mode) const;

	void decodeAggregation(InputCursor &cursor, void *rowObj) const;

	static void putArraySizeInfo(
			XArrayByteOutStream &out, MappingMode mode,
			size_t elementSize, size_t elementCount);

	void encodeField(OutputCursor &cursor, int32_t columnId,
			const GSType *keyType, const void *keyObj,
			const void *rowObj, const GSBindingEntry *generalEntry = NULL) const;

	void decodeField(
			InputCursor &cursor, int32_t columnId, void *rowObj,
			void *&pendingVarData, size_t &pendingPtrArraySize,
			const GSBindingEntry *generalEntry = NULL) const;

	template<GSType ElemType, bool ArrayType>
	void clearField(const GSBindingEntry &entry, void *rowObj) const;
	void clearFieldGeneral(const GSBindingEntry &entry, void *rowObj) const;

	void* allocate(VarDataPool *pool, void *rowObj, size_t size) const;
	void deallocate(VarDataPool *pool, void *rowObj, void *ptr) const;

	void setupAccessInfo();

	template<typename Alloc, typename Mask>
	struct ValueCopier {
		ValueCopier(Alloc *alloc, const GSValue &src);

		template<typename Traits>
		void operator()(GSValue &dest, const GSBindingEntry &entry, const Traits&);

		Alloc *alloc_;
		const GSValue &src_;
	};

	template<typename Traits, typename Alloc, typename Mask,
	bool StringOrArray = Traits::STRING_FAMILY>
	struct StringOrArrayCopier {
		GSValue operator()(const GSValue &src, Alloc *alloc);
	};

	const size_t digest_;
	size_t refCount_;
	const RowTypeCategory rowTypeCategory_;
	const bool general_;
	const bool nullableAllowed_;
	ColumnIdMap columnIdMap_;
	GSBinding binding_;
	int32_t keyColumnId_;

	uint32_t varColumnCount_;
	size_t nullsByteSize_;
	uint32_t nullsOffset_;
};

namespace {
RowMapper::Initializer g_rowMapperInitializer;
}


struct RowMapper::KeyStorage {
	union {
		const GSChar *strValue_;
		int32_t intValue_;
		int64_t longValue_;
		GSTimestamp timestampValue_;
	} value_;
	std::string strStorage_;
};


struct RowMapper::BaseCursor {
	BaseCursor(const RowMapper &mapper, MappingMode mode, int32_t rowCount,
			bool rowIdIncluded, size_t topPos);

	void reset();

	template<typename StreamBase>
	void beginField(StreamBase &s);

	static size_t getVarDataTop(
			const RowMapper &mapper, MappingMode mode, int32_t rowCount,
			bool rowIdIncluded, size_t topPos);

	const RowMapper *mapper_;
	const MappingMode mode_;
	const int32_t rowCount_;
	const bool rowIdIncluded_;
	int32_t rowIndex_;
	int32_t fieldIndex_;
	const size_t topPos_;
	const size_t varDataTop_;
	size_t varDataLast_;
	size_t partialVarDataOffset_;
	const bool varDataSeparated_;
	size_t pendingPos_;
	int64_t lastRowID_;
	int64_t varDataOffset_;
};

struct RowMapper::Config {
	Config(bool anyTypeAllowed, bool nullableAllowed, bool keyExtensible);

	bool anyTypeAllowed_;
	bool nullableAllowed_;
	bool keyExtensible_;
};


class RowMapper::InputCursor {
public:
	friend class RowMapper;
	friend struct RowMapper::Tool;

	InputCursor(ArrayByteInStream &in, const RowMapper &mapper,
			MappingMode mode, int32_t rowCount, bool rowIdIncluded,
			VarDataPool *varDataPool);

	void setVarDataBaseOffset(int64_t varDataBaseOffset);

	MappingMode getMode() const;
	int32_t getRowCount() const;
	int64_t getLastRowID() const;
	bool isRowIdIncluded() const;
	bool hasNext() const;
	bool isInRange() const;
	int32_t getRowIndex() const;

	void reset();

	void getRowFixedPartAll(
			GSRowSetType resultType,
			const util::NormalXArray<uint8_t> &resultData,
			const uint8_t *&data, size_t &size) const;
	void getRowVariablePartAll(
			GSRowSetType resultType,
			const util::NormalXArray<uint8_t> &resultData,
			const uint8_t *&data, size_t &size) const;

private:
	void beginRow(const RowMapper &mapper, uint8_t *nulls);
	void endRow();
	void beginField();
	void beginVarData();
	void endVarData();

	BaseCursor base_;
	ArrayByteInStream &in_;
	VarDataPool *varDataPool_;
	int64_t varDataBaseOffset_;
};


class RowMapper::OutputCursor {
public:
	friend class RowMapper;
	friend struct RowMapper::Tool;

	OutputCursor(XArrayByteOutStream &out, const RowMapper &mapper,
			MappingMode mode, int32_t rowCount, bool rowIdIncluded = false);

	void reset();

private:
	void beginRow(
			const RowMapper &mapper, int64_t rowID, const uint8_t *nulls);
	void endRow();
	void beginField();
	void beginVarData();
	void endVarData();

	BaseCursor base_;
	XArrayByteOutStream &out_;
};


class RowMapper::VarDataPool {
public:
	VarDataPool();
	~VarDataPool();

	void* allocate(size_t size);
	void deallocate(void*);
	void clear();
	size_t getUsageSize();

private:
	static const size_t NORMAL_BLOCK_SIZE;
	struct BlockHead {
		BlockHead *prev_;
		size_t bodySize_;
	};

	union AlignmentUnit {
		void *u1_;
		int64_t u2_;
		double u3_;
	};

	VarDataPool(const VarDataPool&);
	VarDataPool& operator=(const VarDataPool&);

	BlockHead *block_;
	uint8_t *tail_;
	uint8_t *end_;
};

class RowMapper::Cache {
public:
	Cache();

	const RowMapper* resolve(
			RowTypeCategory rowTypeCategory, const GSBinding *binding,
			bool general, const Config &config);
	const RowMapper* resolve(
			const RowMapper &baseMapper, ArrayByteInStream &schemaIn,
			const Config &config, bool columnOrderIgnorable);
	const RowMapper* resolve(
			const RowMapper::ContainerInfoRef<true> &infoRef,
			const Config &config);

	const RowMapper* duplicate(const RowMapper &mapper);

	void release(const RowMapper **mapper);

private:
	Cache(const Cache&);
	Cache& operator=(const Cache&);

	typedef std::multimap<size_t, RowMapper*> EntryMap;
	typedef EntryMap::iterator EntryMapIterator;
	EntryMap entryMap_;
	util::Mutex mutex_;
};

class RowMapper::Reference {
public:
	Reference(Cache &cache, const RowMapper *mapper);
	Reference(std::auto_ptr<RowMapper> mapper);
	~Reference();

	Reference(Reference &another);
	Reference& operator=(Reference &another);

	const RowMapper* operator->() const;
	const RowMapper& operator*() const;

	const RowMapper* get() const;
	const RowMapper* release();
	Cache* getCache() const;
	void reset() throw();

private:
	Cache *cache_;
	const RowMapper *mapper_;
};

#define CLIENT_TYPE_TRAITS_PRIMITIVE( \
		upperSymbol, camelSymbol, type) \
		template<> \
		struct RowMapper::ObjectTypeTraits<GS_TYPE_##upperSymbol, false> { \
			typedef type Object; \
			static Object& as(GSValue &v) { \
					return v.as##camelSymbol; } \
			static const Object& as(const GSValue &v) { \
					return v.as##camelSymbol; } \
			template<typename Value> \
			static size_t& arraySizeOf(Value &v) { \
					UTIL_STATIC_ASSERT(sizeof(Value) == 0); \
					return *static_cast<size_t*>(NULL); } \
			template<typename Value> \
			static const size_t& arraySizeOf(const Value &v) { \
					UTIL_STATIC_ASSERT(sizeof(Value) == 0); \
					return *static_cast<size_t*>(NULL); } \
		}
#define CLIENT_TYPE_TRAITS_ARRAY( \
		upperSymbol, camelSymbol, type) \
		template<> \
		struct RowMapper::ObjectTypeTraits<GS_TYPE_##upperSymbol, true> { \
			typedef type Object; \
			static Object& as(GSValue &v) { \
					return v.asArray.elements.as##camelSymbol; } \
			static const Object& as(const GSValue &v) { \
					return v.asArray.elements.as##camelSymbol; } \
			template<typename Value> \
			static size_t& arraySizeOf(Value &v) { \
					return v.asArray.length; } \
			template<typename Value> \
			static const size_t& arraySizeOf(const Value &v) { \
					return v.asArray.length; } \
		}

CLIENT_TYPE_TRAITS_PRIMITIVE(STRING, String, const GSChar*);
CLIENT_TYPE_TRAITS_PRIMITIVE(BOOL, Bool, GSBool);
CLIENT_TYPE_TRAITS_PRIMITIVE(BYTE, Byte, int8_t);
CLIENT_TYPE_TRAITS_PRIMITIVE(SHORT, Short, int16_t);
CLIENT_TYPE_TRAITS_PRIMITIVE(INTEGER, Integer, int32_t);
CLIENT_TYPE_TRAITS_PRIMITIVE(LONG, Long, int64_t);
CLIENT_TYPE_TRAITS_PRIMITIVE(FLOAT, Float, float);
CLIENT_TYPE_TRAITS_PRIMITIVE(DOUBLE, Double, double);
CLIENT_TYPE_TRAITS_PRIMITIVE(TIMESTAMP, Timestamp, GSTimestamp);
CLIENT_TYPE_TRAITS_PRIMITIVE(GEOMETRY, Geometry, const GSChar*);
CLIENT_TYPE_TRAITS_PRIMITIVE(BLOB, Blob, GSBlob);

CLIENT_TYPE_TRAITS_ARRAY(STRING, String, const GSChar *const*);
CLIENT_TYPE_TRAITS_ARRAY(BOOL, Bool, const GSBool*);
CLIENT_TYPE_TRAITS_ARRAY(BYTE, Byte, const int8_t*);
CLIENT_TYPE_TRAITS_ARRAY(SHORT, Short, const int16_t*);
CLIENT_TYPE_TRAITS_ARRAY(INTEGER, Integer, const int32_t*);
CLIENT_TYPE_TRAITS_ARRAY(LONG, Long, const int64_t*);
CLIENT_TYPE_TRAITS_ARRAY(FLOAT, Float, const float*);
CLIENT_TYPE_TRAITS_ARRAY(DOUBLE, Double, const double*);
CLIENT_TYPE_TRAITS_ARRAY(TIMESTAMP, Timestamp, const GSTimestamp*);

template<GSType ElemType, bool ArrayType>
struct RowMapper::TypeTraits {

	static const GSType ELEMENT_TYPE = ElemType;

	static const bool ARRAY_TYPE = ArrayType;

	static const bool KEY_TYPE = !ARRAY_TYPE && (
			ELEMENT_TYPE == GS_TYPE_STRING ||
			ELEMENT_TYPE == GS_TYPE_INTEGER ||
			ELEMENT_TYPE == GS_TYPE_LONG ||
			ELEMENT_TYPE == GS_TYPE_TIMESTAMP);

	static const bool STRING_FAMILY = (
			ELEMENT_TYPE == GS_TYPE_STRING ||
			ELEMENT_TYPE == GS_TYPE_GEOMETRY);

	static const bool VAR_DATA_FAMILY = (
			ARRAY_TYPE ||
			STRING_FAMILY ||
			ELEMENT_TYPE == GS_TYPE_BLOB);

	typedef ObjectTypeTraits<ElemType, ArrayType> ObjectTraits;
	typedef typename ObjectTraits::Object Object;

	static Object& as(GSValue &v) { return ObjectTraits::as(v); }
	static const Object& as(const GSValue &v) { return ObjectTraits::as(v); }

	template<typename Value>
	static size_t& arraySizeOf(Value &v) {
		return ObjectTraits::arraySizeOf(v);
	}

	template<typename Value>
	static const size_t& arraySizeOf(const Value &v) {
		return ObjectTraits::arraySizeOf(v);
	}

	static const GSType* resolveKeyType() {
		static const GSType elementType = ElemType;
		return (KEY_TYPE ? &elementType : NULL);
	}

};

template<> struct RowMapper::ColumnInfoTraits<0> {
	struct Type {
		const GSChar *name;
		GSType type;
	};
};

template<> struct RowMapper::ColumnInfoTraits<1> {
	struct Type {
		const GSChar *name;
		GSType type;
		GSIndexTypeFlags indexTypeFlags;
	};
};

template<> struct RowMapper::ColumnInfoTraits<2> {
	typedef GSColumnInfo Type;
};

template<bool Const> class RowMapper::ContainerInfoRef {
public:
	typedef typename util::Conditional<
			Const, const GSContainerInfo*, GSContainerInfo*>::Type RefType;

	ContainerInfoRef(RefType ref, const ClientVersion &version) throw();

	ContainerInfoRef(const ContainerInfoRef &another) throw();
	ContainerInfoRef& operator=(const ContainerInfoRef &another) throw();

	static void clear(RefType ref, const ClientVersion &version) throw();
	void clear() throw();

	bool hasRef() const;
	ContainerInfoRef<true> toConst() const;

	void get(GSContainerInfo &info) const;
	void set(const GSContainerInfo &info);

	GSContainerType getType() const;
	bool isRowKeyAssigned() const;

	size_t getColumnCount() const;
	GSColumnInfo getColumnInfo(int32_t column) const;
	void setColumnInfo(int32_t column, const GSColumnInfo &columnInfo);
	void createColumnInfoList(size_t columnCount, VarDataPool &pool);

	const void* getRawColumnInfoList() const;

private:
	typedef typename util::Conditional<
			Const, const GSColumnInfo*, GSColumnInfo*>::Type ColumnInfoRefType;
	typedef typename util::Conditional<
			Const, const void*, void*>::Type RawPtrType;

	template<bool RefConst>
	static typename util::Conditional<
			RefConst, const void*, void*>::Type getColumnInfoRef(
			const ContainerInfoRef &infoRef,
			typename util::Conditional<
					RefConst, const void*, void*>::Type columnInfoList,
			int32_t column);
	size_t getColumnInfoSize() const;

	const GSTimeSeriesProperties* getTimeSeriesProperties() const;

	RefType ref_;
	ClientVersion version_;

	RawPtrType columnInfoList_;
	mutable GSTimeSeriesProperties tsProps_;
};

template<> void RowMapper::ContainerInfoRef<false>::clear() throw();

template<> void RowMapper::ContainerInfoRef<false>::set(
		const GSContainerInfo &info);

template<> void RowMapper::ContainerInfoRef<false>::setColumnInfo(
		int32_t column, const GSColumnInfo &columnInfo);

template<> void RowMapper::ContainerInfoRef<false>::createColumnInfoList(
		size_t columnCount, VarDataPool &pool);


struct ContainerKey {
	ContainerKey();
	ContainerKey toCaseSensitive(bool caseSensitive) const;

	std::vector<uint8_t> bytes_;
	bool caseSensitive_;
	bool compatible_;
};

bool operator<(const ContainerKey &key1, const ContainerKey &key2);
bool operator==(const ContainerKey &key1, const ContainerKey &key2);
std::ostream& operator<<(std::ostream &out, const ContainerKey &key);

class ContainerKeyConverter {
public:
	typedef ContainerKeyUtils::Components Components;

	ContainerKeyConverter(bool compatible, bool internalMode);

	static int64_t getPublicDatabaseId();

	bool isCompatible() const;

	ContainerKey get(
			ArrayByteInStream &in, bool caseSensitive) const;
	void put(
			XArrayByteOutStream &out,
			const ContainerKey &key, int64_t databaseId) const;

	ContainerKey compose(
			const Components &components, bool caseSensitive) const;
	void decompose(
			const ContainerKey &key, Components &components) const;

	std::string format(const ContainerKey &key) const;
	std::ostream& format(std::ostream &os, const ContainerKey &key) const;
	ContainerKey parse(const char8_t *str, bool caseSensitive) const;

	int32_t compare(
			const ContainerKey &key1, const ContainerKey &key2) const;

private:
	static const int64_t PUBLIC_DATABASE_ID;

	bool compatible_;
	bool internalMode_;
};



struct Statement {
	enum Id {
		CONNECT,
		DISCONNECT,
		LOGIN,
		LOGOUT,
		GET_PARTITION_ADDRESS,
		GET_CONTAINER,
		GET_TIME_SERIES,		
		PUT_CONTAINER,
		PUT_TIME_SERIES,		
		DROP_COLLECTION,		
		DROP_TIME_SERIES,		
		CREATE_SESSION,
		CLOSE_SESSION,
		CREATE_INDEX,
		DROP_INDEX,
		CREATE_EVENT_NOTIFICATION,
		DROP_EVENT_NOTIFICATION,
		FLUSH_LOG,
		COMMIT_TRANSACTION,
		ABORT_TRANSACTION,
		GET_ROW,
		QUERY_TQL,
		QUERY_COLLECTION_GEOMETRY_RELATED,
		QUERY_COLLECTION_GEOMETRY_WITH_EXCLUSION,
		PUT_ROW,
		PUT_MULTIPLE_ROWS,
		UPDATE_ROW_BY_ID,
		DELETE_ROW,
		DELETE_ROW_BY_ID,
		APPEND_TIME_SERIES_ROW,
		GET_TIME_SERIES_ROW,		
		GET_TIME_SERIES_ROW_RELATED,
		INTERPOLATE_TIME_SERIES_ROW,
		AGGREGATE_TIME_SERIES,
		QUERY_TIME_SERIES_TQL,		
		QUERY_TIME_SERIES_RANGE,
		QUERY_TIME_SERIES_SAMPLING,
		PUT_TIME_SERIES_ROW,		
		PUT_TIME_SERIES_MULTIPLE_ROWS,		
		DELETE_TIME_SERIES_ROW,		
		GET_CONTAINER_PROPERTIES,
		GET_MULTIPLE_ROWS,
		GET_TIME_SERIES_MULTIPLE_ROWS,		

		GET_PARTITION_CONTAINER_NAMES,
		DROP_CONTAINER,
		CREATE_MULTIPLE_SESSIONS,
		CLOSE_MULTIPLE_SESSIONS,
		EXECUTE_MULTIPLE_QUERIES,
		GET_MULTIPLE_CONTAINER_ROWS,
		PUT_MULTIPLE_CONTAINER_ROWS,
		CLOSE_ROW_SET,
		FETCH_ROW_SET,
		CREATE_TRIGGER,
		DROP_TRIGGER,

		GET_USERS,
		PUT_USER,
		DROP_USER,
		GET_DATABASES,
		PUT_DATABASE,
		DROP_DATABASE,
		PUT_PRIVILEGE,
		DROP_PRIVILEGE,

		CREATE_INDEX_DETAIL,
		DROP_INDEX_DETAIL,

		STATEMENT_ID_MAX
	};
};


struct StatementResult {
	enum Id {
		SUCCESS,
		STATEMENT_ERROR,
		NODE_ERROR,
		DENY
	};
};


class NodeConnection {
public:
	struct ClientId;
	struct Config;
	struct LoginInfo;
	struct OptionalRequest;
	class OptionalRequestSource;
	struct Heartbeat;
	static const int32_t EE_MAGIC_NUMBER;

	NodeConnection(const util::SocketAddress &address, const Config &config);
	~NodeConnection();

	static int32_t getProtocolVersion();
	int32_t getRemoteProtocolVersion() const;
	static bool isSupportedProtocolVersion(int32_t protocolVersion);
	static void setProtocolVersion(int32_t protocolVersion);
	const util::SocketAddress& getRemoteAddress() const;

	static size_t getEEHeadLength(bool ipv6Enabled);
	static size_t getRequestHeadLength(
			bool ipv6Enabled, bool firstStatement = false);
	static void fillRequestHead(
			bool ipv6Enabled, util::NormalXArray<uint8_t> &req,
			bool firstStatement = false);

	static void putStatementId(XArrayByteOutStream &reqOut, int64_t statementId,
			bool firstStatement = false);
	static bool isOptionalRequestEnabled();
	static bool isClientIdOnLoginEnabled();
	static bool isDatabaseIdEnabled();
	static void tryPutEmptyOptionalRequest(XArrayByteOutStream &reqOut);

	ArrayByteInStream executeStatement(
			Statement::Id statement, int32_t partitionId, int64_t statementId,
			util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> *resp,
			Heartbeat *heartbeat = NULL);

	ArrayByteInStream executeStatementDirect(
			int32_t statementTypeNumber, int32_t partitionId, int64_t statementId,
			util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> *resp,
			Heartbeat *heartbeat = NULL);

	void connect(
			util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp);
	void disconnect(
			util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp);

	void login(
			util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp,
			const LoginInfo &loginInfo, int64_t *databaseId);
	bool loginInternal(
			util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp,
			const LoginInfo &loginInfo, Auth::Challenge &challenge,
			size_t &lastInPos);

	void logout(
			util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp);

	void reuse(
			util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp,
			const LoginInfo &loginInfo, int64_t *databaseId);

	int64_t getHeartbeatReceiveCount() const;

	static std::string getDigest(const char8_t *password);

	static int32_t statementToNumber(Statement::Id statement);

	static int32_t getStatementNumberOffset();

	static bool isDatabaseEnabled();

private:
	static const int32_t DEFAULT_PROTOCOL_VERSION;
	static const int32_t STATEMENT_TYPE_NUMBER_V2_OFFSET;

	static const char8_t ERROR_PARAM_ADDRESS[];
	static const char8_t ERROR_PARAM_PARTITION[];

	static volatile int32_t protocolVersion_;
	static bool tcpNoDelayEnabled_;

	NodeConnection(const NodeConnection&);
	NodeConnection& operator=(const NodeConnection&);

	bool isIPV6Enabled() const;
	void readFully(void *buf, size_t length);
	util::NormalXArray<uint8_t>* processHeartbeat(
			int32_t partitionId, util::NormalXArray<uint8_t> &resp,
			util::NormalXArray<uint8_t> &heartbeatBuf, Heartbeat &heartbeat);

	void putConnectRequest(XArrayByteOutStream &reqOut);
	void acceptConnectResponse(ArrayByteInStream &respIn);

	static bool isStatementIdLarge(bool firstStatement);
	static int64_t getStatementId(
			ArrayByteInStream &respIn, bool firstStatement);

	util::Socket socket_;
	util::SocketAddress address_;
	const int64_t statementTimeoutMillis_;
	const int64_t heartbeatTimeoutMillis_;
	int32_t alternativeVersion_;
	bool alternativeVersionEnabled_;
	Auth::Mode authMode_;
	int32_t remoteProtocolVersion_;
	int64_t statementId_;
	int64_t heartbeatReceiveCount_;
	std::string authenticatedUser_;
	std::string authenticatedPasswordDigest_;
	std::string authenticatedDatabase_;
	bool responseUnacceptable_;
	bool authenticated_;
	bool ownerMode_;
	int32_t transactionTimeoutSecs_;
};


struct NodeConnection::ClientId {
public:
	ClientId();
	ClientId(const uuid_t &uuid, int64_t sessionId);

	ClientId(const ClientId &another);
	ClientId& operator=(const ClientId &another);

	void generate(int64_t sessionId);

	bool isEmpty() const;

	int64_t getSessionId() const;
	const uuid_t& getUUID() const;

private:
	uuid_t uuid_;
	int64_t sessionId_;
	bool generated_;
};


struct NodeConnection::Config {
	static const int32_t CONNECT_TIMEOUT_DEFAULT;
	static const int32_t STATEMENT_TIMEOUT_DEFAULT;
	static const int32_t HEARTBEAT_TIMEOUT_DEFAULT;

	Config();
	bool set(const Properties &properties);
	void setAlternativeVersion(int32_t alternativeVersion);

	int64_t connectTimeoutMillis_;
	int64_t statementTimeoutMillis_;
	int64_t heartbeatTimeoutMillis_;
	bool statementTimeoutEnabled_;
	int32_t alternativeVersion_;
	bool alternativeVersionEnabled_;
};


struct NodeConnection::LoginInfo {
	LoginInfo(
			const GSChar *user, const GSChar *password, bool ownerMode,
			const GSChar *clusterName, const GSChar *dbName,
			int64_t transactionTimeoutMillis, const GSChar *applicationName,
			double storeMemoryAgingSwapRate);
	void setPassword(const GSChar *password);

	std::allocator<uint8_t> alloc_;
	std::string user_;
	Auth::PasswordDigest passwordDigest_;
	std::string database_;
	bool ownerMode_;
	std::string clusterName_;
	int32_t transactionTimeoutSecs_;
	ClientId clientId_;
	std::string applicationName_;
	double storeMemoryAgingSwapRate_;
};

struct NodeConnection::OptionalRequest {
	enum Type {
		TYPE_TRANSACTION_TIMEOUT = 1,
		FOR_UPDATE = 2,
		CONTAINER_LOCK_REQUIRED = 3,
		SYSTEM_MODE = 4,
		DB_NAME = 5,
		CONTAINER_ATTRIBUTE = 6,
		ROW_INSERT_UPDATE = 7,
		STATEMENT_TIMEOUT = 10001,
		FETCH_LIMIT = 10002,
		FETCH_SIZE = 10003,
		CLIENT_ID = 11001,
		FETCH_BYTES_SIZE = 11002,
		APPLICATION_NAME = 11009,
		STORE_MEMORY_AGING_SWAP_RATE = 11010
	};

	typedef std::vector<uint8_t> ExtValue;
	typedef std::map<int32_t, ExtValue> ExtMap;

	OptionalRequest();
	OptionalRequest(
			int32_t statementTimeout, int64_t fetchLimit, int64_t fetchSize,
			const std::string &dbName);

	void putExt(int32_t type, const void *value, size_t valueSize);
	void format(XArrayByteOutStream &reqOut) const;

	int32_t transactionTimeout_;
	bool forUpdate_;
	bool containerLockRequired_;
	bool systemMode_;
	std::string dbName_;
	int32_t containerAttribute_;
	int32_t rowInsertUpdate_;
	int32_t statementTimeout_;
	int64_t fetchLimit_;
	int64_t fetchSize_;
	ClientId clientId_;
	int32_t fetchBytesSize_;
	std::auto_ptr<ExtMap> extRequestMap_;
	std::string applicationName_;
	double storeMemoryAgingSwapRate_;

private:
	struct Formatter;
};

struct NodeConnection::OptionalRequest::Formatter {
	static const int32_t RANGE_SIZE;
	static const int32_t RANGE_START_ID;

	explicit Formatter(XArrayByteOutStream &reqOut);
	void putType(int32_t type);
	void close();

	static void putBodySize(
			XArrayByteOutStream &reqOut, size_t headPos, size_t bodyPos);

	XArrayByteOutStream &reqOut_;

	int32_t lastRangeId_;
	size_t headPos_;
	size_t bodyPos_;
	size_t rangeHeadPos_;
	size_t rangeBodyPos_;
};

class NodeConnection::OptionalRequestSource {
public:
	virtual bool hasOptions() const = 0;
	virtual void putOptions(OptionalRequest &optionalRequest) const = 0;
};

struct NodeConnection::Heartbeat {
	int32_t orgStatementTypeNumber_;
	int64_t orgStatementId_;
	bool orgStatementFound_;
	std::auto_ptr<StatementException> orgException_;
	size_t orgRespPos_;
	size_t orgRespSize_;
};


class NodeConnectionPool {
public:
	NodeConnectionPool();
	~NodeConnectionPool();

	size_t getMaxSize();
	void setMaxSize(size_t maxSize);

	void add(std::auto_ptr<NodeConnection> connection);
	std::auto_ptr<NodeConnection> pull(const util::SocketAddress &address);

	std::auto_ptr<NodeConnection> resolve(
			const util::SocketAddress &address,
			util::NormalXArray<uint8_t> &req,
			util::NormalXArray<uint8_t> &resp,
			const NodeConnection::Config &config,
			const NodeConnection::LoginInfo &loginInfo,
			int64_t *databaseId,
			bool preferCache);

	void clear();

private:
	static const size_t DEFAULT_MAX_SIZE;

	typedef std::map<util::SocketAddress, std::vector<NodeConnection*>,
			SocketAddressLess> ConnectionMap;
	typedef std::deque<util::SocketAddress> AddressQueue;

	NodeConnectionPool(const NodeConnectionPool&);
	NodeConnectionPool& operator=(const NodeConnectionPool&);

	void adjustSize(size_t maxSize);
	void closeExceededConnections();

	ConnectionMap connectionMap_;
	std::deque<util::SocketAddress> addressQueue_;
	std::vector<NodeConnection*> exceededConnectionList_;
	size_t maxSize_;
	util::Mutex mutex_;
};


class ContainerHashMode {
public:
	enum Id {
		COMPATIBLE1,
		MD5
	};
};


class NodeResolver {
public:
	static const int32_t NOTIFICATION_RECEIVE_TIMEOUT;
	static const int32_t PROVIDER_UPDATE_TIMEOUT;

	class ProtocolConfig;
	class DefaultProtocolConfig;
	struct AddressConfig;

	template<typename T> class ClusterInfoEntry;
	struct ClusterInfo;

	NodeResolver(NodeConnectionPool &pool, bool passive,
			const util::SocketAddress &address,
			const NodeConnection::Config &connectionConfig,
			const ServiceAddressResolver::Config &sarConfig,
			const std::vector<util::SocketAddress> &memberList,
			const AddressConfig &addressConfig);

	void setConnectionConfig(const NodeConnection::Config &connectionConfig);
	void setNotificationReceiveTimeoutMillis(int64_t timeout);
	void setProviderTimeoutMillis(int64_t timeout);
	void setPreferableConnectionPoolSize(int32_t size);

	int32_t getPartitionCount(ClusterInfo &clusterInfo);
	ContainerHashMode::Id getContainerHashMode(ClusterInfo &clusterInfo);
	int64_t getDatabaseId(ClusterInfo &clusterInfo);
	void acceptDatabaseId(
			ClusterInfo &clusterInfo, int64_t databaseId,
			const util::SocketAddress &address);

	util::SocketAddress getMasterAddress(ClusterInfo &clusterInfo);
	util::SocketAddress getNodeAddress(
			ClusterInfo &clusterInfo, int32_t partitionId, bool backupPreferred,
			const util::SocketAddress *preferableHost = NULL);
	void getNodeAddressList(
			ClusterInfo &clusterInfo, int32_t partitionId,
			std::vector<util::SocketAddress> &addressList);

	void invalidateMaster(ClusterInfo &clusterInfo);

	void setProtocolConfig(ProtocolConfig *protocolConfig);

	void close();

	static util::SocketAddress getAddressProperties(
			const Properties &props, bool *passive,
			ServiceAddressResolver::Config &sarConfig,
			std::vector<util::SocketAddress> &memberList,
			const AddressConfig *addressConfig);

	static util::SocketAddress getNotificationProperties(
			const Properties &props, const GSChar *host,
			const bool *ipv6Expected,
			ServiceAddressResolver::Config &sarConfig,
			std::vector<util::SocketAddress> &memberList,
			const AddressConfig &addressConfig);

	static util::SocketAddress getNotificationAddress(
			const GSChar *host, const bool *ipv6Expected,
			const AddressConfig &config, const GSChar *key);

	static void parseNotificationMember(
			const GSChar *value, const bool *ipv6Expected,
			std::vector<util::SocketAddress> &memberList);

	static util::SocketAddress resolveAddress(
			const GSChar *host, const bool *ipv6Expected, const GSChar *key);

	static int getAddressFamily(const bool *ipv6Expected);

	static void makeServiceAddressResolver(
			ServiceAddressResolver &resolver,
			const ServiceAddressResolver::Config &sarConfig,
			const std::vector<util::SocketAddress> &memberList,
			const AddressConfig &addressConfig);

private:
	static const bool IPV6_ENABLED_DEFAULT;

	static const int32_t NOTIFICATION_STATEMENT_TYPE_V1;
	static const int32_t NOTIFICATION_STATEMENT_TYPE;

	static const int32_t DEFAULT_NOTIFICATION_PORT;
	static const GSChar *const DEFAULT_NOTIFICATION_ADDRESS;
	static const GSChar *const DEFAULT_NOTIFICATION_ADDRESS_V6;
	static const GSChar *const DEFAULT_SERVICE_TYPE;

	static AddressConfig DEFAULT_ADDRESS_CONFIG;
	static DefaultProtocolConfig DEFAULT_PROTOCOL_CONFIG;

	typedef std::vector<util::SocketAddress> NodeAddressList;
	typedef std::set<util::SocketAddress, SocketAddressLess> NodeAddressSet;
	typedef std::map<int32_t, NodeAddressList> NodeAddressMap;

	NodeResolver(const NodeResolver&);
	NodeResolver& operator=(const NodeResolver&);

	void prepareConnectionAndClusterInfo(
			ClusterInfo &clusterInfo, size_t startTrialCount);
	void updateNotificationMember();
	bool updateConnectionAndClusterInfo(ClusterInfo &clusterInfo);
	void updateMasterInfo(ClusterInfo &clusterInfo);
	void invalidateMasterInternal(ClusterInfo &clusterInfo, bool inside);
	void releaseMasterCache(bool forceClose);
	void applyMasterCacheCounter(ClusterInfo &clusterInfo);

	void acceptClusterInfo(
			ClusterInfo &clusterInfo, const int32_t *partitionCount,
			const ContainerHashMode::Id *hashMode, const int64_t *databaseId,
			const util::SocketAddress &address, bool byConnection);
	template<typename T> void acceptClusterInfoEntry(
			ClusterInfo &clusterInfo, ClusterInfoEntry<T> &entry,
			const char8_t *name, const T &value,
			const util::SocketAddress &address,
			bool byConnection, bool checkOnly);

	const std::vector<util::SocketAddress>* getNodeAddressList(
			ClusterInfo &clusterInfo, int32_t partitionId, bool backupPreferred,
			size_t startTrialCount, bool allowEmpty);
	size_t getInetAddressSize() const;
	static ContainerHashMode::Id decodeContainerHashMode(
			ArrayByteInStream &in);
	void decodeSocketAddress(
			ArrayByteInStream &in, util::SocketAddress &socketAddress);

	void updateConnectionPoolSize();

	static const GSChar* findLastChar(
			const GSChar *begin, const GSChar *end, int32_t ch);

	NodeConnectionPool &pool_;
	bool ipv6Enabled_;
	util::SocketAddress notificationAddress_;
	util::SocketAddress masterAddress_;
	NodeConnection::Config connectionConfig_;
	std::auto_ptr<NodeConnection> masterConnection_;
	util::NormalXArray<uint8_t> req_;
	util::NormalXArray<uint8_t> resp_;
	int64_t notificationReceiveTimeoutMillis_;
	int64_t providerTimeoutMillis_;
	uint64_t masterCacheCounter_;
	volatile size_t connectionTrialCounter_;
	bool connectionFailedPreviously_;
	NodeAddressSet cachedAddressSet_;
	NodeAddressMap nodeAddressMap_;
	size_t preferableConnectionPoolSize_;
	std::allocator<uint8_t> alloc_;
	ServiceAddressResolver serviceAddressResolver_;
	std::pair<uint32_t, uint32_t> providerCheckInterval_;
	util::Random random_;
	int32_t lastSelectedMember_;
	bool alwaysMaster_;
	ProtocolConfig *protocolConfig_;
	util::Mutex mutex_;
};

class NodeResolver::ProtocolConfig {
public:
	virtual ~ProtocolConfig();
	virtual int32_t getNotificationStatementType() = 0;
	virtual int32_t getNormalStatementType(Statement::Id statement) = 0;
};

class NodeResolver::DefaultProtocolConfig : public ProtocolConfig {
public:
	virtual ~DefaultProtocolConfig();
	virtual int32_t getNotificationStatementType();
	virtual int32_t getNormalStatementType(Statement::Id statement);
};

struct NodeResolver::AddressConfig {
	AddressConfig();

	int32_t notificationPort_;
	const GSChar *notificationAddress_;
	const GSChar *notificationAddressV6_;
	const GSChar *serviceType_;
	bool alwaysMaster_;
};

template<typename T>
class NodeResolver::ClusterInfoEntry {
public:
	ClusterInfoEntry();

	const T* get() const;

	bool tryAccept(
			const T *value, const util::SocketAddress &address,
			bool byConnection, bool checkOnly);
	bool invalidate();

private:
	T value_;
	bool acceptedByAny_;
	bool acceptedByConnection_;
	util::SocketAddress acceptedAddress_;
};

struct NodeResolver::ClusterInfo {
	explicit ClusterInfo(const NodeConnection::LoginInfo &loginInfo);

	bool invalidate();
	bool acceptMasterInvalidation(uint64_t masterCacheCounter);

	const int32_t* getPartitionCount() const;
	const ContainerHashMode::Id* getHashMode() const;
	const int64_t* getDatabaseId() const;

	void setPartitionCount(const int32_t *partitionCount);

	NodeConnection::LoginInfo loginInfo_;
	ClusterInfoEntry<int32_t> partitionCount_;
	ClusterInfoEntry<ContainerHashMode::Id> hashMode_;
	ClusterInfoEntry<int64_t> databaseId_;
	uint64_t lastMasterCacheCounter_;
};


class GSInterceptorManager;


class GridStoreChannel {
public:
	struct Config {
		Config();
		bool set(Properties properties);
		int64_t getFailoverTimeoutMillis(util::Mutex &mutex);
		int64_t getFailoverRetryIntervalMillis(util::Mutex &mutex);
		NodeConnection::Config getConnectionConfig(util::Mutex &mutex);

		NodeConnection::Config connectionConfig_;
		int64_t failoverTimeoutMillis_;
		int64_t failoverRetryIntervalMillis_;
		int64_t notificationReceiveTimeoutMillis_;
		int32_t maxConnectionPoolSize_;
	};

	struct Key {
		Key();

		bool passive_;
		util::SocketAddress address_;
		std::string clusterName_;

		std::string providerURL_;
		int64_t providerTimeoutMillis_;
		ServiceAddressResolver::Config sarConfig_;
		std::vector<util::SocketAddress> memberList_;
	};

	struct KeyLess {
		bool operator()(const Key &key1, const Key &key2) const;
	};

	struct ConnectionId;
	struct LocalConfig;
	struct Source;
	struct Context;
	class ContextQueryOptions;

	struct LocatedSchema;
	struct SessionInfo;
	class ContainerCache;

	static bool v1ProtocolCompatibleSpecified_;
	static bool v1ProtocolCompatible_;
	static bool v10ResourceCompatible_;
	static bool v15TSPropsCompatible_;
	static bool v15DDLCompatible_;
	static bool v20AffinityCompatible_;
	static bool v21StatementIdCompatible_;
	static bool v40QueryCompatible_;
	static bool v40ContainerHashCompatible_;
	static bool v40SchemaCompatible_;

	GridStoreChannel(const Config &config, const Source &source);

	NodeConnectionPool& getConnectionPool();
	GSInterceptorManager* getInterceptorManager() throw();
	void apply(const Config &config);
	int64_t getFailoverTimeoutMillis(const Context &context);
	void setStatementRetryMode(int32_t statementRetryMode);
	void setMonitoring(bool monitoring);
	bool isMonitoring();

	void initializeRequestBuffer(
			util::NormalXArray<uint8_t> &req,
			size_t capacity = INITIAL_BUFFER_SIZE);
	void initializeResponseBuffer(
			util::NormalXArray<uint8_t> &resp,
			size_t capacity = INITIAL_BUFFER_SIZE);

	XArrayByteOutStream getRequestOutStream(
			util::NormalXArray<uint8_t> &initializedReq);

	void clearContext(Context &context, bool doClose);
	void applyPartitionId(Context &context, int32_t partitionId);
	void checkActiveConnection(
			const Context &context, int32_t partitionId,
			const ConnectionId &connectionId);

	ArrayByteInStream executeStatement(
			Context &context, Statement::Id statement, int64_t statementId,
			util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp);

	void invalidateMaster(Context &context);
	void checkContextAvailable(const Context &context);

	int64_t getDatabaseId(Context &context);

	void getNodeAddressList(
			Context &context, int32_t partitionId,
			std::vector<util::SocketAddress> &addressList);

	int32_t getPartitionCount(Context &context);

	int32_t resolvePartitionId(
			Context &context, const ContainerKey &containerKey,
			const ContainerKeyConverter &keyConverter);
	static int32_t calculatePartitionId(
			const ContainerKey &containerKey,
			const ContainerKeyConverter &keyConverter,
			ContainerHashMode::Id hashMode, int32_t partitionCount);
	static int32_t calculatePartitionId(
			const GSChar *containerName, ContainerHashMode::Id hashMode,
			int32_t partitionCount);

	static bool isMasterResolvableByConnection();
	static ContainerHashMode::Id getDefaultContainerHashMode();
	static int32_t statementToNumber(Statement::Id statement);
	static bool isRecoverableError(GSResult result) throw();

private:
	static const int32_t FAILOVER_TIMEOUT_DEFAULT;
	static const int32_t FAILOVER_RETRY_INTERVAL;
	static const size_t INITIAL_BUFFER_SIZE;
	static const size_t MAPPER_CACHE_SIZE;

	struct ResolverExecutor;
	struct ResolverExecutorScope;

	GridStoreChannel(const GridStoreChannel&);
	GridStoreChannel& operator=(const GridStoreChannel&);

	void executeResolver(Context &context, ResolverExecutor &executor);

	void updateConnection(Context &context, bool preferConnectionPool);

	static bool isConnectionDependentStatement(Statement::Id statement);
	static bool isContainerKeyComposed();

	NodeConnectionPool pool_;
	Config config_;
	int32_t statementRetryMode_;
	util::Atomic<bool> monitoring_;
	const Key key_;
	NodeResolver nodeResolver_;
	size_t reqHeadLength_;
	GSInterceptorManager *interceptorManager_;
	util::Mutex mutex_;
};

struct GridStoreChannel::ConnectionId {
public:
	ConnectionId();

	bool isEmpty() const;
	const util::SocketAddress& getAddress() const;

	ConnectionId newId(const util::SocketAddress &address) const;
	bool matches(const ConnectionId &connectionId) const;

private:
	uint64_t value_;
	util::SocketAddress address_;
};

struct GridStoreChannel::LocalConfig {
	LocalConfig();

	void set(const Properties &properties);

	int64_t failoverTimeoutMillis_;
	int64_t transactionTimeoutMillis_;
	int32_t fetchBytesSize_;
	int32_t containerCacheSize_;
};


struct GridStoreChannel::Source {
	friend class GridStoreChannel;

	Source();
	~Source();

	const Key& getKey() const;
	NodeResolver::ClusterInfo createClusterInfo() const;
	Context createContext() const;

	void set(const Properties &properties);

	static double resolveStoreMemoryAgingSwapRate(
			const Properties &properties);

	Key key_;
	int32_t partitionCount_;
	LocalConfig localConfig_;
	NodeConnection::LoginInfo loginInfo_;
	GSInterceptorManager *interceptorManager_;
};


struct GridStoreChannel::Context {
public:
	friend class GridStoreChannel;
	friend struct ResolverExecutorScope;

	~Context();

	Context(const Context &another);
	Context& operator=(const Context &another);

	int64_t getTransactionTimeoutMillis() const;
	ContextQueryOptions bindQueryOptions(
			const ContextQueryOptions *source) const;
	int32_t getFailoverCount() const;

	util::NormalXArray<uint8_t>& getRequestBuffer();
	util::NormalXArray<uint8_t>& getResponseBuffer();

	const uuid_t& getSessionUUID() const;
	int64_t generateSessionId();
	NodeConnection::ClientId generateClientId();

	ConnectionId getLastConnectionId() const;
	ContainerCache* getContainerCache();

	void setPreferableHost(
			int32_t partitionId, const util::SocketAddress *host);
	const util::SocketAddress* getPreferableHost(int32_t partitionId) const;

	const ContainerKeyConverter& getKeyConverter() const;

private:
	typedef std::pair<ConnectionId, NodeConnection*> ConnectionEntry;
	typedef std::map<util::SocketAddress, ConnectionEntry,
			SocketAddressLess> ConnectionMap;
	typedef std::map<int32_t, util::SocketAddress> PreferableHostMap;

	Context(
			const LocalConfig &localConfig,
			const NodeConnection::LoginInfo &loginInfo,
			const NodeResolver::ClusterInfo &clusterInfo);

	LocalConfig localConfig_;
	int32_t partitionId_;
	ConnectionEntry lastConnection_;
	int64_t lastHeartbeatCount_;
	ConnectionMap activeConnections_;
	NodeConnection::LoginInfo loginInfo_;
	NodeResolver::ClusterInfo clusterInfo_;
	bool closed_;

	int32_t failoverCount_;

	util::NormalXArray<uint8_t> req_;
	util::NormalXArray<uint8_t> resp_;
	int64_t lastSessionId_;
	ConnectionId connectionId_;
	std::auto_ptr<ContainerCache> containerCache_;
	PreferableHostMap preferableHosts_;

	ResolverExecutor *resolverExecutor_;

	ContainerKeyConverter keyConverter_;
};

class GridStoreChannel::ContextQueryOptions :
		public NodeConnection::OptionalRequestSource {
public:
	ContextQueryOptions(
			const LocalConfig &localConfig, const ContextQueryOptions *source);

	bool hasOptions() const;
	void putOptions(NodeConnection::OptionalRequest &optionalRequest) const;

private:
	const LocalConfig &localConfig_;
	const ContextQueryOptions *source_;
};

struct GridStoreChannel::LocatedSchema {
public:
	friend class ContainerCache;

	LocatedSchema();

	const RowMapper* getMapper() const;
	int64_t getContainerId() const;
	int32_t getVersionId() const;

private:
	LocatedSchema(
			uint64_t schemaCacheId,
			const RowMapper *mapper, int64_t containerId, int32_t versionId);

	uint64_t schemaCacheId_;
	const RowMapper *mapper_;
	int64_t containerId_;
	int32_t versionId_;
};

struct GridStoreChannel::SessionInfo {
public:
	friend class ContainerCache;

	SessionInfo();
	SessionInfo(
			uint64_t sessionCacheId, int32_t partitionId, int64_t containerId,
			int64_t sessionId, int64_t lastStatementId);

	int32_t getPartitionId() const;
	int64_t getContainerId() const;
	int64_t getSessionId() const;
	int64_t getLastStatementId() const;

private:
	typedef std::pair<int32_t, int64_t> Key;

	uint64_t sessionCacheId_;
	Key key_;
	int64_t sessionId_;
	int64_t lastStatementId_;
};

class GridStoreChannel::ContainerCache {
public:
	explicit ContainerCache(int32_t cacheSize);
	~ContainerCache();

	const LocatedSchema* findSchema(
			const ContainerKey &normalizedContainerKey,
			const GSBinding *binding,
			const GSContainerType *containerType,
			bool general, const RowMapper::Config &config) const;
	void cacheSchema(
			const ContainerKey&normalizedContainerKey,
			const RowMapper &mapper, int64_t containerId, int32_t versionId);
	bool removeSchema(const ContainerKey &normalizedContainerKey);

	bool takeSession(
			int32_t partitionId, int64_t containerId, SessionInfo &sessionInfo);
	bool cacheSession(
			int32_t partitionId, int64_t containerId,
			int64_t sessionId, int64_t lastStatementId,
			SessionInfo &oldSessionInfo);
	void takeAllSessions(std::vector<SessionInfo> &sessionList);

private:
	ContainerCache(const ContainerCache&);
	ContainerCache& operator=(const ContainerCache&);

	typedef std::map<uint64_t, ContainerKey> SchemaNameMap;
	typedef std::map<ContainerKey, LocatedSchema> SchemaCache;
	typedef std::map<uint64_t, SessionInfo::Key> SessionKeyMap;
	typedef std::map<SessionInfo::Key, SessionInfo> SessionCache;

	int32_t cacheSize_;

	SchemaNameMap schemaNameMap_;
	SchemaCache schemaCache_;
	uint64_t lastSchemaCacheId_;

	SessionKeyMap sessionKeyMap_;
	SessionCache sessionCache_;
	uint64_t lastSessionCacheId_;
};

struct GridStoreChannel::ResolverExecutor {
	enum Command {
		COMMAND_GET_DATABASE_ID,
		COMMAND_GET_ADDRESS_LIST,
		COMMAND_GET_PARTITION_COUNT,
		COMMAND_RESOLVE_PARTITION_ID
	};

	explicit ResolverExecutor(Command command);
	void execute(
			NodeResolver &resolver, NodeResolver::ClusterInfo &clusterInfo);

	Command command_;
	const ContainerKey *containerKey_;
	const ContainerKeyConverter *keyConverter_;
	std::vector<util::SocketAddress> *addressList_;
	int32_t partitionId_;
	int32_t partitionCount_;
	int64_t databaseId_;
};

struct GridStoreChannel::ResolverExecutorScope {
public:
	ResolverExecutorScope(Context &context, ResolverExecutor &executor);
	~ResolverExecutorScope();

private:
	Context &context_;
};



class GSResourceType {
public:
	enum Id {
		GRID_STORE_FACTORY,
		GRID_STORE,
		CONTAINER,
		QUERY,
		ROW_SET,
		AGGREGATION_RESULT,
		ROW,
		ROW_KEY_PREDICATE,
		PARTITION_CONTROLLER,

		TYPE_ID_MAX
	};

	template<typename T>
	struct Resolver {
		static const Id TYPE =
				util::IsSame<T, GSGridStoreFactory>::VALUE ?
						GRID_STORE_FACTORY :
				util::IsSame<T, GSGridStore>::VALUE ? GRID_STORE :
				util::IsSame<T, GSContainer>::VALUE ? CONTAINER :
				util::IsSame<T, GSQuery>::VALUE ? QUERY :
				util::IsSame<T, GSRowSet>::VALUE ? ROW_SET :
				util::IsSame<T, GSAggregationResult>::VALUE ?
						AGGREGATION_RESULT :
				util::IsSame<T, GSRow>::VALUE ? ROW :
				util::IsSame<T, GSRowKeyPredicate>::VALUE ?
						ROW_KEY_PREDICATE :
				util::IsSame<T, GSPartitionController>::VALUE ?
						PARTITION_CONTROLLER :
				TYPE_ID_MAX;
	};
};


struct GSResourceHeader {
public:
	GSResourceHeader(
			GSResourceType::Id type, void *parentResource,
			GSInterceptorManager *interceptorManager);
	~GSResourceHeader();

	static bool getType(
			const void *resource, GSResourceType::Id &type) throw();
	static bool checkType(
			const void *resource, GSResourceType::Id type) throw();

	template<typename T>
	static GSResourceType::Id getStaticType(T *resource) throw();
	template<typename T>
	static GSResourceType::Id getStaticType(T **resource) throw();

	static ErrorStack* findErrorStack(
			void *resource, util::Mutex *&mutex) throw();
	static GSInterceptorManager* findInterceptorManager(
			void *resource) throw();
	static void findStoreOrFactory(
			void *resource, GSGridStore *&store,
			GSGridStoreFactory *&factory) throw();

	static void clearLastError(void *resource) throw();
	static GSResult setCurrentException(void *resource) throw();

#ifdef GS_CLIENT_UNIT_TEST
	static int64_t getResourceCount(GSResourceType::Id type);
#endif

private:
	static const uint32_t RESOURCE_TYPE_MASK;
	static const uint32_t FLAG_INTERCEPTABLE;

	static bool resolveInterceptable(
			GSResourceType::Id type, void *parentResource,
			GSInterceptorManager *interceptorManager);

	GSResourceType::Id getType() const throw();
	bool isInterceptable() const throw();

	static uint32_t maskType(
			GSResourceType::Id type, bool interceptable) throw();
	static GSResourceType::Id unmaskType(uint32_t maskedType) throw();

	const uint32_t maskedType_;

#ifdef GS_CLIENT_UNIT_TEST
	static int64_t resourceCountList_[];
#endif
};


class GSInterceptor {
public:
	typedef uint32_t Id;

	class FunctionInfo;
	class Parameter;
	class ParameterList;

	explicit GSInterceptor(GSInterceptorManager &manager);
	virtual ~GSInterceptor();

	Id getId() const;

	virtual bool isActivated(GSResourceType::Id type);

	virtual bool start(
			const FunctionInfo &funcInfo, const ParameterList &args,
			const Parameter &ret);

	virtual void finish(
			const FunctionInfo &funcInfo, const ParameterList &args);

private:

	GSInterceptor(const GSInterceptor&);
	GSInterceptor& operator=(const GSInterceptor&);

	GSInterceptorManager &manager_;
	Id id_;
};

class GSInterceptor::FunctionInfo {
public:
	FunctionInfo(const GSChar *rawName, GSResourceType::Id resourceType);

	const GSChar* getRawName() const;
	GSResourceType::Id getResourceType() const;

	bool matchName(const GSChar *name) const;
	void formatName(std::ostream &stream) const;

	bool isEmpty() const;
	static FunctionInfo emptyInfo();

private:
	static const GSChar* findMinorSuffix(const GSChar *rawName);

	const GSChar *rawName_;
	GSResourceType::Id resourceType_;
};

std::ostream& operator<<(
		std::ostream &stream, const GSInterceptor::FunctionInfo &funcInfo);

class GSInterceptor::Parameter {
public:
	explicit Parameter(const GSChar *name = NULL);

	const GSChar* getName() const;

	bool isResource() const;
	GSResourceType::Id getResourceType() const;
	void* findResource() const;

	bool isString() const;
	void setNoString();

	bool isSize() const;
	size_t& getSize() const;
	size_t*& getSizePtr() const;

	util::FalseType with(const util::FalseType&) const {
		return util::FalseType();
	}

	template<typename T> Parameter with(T &value) const {
		Parameter param = *this;
		param.typeInfo_ = TypeInfo::create<T>();
		param.storage_ = &value;
		return param;
	}

	template<typename T> T& get() const {
		typeInfo_.check(TypeInfo::create<T>());
		return *static_cast<T*>(storage_);
	}

	void formatValue(std::ostream &os) const;

private:
	struct TypeInfo {
		typedef void (*CheckerFunc)();
		typedef void (*FormatterFunc)(std::ostream&, void*);

		TypeInfo();

		void check(const TypeInfo &another) const;

		template<typename T>
		static TypeInfo create() {
			UTIL_STATIC_ASSERT(
					util::IsPointer<T>::VALUE ||
							(std::numeric_limits<T>::is_specialized &&
							!util::IsSame<T, bool>::VALUE));
			typedef typename util::Conditional<
					util::IsPointer<T>::VALUE, void*, T>::Type FormatterType;
			CheckerFunc sizeCheckerFunc = &checkerFunc<size_t>;

			TypeInfo info;
			info.resourceType_ = getResourceType(static_cast<T*>(NULL));
			info.checkerFunc_ = &checkerFunc<T>;
			info.formatterFunc_ = &formatterFunc<FormatterType>;
			info.forString_ = (util::IsSame<const GSChar*, T>::VALUE);
			info.forSize_ = (info.checkerFunc_ == sizeCheckerFunc);
			return info;
		}

		template<typename R>
		static GSResourceType::Id getResourceType(R**) {
			return GSResourceType::Resolver<R>::TYPE;
		}

		template<typename T>
		static GSResourceType::Id getResourceType(T*) {
			return GSResourceType::TYPE_ID_MAX;
		}

		template<typename T>
		static void checkerFunc() {}

		template<typename V>
		static void formatterFunc(std::ostream &os, void *ptr) {
			UTIL_STATIC_ASSERT(
					(!util::IsPointer<V>::VALUE || util::IsSame<void*, V>::VALUE));
			typedef typename util::Conditional<
					std::numeric_limits<V>::is_signed, int64_t, uint64_t>::Type
					IntType;
			typedef typename util::Conditional<
					std::numeric_limits<V>::is_integer, IntType, V>::Type
					FilteredType;
			if (util::IsPointer<V>::VALUE) {
				os << "0x";
			}
			os << static_cast<FilteredType>(*static_cast<V*>(ptr));
		}

		static void stringFormatterFunc(std::ostream &os, void *ptr) {
			const GSChar *value = *static_cast<const GSChar**>(ptr);
			if (value == NULL) {
				os << "(null)";
			}
			else {
				os << value;
			}
		}

		GSResourceType::Id resourceType_;
		CheckerFunc checkerFunc_;
		FormatterFunc formatterFunc_;
		bool forString_;
		bool forSize_;
	};

	const GSChar *name_;
	TypeInfo typeInfo_;
	void *storage_;
};

class GSInterceptor::ParameterList {
public:
	template<
			typename P1, typename P2, typename P3, typename P4,
			typename P5, typename P6, typename P7, typename P8,
			typename P9, typename P10>
	ParameterList(
			const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4,
			const P5 &p5, const P6 &p6, const P7 &p7, const P8 &p8,
			const P9 &p9, const P10 &p10, ...);

	size_t getSize() const;
	const Parameter& at(size_t index) const;

private:
	enum {
		MAX_PARAMETER_COUNT = 8
	};

	typedef Parameter BaseList[MAX_PARAMETER_COUNT];

	template<typename P>
	static void checkTooManyArguments(const P&);

	void add(const Parameter &param);
	void add(const util::FalseType&);

	BaseList baseList_;
	size_t size_;
};

class GSInterceptorManager {
public:
	class Scope;

	typedef GSInterceptor::FunctionInfo FunctionInfo;
	typedef GSInterceptor::Parameter Parameter;
	typedef GSInterceptor::ParameterList ParameterList;
	typedef GSInterceptor::Id InterceptorId;

	GSInterceptorManager();

	bool start(
			const FunctionInfo &funcInfo, const ParameterList &args,
			const Parameter &ret, InterceptorId &endId);
	void finish(
			const FunctionInfo &funcInfo, const ParameterList &args,
			InterceptorId endId);

	bool filterByResourceType(GSResourceType::Id type);

	void activate(InterceptorId id, bool enabled);

private:
	friend class GSInterceptor;

	enum {
		MAX_INTERCEPTOR_COUNT = 3
	};

	struct Entry {
		Entry();

		InterceptorId id_;
		GSInterceptor *interceptor_;
		bool enabled_;
	};

	class CheckerScope {
	public:
		CheckerScope();
		~CheckerScope();

	private:
		static void abortByResursiveAccess();

		static void (*const volatile abortByResursiveAccessFunc_)();
		static UTIL_THREAD_LOCAL size_t counter_;
	};

	typedef Entry EntryList[MAX_INTERCEPTOR_COUNT];

	InterceptorId add(GSInterceptor &interceptor);
	void remove(InterceptorId id);

	void getActiveEntryList(
			util::LockGuard<util::Mutex>&, GSResourceType::Id type,
			EntryList &entryList, InterceptorId &count);

	EntryList entryList_;
	InterceptorId entryCount_;

	util::Mutex mutex_;
};

class GSInterceptorManager::Scope {
public:
	Scope() throw();
	~Scope();

	bool set(
			GSInterceptorManager &manager, const FunctionInfo &funcInfo,
			const ParameterList &args, const Parameter &ret) throw();

private:
	void setCurrentException(const ParameterList &args) throw();

	struct Data {
		Data(
				GSInterceptorManager &manager, const FunctionInfo &funcInfo,
				const ParameterList &args);

		GSInterceptorManager &manager_;
		FunctionInfo funcInfo_;
		ParameterList args_;
		InterceptorId endId_;
	};

	UTIL_UNIQUE_PTR<Data> data_;
};


struct GSGridStoreFactoryTag {
public:
	friend struct GSResourceHeader;

	GSGridStoreFactoryTag() throw();
	~GSGridStoreFactoryTag();

	static bool isAlive() throw();

	static GSGridStoreFactory& getDefaultFactory() throw();
	static void close(GSGridStoreFactory **factory, bool allRelated) throw();

	GSGridStore* getGridStore(
			const GSPropertyEntry *properties, const size_t *propertyCount);
	void setProperties(
			const GSPropertyEntry *properties, const size_t *propertyCount,
			bool forInitial = false);

	struct Initializer {
	public:
		Initializer() throw();
		~Initializer();

	private:
		static size_t counter_;
	};

private:
	typedef std::map<GridStoreChannel::Key, GridStoreChannel*,
			GridStoreChannel::KeyLess> ChannelMap;

	class ConfigLoader;
	struct Data;

	GSGridStoreFactoryTag(const GSGridStoreFactory&);
	GSGridStoreFactory& operator=(const GSGridStoreFactory&);

	void prepareConfigFile() throw();

	void setPropertiesInternal(
			util::LockGuard<util::Mutex> &guard, bool forInitial,
			const GSPropertyEntry *properties, const size_t *propertyCount);

	void setLoggingProperties(
			util::LockGuard<util::Mutex> &guard, const Properties &properties,
			bool forInitial);

	void setMonitoring(
			util::LockGuard<util::Mutex>&, bool monitoring);

	GSResourceHeader resourceHeader_;
	std::auto_ptr<Data> data_;

	static GSGridStoreFactory *defaultFactory_;
};

class GSGridStoreFactoryTag::ConfigLoader {
public:
	ConfigLoader();

	bool isPrepared() const;

	void applyFactoryConfig(Properties &props);
	void applyStoreConfig(Properties &props);

	void handleConfigError(std::exception&) throw();

private:
	typedef Properties::PropertyMap PropertyMap;

	void prepare();

	static void applyConfig(const Properties *src, Properties &dest);

	void acceptFileData(std::string &buf, bool eof, uint64_t &lineNumber);
	void acceptFileLine(const char8_t *line, size_t size);
	void acceptProperty(const char8_t *name, const char8_t *value);

	static std::string unescape(const std::string &src);

	static const char8_t CONFIG_FILE_NAME[];

	bool configFileEnabled_;
	bool errorOccurred_;
	bool prepared_;
	UTIL_UNIQUE_PTR<ErrorStack> initialError_;
	UTIL_UNIQUE_PTR<Properties> factoryProps_;
	UTIL_UNIQUE_PTR<Properties> storeProps_;
};

struct GSGridStoreFactoryTag::Data {
	Data();

	void setUpInterceptors();

	ChannelMap channelMap_;
	GridStoreChannel::Config channelConfig_;
	ErrorStack stack_;
	GSInterceptorManager interceptorManager_;
	bool monitoring_;
	ConfigLoader configLoader_;

	UTIL_UNIQUE_PTR<GSInterceptor> callLogger_;

	util::Mutex mutex_;
};

namespace {
GSGridStoreFactory::Initializer g_gridStoreFactoryInitializer;
}


struct GSGridStoreTag {
public:
	friend struct GSResourceHeader;

	enum ContainerPropertyKey {
		CONTAINER_PROPERTY_ID,
		CONTAINER_PROPERTY_SCHEMA,
		CONTAINER_PROPERTY_INDEX,
		CONTAINER_PROPERTY_EVENT_NOTIFICATION,
		CONTAINER_PROPERTY_TRIGGER,
		CONTAINER_PROPERTY_ATTRIBUTE,
		CONTAINER_PROPERTY_INDEX_DETAIL,

		CONTAINER_PROPERTY_KEY_MAX
	};

	class ContainerPropertyKeySet {
	public:
		typedef ContainerPropertyKeySet Keys;

		static const ContainerPropertyKeySet KEYS_DEFAULT;
		static const ContainerPropertyKeySet KEYS_COMPATIBLE_TRIGGER;
		static const ContainerPropertyKeySet KEYS_COMPATIBLE_INDEX;
		static const ContainerPropertyKeySet KEYS_FOR_OBJECT;

		ContainerPropertyKeySet(
				ContainerPropertyKey key1 = CONTAINER_PROPERTY_KEY_MAX,
				ContainerPropertyKey key2 = CONTAINER_PROPERTY_KEY_MAX,
				ContainerPropertyKey key3 = CONTAINER_PROPERTY_KEY_MAX);

		static ContainerPropertyKeySet all();

		ContainerPropertyKeySet merge(bool exclusive, const Keys &keys) const;

		uint32_t size() const;

		ContainerPropertyKey begin() const;
		ContainerPropertyKey next(ContainerPropertyKey lastKey) const;

	private:
		uint32_t flags_;
	};

	struct ContainerIdInfo {
		ContainerIdInfo();

		int32_t versionId_;
		int64_t containerId_;
		ContainerKey remoteKey_;
		const GSChar *remoteName_;
	};

	static const GSChar *const CONTEXT_CONTROLLER_NAME;

	GSGridStoreTag(
			GridStoreChannel &channel, GridStoreChannel::Context context);
	~GSGridStoreTag();

	static void setPathKeyOperationEnabled(bool enabled);

	static void close(GSGridStore **store, bool allRelated) throw();

	void createReference(void *resource);
	static void removeReference(GSGridStore *&store, void *resource);

	GridStoreChannel& getChannel();
	GridStoreChannel::Context& getContext();
	util::NormalXArray<uint8_t>& getRequestBuffer();
	util::NormalXArray<uint8_t>& getResponseBuffer();
	RowMapper::VarDataPool& getVarDataPool();

	bool getContainerInfo(
			const GSChar *name, RowMapper::ContainerInfoRef<> &infoRef,
			const ContainerPropertyKeySet *propKeySet = NULL,
			RowMapper::VarDataPool *varDataPool = NULL,
			ContainerIdInfo *idInfo = NULL);
	static GSResult getContainerInfo(
			GSGridStore *store, const GSChar *name, GSContainerInfo *info,
			GSBool *exists, const ClientVersion &version,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

	GSContainer* getContainer(const GSChar *name, const GSBinding &binding,
			GSContainerType containerType);
	GSCollection* getCollection(const GSChar *name, const GSBinding &binding);
	GSTimeSeries* getTimeSeries(const GSChar *name, const GSBinding &binding);

	GSContainer* putContainer(
			const GSChar *name, const GSBinding &binding,
			const RowMapper::ContainerInfoRef<true> &containerInfoRef,
			GSBool modifiable, const GSContainerType *containerType);
	static GSResult putContainer(
			GSGridStore *store, const GSChar *name, bool nameRequired,
			const GSBinding *binding, const GSContainerInfo *info,
			GSBool modifiable, GSContainer **container,
			const GSContainerType *containerType,
			const ClientVersion &version,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

	GSContainer* getContainer(
			const GSChar *name, const GSContainerType *expectedType);

	GSContainer* putContainer(
			const GSChar *name,
			const RowMapper::ContainerInfoRef<true> &containerInfoRef,
			GSBool modifiable, const GSContainerType *containerType);
	static GSResult putContainer(
			GSGridStore *store, const GSChar *name,
			const GSContainerInfo *info, GSBool modifiable,
			GSContainer **container, const GSContainerType *containerType,
			const ClientVersion &version,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

	void dropContainer(
			const GSChar *name, const GSContainerType *containerType);
	void dropCollection(const GSChar *name);
	void dropTimeSeries(const GSChar *name);

	bool getRow(const GSChar *pathKey, void *rowObj);
	bool putRow(const GSChar *pathKey, const void *rowObj);
	bool removeRow(const GSChar *pathKey);

	void fetchAll(GSQuery *const *queryList, size_t queryCount);
	void multiPut(
			const GSContainerRowEntry *entryList, size_t entryCount);
	void multiGet(
			const GSRowKeyPredicateEntry *const &predicateList,
			size_t predicateCount,
			const GSContainerRowEntry *&entryList, size_t &entryCount);

	GSRow* createRow(const RowMapper::ContainerInfoRef<true> &infoRef);
	static GSResult createRow(
			GSGridStore *store, const GSContainerInfo *info, GSRow **row,
			const ClientVersion &version,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

	GSRowKeyPredicate* createRowKeyPredicate(GSType keyType);

	GSPartitionController* getPartitionController();

	static void importSchemaProperty(
			ArrayByteInStream &in, const RowMapper::Config &config,
			RowMapper::VarDataPool &varDataPool,
			GSContainerInfo &containerInfo,
			std::vector<GSColumnInfo> &columnInfoList, bool withContainerType);

	static void importIndexInfo(
			ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
			GSIndexInfo &indexInfo);
	static void exportIndexInfo(
			XArrayByteOutStream &out, const GSIndexInfo &indexInfo);
	static int32_t getIndexTypeOrdinal(
			GSIndexTypeFlags typeFlag, bool emptyOrDefaultAllowed);

	static bool isContainerStatementUnified();
	static bool isSessionUUIDSummarized();
	static bool isTSDivisionAndAffinityEnabled();

	static bool isClientIdEnabled();
	static bool isIndexDetailEnabled();
	static bool isAttributeVerifiable();
	static bool isNullableColumnAllowed();
	static bool isNullableColumnAllowed(int32_t protocolVersion);

	static bool isQueryOptionsExtensible();
	static const RowMapper::Config& getRowMapperConfig();
	static const RowMapper::Config& getRowMapperConfig(
			int32_t protocolVersion);

private:
	typedef std::set<void*> ResourceSet;
	typedef NodeConnection::OptionalRequestSource OptionalRequestSource;
	typedef GridStoreChannel::ContainerCache ContainerCache;
	typedef std::multimap<ContainerKey, GSContainer*> ContainerMap;

	struct ContainerPropertiesOption;

	struct MultiQueryStatement {
		explicit MultiQueryStatement(GridStoreChannel::Context &context);
		static void check(GSQuery &query, GSGridStore &store);
		void add(GSQuery &query);

		bool makeCreateSessionRequest(XArrayByteOutStream &req,
				GridStoreChannel &channel, GridStoreChannel::Context &context);
		void acceptCreateSessionResponse(ArrayByteInStream &resp);
		bool makeMainRequest(
				XArrayByteOutStream &req, GridStoreChannel::Context &context);
		void acceptMainResponse(ArrayByteInStream &resp,
				const GridStoreChannel::ConnectionId &connectionId);
		bool makeCloseSessionRequest(XArrayByteOutStream &req);
		void acceptSessionClosed();
		bool acceptStatementErrorForSession(
				const StatementException &cause);

		typedef std::vector<GSQuery*> QueryList;

		QueryList queryList_;
		QueryList optionalQueryList_;
		uuid_t sessionUUID_;
		bool updateQueryFound_;
	};

	struct MultiPutStatement {
		explicit MultiPutStatement(GridStoreChannel::Context &context);
		void add(
				const GSContainerRowEntry &rowEntry, const RowMapper *mapper,
				const ContainerKeyConverter &keyConverter);

		bool makeCreateSessionRequest(XArrayByteOutStream &req,
				GridStoreChannel &channel, GridStoreChannel::Context &context);
		void acceptCreateSessionResponse(ArrayByteInStream &resp);
		bool makeMainRequest(
				XArrayByteOutStream &req, GridStoreChannel::Context &context);
		void acceptMainResponse(ArrayByteInStream &resp,
				const GridStoreChannel::ConnectionId &connectionId);
		bool makeCloseSessionRequest(XArrayByteOutStream &req);
		void acceptSessionClosed();
		bool acceptStatementErrorForSession(
				const StatementException &cause);

		struct SubEntry {
			SubEntry();

			std::vector<GSRow*> rowList_;
			int32_t mapperIndex_;
			int64_t containerId_;
			int64_t sessionId_;
		};

		typedef std::vector<const RowMapper*> MapperList;
		typedef std::vector<ContainerKey> ContainerKeyList;
		typedef std::map<ContainerKey, SubEntry> SubEntryMap;

		MapperList mapperList_;
		ContainerKeyList containerKeyList_;
		SubEntryMap subEntryMap_;
		uuid_t sessionUUID_;
	};

	struct MultiGetRequest {
		enum PredicateType {
			PREDICATE_RANGE,
			PREDICATE_DISTINCT
		};

		typedef std::vector<GSRowKeyPredicate*> PredicateList;
		typedef std::vector<GSRowKeyPredicateEntry> EntryList;

		void add(const GSRowKeyPredicateEntry &predicateEntry);
		bool makeRequest(
				XArrayByteOutStream &req,
				GridStoreChannel &channel, GridStoreChannel::Context &context);

		PredicateList predicateList_;
		EntryList entryList_;
	};

	static bool pathKeyOperationEnabled_;

	static RowMapper::Config DEFAULT_MAPPER_CONFIG;
	static RowMapper::Config COMPATIBLE_MAPPER_CONFIG_14;
	static RowMapper::Config COMPATIBLE_MAPPER_CONFIG_13;

	GSGridStoreTag(const GSGridStore&);
	GSGridStore& operator=(const GSGridStore&);

	GSContainer* findContainerByCache(
			ContainerCache &cache, const ContainerKey &key,
			const ContainerKeyConverter &keyConverter,
			const GSBinding *binding, const GSContainerType *containerType,
			bool general);
	std::auto_ptr<ContainerKey> acceptRemoteContainerKey(
			ArrayByteInStream *in, const ContainerKey &localKey,
			const ContainerKeyConverter &keyConverter, bool &cached);

	void splitPathKey(
			const GSChar *pathKey,
			ContainerKey &containerKey, std::string &rowKeyStr);
	static void splitPathKey(
			const GSChar *pathKey,
			std::string &containerName, std::string &rowKeyStr);

	static void tryPutSystemOptionalRequest(
			XArrayByteOutStream &reqOut, GridStoreChannel::Context &context,
			bool forCreationDDL, const OptionalRequestSource *source = NULL);

	static ContainerPropertiesOption containerPropertiesToOption(
			const RowMapper &mapper);

	static void exportContainerProperties(
			XArrayByteOutStream &out, const GSContainerType type,
			const GSContainerInfo *info, const RowMapper &mapper);
	static void importContainerProperties(
			ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
			GSContainerInfo &containerInfo,
			const std::vector<GSColumnInfo> &columnInfoList);

	static GSCompressionMethod checkCompressionMethod(
			GSCompressionMethod compressionMethod);
	static double checkCompressionRate(double rate);

	const GSContainer& resolveContainer(const ContainerKey &containerKey);
	std::auto_ptr<GSContainer> duplicateContainer(const GSContainer &src);

	static Statement::Id getContainerStatement(
			Statement::Id statement, const GSContainerType *containerType);
	static void putContainerKey(
			XArrayByteOutStream &reqOut, GridStoreChannel &channel,
			GridStoreChannel::Context &context, const ContainerKey &key,
			const ContainerKeyConverter &keyConverter);
	static void tryPutContainerType(
			XArrayByteOutStream &reqOut, const GSContainerType *containerType);

	static void importIdProperty(
			ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
			ContainerIdInfo &idInfo, const ContainerKeyConverter &keyConverter);
	static void importIndexProperty(
			ArrayByteInStream &in, std::vector<GSColumnInfo> &columnInfoList);
	static void importEventNotificationProperty(
			ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
			std::vector<std::string> *eventNotificationInfoList);
	static void importTriggerProperty(
			ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
			GSContainerInfo &containerInfo,
			const std::vector<GSColumnInfo> &columnInfoList);
	static void importIndexDetailProperty(
			ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
			std::vector<GSIndexInfo> &indexInfoList);

	static void applyIndexInfoList(
			const std::vector<GSIndexInfo> &indexInfoList,
			const std::vector<GSColumnInfo> &columnInfoList,
			bool indexFlagsSpecified, RowMapper::VarDataPool &varDataPool,
			GSContainerInfo &containerInfo);

	static const GSChar* resolveContainerName(
			const GSChar *name, const GSContainerInfo *info);
	static const GSContainerType* resolveContainerType(
			const GSContainerType *type, const GSContainerInfo *info);

	template<typename MultiStatement>
	void executeStatement(int32_t partitionId, Statement::Id statement,
			MultiStatement &multiStatement);

	GSCollection* getContextControllerCollection(
			const GSContainerType *expectedType);

	GSResourceHeader resourceHeader_;

	size_t referenceCount_;

	GridStoreChannel &channel_;
	GridStoreChannel::Context context_;
	util::NormalXArray<uint8_t> &req_;
	util::NormalXArray<uint8_t> &resp_;
	RowMapper::VarDataPool varDataPool_;
	ResourceSet activeResources_;
	ContainerMap containerMap_;
	ErrorStack stack_;
};

struct GSGridStoreTag::ContainerPropertiesOption {
	const OptionalRequestSource* get() const;
};


struct GSContainerTag {
public:
	friend struct GSResourceHeader;
	friend struct GSGridStoreTag;

	struct PartialFetchStatus;
	struct PartialExecutionStatus;
	struct QueryFormatter;
	struct QueryData;
	struct QueryParameters;

	static const int32_t SESSION_NOT_FOUND_ERROR_CODE;
	static const int32_t UUID_UNMATCHED_ERROR_CODE;
	static const int32_t ROW_SET_NOT_FOUND_ERROR_CODE;

	static const int8_t ROW_SET_TYPE_PARTIAL_ROWS;

	GSContainerTag(GSGridStore &store, RowMapper::Reference mapper,
			int32_t schemaVerId, int32_t partitionId, int64_t containerId,
			std::auto_ptr<ContainerKey> normalizedContainerKey, bool cached);
	~GSContainerTag();

	bool isClosed() const;
	static void close(GSContainer **container, bool allRelated) throw();

	void createReference(void *resource);
	static void removeReference(GSContainer *&container, void *resource);
	static RowMapper::MappingMode getRowMappingMode();

	const RowMapper& getMapper() const;
	int32_t getSchemaVersionId() const;
	int32_t getPartitionId() const;
	int64_t getContainerId() const;
	const ContainerKey* getContainerKey() const;
	const ContainerKey* getNormalizedContainerKey() const;
	GSContainerType getType() const;

	RowMapper::VarDataPool& getVarDataPool();

	void createEventNotification(const GSChar *url);
	void dropEventNotification(const GSChar *url);

	void createTrigger(const GSTriggerInfo *info);
	void dropTrigger(const GSChar *name);

	void createOrDropIndex(
			const GSChar *columnName, GSIndexTypeFlags flags,
			bool forCreation);
	void createOrDropIndex(const GSIndexInfo &info, bool forCreation);

	void flush();

	bool getRow(const GSType *keyType, const void *key,
			void *rowObj, bool forUpdate, bool keyFormatted);
	bool putRow(const GSType *keyType, const void *key,
			const void *rowObj, bool keyFormatted);
	bool putMultipleRows(size_t rowCount, const void *const *rowObjs);
	bool removeRow(
			const GSType *keyType, const void *key, bool keyFormatted);

	GSQuery* query(const GSChar *queryString);
	GSRowSet* queryAndFetch(
			const QueryParameters &parameters, bool forUpdate);

	void makeQueryRequest(
			const QueryParameters &parameters, bool forUpdate,
			XArrayByteOutStream &reqOut, bool noUUID);
	void formatQuery(
			const QueryParameters &parameters,
			XArrayByteOutStream &reqOut);
	GSRowSet* acceptQueryResponse(
			const QueryParameters &parameters, bool forUpdate,
			ArrayByteInStream &respIn, bool bufSwapAllowed);

	void closeRowSet(
			const PartialFetchStatus &fetchStatus,
			GridStoreChannel::ConnectionId &connectionId);
	void fetchRowSet(
			int64_t remainingCount, const PartialFetchStatus &fetchStatus,
			const QueryParameters &parameters,
			const RowMapper &resultMapper,
			util::NormalXArray<uint8_t> &resultData,
			ArrayByteInStream &resultInStream,
			RowMapper::InputCursor &cursor,
			GridStoreChannel::ConnectionId &connectionId);

	void removeRow(
			const RowMapper &resolvedMapper, int64_t transactionId,
			const bool *transactionStarted, int64_t rowId, const void *key);
	void updateRow(
			const RowMapper &resolvedMapper, int64_t transactionId,
			const bool *transactionStarted, int64_t rowId, const void *key,
			const void *rowObj);

	void abort();
	void commit();
	void setAutoCommit(bool enabled);

	GSQuery* queryByGeometry(const GSChar *column, const GSChar *geometry,
			GSGeometryOperator geometryOp);
	GSQuery* queryByGeometry(const GSChar *column,
			const GSChar *geometryIntersection, const GSChar *geometryDisjoint);

	GSAggregationResult* aggregateTimeSeries(
			GSTimestamp start, GSTimestamp end,
			const GSChar *column, GSAggregation aggregation);
	bool appendTimeSeriesRow(const void *rowObj);
	bool getTimeSeriesRow(
			GSTimestamp base, GSTimeOperator timeOp, void *rowObj);
	bool interpolateTimeSeriesRow(GSTimestamp base, const GSChar *column,
			void *rowObj);
	GSQuery* queryByTime(
			const GSTimestamp *start, const GSTimestamp *end,
			GSQueryOrder order);
	GSQuery* queryByTime(
			GSTimestamp start, GSTimestamp end,
			const GSChar *const *columnSet,
			size_t columnCount, bool useColumnCount,
			GSInterpolationMode mode,
			int32_t interval, GSTimeUnit intervalUnit);

	struct AnyKeyTraits {
		typedef void Object;
		static const GSType* resolveKeyType() { return NULL; }
	};

	template<typename Traits>
	static GSResult getRowChecked(
			GSContainer *container, const typename Traits::Object *key,
			void *rowObj, GSBool forUpdate, GSBool *exists,
			const GSInterceptor::FunctionInfo &funcInfo);
	template<typename Traits>
	static GSResult putRowChecked(
			GSContainer *container, const typename Traits::Object *key,
			const void *rowObj, GSBool *exists,
			const GSInterceptor::FunctionInfo &funcInfo);
	template<typename Traits>
	static GSResult removeRowChecked(
			GSContainer *container, const typename Traits::Object *key,
			GSBool *exists, const GSInterceptor::FunctionInfo &funcInfo);

	GSRow* createRow();

private:
	typedef std::set<void*> ResourceSet;
	typedef NodeConnection::OptionalRequestSource OptionalRequestSource;
	typedef GridStoreChannel::ContainerCache ContainerCache;

	enum StatementFamily {
		STATEMENT_FAMILY_QUERY,
		STATEMENT_FAMILY_LOCK,
		STATEMENT_FAMILY_UPDATE,
		STATEMENT_FAMILY_POST,
		STATEMENT_FAMILY_NONE
	};

	enum TransactionMode {
		TRANSACTION_MODE_AUTO,
		TRANSACTION_MODE_BEGIN,
		TRANSACTION_MODE_CONTINUE
	};

	enum SessionMode {
		SESSION_MODE_AUTO,
		SESSION_MODE_CREATE,
		SESSION_MODE_GET
	};

	enum TransactionInfoType {
		TRANSACTION_INFO_DEFAULT,
		TRANSACTION_INFO_NO_UUID,
		TRANSACTION_INFO_SKIP_COMMIT_MODE
	};

	enum QueryResultType {
		RESULT_ROW_SET,
		RESULT_AGGREGATION,
		RESULT_QUERY_ANALYSIS,
		RESULT_PARTIAL_FETCH_STATE,
		RESULT_PARTIAL_EXECUTION_STATE
	};

	static const bool TIME_SERIES_UPDATE_ENABLED;

	static bool queryStatementIdPreserved_;

	static const int32_t MAX_SESSION_REPAIR_COUNT;

	static const Statement::Id TIME_SERIES_STATEMENT_UNIFIED_LIST[];
	static const Statement::Id TIME_SERIES_STATEMENT_SEPARATED_LIST[];
	static const Statement::Id FIXED_SESSION_MODE_STATEMENT_LIST[];

	struct SessionSortFunc : public std::binary_function<
			GridStoreChannel::SessionInfo, GridStoreChannel::SessionInfo, bool> {
		bool operator()(
				const GridStoreChannel::SessionInfo &left,
				const GridStoreChannel::SessionInfo &right) const {
			return left.getPartitionId() < right.getPartitionId();
		}
	};

	GSContainerTag(const GSContainer&);
	GSContainer& operator=(const GSContainer&);

	void checkOpened() const;

	GridStoreChannel& getChannel();
	GridStoreChannel::Context& getContext();
	util::NormalXArray<uint8_t>& getRequestBuffer();
	util::NormalXArray<uint8_t>& getResponseBuffer();

	static GSInterpolationMode checkInterpolationMode(
			GSInterpolationMode interpolationMode);
	static GSQueryOrder checkQueryOrder(GSQueryOrder order);

	ArrayByteInStream executeStatement(
			Statement::Id statement, StatementFamily familyForSession);
	ArrayByteInStream executeMultiStepStatement(
			Statement::Id statement, StatementFamily familyForSession,
			int32_t statementStep);

	int64_t getSessionIdDirect();
	void setSessionIdDirect(int64_t sessionId, bool statusUpdatable);
	int64_t updateStatementIdDirect();

	static bool isInitialSessionRetrialEnabled();
	static bool isNewSessionConflicted(const StatementException &cause);
	static bool isInitialSessionLost(
			Statement::Id statement, int64_t statementId,
			bool transactionStarted,
			const StatementException &cause);
	void putSessionInfo(XArrayByteOutStream &reqOut, int64_t sessionId);

	void commitForDDL();

	XArrayByteOutStream getRequestOutStream();

	StatementFamily prepareSession(StatementFamily family);
	void createSession();
	void putTransactionInfo(
			XArrayByteOutStream &reqOut, StatementFamily familyForSession,
			TransactionInfoType type = TRANSACTION_INFO_DEFAULT,
			const bool *forUpdate = NULL,
			const OptionalRequestSource *source = NULL);
	void tryPutOptionalRequest(
			XArrayByteOutStream &reqOut,
			bool forUpdate, bool containerLockAware, bool forCreationDDL,
			const OptionalRequestSource *source = NULL);

	static bool isSessionIdGeneratorEnabled();
	static bool isDDLSessionEnabled();
	static bool isRowSetIdHintDisabled();
	static bool isPartialRowSetLostAcceptable();
	static bool isMultiStepStatementIdEnabled();

	static void putNewSessionProperties(
			XArrayByteOutStream &reqOut,
			GridStoreChannel &channel,
			GridStoreChannel::Context &context);
	void closeSession(bool invalidating);
	static void closeSession(
			GridStoreChannel &channel,
			GridStoreChannel::Context &context,
			const GridStoreChannel::SessionInfo &sessionInfo);
	static void closeAllSessions(
			GridStoreChannel &channel,
			GridStoreChannel::Context &context,
			const GridStoreChannel::SessionInfo *sessionInfoList,
			size_t sessionInfoCount);
	void closeSubResources(bool silent, bool transactionalOnly);
	void closeRemote() throw();

	void disableCache();

	StatementFamily prepareQuerySession(
			const QueryParameters &parameters, bool forUpdate,
			bool neverCreate);

	static QueryResultType resolveQueryResultType(int8_t rawType);
	static GSRowSetType resolveRowSetType(QueryResultType type);
	const RowMapper& getResultRowMapper(QueryResultType type) const;
	RowMapper::MappingMode getResultRowMappingMode(QueryResultType type);
	bool isResultRowIdIncluded(QueryResultType type);

	void checkTransactionPreserved(
			bool forUpdate, int64_t transactionId,
			const bool *transactionStarted, bool updatable);

	bool filterIndexInfo(
			const GSIndexInfo &info, bool forCreation,
			GSIndexInfo &filteredInfo) const;
	GSIndexTypeFlags getDefaultIndexTypeFlag(int32_t columnId) const;

	static GSTimestamp wrapOptionalTimestamp(const GSTimestamp *timestamp);

	static bool isAnyQueryResultTypeEnabled();

	bool getRowForInternalController(
			const GSType *keyType, const void *key, void *rowObj);
	bool putRowForInternalController(
			const GSType *keyType, const void *key, const void *rowObj);

	GSResourceHeader resourceHeader_;

	size_t referenceCount_;

	GSGridStore *store_;
	ResourceSet activeResources_;

	RowMapper::Reference mapper_;
	const int32_t schemaVerId_;
	const int32_t partitionId_;
	const int64_t containerId_;
	std::auto_ptr<ContainerKey> normalizedContainerKey_;

	int64_t sessionId_;
	int64_t transactionId_;
	int64_t statementId_;
	bool closed_;
	bool sessionPrepared_;
	bool containerLocked_;
	bool transactionStarted_;
	bool autoCommit_;
	bool cacheDisabled_;
};

struct GSContainerTag::PartialFetchStatus {
	PartialFetchStatus();

	static PartialFetchStatus get(ArrayByteInStream &in);

	bool isEnabled() const;

	int64_t totalRowCount_;
	int64_t rowSetId_;
	int64_t rowSetIdHint_;
};

struct GSContainerTag::PartialExecutionStatus {
	typedef std::vector<uint8_t> EntryBytes;
	typedef std::map<int32_t, EntryBytes> EntryMap;

	static const PartialExecutionStatus STATUS_DISABLED;
	static const PartialExecutionStatus STATUS_ENABLED_INITIAL;

	explicit PartialExecutionStatus(bool enabled);

	PartialExecutionStatus(const PartialExecutionStatus &another);
	PartialExecutionStatus& operator=(const PartialExecutionStatus &another);

	static PartialExecutionStatus get(ArrayByteInStream &in);
	void put(XArrayByteOutStream &out) const;

	bool isEnabled() const;

	const PartialExecutionStatus& inherit(
			const PartialExecutionStatus &next) const;

	bool enabled_;
	std::auto_ptr<EntryMap> entryMap_;
};

struct GSContainerTag::QueryFormatter {
public:
	template<Statement::Id S> struct Typed;

	explicit QueryFormatter(Statement::Id statement);

	Statement::Id getStatement() const;

	virtual void getString(std::ostream &os, ArrayByteInStream &in) const;

private:
	Statement::Id statement_;
};

template<Statement::Id S>
struct GSContainerTag::QueryFormatter::Typed : public QueryFormatter {
public:
	Typed() : QueryFormatter(S) {}
};

struct GSContainerTag::QueryData {
	QueryData();

	QueryData(const QueryData &another);
	QueryData& operator=(const QueryData &another);

	util::NormalXArray<uint8_t> data_;
};

struct GSContainerTag::QueryParameters {
	struct StringFormatter;

	static const int64_t DEFAULT_SIZE_OPTION_VALUE;

	explicit QueryParameters(const QueryFormatter &formatter);

	Statement::Id getStatement() const;
	StringFormatter toString() const;

	void putFixed(XArrayByteOutStream &out) const;

	void setFetchLimit(int64_t fetchLimit);
	void setFetchSize(int64_t fetchSize);
	void setPartialExecutionEnabled(bool enabled);
	bool isPartialExecutionConfigured() const;

	static void checkPartialOptions(
			int64_t fetchSize, const PartialExecutionStatus &executionStatus);
	static bool isPartialFetch(int64_t fetchSize);

	bool isForUpdate(bool forUpdate) const;

	QueryParameters inherit(
			bool forUpdate, int64_t transactionId, bool transactionStarted,
			const PartialExecutionStatus &executionStatus) const;

	const QueryFormatter *formatter_;
	QueryData queryData_;

	int64_t fetchLimit_;
	int64_t fetchSize_;
	PartialExecutionStatus executionStatus_;
	bool executionPartial_;
	bool forUpdate_;
	bool transactionIdSpecified_;
	bool initialTransactionStarted_;
	int64_t initialTransactionId_;
};

struct GSContainerTag::QueryParameters::StringFormatter {
	explicit StringFormatter(const QueryParameters &parameters);

	void format(std::ostream &os) const;

	const QueryParameters &parameters_;
};

std::ostream& operator<<(
		std::ostream &os,
		const GSContainerTag::QueryParameters::StringFormatter &formatter);


struct GSQueryTag {
public:
	friend struct GSResourceHeader;
	friend struct GSContainerTag;

	typedef GSContainer::QueryFormatter QueryFormatter;
	typedef GSContainer::QueryParameters QueryParameters;
	typedef QueryParameters::StringFormatter StringFormatter;

	GSQueryTag(GSContainer &container, const QueryFormatter &formatter);
	~GSQueryTag();

	static void close(GSQuery **query) throw();

	GSContainer* getContainer();
	Statement::Id getStatement() const;
	StringFormatter toString() const;

	XArrayByteOutStream getParametersOutStream();

	GSRowSet* fetch(bool forUpdate);
	GSRowSet* getRowSet();

	void setFetchOption(
			GSFetchOption option, const void *value, GSType valueType);

	static bool isFetchSizeEnabled();

	void setForUpdate(bool forUpdate);
	bool isForUpdate() const;

	void makeRequest(XArrayByteOutStream &reqOut, bool noUUID);
	void acceptResponse(
			ArrayByteInStream &respIn,
			const GridStoreChannel::ConnectionId &connectionId,
			bool bufSwapAllowed);

private:
	GSQueryTag(const GSQuery&);
	GSQuery& operator=(const GSQuery&);

	void closeRemote() throw();

	void checkOpened();
	void clearLastRowSet() throw();

	static int64_t filterSizedFetchOption(
			GSFetchOption option, const void *value, GSType valueType);

	template<GSType T>
	static const typename RowMapper::ObjectTypeTraits<T, false>::Object*
	filterFetchOption(
			GSFetchOption option, const void *value, GSType valueType,
			bool force);

	GSResourceHeader resourceHeader_;

	GSContainer *container_;
	QueryParameters parameters_;
	GSRowSet *lastRowSet_;
	bool lastRowSetVisible_;
	bool closed_;
};


struct GSRowSetTag {
public:
	friend struct GSResourceHeader;
	friend struct GSContainerTag;

	typedef GSContainer::PartialFetchStatus PartialFetchStatus;
	typedef GSContainer::QueryParameters QueryParameters;

	GSRowSetTag(
			GSContainer &container, const RowMapper &mapper,
			int64_t rowCount, util::NormalXArray<uint8_t> &resultData,
			const ArrayByteInStream &resultInStream,
			RowMapper::MappingMode mappingMode, bool rowIdIncluded,
			GSRowSetType type, const QueryParameters &queryParameters,
			const PartialFetchStatus &fetchStatus,
			const GridStoreChannel::ConnectionId &connectionId,
			bool bufSwapAllowed);
	~GSRowSetTag();

	static void close(GSRowSet **rowSet) throw();

	void createReference() throw();
	static void removeReference(GSRowSet *&rowSet) throw();

	GSQuery::StringFormatter toQueryString() const;

	int32_t getSize() const;
	GSRowSetType getType() const;
	bool isPartial() const;
	bool hasNext() const;

	GSContainer* getContainer() const;
	int64_t getTransactionId() const;
	bool isTransactionStarted() const;
	int64_t getRowId() const;
	const void* getRowKey() const;

	void closeRemote(bool transactionalOnly);

	void next(void *rowObj);
	void nextAggregation(GSAggregationResult **aggregationResult);
	void nextQueryAnalysis(GSQueryAnalysisEntry *queryAnalysis);

	void remove();
	void update(const void *rowObj);

	void getRowFixedPart(const uint8_t *&data, size_t &size) const;
	void getRowVariablePart(const uint8_t *&data, size_t &size) const;

	void prepareFollowing();
	void prepareFollowingDirect();

	void fetchFollowing();

	void executeFollowing();

private:
	GSRowSetTag(const GSRowSet&);
	GSRowSet& operator=(const GSRowSet&);

	static ArrayByteInStream swapAndCreateInput(
			util::NormalXArray<uint8_t> &destBuf,
			util::NormalXArray<uint8_t> &srcBuf,
			const ArrayByteInStream &srcInStream,
			bool bufSwapAllowed);

	void checkOpened() const;
	void checkInRange() const;

	GSResourceHeader resourceHeader_;

	size_t referenceCount_;

	GSContainer *container_;
	const RowMapper &mapper_;
	util::NormalXArray<uint8_t> resultData_;
	ArrayByteInStream resultInStream_;
	RowMapper::VarDataPool &varDataPool_;
	RowMapper::InputCursor cursor_;
	const GSRowSetType type_;
	const void *lastKey_;
	RowMapper::KeyStorage lastKeyStorage_;

	const int64_t totalRowCount_;
	int64_t remainingRowCount_;
	QueryParameters queryParameters_;
	const PartialFetchStatus fetchStatus_;
	GridStoreChannel::ConnectionId connectionId_;
	bool previousFound_;
	bool followingLost_;
	bool closed_;
};


struct GSAggregationResultTag {
public:
	friend struct GSResourceHeader;
	friend struct GSContainerTag;
	friend class RowMapper;

	explicit GSAggregationResultTag(GSContainer &container);
	~GSAggregationResultTag();

	static void close(GSAggregationResult **aggregationResult) throw();
	static std::auto_ptr<GSAggregationResult> newStandaloneInstance();

	bool getValue(void *value, GSType valueType);

	template<typename Traits>
	static GSResult getValueTyped(
			GSAggregationResult *aggregationResult,
			typename Traits::Object *value, GSBool *assigned,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

private:
	GSAggregationResultTag();
	GSAggregationResultTag(const GSAggregationResult&);
	GSAggregationResult& operator=(const GSAggregationResult&);

	GSResourceHeader resourceHeader_;

	GSContainer *container_;
	GSType resultType_;
	union {
		double doubleValue_;
		int64_t longOrTimestampValue_;
	} value_;
};


struct GSRowTag {
public:
	friend struct GSResourceHeader;

	static GSRow* create(GSGridStore &store, RowMapper::Reference mapper);
	static GSRow* create(GSContainer &container, RowMapper::Reference mapper);
	static GSRow* create(RowMapper::VarDataPool &pool, RowMapper::Reference mapper);

	static void close(GSRow **row) throw();

	static const GSRow& resolve(const void *resource);
	static GSRow& resolve(void *resource);

	static GSBinding createBinding(
			const RowMapper::ContainerInfoRef<true> &infoRef,
			std::vector<GSBindingEntry> &entryList,
			bool anyTypeAllowed = false);

	const RowMapper& getRowMapper() const;
	void getContainerSchema(RowMapper::ContainerInfoRef<> &infoRef);

	static GSResult getContainerSchema(
			GSRow *row, GSContainerInfo *schemaInfo,
			const ClientVersion &version,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

	template<typename Traits>
	static GSResult getPrimitiveField(
			GSRow *row, int32_t columnId, typename Traits::Object *value,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

	template<typename Traits>
	static GSResult getArrayField(
			GSRow *row, int32_t columnId, typename Traits::Object *value,
			size_t *arraySize,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

	template<typename Traits>
	static GSResult setPrimitiveField(
			GSRow *row, int32_t columnId, typename Traits::Object value,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

	template<typename Traits>
	static GSResult setArrayField(
			GSRow *row, int32_t columnId, typename Traits::Object value,
			size_t arraySize,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

	void getField(int32_t columnId, GSValue &value, GSType &type,
			const GSType *expectedType);
	void getField(int32_t columnId, GSValue &value, GSType &type,
			const GSType *expectedType, RowMapper::VarDataPool &pool) const;
	void setField(int32_t columnId, const GSValue &value,
			GSType expectedType);

	void clear(bool silent);

	void* allocate(size_t size);
	void deallocate(void *varData);

	GSType getTypeGeneral(const GSBindingEntry &entry) const;
	GSBindingEntry getEntryGeneral(const GSBindingEntry &src) const;

	bool isNull(int32_t columnId) const;
	void setNull(int32_t columnId, bool nullValue);

	bool isNullDirect(int32_t columnId) const;
	void setNullDirect(
			int32_t columnId, const GSBindingEntry &entry, bool nullValue,
			bool withClear);

	const uint8_t* getNulls() const;
	uint8_t* getNulls();

	struct FieldClearer {
		template<typename Traits>
		void operator()(GSRow &row, const GSBindingEntry &entry, const Traits&);
	};

private:

	static const size_t DATA_OFFSET;

	GSRowTag(void *parentResource,
			RowMapper::Reference mapper, size_t objectSize);
	~GSRowTag();

	GSRowTag(const GSRow&);
	GSRow& operator=(const GSRow&);

	static GSRow* create(void *parentResource, RowMapper::Reference mapper);

	const GSBindingEntry& getBindingEntry(int32_t columnId) const;
	RowMapper::VarDataPool& getVarDataPool();
	GSType checkType(
			int32_t columnId, const GSBindingEntry &entry,
			const GSType *expectedType, bool overwriting) const;

	struct FieldGetter {
		FieldGetter(GSValue &value, RowMapper::VarDataPool &pool);

		template<typename Traits>
		void operator()(
				const GSRow &row, const GSBindingEntry &entry, const Traits&);
		void operator()(
				const GSRow &row, const GSBindingEntry &entry,
				const RowMapper::AnyNullTraits&);

		GSValue &value_;
		RowMapper::VarDataPool &pool_;
	};

	struct FieldSetter {
		FieldSetter(const GSValue &value, GSType type);

		template<typename Traits>
		void operator()(GSRow &row, const GSBindingEntry &entry, const Traits&);
		void operator()(
				GSRow &row, const GSBindingEntry &entry,
				const RowMapper::AnyNullTraits&);

		const GSValue &value_;
		GSType type_;
		bool deallocated_;
	};

	struct FieldDeallocator {
		template<typename Traits>
		void operator()(GSRow &row, const GSBindingEntry &entry, const Traits&);
		void operator()(
				GSRow &row, const GSBindingEntry &entry,
				const RowMapper::AnyNullTraits&);
	};

	struct FieldInitializer {
		template<typename Traits>
		void operator()(GSRow &row, const GSBindingEntry &entry, const Traits&);
		void operator()(
				GSRow &row, const GSBindingEntry &entry,
				const RowMapper::AnyNullTraits&);
	};

	template<typename Traits, bool ArrayType = Traits::ARRAY_TYPE>
	struct DirectFieldGetter {
		GSValue operator()(const GSRow &row, const GSBindingEntry &entry);
	};

	template<typename Traits, bool ArrayType = Traits::ARRAY_TYPE>
	struct DirectFieldSetter {
		void operator()(GSRow &row, const GSBindingEntry &entry,
				const GSValue &value);
	};

	template<typename Traits, typename T> void deallocate(
			const GSBindingEntry&, T, const Traits&);

	template<typename Traits, typename E>
	void deallocate(
			const GSBindingEntry&, const E *value, const Traits&);

	template<typename Traits>
	void deallocate(const GSBindingEntry &entry,
			const GSChar *const *value, const Traits&);

	template<typename Traits>
	void deallocate(const GSBindingEntry&, const GSBlob &value, const Traits&);

	template<bool ArrayType>
	void initializeArraySize(const GSBindingEntry &entry);

	uint8_t* data();
	const uint8_t* data() const;

	GSResourceHeader resourceHeader_;

	void *parentResource_;
	RowMapper::Reference mapper_;
	RowMapper::VarDataPool *varDataPool_;
};


struct GSRowKeyPredicateTag {
public:
	friend struct GSResourceHeader;
	friend struct GSGridStoreTag;

	enum RangeElementType {
		RANGE_START,
		RANGE_FINISH,

		RANGE_COUNT
	};

	GSRowKeyPredicateTag(GSGridStore &store, GSType keyType);
	~GSRowKeyPredicateTag();

	static void close(GSRowKeyPredicate **predicate) throw();

	template<typename Traits, RangeElementType RangeType>
	static GSResult getRangeKey(
			GSRowKeyPredicate *predicate, const typename Traits::Object **key,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

	template<typename Traits, RangeElementType RangeType>
	static GSResult setRangeKey(
			GSRowKeyPredicate *predicate, const typename Traits::Object *key,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

	template<typename Traits>
	static GSResult getDistinctKeys(
			GSRowKeyPredicate *predicate,
			const typename Traits::Object **keyList, size_t *size,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

	template<typename Traits>
	static GSResult addDistinctKey(
			GSRowKeyPredicate *predicate, typename Traits::Object key,
			const GSInterceptor::FunctionInfo &funcInfo) throw();

	GSType getKeyType() const;

	template<RangeElementType RangeType>
	void getRangeKey(const GSValue *&key, const GSType *expectedType) const;
	template<RangeElementType RangeType>
	void setRangeKey(const GSValue *key, GSType expectedType);

	template<typename T, typename Receiver>
	void getDistinctKeys(
			const T *&keyList, size_t &size,
			const GSType *expectedType, Receiver &receiver) const;
	void addDistinctKey(
			const GSValue &key, GSType expectedType);

	void* allocate(size_t size);
	void deallocate(void *varData);

	struct GeneralDistinctKeyReceiver {
		GSValue operator()(const GSValue &src) { return src; }
	};

private:
	GSRowKeyPredicateTag(const GSRowKeyPredicate&);
	GSRowKeyPredicate& operator=(const GSRowKeyPredicate&);

	struct KeyLess {
		explicit KeyLess(GSType keyType);

		bool operator()(const GSValue &key1, const GSValue &key2) const;

		const GSType keyType_;
	};

	template<typename Traits>
	struct TypedDistinctKeyReceiver {
		typename Traits::Object operator()(const GSValue &src) {
			return Traits::as(src);
		}
	};

	typedef std::pair<GSValue, bool> RangeKeyEntry;
	typedef std::set<GSValue, KeyLess> DistinctKeySet;

	void checkKeyType(const GSType *expectedType) const;

	void clear();
	void clearRangeKeyEntry(RangeKeyEntry &keyEntry);
	void clearKey(GSValue &value);

	GSResourceHeader resourceHeader_;

	GSGridStore *store_;
	const GSType keyType_;

	RangeKeyEntry rangeKeyEntries_[RANGE_COUNT];
	std::auto_ptr<DistinctKeySet> distinctKeys_;
};


struct GSPartitionControllerTag {
public:
	friend struct GSResourceHeader;

	explicit GSPartitionControllerTag(GSGridStore &store);
	~GSPartitionControllerTag();

	static void close(GSPartitionController **controller) throw();

	int32_t getPartitionCount();
	int64_t getContainerCount(int32_t partitionIndex);

	void getContainerNames(int32_t partitionIndex,
			int64_t start, const int64_t *limit,
			const GSChar *const *&nameList, size_t &size);

	void getHosts(int32_t partitionIndex,
			const GSChar *const *&addressList, size_t &size);
	void getOwnerHost(int32_t partitionIndex, const GSChar *&address);
	void getBackupHosts(int32_t partitionIndex,
			const GSChar *const *&addressList, size_t &size);

	int32_t getPartitionIndexOfContainer(const GSChar *containerName);

	void assignPreferableHost(
			int32_t partitionIndex, const GSChar *host);

private:
	GSPartitionControllerTag(const GSPartitionController&);
	GSPartitionController& operator=(const GSPartitionController&);

	void checkOpened();
	void checkPartitionIndex(int32_t partitionIndex);

	const GSChar *const* toAddressList(
			const std::vector<util::SocketAddress> &baseAddressList,
			size_t &resultListSize, RowMapper::VarDataPool &varDataPool);

	GSResourceHeader resourceHeader_;

	GSGridStore *store_;
};


template<typename T> inline bool ClientUtil::parseValue(
		const GSChar *strValue, T &resultValue) {
	return util::StrictLexicalConverter<T>()(strValue, resultValue);
}

template<typename T> inline T ClientUtil::parseValue(const GSChar *strValue) {
	T resultValue;
	if (!parseValue(strValue, resultValue)) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_ILLEGAL_VALUE_FORMAT, strValue);
	}

	return resultValue;
}

template<typename T> inline T ClientUtil::getNonAlignedValue(
		const void *ptr) {
	T value;
	memcpy(&value, ptr, sizeof(T));

	return value;
}

template<typename T> inline void ClientUtil::setNonAlignedValue(
		void *ptr, const T &value) {
	memcpy(ptr, &value, sizeof(T));
}

template<typename T>
inline const T* ClientUtil::findFromArray(
		const T *target, size_t totalByteSize, const T& value) {

	const T *end = target + totalByteSize / sizeof(T);
	const T *it = std::find(target, end, value);

	if (it == end) {
		return NULL;
	}

	return it;
}

template<typename C>
ArrayByteInStream ClientUtil::subStream(
		const C &buf, const ArrayByteInStream &in, size_t size) {
	if (size > in.base().remaining()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_MESSAGE_CORRUPTED, "");
	}

	const size_t beginPos = in.base().position();
	const size_t endPos = beginPos + size;
	if (endPos > buf.size()) {
		GS_CLIENT_THROW_ERROR(GS_ERROR_CC_INTERNAL_ERROR, "");
	}

	ArrayByteInStream subIn((util::ArrayInStream(buf.data(), endPos)));
	subIn.base().position(beginPos);
	return subIn;
}


inline bool ClientUtil::varSizeIs1Byte(uint8_t val) {
	return ((val & 0x01) == 0x01);
}
inline bool ClientUtil::varSizeIs4Byte(uint8_t val) {
	return ((val & 0x03) == 0x00);
}
inline bool ClientUtil::varSizeIs8Byte(uint8_t val) {
	return ((val & 0x03) == 0x02);
}


inline uint32_t ClientUtil::decode1ByteVarSize(uint8_t val) {
	return val >> 1;
}
inline uint32_t ClientUtil::decode4ByteVarSize(uint32_t val) {
	return val >> 2;
}
inline uint64_t ClientUtil::decode8ByteVarSize(uint64_t val) {
	return val >> 2;
}


inline uint8_t ClientUtil::encode1ByteVarSize(uint8_t val) {
	assert(val < VAR_SIZE_1BYTE_THRESHOLD);
	return static_cast<uint8_t>((val << 1) | 0x01);
}
inline uint32_t ClientUtil::encode4ByteVarSize(uint32_t val) {
	assert(VAR_SIZE_1BYTE_THRESHOLD <= val
			&& val < VAR_SIZE_4BYTE_THRESHOLD);
	return val << 2;
}
inline uint64_t ClientUtil::encode8ByteVarSize(uint64_t val) {
	assert(VAR_SIZE_4BYTE_THRESHOLD <= val
			&& val < VAR_SIZE_8BYTE_THRESHOLD);
	return (val << 2) | 0x02;
}


inline uint32_t ClientUtil::getEncodedVarSize(size_t val) {
	if (val < VAR_SIZE_1BYTE_THRESHOLD) {
		return 1;
	}
	else if (val < VAR_SIZE_4BYTE_THRESHOLD) {
		return 4;
	}
	else {
		sizeValueToInt32(val); 
		return 8;
	}
}


inline size_t ClientUtil::calcNullsByteSize(size_t columnNum) {
	return (columnNum + 7) / 8;
}

#endif	
