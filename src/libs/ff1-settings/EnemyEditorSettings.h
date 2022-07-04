#pragma once

#include "common_symbols.h"
class CFFHacksterProject;

class CEnemyEditorSettings
{
public:
	CEnemyEditorSettings(CFFHacksterProject & proj, initflag flag = initflag::read);
	~CEnemyEditorSettings();

	void SetDefaults();
	bool Read();
	bool Write();

	bool Byte15AsFlags;
	CString Byte15Name;
	bool ViewUsageData;

private:
	CFFHacksterProject * Project;
};
