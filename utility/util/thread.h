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
/*!
	@file
    @brief Definition of Utility of threads
*/
#ifndef UTIL_THREAD_H_
#define UTIL_THREAD_H_

#include "util/type.h"
#include "util/file.h"
#include <memory>

/*!
    @brief Declaration of thread local.
*/
#ifdef WIN32
#define UTIL_THREAD_LOCAL __declspec(thread)
#else
#define UTIL_THREAD_LOCAL __thread
#endif

namespace util {

namespace detail {
template<typename T> struct NonAtomic {
	NonAtomic(const T &value = T()) : value_(value) {}
	NonAtomic& operator=(const T &value) {
		value_ = value;
		return *this;
	}
	NonAtomic operator++(int) { return value_++; }
	NonAtomic operator++() { return ++value_; }
	NonAtomic operator--(int) { return value_--; }
	NonAtomic operator--() { return --value_; }
	operator T() const { return value_; }
	T value_;
};
template<typename T> struct AtomicImplResolver {
	typedef NonAtomic<T> Result;
};
}


/*!
    @brief Value for atomic operation.
*/
template<typename T>
class Atomic : public detail::AtomicImplResolver<T>::Result {
public:
	typedef Atomic<T> ThisType;
	typedef T ValueType;
	typedef typename detail::AtomicImplResolver<T>::Result BaseType;

	Atomic() {}
	~Atomic() {}
	explicit Atomic(ValueType value) : BaseType(value) {}

	ThisType& operator=(ValueType value) {
		BaseType::operator=(value);
		return *this;
	}
};

template<typename L>
class LockGuard {
public:
	explicit LockGuard(L &lockObject);
	~LockGuard();

private:
	LockGuard(const LockGuard&);
	LockGuard& operator=(const LockGuard&);

	L &lockObject_;
};

template<typename L>
inline LockGuard<L>::LockGuard(L &lockObject) : lockObject_(lockObject) {
	lockObject_.lock();
}

template<typename L>
inline LockGuard<L>::~LockGuard() {
	lockObject_.unlock();
}

/*!
    @brief Locks in the scope if object exists.
*/
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

class MutexAttribute;

extern UTIL_FLAG_TYPE UTIL_MUTEX_DEFAULT; 
extern UTIL_FLAG_TYPE UTIL_MUTEX_FAST; 
extern UTIL_FLAG_TYPE UTIL_MUTEX_RECURSIVE; 
extern UTIL_FLAG_TYPE UTIL_MUTEX_ERRORCHECK; 

/*!
    @brief Mutex for prevention of concurrent execution.
*/
class Mutex {
public:
	explicit Mutex(int type = UTIL_MUTEX_FAST);

	explicit Mutex(const MutexAttribute *attr);

	virtual ~Mutex();

	void lock(void);

	bool tryLock(void);

	bool tryLock(uint32_t msec);

	void unlock(void);

private:
	Mutex(const Mutex&);
	Mutex& operator=(const Mutex&);

	struct Data;
	UTIL_UNIQUE_PTR<Data> data_;
};

/*!
    @brief Attributes of Mutex.
*/
class MutexAttribute {
public:
	MutexAttribute();

	virtual ~MutexAttribute();

public:
	void setType(int type = UTIL_MUTEX_DEFAULT);

	void getType(int &type) const;

	void setShared(bool shared);

	void getShared(bool &shared) const;

private:
	MutexAttribute(const MutexAttribute&);
	MutexAttribute& operator=(const MutexAttribute&);

	friend class Mutex;
	struct Data;
	UTIL_UNIQUE_PTR<Data> data_;
};

class ThreadRunner {
public:
};

class Thread : public ThreadRunner {
public:
	static void sleep(uint32_t millisecTime);
};

} 

#endif
