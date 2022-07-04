#include "stdafx.h"
#include "EnemyEditorSettings.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"

using namespace Ini;

constexpr auto SECT_ENEMIES					= "ENEMIES";
constexpr auto ENEMIES_BYTE15ASFLAGS		= "Byte15AsFlags";
constexpr auto ENEMIES_BYTE15ASFLAGS_DEFAULT = false;
constexpr auto ENEMIES_BYTE15NAME			= "Byte $0E Name";      // byte $0# (14) is the 15th byte in the enemy record
constexpr auto ENEMIES_BYTE15NAME_DEFAULT	= "Attack Elm";
constexpr auto ENEMIES_VIEWUSAGE			= "ViewUsage";
constexpr auto ENEMIES_VIEWUSAGE_DEFAULT	= true;

CEnemyEditorSettings::CEnemyEditorSettings(CFFHacksterProject & proj, initflag flag)
	: Project(&proj)
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
	Byte15AsFlags = ENEMIES_BYTE15ASFLAGS_DEFAULT;
	Byte15Name = ENEMIES_BYTE15NAME_DEFAULT;
	ViewUsageData = ENEMIES_VIEWUSAGE_DEFAULT;
}

bool CEnemyEditorSettings::Read()
{
	Byte15AsFlags = ReadIniBool(Project->EditorSettingsPath, SECT_ENEMIES, ENEMIES_BYTE15ASFLAGS, ENEMIES_BYTE15ASFLAGS_DEFAULT);
	Byte15Name = ReadIni(Project->EditorSettingsPath, SECT_ENEMIES, ENEMIES_BYTE15NAME, ENEMIES_BYTE15NAME_DEFAULT);
	ViewUsageData = ReadIniBool(Project->EditorSettingsPath, SECT_ENEMIES, ENEMIES_VIEWUSAGE, ENEMIES_VIEWUSAGE_DEFAULT);
	return true;
}

bool CEnemyEditorSettings::Write()
{
	WriteIniBool(Project->EditorSettingsPath, SECT_ENEMIES, ENEMIES_BYTE15ASFLAGS, Byte15AsFlags);
	WriteIni(Project->EditorSettingsPath, SECT_ENEMIES, ENEMIES_BYTE15NAME, Byte15Name);
	WriteIniBool(Project->EditorSettingsPath, SECT_ENEMIES, ENEMIES_VIEWUSAGE, ViewUsageData);
	return true;
}