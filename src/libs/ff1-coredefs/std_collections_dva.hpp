#pragma once

#include "FFHDataValue.h"
#include "DataValueAccessor.h"
#include <vector>

//TODO - put nlohmann part into cpp file
#include "nlohmann/nlohmann-integration.hpp"

namespace ff1coredefs
{
	template <>
	inline std::vector<std::string> FromData(const FFHDataValue& value)
	{
		if (value.type == "str[]")
		{
			auto uj = ujson::parse(value.data);
			std::vector<std::string> arr;
			uj.get_to(arr);
			return arr;
		}

		throw std::runtime_error("Value '" + value.name + "' doesn't support conversion from '" + value.type + "' to int.");
	}
}
