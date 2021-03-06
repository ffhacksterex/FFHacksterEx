// CTextEditorSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TextEditorSettingsDlg.h"
#include "TextEditorSettings.h"
#include "FFHacksterProject.h"
#include "afxdialogex.h"
#include "type_support.h"
#include "ui_helpers.h"

using namespace Types;
using namespace Ui;


// CTextEditorSettingsDlg dialog

IMPLEMENT_DYNAMIC(CTextEditorSettingsDlg, CFFBaseDlg)

CTextEditorSettingsDlg::CTextEditorSettingsDlg(CWnd* pParent /*=nullptr*/)
	: CFFBaseDlg(IDD_TEXT_SETTINGS, pParent)
{
}

CTextEditorSettingsDlg::~CTextEditorSettingsDlg()
{
}

void CTextEditorSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEXTSTGS_INGAME, m_showactualtextcheck);
}


BEGIN_MESSAGE_MAP(CTextEditorSettingsDlg, CFFBaseDlg)
END_MESSAGE_MAP()


// CTextEditorSettingsDlg message handlers


BOOL CTextEditorSettingsDlg::OnInitDialog()
{
	CFFBaseDlg::OnInitDialog();

	if (Project == nullptr) {
		EndDialog(IDABORT);
		return TRUE;
	}

	TextEditorSettings stgs(*Project);
	SetCheckValue(m_showactualtextcheck, stgs.ShowActualText);
	//SetEditValue(m_shorttextlengthedit, dec(stgs.ShortTextLength)); //REFACTOR - not sure about adding this...

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CTextEditorSettingsDlg::OnOK()
{
	TextEditorSettings stgs(*Project);
	stgs.ShowActualText = GetCheckValue(m_showactualtextcheck);
	//stgs.ShortTextLength = dec(GetEditValue(m_shorttextlengthedit)); //REFACTOR - not sure about adding this...
	if (stgs.Write())
		CFFBaseDlg::OnOK();
	else
		AfxMessageBox("Unable to write the editor settings due to an unexpected error.", MB_ICONERROR);
}