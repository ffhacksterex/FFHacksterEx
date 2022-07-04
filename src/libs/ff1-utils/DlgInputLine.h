#pragma once

#include "resource_utils.h"

// CDlgInputLine dialog

class CDlgInputLine : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgInputLine)

public:
	CDlgInputLine(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgInputLine();

	CString Label;
	CString Value;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUTLINE };
#endif

protected:
	CEdit m_inlineedit;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CStatic m_label;
};
