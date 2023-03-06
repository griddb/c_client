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
#include "util/thread.h"
#include "util/code.h"
#include "util/time.h"
#include "util/os.h"

#ifdef _WIN32
#include <process.h>
#else
#include <sys/syscall.h>
#endif

#include <assert.h>

#define UTIL_WIN_COND_CHANGE

namespace util {



#ifndef _WIN32
UTIL_FLAG_TYPE UTIL_MUTEX_DEFAULT = PTHREAD_MUTEX_DEFAULT;
UTIL_FLAG_TYPE UTIL_MUTEX_FAST = PTHREAD_MUTEX_NORMAL;
UTIL_FLAG_TYPE UTIL_MUTEX_RECURSIVE = PTHREAD_MUTEX_RECURSIVE;
UTIL_FLAG_TYPE UTIL_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK;
#else
UTIL_FLAG_TYPE UTIL_MUTEX_DEFAULT = 0;
UTIL_FLAG_TYPE UTIL_MUTEX_FAST = 0;
UTIL_FLAG_TYPE UTIL_MUTEX_RECURSIVE = 0;
UTIL_FLAG_TYPE UTIL_MUTEX_ERRORCHECK = 0;
#endif

struct Mutex::Data {
#ifdef UTIL_HAVE_POSIX_MUTEX
	Data(const pthread_mutexattr_t *attr);
#else
	Data();
#endif

	~Data();

#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_mutex_t mutex_;
#else
	CRITICAL_SECTION cs_;
#endif
};

struct MutexAttribute::Data {
#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_mutexattr_t attr_;
#endif
};

#ifdef UTIL_HAVE_POSIX_MUTEX
Mutex::Data::Data(const pthread_mutexattr_t *attr) {
	if (0 != pthread_mutex_init(&mutex_, attr)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
}
#else
Mutex::Data::Data() {
	InitializeCriticalSection(&cs_);
}
#endif

Mutex::Data::~Data() {
#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_mutex_destroy(&mutex_);
#else
	DeleteCriticalSection(&cs_);
#endif
}

Mutex::Mutex(int type) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	MutexAttribute attr;
	attr.setType(type);
	data_.reset(new Data(&attr.data_->attr_));
#else
	data_.reset(new Data());
#endif 
}

Mutex::Mutex(const MutexAttribute *attr) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	const pthread_mutexattr_t *baseAttr = NULL;
	if (attr != NULL) {
		baseAttr = &attr->data_->attr_;
	}
	data_.reset(new Data(baseAttr));
#else
	data_.reset(new Data());
#endif 
}

Mutex::~Mutex() {
}

void Mutex::lock(void) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_mutex_lock(&data_->mutex_);
#else
	EnterCriticalSection(&data_->cs_);
#endif
}

void Mutex::unlock(void) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_mutex_unlock(&data_->mutex_);
#else
	LeaveCriticalSection(&data_->cs_);
#endif
}

bool Mutex::tryLock(void) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	return (0 == pthread_mutex_trylock(&data_->mutex_));
#else
	return (TryEnterCriticalSection(&data_->cs_) ? true : false);
#endif
}

bool Mutex::tryLock(uint32_t msec) {
	(void) msec;
	UTIL_THROW_NOIMPL_UTIL();
}

MutexAttribute::MutexAttribute() : data_(new Data()) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	if (0 != pthread_mutexattr_init(&data_->attr_)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#endif
}

MutexAttribute::~MutexAttribute() {
#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_mutexattr_destroy(&data_->attr_);
#endif
}

void MutexAttribute::setType(int type) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	if (0 != pthread_mutexattr_settype(&data_->attr_, type)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}

