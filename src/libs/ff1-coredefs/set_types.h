#pragma once

#include <set>
#include "pair_result.h"
#include "stream_ops.h"


using intset = std::set<int>;
using stdstringset = std::set<std::string>;
using mfcstringset = std::set<CString>;



// IMPLEMENTATION

template <typename T>
std::set<T> & append(std::set<T> & theset, const std::set<T> & inputset)
{
	theset.insert(begin(inputset), end(inputset));
	return theset;
}

template <typename T>
bool has(const std::set<T> & cont, const T & key)
{
	return cont.find(key) != cend(cont);
}

// Returns true if the key is in the container.
// is_in() reverses the params of has(), with the key first and container second.
// It's used in places with small throwaway containers defined inplace.
//     e.g. is_in(strvalue, settype{value1, value2, value3, value4})
template <typename T>
bool is_in(const T & key, const std::set<T> & cont)
{
	return cont.find(key) != cend(cont);
}

//DEVNOTE - this only exists to effectively allow overloading with other collections using the same function
template <typename T>
pair_result<T> find(const std::set<T> & cont, const T & key)
{
	auto iter = cont.find(key);
	if (iter == cend(cont))
		return pair_result<T>();

	//return pair_result(true, *iter); //REFACTOR - revisit
	return pair_result<T>(true, *iter);
}

//REFACTOR - maybe just use is_in and remove this function from use, no need to return a copy of the value.
template <typename T>
T findvalue(const std::set<T> & cont, const T & key)
{
	auto iter = cont.find(key);
	if (iter != cend(cont))
		return key;
		//return pair_result(true, *iter); //REFACTOR - revisit

	throw std::invalid_argument("set doesn't contain key '" + key + " '.");
}

template <typename T>
T find_or_default(const std::set<T> & cont, T key, T defaultvalue)
{
	auto iter = cont.find(key);
	return iter != cend(cont) ? *iter : defaultvalue;
}



// OPERATORS

template <typename T>
std::set<T> operator+(const std::set<T> & theset, const std::set<T> & add)
{
	std::set<T> newset(theset);
	return append(newset, add);
}


#define SET_DELIMITERS "<>(),set"

template <typename T>
std::ostream & operator << (std::ostream & os, const std::set<T> & c)
{
	return stream_out(os, c, SET_DELIMITERS);
}

template <typename T>
std::istream & operator >> (std::istream & is, std::set<T> & c)
{
	return stream_in(is, c, SET_DELIMITERS);
}

template <typename T>
void write_element(std::ostream & os, const std::set<T> & c, typename std::set<T>::const_iterator it, const char * pelemsep)
{
	if (pelemsep) os << *pelemsep << ' ';
	os << *it;
}

template <typename T>
void read_element(std::istream & is, std::set<T> & c, const std::string & separators)
{
	T value;
	is >> value;
	c.insert(value);
}


// TYPE SPECIALIZATIONS

// string specialization to avoid delimters being included as part of the string.
// this implies that strings cannot contain separators (usually element and closebody delimiters).
void read_element(std::istream & is, std::set<std::string> & c, const std::string & separators);
