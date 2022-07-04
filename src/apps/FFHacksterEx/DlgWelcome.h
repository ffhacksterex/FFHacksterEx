#pragma once

#include "resource.h"
#include "afxwin.h"
#include "DialogWithBackground.h"
#include "afxmenubutton.h"

// CDlgWelcome dialog

class CDlgWelcome : public CDialogWithBackground
{
	DECLARE_DYNAMIC(CDlgWelcome)

public:
	CDlgWelcome(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgWelcome();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WELCOME };
#endif
	CString Action;
	CString ActionParam;

protected:
	CFont m_bannerfont;
	CFont m_actionbuttonfont;

	std::vector<CString> m_mrupaths;
	CMenu m_mrupopup;

	virtual void PaintClient(CDC& dc);

	void OnSelectAction();

	void LoadMru();
	void SaveMru();
	void InvokeMruMenu();

	CClearButton m_openprojectbutton = { IDPNG_OPENBLACKFOLDER, "PNG" };
	CClearButton m_editrombutton = { IDPNG_ADDFILESM, "PNG" };
	CClearButton m_newrombutton = { IDPNG_FOLDERBLACK, "PNG" };
	CClearButton m_newasmbutton = { IDPNG_FOLDERWHITE, "PNG" };
	CClearStatic m_editromstatic;
	CClearStatic m_newromstatic;
	CClearStatic m_newasmstatic;
	CClearStatic m_openprojectstatic;
	CClearStatic m_subbannerstatic;
	CClearButton m_unzipbutton = { IDPNG_UNZIP24, "PNG" };
	CClearStatic m_unzipstatic;
	CClearButton m_aboutbutton = { IDPNG_HELPSM, "PNG" };
	CClearButton m_appsettingsbutton = { IDPNG_GEARSM, "PNG" };
	CHelpbookButton m_helpbookbutton;
	CCloseButton m_cancelbutton;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedProjectsBtnNewrom();
	afx_msg void OnBnClickedProjectsBtnNewasm();
	afx_msg void OnBnClickedProjectsBtnOpen();
	afx_msg void OnBnClickedProjectsBtnEditrom();
	afx_msg void OnBnClickedProjectsBtnUnzip();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnBnClickedFfhBtnAppsettings();
	afx_msg void OnBnClickedAbout();
};
