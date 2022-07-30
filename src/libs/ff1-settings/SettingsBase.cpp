#include "stdafx.h"
#include "SettingsBase.h"
#include <FFHacksterProject.h>

SettingsBase::SettingsBase(CFFHacksterProject& proj, CString sectionname)
	: m_proj(proj)
	, m_sectionname(sectionname)
{
}

SettingsBase::~SettingsBase()
{
}
