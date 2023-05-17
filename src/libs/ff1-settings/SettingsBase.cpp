#include "stdafx.h"
#include "SettingsBase.h"
#include <FFHacksterProject.h>
#include <FFH2Project.h>

namespace {
	CFFHacksterProject ffdummy;
	FFH2Project ff2dummy;
}

SettingsBase::SettingsBase(CFFHacksterProject& proj, CString sectionname)
	: m_proj(proj)
	, m_sectionname(sectionname)
	, m_prj2(ff2dummy)
	, m_is2(false)
{
}

SettingsBase::SettingsBase(FFH2Project& proj, CString sectionname)
	: m_prj2(proj)
	, m_sectionname(sectionname)
	, m_proj(ffdummy)
	, m_is2(true)
{
}

SettingsBase::~SettingsBase()
{
}
