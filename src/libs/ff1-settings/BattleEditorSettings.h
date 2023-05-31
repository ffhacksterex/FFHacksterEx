#pragma once

#include "common_symbols.h"
#include "SettingsBase.h"

class CBattleEditorSettings : public SettingsBase
{
public:
	CBattleEditorSettings(CFFHacksterProject& proj, initflag flag = initflag::read);
	CBattleEditorSettings(CFFHacksterProject& proj, CString sectionname, initflag flag = initflag::read);
	CBattleEditorSettings(FFH2Project& proj, initflag flag = initflag::read);
	CBattleEditorSettings(FFH2Project& proj, CString sectionname, initflag flag = initflag::read);
	~CBattleEditorSettings();

	void SetDefaults();
	bool Read();
	bool Write();

	bool ViewUsage;
};
