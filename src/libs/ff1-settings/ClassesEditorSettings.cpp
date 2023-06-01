#include "stdafx.h"
#include "ClassesEditorSettings.h"
#include <FFH2Project.h>
#include "ini_functions.h"
#include <string_conversions.hpp>

using namespace Ini;

#define SECT_NAME "classes"

#define IgnoreHoldMP1_name "IgnoreHoldMP1"
#define IgnoreHoldMP1_default false
#define IgnoreHoldMP2_name "IgnoreHoldMP2"
#define IgnoreHoldMP2_default false
#define IgnoreCapMP1_name "IgnoreCapMP1"
#define IgnoreCapMP1_default false
#define IgnoreCapMP2_name "IgnoreCapMP2"
#define IgnoreCapMP2_default false
#define IgnoreMPRange_name "IgnoreMPRange"
#define IgnoreMPRange_default false
#define IgnoreBBMA1_name "IgnoreBBMA1"
#define IgnoreBBMA1_default false
#define IgnoreBBMA2_name "IgnoreBBMA2"
#define IgnoreBBMA2_default false


CClassesEditorSettings::CClassesEditorSettings(CFFHacksterProject& proj, initflag flag)
	: CClassesEditorSettings(proj, SECT_NAME, flag)
{
}

namespace {
	FFH2Project dummy;
}
CClassesEditorSettings::CClassesEditorSettings(CFFHacksterProject& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
	, s(dummy, ffh::str::tostd(sectionname))
{
}

CClassesEditorSettings::CClassesEditorSettings(FFH2Project& proj, initflag flag)
	: CClassesEditorSettings(proj, SECT_NAME, flag)
{
}

CClassesEditorSettings::CClassesEditorSettings(FFH2Project& proj, CString sectionname, initflag flag)
	: SettingsBase(proj, sectionname)
	, s(proj, ffh::str::tostd(sectionname))
{
	s.EnsureSetting(IgnoreHoldMP1_name, "bool", ppstr(IgnoreHoldMP1_default), "a");
	s.EnsureSetting(IgnoreHoldMP2_name, "bool", ppstr(IgnoreHoldMP2_default), "a");
	s.EnsureSetting(IgnoreCapMP1_name, "bool", ppstr(IgnoreCapMP1_default), "a");
	s.EnsureSetting(IgnoreCapMP2_name, "bool", ppstr(IgnoreCapMP2_default), "a");
	s.EnsureSetting(IgnoreMPRange_name, "bool", ppstr(IgnoreMPRange_default), "a");
	s.EnsureSetting(IgnoreBBMA1_name, "bool", ppstr(IgnoreBBMA1_default), "a");
	s.EnsureSetting(IgnoreBBMA2_name, "bool", ppstr(IgnoreBBMA2_default), "a");

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
	IgnoreHoldMP1 = s.get<bool>(IgnoreHoldMP1_name);
	IgnoreHoldMP2 = s.get<bool>(IgnoreHoldMP2_name);
	IgnoreCapMP1 = s.get<bool>(IgnoreCapMP1_name);
	IgnoreCapMP2 = s.get<bool>(IgnoreCapMP2_name);
	IgnoreMPRange = s.get<bool>(IgnoreMPRange_name);
	IgnoreBBMA1 = s.get<bool>(IgnoreBBMA1_name);
	IgnoreBBMA2 = s.get<bool>(IgnoreBBMA2_name);
	return true;
}

bool CClassesEditorSettings::Write()
{
	s.set<bool>(IgnoreHoldMP1_name, IgnoreHoldMP1);
	s.set<bool>(IgnoreHoldMP2_name, IgnoreHoldMP2);
	s.set<bool>(IgnoreCapMP1_name, IgnoreCapMP1);
	s.set<bool>(IgnoreCapMP2_name, IgnoreCapMP2);
	s.set<bool>(IgnoreMPRange_name, IgnoreMPRange);
	s.set<bool>(IgnoreBBMA1_name, IgnoreBBMA1);
	s.set<bool>(IgnoreBBMA2_name, IgnoreBBMA2);
	return true;
}