/*
   Copyright (c) 2019 TOSHIBA Digital Solutions Corporation

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
	@brief Implementation of Client logging
*/
#include "client_logging.h"
#include "util/trace.h"

UTIL_TRACER_DECLARE(API_CALL);

class ClientTraceFormatter : public util::TraceFormatter {
public:
	ClientTraceFormatter() {
		util::TraceManager::getInstance().setFormatter(this);
	}

	virtual ~ClientTraceFormatter() {
		util::TraceManager::getInstance().setFormatter(NULL);
	}

protected:
	virtual bool formatMainLocation(
			std::ostream &stream, const util::TraceRecord &record,
			const char8_t *separator) {
		static_cast<void>(stream);
		static_cast<void>(record);
		static_cast<void>(separator);
		return false;
	}

private:
	static ClientTraceFormatter instance_;
};

ClientTraceFormatter ClientTraceFormatter::instance_;

CallLogger::CallLogger(GSInterceptorManager &manager) :
		GSInterceptor(manager) {
	util::Tracer &tracer = UTIL_TRACER_RESOLVE(API_CALL);
	setLogTargetFunctionNames();
}

CallLogger::~CallLogger() {
	for (size_t i = 0; i < pidFiles_.size(); i++) {
		delete pidFiles_[i];
	}
}

bool CallLogger::isActivated(GSResourceType::Id type) {
	switch (type) {
	case GSResourceType::GRID_STORE_FACTORY:
	case GSResourceType::GRID_STORE:
	case GSResourceType::CONTAINER:
	case GSResourceType::QUERY:
		return true;
	default:
		return false;
	}
}

bool CallLogger::start(
		const FunctionInfo &funcInfo, const ParameterList &args,
		const Parameter &ret) {
	if (!isLoggingTargetMethod(funcInfo)) {
		return false;
	}

	util::NormalOStringStream oss;
	size_t count = args.getSize();
	for (size_t i = 0; i < count; i++) {
		const Parameter &arg = args.at(i);
		oss << " " << arg.getName() << "=";
		arg.formatValue(oss);
	}
	UTIL_TRACE_DEBUG(API_CALL, "start " << funcInfo << oss.str());

	return false;
}

void CallLogger::finish(
		const FunctionInfo &funcInfo, const ParameterList &args) {
	if (!isLoggingTargetMethod(funcInfo)) {
		return;
	}

	ErrorStack *errorStack;
	util::Mutex *mutex;
	if (args.getSize() > 0 && args.at(0).isResource() &&
			(errorStack = GSResourceHeader::findErrorStack(
					args.at(0).findResource(), mutex)) != NULL &&
			errorStack->getSize() > 0 &&
			errorStack->getErrorCode(0) != 0) {
		util::DynamicLockGuard<util::Mutex> guard(mutex);
		UTIL_TRACE_DEBUG(API_CALL, "error:" <<
				errorStack->getErrorCode(0) << " " << funcInfo);
	}
	else {
		UTIL_TRACE_DEBUG(API_CALL, "finish " << funcInfo);
	}
}

bool CallLogger::createLockFile(const std::string &fileName) {
	util::PIdFile *file = new util::PIdFile();
	try {
		file->open(fileName.c_str());
		pidFiles_.push_back(file);
	}
	catch (...) {
		delete file;
		return false;
	}
	return true;
}

bool CallLogger::isLoggingTargetMethod(const FunctionInfo &funcInfo) {
	std::set<std::string>::iterator itr = logTargetFunctionNames_.find(funcInfo.getRawName());
	if (itr == logTargetFunctionNames_.end()) {
		return false;
	}
	return true;
}

void CallLogger::setLogTargetFunctionNames() {
	setLogTargetFunctionNamesOfContainer();
	setLogTargetFunctionNamesOfCollection();
	setLogTargetFunctionNamesOfTimeSeries();
	setLogTargetFunctionNamesOfGridStore();
	setLogTargetFunctionNamesOfQuery();
	setLogTargetFunctionNamesOfGridStoreFactory();
}

