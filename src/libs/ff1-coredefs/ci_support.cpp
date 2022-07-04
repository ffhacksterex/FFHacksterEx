#include "stdafx.h"
#include "ci_support.h"
#include <algorithm>
#include <cctype>

namespace ci
{

	bool comp_lower(char ch1, char ch2)
	{
		return std::tolower(ch1) == std::tolower(ch2);
	}

	size_t find(const std::string & strtosearch, const std::string & strtofind)
	{
		if (strtosearch.empty() && !strtofind.empty())
			return std::string::npos;
		if (!strtosearch.empty() && strtofind.empty())
			return std::string::npos;

		auto it = std::search(begin(strtosearch), end(strtosearch), begin(strtofind), end(strtofind), comp_lower);
		return it != strtosearch.end() ? std::distance(begin(strtosearch), it) : std::string::npos;
	}

	bool compare(const std::string & strtosearch, const std::string & strtocompare)
	{
		return std::equal(begin(strtosearch), end(strtosearch), begin(strtocompare), end(strtocompare), comp_lower);
	}

	bool pred_ch(char lhs, char rhs)
	{
		return std::toupper(lhs) < std::toupper(rhs);
	}

	bool pred_str(const std::string & lhs, const std::string & rhs)
	{
		return std::lexicographical_compare(cbegin(lhs), cend(lhs), cbegin(rhs), cend(rhs), pred_ch);
	}

} // end namespace ci