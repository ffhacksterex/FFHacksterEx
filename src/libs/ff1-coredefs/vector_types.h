#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <sstream>
#include <functional>
#include "pair_result.h"
#include "stream_ops.h"


// TYPES

using boolvector = std::vector<bool>;
using bytevector = std::vector<unsigned char>;
using wordvector = std::vector<unsigned short>;
using dwordvector = std::vector<unsigned long>;
using intvector = std::vector<int>;
using uintvector = std::vector<unsigned int>;

using mfcstringvector = std::vector<CString>;
using stdstringvector = std::vector<std::string>;

using stdstrintvector = std::vector<std::pair<std::string, int>>;



// IMPLEMENTATION

size_t find_by_name(const stdstrintvector & vec, std::string name);
size_t find_by_value(const stdstrintvector & vec, int valtofind);
size_t find_by_prefix(const stdstrintvector & vec, std::string prefix);

mfcstringvector convert(const stdstringvector & svec);
stdstringvector convert(const mfcstringvector & mvec);



template <typename T>
std::vector<T> remove_duplicates(std::vector<T> vec)
{
	std::set<T> uniques;
	for (size_t st = 0; st < vec.size(); ++st) {
		if (uniques.find(vec[st]) != cend(uniques)) {
			vec.erase(vec.begin() + st);
			--st;
		}
		else {
			uniques.insert(vec[st]);
		}
	}
	return vec;
}

template <typename T>
const T * address(const std::vector<T> & vec)
{
	return vec.get_allocator().address(vec.front());
}

template <typename T>
T * address(std::vector<T> & vec)
{
	return vec.get_allocator().address(vec.front());
}



template <typename T>
std::vector<T> & append(std::vector<T> & vec, const std::vector<T> & insvec)
{
	vec.insert(end(vec), begin(insvec), end(insvec));
	return vec;
}

// Overwrite the buffer
template <typename T>
std::vector<T> & overwrite(std::vector<T> & vec, const std::vector<T> & source, size_t offset, size_t count, size_t srcoffset = 0)
{
	if (count > vec.size()) {
		LogMsg << "overwrite() can't write " << count << " entries starting at " << offset << " since it contains " << vec.size() << " elements." << std::endl;
		throw std::runtime_error("overwrite extends past the bounds of the vector");
	}

	vec.erase(begin(vec) + offset, begin(vec) + offset + count);
	vec.insert(begin(vec) + offset, begin(source) + srcoffset, begin(source) + srcoffset + count);
	return vec;
}

// Insert bytes from srcvec to vec. Note that srcvec is not necessarily count bytes in size,
// as srcoffset specifies the start position from which to insert bytes from srcvec.
template <typename T>
std::vector<T> & insert(std::vector<T> & vec, size_t offset, const std::vector<T> & srcvec, size_t count, size_t srcoffset = 0)
{
	vec.insert(begin(vec) + offset, begin(srcvec) + srcoffset, end(srcvec) + srcoffset + count);
	return vec;
}

template <typename T>
std::vector<T> & delete_elements_if(std::vector<T> & v, std::function<bool(const T & elem)> pred)
{
	auto iter = std::remove_if(begin(v), end(v), pred);
	v.erase(iter, end(v));
	return v;
}



template <typename T>
bool has(const std::vector<T> & cont, const T & key)
{
	return std::find(cbegin(cont), cend(cont), key) != cend(cont);
}

// Returns true if the key is in the container.
// is_in() reverses the params of has(), with the key first and container second.
// It's used in places with small throwaway containers defined inplace.
//     e.g. is_in(strvalue, vectortype{value1, value2, value3, value4})
template <typename T>
bool is_in(const T & key, const std::vector<T> & cont)
{
	//return cont.find(key) != cend(cont); //REFACTOR
	return std::find(cbegin(cont), cend(cont), key) != cend(cont);
}

template <typename T>
pair_result<T> find(const std::vector<T> & cont, const T & key)
{
	auto iter = std::find(cbegin(cont), cend(cont), key);
	if (iter == cend(cont))
		return pair_result<T>();

	//return pair_result(true, *iter); //REFACTOR - revisit
	return pair_result<T>(true, key);
}

