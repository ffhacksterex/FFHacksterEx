#include "stdafx.h"
#include "sva_primitives.h"
#include "FFHSettingValue.h"
#include "SettingValueAccessor.h"
#include <set>

namespace ffh
{
	namespace fda
	{

		FFHSettingValue& operator>>(FFHSettingValue& stg, bool& value)
		{
			if (stg.type != "bool")
				THROW_SVA_TYPE_ERROR(stg.name, stg.type, "bool");

			value = (stg.data == "true");
			return stg;
		}

		FFHSettingValue& operator<<(FFHSettingValue& stg, const bool& value)
		{
			if (stg.type != "bool")
				THROW_SVA_TYPE_ERROR(stg.name, "bool", stg.type);

			stg.data = value ? "true" : "false";
			return stg;
		}

		namespace {
			std::set<std::string> s_intTypes = { "int", "hex", "addr", "rgb" };
		}

		FFHSettingValue& operator>>(FFHSettingValue& stg, int& value)
		{
			if (s_intTypes.find(stg.type) == cend(s_intTypes))
				THROW_SVA_TYPE_ERROR(stg.name, stg.type, "int");

			sscanf(stg.data.c_str(), stg.format.c_str(), &value);
			return stg;
		}

		FFHSettingValue& operator<<(FFHSettingValue& stg, const int& value)
		{
			if (s_intTypes.find(stg.type) == cend(s_intTypes))
				THROW_SVA_TYPE_ERROR(stg.name, "int", stg.type);

			char buf[24] = { 0 };
			sprintf_s<24>(buf, stg.format.c_str(), stg.data, value);
			stg.data = buf;
			return stg;
		}

		FFHSettingValue& operator>>(FFHSettingValue& stg, std::string& value)
		{
			if (stg.type != "str")
				THROW_SVA_TYPE_ERROR(stg.name, stg.type, "str");

			value = stg.data;
			return stg;
		}

		FFHSettingValue& operator<<(FFHSettingValue& stg, const std::string& value)
		{
			if (stg.type != "str")
				THROW_SVA_TYPE_ERROR(stg.name, "str", stg.type);

			stg.data = value;
			return stg;
		}
	}
}