#pragma once

#include "resource_settings.h"
#include <FFBaseDlg.h>
class CFFHacksterProject;

// CMagicEditorSettingsDlg dialog

class CMagicEditorSettingsDlg : public CFFBaseDlg
{
	DECLARE_DYNAMIC(CMagicEditorSettingsDlg)

public:
	CMagicEditorSettingsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMagicEditorSettingsDlg();

	CFFHacksterProject * m_proj;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAGIC_SETTINGS };
#endif

protected:
	CEdit m_editMagicByte7;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
