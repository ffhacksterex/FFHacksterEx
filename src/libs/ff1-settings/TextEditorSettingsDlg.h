#pragma once

#include "afxwin.h"
#include "resource_settings.h"
#include <FFBaseDlg.h>
class CFFHacksterProject;

// CTextEditorSettingsDlg dialog

class CTextEditorSettingsDlg : public CFFBaseDlg
{
	DECLARE_DYNAMIC(CTextEditorSettingsDlg)

public:
	CTextEditorSettingsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTextEditorSettingsDlg();

	CFFHacksterProject * Project = nullptr;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEXT_SETTINGS };
#endif

protected:
	CStrikeCheck m_showactualtextcheck;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
