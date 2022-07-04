#if !defined(AFX_TINT_H__C369665D_8440_4B76_8A25_E71482A6F152__INCLUDED_)
#define AFX_TINT_H__C369665D_8440_4B76_8A25_E71482A6F152__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tint.h : header file
//

#include "resource_subeditors.h"
#include <FFBaseDlg.h>

/////////////////////////////////////////////////////////////////////////////
// CTint dialog

class CTint : public CFFBaseDlg
{
// Construction
public:
	CTint(CWnd* pParent = nullptr);   // standard constructor
	int tintvalue;
	void UpdateTint(int);

// Dialog Data
	enum { IDD = IDD_TINT };

	int		m_tintvariant;

protected:
	CButton	m_tint8;
	CButton	m_tint7;
	CButton	m_tint6;
	CButton	m_tint5;
	CButton	m_tint4;
	CButton	m_tint3;
	CButton	m_tint2;
	CButton	m_tint1;
	CButton	m_tint0;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
	// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTint0();
	afx_msg void OnTint1();
	afx_msg void OnTint2();
	afx_msg void OnTint3();
	afx_msg void OnTint4();
	afx_msg void OnTint5();
	afx_msg void OnTint6();
	afx_msg void OnTint7();
	afx_msg void OnTint8();
};

#endif // !defined(AFX_TINT_H__C369665D_8440_4B76_8A25_E71482A6F152__INCLUDED_)
