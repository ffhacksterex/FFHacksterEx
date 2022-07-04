#pragma once

#include "common_symbols.h"
class CFFHacksterProject;

class CBattleEditorSettings
{
public:
	CBattleEditorSettings(CFFHacksterProject& proj, initflag flag = initflag::read);
	~CBattleEditorSettings();

	void SetDefaults();
	bool Read();
	bool Write();

	bool ViewUsageData;

private:
	CFFHacksterProject* Project;
};

