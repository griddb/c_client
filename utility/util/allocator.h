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
    @brief Definition of Utility of allocators
*/
#ifndef UTIL_MEMORY_H_
#define UTIL_MEMORY_H_

#if defined(max) || defined(min)
#error "windows.h (or winsock2.h etc) may be included without NOMINMAX"
#endif

#include "util/type.h"
#include "util/thread.h"
#include <limits>
#include <cstdlib>
#include <cassert>
#include <algorithm> 

#ifndef UTIL_ALLOCATOR_EMPTY_ALLOCATOR_CONSTRUCTOR_ALLOWED
#if defined(_GLIBCXX_FULLY_DYNAMIC_STRING) && _GLIBCXX_FULLY_DYNAMIC_STRING
#define UTIL_ALLOCATOR_EMPTY_ALLOCATOR_CONSTRUCTOR_ALLOWED 0
#else
#define UTIL_ALLOCATOR_EMPTY_ALLOCATOR_CONSTRUCTOR_ALLOWED 1
#endif
#endif

#ifndef UTIL_ALLOCATOR_BASIC_STRING_ALTER_MODIFIERS
#ifdef __GNUC__
#define UTIL_ALLOCATOR_BASIC_STRING_ALTER_MODIFIERS 0
#else
#define UTIL_ALLOCATOR_BASIC_STRING_ALTER_MODIFIERS 1
#endif
#endif

#ifndef UTIL_ALLOCATOR_BASIC_STRING_SUBSTRING_ENABLED
#define UTIL_ALLOCATOR_BASIC_STRING_SUBSTRING_ENABLED 0
#endif


namespace util {

/*!
	@brief Allocates for STL containers or strings. (std::allocator compatible)
*/
template<typename T, typename BaseAllocator>
class StdAllocator {
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T value_type;
	typedef value_type *pointer;
	typedef value_type &reference;
	typedef const value_type *const_pointer;
	typedef const value_type &const_reference;

#if UTIL_ALLOCATOR_EMPTY_ALLOCATOR_CONSTRUCTOR_ALLOWED
	inline StdAllocator() throw() : base_(NULL) {
	}
#endif

	inline StdAllocator(BaseAllocator &base) throw() : base_(&base) {
	}

	inline explicit StdAllocator(BaseAllocator *base) throw() : base_(base) {
	}

	template<typename U>
	inline StdAllocator(const StdAllocator<U, BaseAllocator> &other) throw() :
			base_(other.base()) {
	}

	template<typename U>
	inline StdAllocator& operator=(
			const StdAllocator<U, BaseAllocator> &other) {
		base_ = other.base_;
		return *this;
	}

	inline size_type max_size() const throw() {
		return std::numeric_limits<size_t>::max() / sizeof(T);
	}

	inline pointer allocate(
			size_type size, std::allocator<void>::const_pointer = NULL) {
#if UTIL_ALLOCATOR_EMPTY_ALLOCATOR_CONSTRUCTOR_ALLOWED
		assert(base_ != NULL);
#endif
		return static_cast<pointer>(base_->allocate(size * sizeof(T)));
	}

	inline void deallocate(pointer ptr, size_type) {
#if UTIL_ALLOCATOR_EMPTY_ALLOCATOR_CONSTRUCTOR_ALLOWED
		assert(base_ != NULL);
#endif
		base_->deallocate(ptr);
	}

#if UTIL_CXX11_SUPPORTED
	template<typename U, typename ...Args>
	inline void construct(U *ptr, Args &&...args) {
		::new (static_cast<void*>(ptr)) U(std::forward<Args>(args)...);
	}
#else
	inline void construct(pointer ptr, const T &value) {
		new(static_cast<void*>(ptr)) T(value);
	}
#endif

#if UTIL_CXX11_SUPPORTED
	template<typename U>
	inline void destroy(U *ptr) {
		ptr->~U();
	}
#else
	inline void destroy(pointer ptr) {
		ptr->~T();
	}
#endif

	inline pointer address(reference value) const {
		return &value;
	}

