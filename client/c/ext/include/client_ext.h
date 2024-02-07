/*
   Copyright (c) 2020 TOSHIBA Digital Solutions Corporation

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
	@brief Definition of client extension
*/

#ifndef CLIENT_EXT_H_
#define CLIENT_EXT_H_

#include "util/system.h"
#include "gridstore.h"

#ifdef GS_EXTENSION_LIB_INSIDE
#define GS_CLIENT_EXT_PUBLIC UTIL_DLL_PUBLIC
#else
#define GS_CLIENT_EXT_PUBLIC
#endif

extern "C" {
GS_CLIENT_EXT_PUBLIC int32_t UTIL_DLL_CALL gsGetClientExtensionFunctions(
		const void *const **funcList, size_t *funcCount,
		int32_t reqVersion, int32_t *libVersion);
}

class GSClientExtension {
public:
	enum {
		// 0: V4.5
		// 1: V5.4
		CURRENT_LIB_VERSION = 1
	};

	struct Functions;
};

struct GSClientExtension::Functions {
	enum {
		FUNC_EXCEPTION_PROVIDER,
		FUNC_SET_UP_FACTORY,

		END_FUNC
	};

	template<size_t F, int = 0>
	struct Traits {
	};

	template<int C> struct Traits<FUNC_EXCEPTION_PROVIDER, C> {
		typedef util::LibraryFunctions::ProviderFunc Func;
	};
	template<int C> struct Traits<FUNC_SET_UP_FACTORY, C> {
		typedef int32_t (*Func)(
				GSGridStoreFactory*, util::LibraryFunctions::ProviderFunc,
				UtilExceptionTag**);
	};
};

class ExtGridStoreFactory {
public:
	struct Functions;
};

struct ExtGridStoreFactory::Functions {
	enum {
		FUNC_EXCEPTION_PROVIDER,
		FUNC_SET_TRANSPORT_PROVIDER,

		END_FUNC
	};

	template<size_t F, int = 0>
	struct Traits {
	};

	template<int C> struct Traits<FUNC_EXCEPTION_PROVIDER, C> {
		typedef util::LibraryFunctions::ProviderFunc Func;
	};
	template<int C> struct Traits<FUNC_SET_TRANSPORT_PROVIDER, C> {
		typedef int32_t (*Func)(
				GSGridStoreFactory*, util::LibraryFunctions::ProviderFunc,
				UtilExceptionTag**);
	};
};

struct ExtTranportPropertiesTag {};

class ExtTransportProvider {
public:
	struct Functions;

	static const char8_t *const* getReservedTransportPropertyKeys() {
		static const char8_t *const keyList[] = {
			"sslMode", NULL
		};
		return keyList;
	}
};

struct SocketFactoryTag;

struct ExtTransportProvider::Functions {
	enum {
		FUNC_EXCEPTION_PROVIDER,
		FUNC_FILTER_PROPERTIES,
		FUNC_IS_PLAIN_SOCKET_ALLOWED,
		FUNC_CREATE_SECURE_SOCKET_FACTORY,
		FUNC_CLOSE_PROPERTIES,

		END_FUNC
	};

	template<size_t F, int = 0>
	struct Traits {
	};

	template<int C> struct Traits<FUNC_EXCEPTION_PROVIDER, C> {
		typedef util::LibraryFunctions::ProviderFunc Func;
	};
	template<int C> struct Traits<FUNC_FILTER_PROPERTIES, C> {
		typedef int32_t (*Func)(
				const GSPropertyEntry*, size_t, const GSPropertyEntry**,
				size_t*, ExtTranportPropertiesTag**, UtilExceptionTag**);
	};
	template<int C> struct Traits<FUNC_IS_PLAIN_SOCKET_ALLOWED, C> {
		typedef int32_t (*Func)(
				const GSPropertyEntry*, size_t, int32_t*, UtilExceptionTag**);
	};
	template<int C> struct Traits<FUNC_CREATE_SECURE_SOCKET_FACTORY, C> {
		typedef int32_t (*Func)(
				const GSPropertyEntry*, size_t, SocketFactoryTag**,
				util::LibraryFunctions::ProviderFunc*, UtilExceptionTag**);
	};
	template<int C> struct Traits<FUNC_CLOSE_PROPERTIES, C> {
		typedef void (*Func)(ExtTranportPropertiesTag*);
	};
};

#endif
