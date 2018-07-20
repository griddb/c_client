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
#define gsGetRowSchema gsGetRowSchema_Static
#endif

#ifdef GS_CLIENT_UNIT_TEST
#define gsGetContainerInfoV1_5 gsGetContainerInfoV1_5_Stub
#define gsGetContainerInfoV2_1 gsGetContainerInfoV2_1_Stub
#define gsPutTimeSeriesV2_0 gsPutTimeSeriesV2_0_Stub
#define gsPutContainerGeneralV2_0 gsPutContainerGeneralV2_0_Stub
#define gsPutContainerGeneralV2_1 gsPutContainerGeneralV2_1_Stub
#define gsPutCollectionGeneralV2_1 gsPutCollectionGeneralV2_1_Stub
#define gsPutTimeSeriesGeneralV2_0 gsPutTimeSeriesGeneralV2_0_Stub
#define gsPutTimeSeriesGeneralV2_1 gsPutTimeSeriesGeneralV2_1_Stub
#define gsGetRowSchemaV2_1 gsGetRowSchemaV2_1_Stub
#endif

#include "util/container.h"
#include "util/net.h"
#include "gridstore.h"
#include "authentication.h"
#include "service_address.h"
#include "geometry_coder.h"
#include "uuid/uuid.h"


#ifdef GS_DLL_INSIDE
#undef gsGetContainerInfo
#undef gsPutTimeSeries
#undef gsPutContainerGeneral
#undef gsPutCollectionGeneral
#undef gsPutTimeSeriesGeneral
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
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutCollectionGeneral(
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
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowSchema(
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
	static std::string normalizeName(const GSChar *value);
	static bool isIPV6Address(const util::SocketAddress &address);
	static GSTimeUnit checkTimeUnit(GSTimeUnit timeUnit);
	template<typename T> static T getNonAlignedValue(const void *ptr);
	template<typename T> static void setNonAlignedValue(
			void *ptr, const T &value);

	template<typename T>
	static const T* findFromArray(
			const T *target, size_t totalByteSize, const T& value);


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



struct ErrorStack {
public:
	struct Entry {
		int32_t errorCode_;
		std::string optionMessage_;
		std::string type_;
		std::string fileName_;
		std::string functionName_;
		int32_t lineNumber_;
	};

	size_t getSize() const throw();
	GSResult getErrorCode(size_t index) const throw();
	size_t formatErrorMessage(
			size_t index, GSChar *strBuf, size_t bufSize) const throw();
	size_t formatErrorLocation(
			size_t index, GSChar *strBuf, size_t bufSize) const throw();

	void clear() throw();
	GSResult setFromCurrentException() throw();
	static std::auto_ptr<ErrorStack> tryRead(
			ArrayByteInStream &in, bool detailErrorMessageEnabled) throw();

	const Entry* entryAt(size_t index) const throw();

private:
	GSResult setFromException(util::Exception &e, size_t startDepth = 0);

	std::vector<Entry> entryList_;
};


class RemoteException : public util::Exception {
public:
	RemoteException() throw();

	RemoteException(
			UTIL_EXCEPTION_CONSTRUCTOR_ARGS_DECL,
			std::auto_ptr<ErrorStack> remoteErrorStack =
					std::auto_ptr<ErrorStack>()) throw();

	virtual ~RemoteException() throw();

	RemoteException(const RemoteException &another) throw();

	RemoteException& operator=(const RemoteException &another) throw();

	const ErrorStack* getRemoteErrorStack() const throw();

	static std::auto_ptr<ErrorStack> extractRemoteErrorStack(
			const RemoteException &ex) throw();

	static std::auto_ptr<ErrorStack> extractRemoteErrorStack(
			const std::exception &ex) throw();

private:
	void trySetRemoteErrorStack(const ErrorStack *stack) throw();

	std::auto_ptr<ErrorStack> remoteErrorStack_;
};

#define GS_CLIENT_EXCEPTION_CREATE_WITH_STACK( \
		type, errorCode, cause, message, errorStack) \
	type( \
			(errorCode), UTIL_EXCEPTION_CREATE_MESSAGE_CHARS(message), \
			UTIL_EXCEPTION_POSITION_ARGS, cause, #type, \
			util::Exception::STACK_TRACE_NONE, \
			util::Exception::LITERAL_NORMAL, errorStack)


class StatementException : public RemoteException {
public:
	StatementException(const StatementException &another) throw();

	StatementException(
			UTIL_EXCEPTION_CONSTRUCTOR_ARGS_DECL,
			std::auto_ptr<ErrorStack> remoteErrorStack =
					std::auto_ptr<ErrorStack>()) throw();

	virtual ~StatementException() throw();
};

#define GS_CLIENT_THROW_STATEMENT(errorCode, message, errorStack) \
	throw GS_CLIENT_EXCEPTION_CREATE_WITH_STACK( \
			StatementException, errorCode, NULL, message, errorStack)


class ConnectionException : public RemoteException {
public:
	ConnectionException(
			UTIL_EXCEPTION_CONSTRUCTOR_ARGS_DECL,
			std::auto_ptr<ErrorStack> remoteErrorStack =
					std::auto_ptr<ErrorStack>()) throw();

	virtual ~ConnectionException() throw();
};

#define GS_CLIENT_THROW_CONNECTION_WITH_STACK(errorCode, message, errorStack) \
	throw GS_CLIENT_EXCEPTION_CREATE_WITH_STACK( \
			ConnectionException, errorCode, NULL, message, errorStack)
#define GS_CLIENT_THROW_CONNECTION(errorCode, message) \
	throw UTIL_EXCEPTION_CREATE_DETAIL( \
			ConnectionException, errorCode, NULL, message)
#define GS_CLIENT_RETHROW_CONNECTION(errorCode, cause, message) \
	throw GS_CLIENT_EXCEPTION_CREATE_WITH_STACK( \
			ConnectionException, errorCode, &(cause), message, \
			RemoteException::extractRemoteErrorStack(cause))


class WrongNodeException : public ConnectionException {
public:
	WrongNodeException(
			UTIL_EXCEPTION_CONSTRUCTOR_ARGS_DECL,
			std::auto_ptr<ErrorStack> remoteErrorStack =
					std::auto_ptr<ErrorStack>()) throw();

	virtual ~WrongNodeException() throw();
};

#define GS_CLIENT_THROW_WRONG_NODE_WITH_STACK(errorCode, message, errorStack) \
	throw GS_CLIENT_EXCEPTION_CREATE_WITH_STACK( \
			WrongNodeException, errorCode, NULL, message, errorStack)
#define GS_CLIENT_THROW_WRONG_NODE(errorCode, message) \
	throw UTIL_EXCEPTION_CREATE_DETAIL( \
			WrongNodeException, errorCode, NULL, message)
#define GS_CLIENT_RETHROW_WRONG_NODE(errorCode, cause, message) \
	throw GS_CLIENT_EXCEPTION_CREATE_WITH_STACK( \
			WrongNodeException, errorCode, &(cause), message, \
			RemoteException::extractRemoteErrorStack(cause))


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
	GS_ERROR_CC_NO_SHCH_ELEMENT,
	GS_ERROR_CC_CONTAINER_CLOSED,
	GS_ERROR_CC_NOT_LOCKED,
	GS_ERROR_CC_RESOURCE_CLOSED,
	GS_ERROR_CC_ALLOCATION_FAILED,
	GS_ERROR_CC_RECOVERABLE_CONNECTION_PROBLEM,
	GS_ERROR_CC_RECOVERABLE_ROW_SET_LOST
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

	static const GSType ANY_NULL_TYPE = -1;
	static const GSType NULLABLE_MASK =
			~static_cast<GSType>((1U << (CHAR_BIT - 1)) - 1);

	struct Tool;
	struct KeyStorage;
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

public:
	RowMapper(RowTypeCategory rowTypeCategory, const GSBinding *binding,
			bool anyTypeAllowed = false);
	RowMapper(const RowMapper &srcMapper, ArrayByteInStream &schemaIn,
			bool columnOrderIgnorable);
	~RowMapper();

	static std::auto_ptr<RowMapper> getInstance(ArrayByteInStream &in,
			RowTypeCategory category, bool anyTypeAllowed);

	static Cache& getDefaultCache();

	static const RowMapper& getAggregationResultMapper();
	static const RowMapper& getQueryAnalysisMapper();

	void checkSchemaMatched(const RowMapper &mapper) const;

	bool matches(
			RowTypeCategory rowTypeCategory, const GSBinding *binding,
			bool general) const;
	bool matches(
			const RowMapper &baseMapper, ArrayByteInStream schemaIn) const;

	static size_t getDigest(
			RowTypeCategory rowTypeCategory, const GSBinding *binding,
			bool general);
	static size_t getDigest(
			const RowMapper &baseMapper, ArrayByteInStream schemaIn);

	GSContainerType getContainerType() const;
	void getContainerSchema(
			GSContainerInfo &containerInfo, VarDataPool &varDataPool) const;

	RowTypeCategory getCategory() const;
	bool isGeneral() const;
	const GSBinding& getBinding() const;
	bool hasKey() const;

	GSType getElementType(int32_t columnId) const;
	bool isArray(int32_t columnId) const;
	bool hasAnyTypeColumn() const;

	void exportSchema(XArrayByteOutStream &out) const;
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
	static bool isNullable(GSType type);
	static GSType toNullable(GSType type, bool nullable = true);
	static GSType toNonNullable(GSType type);

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

	typedef std::map<std::string, ColumnIdEntry> ColumnIdMap;

	static const bool ACCEPT_AGGREGATION_RESULT_COLUMN_ID;
	static const bool RESTRICT_KEY_ORDER_FIRST;

	static Cache *cache_;
	static const RowMapper AGGREGATION_RESULT_MAPPER;
	static const RowMapper QUERY_ANALYSIS_MAPPER;

	RowMapper(size_t digest, RowTypeCategory rowTypeCategory, bool general);

	RowMapper(const RowMapper&);
	RowMapper& operator=(const RowMapper&);

	static size_t getDigest(const GSBindingEntry &entry);
	static size_t getDigest(const GSChar *str);

	static GSBinding checkAndCopyBinding(
			const GSBinding *src, ColumnIdMap &entryIndexMap,
			RowTypeCategory rowTypeCategory, bool anyTypeAllowed);
	static GSBinding createReorderedBinding(
			const RowMapper &baseMapper, ColumnIdMap &entryIndexMap,
			ArrayByteInStream &in, bool columnOrderIgnorable);

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


class RowMapper::InputCursor {
public:
	friend class RowMapper;
	friend struct RowMapper::Tool;

	InputCursor(ArrayByteInStream &in, const RowMapper &mapper,
			MappingMode mode, int32_t rowCount, bool rowIdIncluded,
			VarDataPool *varDataPool);

	void setVarDataBaseOffset(int64_t varDataBaseOffset);

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
			bool general, bool anyTypeAllowed = false);
	const RowMapper* resolve(
			const RowMapper &baseMapper, ArrayByteInStream &schemaIn,
			bool columnOrderIgnorable);
	const RowMapper* resolve(
			const GSContainerInfo &info, bool anyTypeAllowed = false);

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

		STATEMTNT_ID_MAX
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
	struct Config;
	struct LoginInfo;
	struct OptionalRequest;
	struct Heartbeat;
	static const int32_t EE_MAGIC_NUMBER;

	NodeConnection(const util::SocketAddress &address, const Config &config);
	~NodeConnection();

	static void setDetailErrorMessageEnabled(bool enabled);
	static int32_t getProtocolVersion();
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
			const LoginInfo &loginInfo);
	bool loginInternal(
			util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp,
			const LoginInfo &loginInfo, Auth::Challenge &challenge);

	void logout(
			util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp);

	void reuse(
			util::NormalXArray<uint8_t> &req, util::NormalXArray<uint8_t> &resp,
			const LoginInfo &loginInfo);

	static std::string getDigest(const char8_t *password);

	static int32_t statementToNumber(Statement::Id statement);

	static int32_t getStatementNumberOffset();

	static bool isDatabaseEnabled();

