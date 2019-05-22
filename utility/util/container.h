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
    @brief Definition of Utility for Containers
*/
#ifndef UTIL_CONTAINER_H_
#define UTIL_CONTAINER_H_

#include "util/allocator.h"
#include "util/code.h"

#include <vector>
#include <map>
#include <set>
#include <deque>
#include <limits>
#include <memory>
#include <algorithm>
#include <climits>

#define UTIL_DISABLE_CONTAINER_DEFAULT_CONSTRUCTOR 1


namespace util {

template<typename T, typename Alloc> class XArray;

/*!
	@brief Iterates for XArray
*/
template<typename V, typename T>
class XArrayIterator {
private:
	typedef XArrayIterator<V, T> ThisType;
	template<typename TF, typename AllocF> friend class XArray;
	template<typename VF, typename TF> friend class XArrayIterator;

	template<typename VA, typename TA>
	struct Traits {
		typedef const TA *Pointer;
		typedef const TA &Reference;
	};

	template<typename TA>
	struct Traits<TA, TA> {
		typedef TA *Pointer;
		typedef TA &Reference;
	};

public:
	typedef std::random_access_iterator_tag iterator_category;
	typedef V value_type;
	typedef ptrdiff_t difference_type;
	typedef ptrdiff_t distance_type;
	typedef typename Traits<T, V>::Pointer pointer;
	typedef typename Traits<T, V>::Reference reference;

	XArrayIterator();
	template<typename VA>
	XArrayIterator(const XArrayIterator<VA, T> &another);
	template<typename VA>
	ThisType& operator=(const XArrayIterator<VA, T> &another);

	reference operator*() const;
	reference operator[](difference_type off) const;
	pointer operator->() const;

	ThisType& operator++();
	ThisType operator++(int);
	ThisType& operator+=(difference_type off);
	ThisType& operator--();
	ThisType operator--(int);
	ThisType& operator-=(difference_type off);

private:
	XArrayIterator(pointer cur);
	pointer cur_;
};

/*!
	@brief Manages data in the array: Array Class Wrapper
	@note Only a simple types are available;
	      e.g)
			OK: int32_t, uint8_t, std::pair, std::string*, XArray*, File*
			NG: std::string, util::XArray, util::File
*/
template< typename T, typename Alloc >
class XArray {
public:
	typedef Alloc allocator_type;
	typedef typename allocator_type::size_type size_type;
	typedef typename allocator_type::difference_type difference_type;
	typedef typename allocator_type::value_type value_type;
	typedef typename allocator_type::pointer pointer;
	typedef typename allocator_type::reference reference;
	typedef typename allocator_type::const_pointer const_pointer;
	typedef typename allocator_type::const_reference const_reference;

	typedef XArrayIterator<T, T> iterator;
	typedef XArrayIterator<const T, T> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;


#if UTIL_DISABLE_CONTAINER_DEFAULT_CONSTRUCTOR
	explicit XArray(const Alloc &alloc);
#else
	explicit XArray(const Alloc &alloc = Alloc());
#endif


	template<typename Iter>
	XArray(Iter first, Iter last, const Alloc &alloc = Alloc());


	~XArray();


	Alloc get_allocator() const { return allocator_; }


	template<typename Iter>
	void assign(Iter first, Iter last);

	void assign(size_t count, const T &value);

	void clear();

	void swap(XArray<T, Alloc> &another);


	void resize(size_t count);

	void resize(size_t count, T value);

	size_t size() const;

	bool empty() const;


	void reserve(size_t requestedCapacity);

	size_t capacity() const;


	iterator begin() { return iterator(data_); }
	iterator end() { return iterator(tail_); }

	const_iterator begin() const { return const_iterator(data_); }
	const_iterator end() const { return const_iterator(tail_); }

	reverse_iterator rbegin() { return reverse_iterator( end() ); }
	reverse_iterator rend() { return reverse_iterator( begin() ); }

