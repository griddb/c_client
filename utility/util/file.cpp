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
/*
    Copyright (c) 2011 Minor Gordon
    All rights reserved

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the Yield project nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "util/file.h"
#include "util/code.h"
#include "util/os.h"
#include <vector>
#include <assert.h>

#if defined(UTIL_HAVE_DIRENT_H)
#	include <dirent.h>
#elif defined(UTIL_HAVE_SYS_DIRENT_H)
#	include <sys/dirent.h>
#endif

#ifdef UTIL_HAVE_SYS_RESOURCE_H
#include <limits>
#include <sys/resource.h>
#endif

#ifndef _WIN32
#include <sys/file.h>		
#endif

namespace util {


#ifdef _WIN32
const int FileFlag::TYPE_READ_ONLY = 00;
const int FileFlag::TYPE_WRITE_ONLY = 01;
const int FileFlag::TYPE_READ_WRITE = 02;
const int FileFlag::TYPE_CREATE = 0100;
const int FileFlag::TYPE_EXCLUSIVE = 0200;
const int FileFlag::TYPE_TRUNCATE = 01000;
const int FileFlag::TYPE_APPEND = 02000;
const int FileFlag::TYPE_NON_BLOCK = 04000;
const int FileFlag::TYPE_SYNC = 04010000;
const int FileFlag::TYPE_ASYNC = 020000;
const int FileFlag::TYPE_DIRECT = 040000;
#else
const int FileFlag::TYPE_READ_ONLY = O_RDONLY;
const int FileFlag::TYPE_WRITE_ONLY = O_WRONLY;
const int FileFlag::TYPE_READ_WRITE = O_RDWR;
const int FileFlag::TYPE_CREATE = O_CREAT;
const int FileFlag::TYPE_EXCLUSIVE = O_EXCL;
const int FileFlag::TYPE_TRUNCATE = O_TRUNC;
const int FileFlag::TYPE_APPEND = O_APPEND;
const int FileFlag::TYPE_NON_BLOCK = O_NONBLOCK;
const int FileFlag::TYPE_SYNC = O_SYNC;
const int FileFlag::TYPE_ASYNC = O_ASYNC;
const int FileFlag::TYPE_DIRECT = O_DIRECT;
#endif

FileFlag::FileFlag() :
		flags_(0) {
}

FileFlag::FileFlag(int flags) :
		flags_(flags) {
}

FileFlag::FileFlag(const FileFlag &obj) :
		flags_(obj.flags_) {
}

FileFlag::~FileFlag() {
}

int FileFlag::getFlags(void) const {
	return flags_;
}

int FileFlag::setFlags(int flags) {
	flags_ = flags;
	return flags_;
}

void FileFlag::setCreate(bool v) {
	FileLib::setFlag(flags_, TYPE_CREATE, v);
}

bool FileFlag::isCreate(void) const {
	return FileLib::getFlag(flags_, TYPE_CREATE);
}

void FileFlag::setExclusive(bool v) {
	FileLib::setFlag(flags_, TYPE_EXCLUSIVE, v);
}

bool FileFlag::isExclusive(void) const {
	return FileLib::getFlag(flags_, TYPE_EXCLUSIVE);
}

void FileFlag::setTruncate(bool v) {
	FileLib::setFlag(flags_, TYPE_TRUNCATE, v);
}

bool FileFlag::isTruncate(void) const {
	return FileLib::getFlag(flags_, TYPE_TRUNCATE);
}

void FileFlag::setAppend(bool v) {
	FileLib::setFlag(flags_, TYPE_APPEND, v);
}

bool FileFlag::isAppend(void) const {
	return FileLib::getFlag(flags_, TYPE_APPEND);
}

void FileFlag::setNonBlock(bool v) {
	FileLib::setFlag(flags_, TYPE_NON_BLOCK, v);
}

bool FileFlag::isNonBlock(void) const {
	return FileLib::getFlag(flags_, TYPE_NON_BLOCK);
}

void FileFlag::setSync(bool v) {
	FileLib::setFlag(flags_, TYPE_SYNC, v);
}

bool FileFlag::isSync(void) const {
	return FileLib::getFlag(flags_, TYPE_SYNC);
}

void FileFlag::setAsync(bool v) {
	FileLib::setFlag(flags_, TYPE_ASYNC, v);
}

bool FileFlag::isAsync(void) const {
	return FileLib::getFlag(flags_, TYPE_ASYNC);
}

void FileFlag::setDirect(bool v) {
	FileLib::setFlag(flags_, TYPE_DIRECT, v);
}

bool FileFlag::isDirect(void) const {
	return FileLib::getFlag(flags_, TYPE_DIRECT);
}

bool FileFlag::isReadOnly(void) const {
	return ((flags_ & (TYPE_WRITE_ONLY | TYPE_READ_WRITE)) ==
			TYPE_READ_ONLY);
}

bool FileFlag::isWriteOnly(void) const {
	return ((flags_ & TYPE_WRITE_ONLY) == TYPE_WRITE_ONLY);
}

bool FileFlag::isReadAndWrite(void) const {
	return ((flags_ & TYPE_READ_WRITE) == TYPE_READ_WRITE);
}



void FileFlag::swap(FileFlag &obj) {
	std::swap(flags_, obj.flags_);
}

FileFlag::operator int(void) const {
	return flags_;
}

FileFlag& FileFlag::operator=(int flags) {
	flags_ = flags;
	return *this;
}

FileFlag& FileFlag::operator=(const FileFlag &flag) {
	flags_ = flag.flags_;
	return *this;
}

bool FileFlag::operator==(int flags) const {
	return flags_ == flags;
}

bool FileFlag::operator==(const FileFlag &flags) const {
	return flags_ == flags.flags_;
}

bool FileFlag::operator!=(int flags) const {
	return flags_ != flags;
}

bool FileFlag::operator!=(const FileFlag &flags) const {
	return flags_ != flags.flags_;
}


FilePermission::FilePermission() :
		mode_(0) {
}

FilePermission::FilePermission(int mode) :
		mode_(mode) {
}

FilePermission::FilePermission(const FilePermission &mode) :
		mode_(mode.mode_) {
}

FilePermission::~FilePermission() {
}

int FilePermission::getMode(void) const {
	return mode_;
}

int FilePermission::setMode(int mode) {
	mode_ = mode;
	return mode_;
}

void FilePermission::setOwnerRead(bool v) {
	FileLib::setFlag(mode_, 00400, v);
}

bool FilePermission::isOwnerRead(void) const {
	return FileLib::getFlag(mode_, 00400);
}

void FilePermission::setOwnerWrite(bool v) {
	FileLib::setFlag(mode_, 00200, v);
}

bool FilePermission::isOwnerWrite(void) const {
	return FileLib::getFlag(mode_, 00200);
}

void FilePermission::setOwnerExecute(bool v) {
	FileLib::setFlag(mode_, 00100, v);
}

bool FilePermission::isOwnerExecute(void) const {
	return FileLib::getFlag(mode_, 00100);
}

void FilePermission::setGroupRead(bool v) {
	FileLib::setFlag(mode_, 00040, v);
}

bool FilePermission::isGroupRead(void) const {
	return FileLib::getFlag(mode_, 00040);
}

void FilePermission::setGroupWrite(bool v) {
	FileLib::setFlag(mode_, 00020, v);
}

bool FilePermission::isGroupWrite(void) const {
	return FileLib::getFlag(mode_, 00020);
}

void FilePermission::setGroupExecute(bool v) {
	FileLib::setFlag(mode_, 00010, v);
}

bool FilePermission::isGroupExecute(void) const {
	return FileLib::getFlag(mode_, 00010);
}

void FilePermission::setGuestRead(bool v) {
	FileLib::setFlag(mode_, 00004, v);
}

bool FilePermission::isGuestRead(void) const {
	return FileLib::getFlag(mode_, 00004);
}

void FilePermission::setGuestWrite(bool v) {
	FileLib::setFlag(mode_, 00002, v);
}

bool FilePermission::isGuestWrite(void) const {
	return FileLib::getFlag(mode_, 00002);
}

void FilePermission::setGuestExecute(bool v) {
	FileLib::setFlag(mode_, 00001, v);
}

bool FilePermission::isGuestExecute(void) const {
	return FileLib::getFlag(mode_, 00001);
}

FilePermission::operator int(void) const {
	return mode_;
}

FilePermission& FilePermission::operator=(int mode) {
	mode_ = mode;
	return *this;
}

FilePermission& FilePermission::operator=(const FilePermission &perm) {
	mode_ = perm.mode_;
	return *this;
}

bool FilePermission::operator==(int mode) const {
	return mode_ == mode;
}

bool FilePermission::operator==(const FilePermission &perm) const {
	return mode_ == perm.mode_;
}

bool FilePermission::operator!=(int mode) const {
	return mode_ != mode;
}

bool FilePermission::operator!=(const FilePermission &perm) const {
	return mode_ != perm.mode_;
}


#ifdef _WIN32
const File::FD File::INITIAL_FD = NULL;
#else
const File::FD File::INITIAL_FD = -1;
#endif

const FilePermission File::DEFAULT_PERMISSION = 0664;

File::File() : fd_(INITIAL_FD) {
}

File::~File() try {
	close();
}
catch (...) {
}

void File::close(void) {
	FD fd = detach();
	if (INITIAL_FD != fd) {
#ifdef _WIN32
		CloseHandle(fd);
#else
		::close(fd);
#endif
	}
}

bool File::isClosed(void) {
	return (INITIAL_FD == fd_);
}

void File::attach(FD fd) {
	assert (isClosed());
	fd_ = fd;
}

File::FD File::detach(void) {
	FD fd = fd_;
	fd_ = INITIAL_FD;
	return fd;
}

ssize_t File::read(void *buf, size_t blen) {
#ifdef _WIN32
	return read(buf, blen, tell());
#else
	const ssize_t result = ::read(fd_, static_cast<char*>(buf), blen);
	if (result < 0) {
		if (errno == EINTR) {
			return 0;
		}
		else {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
	}
	return result;
#endif
}

ssize_t File::write(const void *buf, size_t blen) {
#ifdef _WIN32
	return write(buf, blen, tell());
#else
	const ssize_t result = ::write(fd_, static_cast<const char*>(buf), blen);
	if (result < 0) {
		if (errno == EINTR) {
			return 0;
		}
		else {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
	}
	return result;
#endif
}

ssize_t File::read(void *buf, size_t blen, off_t offset) {
#ifdef _WIN32
	OVERLAPPED overlapped;

	overlapped.Offset = static_cast<DWORD>(offset);
	overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);
	overlapped.hEvent = NULL;
	overlapped.Internal = 0;
	overlapped.InternalHigh = 0;

	DWORD readBytes;
	if (!ReadFile(fd_, buf,
			static_cast<DWORD>(blen), &readBytes, &overlapped)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}

	return static_cast<ssize_t>(readBytes);
#else
	const ssize_t result = ::pread(fd_, buf, blen, offset);
	if (result < 0) {
		if (errno == EINTR) {
			return 0;
		}
		else {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
	}
	return result;
#endif
}

ssize_t File::write(const void *buf, size_t blen, off_t offset) {
#ifdef _WIN32
	OVERLAPPED overlapped;

	overlapped.Offset = static_cast<DWORD>(offset);
	overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);
	overlapped.hEvent = NULL;
	overlapped.Internal = 0;
	overlapped.InternalHigh = 0;

	DWORD writeBytes;
	if (!WriteFile(fd_, buf,
			static_cast<DWORD>(blen), &writeBytes, &overlapped)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}

	return static_cast<ssize_t>(writeBytes);
#else
	const ssize_t result = ::pwrite(fd_, buf, blen, offset);
	if (result < 0) {
		if (errno == EINTR) {
			return 0;
		}
		else {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
	}
	return result;
#endif
}

void File::read(IOOperation &operation) {
	(void) operation;
	UTIL_THROW_NOIMPL_UTIL();
}

void File::write(IOOperation &operation) {
	(void) operation;
	UTIL_THROW_NOIMPL_UTIL();
}

off_t File::tell() {
#ifdef _WIN32
	ULARGE_INTEGER fp;
	LONG fpHigh = 0;
	fp.LowPart = SetFilePointer(fd_, 0, &fpHigh, FILE_CURRENT);
	if (fp.LowPart == INVALID_SET_FILE_POINTER) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
	fp.HighPart = fpHigh;
	return fp.QuadPart;
#else
	const off_t result = lseek64(fd_, 0, SEEK_CUR);
	if (result == static_cast<off_t>(-1)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
	return result;
#endif
}

void File::sync() {
#ifdef _WIN32
	if (!FlushFileBuffers(fd_)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#else
	if (fsync(fd_) != 0) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#endif
}

void File::preAllocate(int mode, off_t offset, off_t len) {
#ifdef _WIN32
#else
	if (fallocate(fd_, mode, offset, len) != 0) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#endif
}

void File::setSize(uint64_t size) {
#ifdef _WIN32
	LARGE_INTEGER fp;
	fp.QuadPart = static_cast<LONGLONG>(size);
	if (SetFilePointerEx(fd_, fp, NULL, FILE_BEGIN) ==
			INVALID_SET_FILE_POINTER) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}

	if (!SetEndOfFile(fd_)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#else
	if (ftruncate64(fd_, static_cast<off64_t>(size)) != 0) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#endif
}

void File::setBlockingMode(bool block) {
#ifdef O_NONBLOCK
	FileLib::setFlags(!block, fd_, O_NONBLOCK);
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}

void File::getBlockingMode(bool &block) const {
#ifdef O_NONBLOCK
	FileLib::getFlags(block, fd_, O_NONBLOCK);

	block = !block;
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}

void File::setAsyncMode(bool async) {
#ifdef O_ASYNC
	FileLib::setFlags(async, fd_, O_ASYNC);
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}

void File::getAsyncMode(bool &async) const {
#ifdef O_ASYNC
	FileLib::getFlags(async, fd_, O_ASYNC);
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}

void File::setCloseOnExecMode(bool coe) {
#ifdef FD_CLOEXEC
	FileLib::setFDFlags(coe, fd_, FD_CLOEXEC);
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}

void File::getCloseOnExecMode(bool &coe) const {
#ifdef FD_CLOEXEC
	FileLib::getFDFlags(coe, fd_, FD_CLOEXEC);
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}

void File::getMode(int &mode) const {
#ifdef UTIL_HAVE_FCNTL
	const int result = fcntl(fd_, F_GETFL);
	if (-1 == result) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}

	mode = result;
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}

void File::setMode(int mode) {
#ifdef UTIL_HAVE_FCNTL
	if (0 != fcntl(fd_, F_SETFL, mode)) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#else
	UTIL_THROW_NOIMPL_UTIL();
#endif
}

void File::duplicate(FD srcfd) {
#ifdef _WIN32
	UTIL_THROW_NOIMPL_UTIL();
#else
	const FD dfd = dup(srcfd);
	if (INITIAL_FD == dfd)
		UTIL_THROW_PLATFORM_ERROR(NULL);

	attach(dfd);
#endif
}

void File::duplicate(FD srcfd, FD newfd) {
#ifdef _WIN32
	UTIL_THROW_NOIMPL_UTIL();
#else
	const FD dfd = dup2(srcfd, newfd);
	if (INITIAL_FD == dfd)
		UTIL_THROW_PLATFORM_ERROR(NULL);

	attach(dfd);
#endif
}


NamedFile::NamedFile() {
}

NamedFile::~NamedFile() {
}

void NamedFile::open(const char8_t *name, FileFlag flags, FilePermission perm) {
	u8string nameStr(name);

#ifdef _WIN32
	DWORD dwDesiredAccess = 0;
	DWORD dwCreationDisposition = 0;
	DWORD dwFlagsAndAttributes = FILE_FLAG_SEQUENTIAL_SCAN;

	if (flags.isAppend()) {
		dwDesiredAccess |= FILE_APPEND_DATA;
	}
	else if (flags.isReadAndWrite()) {
		dwDesiredAccess |= GENERIC_READ | GENERIC_WRITE;
	}
	else if (flags.isWriteOnly()) {
		dwDesiredAccess |= GENERIC_WRITE;
	}
	else {
		dwDesiredAccess |= GENERIC_READ;
	}

	if (flags.isCreate()) {
		if (flags.isExclusive()) {
			dwCreationDisposition = CREATE_NEW;
		}
		else if (flags.isTruncate()) {
			dwCreationDisposition = CREATE_ALWAYS;
		}
		else {
			dwCreationDisposition = OPEN_ALWAYS;
		}
	} else {
		dwCreationDisposition = OPEN_EXISTING;
	}


	if (flags.isSync()) {
		dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;
	}

	if (flags.isDirect()) {
		dwFlagsAndAttributes |= FILE_FLAG_NO_BUFFERING;
	}

	if (flags.isAsync()) {
		dwFlagsAndAttributes |= FILE_FLAG_OVERLAPPED;
	}

	std::wstring encodedName;
	CodeConverter(Code::UTF8, Code::WCHAR_T)(nameStr, encodedName);

	const uint32_t MAX_RETRIES = 10;
	const DWORD RETRY_DELAY = 250;
	HANDLE fd;
	for (uint32_t retries = 0;;) {
		fd = CreateFileW(encodedName.c_str(), dwDesiredAccess,
				FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
				dwCreationDisposition, dwFlagsAndAttributes, NULL);

		if (fd == INVALID_HANDLE_VALUE) {
			if (GetLastError() == ERROR_SHARING_VIOLATION &&
					++retries <= MAX_RETRIES) {
				Sleep(RETRY_DELAY);
				continue;
			}
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
		else {
			break;
		}
	}

	if (flags.isTruncate()/* && flags.isCreate()*/) {
		SetFilePointer(fd, 0, NULL, FILE_BEGIN);
		SetEndOfFile(fd);
	}

