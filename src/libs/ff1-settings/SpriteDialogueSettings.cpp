#include "stdafx.h"
#include "SpriteDialogueSettings.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "type_support.h"

#define SECT_NAME "DIALOGUE"
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
	READ_SETTING_BOOL(ShowActualText);
	READ_SETTING_DEC(ShortTextLength);
	READ_SETTING_BOOL(ThrowOnBadSpriteAddr);
	return true;
}

bool CSpriteDialogueSettings::Write()
{
	WRITE_SETTING_BOOL(ShowActualText);
	WRITE_SETTING_DEC(ShortTextLength);
	WRITE_SETTING_BOOL(ThrowOnBadSpriteAddr);
	return true;
}