//REFACTOR - maybe just use is_in and remove this function from use, no need to return a copy of the value.
template <typename T>
T findvalue(const std::vector<T> & cont, const T & key)
{
	auto iter = std::find(cbegin(cont), cend(cont), key);
	if (iter != cend(cont))
		return *iter;

	throw std::invalid_argument("vector doesn't contain key '" + key + " '.");
}

template <typename T>
T find_or_default(const std::vector<T> & cont, T key, T defaultvalue)
{
	auto iter = cont.find(key);
	return iter == cend(cont) ? *iter : defaultvalue;
}



// COMMON SPECIALIZATIONS

std::string ToString(const bytevector & vec);   // convert the bytes to numbers, not characters (some being non-displayable)
//std::string ToString(const boolvector & vec);

// XXX_to_array and array_to_XXX convert between fixed-size arrays and vectors.
// Primarily used to convert the fixed-sized label arrays in DATA.

template <int ROWS, int COLS>
bool bytes_to_array(const bytevector & bytes, unsigned char array[ROWS][COLS])
{
	const size_t count = ROWS * COLS;
	if (bytes.size() != count)
		return false;

	const unsigned char * src = address(bytes);
	unsigned char * dst = *array; // point to the start of the first row
	for (size_t st = 0; st < count; ++st) dst[st] = src[st];
	return true;
}

template <int ROWS, int COLS>
bool array_to_bytes(unsigned char array[ROWS][COLS], bytevector & bytes)
{
	bytes.resize(ROWS*COLS);
	if (bytes.empty())
		return false; // don't allow zero-element arrays to call this

	const unsigned char * src = *array; // point to the start of the first row
	unsigned char * dst = address(bytes);
	for (size_t st = 0; st < bytes.size(); ++st) dst[st] = src[st];
	return true;
}

template <int SIZE>
bool bools_to_array(const boolvector & bools, bool array[SIZE])
{
	if (bools.size() != (SIZE))
		return false;

	//N.B. - because of the vector<bool> optimization in some compilers, don't try to block-copy this
	const bool* src = address(bools);
	for (size_t st = 0; st < bools.size(); ++st) array[st] = bools[st];
	return true;
}

template <int SIZE>
bool array_to_bools(bool array[SIZE], boolvector & bools)
{
	bools.resize(SIZE);
	if (bools.empty())
		return false; // don't allow zero-element arrays to call this

	for (size_t st = 0; st < bools.size(); ++st) bools[st] = array[st];
	return true;
}



// OPERATORS

template <typename T>
std::vector<T> operator+(const std::vector<T> & vec, const std::vector<T> & add)
{
	std::vector<T> newvec(vec);
	return append(newvec, add);
}

template <typename T>
std::vector<T> & operator+=(std::vector<T> & vec, T add)
{
	vec.push_back(add);
	return vec;
}

template <typename T>
bool operator==(const std::vector<T> & left, const std::vector<T> & right)
{
	if (left.size() != right.size()) return false;
	for (size_t st = 0; st < left.size(); ++st)
		if (left[st] != right[st]) return false;
	return true;
}

template <typename T>
bool operator!=(const std::vector<T> & left, const std::vector<T> & right)
{
	return !operator=(left, right);
}



#define VECTOR_DELIMITERS "<>[],vec"

template <typename T>
std::ostream & operator << (std::ostream & os, const std::vector<T> & c)
{
	return stream_out(os, c, VECTOR_DELIMITERS);
}

template <typename T>
std::istream & operator >> (std::istream & is, std::vector<T> & c)
{
	return stream_in(is, c, VECTOR_DELIMITERS);
}

template <typename T>
void write_element(std::ostream & os, const std::vector<T> & c, typename std::vector<T>::const_iterator it, const char * pelemsep)
{
	UNREFERENCED_PARAMETER(c);

	if (pelemsep) os << *pelemsep << ' ';
	os << *it;
}

template <typename T>
void read_element(std::istream & is, std::vector<T> & c, const std::string & separators)
{
	UNREFERENCED_PARAMETER(separators);

	T value;
	is >> value;
	c.push_back(value);
}



// TYPE SPECIALIZATIONS

// string specialization to avoid delimters being included as part of the string.
// this implies that strings cannot contain separators (usually element and closebody delimiters).
void read_element(std::istream & is, std::vector<std::string> & c, const std::string & separators);
