#include "stdafx.h"
#include "MagicEditorSettings.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"

using namespace Ini;

#define SECT_NAME "MAGIC"
#define Byte7Name_default "Byte 7"

CMagicEditorSettings::CMagicEditorSettings(CFFHacksterProject& proj, initflag flag)
	: CMagicEditorSettings(proj, SECT_NAME, flag)
{
}

CMagicEditorSettings::CMagicEditorSettings(CFFHacksterProject& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
{
	if (flag == initflag::read)
		Read();
	else
		SetDefaults();
}

void CMagicEditorSettings::SetDefaults()
{
	Byte7Name = Byte7Name_default;
}

bool CMagicEditorSettings::Read()
{
	READ_SETTING_STR(Byte7Name);
	return true;
}

bool CMagicEditorSettings::Write()
{
	WRITE_SETTING_STR(Byte7Name);
	return true;
}