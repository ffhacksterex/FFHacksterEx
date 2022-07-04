#include "stdafx.h"
#include "BattleEditorSettings.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"

constexpr auto SECT_BATTLE = "BATTLE";
constexpr auto VIEWUSAGE = "ViewUsage";
constexpr auto VIEWUSAGE_DEFAULT = true;

CBattleEditorSettings::CBattleEditorSettings(CFFHacksterProject& proj, initflag flag)
	: Project(&proj)
{
	if (flag == initflag::read)
		Read();
	else
		SetDefaults();
}

CBattleEditorSettings::~CBattleEditorSettings()
{
}

void CBattleEditorSettings::SetDefaults()
{
	ViewUsageData = VIEWUSAGE_DEFAULT;
}

bool CBattleEditorSettings::Read()
{
	ViewUsageData = Ini::ReadIniBool(Project->EditorSettingsPath, SECT_BATTLE, VIEWUSAGE, VIEWUSAGE_DEFAULT);
	return true;
}

bool CBattleEditorSettings::Write()
{
	Ini::WriteIniBool(Project->EditorSettingsPath, SECT_BATTLE, VIEWUSAGE, ViewUsageData);
	return true;
}