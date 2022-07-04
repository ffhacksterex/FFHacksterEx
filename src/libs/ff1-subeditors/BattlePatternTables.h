#if !defined(AFX_BATTLEPATTERNTABLES_H__FF7A1F7A_4BAC_4748_8C4F_5842C9473EA4__INCLUDED_)
#define AFX_BATTLEPATTERNTABLES_H__FF7A1F7A_4BAC_4748_8C4F_5842C9473EA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BattlePatternTables.h : header file
//

#include <SaveableDialog.h>
#include "resource_subeditors.h"
#include "DRAW_STRUCT.h"
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// CBattlePatternTables dialog

class CBattlePatternTables : public CSaveableDialog
{
// Construction
public:
	CBattlePatternTables(CWnd* pParent = nullptr);   // standard constructor

	CFFHacksterProject* cart = nullptr;
	BYTE palette[16];
	BYTE patterntable;
	BYTE view;
	BYTE palview;
	BYTE palvalues[2];

	DRAW_STRUCT draw;
	BYTE bitmap[0x2000];
	CRect rcPattern;
	CRect rcMaxGraphic;
	CRect rcFinger;
	BYTE PicFormation[168];
	BYTE PalFormation[168];

	short cur_backdrop;

	void LoadRom();
	virtual void SaveRom();
	virtual void StoreValues();

	void InitView();
	void UpdatePalView(BYTE up,bool redraw = 1);
	void UpdatePaletteRadios();

// Dialog Data
	enum { IDD = IDD_BATTLEPATTERNTABLES };

protected:
	CButton	m_reshape;
	CButton	m_repalette;
	CComboBox	m_backdrop;
	CButton	m_pal4;
	CButton	m_pal3;
	CButton	m_pal2;
	CButton	m_pal1;

	int BATTLEPALETTE_OFFSET = -1;
	int BATTLEPATTERNTABLE_OFFSET = -1;
	int CHAOSPALETTE_TABLE = -1;
	int CHAOSDRAW_TABLE = -1;
	int FIENDPALETTE_TABLE = -1;
	int FIENDDRAW_SHIFT = -1;
	int FIENDDRAW_TABLE = -1;
	int BATTLEBACKDROPPALETTE_OFFSET = -1;

	int BANK00_OFFSET = -1;
	int BANK07_OFFSET = -1;
	int BINBATTLEPALETTES_OFFSET = -1;
	int BINCHAOSTSA_OFFSET = -1;
	int BINFIENDTSA_OFFSET = -1;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
	// Generated message map functions
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPal1();
	afx_msg void OnPal2();
	afx_msg void OnPal3();
	afx_msg void OnPal4();
	afx_msg void OnSelchangeBackdrop();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRepalette();
	afx_msg void OnReshape();
	afx_msg void OnExportbitmap();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnImportbitmap();
};

#endif // !defined(AFX_BATTLEPATTERNTABLES_H__FF7A1F7A_4BAC_4748_8C4F_5842C9473EA4__INCLUDED_)
