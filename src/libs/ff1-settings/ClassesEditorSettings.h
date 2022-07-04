#pragma once

#include "common_symbols.h"
class CFFHacksterProject;

class CClassesEditorSettings
{
public:
	CClassesEditorSettings(CFFHacksterProject & proj, initflag flag = initflag::read);

	void SetDefaults();
	bool Read();
	bool Write();

	bool IgnoreHoldMP1 = false;
	bool IgnoreHoldMP2 = false;
	bool IgnoreCapMP1 = false; // This is Post-HoldMP1
	bool IgnoreCapMP2 = false; // This is Post-HoldMP2
	bool IgnoreMPRange = false;
	bool IgnoreBBMA1 = false;
	bool IgnoreBBMA2 = false;

private:
	CFFHacksterProject * m_proj;
};