private:
	static const int32_t DEFAULT_PROTOCOL_VERSION;
	static const int32_t STATEMENT_TYPE_NUMBER_V2_OFFSET;
	static volatile bool detailErrorMessageEnabled_;
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
	int64_t statementId_;
	std::string authenticatedUser_;
	std::string authenticatedPasswordDigest_;
	std::string authenticatedDatabase_;
	bool responseUnacceptable_;
	bool authenticated_;
	bool ownerMode_;
	int32_t transactionTimeoutSecs_;
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
			int64_t transactionTimeoutMillis);
	void setPassword(const GSChar *password);

	std::allocator<uint8_t> alloc_;
	std::string user_;
	Auth::PasswordDigest passwordDigest_;
	std::string database_;
	bool ownerMode_;
	std::string clusterName_;
	int32_t transactionTimeoutSecs_;
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
		FETCH_SIZE = 10003
	};

	OptionalRequest();
	OptionalRequest(
			int32_t statementTimeout, int64_t fetchLimit, int64_t fetchSize,
			const std::string &dbName);

	void format(XArrayByteOutStream &reqOut);
	static void putType(XArrayByteOutStream &reqOut, Type type);

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
		CRC32
	};
};


class NodeResolver {
public:
	static const int32_t NOTIFICATION_RECEIVE_TIMEOUT;
	static const int32_t PROVIDER_UPDATE_TIMEOUT;

