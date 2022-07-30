#pragma once

#include "common_symbols.h"
#include "SettingsBase.h"

class CMagicEditorSettings : public SettingsBase
{
public:
	CMagicEditorSettings(CFFHacksterProject & proj, initflag flag = initflag::read);
	CMagicEditorSettings(CFFHacksterProject& proj, CString sectionname, initflag flag = initflag::read);

	void SetDefaults();
	bool Read();
	bool Write();

	CString Byte7Name;
};
