#include "stdafx.h"
#include "IniOverrider.h"
#include "ini_functions.h"

namespace Ini
{

	IniOverrider::IniOverrider(CString ainifile, CString anewsource)
	{
		m_inifile = ainifile;
		m_newsource = anewsource;
	}

	//DEVNOTE - the type system is currently notional - i.e. informal - and isn't strongly or uniformly enforced.
	//		hence, it's possible to pass invalid types to this function, which will break any
	//		calls that later attempt to read it using typed INI functions (e.g. ReadHex) or
	//		pass the read string to conversion functions (e.g. boole, dec, hex, etc).
	//		Attempting to deduce the type is more work than it's worth, formalizing the type
	//		system would be more worthwhile.

	// Overrides the value without changing the group. If the group is blank, it remains blank.
	bool IniOverrider::OverrideWithinGroup(CString section, CString type, CString value, CString desc)
	{
		auto group = ReadIni(m_inifile, section, KEY_GROUP, "");
		return DoOverride(section, type, value, group, desc);
	}

	// Overrides the value, possibly changing its group. If 'group' is empty, then the value
	// will be ungrouped.
	bool IniOverrider::Override(CString section, CString type, CString value, CString group, CString desc)
	{
		return DoOverride(section, type, value, group, desc);
	}

	bool IniOverrider::DoOverride(CString section, CString type, CString value, CString group, CString desc)
	{
		auto prevoverrider = ReadIni(m_inifile, section, KEY_SOURCE, "");

		// If the source is newsource, no need to change anything.
		if (prevoverrider == m_newsource) return true;

		//FUTURE - revisit the multiple overrides issue, maybe the last-to-override wins? for now, first one wins.
		// If another source overrode this setting, I can't override it.
		if (!prevoverrider.IsEmpty()) {
			CString err;
			err.Format("The INI key '%s' can't be overridden because it has already been overriden by another project '%s'.\n"
				"You can manually remove the source value from the INI key, then try again.", (LPCSTR)section, (LPCSTR)prevoverrider);
			THROWEXCEPTION(std::runtime_error, (LPCSTR)err);
		}

		// Overwrite the setting, using this project as the source.
		WriteIni(m_inifile, section, KEY_SOURCE, m_newsource);
		WriteIni(m_inifile, section, "type", type);
		WriteIni(m_inifile, section, "value", value);
		WriteIni(m_inifile, section, KEY_GROUP, group);
		if (!desc.IsEmpty()) WriteIni(m_inifile, section, "desc", desc);
		return true;
	}

	// Removes a section.
	//DEVNOTE - Currently, INI failure isn't actually handled, so this function always returns true...
	bool IniOverrider::Remove(CString section)
	{
		if (Ini::HasIniSection(m_inifile, section)) {
			auto newgroup = "***HIDDEN***" + section;
			Ini::RenameSection(m_inifile, section, newgroup);
			WriteIni(m_inifile, newgroup, "ishidden", "true");
		}
		return true;
	}

	// Removes the list of sections.
	// Returns the number of sections that could NOT be removed.
	size_t IniOverrider::Remove(std::initializer_list<CString> sections)
	{
		size_t removecount = 0;
		for (auto section : sections)
			if (Remove(section)) ++removecount;
		return sections.size() - removecount;
	}
}