	class ProtocolConfig;
	class DefaultProtocolConfig;
	struct AddressConfig;

	NodeResolver(NodeConnectionPool &pool, bool passive,
			const util::SocketAddress &address,
			const NodeConnection::Config &connectionConfig,
			const NodeConnection::LoginInfo &loginInfo,
			int32_t partitionCount,
			const ServiceAddressResolver::Config &sarConfig,
			const std::vector<util::SocketAddress> &memberList,
			const AddressConfig &addressConfig);

	void setPassword(const NodeConnection::LoginInfo &loginInfo);
	void setConnectionConfig(const NodeConnection::Config &connectionConfig);
	void setNotificationReceiveTimeoutMillis(int64_t timeout);
	void setProviderTimeoutMillis(int64_t timeout);
	void setPreferableConnectionPoolSize(int32_t size);

	int32_t getPartitionCount();
	ContainerHashMode::Id getContainerHashMode();

	util::SocketAddress getMasterAddress();
	util::SocketAddress getNodeAddress(
			int32_t partitionId, bool backupPreferred,
			const util::SocketAddress *preferebleHost = NULL);
	void getNodeAddressList(
			int32_t partitionId, std::vector<util::SocketAddress> &addressList);

	void invalidateMaster();

	void setProtocolConfig(ProtocolConfig *protocolConfig);

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

	static void makeServiceAddressResolver(
			ServiceAddressResolver &resolver,
			const ServiceAddressResolver::Config &sarConfig,
			const std::vector<util::SocketAddress> &memberList,
			const AddressConfig &addressConfig);

	void prepareMasterConnection(size_t startTrialCount);
	void updateNotificationMember();
	bool tryUpdateMasterConnection();
	void prepareMasterInfo(size_t startTrialCount);
	void invalidateMasterInternal();
	void acceptPartitionCount(int32_t partitionCount, bool fromNotification);

