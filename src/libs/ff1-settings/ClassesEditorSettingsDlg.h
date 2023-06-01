#pragma once

#include <functional>
#include "afxwin.h"
#include "resource_settings.h"
#include <FFBaseDlg.h>
class CFFHacksterProject;
class FFH2Project;

class CClassesEditorSettingsDlg : public CFFBaseDlg
{
	DECLARE_DYNAMIC(CClassesEditorSettingsDlg)

public:
	CClassesEditorSettingsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CClassesEditorSettingsDlg();

	FFH2Project* Proj2 = nullptr;
	CFFHacksterProject * m_proj;
	CString PreSaveOKCancelPrompt;

// Dialog Data
	enum { IDD = IDD_CLASSES_SETTINGS };

protected:
	CClearButton m_checkIgnoreHoldMp1;
	CClearButton m_checkIgnoreHoldMp2;
	CClearButton m_checkIgnoreCapMp1;
	CClearButton m_checkIgnoreCapMp2;
	CClearButton m_checkIgnoreMPRange;
	CButton m_checkIgnoreBBMA1;
	CButton m_checkIgnoreBBMA2;
	CButton m_okbutton;
	CButton m_cancelbutton;

	virtual bool Load();
	virtual bool Save();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
