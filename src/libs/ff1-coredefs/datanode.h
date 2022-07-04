#pragma once

#include <afx.h>
#include <utility>
#include <vector>
#include <set>

template <typename T>
class datanode : private std::pair<CString, T>
{
public:
	datanode() : name(first), value(second) {}
	~datanode() {}
	datanode(CString aname, const T & avalue) : name(first), value(second) {
		name = aname;
		value = avalue;
	}
	datanode(const datanode & rhs) : name(first), value(second) {
		this->operator=(rhs);
	}
	datanode & operator=(const datanode & rhs) {
		if (this != &rhs) {
			name = rhs.name;
			value = rhs.value;
		}
		return *this;
	}
	bool operator<(const datanode & rhs) {
		return name < rhs.name && value < rhs.value;
	}
	bool operator==(const datanode & rhs) {
		return name == rhs.name && value == rhs.value;
	}
	bool operator!=(const datanode & rhs) {
		return !(*this == rhs);
	}

	operator CString() { return name; }
	operator T& () { return value; }

	CString Display(int width = 2) const {
		CString cs;
		cs.Format("%s (%0*X)", name, width, value);
		return cs;
	}

	CString & name;
	T & value;
};

template <typename T>
using datanodevector = std::vector<datanode<T>>;

using dataintnode = datanode<int>;
using dataintnodevector = datanodevector<int>;

inline dataintnodevector remove_duplicates(dataintnodevector vec)
{
	//size_t removecount = 0;
	std::set<CString> uniques;
	for (size_t st = 0; st < vec.size(); ++st) {
		if (uniques.find(vec[st].name) != cend(uniques)) {
			vec.erase(vec.begin() + st);
			--st;
			//++removecount;
		}
		else {
			uniques.insert(vec[st].name);
		}
	}
	//return removecount;
	return vec;
}

inline std::vector<CString> get_names(dataintnodevector vec)
{
	std::vector<CString> names;
	names.resize(vec.size());
	for (size_t st = 0; st < vec.size(); ++st) names[st] = vec[st].name;
	return names;
}