	const std::vector<util::SocketAddress>* getNodeAddressList(
			int32_t partitionId, bool backupPreferred, size_t startTrialCount,
			bool allowEmpty);
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
	NodeConnection::LoginInfo loginInfo_;
	int32_t partitionCount_;
	bool partitionCountSpecified_;
	ContainerHashMode::Id containerHashMode_;
	std::auto_ptr<NodeConnection> masterConnection_;
	util::NormalXArray<uint8_t> req_;
	util::NormalXArray<uint8_t> resp_;
	int64_t notificationReceiveTimeoutMillis_;
	int64_t providerTimeoutMillis_;
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
		std::string user_;
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
	struct Source;
	struct Context;

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

	GridStoreChannel(const Config &config, const Source &source);

	NodeConnectionPool& getConnectionPool();
	void apply(const Source &source);
	void apply(const Config &config);
	int64_t getFailoverTimeoutMillis(const Context &context);
	void setStatementRetryMode(int32_t statementRetryMode);

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

	void getNodeAddressList(
			Context &context, int32_t partitionId,
			std::vector<util::SocketAddress> &addressList);

	int32_t getPartitionCount(Context &context);

	int32_t resolvePartitionId(Context &context, const GSChar *containerName);
	static int32_t calculatePartitionId(
			const GSChar *containerName, ContainerHashMode::Id mode,
			int32_t partitionCount);

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

	NodeConnectionPool pool_;
	Config config_;
	int32_t statementRetryMode_;
	const Key key_;
	NodeResolver nodeResolver_;
	size_t reqHeadLength_;
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


struct GridStoreChannel::Source {
	friend class GridStoreChannel;

	Source();
	~Source();

	const Key& getKey() const;
	Context createContext() const;

	void set(const Properties &properties);

	Key key_;
	int32_t partitionCount_;
	int64_t failoverTimeoutMillis_;
	int64_t transactionTimeoutMillis_;
	NodeConnection::LoginInfo loginInfo_;
	int32_t containerCacheSize_;
};


struct GridStoreChannel::Context {
public:
	friend class GridStoreChannel;
	friend struct ResolverExecutorScope;

	~Context();

	Context(const Context &another);
	Context& operator=(const Context &another);

	int64_t getTransactionTimeoutMillis() const;
	int32_t getFailoverCount() const;

	util::NormalXArray<uint8_t>& getRequestBuffer();
	util::NormalXArray<uint8_t>& getResponseBuffer();

	void getSessionUUID(uuid_t sessionUUID) const;
	int64_t generateSessionId();

	ConnectionId getLastConnectionId() const;
	ContainerCache *getContainerCache();

	void setPreferebleHost(
			int32_t partitionId, const util::SocketAddress *host);
	const util::SocketAddress* getPreferebleHost(int32_t partitionId) const;

private:
	typedef std::pair<ConnectionId, NodeConnection*> ConnectionEntry;
	typedef std::map<util::SocketAddress, ConnectionEntry,
			SocketAddressLess> ConnectionMap;
	typedef std::map<int32_t, util::SocketAddress> PreferebleHostMap;

	Context(
			int64_t failoverTimeoutMillis, int64_t transactionTimeoutMillis,
			const NodeConnection::LoginInfo &loginInfo,
			int32_t containerCacheSize);

	int64_t failoverTimeoutMillis_;
	int64_t transactionTimeoutMillis_;
	int32_t partitionId_;
	int32_t partitionCount_;
	ConnectionEntry lastConnection_;
	ConnectionMap activeConnections_;
	NodeConnection::LoginInfo loginInfo_;
	bool closed_;

	int32_t failoverCount_;

	util::NormalXArray<uint8_t> req_;
	util::NormalXArray<uint8_t> resp_;
	uuid_t sessionUUID_;
	int64_t lastSessionId_;
	ConnectionId connectionId_;
	int32_t containerCacheSize_;
	std::auto_ptr<ContainerCache> containerCache_;
	PreferebleHostMap preferebleHosts_;

	ResolverExecutor *resolverExecutor_;
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
			const std::string &normalizedContainerName,
			const GSBinding *binding,
			const GSContainerType *containerType,
			bool general) const;
	void cacheSchema(
			const std::string &normalizedContainerName,
			const RowMapper &mapper, int64_t containerId, int32_t versionId);
	bool removeSchema(const std::string &normalizedContainerName);

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

	typedef std::map<uint64_t, std::string> SchemaNameMap;
	typedef std::map<std::string, LocatedSchema> SchemaCache;
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
		COMMAND_GET_ADDRESS_LIST,
		COMMAND_GET_PARTITION_COUNT,
		COMMAND_RESOLVE_PARTITION_ID
	};

	explicit ResolverExecutor(Command command);
	void execute(NodeResolver &resolver);

	Command command_;
	const GSChar *containerName_;
	std::vector<util::SocketAddress> *addressList_;
	int32_t partitionId_;
	int32_t partitionCount_;
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
};


struct GSResourceHeader {
public:
	GSResourceHeader(GSResourceType::Id type);
	~GSResourceHeader();

