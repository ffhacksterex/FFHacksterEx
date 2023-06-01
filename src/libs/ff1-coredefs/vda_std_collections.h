#pragma once

struct FFHValue;
#include <vector>

namespace ffh
{
	namespace acc
	{
		const FFHValue& operator>>(const FFHValue& stg, std::vector<std::string>& value);
		FFHValue& operator<<(FFHValue& stg, const std::vector<std::string>& value);

		const FFHValue& operator>>(const FFHValue& stg, std::vector<unsigned char>& value);
		FFHValue& operator<<(FFHValue& stg, const std::vector<unsigned char>& value);
	}
}
