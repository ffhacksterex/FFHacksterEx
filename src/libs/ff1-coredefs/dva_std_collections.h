#pragma once
//#include "FFHValue.h"
struct FFHValue;
#include <vector>

namespace ffh
{
	namespace fda
	{
		const FFHValue& operator>>(const FFHValue& stg, std::vector<std::string>& value);
		FFHValue& operator<<(FFHValue& stg, const std::vector<std::string>& value);
	}
}