	static bool getType(
			const void *resource, GSResourceType::Id &type) throw();
	static bool checkType(
			const void *resource, GSResourceType::Id type) throw();

	static ErrorStack* findErrorStack(
			void *resource, util::Mutex *&mutex) throw();
	static void clearLastError(void *resource) throw();
	static GSResult setCurrentException(void *resource) throw();

#ifdef GS_CLIENT_UNIT_TEST
	static int64_t getResourceCount(GSResourceType::Id type);
#endif

private:
	static const uint32_t RESOURCE_TYPE_MASK;

	GSResourceType::Id getType() const throw();

	static uint32_t maskType(GSResourceType::Id type) throw();
	static GSResourceType::Id unmaskType(uint32_t maskedType) throw();

	const uint32_t maskedType_;

#ifdef GS_CLIENT_UNIT_TEST
	static int64_t resourceCountList_[];
#endif
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
			const GSPropertyEntry *properties, const size_t *propertyCount);

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

	struct Data {
		ChannelMap channelMap_;
		GridStoreChannel::Config channelConfig_;
		ErrorStack stack_;
		util::Mutex mutex_;
	};

	GSGridStoreFactoryTag(const GSGridStoreFactory&);
	GSGridStoreFactory& operator=(const GSGridStoreFactory&);

	GSResourceHeader resourceHeader_;
	std::auto_ptr<Data> data_;

	static GSGridStoreFactory *defaultFactory_;
};

namespace {
GSGridStoreFactory::Initializer g_gridStoreFactoryInitializer;
}


struct GSGridStoreTag {
public:
	friend struct GSResourceHeader;

	enum ContainerPropertyType {
		CONTAINER_PROPERTY_ID,
		CONTAINER_PROPERTY_SCHEMA,
		CONTAINER_PROPERTY_INDEX,
		CONTAINER_PROPERTY_EVENT_NOTIFICATION,
		CONTAINER_PROPERTY_TRIGGER
	};

	struct ContainerIdInfo {
		int32_t versionId_;
		int64_t containerId_;
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
			const GSChar *name, GSContainerInfo &info,
			const ContainerPropertyType *propertyTypeList = NULL,
			size_t propertyCount = 0,
			RowMapper::VarDataPool *varDataPool = NULL,
			ContainerIdInfo *idInfo = NULL,
			int32_t majorVersion = GS_CLIENT_VERSION_MAJOR,
			int32_t minorVersion = GS_CLIENT_VERSION_MINOR);
	static GSResult getContainerInfo(
			GSGridStore *store, const GSChar *name,
			GSContainerInfo *info, GSBool *exists,
			int32_t majorVersion = GS_CLIENT_VERSION_MAJOR,
			int32_t minorVersion = GS_CLIENT_VERSION_MINOR) throw();

	GSContainer* getContainer(const GSChar *name, const GSBinding &binding,
			GSContainerType containerType);
	GSCollection* getCollection(const GSChar *name, const GSBinding &binding);
	GSTimeSeries* getTimeSeries(const GSChar *name, const GSBinding &binding);

	GSContainer* putContainer(
			const GSChar *name, const GSBinding &binding,
			const GSContainerInfo *info, GSBool modifiable,
			const GSContainerType *containerType,
			int32_t majorVersion, int32_t minorVersion);
	static GSResult putContainer(
			GSGridStore *store, const GSChar *name, bool nameRequired,
			const GSBinding *binding, const GSContainerInfo *info,
			GSBool modifiable, GSContainer **container,
			const GSContainerType *containerType,
			int32_t majorVersion = GS_CLIENT_VERSION_MAJOR,
			int32_t minorVersion = GS_CLIENT_VERSION_MINOR) throw();

	GSContainer* getContainer(
			const GSChar *name, const GSContainerType *expectedType);

	GSContainer* putContainer(
			const GSChar *name, const GSContainerInfo &containerInfo,
			GSBool modifiable, const GSContainerType *containerType,
			int32_t majorVersion, int32_t minorVersion);
	static GSResult putContainer(
			GSGridStore *store, const GSChar *name,
			const GSContainerInfo *info, GSBool modifiable,
			GSContainer **container, const GSContainerType *containerType,
			int32_t majorVersion = GS_CLIENT_VERSION_MAJOR,
			int32_t minorVersion = GS_CLIENT_VERSION_MINOR) throw();

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

	GSRow* createRow(const GSContainerInfo &info);
	GSRowKeyPredicate* createRowKeyPredicate(GSType keyType);

	GSPartitionController* getPartitionController();

	static void initializeContainerInfo(GSContainerInfo &info,
			int32_t majorVersion, int32_t minorVersion) throw();

	static void importSchemaProperty(ArrayByteInStream &in,
			RowMapper::VarDataPool &varDataPool,
			std::vector<const GSChar*> &columnNameList,
			std::vector<GSType> &columnTypeList,
			GSContainerType *containerType, bool &keyAvailable,
			bool anyTypeAllowed);

	static bool isContainerStatementUnified();
	static bool isSessionUUIDSummarized();
	static bool isTSDivisionAndAffinityEnabled();

private:
	typedef GridStoreChannel::ContainerCache ContainerCache;
	typedef util::NormalSortedList<void*> ResourceList;
	typedef std::multimap<std::string, GSContainer*> ContainerMap;

