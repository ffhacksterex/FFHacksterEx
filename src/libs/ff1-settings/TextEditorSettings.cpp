#include "stdafx.h"
#include "TextEditorSettings.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "type_support.h"

using namespace Ini;
using namespace Types;

constexpr auto SECT_NAME = "TEXT";
#define ShowActualText_default true

TextEditorSettings::TextEditorSettings(CFFHacksterProject & proj, initflag flag)
	: TextEditorSettings(proj, SECT_NAME, flag)
{
}

TextEditorSettings::TextEditorSettings(CFFHacksterProject& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
{
	if (flag == initflag::read)
		Read();
	else
		SetDefaults();
}

TextEditorSettings::~TextEditorSettings()
{
}

void TextEditorSettings::SetDefaults()
{
	ShowActualText = ShowActualText_default;
}

bool TextEditorSettings::Read()
{
	READ_SETTING_BOOL(ShowActualText);
	return true;
}

bool TextEditorSettings::Write()
{
	WRITE_SETTING_BOOL(ShowActualText);
	return true;
}

bool TextEditorSettings::ReadDteSetting(CString key, bool defaultvalue)
{
	return ReadIniBool(m_proj.EditorSettingsPath, m_sectionname, "DTE" + key, defaultvalue);
}

void TextEditorSettings::WriteDteSetting(CString key, bool value)
{
	WriteIniBool(m_proj.EditorSettingsPath, m_sectionname, "DTE" + key, value);
}