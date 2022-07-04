#pragma once

#include "resource_settings.h"
#include <FFBaseDlg.h>
class CFFHacksterProject;

// CBattleEditorSettingsDlg dialog

class CBattleEditorSettingsDlg : public CFFBaseDlg
{
	DECLARE_DYNAMIC(CBattleEditorSettingsDlg)

public:
	CBattleEditorSettingsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CBattleEditorSettingsDlg();

	CFFHacksterProject* Project;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BATTLE_SETTINGS };
#endif

protected:
	CClearButton m_viewusage;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnOK();
};
