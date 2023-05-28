#include "stdafx.h"
#include "SpriteDialogueSettings.h"
#include <FFH2Project.h>
#include <SettingValueAccessor.h>
#include <sva_primitives.h>
#include "ini_functions.h"
#include <string_conversions.hpp>
#include "type_support.h"

#define SECT_NAME "dialogue"
#define ShowActualText_default true
#define ShortTextLength_default 50
#define ThrowOnBadSpriteAddr_default false

CSpriteDialogueSettings::CSpriteDialogueSettings(CFFHacksterProject& proj, initflag flag)
	: CSpriteDialogueSettings(proj, SECT_NAME, flag)
{
}

CSpriteDialogueSettings::CSpriteDialogueSettings(CFFHacksterProject& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
{
	FFH_SWITCH_TO_FFH2;
	if (flag == initflag::read)
		Read();
	else
		SetDefaults();
}

CSpriteDialogueSettings::CSpriteDialogueSettings(FFH2Project& proj, initflag flag)
	: CSpriteDialogueSettings(proj, SECT_NAME, flag)
{
}

CSpriteDialogueSettings::CSpriteDialogueSettings(FFH2Project& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
{
	if (flag == initflag::read)
		Read();
	else
		SetDefaults();
}

CSpriteDialogueSettings::~CSpriteDialogueSettings()
{
}

void CSpriteDialogueSettings::SetDefaults()
{
	ShowActualText = ShowActualText_default;
	ShortTextLength = ShortTextLength_default;
	ThrowOnBadSpriteAddr = ThrowOnBadSpriteAddr_default;
}

bool CSpriteDialogueSettings::Read()
{
	ffh::fda::SettingValueAccessor s(m_prj2, ffh::str::tostd(m_sectionname));
	ShowActualText = s.get<bool>("ShowActualText");
	ShortTextLength = s.get<int>("ShortTextLength");
	ThrowOnBadSpriteAddr = s.get<bool>("ThrowOnBadSpriteAddr");
	return true;
}

bool CSpriteDialogueSettings::Write()
{
	ffh::fda::SettingValueAccessor s(m_prj2, ffh::str::tostd(m_sectionname));
	s.set<bool>("ShowActualText", ShowActualText);
	s.set<int>("ShortTextLength", ShortTextLength);
	s.set<bool>("ThrowOnBadSpriteAddr", ThrowOnBadSpriteAddr);
	return true;
}