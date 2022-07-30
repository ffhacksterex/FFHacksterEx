#pragma once

#include "SettingsBaseSymbols.h"
#include "common_symbols.h"
class CFFHacksterProject;

//REFACTOR - consider how to move settings management into a factory

class SettingsBase
{
public:
	SettingsBase(CFFHacksterProject& proj, CString sectionname);
	virtual ~SettingsBase();

	virtual void SetDefaults() = 0;
	virtual bool Read() = 0;
	virtual bool Write() = 0;

protected:
	CString m_sectionname;
	CFFHacksterProject& m_proj;
};
