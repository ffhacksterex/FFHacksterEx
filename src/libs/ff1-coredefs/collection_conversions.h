#pragma once
#include <string>
#include <vector>

namespace ffh
{
	namespace cnv
	{
		std::vector<std::string> tostd(const std::vector<CString>& v);
		std::vector<CString> tomfc(const std::vector<std::string>& v);
	}
}
