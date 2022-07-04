#include "stdafx.h"
#include "MagicEditorSettings.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"

using namespace Ini;

#define SECT_MAGIC "MAGIC"
#define MAGIC_BYTE7NAME "Byte7Name"
#define MAGIC_BYTE7NAME_DEFAULT "Byte 7"

CMagicEditorSettings::CMagicEditorSettings(CFFHacksterProject & proj, initflag flag)
	: m_proj(&proj)
{
	if (flag == initflag::read)
		Read();
	else
		SetDefaults();
}

void CMagicEditorSettings::SetDefaults()
{
	Byte7Name = MAGIC_BYTE7NAME_DEFAULT;
}

bool CMagicEditorSettings::Read()
{
	Byte7Name = ReadIni(m_proj->EditorSettingsPath, SECT_MAGIC, MAGIC_BYTE7NAME, MAGIC_BYTE7NAME_DEFAULT);
	return true;
}

bool CMagicEditorSettings::Write()
{
	WriteIni(m_proj->EditorSettingsPath, SECT_MAGIC, MAGIC_BYTE7NAME, Byte7Name);
	return true;
}