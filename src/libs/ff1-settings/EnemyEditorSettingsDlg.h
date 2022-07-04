#pragma once

#include "resource_settings.h"
#include <FFBaseDlg.h>
class CFFHacksterProject;

// CEnemyEditorSettingsDlg dialog

class CEnemyEditorSettingsDlg : public CFFBaseDlg
{
	DECLARE_DYNAMIC(CEnemyEditorSettingsDlg)

public:
	CEnemyEditorSettingsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CEnemyEditorSettingsDlg();

	CFFHacksterProject * Project;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ENEMY_SETTINGS };
#endif

protected:
	CClearButton m_checkEnemyByte15hex;
	CEdit m_editByte15name;
	CClearStatic m_enemybyte15static;
	CClearButton m_viewusage;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
