// EnemyEditorSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EnemyEditorSettingsDlg.h"
#include "afxdialogex.h"
#include "FFHacksterProject.h"
#include "EnemyEditorSettings.h"
#include "ui_helpers.h"

using namespace Ui;

// CEnemyEditorSettingsDlg dialog

IMPLEMENT_DYNAMIC(CEnemyEditorSettingsDlg, CFFBaseDlg)

CEnemyEditorSettingsDlg::CEnemyEditorSettingsDlg(CWnd* pParent /*= nullptr */)
	: CFFBaseDlg(IDD_ENEMY_SETTINGS, pParent)
{
}

CEnemyEditorSettingsDlg::~CEnemyEditorSettingsDlg()
{
}

void CEnemyEditorSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROJSET_CHECK_ENEMYBYTE15, m_checkEnemyByte15hex);
	DDX_Control(pDX, IDC_PROJSET_STATIC_BYTE15VAL, m_enemybyte15static);
	DDX_Control(pDX, IDC_PROJSET_EDIT_BYTE15VAL, m_editByte15name);
	DDX_Control(pDX, IDC_ENEMSET_CHK_VIEWUSAGE, m_viewusage);
}


BEGIN_MESSAGE_MAP(CEnemyEditorSettingsDlg, CFFBaseDlg)
END_MESSAGE_MAP()


// CEnemyEditorSettingsDlg message handlers


BOOL CEnemyEditorSettingsDlg::OnInitDialog()
{
	CFFBaseDlg::OnInitDialog();

	if (Project == nullptr) {
		EndDialog(IDABORT);
		return TRUE;
	}

	CEnemyEditorSettings stgs(*Project);
	SetCheckValue(m_checkEnemyByte15hex, stgs.Byte15AsFlags);
	m_editByte15name.SetWindowText(stgs.Byte15Name);
	SetCheckValue(m_viewusage, stgs.ViewUsageData);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CEnemyEditorSettingsDlg::OnOK()
{
	CString enemiesByte15Name;
	m_editByte15name.GetWindowText(enemiesByte15Name);
	if (enemiesByte15Name.IsEmpty()) {
		AfxMessageBox("Enemy byte 15 must have an in-editor name.", MB_ICONERROR);
		m_editByte15name.SetSel(0, -1);
		m_editByte15name.SetFocus();
		return;
	}

	CEnemyEditorSettings stgs(*Project);
	stgs.Byte15AsFlags = GetCheckValue(m_checkEnemyByte15hex);
	stgs.Byte15Name = enemiesByte15Name;
	stgs.ViewUsageData = GetCheckValue(m_viewusage);
	if (stgs.Write())
		CFFBaseDlg::OnOK();
	else
		AfxMessageBox("Unable to write the editor settings due to an unexpected error.", MB_ICONERROR);
}