	inline const_pointer address(const_reference value) const {
		return &value;
	}

	template<typename U>
	struct rebind { typedef StdAllocator<U, BaseAllocator> other; };

	inline BaseAllocator* base() const throw() { return base_; }

private:
	BaseAllocator *base_;
};

/*!
	@brief StdAllocator specified in void type.
*/
template<typename BaseAllocator>
class StdAllocator<void, BaseAllocator> {
public:
	typedef void value_type;
	typedef void *pointer;
	typedef const void *const_pointer;

#if UTIL_ALLOCATOR_EMPTY_ALLOCATOR_CONSTRUCTOR_ALLOWED
	inline StdAllocator() throw() : base_(NULL) {
	}
#endif

	inline StdAllocator(BaseAllocator &base) throw() : base_(&base) {
	}

	inline explicit StdAllocator(BaseAllocator *base) throw() : base_(base) {
	}

	template<typename U>
	inline StdAllocator(const StdAllocator<U, BaseAllocator> &other) throw() :
			base_(other.base()) {
	}

	template<typename U>
	inline StdAllocator& operator=(
			const StdAllocator<U, BaseAllocator> &other) {
		base_ = other.base_;
		return *this;
	}

	template<typename U>
	struct rebind { typedef StdAllocator<U, BaseAllocator> other; };

	inline BaseAllocator* base() const throw() { return base_; }

private:
	BaseAllocator *base_;
};

/*!
	@brief StdAllocator specified in void type.
*/
template<typename T>
class StdAllocator<T, void> {
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T value_type;
	typedef value_type *pointer;
	typedef value_type &reference;
	typedef const value_type *const_pointer;
	typedef const value_type &const_reference;

	typedef void* (WrapperFunc)(void*, void*, size_t);
	typedef std::pair<void*, WrapperFunc*> WrapperResult;

#if UTIL_ALLOCATOR_EMPTY_ALLOCATOR_CONSTRUCTOR_ALLOWED
	inline StdAllocator() throw() : base_(NULL), wrapper_(NULL) {
	}
#endif

	template<typename BaseAllocator>
	inline StdAllocator(const BaseAllocator &base) throw() :
			base_(wrap(&base).first), wrapper_(wrap(&base).second) {
	}

	template<typename BaseAllocator>
	inline StdAllocator(BaseAllocator &base) throw() :
			base_(wrap(&base).first), wrapper_(wrap(&base).second) {
	}

	template<typename BaseAllocator>
	inline explicit StdAllocator(BaseAllocator *base) throw() :
			base_(wrap(base).first), wrapper_(wrap(base).second) {
	}

	template<typename U, typename BaseAllocator>
	inline StdAllocator<T, void>& operator=(
			const StdAllocator<U, BaseAllocator> &other) {
		base_ = wrap(&other).first;
		wrapper_ = wrap(&other).second;
		return *this;
	}

	inline size_type max_size() const throw() {
		return std::numeric_limits<size_t>::max() / sizeof(T);
	}

	inline pointer allocate(
			size_type size, std::allocator<void>::const_pointer = NULL) {
#if UTIL_ALLOCATOR_EMPTY_ALLOCATOR_CONSTRUCTOR_ALLOWED
		assert(base_ != NULL);
#endif
		return static_cast<pointer>(
				(*wrapper_)(base_, NULL, size * sizeof(T)));
	}

	inline void deallocate(pointer ptr, size_type size) {
#if UTIL_ALLOCATOR_EMPTY_ALLOCATOR_CONSTRUCTOR_ALLOWED
		assert(base_ != NULL);
#endif
		if (ptr != NULL) {
			(*wrapper_)(base_, ptr, size);
		}
	}

#if UTIL_CXX11_SUPPORTED
	template<typename U, typename ...Args>
	inline void construct(U *ptr, Args &&...args) {
		::new (static_cast<void*>(ptr)) U(std::forward<Args>(args)...);
	}
#else
	inline void construct(pointer ptr, const T &value) {
		new(static_cast<void*>(ptr)) T(value);
	}
#endif

#if UTIL_CXX11_SUPPORTED
	template<typename U>
	inline void destroy(U *ptr) {
		ptr->~U();
	}
#else
	inline void destroy(pointer ptr) {
		ptr->~T();
	}
#endif

