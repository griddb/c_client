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
	@brief Definition of Auth class
*/
#ifndef AUTHENTICATION_H_
#define AUTHENTICATION_H_

#include "util/container.h"

struct Auth {
	enum Mode {
		MODE_NONE,
		MODE_BASIC,
		MODE_CHALLENGE
	};

	class Challenge;
	class PasswordDigest;

	typedef util::StdAllocator<void, void> Allocator;

	typedef util::BasicString< char8_t, std::char_traits<char8_t>,
			util::StdAllocator<char8_t, void> > String;
};

class Auth::Challenge {
public:
	enum {
		MAX_NONCE_SIZE = 32,
		MAX_NC_SIZE = 8,
		MAX_OPAQUE_SIZE = 32,
		MAX_BASE_SALT_SIZE = 16,
		MAX_CNONCE_SIZE = 8
	};

	Challenge();

	void initialize(
			const char8_t *nonce, const char8_t *nc, const char8_t *opaque,
			const char8_t *baseSalt);

	bool isEmpty() const;
	void clear();

	static PasswordDigest makeDigest(
			const Allocator &alloc,
			const char8_t *user, const char8_t *password);

	static void makeStoredDigest(
			const char8_t *user, const char8_t *password, String *digestStr,
			bool compatible);

	template<typename T> static void putRequest(
			T &out, Mode mode, const Challenge &lastChallenge, bool full);

	static void getRequest(
			util::ArrayByteInStream &in, const Allocator &alloc,
			Mode &reqMode, Challenge &challenge, bool full);

	template<typename T> static void putResponse(
			T &out, Mode mode, const Challenge &challenge);

	static bool getResponse(
			util::ArrayByteInStream &in, const Allocator &alloc, Mode &mode,
			Challenge &challenge);

	bool authenticate(
			const Allocator &alloc, const char8_t *user,
			const char8_t *reqDigest, const char8_t *storedDigest,
			bool plain, Mode &mode);

	static void build(
			Mode mode, Challenge &challenge,
			const PasswordDigest &digest, String *challengeStr);

	static void parseDigest(
			const char8_t *digestStr, String **outList, size_t outCount,
			String *alternative);

	const char8_t* getOpaque() const;
	const char8_t* getLastCNonce() const;

	static void generateCNonce(String *cnonce);

	void build(const PasswordDigest &digest, String *challengeStr);

	void build(
			const PasswordDigest &digest, const char8_t *cnonce,
			String *challengeStr) const;

	void getChallengeDigest(
			const PasswordDigest &digest, const char8_t *cnonce,
			String *challengeStr) const;

	void getCryptSecret(
			const PasswordDigest &digest, String *challengeStr) const;

	static void sha256Hash(const String &value, String *hash);

	static void sha256Hash(const void *value, size_t size, String *hash);

	static void md5Hash(const String &value, String *hash);

	static void md5Hash(const void *value, size_t size, String *hash);

	static void generateBcryptSalt(int32_t factor, String *salt);

	static void getBcryptHash(
			const char8_t *password, const char8_t *salt, String *hash);

	static bool compareBcryptPassword(
			const char8_t *password, const char8_t *hash);

	static void randomHexString(size_t bytesSize, String *str);

	static void bytesToHex(const void *bytes, size_t size, String *str);

	static void getRandomBytes(void *bytes, size_t size);

	static Mode getDefaultMode();

	static Mode getMode(util::ArrayByteInStream &in);

	template<typename T> static bool putMode(T &out, Mode mode);

private:
	static const Mode DEFAULT_MODE;

	static const char8_t *const DEFAULT_METHOD;
	static const char8_t *const DEFAULT_REALM;
	static const char8_t *const DEFAULT_URI;
	static const char8_t *const DEFAULT_QOP;
	static const char8_t *const DIGEST_PREFIX;

	static const size_t BASE_SALT_SIZE;
	static const int32_t CRYPT_SALT_FACTOR;

	template<size_t Size> static void initializeFixedStr(char8_t (&str)[Size]);
	template<size_t Size> static void copyToFixedStr(
			char8_t (&dest)[Size], const char8_t *src);

	static bool challengeEnabled_;

	char8_t nonce_[MAX_NONCE_SIZE + 1];
	char8_t nc_[MAX_NC_SIZE + 1];
	char8_t opaque_[MAX_OPAQUE_SIZE + 1];
	char8_t baseSalt_[MAX_BASE_SALT_SIZE + 1];
	char8_t cnonce_[MAX_CNONCE_SIZE + 1];
};

class Auth::PasswordDigest {
public:
	PasswordDigest(
			const Allocator &alloc, const char8_t *basicSecret,
			const char8_t *cryptBase, const char8_t *challengeBase);

	const char8_t* getBasicSecret() const;

private:
	friend class Challenge;

	String basicSecret_;
	String cryptBase_;
	String challengeBase_;
};

template<typename T> void Auth::Challenge::putRequest(
		T &out, Mode mode, const Challenge &lastChallenge, bool full) {
	if (!putMode(out, mode)) {
		return;
	}

	const bool challenged = !lastChallenge.isEmpty();
	out << static_cast<int8_t>(challenged);

	if (challenged) {
		out << lastChallenge.opaque_;
		out << lastChallenge.cnonce_;

		if (full) {
			out << lastChallenge.nonce_;
			out << lastChallenge.nc_;
			out << lastChallenge.baseSalt_;
		}
	}
}

template<typename T> void Auth::Challenge::putResponse(
		T &out, Mode mode, const Challenge &challenge) {
	if (!putMode(out, mode)) {
		return;
	}

	const bool challenging =
			(mode == MODE_CHALLENGE && !challenge.isEmpty());
	out << static_cast<int8_t>(challenging);

	if (challenging) {
		out << challenge.nonce_;
		out << challenge.nc_;
		out << challenge.opaque_;
		out << challenge.baseSalt_;
	}
}

template<typename T> bool Auth::Challenge::putMode(T &out, Mode mode) {
	out << static_cast<int8_t>(mode);

	if (mode == MODE_NONE) {
		return false;
	}

	return true;
}

#endif