	const_reverse_iterator rbegin() const { return const_reverse_iterator( end() ); }
	const_reverse_iterator rend() const { return const_reverse_iterator( begin() ); }


	T& front();
	const T& front() const;
	T& back();
	const T& back() const;

	void push_back(const T &value);

	void push_back(const T *values, size_t count);

	void pop_back();


	T& operator[](size_t index);
	const T& operator[](size_t index) const;

	T* data();

	const T* data() const;


	iterator insert(iterator pos, const T &value);

	template<typename Iter>
	void insert(iterator pos, Iter first, Iter last);


	iterator erase(iterator pos);
	iterator erase(iterator first, iterator last);

protected:
	XArray(const Alloc &alloc, size_t capacityCount);

private:
	void reserveInternal(size_t requestedCapacity);

	XArray(const XArray&);
	XArray& operator=(const XArray&);

	Alloc allocator_;
	size_t restSize_;
	T *data_;		
	T *tail_;		
};

/*!
	@brief XArray using usual dynamic memory allocator.
*/
template<typename T>
class NormalXArray : public XArray< T, std::allocator<T> > {
private:
	typedef XArray< T, std::allocator<T> > BaseType;

public:
	typedef typename BaseType::allocator_type allocator_type;

	explicit NormalXArray(
			const allocator_type& alloc = allocator_type()) : BaseType(alloc) {
	}

	template<typename Iter>
	NormalXArray(
			Iter first, Iter last, const allocator_type& alloc = allocator_type()) :
			BaseType(first, last, alloc) {
	}
};


template<typename T, typename Comp, typename Base>
class SortedList;

/*!
	@brief Lists data in order.
*/
template<typename T, typename Comp, typename Base>
class SortedList {
public:
	typedef Comp value_compare;
	typedef typename Base::allocator_type allocator_type;
	typedef typename allocator_type::size_type size_type;
	typedef typename allocator_type::difference_type difference_type;
	typedef typename allocator_type::value_type value_type;
	typedef typename allocator_type::pointer pointer;
	typedef typename allocator_type::reference reference;
	typedef typename allocator_type::const_pointer const_pointer;
	typedef typename allocator_type::const_reference const_reference;

	typedef typename Base::iterator iterator;
	typedef typename Base::const_iterator const_iterator;
	typedef typename Base::reverse_iterator reverse_iterator;
	typedef typename Base::const_reverse_iterator const_reverse_iterator;

#if UTIL_DISABLE_CONTAINER_DEFAULT_CONSTRUCTOR
	explicit SortedList(const allocator_type &alloc) : base_(alloc) {
	}
#else
	explicit SortedList(const allocator_type &alloc = allocator_type()) :
			base_(alloc) {
	}
#endif

	explicit SortedList(
			const Comp &comp, const allocator_type &alloc = allocator_type()) :
			base_(alloc), comp_(comp) {
	}

	const Base& base() const { return base_; };

	void clear() { base_.clear(); }
	void reserve(size_t count) { base_.reserve(count); }

	size_t size() const { return base_.size(); }
	bool empty() const { return base_.empty(); }

	iterator begin() { return base_.begin(); }
	iterator end() { return base_.end(); }
	const_iterator begin() const { return base_.begin(); }
	const_iterator end() const { return base_.end(); }

	iterator rbegin() { return base_.rbegin(); }
	iterator rend() { return base_.rend(); }
	const_iterator rbegin() const { return base_.rbegin(); }
	const_iterator rend() const { return base_.rend(); }

	void insert(const T &value) {
		base_.insert(upper_bound(value), value);
	}

	iterator find(const T &value) {
		iterator it = lower_bound(value);
		return (it == end() || comp_(value, *it) ? end() : it);
	}

	const_iterator find(const T &value) const {
		return lower_bound(value);
	}

	iterator lower_bound(const T &value) {
		return std::lower_bound(begin(), end(), value, comp_);
	}

