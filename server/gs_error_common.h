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
	@brief Definitions of common errors
*/
#ifndef GS_ERROR_COMMON_H_
#define GS_ERROR_COMMON_H_

#ifndef GS_ERROR_COMMON_ONLY
#define GS_ERROR_COMMON_ONLY 0
#endif

#if GS_ERROR_COMMON_ONLY
#include "util/type.h"
#else
#include "gs_error.h"
#endif

enum GSCommonErrorCode {
	GS_ERROR_CM_FAILED = 1000,
	GS_ERROR_CM_NO_MEMORY,
	GS_ERROR_CM_SERVICE_UNKNOWN,
	GS_ERROR_CM_NAME_HAS_NO_OWNER,
	GS_ERROR_CM_NO_REPLY,
	GS_ERROR_CM_IO_ERROR,
	GS_ERROR_CM_BAD_ADDRESS,
	GS_ERROR_CM_NOT_SUPPORTED,
	GS_ERROR_CM_LIMITS_EXCEEDED,
	GS_ERROR_CM_ACCESS_DENIED,
	GS_ERROR_CM_AUTH_FAILED,
	GS_ERROR_CM_NO_SERVER,
	GS_ERROR_CM_TIMEOUT,
	GS_ERROR_CM_NO_NETWORK,
	GS_ERROR_CM_ADDRESS_IN_USE,
	GS_ERROR_CM_DISCONNECTED,
	GS_ERROR_CM_INVALID_ARGS,
	GS_ERROR_CM_FILE_NOT_FOUND,
	GS_ERROR_CM_FILE_EXISTS,
	GS_ERROR_CM_UNKNOWN_METHOD,
	GS_ERROR_CM_TIMED_OUT,
	GS_ERROR_CM_MATCH_RULE_NOT_FOUND,
	GS_ERROR_CM_MATCH_RULE_INVALID,
	GS_ERROR_CM_SPAWN_EXEC_FAILED,
	GS_ERROR_CM_SPAWN_FORK_FAILED,
	GS_ERROR_CM_SPAWN_CHILD_EXITED,
	GS_ERROR_CM_SPAWN_CHILD_SIGNALED,
	GS_ERROR_CM_SPAWN_FAILED,
	GS_ERROR_CM_SPAWN_SETUP_FAILED,
	GS_ERROR_CM_SPAWN_CONFIG_INVALID,
	GS_ERROR_CM_SPAWN_SERVICE_INVALID,
	GS_ERROR_CM_SPAWN_SERVICE_NOT_FOUND,
	GS_ERROR_CM_SPAWN_PERMISSIONS_INVALID,
	GS_ERROR_CM_SPAWN_FILE_INVALID,
	GS_ERROR_CM_SPAWN_NO_MEMORY,
	GS_ERROR_CM_UNIX_PROCESS_ID_UNKNOWN,
	GS_ERROR_CM_INVALID_SIGNATURE,
	GS_ERROR_CM_INVALID_FILE_CONTENT,
	GS_ERROR_CM_SELINUX_SECURITY_CONTEXT_UNKNOWN,
	GS_ERROR_CM_ADT_AUDIT_DATA_UNKNOWN,
	GS_ERROR_CM_OBJECT_PATH_IN_USE,
	GS_ERROR_CM_INTERNAL_ERROR,
	GS_ERROR_CM_PLATFORM_ERROR,
	GS_ERROR_CM_MEMORY_LIMIT_EXCEEDED,
	GS_ERROR_CM_SIZE_LIMIT_EXCEEDED,
	GS_ERROR_CM_CANCELED,
	GS_ERROR_CM_COMPRESSION_FAILED,
	GS_ERROR_CM_UNCOMPRESSION_FAILED,
	GS_ERROR_CM_INCOMPATIBLE_ZLIB_VERSION,

