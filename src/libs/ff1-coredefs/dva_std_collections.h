#pragma once
//#include "FFHDataValue.h"
struct FFHDataValue;
#include <vector>

namespace ffh
{
	namespace fda
	{
		const FFHDataValue& operator>>(const FFHDataValue& stg, std::vector<std::string>& value);
		FFHDataValue& operator<<(FFHDataValue& stg, const std::vector<std::string>& value);
	}
}
