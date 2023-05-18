#pragma once

struct FFHSettingValue;

namespace ffh
{
	namespace fda //TODO - rename to fda (field data accessors)
	{
		FFHSettingValue& operator>>(FFHSettingValue& stg, bool& value);
		FFHSettingValue& operator<<(FFHSettingValue& stg, const bool& value);

		FFHSettingValue& operator>>(FFHSettingValue& stg, int& value);
		FFHSettingValue& operator<<(FFHSettingValue& stg, const int& value);

		FFHSettingValue& operator>>(FFHSettingValue& stg, std::string& value);
		FFHSettingValue& operator<<(FFHSettingValue& stg, const std::string& value);
	}

}