	GS_ERROR_DS_UNDEFINED = 60000,
	GS_ERROR_DS_DS_STATEMENT_FAILED,
	GS_ERROR_DS_DS_VERSION_INVALID,
	GS_ERROR_DS_DS_OPEN_FAILED,
	GS_ERROR_DS_DS_DATA_DIRECTORY_CREATE_FAILED,
	GS_ERROR_DS_DS_CONTAINER_ID_INVALID,
	GS_ERROR_DS_DS_CHUNK_SIZE_INVALID,
	GS_ERROR_DS_DS_PARTITION_NUM_INVALID,
	GS_ERROR_DS_DS_PARTITION_GROUP_NUM_INVALID,
	GS_ERROR_DS_DS_REDO_FAILED,
	GS_ERROR_DS_DS_REDO_INVALID,
	GS_ERROR_DS_DS_CHUNK_OFFSET_INVALID,
	GS_ERROR_DS_DS_ROW_LOCKED,
	GS_ERROR_DS_DS_DROP_PARTITION_FAILED,
	GS_ERROR_DS_DS_RESTORE_FAILED,
	GS_ERROR_DS_DS_SCHEMA_INVALID,
	GS_ERROR_DS_DS_CHANGE_SCHEMA_DISABLE,
	GS_ERROR_DS_DS_CHANGE_COLLECTION_SCHEMA_FAILED,
	GS_ERROR_DS_DS_CREATE_COLLECTION_FAILED,
	GS_ERROR_DS_DS_DROP_COLLECTION_FAILED,
	GS_ERROR_DS_DS_GET_COLLECTION_FAILED,
	GS_ERROR_DS_DS_CREATE_TIME_SERIES_FAILED,
	GS_ERROR_DS_DS_DROP_TIME_SERIES_FAILED,
	GS_ERROR_DS_DS_GET_TIME_SERIES_FAILED,
	GS_ERROR_DS_DS_CHANGE_TIME_SERIES_SCHEMA_FAILED,
	GS_ERROR_DS_DS_CONTAINER_NAME_INVALID,
	GS_ERROR_DS_DS_CONTAINER_TYPE_INVALID,
	GS_ERROR_DS_DS_PARAMETER_INVALID,
	GS_ERROR_DS_CONTAINER_NOT_FOUND,
	GS_ERROR_DS_OM_BAD_ADDRESS,
	GS_ERROR_DS_OA_DOUBLE_FREE,
	GS_ERROR_DS_OM_OPEN_DUMP_FILE_FAILED,
	GS_ERROR_DS_CM_PARTITION_ID_INVALID,
	GS_ERROR_DS_CM_CHUNK_ID_INVALID,
	GS_ERROR_DS_CM_EXPIRATION_TIME_INVALID,
	GS_ERROR_DS_OM_PARTITION_GROUP_INVALID,
	GS_ERROR_DS_CM_OPEN_SWAP_FILE_FAILED,
	GS_ERROR_DS_CM_READ_CHUNK_FAILED,
	GS_ERROR_DS_CM_WRITE_CHUNK_FAILED,
	GS_ERROR_DS_CM_CHUNK_NUM_LIMIT,
	GS_ERROR_DS_CM_SWAP_DIRECTORY_CREATE_FAILED,
	GS_ERROR_DS_CM_CHUNK_CHECKSUM_INVALID,
	GS_ERROR_DS_COL_INDEXINFO_INVALID,
	GS_ERROR_DS_COL_CREATE_INDEX_FAILED,
	GS_ERROR_DS_COL_DROP_INDEX_FAILED,
	GS_ERROR_DS_COL_GET_COLINFO_FAILED,
	GS_ERROR_DS_COL_ROWDATA_INVALID,
	GS_ERROR_DS_COL_PUT_ROW_FAILED,
	GS_ERROR_DS_COL_PUT_ROWLIST_FAILED,
	GS_ERROR_DS_COL_ROWKEY_INVALID,
	GS_ERROR_DS_COL_ROWKEY_UNDEFINED,
	GS_ERROR_DS_COL_ROWID_INVALID,
	GS_ERROR_DS_COL_DELETE_ROW_FAILED,
	GS_ERROR_DS_COL_UPDATE_ROW_FAILED,
	GS_ERROR_DS_COL_LOCK_CONFLICT,
	GS_ERROR_DS_COL_NOLOCK,
	GS_ERROR_DS_COL_COMMIT_FAILED,
	GS_ERROR_DS_COL_ABORT_FAILED,
	GS_ERROR_DS_COL_PUT_EVENT_NOTIFICATIOIN_FAILED,
	GS_ERROR_DS_COL_DELETE_EVENT_NOTIFICATIOIN_FAILED,
	GS_ERROR_DS_COL_GET_EVENT_NOTIFICATIOIN_FAILED,
	GS_ERROR_DS_TIM_CREATE_INDEX_FAILED,
	GS_ERROR_DS_TIM_DROP_INDEX_FAILED,
	GS_ERROR_DS_TIM_GET_COLINFO_FAILED,
	GS_ERROR_DS_TIM_LOCK_CONFLICT,
	GS_ERROR_DS_TIM_APPEND_ROW_FAILED,
	GS_ERROR_DS_TIM_PUT_ROW_FAILED,
	GS_ERROR_DS_TIM_PUT_ROWLIST_FAILED,
	GS_ERROR_DS_TIM_DELETE_ROW_FAILED,
	GS_ERROR_DS_TIM_COMMIT_FAILED,
	GS_ERROR_DS_TIM_ABORT_FAILED,
	GS_ERROR_DS_TIM_PUT_EVENT_NOTIFICATIOIN_FAILED,
	GS_ERROR_DS_TIM_DELETE_EVENT_NOTIFICATIOIN_FAILED,
	GS_ERROR_DS_TIM_GET_EVENT_NOTIFICATIOIN_FAILED,
	GS_ERROR_DS_TIM_SAMPLING_TIME_UNIT_INVALID,
	GS_ERROR_DS_TIM_INTERPORATED_COLUMN_TYPE_INVALID,
	GS_ERROR_DS_TIM_INTERPORATED_COLUMN_IDLIST_INVALID,
	GS_ERROR_DS_TIM_PUT_ROW_ERROR,
	GS_ERROR_DS_TIM_ROW_KEY_INVALID,
	GS_ERROR_DS_TIM_ROW_DATA_INVALID,
	GS_ERROR_DS_TIM_INDEXINFO_INVALID,
	GS_ERROR_DS_TIM_CREATEINDEX_ON_ROWKEY,
	GS_ERROR_DS_TIM_DROPINDEX_ON_ROWKEY,
	GS_ERROR_DS_TIM_NOLOCK,
	GS_ERROR_DS_TIM_INVALID_SCHEMA_OPTION,
	GS_ERROR_DS_TIM_CHANGE_SCHEMA_TIMEOUT,
	GS_ERROR_DS_TIM_UPDATE_INVALID,
	GS_ERROR_DS_TM_INSERT_FAILED,
	GS_ERROR_DS_TM_REMOVE_FAILED,
	GS_ERROR_DS_TM_UPDATE_FAILED,
	GS_ERROR_DS_TM_SEARCH_FAILED,
	GS_ERROR_DS_HM_INSERT_FAILED,
	GS_ERROR_DS_HM_REMOVE_FAILED,
	GS_ERROR_DS_HM_UPDATE_FAILED,
	GS_ERROR_DS_HM_SEARCH_FAILED,
	GS_ERROR_DS_HM_UNEXPECTED_ERROR,
	GS_ERROR_DS_HM_MAX_COLLISION_SIZE_OVER,
	GS_ERROR_DS_HM_MAX_ARRY_SIZE_OVER,
	GS_ERROR_DS_TYPE_INVALID,
	GS_ERROR_DS_KEY_RANGE_INVALID,
	GS_ERROR_DS_AGGREGATED_COLUMN_TYPE_INVALID,
	GS_ERROR_DS_FILTERING_CONDITION_INVALID,
	GS_ERROR_DS_COLUMN_ID_INVALID,
	GS_ERROR_DS_ARRAY_INDEX_INVALID,
	GS_ERROR_DS_CM_OPEN_CHUNK_IMAGE_FILE_FAILED,
	GS_ERROR_DS_CM_CHUNK_IMAGE_DIRECTORY_CREATE_FAILED,
	GS_ERROR_DS_CM_GET_CHUNK_FAILED,
	GS_ERROR_DS_COL_HIT_COUNT_INVALID,
	GS_ERROR_DS_TIM_HIT_COUNT_INVALID,
	GS_ERROR_DS_DS_GET_CONTAINER_LIST_FAILED,
	GS_ERROR_DS_CM_START_GROUP_CHECKPOINT_FAILED,
	GS_ERROR_DS_CM_END_GROUP_CHECKPOINT_FAILED,
	GS_ERROR_DS_CM_START_PARTITION_CHECKPOINT_FAILED,
	GS_ERROR_DS_CM_END_PARTITION_CHECKPOINT_FAILED,
	GS_ERROR_DS_CM_CHECKPOINT_ID_INVALID,
	GS_ERROR_DS_CM_NO_CHUNK_IMAGE,
	GS_ERROR_DS_CM_ANOTHER_SYNC_RUNNING,		
	GS_ERROR_DS_CM_PARTITION_GROUP_ID_INVALID,  
	GS_ERROR_DS_CM_COPY_CHUNK_FAILED,
	GS_ERROR_DS_CM_RECOVERY_FAILED,
	GS_ERROR_DS_OM_FREE_TYPE_INVALID,  
	GS_ERROR_DS_COL_GET_INDEX_INFO_LIST_FAILED,
	GS_ERROR_DS_TIM_GET_INDEX_INFO_LIST_FAILED,
	GS_ERROR_DS_CM_SWAP_CHUNK_FAILED,  
	GS_ERROR_DS_CM_INVALID_KICK_MODE,
	GS_ERROR_DS_CM_CP_FILE_INVALID,  
	GS_ERROR_DS_DUMMY4,
	GS_ERROR_DS_CM_CHECKPOINT_FILE_FLUSH_FAILED,
	GS_ERROR_DS_CM_CHUNK_IMAGE_DIRECTORY_IS_NOT_DIRECTORY,
	GS_ERROR_DS_CM_CHUNK_IMAGE_DIRECTORY_NOT_FOUND,
	GS_ERROR_DS_CM_CHUNK_CHECKPOINT_ID_INVALID,
	GS_ERROR_DS_HICOMPRESSION_INVALID,
	GS_ERROR_DS_DS_RESULT_ID_INVALID,  
	GS_ERROR_DS_FETCH_START_POS_INVALID,
	GS_ERROR_DS_PUT_TRIGGER_FAILED,
	GS_ERROR_DS_UPDATE_TRIGGER_FAILED,
	GS_ERROR_DS_DELETE_TRIGGER_FAILED,
	GS_ERROR_DS_GET_TRIGGER_FAILED,
	GS_ERROR_DS_PARSE_TRIGGER_FAILED,
	GS_ERROR_DS_INVALID_MVCC_MAP,
	GS_ERROR_DS_OM_INVALID,
	GS_ERROR_DS_COL_GET_LOCK_ID_INVALID,
	GS_ERROR_DS_TIM_GET_LOCK_ID_INVALID,
	GS_ERROR_DS_TIM_INVALID_ROW_ACCESS,
	GS_ERROR_DS_CON_GET_ROW_ID_LIST_FAILED,
	GS_ERROR_DS_CON_GET_OID_LIST_FAILED,
	GS_ERROR_DS_RS_PARTIAL_RS_CONTAINER_ALREADY_CLOSED,
	GS_ERROR_DS_CON_GET_ROW_LIST_FAILED,
	GS_ERROR_DS_UNEXPECTED_ERROR,
	GS_ERROR_DS_DS_SCHEMA_CHANGE_INVALID,
	GS_ERROR_DS_TIM_UPDATE_ROW_INVALID,
	GS_ERROR_DS_CON_STATUS_INVALID,
	GS_ERROR_DS_OUT_OF_RANGE,
	GS_ERROR_DS_CON_ACCESS_INVALID,
	GS_ERROR_CON_PUT_ROW_OPTION_INVALID,  
	GS_ERROR_DS_FETCH_PARAMETER_INVALID,
	GS_ERROR_DS_UNUSED1,  
	GS_ERROR_DS_CONTAINER_UNEXPECTEDLY_REMOVED,
	GS_ERROR_DS_CONTAINER_TYPE_UNKNOWN,
	GS_ERROR_DS_INPUT_MESSAGE_INVALID,
	GS_ERROR_DS_BACKGROUND_TASK_INVALID,

