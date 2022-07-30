#include "stdafx.h"
#include "EnemyEditorSettings.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"

using namespace Ini;

constexpr auto SECT_NAME = "ENEMIES";
#define Byte15AsFlags_default false
#define Byte15Name_default "Attack Elm"
#define ViewUsage_default true

CEnemyEditorSettings::CEnemyEditorSettings(CFFHacksterProject& proj, initflag flag)
	: CEnemyEditorSettings(proj, SECT_NAME, flag)
{
}

CEnemyEditorSettings::CEnemyEditorSettings(CFFHacksterProject& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
{
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
	READ_SETTING_BOOL(Byte15AsFlags);
	READ_SETTING_STR(Byte15Name);
	READ_SETTING_BOOL(ViewUsage);
	return true;
}

bool CEnemyEditorSettings::Write()
{
	WRITE_SETTING_BOOL(Byte15AsFlags);
	WRITE_SETTING_STR(Byte15Name);
	WRITE_SETTING_BOOL(ViewUsage);
	return true;
}