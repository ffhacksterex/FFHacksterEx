#if !defined(AFX_LOADING_H__CF045029_96E6_4767_A0D4_E1D5E4948C26__INCLUDED_)
#define AFX_LOADING_H__CF045029_96E6_4767_A0D4_E1D5E4948C26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Loading.h : header file
//

#include "resource_subeditors.h"
#include "afxwin.h"
#include "IProgress.h"

/////////////////////////////////////////////////////////////////////////////
// CLoading dialog

class CLoading : public CDialog, public IProgress
{
// Construction
public:
	CLoading(CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	enum { IDD = IDD_LOADING };
	CProgressCtrl	m_progress;

	void SetMessage(CString message);
	void Init(int min, int max, int step, CWnd* parent = nullptr);

protected:
// Overrides
	virtual void DoAddSteps(int numtoadd) override;
	virtual void DoStep() override;
	virtual void DoSetProgressText(LPCSTR text) override;
	virtual void DoSetStatusText(LPCSTR text) override;

	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
	// Generated message map functions
	DECLARE_MESSAGE_MAP()
	CStatic m_messagestatic;
};

#endif // !defined(AFX_LOADING_H__CF045029_96E6_4767_A0D4_E1D5E4948C26__INCLUDED_)
