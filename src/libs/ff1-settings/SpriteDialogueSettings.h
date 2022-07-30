#pragma once

#include "SettingsBase.h"
#include "common_symbols.h"

class CSpriteDialogueSettings : public SettingsBase
{
public:
	CSpriteDialogueSettings(CFFHacksterProject& proj, initflag flag = initflag::read);
	CSpriteDialogueSettings(CFFHacksterProject & proj, CString sectionname, initflag flag = initflag::read);
	~CSpriteDialogueSettings();

	void SetDefaults();
	bool Read();
	bool Write();

	bool ShowActualText;
	int ShortTextLength;
	bool ThrowOnBadSpriteAddr;
};
