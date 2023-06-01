#include "stdafx.h"
#include "cnv_primitives.h"
#include <string>

namespace ffh
{
	namespace cnv
	{
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

		namespace {
			const std::string StdHexFormat = "0x%X";
		}

		// Converts a standard hex prefixed string ("0x%X") to integer.
		// Handles negative values using the form "-0x%X", e.g. -0X12AB.
		int hex(const std::string& text)
		{
			int value = hex(text, StdHexFormat);
			return value;
		}

		// Converts a hex string with a specific format to integer.
		// Handles negative values using the form "-0x%X", e.g. -0X12AB.
		int hex(const std::string& text, const std::string & format)
		{
			//DEVNOTE - hex values can be negative, instead of One's or Two's complement,
			//	support a negative sign before the 0x, e.g. -0x12AB.
			int skip = !text.empty() && (text[0] == '-') ? 1 : 0;
			int sign = skip == 1 ? -1 : 1;

			int value = 0;
			sscanf(text.c_str() + skip, format.c_str(), &value);
			value *= sign;
			return value;
		}

		// Converts integer to %0*X, with * specified by the digits param.
		// Handles negative values using the form "-0x%X", e.g. -0X12AB.
		std::string hex(int value, int digits)
		{
			auto format = build_hex_format(digits);
			return hex(value, format);
		}

		std::string hex(int value, std::string format)
		{
			//DEVNOTE - hex values can be negative, instead of One's or Two's complement,
			//	support a negative sign before the 0x, e.g. -0x12AB.
			int sign = (value < 0) ? -1 : 1;
			char buf[128] = { 0 };
			sprintf_s(buf, format.c_str(), value * sign);
			std::string data = sign == -1 ? "-" : "";
			data += buf;
			return data;
		}

		std::string build_hex_format(int digits)
		{
			std::string format = "0x%0" + std::to_string(digits) + "X";
			return format;
		}
	}
}