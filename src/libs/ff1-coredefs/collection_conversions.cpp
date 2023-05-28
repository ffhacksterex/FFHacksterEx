#include "stdafx.h"
#include "collection_conversions.h"
#include "string_conversions.hpp"

namespace ffh
{
	namespace cnv
	{
		std::vector<std::string> tostd(const std::vector<CString>& v)
		{
			std::vector<std::string> res;
			for (const auto& elem : v) res.push_back(ffh::str::tostd(elem));
			return res;
		}

		std::vector<CString> tomfc(const std::vector<std::string>& v)
		{
			std::vector<CString> res;
			for (const auto& elem : v) res.push_back(ffh::str::tomfc(elem));
			return res;
		}
	}
}