	GS_ERROR_JSON_INVALID_SYNTAX = 121000,
	GS_ERROR_JSON_KEY_NOT_FOUND,
	GS_ERROR_JSON_VALUE_OUT_OF_RANGE,
	GS_ERROR_JSON_UNEXPECTED_TYPE,

	GS_ERROR_HTTP_INTERNAL_ILLEGAL_OPERATION = 122000,
	GS_ERROR_HTTP_INTERNAL_ILLEGAL_PARAMETER,
	GS_ERROR_HTTP_UNEXPECTED_MESSAGE,
	GS_ERROR_HTTP_INVALID_MESSAGE,

	GS_ERROR_SA_INTERNAL_ILLEGAL_OPERATION = 123000,
	GS_ERROR_SA_INTERNAL_ILLEGAL_PARAMETER,
	GS_ERROR_SA_INVALID_CONFIG,
	GS_ERROR_SA_ADDRESS_CONFLICTED,
	GS_ERROR_SA_ADDRESS_NOT_ASSIGNED,
	GS_ERROR_SA_INVALID_ADDRESS,

	GS_ERROR_AUTH_INTERNAL_ILLEGAL_OPERATION = 124000,
	GS_ERROR_AUTH_INTERNAL_ILLEGAL_MESSAGE,
	GS_ERROR_AUTH_INVALID_CREDENTIALS
};

