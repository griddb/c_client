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
	@brief Definition of Client logging
*/
#ifndef CLIENT_LOGGING_H_
#define CLIENT_LOGGING_H_

#include "client.h"

class CallLogger : public GSInterceptor {
public:
	explicit CallLogger(GSInterceptorManager &manager);
	virtual ~CallLogger();

	virtual bool isActivated(GSResourceType::Id type);

	virtual bool start(
			const FunctionInfo &funcInfo, const ParameterList &args,
			const Parameter &ret);
	virtual void finish(
			const FunctionInfo &funcInfo, const ParameterList &args);
	virtual bool createLockFile(const std::string &fileName);

private:
	bool isLoggingTargetMethod(const FunctionInfo &funcInfo);
	void setLogTargetFunctionNames();
	void setLogTargetFunctionNamesOfContainer();
	void setLogTargetFunctionNamesOfCollection();
	void setLogTargetFunctionNamesOfTimeSeries();
	void setLogTargetFunctionNamesOfGridStore();
	void setLogTargetFunctionNamesOfQuery();
	void setLogTargetFunctionNamesOfGridStoreFactory();

	std::set<std::string> logTargetFunctionNames_;
	std::vector<util::PIdFile*> pidFiles_;
};

#endif	
