#pragma once


// CDlgPickNamedDestination dialog

class CDlgPickNamedDestination : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPickNamedDestination)

public:
	CDlgPickNamedDestination(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgPickNamedDestination();

	bool PromptImmediately = false;
	CString Title;
	CString Blurb;
	CString StartInFolder;
	CString SourceFolderName;
	CString DestParentFolder;
	CString DestFolderName;
	CString DestFolderPath;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PICK_NAMED_DESTINATION };
#endif

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

protected:
	UINT m_timer;

	CEdit m_editParentFolder;
	CButton m_browsebutton;
	CEdit m_editName;
	CStatic m_blurb;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBrowseParentfolder();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
