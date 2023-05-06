#pragma once

#include "FFHDataValue.h"
#include "DataValueAccessor.h"
#include <set>

namespace ff1coredefs
{
	template <>
	inline int FromData(const FFHDataValue& value)
	{
		//TODO - use the format
		//int i = 0;
		//if (value.type == "int") return atol(value.data.c_str());
		//if (value.type == "hex" || value.type == "addr" || value.type == "rgb") {
		//	//TODO - currently, we remove dquotes from string_t properties when loading them
		//	//auto fmt = "\"" + value.format + "\"";
		//	auto fmt = value.format;
		//	sscanf(value.data.c_str(), fmt.c_str(), &i);
		//	return i;
		//}

		//if (value.type == "int" || value.type == "hex" || value.type == "addr" || value.type == "rgb")

		static std::set<std::string> intTypes = { "int", "hex", "addr", "rgb" };
		if (intTypes.find(value.type) != cend(intTypes))
		{
			int i = 0;
			auto fmt = value.format;
			sscanf(value.data.c_str(), fmt.c_str(), &i);
			return i;
		}

		throw std::runtime_error("Value '" + value.name + "' doesn't support conversion from '" + value.type + "' to int.");
	}
}
