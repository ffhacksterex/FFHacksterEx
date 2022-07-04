#pragma once

#include "afxwin.h"
#include "resource_settings.h"
#include <FFBaseDlg.h>
class CFFHacksterProject;

// CSpriteDialogueSettingsDlg dialog

class CSpriteDialogueSettingsDlg : public CFFBaseDlg
{
	DECLARE_DYNAMIC(CSpriteDialogueSettingsDlg)

public:
	CSpriteDialogueSettingsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSpriteDialogueSettingsDlg();

	CFFHacksterProject * m_proj = nullptr;

// Dialog Data
	enum { IDD = IDD_SPRITE_DIALOGUE_SETTINGS };

protected:
	CButton m_showactualtextcheck;
	CEdit m_shorttextlengthedit;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
