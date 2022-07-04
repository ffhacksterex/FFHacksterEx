#include "stdafx.h"
#include "TextEditorSettings.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "type_support.h"

using namespace Ini;
using namespace Types;

#define SECT_TEXT "TEXT"
#define TEXT_SHOWACTUALTEXT "ShowActualText"
#define TEXT_SHOWACTUALTEXT_DEFAULT true

TextEditorSettings::TextEditorSettings(CFFHacksterProject & proj, initflag flag)
	: m_proj(&proj)
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
	ShowActualText = TEXT_SHOWACTUALTEXT_DEFAULT;
	//ShortTextLength = TEXT_SHORTTEXTLENGTH_DEFAULT; //REFACTOR - not sure about adding a length yet...
}

bool TextEditorSettings::Read()
{
	ShowActualText = ReadIniBool(m_proj->EditorSettingsPath, SECT_TEXT, TEXT_SHOWACTUALTEXT, TEXT_SHOWACTUALTEXT_DEFAULT);
	//ShortTextLength = dec(ReadIni(m_proj->EditorSettingsPath, SECT_TEXT, TEXT_SHORTTEXTLENGTH, dec(TEXT_SHORTTEXTLENGTH_DEFAULT)));
	return true;
}

bool TextEditorSettings::Write()
{
	WriteIniBool(m_proj->EditorSettingsPath, SECT_TEXT, TEXT_SHOWACTUALTEXT, ShowActualText);
	//WriteIni(m_proj->EditorSettingsPath, SECT_TEXT, TEXT_SHORTTEXTLENGTH, dec(ShortTextLength));
	return true;
}

bool TextEditorSettings::ReadDteSetting(CString key, bool defaultvalue)
{
	return ReadIniBool(m_proj->EditorSettingsPath, SECT_TEXT, "DTE" + key, defaultvalue);
}

void TextEditorSettings::WriteDteSetting(CString key, bool value)
{
	WriteIniBool(m_proj->EditorSettingsPath, SECT_TEXT, "DTE" + key, value);
}