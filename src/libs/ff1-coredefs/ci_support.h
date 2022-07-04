#pragma once

#include <string>

namespace ci
{

	bool comp_lower(char ch1, char ch2);
	size_t find(const std::string & strtosearch, const std::string & strtofind);    // returns std::string::npos if not found
	bool compare(const std::string & strtosearch, const std::string & strtocompare);

	bool pred_ch(char lhs, char rhs);
	bool pred_str(const std::string & lhs, const std::string & rhs);

}
