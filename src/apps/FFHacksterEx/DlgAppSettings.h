#pragma once

#include "resource.h"
#include "DialogWithBackground.h"
#include <ColorStatic.h>
#include <Help.h>

class AppSettings;
struct EditorRef;

// CDlgAppSettings dialog

class CDlgAppSettings : public CDialogWithBackground
{
	DECLARE_DYNAMIC(CDlgAppSettings)

public:
	CDlgAppSettings(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgAppSettings();

	CHelp* HelpSettings = nullptr;
	bool MruNeedsUpdate() const;

protected:
	void HideHelpOptions();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APPSETTINGS };
#endif

	bool m_bUpdateMru;
	CButton m_checkPromptOnClose;
	CButton m_purgelogcheckbox;
	CButton m_savehexuppercheck;
	CButton m_showbackgroundartcheck;
	CButton m_forcenewlabelscheck;
	CButton m_displaystrikeasnormalcheck;
	CColorStatic m_strikecheckcolorstatic;
	CButton m_alwayscompilecheck;
	CButton m_usefiledialogtobrowsecheck;
	CClearButton m_exebrowsebutton;
	CClearButton m_clearmrubutton;
	CClearButton m_clearinvalidmrubutton;
	CClearButton m_enforceasmcompatcheck;
	CClearButton m_warnasmcompatcheck;
	CClearStatic m_exestatic;
	CClearEdit m_editRunExe;
	CClearEdit m_cmdparamsedit;
	CHelpbookButton m_helpbookbutton;
	CCloseButton m_closebutton;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton;
	CClearButton m_localhelpbutton;
	CClearEdit m_editlocalhelp;
	CClearComboBox m_helptypes;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedAppsettingsBtnRunexe();
	afx_msg void OnBnClickedClearMru();
	afx_msg void OnBnClickedClearInvalidMRU();
	afx_msg void OnBnClickedShowBackgroundArt();
	afx_msg void OnBnClickedColorStrikeCheck();
	afx_msg void OnBnClickedStrikeCheck();
	afx_msg void OnBnClickedEnforceAsmCompatibility();
	afx_msg void OnBnClickedRadioLocalhelp();
	afx_msg void OnBnClickedRadioWebhelp();
	afx_msg void OnBnClickedRadioHelpInAppdir();
	afx_msg void OnBnClickedButtonLocalhelp();
	afx_msg void OnCbnSelchangeComboHelpTypes();
};
