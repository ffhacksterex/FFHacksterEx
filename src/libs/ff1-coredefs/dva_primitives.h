#pragma once

#include "FFHValue.h"

namespace ffh
{
	namespace fda
	{
		FFHValue& operator>>(FFHValue& stg, bool& value);
		FFHValue& operator<<(FFHValue& stg, const bool& value);

		FFHValue& operator>>(FFHValue& stg, std::string& value);
		FFHValue& operator<<(FFHValue& stg, const std::string& value);

		FFHValue& operator>>(FFHValue& stg, int& value);
		FFHValue& operator<<(FFHValue& stg, const int& value);
	}
}
