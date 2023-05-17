#pragma once

#include "FFHDataValue.h"

namespace ffh
{
	namespace fda
	{
		FFHDataValue& operator>>(FFHDataValue& stg, bool& value);
		FFHDataValue& operator<<(FFHDataValue& stg, const bool& value);

		FFHDataValue& operator>>(FFHDataValue& stg, std::string& value);
		FFHDataValue& operator<<(FFHDataValue& stg, const std::string& value);

		FFHDataValue& operator>>(FFHDataValue& stg, int& value);
		FFHDataValue& operator<<(FFHDataValue& stg, const int& value);
	}
}