	const_iterator lower_bound(const T &value) const {
		return std::lower_bound(begin(), end(), value, comp_);
	}

	iterator upper_bound(const T &value) {
		return std::upper_bound(begin(), end(), value, comp_);
	}

	const_iterator upper_bound(const T &value) const {
		return std::upper_bound(begin(), end(), value, comp_);
	}

	std::pair<iterator, iterator> equal_range(const T &value) {
		return std::equal_range(begin(), end(), value, comp_);
	}

	std::pair<const_iterator, const_iterator> equal_range(const T &value) const {
		return std::equal_range(begin(), end(), value, comp_);
	}

	iterator erase(iterator pos) { return base_.erase(pos); }
	iterator erase(iterator first, iterator last) { return base_.erase(first, last); }

private:
	Base base_;
	Comp comp_;
};

/*!
	@brief Lists data in order using usual dynamic memory allocator.
*/
template< typename T, typename Comp = typename std::set<T>::key_compare >
class NormalSortedList : public SortedList< T, Comp, NormalXArray<T> > {
private:
	typedef SortedList< T, Comp, NormalXArray<T> > BaseType;

public:
	typedef typename BaseType::allocator_type allocator_type;

	explicit NormalSortedList(const allocator_type &alloc) : BaseType(alloc) {
	}

	explicit NormalSortedList(
			const Comp &comp = Comp(),
			const allocator_type &alloc = allocator_type()) :
			BaseType(comp, alloc) {
	}
};



class InsertionResetter {
public:
	template<typename C>
	explicit InsertionResetter(C &container);

	template<typename C>
	InsertionResetter(C &container, typename C::const_iterator insertionPoint);

	~InsertionResetter();

	void release() throw();
	void reset() throw();

private:
	typedef void (*ResetterFunc)(void*, size_t);

	struct Entry {
		Entry();
		Entry(ResetterFunc func, void *container, size_t pos);

		ResetterFunc func_;
		void *container_;
		size_t pos_;
	};

	template<typename C>
	static Entry createEntry(
			C &container, typename C::const_iterator insertionPoint);

	template<typename C>
	static void resetSpecific(void *container, size_t pos);

	InsertionResetter(const InsertionResetter&);
	InsertionResetter& operator=(const InsertionResetter&);

	Entry entry_;
};


template<typename Alloc> class XArrayOutStream;


/*!
	@brief Manages byte stream for XArray.
*/
template<typename Alloc>
class XArrayOutStream : public BasicByteStream< XArrayOutStream<Alloc> > {
public:
	inline explicit XArrayOutStream(XArray<uint8_t, Alloc> &buffer);

	size_t write(const void *buf, size_t length);

	void writeAll(const void *buf, size_t length);

	inline size_t position() const { return buffer_.size(); }

	inline void position(size_t pos) { buffer_.resize(pos); }

	inline void clear() { position(0); }

private:
	XArray<uint8_t, Alloc> &buffer_;
};


typedef XArrayOutStream< std::allocator<uint8_t> > NormalOutStream;
typedef ByteStream<NormalOutStream> NormalByteOutStream;

/*!
	@brief Manages byte stream for writable object, e.g. files.
*/
template<typename Base>
class BufferedOutStream : public BasicByteStream< BufferedOutStream<Base> > {
public:
	inline explicit BufferedOutStream(
		Base &base, NormalXArray<uint8_t> &buffer, size_t limitSize);

	inline ~BufferedOutStream();

	inline size_t write(const void *buf, size_t length);

