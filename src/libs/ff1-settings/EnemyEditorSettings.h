#pragma once

#include "common_symbols.h"
#include "SettingsBase.h"

class CEnemyEditorSettings : public SettingsBase
{
public:
	CEnemyEditorSettings(CFFHacksterProject & proj, initflag flag = initflag::read);
	CEnemyEditorSettings(CFFHacksterProject& proj, CString sectionname, initflag flag = initflag::read);
	~CEnemyEditorSettings();

	void SetDefaults();
	bool Read();
	bool Write();

	bool Byte15AsFlags;
	CString Byte15Name;
	bool ViewUsage;
};