class GSCommonException : public util::Exception {
public:
	explicit GSCommonException(UTIL_EXCEPTION_CONSTRUCTOR_ARGS_DECL) throw() :
			Exception(UTIL_EXCEPTION_CONSTRUCTOR_ARGS_SET) {}
	virtual ~GSCommonException() throw() {}
};


class GSCommonExceptionRegenerator {
public:
	typedef util::Exception::NamedErrorCode NamedErrorCode;

	enum CauseType {
		CAUSE_NONE,
		CAUSE_CUSTOM,
		CAUSE_PLATFORM,
		CAUSE_UTILITY,
		CAUSE_GENERAL,
		CAUSE_OTHER
	};

	struct CustomFilter {
		static void filterCause(
				CauseType causeType, const NamedErrorCode *utilCode,
				NamedErrorCode &causeCode, bool &codeAsLiteral,
				const char8_t *&extraMessage);
	};

	template <typename V>
	GSCommonExceptionRegenerator(
			const V&, const util::Exception *customCause,
			UTIL_EXCEPTION_CONSTRUCTOR_ARGS_DECL) throw();

	template <typename T>
	T generate(const char8_t *typeName) const throw();

	template <typename V>
	void generate(
			const V&, util::Exception &dest,
			const char8_t *typeName) const throw();

private:
	typedef util::Exception::DuplicatedLiteralFlags DuplicatedLiteralFlags;
	typedef util::NormalOStringStream StrStream;
	typedef util::Exception::NoThrowString<StrStream::allocator_type> SafeStr;

