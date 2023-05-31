#pragma once

#include "SettingsBaseSymbols.h"
#include "common_symbols.h"
class CFFHacksterProject;
class FFH2Project;

//REFACTOR - consider how to move settings management into a factory

class SettingsBase
{
public:
	SettingsBase(CFFHacksterProject& proj, CString sectionname);
	SettingsBase(FFH2Project& proj, CString sectionname);
	virtual ~SettingsBase();

	virtual void SetDefaults() = 0;
	virtual bool Read() = 0;
	virtual bool Write() = 0;

protected:
	CString m_sectionname; //TODO - change to std::string
	CFFHacksterProject& m_proj;
	FFH2Project& m_prj2;
	bool m_is2 = false;
};
