#include "stdafx.h"
#include "vector_types.h"
#include <sstream>

size_t find_by_name(const stdstrintvector & vec, std::string name)
{
	auto iter = std::find_if(cbegin(vec), cend(vec),
		[name](const auto & node) { return node.first == name; });
	return iter != cend(vec) ? std::distance(cbegin(vec), iter) : -1;
}

size_t find_by_value(const stdstrintvector & vec, int valtofind)
{
	auto iter = std::find_if(cbegin(vec), cend(vec), [valtofind](const auto & node) { return node.second == valtofind; });
	return iter != cend(vec) ? std::distance(cbegin(vec), iter) : -1;
}

size_t find_by_prefix(const stdstrintvector & vec, std::string prefix)
{
	auto iter = std::find_if(cbegin(vec), cend(vec),
		[prefix](const auto & node) { return node.first.find(prefix) == 0; });
	return iter != cend(vec) ? std::distance(cbegin(vec), iter) : -1;
}

mfcstringvector convert(const stdstringvector & svec)
{
	auto mvec = mfcstringvector();
	for (const auto & s : svec) mvec.push_back(s.c_str());
	return mvec;
}

stdstringvector convert(const mfcstringvector & mvec)
{
	auto svec = stdstringvector();
	for (const auto & m : mvec) svec.push_back((LPCSTR)m);
	return svec;
}

std::string ToString(const bytevector & vec)
{
	std::ostringstream o;
	for (size_t st = 0; st < vec.size(); ++st) {
		if (st > 0) o << ",";
		o << (int)vec[st];
	}
	return o.str();
}

void read_element(std::istream & is, std::vector<std::string> & c, const std::string & separators)
{
	auto value = read_until(is, separators);
	c.push_back(value);
}
