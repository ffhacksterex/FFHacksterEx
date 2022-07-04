#pragma once

#include "InplaceEdit.h"
#include "InplaceCombo.h"
#include "InplaceCheck.h"
#include "resource_subeditors.h"
#include "afxcmn.h"

// CInplaceArrayEdit dialog

class CInplaceArrayEdit : public CDialogEx
{
	DECLARE_DYNAMIC(CInplaceArrayEdit)

public:
	CInplaceArrayEdit(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CInplaceArrayEdit();

	CString Caption;
	CString Type;
	CString ArrayText;
	bool HexUppercase;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPLACEARRAYEDIT };
#endif

protected:
	CListCtrl m_list;
	CInplaceEdit m_inplaceedit;
	CInplaceCombo m_inplacecombo;
	CInplaceCheck m_inplacecheck;

	void Edit(int item);
	void Add(int item);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickAdd();
	afx_msg void OnClickDelete();
	afx_msg void OnClickAppend();
};
