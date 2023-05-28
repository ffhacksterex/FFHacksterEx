#if !defined(AFX_TILEEDIT_H__859B9274_D046_4A80_BFF1_282A41EF7993__INCLUDED_)
#define AFX_TILEEDIT_H__859B9274_D046_4A80_BFF1_282A41EF7993__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TileEdit.h : header file
//

#include <SaveableDialog.h>
#include "resource_subeditors.h"
#include "DRAW_STRUCT.h"
class CFFHacksterProject;
class FFH2Project;

/////////////////////////////////////////////////////////////////////////////
// CTileEdit dialog

class CTileEdit : public CSaveableDialog
{
// Construction
public:
	CTileEdit(CWnd* pParent = nullptr);   // standard constructor

	enum InvokerType { None = 0, Overworld, Maps };

	CFFHacksterProject* cart = nullptr;
	FFH2Project* Proj2 = nullptr;
	
	InvokerType Invoker = None;

	BYTE tileset;
	BYTE tile;
	BYTE* pal[2];
	BYTE palette[2][16];
	BYTE PicFormation[4];
	BYTE palassign;
	BYTE bitmap[0x4000];
	BYTE curblock;

	DRAW_STRUCT draw;
	CRect rcPatternTables;
	CRect rcFinger;
	CPen redpen;

// Dialog Data
	enum { IDD = IDD_TILEEDIT };

protected:
	CButton	m_small;
	CButton	m_large;
	CButton	m_showroom;

	int OVERWORLDPALETTE_ASSIGNMENT = -1;
	int OVERWORLDPALETTE_OFFSET = -1;
	int OVERWORLDPATTERNTABLE_ASSIGNMENT = -1;
	int OVERWORLDPATTERNTABLE_OFFSET = -1;
	int TILESETPATTERNTABLE_OFFSET = -1;
	int TILESETPALETTE_ASSIGNMENT = -1;
	int TILESETPATTERNTABLE_ASSIGNMENT = -1;
	int BANK03_OFFSET = -1;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	void LoadRom();
	virtual void SaveRom();
	virtual void StoreValues();

	// Generated message map functions
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnShowroom();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

#endif // !defined(AFX_TILEEDIT_H__859B9274_D046_4A80_BFF1_282A41EF7993__INCLUDED_)
