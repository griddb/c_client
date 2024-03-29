﻿/*
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
	@brief Definition of ServiceAddressResolver
*/
#ifndef SERVICE_ADDRESS_H_
#define SERVICE_ADDRESS_H_

#include "util/container.h"
#include "socket_wrapper.h"

namespace picojson {
class value;
} 

class ServiceAddressResolver {
public:
	struct Config {
		Config();

		const char8_t *providerURL_;
		int addressFamily_;
		SocketFactory *plainSocketFactory_;
		SocketFactory *secureSocketFactory_;
	};

	typedef util::StdAllocator<void, void> Allocator;

	ServiceAddressResolver(const Allocator &alloc, const Config &config);

	~ServiceAddressResolver();

	const Config& getConfig() const;

	static void checkConfig(
			const Allocator &alloc, const Config &config, bool &secure);

	void initializeType(const ServiceAddressResolver &another);

	void initializeType(uint32_t type, const char8_t *name);

	uint32_t getTypeCount() const;

	uint32_t getType(const char8_t *name) const;

	bool update();

	bool checkUpdated(size_t *readSize = NULL);

	bool isChanged() const;

	bool isAvailable() const;

	bool isSameEntries(const ServiceAddressResolver &another) const;

	size_t getEntryCount() const;

	util::SocketAddress getAddress(size_t index, uint32_t type) const;

	void setAddress(
			size_t index, uint32_t type, const util::SocketAddress &addr);

	void importFrom(const picojson::value &value, bool strict = true);

	bool exportTo(picojson::value &value) const;
 
	void validate();

	void normalize();

	util::IOPollHandler* getIOPollHandler();
	util::IOPollEvent getIOPollEvent();

	util::SocketAddress makeSocketAddress(const char8_t *host, int64_t port);

private:
	struct Entry;
	struct EntryLess;
	struct ProviderContext;

	typedef util::BasicString< char8_t, std::char_traits<char8_t>,
			util::StdAllocator<char8_t, void> > String;

	typedef std::vector<String, util::StdAllocator<String, void> > TypeList;
	typedef std::map<
			String, uint32_t, std::map<String, uint32_t>::key_compare,
			util::StdAllocator<
					std::pair<const String, uint32_t>, void> > TypeMap;

	typedef std::multiset<
			util::SocketAddress, std::set<util::SocketAddress>::key_compare,
			util::StdAllocator<util::SocketAddress, void> > AddressSet;

	typedef std::vector<Entry, util::StdAllocator<Entry, void> > EntryList;

	friend std::ostream& operator<<(
			std::ostream &s, const ProviderContext &cxt);

	static const char8_t JSON_KEY_ADDRESS[];
	static const char8_t JSON_KEY_PORT[];

	static SocketFactory DEFAULT_SOCKET_FACTORY;

	ServiceAddressResolver(const ServiceAddressResolver&);
	ServiceAddressResolver& operator=(const ServiceAddressResolver&);

	void initializeRaw();

	void completeInit();
	void checkEntry(size_t index) const;
	void checkType(uint32_t type) const;

	const char8_t* getTypeName(uint32_t type) const;

	static bool isSameEntries(
			const EntryList &list1, bool normalized1,
			const EntryList &list2, bool normalized2);

	static void normalizeEntries(EntryList *entryList);

	void createSocket(AbstractSocket &socket) const;
	static util::IOPollEvent resolvePollEvent(
			AbstractSocket &socket, util::IOPollEvent base);

	Allocator alloc_;
	Config config_;

	String providerURL_;

	TypeList typeList_;
	TypeMap typeMap_;

	AddressSet addressSet_;
	EntryList entryList_;

	bool initialized_;
	std::pair<bool, bool> updated_;
	bool changed_;
	bool normalized_;
	bool secure_;

	ProviderContext *providerCxt_;
};

struct ServiceAddressResolver::Entry {
	typedef std::vector<
			util::SocketAddress,
			util::StdAllocator<util::SocketAddress, void> > AddressList;

	Entry(const Allocator &alloc, size_t typeCount);

	int32_t compare(const Entry &another) const;

	AddressList list_;
};

struct ServiceAddressResolver::EntryLess {
	bool operator()(const Entry &entry1, const Entry &entry2) const;
};

#endif
