#pragma once

#include "common_symbols.h"
class CFFHacksterProject;

class TextEditorSettings
{
public:
	TextEditorSettings(CFFHacksterProject & proj, initflag flag = initflag::read);
	~TextEditorSettings();

	void SetDefaults();
	bool Read();
	bool Write();

	bool ReadDteSetting(CString key, bool defaultvalue);
	void WriteDteSetting(CString key, bool value);

	bool ShowActualText;
	//int ShortTextLength;

private:
	CFFHacksterProject * m_proj;
};
