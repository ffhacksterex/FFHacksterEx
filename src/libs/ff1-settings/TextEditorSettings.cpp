#include "stdafx.h"
#include "TextEditorSettings.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "type_support.h"
#include <SettingDataAccessor.h>

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

TextEditorSettings::TextEditorSettings(FFH2Project& proj, initflag flag)
	: TextEditorSettings(proj, SECT_NAME, flag)
{
}

TextEditorSettings::TextEditorSettings(FFH2Project& proj, CString sectionname, initflag flag)
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
	//READ_SETTING_BOOL(ShowActualText);
	ffh::acc::SettingDataAccessor s(m_prj2, "text");
	ShowActualText = s.get<bool>("ShowActualText");
	return true;
}

bool TextEditorSettings::Write()
{
	//WRITE_SETTING_BOOL(ShowActualText);
	ffh::acc::SettingDataAccessor s(m_prj2, "text");
	s.set("ShowActualText", ShowActualText);
	return true;
}

//TODO - how to handle this?
bool TextEditorSettings::ReadDteSetting(CString key, bool defaultvalue)
{
	return ReadIniBool(m_proj.EditorSettingsPath, m_sectionname, "DTE" + key, defaultvalue);
}

//TODO - how to handle this?
void TextEditorSettings::WriteDteSetting(CString key, bool value)
{
	WriteIniBool(m_proj.EditorSettingsPath, m_sectionname, "DTE" + key, value);
}