void MutexAttribute::getType(int &type) const {
#ifdef UTIL_HAVE_POSIX_MUTEX
	if (0 != pthread_mutexattr_gettype(&data_->attr_, &type)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}

void MutexAttribute::setShared(bool shared) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	if (0 != pthread_mutexattr_setpshared(&data_->attr_,
			shared ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}

void MutexAttribute::getShared(bool &shared) const {
#ifdef UTIL_HAVE_POSIX_MUTEX
	int type;
	if (0 != pthread_mutexattr_getpshared(&data_->attr_, &type)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
	shared = (type == PTHREAD_PROCESS_SHARED);
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}


struct RWLock::Data {
#ifdef UTIL_HAVE_POSIX_MUTEX
	Data(const pthread_rwlockattr_t *attr);
#else
	Data();
	void unlock();
	void addWriter();
	void removeWriter();
#endif

	~Data();
	void close();

#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_rwlock_t rwlock_;
#else
	HANDLE mutex_;
	HANDLE readEvent_;
	HANDLE writeEvent_;
	size_t readerCount_;
	size_t waitingWriterCount_;
	size_t writerCount_;
#endif
};

struct RWLockAttribute::Data {
#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_rwlockattr_t attr_;
#endif
};

#ifdef UTIL_HAVE_POSIX_MUTEX
RWLock::Data::Data(const pthread_rwlockattr_t *attr) {
	if (0 != pthread_rwlock_init(&rwlock_, attr)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
}
#else
RWLock::Data::Data() {
	readerCount_ = 0;
	waitingWriterCount_ = 0;
	writerCount_ = 0;
	mutex_ = NULL;
	readEvent_ = NULL;
	writeEvent_ = NULL;
	try {
		mutex_ = CreateMutexW(NULL, FALSE, NULL);
		if (mutex_ == NULL) {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
		readEvent_ = CreateEventW(NULL, TRUE, TRUE, NULL);
		if (readEvent_ == NULL) {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
		writeEvent_ = CreateEventW(NULL, TRUE, TRUE, NULL);
		if (writeEvent_ == NULL) {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
	}
	catch (...) {
		close();
		throw;
	}
}
#endif

#ifndef UTIL_HAVE_POSIX_MUTEX
void RWLock::Data::unlock() {
	switch (WaitForSingleObject(mutex_, INFINITE)) {
	case WAIT_OBJECT_0:
		if (readerCount_ == 0) {
			ReleaseMutex(mutex_);
			UTIL_THROW_UTIL_ERROR(CODE_ILLEGAL_OPERATION,
					"Not locked");
		}

		writerCount_ = 0;
		if (waitingWriterCount_ == 0) {
			SetEvent(readEvent_);
		}
		if ((--readerCount_) == 0) {
			SetEvent(writeEvent_);
		}
		ReleaseMutex(mutex_);
		break;
	case WAIT_FAILED:
		UTIL_THROW_PLATFORM_ERROR(NULL);
	default:
		UTIL_THROW_UTIL_ERROR_CODED(CODE_INVALID_STATUS);
	}
}
#endif

#ifndef UTIL_HAVE_POSIX_MUTEX
void RWLock::Data::addWriter() {
	switch (WaitForSingleObject(mutex_, INFINITE)) {
	case WAIT_OBJECT_0:
		if (++waitingWriterCount_ == 1) {
			ResetEvent(readEvent_);
		}
		ReleaseMutex(mutex_);
		break;
	case WAIT_FAILED:
		UTIL_THROW_PLATFORM_ERROR(NULL);
	default:
		UTIL_THROW_UTIL_ERROR_CODED(CODE_INVALID_STATUS);
	}
}
#endif

#ifndef UTIL_HAVE_POSIX_MUTEX
void RWLock::Data::removeWriter() {
	switch (WaitForSingleObject(mutex_, INFINITE)) {
	case WAIT_OBJECT_0:
		if (--waitingWriterCount_ == 0 && writerCount_ == 0) {
			SetEvent(readEvent_);
		}
		ReleaseMutex(mutex_);
		break;
	case WAIT_FAILED:
		UTIL_THROW_PLATFORM_ERROR(NULL);
	default:
		UTIL_THROW_UTIL_ERROR_CODED(CODE_INVALID_STATUS);
	}
}
#endif

RWLock::Data::~Data() {
	close();
}

void RWLock::Data::close() {
#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_rwlock_destroy(&rwlock_);
#else
	if (mutex_ != NULL) {
		CloseHandle(mutex_);
	}
	if (readEvent_ != NULL) {
		CloseHandle(readEvent_);
	}
	if (writeEvent_ != NULL) {
		CloseHandle(writeEvent_);
	}
#endif
}

RWLock::RWLock(const RWLockAttribute *attr) :
		readLock_(newData(attr)),
		writeLock_(readLock_.data_) {
}

RWLock::RWLock() :
		readLock_(newData(NULL)),
		writeLock_(readLock_.data_) {
}

RWLock::~RWLock() {
	delete readLock_.data_;
}

RWLock::Data* RWLock::newData(const RWLockAttribute *attr) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	const pthread_rwlockattr_t *baseAttr = NULL;
	if (attr != NULL) {
		baseAttr = &attr->data_->attr_;
	}

	return new Data(baseAttr);
#else
	return new Data();
#endif 
}

void RWLock::ReadLock::lock(void) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_rwlock_rdlock(&data_->rwlock_);
#else
	HANDLE handles[] = { data_->mutex_, data_->readEvent_ };
	switch (WaitForMultipleObjects(2, handles, TRUE, INFINITE)) {
	case WAIT_OBJECT_0:
	case (WAIT_OBJECT_0 + 1):
		data_->readerCount_++;
		ResetEvent(data_->writeEvent_);
		ReleaseMutex(data_->mutex_);
		break;
	case WAIT_FAILED:
		UTIL_THROW_PLATFORM_ERROR(NULL);
	default:
		UTIL_THROW_UTIL_ERROR_CODED(CODE_INVALID_STATUS);
	}
#endif
}

bool RWLock::ReadLock::tryLock(void) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	return (0 == pthread_rwlock_tryrdlock(&data_->rwlock_));
#else
	return tryLock(1);
#endif
}

bool RWLock::ReadLock::tryLock(uint32_t msec) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	timespec ts = FileLib::calculateTimeoutSpec(CLOCK_REALTIME, msec);
	return (0 == pthread_rwlock_timedrdlock(&data_->rwlock_, &ts));
#else
	HANDLE handles[] = { data_->mutex_, data_->readEvent_ };
	switch (WaitForMultipleObjects(2, handles, TRUE, msec)) {
	case WAIT_OBJECT_0:
	case (WAIT_OBJECT_0 + 1):
		data_->readerCount_++;
		assert (data_->writerCount_ == 0);
		assert (data_->waitingWriterCount_ == 0);
		ResetEvent(data_->writeEvent_);
		ReleaseMutex(data_->mutex_);
		return true;
	case WAIT_TIMEOUT:
		return false;
	case WAIT_FAILED:
		UTIL_THROW_PLATFORM_ERROR(NULL);
	default:
		UTIL_THROW_UTIL_ERROR_CODED(CODE_INVALID_STATUS);
	}
#endif
}

void RWLock::ReadLock::unlock(void) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_rwlock_unlock(&data_->rwlock_);
#else
	data_->unlock();
#endif
}

void RWLock::WriteLock::lock(void) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_rwlock_wrlock(&data_->rwlock_);
#else
	if (!tryLock(INFINITE)) {
		UTIL_THROW_UTIL_ERROR_CODED(CODE_INVALID_STATUS);
	}
#endif
}

bool RWLock::WriteLock::tryLock(void) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	return (0 == pthread_rwlock_trywrlock(&data_->rwlock_));
#else
	return tryLock(1);
#endif
}

