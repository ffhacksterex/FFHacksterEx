// MagicEditorSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MagicEditorSettingsDlg.h"
#include "afxdialogex.h"
#include "FFHacksterProject.h"
#include "MagicEditorSettings.h"
#include "ui_helpers.h"

using namespace Ui;

// CMagicEditorSettingsDlg dialog

IMPLEMENT_DYNAMIC(CMagicEditorSettingsDlg, CFFBaseDlg)

CMagicEditorSettingsDlg::CMagicEditorSettingsDlg(CWnd* pParent /*= nullptr */)
	: CFFBaseDlg(IDD_MAGIC_SETTINGS, pParent)
{
}

CMagicEditorSettingsDlg::~CMagicEditorSettingsDlg()
{
}

void CMagicEditorSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROJSET_EDIT_MAGICBYTE7, m_editMagicByte7);
}


BEGIN_MESSAGE_MAP(CMagicEditorSettingsDlg, CFFBaseDlg)
END_MESSAGE_MAP()


// CMagicEditorSettingsDlg message handlers


BOOL CMagicEditorSettingsDlg::OnInitDialog()
{
	CFFBaseDlg::OnInitDialog();

	if (m_proj == nullptr) {
		EndDialog(IDABORT);
		return TRUE;
	}

	CMagicEditorSettings stgs(*m_proj);
	m_editMagicByte7.SetWindowText(stgs.Byte7Name);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CMagicEditorSettingsDlg::OnOK()
{
	CString magicByte7Name;
	m_editMagicByte7.GetWindowText(magicByte7Name);
	if (magicByte7Name.IsEmpty()) {
		AfxMessageBox("Magic byte 7 must have an in-editor name.", MB_ICONERROR);
		m_editMagicByte7.SetSel(0, -1);
		m_editMagicByte7.SetFocus();
		return;
	}

	CMagicEditorSettings stgs(*m_proj);
	stgs.Byte7Name = magicByte7Name;
	if (stgs.Write())
		CFFBaseDlg::OnOK();
	else
		AfxMessageBox("Unable to write the editor settings due to an unexpected error.", MB_ICONERROR);
}