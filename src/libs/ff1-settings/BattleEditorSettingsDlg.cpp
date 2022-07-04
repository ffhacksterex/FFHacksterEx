// BattleEditorSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BattleEditorSettingsDlg.h"
#include "afxdialogex.h"
#include "FFHacksterProject.h"
#include "BattleEditorSettings.h"
#include <ui_helpers.h>

using namespace Ui;

// CBattleEditorSettingsDlg dialog

IMPLEMENT_DYNAMIC(CBattleEditorSettingsDlg, CFFBaseDlg)

CBattleEditorSettingsDlg::CBattleEditorSettingsDlg(CWnd* pParent /*=nullptr*/)
	: CFFBaseDlg(IDD_BATTLE_SETTINGS, pParent)
{
}

CBattleEditorSettingsDlg::~CBattleEditorSettingsDlg()
{
}

void CBattleEditorSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BATTLESTGS_CHECK_VIEWUSAGE, m_viewusage);
}


BEGIN_MESSAGE_MAP(CBattleEditorSettingsDlg, CFFBaseDlg)
	ON_BN_CLICKED(IDOK, &CBattleEditorSettingsDlg::OnOK)
END_MESSAGE_MAP()


// CBattleEditorSettingsDlg message handlers


BOOL CBattleEditorSettingsDlg::OnInitDialog()
{
	CFFBaseDlg::OnInitDialog();

	if (Project == nullptr) {
		EndDialog(IDABORT);
		return TRUE;
	}

	CBattleEditorSettings stgs(*Project);
	SetCheckValue(m_viewusage, stgs.ViewUsageData);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CBattleEditorSettingsDlg::OnOK()
{
	CBattleEditorSettings stgs(*Project);
	stgs.ViewUsageData = GetCheckValue(m_viewusage);
	if (stgs.Write())
		CFFBaseDlg::OnOK();
	else
		AfxMessageBox("Unable to write the editor settings due to an unexpected error.", MB_ICONERROR);
}
