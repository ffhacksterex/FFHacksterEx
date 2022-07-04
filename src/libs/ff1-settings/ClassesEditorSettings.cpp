#include "stdafx.h"
#include "ClassesEditorSettings.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"

using namespace Ini;

#define SECT_CLASSES "Classes"
#define IGNOREHOLDMP1 "IgnoreHoldMP1"
#define IGNOREHOLDMP1_DEFAULT false
#define IGNOREHOLDMP2 "IgnoreHoldMP2"
#define IGNOREHOLDMP2_DEFAULT false
#define IGNORECAPMP1 "IgnoreCapMP1"
#define IGNORECAPMP1_DEFAULT false
#define IGNORECAPMP2 "IgnoreCapMP2"
#define IGNORECAPMP2_DEFAULT false
#define IGNOREMPRANGE "IgnoreMPRange"
#define IGNOREMPRANGE_DEFAULT false
#define IGNOREBBMA1 "IgnoreBBMA1"
#define IGNOREBBMA1_DEFAULT false
#define IGNOREBBMA2 "IgnoreBBMA2"
#define IGNOREBBMA2_DEFAULT false


CClassesEditorSettings::CClassesEditorSettings(CFFHacksterProject & proj, initflag flag)
	: m_proj(&proj)
{
	if (flag == initflag::read)
		Read();
	else
		SetDefaults();
}

void CClassesEditorSettings::SetDefaults()
{
	IgnoreHoldMP1 = IGNOREHOLDMP1_DEFAULT;
	IgnoreHoldMP2 = IGNOREHOLDMP2_DEFAULT;
	IgnoreCapMP1 = IGNORECAPMP1_DEFAULT;
	IgnoreCapMP2 = IGNORECAPMP2_DEFAULT;
	IgnoreMPRange = IGNOREMPRANGE_DEFAULT;
	IgnoreBBMA1 = IGNOREBBMA1_DEFAULT;
	IgnoreBBMA2 = IGNOREBBMA2_DEFAULT;
}

bool CClassesEditorSettings::Read()
{
	IgnoreHoldMP1 = ReadIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNOREHOLDMP1, IGNOREHOLDMP1_DEFAULT);
	IgnoreHoldMP2 = ReadIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNOREHOLDMP2, IGNOREHOLDMP2_DEFAULT);
	IgnoreCapMP1 = ReadIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNORECAPMP1, IGNORECAPMP1_DEFAULT);
	IgnoreCapMP2 = ReadIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNORECAPMP2, IGNORECAPMP2_DEFAULT);
	IgnoreMPRange = ReadIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNOREMPRANGE, IGNOREMPRANGE_DEFAULT);
	IgnoreBBMA1 = ReadIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNOREBBMA1, IGNOREBBMA1_DEFAULT);
	IgnoreBBMA2 = ReadIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNOREBBMA2, IGNOREBBMA2_DEFAULT);
	return true;
}

bool CClassesEditorSettings::Write()
{
	WriteIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNOREHOLDMP1, IgnoreHoldMP1);
	WriteIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNOREHOLDMP2, IgnoreHoldMP2);
	WriteIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNORECAPMP1, IgnoreCapMP1);
	WriteIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNORECAPMP2, IgnoreCapMP2);
	WriteIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNOREMPRANGE, IgnoreMPRange);
	WriteIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNOREBBMA1, IgnoreBBMA1);
	WriteIniBool(m_proj->EditorSettingsPath, SECT_CLASSES, IGNOREBBMA2, IgnoreBBMA2);
	return true;
}