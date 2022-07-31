#pragma once

#include <AppSettings.h>

// CDlgFolderPrefs dialog

class CDlgFolderPrefs : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFolderPrefs)

public:
	CDlgFolderPrefs(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgFolderPrefs();

	sAppFolderPrefs Prefs;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FOLDER_PREFS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
private:
	CMFCPropertyGridCtrl m_propgridfolders;
};
