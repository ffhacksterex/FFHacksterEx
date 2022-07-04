#pragma once

#include "afxwin.h"
#include "IProgress.h"
#include "resource_utils.h"

// CWaitingDlg dialog

class CWaitingDlg : public CDialogEx, public IProgress
{
	DECLARE_DYNAMIC(CWaitingDlg)

public:
	CWaitingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWaitingDlg();

	void SetMessage(CString message);
	void Init();

// Dialog Data
	enum { IDD = IDD_WAITING };

protected:
	CStatic m_messagestatic;

	virtual void DoAddSteps(int numtoadd) override;
	virtual void DoStep() override;
	virtual void DoSetProgressText(LPCSTR text) override;
	virtual void DoSetStatusText(LPCSTR text) override;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnFFHacksterExShowWaitMessage(WPARAM wParam, LPARAM lParam);
};
