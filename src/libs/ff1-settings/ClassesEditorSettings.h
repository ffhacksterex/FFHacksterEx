#pragma once

#include "common_symbols.h"
#include "SettingsBase.h"
#include <SettingDataAccessor.h>

class CClassesEditorSettings : public SettingsBase
{
public:
	CClassesEditorSettings(CFFHacksterProject & proj, initflag flag = initflag::read);
	CClassesEditorSettings(CFFHacksterProject& proj, CString sectionname, initflag flag = initflag::read);
	CClassesEditorSettings(FFH2Project& proj, initflag flag = initflag::read);
	CClassesEditorSettings(FFH2Project& proj, CString sectionname, initflag flag = initflag::read);

	virtual void SetDefaults();
	virtual bool Read();
	virtual bool Write();

	bool IgnoreHoldMP1 = false;
	bool IgnoreHoldMP2 = false;
	bool IgnoreCapMP1 = false; // This is Post-HoldMP1
	bool IgnoreCapMP2 = false; // This is Post-HoldMP2
	bool IgnoreMPRange = false;
	bool IgnoreBBMA1 = false;
	bool IgnoreBBMA2 = false;

private:
	ffh::acc::SettingDataAccessor s;
};
