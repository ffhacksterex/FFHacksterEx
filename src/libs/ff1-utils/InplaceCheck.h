#pragma once

#include "afxwin.h"
#include "resource_utils.h"
#include "function_types.h"

// CInplaceCheck dialog

class CInplaceCheck : public CDialogEx
{
	DECLARE_DYNAMIC(CInplaceCheck)

public:
	CInplaceCheck(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CInplaceCheck();

	void InitEdit(CRect rcitem, CString text, bool checked);
	void ClearData();
	void SetPosition(CRect rcitem);
	bool IsChecked() const;

	std::function<InplaceCheckOKFunc> OKFunc = nullptr;
	CButton m_check;

// Dialog Data
	enum { IDD = IDD_INLINEBOOL };

protected:
	bool m_bOked;
	bool m_bCancelled;
	bool m_bNoName;
	bool m_bChecked;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillfocusInlineboolCheck();
	afx_msg void OnClickedInlineboolCheck();
};
