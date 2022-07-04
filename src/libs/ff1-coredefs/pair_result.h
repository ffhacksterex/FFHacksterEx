#pragma once

#include <afx.h>
#include <utility>

template <typename T>
class pair_result : private std::pair<bool, T>
{
public:
	pair_result(): result(first), value(second) {}
	~pair_result() {}
	pair_result(bool res, const T & val) : result(first), value(second) {
		result = res;
		value = val;
	}
	pair_result(const pair_result & rhs) : result(first), value(second) {
		this->operator=(rhs);
	}
	pair_result & operator=(const pair_result & rhs) {
		if (this != &rhs) {
			result = rhs.result;
			value = rhs.value;
		}
		return *this;
	}

	operator bool() { return result; }
	operator T& () { return value; }

	bool & result;
	T & value;
};
