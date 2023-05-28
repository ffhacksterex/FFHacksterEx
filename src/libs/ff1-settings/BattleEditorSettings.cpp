#include "stdafx.h"
#include "BattleEditorSettings.h"
#include <SettingDataAccessor.h>
#include <sva_primitives.h>
#include "FFHacksterProject.h"
#include <FFH2Project.h>
#include "ini_functions.h"

constexpr auto SECT_NAME = "BATTLE";
#define ViewUsage_default true

CBattleEditorSettings::CBattleEditorSettings(CFFHacksterProject& proj, initflag flag)
	: CBattleEditorSettings(proj, SECT_NAME, flag)
{
}

CBattleEditorSettings::CBattleEditorSettings(CFFHacksterProject& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
{
	if (flag == initflag::read)
		Read();
	else
		SetDefaults();
}

CBattleEditorSettings::CBattleEditorSettings(FFH2Project& proj, initflag flag)
	: CBattleEditorSettings(proj, SECT_NAME, flag)
{
}

CBattleEditorSettings::CBattleEditorSettings(FFH2Project& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
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
	ViewUsage = ViewUsage_default;
}

bool CBattleEditorSettings::Read()
{
	if (!m_is2) THROW_FFPROJECT_ERROR;

	ffh::acc::SettingDataAccessor s(m_prj2, "battles");
	ViewUsage = s.get<bool>("ViewUsage");
	return true;
}

bool CBattleEditorSettings::Write()
{
	if (!m_is2) THROW_FFPROJECT_ERROR;

	ffh::acc::SettingDataAccessor s(m_prj2, "battles");
	s.set("ViewUsage", ViewUsage);
	return true;
}