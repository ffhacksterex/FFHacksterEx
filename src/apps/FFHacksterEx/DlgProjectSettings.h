#pragma once

#include "resource.h"
#include <DialogWithBackground.h>
class CFFHacksterProject;

// CDlgProjectSettings dialog

class CDlgProjectSettings : public CDialogWithBackground
{
	DECLARE_DYNAMIC(CDlgProjectSettings)

public:
	CDlgProjectSettings(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgProjectSettings();

	CFFHacksterProject * m_proj;
	bool ShowBackgroundArt;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJECTSETTINGS };
#endif

protected:
	void HideAssemblySpecificControls();

	CClearStatic m_statProjtype;
	CClearEdit m_editRevertPath;
	CClearEdit m_editPublishPath;
	CCloseButton m_closebutton;
	CClearButton m_setbutton;
	CClearButton m_publishbutton;
	CClearButton m_clearbutton;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton;
	CClearStatic m_static1;
	CClearStatic m_static2;
	CClearEdit m_addlmodfolderedit;
	CClearStatic m_addlmodfolderstatic;
	CClearButton m_addlmodfolderbutton;
	CClearButton m_asmdllbrowsebutton;
	CClearEdit m_asmdlledit;
	CClearStatic m_asmdllstatic;
	CButton m_suppressasmwarningcheck;
	CClearButton m_coerceasmbutton;
	CHelpbookButton m_helpbookbutton;

	CFont m_projfonttype;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedProjsetButtonRevertpath();
	afx_msg void OnBnClickedProjsetButtonPublishpath();
	afx_msg void OnBnClickedProjsetButtonRevertclear();
	afx_msg void OnBnClickedAppsettingsBtnAddlmodfolder();
	afx_msg void OnBnClickedProjectAssemblyDllPath();
	afx_msg void OnBnClickedProjstgsButtonCoerceasm();
};
