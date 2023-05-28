#pragma once

namespace ffh
{
	namespace cnv
	{
		int hex(const std::string& text);
		std::string hex(int value, int digits = 4);

		int dec(const std::string& text);
		std::string dec(int value);
	}
}