	GSCommonExceptionRegenerator(const GSCommonExceptionRegenerator&);

	GSCommonExceptionRegenerator& operator=(
			const GSCommonExceptionRegenerator&);

	template <typename V>
	static util::Exception resolveCause(
			const V&, const std::exception *causeInHandling,
			const util::Exception *customCause,
			const char8_t *&extraMessage,
			NamedErrorCode &filteredCode,
			DuplicatedLiteralFlags &filteredLiteralFlags) throw();

	template <typename V>
	static void filterExtraMessage(
			const V&, CauseType causeType, const NamedErrorCode *utilCode,
			const NamedErrorCode &causeCode,
			const char8_t *&extraMessage) throw();

	template <typename V>
	static StrStream& generateMessage(
			const V&, const util::Exception &cause,
			const char8_t *extraMessage,
			const char8_t *message, StrStream &messageStream) throw();

	const char8_t *fileNameLiteral_;
	const char8_t *functionNameLiteral_;
	int32_t lineNumber_;
	const char8_t *extraMessage_;

	NamedErrorCode filteredCode_;
	DuplicatedLiteralFlags filteredLiteralFlags_;
	util::Exception cause_;

	StrStream messageStream_;
	SafeStr messageStr_;

	util::Exception base_;
};

class GSDefaultCommonExceptionRegenerator {
public:
	explicit GSDefaultCommonExceptionRegenerator(
			UTIL_EXCEPTION_CONSTRUCTOR_ARGS_DECL) throw() :
			base_(int(), NULL, UTIL_EXCEPTION_CONSTRUCTOR_ARGS_SET) {
	}

	template <typename T>
	T generate(const char8_t *typeName) const throw() {
		return base_.generate<T>(typeName);
	}

private:
	GSCommonExceptionRegenerator base_;
};

class GSExceptionCoder {
public:
	GSExceptionCoder() {
	}

	template<typename E, typename Str>
	void decode(
			util::ArrayByteInStream &in, E &dest, Str &strBuf,
			const util::Exception::NamedErrorCode &defaultNamedCode) const;

private:
	struct EmptyParameterBuilder {
		void appendParameter(const char8_t *name, const char8_t *value) {
			static_cast<void>(name);
			static_cast<void>(value);
		}
	};

