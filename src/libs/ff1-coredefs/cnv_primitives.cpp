#include "stdafx.h"
#include "cnv_primitives.h"

namespace ffh
{
	namespace cnv
	{

		// Converts from %X to integer.
		int hex(const std::string& text)
		{
			return strtoul(text.c_str(), nullptr, 16);
		}

		// Converts integer to %0*X, with * specified by the digits param.
		std::string hex(int value, int digits)
		{
			char buf[100] = { 0 };
			sprintf_s(buf, "%0*X", digits, value);
			return std::string(buf);
		}

		int dec(const std::string& text)
		{
			return strtoul(text.c_str(), nullptr, 10);
		}

		std::string dec(int value)
		{
			char buf[100] = { 0 };
			sprintf_s(buf, "%d", value);
			return std::string(buf);
		}

	}
}