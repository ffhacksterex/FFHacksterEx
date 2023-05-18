#include "stdafx.h"
#include "dva_primitives.h"
#include "DataValueAccessor.h"
#include <set>

namespace ffh
{
	namespace fda
	{
		//TODO - figure out how to reduce this boilerplate

		FFHDataValue& operator>>(FFHDataValue& stg, bool& value)
		{
			const static std::string mytype = "bool";
			if (stg.type != mytype)
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, mytype);

			value = (stg.data == "true");
			return stg;
		}
		FFHDataValue& operator<<(FFHDataValue& stg, const bool& value)
		{
			const static std::string mytype = "bool";
			if (stg.type != mytype)
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, mytype);

			stg.data = value ? "true" : "false";				
			return stg;
		}

		FFHDataValue& operator>>(FFHDataValue& stg, std::string& value)
		{
			const static std::string mytype = "str";
			if (stg.type != mytype)
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, mytype);

			value = stg.data;
			return stg;
		}
		FFHDataValue& operator<<(FFHDataValue& stg, const std::string& value)
		{
			const static std::string mytype = "str";
			if (stg.type != mytype)
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, mytype);

			stg.data = value;
			return stg;
		}

		namespace {
			const std::set<std::string> intTypes = { "int", "hex", "addr", "rgb" };
		}

		FFHDataValue& operator>>(FFHDataValue& stg, int& value)
		{
			const static std::string mytype = "int";
			if (intTypes.find(stg.type) == cend(intTypes))
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, mytype);

			sscanf(stg.data.c_str(), stg.format.c_str(), &value);
			return stg;
		}
		FFHDataValue& operator<<(FFHDataValue& stg, const int& value)
		{
			const static std::string mytype = "int";
			if (intTypes.find(stg.type) == cend(intTypes))
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, mytype);

			char buf[128] = { 0 };
			sprintf_s(buf, stg.format.c_str(), value);
			stg.data = buf; //TODO - do I need dquotes here? should I use nlohmann here?
			return stg;
		}
		//		template <>
		//		inline int FromData(const FFHDataValue& value)
		//		{
		//			static std::set<std::string> intTypes = { "int", "hex", "addr", "rgb" };
		//			if (intTypes.find(value.type) != cend(intTypes))
		//			{
		//				int i = 0;
		//				auto fmt = value.format;
		//				sscanf(value.data.c_str(), fmt.c_str(), &i);
		//				return i;
		//			}
		//
		//			throw std::runtime_error("Value '" + value.name + "' doesn't support conversion from '" + value.type + "' to int.");
		//		}
		//
		//		template <>
		//		inline bool FromData(const FFHDataValue& value)
		//		{
		//			if (value.type == "bool")
		//				return value.data == "true";
		//
		//			THROW_DVA_TYPE_ERROR(value.name, value.type, "bool");
		//		}
	}
}