	GSExceptionCoder(const GSExceptionCoder&);
	GSExceptionCoder& operator=(const GSExceptionCoder&);

	template<typename Str, typename ParamBuilder>
	void decodeInternal(
			util::ArrayByteInStream &in, util::Exception &dest,
			Str &strBuf, ParamBuilder &paramBuilder,
			const util::Exception::NamedErrorCode &defaultNamedCode) const;

	template<typename E>
	static typename E::ParametersBuilder& getParametersBuilder(
			E &dest, typename E::ParametersBuilder* = NULL) {
		return dest;
	}

	template<typename E>
	static EmptyParameterBuilder& getParametersBuilder(...) {
		static EmptyParameterBuilder builder;
		return builder;
	}
};

template <typename V>
GSCommonExceptionRegenerator::GSCommonExceptionRegenerator(
		const V&, const util::Exception *customCause,
		UTIL_EXCEPTION_CONSTRUCTOR_ARGS_LIST) throw() :
		fileNameLiteral_(fileNameLiteral),
		functionNameLiteral_(functionNameLiteral),
		lineNumber_(lineNumber),
		extraMessage_(NULL),
		filteredCode_(namedErrorCode),
		filteredLiteralFlags_(literalFlags),
		cause_(resolveCause(
				V(), causeInHandling, customCause, extraMessage_,
				filteredCode_, filteredLiteralFlags_)),
		messageStr_(generateMessage(
				V(), cause_, extraMessage_, message, messageStream_)) {
	static_cast<void>(typeNameLiteral);
	static_cast<void>(stackTraceMode);
}

template <typename T>
T GSCommonExceptionRegenerator::generate(
		const char8_t *typeName) const throw() {
	T exception;
	generate(int(), exception, typeName);

	return exception;
}

template <typename V>
void GSCommonExceptionRegenerator::generate(
		const V&, util::Exception &dest,
		const char8_t *typeName) const throw() {
	util::Exception cause;
	try {
		cause.assign(cause_, 1);
		throw cause;
	}
	catch (...) {
		dest = util::Exception(
				cause_.getNamedErrorCode(), messageStr_.get(),
				fileNameLiteral_, functionNameLiteral_, lineNumber_,
				&cause, typeName, util::Exception::STACK_TRACE_NONE,
				filteredLiteralFlags_);
	}
}

template <typename V>
util::Exception GSCommonExceptionRegenerator::resolveCause(
		const V&, const std::exception *causeInHandling,
		const util::Exception *customCause,
		const char8_t *&extraMessage,
		NamedErrorCode &filteredCode,
		DuplicatedLiteralFlags &filteredLiteralFlags) throw() {
	util::Exception baseCause;

	CauseType causeType;
	NamedErrorCode utilCode;
	if (causeInHandling == NULL) {
		causeType = CAUSE_NONE;
	}
	else if (customCause != NULL) {
		baseCause = *customCause;
		causeType = CAUSE_CUSTOM;
	}
	else {
		try {
			throw;
		}
		catch (const util::PlatformException &e) {
			baseCause = e;
			causeType = CAUSE_PLATFORM;
		}
		catch (const util::UtilityException &e) {
			baseCause = e;
			causeType = CAUSE_UTILITY;
			utilCode = e.getNamedErrorCode();
		}
		catch (const util::Exception &e) {
			baseCause = e;
			causeType = CAUSE_GENERAL;
		}
		catch (...) {
			causeType = CAUSE_OTHER;
			try {
				UTIL_RETHROW_UTIL_ERROR(CODE_DEFAULT, *causeInHandling, "");
			}
			catch (const util::UtilityException &e2) {
				utilCode = e2.getNamedErrorCode();
			}
			catch (...) {
			}
		}
	}

	const uint32_t codeFlag = (1U << util::Exception::FIELD_ERROR_CODE_NAME);
	bool codeAsLiteral = ((filteredLiteralFlags & codeFlag) == 0);
	if (filteredCode.isEmpty()) {
		filteredCode = baseCause.getNamedErrorCode();
		codeAsLiteral = ((baseCause.inheritLiteralFlags(codeFlag) & codeFlag) == 0);
	}

	const NamedErrorCode *utilCodeRef =
			(utilCode.isEmpty() ? NULL : &utilCode);

	filterExtraMessage(
			V(), causeType, utilCodeRef, filteredCode, extraMessage);
	CustomFilter::filterCause(
			causeType, utilCodeRef, filteredCode, codeAsLiteral, extraMessage);

	filteredLiteralFlags = static_cast<DuplicatedLiteralFlags>(
			(filteredLiteralFlags & ~codeFlag) |
			(codeAsLiteral ? 0U : codeFlag));

	util::Exception cause(
			filteredCode, NULL, NULL, NULL, 0, NULL, NULL,
			util::Exception::STACK_TRACE_NONE, filteredLiteralFlags);
	cause.append(baseCause);
	return cause;
}