	inline void flush();

private:
	Base base_;
	NormalXArray<uint8_t> &buffer_;
	size_t limit_;
};





template<typename V, typename T>
XArrayIterator<V, T>::XArrayIterator() : cur_(NULL) {
}

template<typename V, typename T>
XArrayIterator<V, T>::XArrayIterator(pointer cur) : cur_(cur) {
}

template<typename V, typename T>
template<typename VA>
XArrayIterator<V, T>::XArrayIterator(const XArrayIterator<VA, T> &another) :
	cur_(another.cur_)
{
}

template<typename V, typename T>
template<typename VA>
XArrayIterator<V, T>&
XArrayIterator<V, T>::operator=(const XArrayIterator<VA, T> &another) {
	cur_ = another.cur_;
	return *this;
}

template<typename V, typename T>
typename XArrayIterator<V, T>::reference
XArrayIterator<V, T>::operator*() const {
	return *cur_;
}

template<typename V, typename T>
typename XArrayIterator<V, T>::reference
XArrayIterator<V, T>::operator[](difference_type off) const {
	return *(cur_ + off);
}

template<typename V, typename T>
typename XArrayIterator<V, T>::pointer
XArrayIterator<V, T>::operator->() const {
	return cur_;
}

template<typename V, typename T>
XArrayIterator<V, T>&
XArrayIterator<V, T>::operator++() {
	cur_++;
	return *this;
}

template<typename V, typename T>
XArrayIterator<V, T>
XArrayIterator<V, T>::operator++(int) {
	ThisType prevIterator = *this;
	cur_++;
	return prevIterator;
}

template<typename V, typename T>
XArrayIterator<V, T>&
XArrayIterator<V, T>::operator+=(difference_type off) {
	cur_ += off;
	return *this;
}

template<typename V, typename T>
XArrayIterator<V, T>&
XArrayIterator<V, T>::operator--() {
	cur_--;
	return *this;
}

template<typename V, typename T>
XArrayIterator<V, T>
XArrayIterator<V, T>::operator--(int) {
	XArrayIterator<V, T> prevIterator = *this;
	cur_--;
	return prevIterator;
}

template<typename V, typename T>
XArrayIterator<V, T>&
XArrayIterator<V, T>::operator-=(difference_type off) {
	cur_ -= off;
	return *this;
}

template<typename V1, typename V2, typename T>
inline bool operator==(
	const XArrayIterator<V1, T> &i1, const XArrayIterator<V2, T> &i2)
{
	return &(*i1) == &(*i2);
}

template<typename V1, typename V2, typename T>
inline bool operator!=(
	const XArrayIterator<V1, T> &i1, const XArrayIterator<V2, T> &i2)
{
	return &(*i1) != &(*i2);
}

template<typename V1, typename V2, typename T>
inline bool operator<(
	const XArrayIterator<V1, T> &i1, const XArrayIterator<V2, T> &i2)
{
	return &(*i1) < &(*i2);
}

template<typename V1, typename V2, typename T>
inline bool operator<=(
	const XArrayIterator<V1, T> &i1, const XArrayIterator<V2, T> &i2)
{
	return &(*i1) <= &(*i2);
}

template<typename V1, typename V2, typename T>
inline bool operator>(
	const XArrayIterator<V1, T> &i1, const XArrayIterator<V2, T> &i2)
{
	return &(*i1) > &(*i2);
}

template<typename V1, typename V2, typename T>
inline bool operator>=(
	const XArrayIterator<V1, T> &i1, const XArrayIterator<V2, T> &i2)
{
	return &(*i1) >= &(*i2);
}

template<typename V, typename T>
inline XArrayIterator<V, T> operator+(
	const XArrayIterator<V, T> &i,
	typename XArrayIterator<V, T>::difference_type off)
{
	XArrayIterator<V, T> iterator = i;
	iterator += off;
	return iterator;
}

template<typename V, typename T>
inline XArrayIterator<V, T> operator+(
	typename XArrayIterator<V, T>::difference_type off,
	const XArrayIterator<V, T> &i)
{
	XArrayIterator<V, T> iterator = i;
	iterator += off;
	return iterator;
}

template<typename V, typename T>
inline XArrayIterator<V, T> operator-(
	const XArrayIterator<V, T> &i,
	typename XArrayIterator<V, T>::difference_type off)
{
	XArrayIterator<V, T> iterator = i;
	iterator -= off;
	return iterator;
}

template<typename V1, typename V2, typename T>
inline typename XArrayIterator<V1, T>::difference_type operator-(
	const XArrayIterator<V1, T> &i1, const XArrayIterator<V2, T> &i2)
{
	return &(*i1) - &(*i2);
}


template<typename T, typename Alloc>
XArray<T, Alloc>::XArray(const Alloc &alloc) :
	allocator_(alloc), restSize_(0), data_(NULL), tail_(NULL)
{
}

template<typename T, typename Alloc>
XArray<T, Alloc>::XArray(const Alloc &alloc, size_t capacityCount) :
	allocator_(alloc), restSize_(0), data_(NULL), tail_(NULL)
{
	reserve(capacityCount);
}

template<typename T, typename Alloc>
template<typename Iter>
XArray<T, Alloc>::XArray(Iter first, Iter last, const Alloc &alloc) :
	allocator_(alloc), restSize_(0), data_(NULL), tail_(NULL)
{
	assign(first, last);
}

template<typename T, typename Alloc>
XArray<T, Alloc>::~XArray() {
	if (data_ != NULL) {
		allocator_.deallocate(data_, capacity());
		data_ = NULL;
		tail_ = NULL;
		restSize_ = 0;
	}
}

template<typename T, typename Alloc>
template<typename Iter>
void XArray<T, Alloc>::assign(Iter first, Iter last) {
	clear();
	for (; first != last; ++first) {
		push_back(*first);
	}
}

template<typename T, typename Alloc>
void XArray<T, Alloc>::assign(size_t count, const T &value) {
	clear();
	reserve(count);
	for (; count > 0; --count) {
		push_back(value);
	}
}

template<typename T, typename Alloc>
void XArray<T, Alloc>::clear() {
	restSize_ += size();
	tail_ = data_;
}

template<typename T, typename Alloc>
void XArray<T, Alloc>::swap(XArray<T, Alloc> &another) {
	if (allocator_ == another.allocator_) {
		std::swap(data_, another.data_);
		std::swap(tail_, another.tail_);
		std::swap(restSize_, another.restSize_);
	}
	else {
		XArray<T, Alloc> tmp(another.begin(), another.end(), allocator_);
		another.assign(begin(), end());
		assign(tmp.begin(), tmp.end());
	}
}

template<typename T, typename Alloc>
size_t XArray<T, Alloc>::size() const {
	return static_cast<size_t>(tail_ - data_);
}

template<typename T, typename Alloc>
bool XArray<T, Alloc>::empty() const {
	return (tail_ == data_);
}

template<typename T, typename Alloc>
void XArray<T, Alloc>::resize(size_t count) {
	reserve(count);
	restSize_ = capacity() - count;
	tail_ = data_ + count;
}

template<typename T, typename Alloc>
void XArray<T, Alloc>::resize(size_t count, T value) {
	const size_t orgCount = size();
	resize(count);

	if (orgCount < count) {
		std::fill(begin() + orgCount, end(), value);
	}
}

template<typename T, typename Alloc>
void XArray<T, Alloc>::reserve(size_t requestedCapacity) {
	if (capacity() < requestedCapacity) {
		reserveInternal(requestedCapacity);
	}
}

template<typename T, typename Alloc>
size_t XArray<T, Alloc>::capacity() const {
	return size() + restSize_;
}

template<typename T, typename Alloc>
T& XArray<T, Alloc>::front() {
	assert ( !empty() );
	return *begin();
}

template<typename T, typename Alloc>
const T& XArray<T, Alloc>::front() const {
	assert ( !empty() );
	return *begin();
}

template<typename T, typename Alloc>
T& XArray<T, Alloc>::back() {
	assert ( !empty() );
	return *( --end() );
}

template<typename T, typename Alloc>
const T& XArray<T, Alloc>::back() const {
	assert ( !empty() );
	return *( --end() );
}

template<typename T, typename Alloc>
void XArray<T, Alloc>::push_back(const T &value) {
	if (restSize_ == 0) {
		reserveInternal(size() + 1);
	}
	restSize_--;
	*(tail_++) = value;
}

template<typename T, typename Alloc>
void XArray<T, Alloc>::push_back(const T *values, size_t count) {
	if (restSize_ < count) {
		reserveInternal(size() + count);
	}

	restSize_ -= count;
	memcpy(tail_, values, count * sizeof(T));
	tail_ += count;
}

template<typename T, typename Alloc>
void XArray<T, Alloc>::pop_back() {
	assert ( !empty() );
	tail_--;
	restSize_++;
}

template<typename T, typename Alloc>
T& XArray<T, Alloc>::operator[](size_t index) {
	assert ( index < size() );
	return data_[index];
}

template<typename T, typename Alloc>
const T& XArray<T, Alloc>::operator[](size_t index) const {
	assert ( index < size() );
	return data_[index];
}

template<typename T, typename Alloc>
T* XArray<T, Alloc>::data() {
	return data_;
}

template<typename T, typename Alloc>
const T* XArray<T, Alloc>::data() const {
	return data_;
}

template<typename T, typename Alloc>
typename XArray<T, Alloc>::iterator XArray<T, Alloc>::insert(
	iterator pos, const T &value)
{
	assert(data_ <= pos.cur_ && pos.cur_ <= tail_);
	const ptrdiff_t posIndex = pos.cur_ - data_;

	if (restSize_ == 0) {
		reserveInternal(size() + 1);
	}

	T *newPos = data_ + posIndex;
	memmove(newPos + 1, newPos, (size() - posIndex) * sizeof(T));
	*newPos = value;
	tail_++;
	restSize_--;

	return iterator(newPos);
}

template<typename T, typename Alloc>
template<typename Iter>
void XArray<T, Alloc>::insert(iterator pos, Iter first, Iter last) {
	assert(data_ <= pos.cur_ && pos.cur_ <= tail_);
	const ptrdiff_t posIndex = pos.cur_ - data_;

	size_t count = 0;
	for (Iter i = first; i != last; i++) {
		count++;
	}

	if (restSize_ < count) {
		reserveInternal(size() + count);
	}

	T *dest = data_ + posIndex;
	memmove(dest + count, dest, (size() - posIndex) * sizeof(T));
	for (Iter src = first; src != last; ++dest, ++src) {
		*dest = *src;
	}
	tail_ += count;
	restSize_ -= count;
}

template<typename T, typename Alloc>
typename XArray<T, Alloc>::iterator XArray<T, Alloc>::erase(iterator pos) {
	assert(data_ <= pos.cur_ && pos.cur_ < tail_);

	memmove(pos.cur_, pos.cur_ + 1, (tail_ - pos.cur_) * sizeof(T));
	tail_--;
	restSize_++;

	return iterator(pos.cur_);
}

template<typename T, typename Alloc>
typename XArray<T, Alloc>::iterator XArray<T, Alloc>::erase(
	iterator first, iterator last)
{
	assert(first.cur_ <= last.cur_);
	assert(data_ <= first.cur_ && last.cur_ <= tail_);

	const ptrdiff_t count = last.cur_ - first.cur_;
	memmove(first.cur_, first.cur_ + count, sizeof(T) * (tail_ - last.cur_));
	tail_ -= count;
	restSize_ += count;

	return iterator(first.cur_);
}

template<typename T, typename Alloc>
void XArray<T, Alloc>::reserveInternal(size_t requestedCapacity) {
	assert (0 < requestedCapacity);
	assert (size() < requestedCapacity);

	const uint32_t MIN_CAPACITY_BIT = 4;	
	const size_t usedSize = this->size();
	const size_t newCapacity = (1U << std::max<uint32_t>(
		MIN_CAPACITY_BIT,
		static_cast<uint32_t>(sizeof(uint32_t) * CHAR_BIT) -
		nlz(static_cast<uint32_t>(requestedCapacity - 1)) ));
	if (newCapacity < requestedCapacity) {
		UTIL_THROW_UTIL_ERROR(CODE_SIZE_LIMIT_EXCEEDED,
				"Too large array capacity requested (size=" <<
				requestedCapacity << ")");
	}

	T *newData;
	try {
		newData = allocator_.allocate(newCapacity);
		assert (newData != NULL);
	}
	catch (std::bad_alloc &e) {
		UTIL_RETHROW_UTIL_ERROR(CODE_NO_MEMORY, e, "Allocation failed");
	}

	if (data_ != NULL) {
		memcpy(newData, data_, sizeof(T) * usedSize);
		allocator_.deallocate(data_, capacity());
	}

	data_ = newData;
	tail_ = data_ + usedSize;
	restSize_ = newCapacity - usedSize;
}





template<typename C>
InsertionResetter::InsertionResetter(C &container) :
		entry_(createEntry(container, container.end())) {
}

template<typename C>
InsertionResetter::InsertionResetter(
		C &container, typename C::const_iterator insertionPoint) :
		entry_(createEntry(container, insertionPoint)) {
}

template<typename C>
InsertionResetter::Entry InsertionResetter::createEntry(
		C &container, typename C::const_iterator insertionPoint) {
	const ptrdiff_t distance = insertionPoint - container.begin();
	return Entry(&resetSpecific<C>, &container, static_cast<size_t>(distance));
}

template<typename C>
void InsertionResetter::resetSpecific(void *container, size_t pos) {
	C *containerObj = static_cast<C*>(container);

	if (pos >= containerObj->size()) {
		assert(false);
		return;
	}

	containerObj->erase(containerObj->begin() + pos);
}



template<typename Alloc>
inline XArrayOutStream<Alloc>::XArrayOutStream(
		XArray<uint8_t, Alloc> &buffer) : buffer_(buffer) {
}

template<typename Alloc>
inline size_t XArrayOutStream<Alloc>::write(const void *buf, size_t length) {
	writeAll(buf, length);
	return length;
}

template<typename Alloc>
inline void XArrayOutStream<Alloc>::writeAll(const void *buf, size_t length) {
	const size_t lastSize = buffer_.size();
	buffer_.resize(lastSize + length);
	memcpy(&buffer_[0] + lastSize, buf, length);
}


template<typename Base>
inline BufferedOutStream<Base>::BufferedOutStream(
		Base &base, NormalXArray<uint8_t> &buffer, size_t limitSize) :
		base_(base), buffer_(buffer), limit_(limitSize) {
	buffer_.reserve(limit_);
}

template<typename Base>
inline BufferedOutStream<Base>::~BufferedOutStream() {
	if ( !buffer_.empty() ) {
		try {
			flush();
		}
		catch (...) {
		}
	}
}

template<typename Base>
inline size_t BufferedOutStream<Base>::write(
		const void *buf, size_t length ) {
	size_t lastSize = buffer_.size();
	if (limit_ > 0 && lastSize >= limit_) {
		flush();
		lastSize = 0;
	}

	const size_t consumed =
		( limit_ == 0 ? length : std::min(length, limit_ - lastSize) );
	buffer_.resize(lastSize + consumed);
	memcpy(&buffer_[0] + lastSize, buf, consumed);

	return consumed;
}

template<typename Base>
inline void BufferedOutStream<Base>::flush() {
	try {
		base_.writeAll( buffer_.data(), buffer_.size() );
		base_.flush();
		buffer_.clear();
	}
	catch (...) {
		this->setError();
		throw;
	}
}

} 

#endif
