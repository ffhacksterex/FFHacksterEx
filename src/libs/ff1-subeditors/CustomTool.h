#if !defined(AFX_CUSTOMTOOL_H__544455A5_9F33_470B_A7D9_FC80F0573604__INCLUDED_)
#define AFX_CUSTOMTOOL_H__544455A5_9F33_470B_A7D9_FC80F0573604__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CustomTool.h : header file
//

#include "resource_subeditors.h"
#include <FFBaseDlg.h>
class FFH2Project;
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// CCustomTool dialog

class CCustomTool : public CFFBaseDlg
{
// Construction
public:
	CCustomTool(CWnd* pParent = nullptr);   // standard constructor
	int tool;
	FFH2Project* Proj2 = nullptr;
	CFFHacksterProject* dat;
	CImageList* m_tiles;
	int slot;
	BYTE toolarray[9];

	CRect rcTiles;
	CRect rcTool;
	CPen redpen;

// Dialog Data
	enum { IDD = IDD_CUSTOMTOOL };

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

// Implementation
	// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

#endif // !defined(AFX_CUSTOMTOOL_H__544455A5_9F33_470B_A7D9_FC80F0573604__INCLUDED_)
