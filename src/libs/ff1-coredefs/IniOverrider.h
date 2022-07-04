#pragma once

#define KEY_SOURCE "source"
#define KEY_GROUP "group"

namespace Ini
{

	class IniOverrider
	{
	public:
		IniOverrider(CString ainifile, CString anewsource);

		bool OverrideWithinGroup(CString section, CString type, CString value, CString desc = "");
		bool Override(CString section, CString type, CString value, CString group, CString desc = "");
		bool Remove(CString section);
		size_t Remove(std::initializer_list<CString> sections);

	private:
		CString m_inifile;
		CString m_newsource;

		bool DoOverride(CString section, CString type, CString value, CString group, CString desc);
	};

}