#else
	std::string encodedName;
	CodeConverter(Code::UTF8, Code::CHAR)(nameStr, encodedName);
	const int fd = ::open(
			encodedName.c_str(), flags | O_LARGEFILE, static_cast<int>(perm));
	if (-1 == fd) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#endif	
	name_.swap(nameStr);
	fd_ = fd;
}

const char8_t* NamedFile::getName(void) const {
	return name_.c_str();
}

bool NamedFile::unlink(void) {
#ifdef _WIN32
	std::wstring encodedName;
	CodeConverter(Code::UTF8, Code::WCHAR_T)(name_, encodedName);
	const bool res = (DeleteFileW(encodedName.c_str()) != 0);
#else
	std::string encodedName;
	CodeConverter(Code::UTF8, Code::CHAR)(name_, encodedName);
	const bool res = (0 == ::unlink(encodedName.c_str()));
#endif
	if (res) {
		name_.clear();
	}

	return res;
}

bool NamedFile::lock() {
	if (isClosed()) {
		UTIL_THROW_UTIL_ERROR_CODED(CODE_ILLEGAL_OPERATION);
	}
#ifdef _WIN32
	if (LockFile(fd_, 0, 0,
			std::numeric_limits<DWORD>::max(),
			std::numeric_limits<DWORD>::max()) == 0) {
		return false;
	}
#else
	if (flock(fd_, LOCK_EX | LOCK_NB) != 0) {
		if (errno == EINTR || errno == EWOULDBLOCK) {
			return false;
		}
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#endif
	return true;
}

void NamedFile::unlock() {
	if (isClosed()) {
		UTIL_THROW_UTIL_ERROR_CODED(CODE_ILLEGAL_OPERATION);
	}

#ifdef _WIN32
	if (UnlockFile(fd_, 0, 0,
			std::numeric_limits<DWORD>::max(),
			std::numeric_limits<DWORD>::max()) == 0) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}
#else
	if (flock(fd_, LOCK_UN | LOCK_NB) != 0) {
		if (errno != EINTR) {
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
	}
#endif
}

NamedFile::FD NamedFile::detach(void) {
	const FD fd = fd_;
	name_.clear();
	fd_ = INITIAL_FD;
	return fd;
}

#if UTIL_FAILURE_SIMULATION_ENABLED
ssize_t NamedFile::read(void *buf, size_t blen) {
	NamedFileFailureSimulator::checkOperation(*this, 0);
	return File::read(buf, blen);
}

ssize_t NamedFile::write(const void *buf, size_t blen) {
	NamedFileFailureSimulator::checkOperation(*this, 0);
	return File::write(buf, blen);
}

ssize_t NamedFile::read(void *buf, size_t blen, off_t offset) {
	NamedFileFailureSimulator::checkOperation(*this, 0);
	return File::read(buf, blen, offset);
}

ssize_t NamedFile::write(const void *buf, size_t blen, off_t offset) {
	NamedFileFailureSimulator::checkOperation(*this, 0);
	return File::write(buf, blen, offset);
}

void NamedFile::sync() {
	NamedFileFailureSimulator::checkOperation(*this, 0);
	File::sync();
}
#endif	

#if UTIL_FAILURE_SIMULATION_ENABLED
volatile bool NamedFileFailureSimulator::enabled_ = false;
volatile NamedFileFailureSimulator::FilterHandler
		NamedFileFailureSimulator::handler_ = NULL;
volatile int32_t NamedFileFailureSimulator::targetType_ = 0;
volatile uint64_t NamedFileFailureSimulator::startCount_ = 0;
volatile uint64_t NamedFileFailureSimulator::endCount_ = 0;
volatile uint64_t NamedFileFailureSimulator::lastOperationCount_ = 0;

void NamedFileFailureSimulator::set(
		int32_t targetType, uint64_t startCount, uint64_t endCount,
		FilterHandler handler) {
	if (startCount < endCount) {
		if (handler == NULL) {
			UTIL_THROW_UTIL_ERROR_CODED(CODE_ILLEGAL_OPERATION);
		}
		enabled_ = false;
		handler_ = handler;
		targetType_ = targetType;
		startCount_ = startCount;
		endCount_ = endCount;
		lastOperationCount_ = 0;
		enabled_ = true;
	}
	else {
		enabled_ = false;
		handler_ = NULL;
		targetType_ = 0;
		startCount_ = 0;
		endCount_ = 0;
	}
}

void NamedFileFailureSimulator::checkOperation(
		NamedFile &file, int32_t operationType) {
	FilterHandler handler = handler_;
	if (enabled_ && handler != NULL &&
			(*handler)(file, targetType_, operationType)) {
		const uint64_t count = lastOperationCount_;
		lastOperationCount_++;

		if (startCount_ <= count && count < endCount_) {
#ifdef _WIN32
			SetLastError(static_cast<DWORD>(-1));
#else
			errno = -1;
#endif
			UTIL_THROW_PLATFORM_ERROR(NULL);
		}
	}
}

#endif	


NamedPipe::NamedPipe() {
}

NamedPipe::~NamedPipe() {
}


void NamedPipe::open(
		const char8_t *name, FileFlag flags, FilePermission perm) {
#ifdef _WIN32
	std::string nameStr(name);

	static const char *const prefix = "\\\\.\\pipe\\";
	if (nameStr.find(prefix) != 0) {
		nameStr.insert(0, prefix);
	}

	DWORD openMode = 0;
	if ((flags & FileFlag::TYPE_ASYNC) != 0) {
		openMode |= FILE_FLAG_OVERLAPPED;
	}
	if ((flags & FileFlag::TYPE_READ_WRITE) != 0) {
		openMode |= PIPE_ACCESS_DUPLEX;
	} else if ((flags & FileFlag::TYPE_WRITE_ONLY) != 0) {
		openMode |= PIPE_ACCESS_OUTBOUND;
	} else {
		openMode |= PIPE_ACCESS_INBOUND;
	}

	DWORD pipeMode = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE;
	if ((flags & FileFlag::TYPE_NON_BLOCK) != 0) {
		pipeMode |= PIPE_NOWAIT;
	} else {
		pipeMode |= PIPE_WAIT;
	}

	std::wstring encodedName;
	CodeConverter(Code::UTF8, Code::WCHAR_T)(nameStr, encodedName);
	const HANDLE handle = CreateNamedPipeW(encodedName.c_str(), openMode,
			pipeMode, PIPE_UNLIMITED_INSTANCES, 4096, 4096, 0, NULL);
	if (handle == INVALID_HANDLE_VALUE) {
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}

	fd_ = handle;
#else
	std::string nameStr(name);

	u8string encodedName;
	CodeConverter(Code::UTF8, Code::CHAR)(nameStr, encodedName);

	int nflags = flags & ~(O_CREAT | O_EXCL);
	int fd = ::open(name, nflags, static_cast<int>(perm));

	if (-1 == fd) {
		if (flags & O_CREAT) {
			if (0 != mkfifo(encodedName.c_str(), perm)) {
				UTIL_THROW_PLATFORM_ERROR(NULL);
			}

			if (-1 == (fd = ::open(
					encodedName.c_str(), nflags, static_cast<int>(perm)))) {
				::unlink(encodedName.c_str());
				UTIL_THROW_PLATFORM_ERROR(NULL);
			}

			fd_ = fd;
			name_.swap(nameStr);
			return;
		}

		UTIL_THROW_PLATFORM_ERROR(NULL);
	}

	if (flags & O_EXCL) {
		::close(fd);
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}

	struct stat stat;
	if (0 != fstat(fd, &stat)) {
		::close(fd);
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}

	if (!S_ISFIFO(stat.st_mode)) {
		::close(fd);
		UTIL_THROW_PLATFORM_ERROR(NULL);
	}

	fd_ = fd;
	name_.swap(nameStr);
#endif
}



} 
