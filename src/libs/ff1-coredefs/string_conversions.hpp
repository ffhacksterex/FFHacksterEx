#pragma once

#include <afx.h>
#include <string>

namespace ffh
{
	namespace str
	{
		inline std::string tostd(const CString& cs) { return (LPCSTR)cs; }
		inline CString tomfc(const std::string& str) { return str.c_str(); }
	}
}

namespace str = ffh::str; //REMOVE