	inline pointer address(reference value) const {
		return &value;
	}

	inline const_pointer address(const_reference value) const {
		return &value;
	}

	template<typename U>
	struct rebind { typedef StdAllocator<U, void> other; };

	inline void* base() const throw() { return base_; }

	inline WrapperFunc* wrapper() const throw() { return wrapper_; }

	template<typename BaseAllocator>
	inline static WrapperResult wrap(BaseAllocator *base) throw() {
		return wrapSame(base, base);
	}

private:

	template<typename BaseAllocator>
	struct CustomWrapper {
		static void* execute(void *alloc, void *ptr, size_t size) {
			if (ptr == NULL) {
				return static_cast<BaseAllocator*>(alloc)->allocate(size);
			}
			else {
				static_cast<BaseAllocator*>(alloc)->deallocate(ptr);
				return NULL;
			}
		}
	};

	template<typename BaseAllocator, typename ValueType>
	struct StdWrapper {
		static void* execute(void *alloc, void *ptr, size_t size) {
			UTIL_STATIC_ASSERT(sizeof(ValueType) == 1);
			if (ptr == NULL) {
				return static_cast<BaseAllocator*>(alloc)->allocate(size);
			}
			else {
				static_cast<BaseAllocator*>(alloc)->deallocate(
						static_cast<ValueType*>(ptr), size);
				return NULL;
			}
		}
	};

	struct Placeholder {};

	template<typename U, typename BaseAllocator>
	inline static WrapperResult wrapSame(
			const StdAllocator<U, void> *base, BaseAllocator*) throw() {
		assert(base != NULL);
		return WrapperResult(base->base(), base->wrapper());
	}

	template<typename BaseAllocator>
	inline static WrapperResult wrapSame(
			const void*, BaseAllocator *base) throw() {
		return wrapOther<BaseAllocator>(
				ensureNonConstAllocator(base), static_cast<Placeholder*>(NULL));
	}

	template<typename BaseAllocator>
	inline static WrapperResult wrapOther(BaseAllocator *base, void*) throw() {
		return WrapperResult(base, &CustomWrapper<BaseAllocator>::execute);
	}

	template<typename BaseAllocator>
	inline static WrapperResult wrapOther(
			BaseAllocator *base,
			typename BaseAllocator::
					template rebind<Placeholder>::other::value_type*) throw() {
		return WrapperResult(base, &StdWrapper<
				BaseAllocator, typename BaseAllocator::value_type>::execute);
	}

	template<typename BaseAllocator>
	inline static WrapperResult wrapOther(
			BaseAllocator *base,
			typename BaseAllocator::StdAllocatorResolver::
					template Rebind<Placeholder>::Other::ValueType*) throw() {
		typedef typename BaseAllocator::StdAllocatorResolver Resolver;
		return WrapperResult(base, Resolver()(base).wrapper_);
	}

	template<typename Alloc>
	inline static Alloc* ensureNonConstAllocator(Alloc *alloc) {
		return alloc;
	}

	template<typename Alloc>
	inline static Alloc* ensureNonConstAllocator(const Alloc*) {
		UTIL_STATIC_ASSERT(sizeof(Alloc) < 0);
		return NULL;
	}

	void *base_;
	WrapperFunc *wrapper_;
};

/*!
	@brief StdAllocator, enable to specify a base allocator to run.
*/
template<>
class StdAllocator<void, void> {
private:
	typedef StdAllocator<char, void> OtherAllocator;

	typedef OtherAllocator::WrapperFunc WrapperFunc;

public:
	typedef void value_type;
	typedef void *pointer;
	typedef const void *const_pointer;

#if UTIL_ALLOCATOR_EMPTY_ALLOCATOR_CONSTRUCTOR_ALLOWED
	inline StdAllocator() throw() : base_(NULL), wrapper_(NULL) {
	}
#endif

