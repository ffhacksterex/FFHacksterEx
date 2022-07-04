// ClassesEditorSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClassesEditorSettingsDlg.h"
#include "FFHacksterProject.h"
#include "ClassesEditorSettings.h"
#include "ui_helpers.h"

using namespace Ui;


// CClassesEditorSettingsDlg dialog

IMPLEMENT_DYNAMIC(CClassesEditorSettingsDlg, CFFBaseDlg)

CClassesEditorSettingsDlg::CClassesEditorSettingsDlg(CWnd* pParent /*= nullptr */)
	: CFFBaseDlg(IDD, pParent)
	, m_proj(nullptr)
{
}

CClassesEditorSettingsDlg::~CClassesEditorSettingsDlg()
{
}

bool CClassesEditorSettingsDlg::Load()
{
	CClassesEditorSettings stgs(*m_proj, initflag::dontread);
	bool read = stgs.Read();
	SetCheckValue(m_checkIgnoreHoldMp1, stgs.IgnoreHoldMP1);
	SetCheckValue(m_checkIgnoreHoldMp2, stgs.IgnoreHoldMP2);
	SetCheckValue(m_checkIgnoreCapMp1, stgs.IgnoreCapMP1);
	SetCheckValue(m_checkIgnoreCapMp2, stgs.IgnoreCapMP2);
	SetCheckValue(m_checkIgnoreMPRange, stgs.IgnoreMPRange);
	SetCheckValue(m_checkIgnoreBBMA1, stgs.IgnoreBBMA1);
	SetCheckValue(m_checkIgnoreBBMA2, stgs.IgnoreBBMA2);
	return read;
}

bool CClassesEditorSettingsDlg::Save()
{
	CClassesEditorSettings stgs(*m_proj);
	stgs.IgnoreHoldMP1 = GetCheckValue(m_checkIgnoreHoldMp1);
	stgs.IgnoreHoldMP2 = GetCheckValue(m_checkIgnoreHoldMp2);
	stgs.IgnoreCapMP1 = GetCheckValue(m_checkIgnoreCapMp1);
	stgs.IgnoreCapMP2 = GetCheckValue(m_checkIgnoreCapMp2);
	stgs.IgnoreMPRange = GetCheckValue(m_checkIgnoreMPRange);
	stgs.IgnoreBBMA1 = GetCheckValue(m_checkIgnoreBBMA1);
	stgs.IgnoreBBMA2 = GetCheckValue(m_checkIgnoreBBMA2);
	return stgs.Write();
}

void CClassesEditorSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROJSET_CHECK_NOHOLDMP1, m_checkIgnoreHoldMp1);
	DDX_Control(pDX, IDC_PROJSET_CHECK_NOHOLDMP2, m_checkIgnoreHoldMp2);
	DDX_Control(pDX, IDC_PROJSET_CHECK_NOCAPMP1, m_checkIgnoreCapMp1);
	DDX_Control(pDX, IDC_PROJSET_CHECK_NOCAPMP2, m_checkIgnoreCapMp2);
	DDX_Control(pDX, IDC_PROJSET_CHECK_NOMPRANGE, m_checkIgnoreMPRange);
	DDX_Control(pDX, IDC_CHECK1, m_checkIgnoreBBMA1);
	DDX_Control(pDX, IDC_CHECK2, m_checkIgnoreBBMA2);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
}


BEGIN_MESSAGE_MAP(CClassesEditorSettingsDlg, CFFBaseDlg)
END_MESSAGE_MAP()


BOOL CClassesEditorSettingsDlg::OnInitDialog()
{
	CFFBaseDlg::OnInitDialog();

	if (m_proj == nullptr) {
		EndDialog(IDABORT);
		return TRUE;
	}

	VERIFY(Load());

	return TRUE;
}

void CClassesEditorSettingsDlg::OnOK()
{
	if (!PreSaveOKCancelPrompt.IsEmpty()) {
		auto result = AfxMessageBox(PreSaveOKCancelPrompt, MB_OKCANCEL | MB_ICONQUESTION);
		if (result != IDOK) return;
	}

	if (Save())
		CFFBaseDlg::OnOK();
	else
		AfxMessageBox("Unable to write the editor settings due to an unexpected error.", MB_ICONERROR);
}