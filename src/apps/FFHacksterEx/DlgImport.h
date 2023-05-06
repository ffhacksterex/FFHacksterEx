#pragma once
#include "afxwin.h"

class CFFHacksterProject;
class FFH2Project;

// CDlgImport dialog

class CDlgImport : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImport)

public:
	CDlgImport(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgImport();

	CFFHacksterProject* Project = nullptr;
	FFH2Project* Proj2 = nullptr;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMPORT };
#endif

protected:
	BOOL IsProjectChecked();

	CButton m_datcheck;
	CButton m_datbutton;
	CClearEdit m_datedit;
	CButton m_valuescheck;
	CButton m_labelscheck;
	CButton m_dialoguecheck;
	CButton m_projectbrowsebtn;
	CClearEdit m_projectedit;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCheckDat();
	afx_msg void OnBnClickedProjectCheckBoxes();
	afx_msg void OnBnClickedButtonDat();
	afx_msg void OnBnClickedButtonProject();
};