	template<typename BaseAllocator>
	inline StdAllocator(const BaseAllocator &base) throw() :
			base_(OtherAllocator::wrap(&base).first),
			wrapper_(OtherAllocator::wrap(&base).second) {
	}

	template<typename BaseAllocator>
	inline StdAllocator(BaseAllocator &base) throw() :
			base_(OtherAllocator::wrap(&base).first),
			wrapper_(OtherAllocator::wrap(&base).second) {
	}

	template<typename BaseAllocator>
	inline explicit StdAllocator(BaseAllocator *base) throw() :
			base_(OtherAllocator::wrap(base).first),
			wrapper_(OtherAllocator::wrap(base).second) {
	}

	template<typename U, typename BaseAllocator>
	inline StdAllocator& operator=(
			const StdAllocator<U, BaseAllocator> &other) {
		base_ = OtherAllocator::wrap(&other).first;
		wrapper_ = OtherAllocator::wrap(&other).second;
		return *this;
	}

	template<typename U>
	struct rebind { typedef StdAllocator<U, void> other; };

	inline void* base() const throw() { return base_; }

	inline WrapperFunc* wrapper() const throw() { return wrapper_; }

private:
	void *base_;
	WrapperFunc *wrapper_;
};

template<typename T, typename U, typename BaseAllocator>
inline bool operator==(
		const StdAllocator<T, BaseAllocator> &op1,
		const StdAllocator<U, BaseAllocator> &op2) throw() {

	return op1.base() == op2.base();
}

template<typename T, typename U, typename BaseAllocator>
inline bool operator!=(
		const StdAllocator<T, BaseAllocator> &op1,
		const StdAllocator<U, BaseAllocator> &op2) throw() {

	return op1.base() != op2.base();
}

template< typename T, typename Alloc = StdAllocator<T, void> >
class AllocDefaultDelete {
public:
	AllocDefaultDelete(const Alloc &alloc) throw();
	void operator()(T *ptr);

private:
	Alloc alloc_;
};

template< typename T, typename D = AllocDefaultDelete<T> >
class AllocUniquePtr {
public:
	typedef T element_type;
	typedef D deleter_type;
	typedef element_type *pointer;

	template<typename B> AllocUniquePtr(pointer ptr, B &deleterBase);
	~AllocUniquePtr();

	pointer get() const throw();
	element_type& operator*() const;
	pointer operator->() const throw();

	pointer release() throw();
	void reset(pointer ptr = pointer()) throw();

private:
	AllocUniquePtr(const AllocUniquePtr&);
	AllocUniquePtr& operator=(const AllocUniquePtr&);

	pointer ptr_;
	deleter_type deleter_;
};


/*!
	@brief STL string template for using allocators with members.
*/
template<
		typename CharT,
		typename Traits = std::char_traits<CharT>,
		typename Alloc = std::allocator<CharT> >
class BasicString : public std::basic_string<CharT, Traits, Alloc> {
private:
	typedef BasicString<CharT, Traits, Alloc> ThisType;
	typedef std::basic_string<CharT, Traits, Alloc> BaseType;

	template<typename T>
	struct IntDetector {
		enum { VALUE = std::numeric_limits<T>::is_integer };
		typedef typename BoolType<VALUE>::Result Result;
	};

public:
	typedef typename BaseType::iterator iterator;
	typedef typename BaseType::size_type size_type;
	typedef typename BaseType::allocator_type allocator_type;

	explicit inline BasicString(const allocator_type& alloc = allocator_type()) :
			BaseType(alloc) {
	}

	inline BasicString(
			const BaseType& str, size_type pos, size_type len = ThisType::npos,
			const allocator_type& alloc = allocator_type()) :
			BaseType(str, pos, len, alloc) {
	}

	inline BasicString(
			const CharT *s, const allocator_type &alloc = allocator_type()) :
			BaseType(s, alloc) {
	}

	inline BasicString(
			const CharT *s, size_type n,
			const allocator_type& alloc = allocator_type()) :
			BaseType(s, n, alloc) {
	}