template <typename V>
void GSCommonExceptionRegenerator::filterExtraMessage(
		const V&, CauseType causeType, const NamedErrorCode *utilCode,
		const NamedErrorCode &causeCode,
		const char8_t *&extraMessage) throw() {
	if (extraMessage != NULL) {
		return;
	}

	bool unexpected = false;
	if (utilCode == NULL) {
		if (causeCode.isEmpty()) {
			if (causeType == CAUSE_OTHER) {
				unexpected = true;
			}
			else if (causeType != CAUSE_PLATFORM) {
				extraMessage = "Internal error by empty error code";
			}
		}
	}
	else {
		switch (utilCode->getCode()) {
		case util::UtilityException::CODE_NO_MEMORY:
			break;
		case util::UtilityException::CODE_MEMORY_LIMIT_EXCEEDED:
			break;
		case util::UtilityException::CODE_SIZE_LIMIT_EXCEEDED:
			break;
		default:
			if (causeType == CAUSE_OTHER) {
				unexpected = true;
			}
			else {
				extraMessage = "Internal error by unexpected utility problem";
			}
			break;
		}
	}
	if (unexpected) {
		extraMessage = "Internal error by unexpected exception type";
	}
}

template <typename V>
GSCommonExceptionRegenerator::StrStream&
GSCommonExceptionRegenerator::generateMessage(
		const V&, const util::Exception &cause, const char8_t *extraMessage,
		const char8_t *message, StrStream &messageStream) throw() {

	StrStream baseStream;
	baseStream << cause.getField(util::Exception::FIELD_MESSAGE, 1);

	SafeStr baseStr(baseStream);
	const char8_t *strPtr = baseStr.get();
	const char8_t *baseMessage = (message != NULL && strlen(message) > 0) ?
			message : (strPtr != NULL && strlen(strPtr) > 0 ? strPtr : NULL);

	if (extraMessage == NULL) {
		if (baseMessage != NULL) {
			messageStream << baseMessage;
		}
	}
	else {
		messageStream << extraMessage;
		if (baseMessage != NULL) {
			messageStream << " (reason=" << baseMessage << ")";
		}
	}

	return messageStream;
}

template<typename E, typename Str>
void GSExceptionCoder::decode(
		util::ArrayByteInStream &in, E &dest, Str &strBuf,
		const util::Exception::NamedErrorCode &defaultNamedCode) const {
	decodeInternal(
			in, dest, strBuf, getParametersBuilder(dest), defaultNamedCode);
}

