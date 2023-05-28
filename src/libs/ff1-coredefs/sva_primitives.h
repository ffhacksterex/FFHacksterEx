#pragma once

struct FFHSetting;

namespace ffh
{
	namespace fda //TODO - rename to fda (field data accessors)
	{
		FFHSetting& operator>>(FFHSetting& stg, bool& value);
		FFHSetting& operator<<(FFHSetting& stg, const bool& value);

		FFHSetting& operator>>(FFHSetting& stg, int& value);
		FFHSetting& operator<<(FFHSetting& stg, const int& value);

		FFHSetting& operator>>(FFHSetting& stg, std::string& value);
		FFHSetting& operator<<(FFHSetting& stg, const std::string& value);
	}

}