	inline BasicString(
			size_type n, CharT c,
			const allocator_type& alloc = allocator_type()) :
			BaseType(n, c, alloc) {
	}

	template<class InputIterator>
	inline BasicString(
			InputIterator first, InputIterator last,
			const allocator_type &alloc = allocator_type()) :
			BaseType(first, last, alloc) {
	}

	inline BasicString(const BaseType& str) : BaseType(str) {
	}

	inline ~BasicString() {
	}

	inline ThisType& operator=(const BaseType &str) {
		BaseType::operator=(str);
		return *this;
	}

	inline ThisType& operator=(const CharT *s) {
		BaseType::operator=(s);
		return *this;
	}

	inline ThisType& operator=(CharT c) {
		BaseType::operator=(c);
		return *this;
	}

#if UTIL_ALLOCATOR_BASIC_STRING_ALTER_MODIFIERS

	inline ThisType& operator+=(const BaseType &str) {
		BaseType::operator+=(str);
		return *this;
	}

	inline ThisType& operator+=(const CharT *s) {
		BaseType::operator+=(s);
		return *this;
	}

	inline ThisType& operator+=(CharT c) {
		BaseType::operator+=(c);
		return *this;
	}

	inline ThisType& append(const BaseType &str) {
		BaseType::append(str);
		return *this;
	}

	inline ThisType& append(const BaseType &str, size_type pos, size_type n) {
		BaseType::append(str, pos, n);
		return *this;
	}

	inline ThisType& append(const CharT *s) {
		BaseType::append(s);
		return *this;
	}

	inline ThisType& append(const CharT *s, size_type n) {
		BaseType::append(s, n);
		return *this;
	}

	inline ThisType& append(size_type n, CharT c) {
		BaseType::append(n, c);
		return *this;
	}

	inline ThisType& assign(const BaseType &str) {
		BaseType::assign(str);
		return *this;
	}

	inline ThisType& assign(const BaseType &str, size_type pos, size_type n) {
		BaseType::assign(str, pos, n);
		return *this;
	}

	inline ThisType& assign(const CharT *s) {
		BaseType::assign(s);
		return *this;
	}

	inline ThisType& assign(const CharT *s, size_type n) {
		BaseType::assign(s, n);
		return *this;
	}

	inline ThisType& assign(size_type n, CharT c) {
		BaseType::assign(n, c);
		return *this;
	}

	inline ThisType& insert(size_type pos, const BaseType &str) {
		BaseType::insert(pos, str);
		return *this;
	}

	inline ThisType& insert(
			size_type pos, const BaseType &str,
			size_type subPos, size_type n) {
		BaseType::insert(pos, str, subPos, n);
		return *this;
	}

	inline ThisType& insert(size_type pos, const CharT *s) {
		BaseType::insert(pos, s);
		return *this;
	}

	inline ThisType& insert(size_type pos, const CharT *s, size_type n) {
		BaseType::insert(pos, s, n);
		return *this;
	}

	inline ThisType& insert(size_type pos, size_type n, CharT c) {
		BaseType::insert(pos, n, c);
		return *this;
	}

	inline void insert(iterator p, size_type n, CharT c) {
		BaseType::insert(p, n, c);
	}

	inline iterator insert(iterator p, CharT c) {
		return BaseType::insert(p, c);
	}

	inline ThisType& replace(
			size_type pos, size_type len, const BaseType &str) {
		BaseType::replace(pos, len, str);
		return *this;
	}

	inline ThisType& replace(
			iterator i1, iterator i2, const BaseType &str) {
		BaseType::replace(i1, i2, str);
		return *this;
	}

	inline ThisType& replace(
			size_type pos, size_type len, const BaseType &str,
			size_type subPos, size_type n) {
		BaseType::replace(pos, len, str, subPos, n);
		return *this;
	}

	inline ThisType& replace(size_type pos, size_type len, const CharT *s) {
		BaseType::replace(pos, len, s);
		return *this;
	}

