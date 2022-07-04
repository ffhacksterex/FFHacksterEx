#pragma once

#include "common_symbols.h"
class CFFHacksterProject;

class CMagicEditorSettings
{
public:
	CMagicEditorSettings(CFFHacksterProject & proj, initflag flag = initflag::read);

	void SetDefaults();
	bool Read();
	bool Write();

	CString Byte7Name;

private:
	CFFHacksterProject * m_proj;
};
