#pragma once


// CDlgPickDestinationFile dialog

class CDlgPickDestinationFile : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPickDestinationFile)
public:
	CDlgPickDestinationFile(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgPickDestinationFile();

	CString Title;
	CString Blurb;
	CString StartInFolder;
	CString Filter;
	CString DefaultExt;
	CString DestFile;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PICK_DESTINATION_FILE };
#endif

protected:
	CEdit m_editParentFolder;
	CButton m_browsebutton;
	CStatic m_blurb;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBrowseParentfolder();
};
