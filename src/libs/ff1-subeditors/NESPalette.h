#if !defined(AFX_NESPALETTE_H__9621A6D8_610B_4320_9F30_E61805FB5711__INCLUDED_)
#define AFX_NESPALETTE_H__9621A6D8_610B_4320_9F30_E61805FB5711__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NESPalette.h : header file
//

#include "resource_subeditors.h"
#include <FFBaseDlg.h>
class CFFHacksterProject;
class FFH2Project;

/////////////////////////////////////////////////////////////////////////////
// CNESPalette dialog

class CNESPalette : public CFFBaseDlg
{
// Construction
public:
	CFFHacksterProject* cart = nullptr;
	FFH2Project* Proj2 = nullptr;
	CRect rcPalette;
	BYTE* color;
	CNESPalette(CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	enum { IDD = IDD_NESPALETTE };

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
	// Generated message map functions
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_NESPALETTE_H__9621A6D8_610B_4320_9F30_E61805FB5711__INCLUDED_)
