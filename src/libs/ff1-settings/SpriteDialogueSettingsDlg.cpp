// SpriteDialogueSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SpriteDialogueSettingsDlg.h"
#include "SpriteDialogueSettings.h"
#include "FFHacksterProject.h"
#include "afxdialogex.h"
#include "type_support.h"
#include "ui_helpers.h"

using namespace Types;
using namespace Ui;

// CSpriteDialogueSettingsDlg dialog

IMPLEMENT_DYNAMIC(CSpriteDialogueSettingsDlg, CFFBaseDlg)

CSpriteDialogueSettingsDlg::CSpriteDialogueSettingsDlg(CWnd* pParent /*= nullptr */)
	: CFFBaseDlg(IDD, pParent)
{
}

CSpriteDialogueSettingsDlg::~CSpriteDialogueSettingsDlg()
{
}

void CSpriteDialogueSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SHOWACTUALTEXT, m_showactualtextcheck);
	DDX_Control(pDX, IDC_EDIT_SHORTTEXTLENGTH, m_shorttextlengthedit);
	DDX_Control(pDX, IDC_STGS_SPRDLG_THROWONBADADDR, m_throwonbadspraddcheck);
}


BEGIN_MESSAGE_MAP(CSpriteDialogueSettingsDlg, CFFBaseDlg)
END_MESSAGE_MAP()


// CSpriteDialogueSettingsDlg message handlers

BOOL CSpriteDialogueSettingsDlg::OnInitDialog()
{
	CFFBaseDlg::OnInitDialog();

	if (m_proj == nullptr) {
		EndDialog(IDABORT);
		return TRUE;
	}

	CSpriteDialogueSettings stgs(*m_proj);
	SetCheckValue(m_showactualtextcheck, stgs.ShowActualText);
	SetEditValue(m_shorttextlengthedit, dec(stgs.ShortTextLength));
	SetCheckValue(m_throwonbadspraddcheck, stgs.ThrowOnBadSpriteAddr);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpriteDialogueSettingsDlg::OnOK()
{
	CSpriteDialogueSettings stgs(*m_proj);
	stgs.ShowActualText = GetCheckValue(m_showactualtextcheck);
	stgs.ShortTextLength = dec(GetEditValue(m_shorttextlengthedit));
	stgs.ThrowOnBadSpriteAddr = GetCheckValue(m_throwonbadspraddcheck);
	if (stgs.Write())
		CFFBaseDlg::OnOK();
	else
		AfxMessageBox("Unable to write the editor settings due to an unexpected error.", MB_ICONERROR);
}