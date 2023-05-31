#include "stdafx.h"
#include "MagicEditorSettings.h"
#include <FFH2Project.h>
#include "ini_functions.h"
#include <SettingDataAccessor.h>
#include <string_conversions.hpp>

using namespace Ini;

#define SECT_NAME "magic"
#define Byte7Name_name "Byte7Name"
#define Byte7Name_default "Byte 7"

CMagicEditorSettings::CMagicEditorSettings(CFFHacksterProject& proj, initflag flag)
	: CMagicEditorSettings(proj, SECT_NAME, flag)
{
	FFH_SWITCH_TO_FFH2;
}

CMagicEditorSettings::CMagicEditorSettings(CFFHacksterProject& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
{
	FFH_SWITCH_TO_FFH2;
}

CMagicEditorSettings::CMagicEditorSettings(FFH2Project& proj, initflag flag)
	: CMagicEditorSettings(proj, SECT_NAME, flag)
{
}

CMagicEditorSettings::CMagicEditorSettings(FFH2Project& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
{
	ffh::acc::SettingDataAccessor s(m_prj2, ffh::str::tostd(m_sectionname));
	s.EnsureSetting(Byte7Name_name, "str", Byte7Name_default);

	if (flag == initflag::read)
		Read();
	else
		SetDefaults();
}

void CMagicEditorSettings::SetDefaults()
{
	Byte7Name = Byte7Name_default;
}

bool CMagicEditorSettings::Read()
{
	ffh::acc::SettingDataAccessor s(m_prj2, ffh::str::tostd(m_sectionname));
	Byte7Name = ffh::str::tomfc(s.get<std::string>(Byte7Name_name));
	return true;
}

bool CMagicEditorSettings::Write()
{
	ffh::acc::SettingDataAccessor s(m_prj2, ffh::str::tostd(m_sectionname));
	s.set<std::string>(Byte7Name_name, ffh::str::tostd(Byte7Name));
	return true;
}