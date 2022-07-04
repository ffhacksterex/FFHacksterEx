#pragma once

#include <afxdialogex.h>
#include "resource_utils.h"
#include "function_types.h"

// CInplaceEdit dialog

class CInplaceEdit : public CDialogEx
{
	DECLARE_DYNAMIC(CInplaceEdit)

public:
	CInplaceEdit(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CInplaceEdit();

	void InitEdit(CRect rcitem, CString text);
	void ClearData();
	void SetPosition(CRect rcitem);
	void SetText(CString text, bool selectall = false);
	CString GetText();
	void Cancel();

	std::function<InplaceEditOKFunc> OKFunc = nullptr;
	CEdit m_edit;

// Dialog Data
	enum { IDD = IDD_INPLACE_EDIT };

protected:
	bool m_bOkInvoked;  // OK has been called either directly or indirectly (eg. Killfocus)
	bool m_bCancelled;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillfocusInplaceEditbox();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};
