#pragma once

class AppSettings;

// CDlgPromptProjectBackup dialog
// Prompts to either skip making a backup or selecting a backup folder location.
// The filename is auto-generated based on project filename and current local time.

class CDlgPromptProjectBackup : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPromptProjectBackup)

public:
	CDlgPromptProjectBackup(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgPromptProjectBackup();

	AppSettings* AppStgs = nullptr;
	CString ProjectFilePath;
	CString Filter;
	CString StartInFolder;
	CString BackupFilePath;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJECTUPGRADE };
#endif

protected:
	enum BackupMode { None, Temp, Browse };;
	BackupMode m_mode = None;

	void EnableRadioBrowseControls(bool enable);

	CStatic m_statictempfilename;
	CStatic m_staticbrowsefilename;
	CButton m_buttonbrowsefilename;
	CEdit m_editbrowsefilename;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBtnClickedProjUpgradeRadioNoUpgrade();
	afx_msg void OnBnClickedProjupgradeRadupgradetemp();
	afx_msg void OnBnClickedProjupgradeRadupgradebrowse();
	afx_msg void OnBnClickedProjupgradeBtnbrowse();
	CButton m_radionoupgrade;
};