template<typename Str, typename ParamBuilder>
void GSExceptionCoder::decodeInternal(
		util::ArrayByteInStream &in, util::Exception &dest,
		Str &strBuf, ParamBuilder &paramBuilder,
		const util::Exception::NamedErrorCode &defaultNamedCode) const {
	strBuf.clear();
	dest = util::Exception();

	util::Exception top;
	util::Exception following;

	int32_t count;
	in >> count;

	for (int32_t i = 0; i < count; i++) {
		int32_t errorCode;
		Str optionMessage(strBuf);
		Str type(strBuf);
		Str fileName(strBuf);
		Str functionName(strBuf);
		int32_t lineNumber;

		in >> errorCode;
		in >> optionMessage;
		in >> type;
		in >> fileName;
		in >> functionName;
		in >> lineNumber;

		(i == 0 ? top : following).append(util::Exception(
				util::Exception::NamedErrorCode(errorCode),
				optionMessage.c_str(),
				fileName.c_str(),
				functionName.c_str(),
				lineNumber,
				NULL,
				type.c_str(),
				util::Exception::STACK_TRACE_NONE,
				util::Exception::LITERAL_ALL_DUPLICATED));
	}

	if (in.base().remaining() > 0 || !defaultNamedCode.isEmpty()) {
		Str errorCodeName(strBuf);

		if (in.base().remaining() > 0) {
			in >> errorCodeName;
		}

		util::Exception::NamedErrorCode topCode(top.getErrorCode());
		if (topCode.isEmpty()) {
			topCode = defaultNamedCode;
		}
		else {
			topCode = util::Exception::NamedErrorCode(
					top.getErrorCode(), errorCodeName.c_str());
		}

		dest.append(util::Exception(
				topCode,
				UTIL_EXCEPTION_CREATE_MESSAGE_CHARS(
						top.getField(util::Exception::FIELD_MESSAGE)),
				UTIL_EXCEPTION_CREATE_MESSAGE_CHARS(
						top.getField(util::Exception::FIELD_FILE_NAME)),
				UTIL_EXCEPTION_CREATE_MESSAGE_CHARS(
						top.getField(util::Exception::FIELD_FUNCTION_NAME)),
				top.getLineNumber(),
				NULL,
				UTIL_EXCEPTION_CREATE_MESSAGE_CHARS(
						top.getField(util::Exception::FIELD_TYPE_NAME)),
				util::Exception::STACK_TRACE_NONE,
				util::Exception::LITERAL_ALL_DUPLICATED));
	}
	else {
		dest.append(top);
	}

	if (count > 1) {
		dest.append(following);
	}

	if (in.base().remaining() > 0) {
		int32_t paramCount;
		in >> paramCount;

		for (int32_t i = 0; i < paramCount; i++) {
			Str name(strBuf);
			Str value(strBuf);

			in >> name;
			in >> value;

			paramBuilder.appendParameter(name.c_str(), value.c_str());
		}
	}
}

#define GS_COMMON_EXCEPTION_NAMED_CODE(codeSymbol) \
		util::Exception::makeNamedErrorCode(codeSymbol, #codeSymbol, "GS_ERROR_")
#define GS_COMMON_EXCEPTION_CREATE_DETAIL(type, errorCode, cause, message) \
		UTIL_EXCEPTION_CREATE_DETAIL( \
				type, GS_COMMON_EXCEPTION_NAMED_CODE(errorCode), cause, message)

#define GS_COMMON_EXCEPTION_CONVERT_CUSTOM(type, defaultCode, cause, message) \
		GS_COMMON_EXCEPTION_CREATE_DETAIL( \
				GSDefaultCommonExceptionRegenerator, \
				defaultCode, &(cause), message).generate<type>(#type)
#define GS_COMMON_EXCEPTION_CONVERT_CODED(defaultCode, cause, message) \
		GS_COMMON_EXCEPTION_CONVERT_CUSTOM( \
				util::Exception, defaultCode, cause, message)
#define GS_COMMON_EXCEPTION_CONVERT(cause, message) \
		GS_COMMON_EXCEPTION_CONVERT_CODED(0, cause, message)

#define GS_COMMON_THROW_CUSTOM_ERROR(type, errorCode, message) \
		throw GS_COMMON_EXCEPTION_CREATE_DETAIL(type, errorCode, NULL, message)
#define GS_COMMON_RETHROW_CUSTOM_ERROR(type, defaultCode, cause, message) \
		throw GS_COMMON_EXCEPTION_CONVERT_CUSTOM( \
				type, defaultCode, cause, message)

#if GS_ERROR_COMMON_ONLY
#define GS_COMMON_THROW_USER_ERROR(errorCode, message) \
	UTIL_THROW_ERROR(errorCode, message)
#define GS_COMMON_RETHROW_USER_ERROR(cause, message) \
	UTIL_RETHROW(0, cause, message)
#define GS_COMMON_EXCEPTION_MESSAGE(cause) ""
#else
#define GS_COMMON_THROW_USER_ERROR(errorCode, message) \
	GS_THROW_USER_ERROR(errorCode, message)
#define GS_COMMON_RETHROW_USER_ERROR(cause, message) \
	GS_RETHROW_USER_ERROR(cause, message)
#define GS_COMMON_EXCEPTION_MESSAGE(cause) \
	GS_EXCEPTION_MESSAGE(cause)
#endif

#endif