	struct ColumnInfoWithoutIndex {
		const GSChar *name_;
		GSType type_;
	};

	struct MultiQueryStatement {
		explicit MultiQueryStatement(GridStoreChannel::Context &context);
		static void check(GSQuery &query, GSGridStore &store);
		void add(GSQuery &query);

		bool makeCreateSessionRequest(XArrayByteOutStream &req,
				GridStoreChannel &channel, GridStoreChannel::Context &context);
		void acceptCreateSessionResponse(ArrayByteInStream &resp);
		bool makeMainRequest(XArrayByteOutStream &req);
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
				const GSContainerRowEntry &rowEntry, const RowMapper *mapper);

		bool makeCreateSessionRequest(XArrayByteOutStream &req,
				GridStoreChannel &channel, GridStoreChannel::Context &context);
		void acceptCreateSessionResponse(ArrayByteInStream &resp);
		bool makeMainRequest(XArrayByteOutStream &req);
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
		typedef std::vector<std::string> ContainerNameList;
		typedef std::map<std::string, SubEntry> SubEntryMap;

		MapperList mapperList_;
		ContainerNameList containerNameList_;
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
				GridStoreChannel::Context &context);

		PredicateList predicateList_;
		EntryList entryList_;
	};

	static bool pathKeyOperationEnabled_;

	GSGridStoreTag(const GSGridStore&);
	GSGridStore& operator=(const GSGridStore&);

	GSContainer* findContainerByCache(
			ContainerCache &cache, const GSChar *name,
			const GSBinding *binding, const GSContainerType *containerType,
			bool general);
	std::auto_ptr<std::string> acceptRemoteContainerName(
			ArrayByteInStream &in, const GSChar *localName);

	static void splitPathKey(const GSChar *pathKey,
			std::string &containerName, std::string &rowKeyStr);
	static void exportContainerProperties(
			XArrayByteOutStream &out, const GSContainerType type,
			const GSContainerInfo *info, const RowMapper &mapper,
			int32_t majorVersion, int32_t minorVersion);
	static const GSTimeSeriesProperties* importContainerProperties(
			ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
			const GSChar *const *columnNameList, size_t columnCount,
			const GSChar *&dataAffinity, GSContainerType containerType);

	static GSCompressionMethod checkCompressionMethod(
			GSCompressionMethod compressionMethod);
	static double checkCompressionRate(double rate);

	const GSContainer& resolveContainer(const std::string &containerName);
	std::auto_ptr<GSContainer> duplicateContainer(const GSContainer &src);

	static Statement::Id getContainerStatement(
			Statement::Id statement, const GSContainerType *containerType);
	static void tryPutContainerType(
			XArrayByteOutStream &reqOut, const GSContainerType *containerType);

	static void importIdProperty(ArrayByteInStream &in,
			RowMapper::VarDataPool &varDataPool, ContainerIdInfo &idInfo);
	static void importIndexProperty(ArrayByteInStream &in,
			std::vector<GSIndexTypeFlags> &indexTypesList,
			size_t columnCount);
	static void importEventNotificationProperty(
			ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
			size_t &entryCount);
	static GSTriggerInfo* importTriggerProperty(
			ArrayByteInStream &in, RowMapper::VarDataPool &varDataPool,
			std::vector<const GSChar*> &columnNameList,
			size_t &entryCount);
	static const void* makeColumnInfoList(
			RowMapper::VarDataPool &varDataPool,
			const std::vector<const GSChar*> &columnNameList,
			const std::vector<GSType> &columnTypeList,
			const std::vector<GSIndexTypeFlags> *indexTypesList,
			int32_t majorVersion, int32_t minorVersion);

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
	ResourceList resourceList_;
	ContainerMap containerMap_;
	ErrorStack stack_;
};


struct GSContainerTag {
public:
	friend struct GSResourceHeader;
	friend struct GSGridStoreTag;

	static const int32_t SESSION_NOT_FOUND_ERROR_CODE;
	static const int32_t UUID_UNMATCHED_ERROR_CODE;
	static const int32_t ROW_SET_NOT_FOUND_ERROR_CODE;

	static const int8_t ROW_SET_TYPE_PARTIAL_ROWS;

	GSContainerTag(GSGridStore &store, RowMapper::Reference mapper,
			int32_t schemaVerId, int32_t partitionId, int64_t containerId,
			std::auto_ptr<std::string> normalizedContainerName);
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
	const std::string* getNormalizedContainerName() const;
	GSContainerType getType() const;

	RowMapper::VarDataPool& getVarDataPool();

	void createEventNotification(const GSChar *url);
	void dropEventNotification(const GSChar *url);

	void createTrigger(const GSTriggerInfo *info);
	void dropTrigger(const GSChar *name);

	void createIndex(const GSChar *columnName, GSIndexTypeFlags flags);
	void dropIndex(const GSChar *columnName, GSIndexTypeFlags flags);

	void flush();

