#pragma once

#include "SettingsBase.h"
#include "common_symbols.h"
class CFFHacksterProject;

class TextEditorSettings : public SettingsBase
{
public:
	TextEditorSettings(CFFHacksterProject & proj, initflag flag = initflag::read);
	TextEditorSettings(CFFHacksterProject& proj, CString sectionname, initflag flag = initflag::read);
	virtual ~TextEditorSettings();

	virtual void SetDefaults();
	virtual bool Read();
	virtual bool Write();

	bool ReadDteSetting(CString key, bool defaultvalue);
	void WriteDteSetting(CString key, bool value);

	bool ShowActualText;
	//int ShortTextLength;
};
