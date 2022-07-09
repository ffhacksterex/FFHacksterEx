#include "stdafx.h"
#include "SpriteDialogueSettings.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "type_support.h"

using namespace Ini;
using namespace Types;

#define SECT_DIALOGUE "DIALOGUE"
#define DIALOGUE_SHOWACTUALTEXT "ShowActualText"
#define DIALOGUE_SHOWACTUALTEXT_DEFAULT true
#define DIALOGUE_SHORTTEXTLENGTH "ShortTextLength"
#define DIALOGUE_SHORTTEXTLENGTH_DEFAULT 50
#define DIALOGUE_THROWONBADSPRITEADDR "ThrowOnBadSpriteAddr"
#define DIALOGUE_THROWONBADSPRITEADDR_DEFAULT false

CSpriteDialogueSettings::CSpriteDialogueSettings(CFFHacksterProject & proj, initflag flag)
	: m_proj(&proj)
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
	ShowActualText = DIALOGUE_SHOWACTUALTEXT_DEFAULT;
	ShortTextLength = DIALOGUE_SHORTTEXTLENGTH_DEFAULT;
	ThrowOnBadSpriteAddr = DIALOGUE_THROWONBADSPRITEADDR_DEFAULT;
}

bool CSpriteDialogueSettings::Read()
{
	ShowActualText = ReadIniBool(m_proj->EditorSettingsPath, SECT_DIALOGUE, DIALOGUE_SHOWACTUALTEXT, DIALOGUE_SHOWACTUALTEXT_DEFAULT);
	ShortTextLength = dec(ReadIni(m_proj->EditorSettingsPath, SECT_DIALOGUE, DIALOGUE_SHORTTEXTLENGTH, dec(DIALOGUE_SHORTTEXTLENGTH_DEFAULT)));
	ThrowOnBadSpriteAddr = dec(ReadIni(m_proj->EditorSettingsPath, SECT_DIALOGUE, DIALOGUE_THROWONBADSPRITEADDR, dec(DIALOGUE_THROWONBADSPRITEADDR_DEFAULT)));
	return true;
}

bool CSpriteDialogueSettings::Write()
{
	WriteIniBool(m_proj->EditorSettingsPath, SECT_DIALOGUE, DIALOGUE_SHOWACTUALTEXT, ShowActualText);
	WriteIni(m_proj->EditorSettingsPath, SECT_DIALOGUE, DIALOGUE_SHORTTEXTLENGTH, dec(ShortTextLength));
	WriteIni(m_proj->EditorSettingsPath, SECT_DIALOGUE, DIALOGUE_THROWONBADSPRITEADDR, dec(ThrowOnBadSpriteAddr));
	return true;
}