	bool getRow(const GSType *keyType, const void *key,
			void *rowObj, bool forUpdate, bool keyFormatted);
	bool putRow(const GSType *keyType, const void *key,
			const void *rowObj, bool keyFormatted);
	bool putMultipleRows(size_t rowCount, const void *const *rowObjs);
	bool removeRow(
			const GSType *keyType, const void *key, bool keyFormatted);

	GSQuery* query(const GSChar *queryString);
	GSRowSet* queryAndFetch(const GSQuery &query, bool forUpdate);

	void makeQueryRequest(const GSQuery &query, bool forUpdate,
			XArrayByteOutStream &reqOut, bool noUUID);
	void formatQuery(XArrayByteOutStream &reqOut, const GSQuery &query);
	GSRowSet* acceptQueryResponse(const GSQuery &query, bool forUpdate,
			ArrayByteInStream &respIn, bool bufSwapAllowed);

	void closeRowSet(int64_t rowSetId, int64_t rowSetIdHint,
			GridStoreChannel::ConnectionId &connectionId);
	void fetchRowSet(int64_t rowSetId, int64_t rowSetIdHint,
			int64_t totalCount, int64_t remainingCount, int64_t fetchSize,
			const RowMapper &resultMapper,
			util::NormalXArray<uint8_t> &resultData,
			ArrayByteInStream &resultInStream,
			RowMapper::InputCursor &cursor,
			GridStoreChannel::ConnectionId &connectionId);

	void removeRow(const RowMapper &resolvedMapper,
			int64_t transactionId, int64_t rowId, const void *key);
	void updateRow(const RowMapper &resolvedMapper,
			int64_t transactionId, int64_t rowId,
			const void *key, const void *rowObj);

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
			void *rowObj, GSBool forUpdate, GSBool *exists);
	template<typename Traits>
	static GSResult putRowChecked(
			GSContainer *container, const typename Traits::Object *key,
			const void *rowObj, GSBool *exists);
	template<typename Traits>
	static GSResult removeRowChecked(
			GSContainer *container, const typename Traits::Object *key,
			GSBool *exists);

	GSRow* createRow();

private:
	typedef std::vector<void*> ResourceList;
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

	XArrayByteOutStream getRequestOutStream();

	StatementFamily prepareSession(StatementFamily family);
	void createSession();
	void putTransactionInfo(
			XArrayByteOutStream &reqOut, StatementFamily familyForSession,
			TransactionInfoType type = TRANSACTION_INFO_DEFAULT,
			const bool *forUpdate = NULL);
	void tryPutOptionalRequest(
			XArrayByteOutStream &reqOut,
			bool forUpdate, bool containerLockAwared);

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

	void createOrDropIndex(
			bool create, const GSChar *columnName, GSIndexTypeFlags flags);

	static GSTimestamp wrapOptionalTimestamp(const GSTimestamp *timestamp);

	static bool isAnyQueryResultTypeEnabled();

	bool getRowForInternalController(
			const GSType *keyType, const void *key, void *rowObj);
	bool putRowForInternalController(
			const GSType *keyType, const void *key, const void *rowObj);

	GSResourceHeader resourceHeader_;

	size_t referenceCount_;

	GSGridStore *store_;
	ResourceList activeResources_;

	RowMapper::Reference mapper_;
	const int32_t schemaVerId_;
	const int32_t partitionId_;
	const int64_t containerId_;
	std::auto_ptr<std::string> normalizedContainerName_;

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


struct GSQueryTag {
public:
	friend struct GSResourceHeader;
	friend struct GSContainerTag;

	GSQueryTag(GSContainer &container, Statement::Id statement);
	~GSQueryTag();

	static void close(GSQuery **query) throw();

	GSContainer* getContainer();
	Statement::Id getStatement() const;

	XArrayByteOutStream getParametersOutStream();

	GSRowSet* fetch(bool forUpdate);
	GSRowSet* getRowSet();

	void setFetchOption(GSFetchOption fetchOption, const void *value,
			GSType valueType);

	static bool isFetchSizeEnabled();

	void setForUpdate(bool forUpdate);
	bool isForUpdate();

	void makeRequest(XArrayByteOutStream &reqOut, bool noUUID);
	void acceptResponse(ArrayByteInStream &respIn,
			const GridStoreChannel::ConnectionId &connectionId,
			bool bufSwapAllowed);

private:
	GSQueryTag(const GSQuery&);
	GSQuery& operator=(const GSQuery&);

	void closeRemote() throw();

	void checkOpened();
	void clearLastRowSet() throw();

	GSResourceHeader resourceHeader_;

	GSContainer *container_;
	Statement::Id statement_;
	util::NormalXArray<uint8_t> parameterData_;
	int64_t fetchLimit_;
	int64_t fetchSize_;
	GSRowSet *lastRowSet_;
	bool lastRowSetVisible_;
	bool forUpdate_;
	bool closed_;
};


struct GSRowSetTag {
public:
	friend struct GSResourceHeader;
	friend struct GSContainerTag;