	inline ThisType& replace(iterator i1, iterator i2, const CharT *s) {
		BaseType::replace(i1, i2, s);
		return *this;
	}

	inline ThisType& replace(
			size_type pos, size_type len, const CharT *s, size_type n) {
		BaseType::replace(pos, len, s, n);
		return *this;
	}

	inline ThisType& replace(
			iterator i1, iterator i2, const CharT *s, size_type n) {
		BaseType::replace(i1, i2, s, n);
		return *this;
	}

	inline ThisType& replace(
			size_type pos, size_type len, size_type n, CharT c) {
		BaseType::replace(pos, len, n, c);
		return *this;
	}

	inline ThisType& replace(
			iterator i1, iterator i2, size_type n, CharT c) {
		BaseType::replace(i1, i2, n, c);
		return *this;
	}

	inline ThisType& erase(size_type pos = 0, size_type len = ThisType::npos) {
		BaseType::erase(pos, len);
		return *this;
	}

	inline iterator erase(iterator p) {
		return BaseType::erase(p);
	}

	inline iterator erase(iterator first, iterator last) {
		return BaseType::erase(first, last);
	}

	template<typename InputIterator>
	inline ThisType& append(InputIterator first, InputIterator last) {
		typedef typename IntDetector<InputIterator>::Result Tag;
		return replaceByRange(this->end(), this->end(), first, last, Tag());
	}

	template<typename InputIterator>
	inline ThisType& assign(InputIterator first, InputIterator last) {
		typedef typename IntDetector<InputIterator>::Result Tag;
		return replaceByRange(this->begin(), this->end(), first, last, Tag());
	}

	template<typename InputIterator>
	inline void insert(iterator p, InputIterator first, InputIterator last) {
		typedef typename IntDetector<InputIterator>::Result Tag;
		replaceByRange(p, p, first, last, Tag());
	}

	template<typename InputIterator>
	inline ThisType& replace(
			iterator i1, iterator i2, InputIterator first, InputIterator last) {
		typedef typename IntDetector<InputIterator>::Result Tag;
		return replaceByRange(i1, i2, first, last, Tag());
	}

#endif	

	inline ThisType substr(
			size_type pos = 0, size_type len = ThisType::npos) const {
#if UTIL_ALLOCATOR_BASIC_STRING_SUBSTRING_ENABLED
		return ThisType(*this, pos, len, this->get_allocator());
#else
		(void) pos;
		(void) len;
		UTIL_THROW_UTIL_ERROR(CODE_ILLEGAL_OPERATION,
				"BasicString::substr is disabled");
#endif
	}

private:
	template<typename InputIterator>
	inline ThisType& replaceByRange(
			iterator i1, iterator i2, InputIterator n, InputIterator c,
			const TrueType&) {
		BaseType::replace(i1, i2, n, c);
		return *this;
	}

	template<typename InputIterator>
	inline ThisType& replaceByRange(
			iterator i1, iterator i2, InputIterator first, InputIterator last,
			const FalseType&) {
		const ThisType str(first, last, this->get_allocator());
		BaseType::replace(i1, i2, str);
		return *this;
	}
};

template<typename CharT, typename Traits, typename Alloc>
inline BasicString<CharT, Traits, Alloc> operator+(
		const BasicString<CharT, Traits, Alloc> &lhs,
		const BasicString<CharT, Traits, Alloc> &rhs) {
	return BasicString<CharT, Traits, Alloc>(lhs) += rhs;
}

template<typename CharT, typename Traits, typename Alloc, typename R>
inline BasicString<CharT, Traits, Alloc> operator+(
		const BasicString<CharT, Traits, Alloc> &lhs, R rhs) {
	return BasicString<CharT, Traits, Alloc>(lhs) += rhs;
}

template<typename CharT, typename Traits, typename Alloc, typename L>
inline BasicString<CharT, Traits, Alloc> operator+(
		L lhs, const BasicString<CharT, Traits, Alloc> &rhs) {
	return (BasicString<CharT, Traits, Alloc>(rhs.get_allocator()) += lhs) += rhs;
}


}	

#endif