bool RWLock::WriteLock::tryLock(uint32_t msec) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	timespec ts = FileLib::calculateTimeoutSpec(CLOCK_REALTIME, msec);
	return (0 == pthread_rwlock_timedwrlock(&data_->rwlock_, &ts));
#else
	data_->addWriter();
	HANDLE handles[] = { data_->mutex_, data_->writeEvent_ };
	switch (WaitForMultipleObjects(2, handles, TRUE, msec)) {
	case WAIT_OBJECT_0:
	case (WAIT_OBJECT_0 + 1):
		data_->waitingWriterCount_--;
		data_->readerCount_++;
		data_->writerCount_++;
		ResetEvent(data_->readEvent_);
		ResetEvent(data_->writeEvent_);
		ReleaseMutex(data_->mutex_);
		return true;
	case WAIT_TIMEOUT:
		data_->removeWriter();
		return false;
	case WAIT_FAILED:
		data_->removeWriter();
		UTIL_THROW_PLATFORM_ERROR(NULL);
	default:
		data_->removeWriter();
		UTIL_THROW_UTIL_ERROR_CODED(CODE_INVALID_STATUS);
	}
#endif
}

void RWLock::WriteLock::unlock(void) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_rwlock_unlock(&data_->rwlock_);
#else
	data_->unlock();
#endif
}

RWLockAttribute::RWLockAttribute() : data_(new Data()) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	if (0 != pthread_rwlockattr_init(&data_->attr_)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#endif
}

RWLockAttribute::~RWLockAttribute() {
#ifdef UTIL_HAVE_POSIX_MUTEX
	pthread_rwlockattr_destroy(&data_->attr_);
#endif
}

void RWLockAttribute::setShared(bool shared) {
#ifdef UTIL_HAVE_POSIX_MUTEX
	if (0 != pthread_rwlockattr_setpshared(&data_->attr_,
			shared ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}

void RWLockAttribute::getShared(bool &shared) const {
#ifdef UTIL_HAVE_POSIX_MUTEX
	int type;
	if (0 != pthread_rwlockattr_getpshared(&data_->attr_, &type)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
	shared = (type == PTHREAD_PROCESS_SHARED);
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}

void Thread::sleep(uint32_t millisecTime) {
#ifdef _WIN32
	Sleep(millisecTime);
#else
	timespec spec;
	spec.tv_sec = millisecTime / 1000;
	spec.tv_nsec = (millisecTime % 1000) * 1000 * 1000;
	nanosleep(&spec, NULL);
#endif
}

void Thread::yield() {
	Thread::sleep(0);
}

uint64_t Thread::getSelfId() {
#ifdef _WIN32
	return GetCurrentThreadId();
#else
	return syscall(SYS_gettid);
#endif
}

} 
