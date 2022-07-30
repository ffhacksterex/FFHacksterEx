#include "stdafx.h"
#include "ClassesEditorSettings.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"

using namespace Ini;

#define SECT_NAME "Classes"

#define IgnoreHoldMP1_default false
#define IgnoreHoldMP2_default false
#define IgnoreCapMP1_default false
#define IgnoreCapMP2_default false
#define IgnoreMPRange_default false
#define IgnoreBBMA1_default false
#define IgnoreBBMA2_default false


CClassesEditorSettings::CClassesEditorSettings(CFFHacksterProject& proj, initflag flag)
	: CClassesEditorSettings(proj, SECT_NAME, flag)
{
}

CClassesEditorSettings::CClassesEditorSettings(CFFHacksterProject& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
{
	if (flag == initflag::read)
		Read();
	else
		SetDefaults();
}

void CClassesEditorSettings::SetDefaults()
{
	IgnoreHoldMP1 = IgnoreHoldMP1_default;
	IgnoreHoldMP2 = IgnoreHoldMP2_default;
	IgnoreCapMP1 = IgnoreCapMP1_default;
	IgnoreCapMP2 = IgnoreCapMP2_default;
	IgnoreMPRange = IgnoreMPRange_default;
	IgnoreBBMA1 = IgnoreBBMA1_default;
	IgnoreBBMA2 = IgnoreBBMA2_default;
}

bool CClassesEditorSettings::Read()
{
	READ_SETTING_BOOL(IgnoreHoldMP1);
	READ_SETTING_BOOL(IgnoreHoldMP2);
	READ_SETTING_BOOL(IgnoreCapMP1);
	READ_SETTING_BOOL(IgnoreCapMP2);
	READ_SETTING_BOOL(IgnoreMPRange);
	READ_SETTING_BOOL(IgnoreBBMA1);
	READ_SETTING_BOOL(IgnoreBBMA2);
	return true;
}

bool CClassesEditorSettings::Write()
{
	WRITE_SETTING_BOOL(IgnoreHoldMP1);
	WRITE_SETTING_BOOL(IgnoreHoldMP2);
	WRITE_SETTING_BOOL(IgnoreCapMP1);
	WRITE_SETTING_BOOL(IgnoreCapMP2);
	WRITE_SETTING_BOOL(IgnoreMPRange);
	WRITE_SETTING_BOOL(IgnoreBBMA1);
	WRITE_SETTING_BOOL(IgnoreBBMA2);
	return true;
}