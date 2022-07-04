#pragma once

#include <afxwin.h>
#include <afxdialogex.h>
#include "resource_utils.h"
#include <functional>

using InplaceComboFillFunc = bool(CComboBox & box);
using InplaceComboOKFunc = bool(int index, DWORD_PTR itemdata);

// CInplaceCombo dialog

class CInplaceCombo : public CDialogEx
{
	DECLARE_DYNAMIC(CInplaceCombo)

public:
	CInplaceCombo(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CInplaceCombo();

	void SetPosition(CRect & rc);
	void SetDropWidth(int width);
	void ClearData();

	std::function<InplaceComboOKFunc> OKFunc = nullptr;
	CComboBox m_combo;

// Dialog Data
	enum { IDD = IDD_INPLACE_COMBO };

protected:
	bool m_bOked;
	bool m_bCancelled;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillfocusCombo();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};
