#pragma once

#include "common_symbols.h"
class CFFHacksterProject;

class CSpriteDialogueSettings
{
public:
	CSpriteDialogueSettings(CFFHacksterProject & proj, initflag flag = initflag::read);
	~CSpriteDialogueSettings();

	void SetDefaults();
	bool Read();
	bool Write();

	bool ShowActualText;
	int ShortTextLength;
	bool ThrowOnBadSpriteAddr;

private:
	CFFHacksterProject * m_proj;
};
