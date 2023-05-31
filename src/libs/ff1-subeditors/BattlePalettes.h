#if !defined(AFX_BATTLEPALETTES_H__2BCE6B23_AC11_46CE_9D3C_34D3FE3D110E__INCLUDED_)
#define AFX_BATTLEPALETTES_H__2BCE6B23_AC11_46CE_9D3C_34D3FE3D110E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BattlePalettes.h : header file
//

#include <FFBaseDlg.h>
class FFH2Project;
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// CBattlePalettes dialog

class CBattlePalettes : public CFFBaseDlg
{
// Construction
public:
	CBattlePalettes(CWnd* pParent = nullptr);   // standard constructor

	FFH2Project* Proj2 = nullptr;
	CFFHacksterProject* Project;
	int oldpal;
	int newpal;
	CRect drawrect;
	CRect rcOldPal;
	CRect rcNewPal;
	CPoint FindCurPoint();

// Dialog Data
	enum { IDD = IDD_BATTLEPALETTES };

protected:
	CStatic	m_origpal;
	CStatic	m_newpal;

	int BATTLEPALETTE_OFFSET = -1;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
	// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

#endif // !defined(AFX_BATTLEPALETTES_H__2BCE6B23_AC11_46CE_9D3C_34D3FE3D110E__INCLUDED_)
