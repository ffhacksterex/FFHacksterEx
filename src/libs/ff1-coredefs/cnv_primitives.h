#pragma once

namespace ffh
{
	namespace cnv
	{
		int dec(const std::string& text);
		std::string dec(int value);

		int hex(const std::string& text);
		int hex(const std::string& text, const std::string& format);
		std::string hex(int value, int digits = 4);
		std::string hex(int value, std::string format);
		std::string build_hex_format(int digits);
	}
}
