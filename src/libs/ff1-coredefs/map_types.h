#pragma once

#include <map>
#include <afx.h>
#include <sstream>
#include <iostream>
#include "pair_result.h"
#include "stream_ops.h"

// TYPES

using mfcstrstrmap = std::map<CString, CString>;
using mfcstrintmap = std::map<CString, int>;
using intmfcstrmap = std::map<int, CString>;

using stdstrstrmap = std::map<std::string, std::string>;
using stdstrintmap = std::map<std::string, int>;
using intstdstrmap = std::map<int, std::string>;



// IMPLEMENTATION

// Returns true if the container has the specified key.
template <typename K, typename T>
bool has(const std::map<K, T> & cont, K key)
{
	return cont.find(key) != cend(cont);
}

// Returns true if the key is in the container.
// is_in() reverses the params of has(), with the key first and container second.
// It's used in places with small throwaway containers defined inplace.
//     e.g. is_in(strvalue, maptype{{key1,val1}, {key2,val2}, {key3,val3}})
template <typename K, typename T>
bool is_in(const K & key, const std::map<K,T> & cont)
{
	return cont.find(key) != cend(cont);
}

template <typename K, typename T>
pair_result<T> find(const std::map<K, T> & cont, K key)
{
	auto iter = cont.find(key);
	if (iter == cend(cont))
		return pair_result<T>();
	return pair_result<T>(true, iter->second);
}

template <typename K, typename T>
T findvalue(const std::map<K, T> & cont, K key)
{
	auto iter = cont.find(key);
	if (iter != cend(cont))
		return iter->second;

	throw std::invalid_argument("map doesn't contain key '" + key + " '.");
}

template <typename K, typename T>
T find_or_default(const std::map<K, T> & cont, K key, T defaultvalue)
{
	auto iter = cont.find(key);
	return iter != cend(cont) ? iter->second : defaultvalue;
}

template <typename K, typename T>
typename std::map<K, T>::const_iterator find_by_data(const std::map<K, T> & cont, const T & refdata)
{
	auto it = std::find_if(cbegin(cont), cend(cont), [&](const auto & kv) { return kv.second == refdata; });
	return it;
}

template <typename K, typename T>
typename std::map<K, T>::iterator find_by_data(std::map<K, T> & cont, const T & refdata)
{
	auto it = std::find_if(begin(cont), end(cont), [&](const auto & kv) { return kv.second == refdata; });
	return it;
}

template <typename K, typename T>
bool has_data(const std::map<K, T> & cont, const T & refdata)
{
	auto end = cend(cont);
	auto it = std::find_if(cbegin(cont), end, [&data](const auto & kv) { return kv.second == refdata; });
	return it != end;
}


// OPERATORS

template <typename KEY, typename VALUE>
std::map<KEY, VALUE> operator+(const std::map<KEY, VALUE> & themap, const std::map<KEY, VALUE> & add)
{
	std::map<KEY, VALUE> newmap(themap);
	newmap.insert(begin(add), end(add));
	return newmap;
}


#define MAP_DELIMITERS "<>{},map"

template <typename KEY, typename VALUE>
std::ostream & operator << (std::ostream & os, const std::map<KEY, VALUE> & c)
{
	return stream_out(os, c, MAP_DELIMITERS);
}

template <typename KEY, typename VALUE>
std::istream & operator >> (std::istream & is, std::map<KEY, VALUE> & c)
{
	return stream_in(is, c, MAP_DELIMITERS);
}

template <typename KEY, typename VALUE>
void write_element(std::ostream & os, const std::map<KEY, VALUE> & c, typename std::map<KEY, VALUE>::const_iterator it, const char * pelemsep)
{
	UNREFERENCED_PARAMETER(c);

	if (pelemsep) os << *pelemsep << ' ';
	os << it->first << '=' << it->second;
}

template <typename KEY, typename VALUE>
void read_element(std::istream & is, std::map<KEY, VALUE> & c, const std::string & separators)
{
	char assign = 0;
	KEY thekey;
	VALUE theval;
	is >> thekey >> assign;
	if (assign != '=') throw std::runtime_error("missing equal sign between key and value at position " + std::to_string(is.tellg()));

	is >> theval;
	c[thekey] = theval;
}

// string specialization to avoid delimters being included as part of the string.
// this implies that strings cannot contain separators (usually element and closebody delimiters).

template <typename VALUE>
void read_element(std::istream & is, std::map<std::string, VALUE> & c, const std::string & separators)
{
	UNREFERENCED_PARAMETER(separators);

	auto thekey = read_until(is, "=" + separators);
	auto assign = is.get();
	if (assign != '=') throw std::runtime_error("missing equal sign between key and value at position " + std::to_string(is.tellg()));

	VALUE theval;
	is >> theval;
	c[thekey] = theval;
}


// TYPE SPECIALIZATIONS

// string specialization to avoid delimters being included as part of the string.
// this implies that strings cannot contain separators (usually element and closebody delimiters).
void read_element(std::istream & is, std::map<std::string, std::string> & c, const std::string & separators);