void CallLogger::setLogTargetFunctionNamesOfContainer() {
	logTargetFunctionNames_.insert("gsGetRow");
	logTargetFunctionNames_.insert("gsGetRowForUpdate");
	logTargetFunctionNames_.insert("gsGetRowByInteger");
	logTargetFunctionNames_.insert("gsGetRowByLong");
	logTargetFunctionNames_.insert("gsGetRowByTimestamp");
	logTargetFunctionNames_.insert("gsGetRowByString");

	logTargetFunctionNames_.insert("gsPutRow");
	logTargetFunctionNames_.insert("gsPutMultipleRows");
	logTargetFunctionNames_.insert("gsPutRowByInteger");
	logTargetFunctionNames_.insert("gsPutRowByLong");
	logTargetFunctionNames_.insert("gsPutRowByTimestamp");
	logTargetFunctionNames_.insert("gsPutRowByString");

	logTargetFunctionNames_.insert("gsDeleteRow");
	logTargetFunctionNames_.insert("gsDeleteRowByInteger");
	logTargetFunctionNames_.insert("gsDeleteRowByLong");
	logTargetFunctionNames_.insert("gsDeleteRowByTimestamp");
	logTargetFunctionNames_.insert("gsDeleteRowByString");
	logTargetFunctionNames_.insert("gsDeleteCurrentRow");

	logTargetFunctionNames_.insert("gsQuery");
}

void CallLogger::setLogTargetFunctionNamesOfCollection() {
	logTargetFunctionNames_.insert("gsQueryByGeometry");
	logTargetFunctionNames_.insert("gsQueryByGeometryWithDisjointCondition");
}

void CallLogger::setLogTargetFunctionNamesOfTimeSeries() {
	logTargetFunctionNames_.insert("gsGetRowByBaseTime");

	logTargetFunctionNames_.insert("gsAppendTimeSeriesRow");

	logTargetFunctionNames_.insert("gsQueryByTimeSeriesRange");
	logTargetFunctionNames_.insert("gsQueryByTimeSeriesOrderedRange");
	logTargetFunctionNames_.insert("gsQueryByTimeSeriesSampling");

	logTargetFunctionNames_.insert("gsAggregateTimeSeries");
	logTargetFunctionNames_.insert("gsInterpolateTimeSeriesRow");
}

void CallLogger::setLogTargetFunctionNamesOfGridStore() {
	logTargetFunctionNames_.insert("gsGetCollection");
	logTargetFunctionNames_.insert("gsGetContainerInfo");
	logTargetFunctionNames_.insert("gsGetTimeSeries");
	logTargetFunctionNames_.insert("gsGetContainerGeneral");
	logTargetFunctionNames_.insert("gsGetCollectionGeneral");
	logTargetFunctionNames_.insert("gsGetTimeSeriesGeneral");
	logTargetFunctionNames_.insert("gsGetMultipleContainerRows");

	logTargetFunctionNames_.insert("gsPutContainer");
	logTargetFunctionNames_.insert("gsPutCollection");
	logTargetFunctionNames_.insert("gsPutTimeSeries");
	logTargetFunctionNames_.insert("gsPutContainerGeneral");
	logTargetFunctionNames_.insert("gsPutCollectionGeneral");
	logTargetFunctionNames_.insert("gsPutTimeSeriesGeneral");
	logTargetFunctionNames_.insert("gsPutMultipleContainerRows");

	logTargetFunctionNames_.insert("gsFetchAll");
}

void CallLogger::setLogTargetFunctionNamesOfQuery() {
	logTargetFunctionNames_.insert("gsFetch");
	logTargetFunctionNames_.insert("gsSetFetchOption");
	logTargetFunctionNames_.insert("gsGetRowSet");
}

void CallLogger::setLogTargetFunctionNamesOfGridStoreFactory() {
	logTargetFunctionNames_.insert("gsGetGridStore");
}