	GSRowSetTag(GSContainer &container, const RowMapper &mapper,
			int64_t transactionId, int64_t rowCount,
			util::NormalXArray<uint8_t> &resultData,
			const ArrayByteInStream &resultInStream,
			RowMapper::MappingMode mappingMode, bool rowIdIncluded,
			int8_t resultBaseType, int64_t totalRowCount,
			int64_t rowSetId, int64_t rowSetIdHint, int64_t fetchSize,
			const GridStoreChannel::ConnectionId &connectionId,
			bool bufSwapAllowed);
	~GSRowSetTag();

	static void close(GSRowSet **rowSet) throw();

	void createReference() throw();
	static void removeReference(GSRowSet *&rowSet) throw();

	int32_t getSize() const;
	GSRowSetType getType() const;
	bool isPartial() const;
	bool hasNext() const;

	GSContainer* getContainer() const;
	int64_t getTransactionId() const;
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

	void fetchFollowing();

private:
	GSRowSetTag(const GSRowSet&);
	GSRowSet& operator=(const GSRowSet&);

	static ArrayByteInStream swapAndCreateInput(
			util::NormalXArray<uint8_t> &destBuf,
			util::NormalXArray<uint8_t> &srcBuf,
			const ArrayByteInStream &srcInStream,
			bool bufSwapAllowed);

	void checkOpened() const;

	GSResourceHeader resourceHeader_;

	size_t referenceCount_;

	GSContainer *container_;
	const RowMapper &mapper_;
	int64_t transactionId_;
	util::NormalXArray<uint8_t> resultData_;
	ArrayByteInStream resultInStream_;
	RowMapper::VarDataPool &varDataPool_;
	RowMapper::InputCursor cursor_;
	int8_t resultBaseType_;
	const void *lastKey_;
	RowMapper::KeyStorage lastKeyStorage_;

	const int64_t totalRowCount_;
	int64_t remainingRowCount_;
	const int64_t rowSetId_;
	const int64_t rowSetIdHint_;
	const int64_t fetchSize_;
	GridStoreChannel::ConnectionId connectionId_;
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
			const GSContainerInfo &info, std::vector<GSBindingEntry> &entryList,
			bool anyTypeAllowed = false);

	const RowMapper& getRowMapper() const;
	void getContainerSchema(GSContainerInfo &info);

	static GSResult getContainerSchema(
			GSRow *row, GSContainerInfo *schemaInfo,
			int32_t majorVersion = GS_CLIENT_VERSION_MAJOR,
			int32_t minorVersion = GS_CLIENT_VERSION_MINOR);

	template<typename Traits>
	static GSResult getPrimitiveField(GSRow *row, int32_t columnId,
			typename Traits::Object *value) throw();

	template<typename Traits>
	static GSResult getArrayField(GSRow *row, int32_t columnId,
			typename Traits::Object *value, size_t *arraySize) throw();

	template<typename Traits>
	static GSResult setPrimitiveField(GSRow *row, int32_t columnId,
			typename Traits::Object value) throw();

	template<typename Traits>
	static GSResult setArrayField(GSRow *row, int32_t columnId,
			typename Traits::Object value, size_t arraySize) throw();

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
	static GSResult getRangeKey(GSRowKeyPredicate *predicate,
			const typename Traits::Object **key) throw();

	template<typename Traits, RangeElementType RangeType>
	static GSResult setRangeKey(GSRowKeyPredicate *predicate,
			const typename Traits::Object *key) throw();

	template<typename Traits>
	static GSResult getDistinctKeys(GSRowKeyPredicate *predicate,
			const typename Traits::Object **keyList,
			size_t *size) throw();

	template<typename Traits>
	static GSResult addDistinctKey(GSRowKeyPredicate *predicate,
			typename Traits::Object key) throw();

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
	for (;;) {
		util::NormalIStringStream iss(strValue);
		iss.peek();
		if (iss.eof()) {
			break;
		}

		iss.unsetf(std::ios::skipws);
		if (sizeof(T) < sizeof(int32_t)) {
			int32_t resultInt32Value;
			iss >> resultInt32Value;
			if (iss.bad() || resultInt32Value <
					static_cast<int32_t>(std::numeric_limits<T>::min()) ||
					resultInt32Value >
					static_cast<int32_t>(std::numeric_limits<T>::max()) ) {
				break;
			}
			resultValue = static_cast<T>(resultInt32Value);
		}
		else {
			iss >> resultValue;
		}

		if (iss.bad()) {
			break;
		}
		if (!iss.eof()) {
			break;
		}

		util::NormalOStringStream oss;
		oss << resultValue;
		if (iss.str() != oss.str()) {
			break;
		}

		return true;
	}

	resultValue = T();
	return false;
}

template<> inline bool ClientUtil::parseValue(
		const GSChar *strValue, bool &resultValue) {
	if (strcmp(strValue, "true") == 0) {
		resultValue = true;
		return true;
	}
	else if (strcmp(strValue, "false") == 0) {
		resultValue = false;
		return true;
	}

	resultValue = bool();
	return false;
}

template<typename T> inline T ClientUtil::parseValue(const GSChar *strValue) {
	T resultValue;
	if (!parseValue(strValue, resultValue)) {
		UTIL_THROW_ERROR(GS_ERROR_CC_ILLEGAL_VALUE_FORMAT, strValue);
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
	return (val << 1) | 0x01;
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
