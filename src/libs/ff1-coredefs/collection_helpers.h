#pragma once

#include <ostream>
#include <afx.h>
#include "vector_types.h"
#include "map_types.h"
#include "set_types.h"

// Convert between bytes and bools, used when interacting with DATA and some parts of Hackster.
bytevector convert(const boolvector & bools);
boolvector convert(const bytevector & bytes);

mfcstringvector & append(mfcstringvector & vec, const stdstringvector & insvec);


template <typename T>
std::set<T> vector_to_set(const std::vector<T> & v)
{
	std::set<T> s;
	for (const auto & item : v) {
		s.insert(item);
	}
	return s;
}

template <typename T>
std::set<T> & operator+=(std::set<T> & s, const std::vector<T> & v)
{
	for (const auto & item : v) {
		s.insert(item);
	}
	return s;
}
