#include "stdafx.h"
#include "EnemyEditorSettings.h"
#include <FFH2Project.h>
#include <SettingDataAccessor.h>
#include <string_conversions.hpp>

constexpr auto SECT_NAME = "enemies";
#define Byte15AsFlags_default false
#define Byte15AsFlags_name "Byte15AsFlags"
#define Byte15Name_default "Attack Elm"
#define Byte15Name_name "Byte15Name"
#define ViewUsage_default true
#define ViewUsage_name "ViewUsage"

namespace {
	FFH2Project dummy;
}

CEnemyEditorSettings::CEnemyEditorSettings(CFFHacksterProject& proj, initflag flag)
	: CEnemyEditorSettings(proj, SECT_NAME, flag)
{
	FFH_SWITCH_TO_FFH2;
}

CEnemyEditorSettings::CEnemyEditorSettings(CFFHacksterProject& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
	, v(dummy, "<error>")
{
	FFH_SWITCH_TO_FFH2;
}

CEnemyEditorSettings::CEnemyEditorSettings(FFH2Project& proj, initflag flag)
	: CEnemyEditorSettings(proj, SECT_NAME, flag)
{
}

CEnemyEditorSettings::CEnemyEditorSettings(FFH2Project& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
	, v(proj, ffh::str::tostd(sectionname))
{
	v.EnsureSetting(Byte15AsFlags_name, "bool", ppstr(Byte15AsFlags_default), "a");
	v.EnsureSetting(Byte15Name_name, "str", ppstr(Byte15Name_default), "");
	v.EnsureSetting(ViewUsage_name, "bool", ppstr(ViewUsage_default), "a");

	if (flag == initflag::read)
		Read();
	else
		SetDefaults();
}

CEnemyEditorSettings::~CEnemyEditorSettings()
{
}

void CEnemyEditorSettings::SetDefaults()
{
	Byte15AsFlags = Byte15AsFlags_default;
	Byte15Name = Byte15Name_default;
	ViewUsage = ViewUsage_default;
}

bool CEnemyEditorSettings::Read()
{
	Byte15AsFlags = v.get<bool>(Byte15AsFlags_name);
	Byte15Name = ffh::str::tomfc(v.get<std::string>(Byte15Name_name));
	ViewUsage = v.get<bool>(ViewUsage_name);
	return true;
}

bool CEnemyEditorSettings::Write()
{
	v.set<bool>(Byte15AsFlags_name, Byte15AsFlags);
	v.set<std::string>(Byte15Name_name, ffh::str::tostd(Byte15Name));
	v.set<bool>(ViewUsage_name, ViewUsage);
	return true;
}