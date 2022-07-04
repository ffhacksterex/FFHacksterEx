#if !defined(AFX_BATTLEPIC_H__E2CFFD07_1ED2_456B_90F8_471E3D1EA4DB__INCLUDED_)
#define AFX_BATTLEPIC_H__E2CFFD07_1ED2_456B_90F8_471E3D1EA4DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BattlePic.h : header file
//

#include <SaveableDialog.h>
#include "DRAW_STRUCT.h"
#include "resource_subeditors.h"
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// CBattlePic dialog

class CBattlePic : public CSaveableDialog
{
// Construction
public:
	CBattlePic(CWnd* pParent = nullptr);   // standard constructor

	CFFHacksterProject * cart = nullptr;
	int Class = -1;
	bool InMemory = false;

protected:
	DRAW_STRUCT draw;

	BYTE bitmap[0x800];
	BYTE PalAssign;
	BYTE palette[4];
	BYTE PicFormation[39];
	
	void LoadRom();
	virtual void SaveRom();
	virtual void StoreValues();

	void StorePalette();
	void ReLoadPalette();
	void UpdatePalAssignment(BYTE);

// Dialog Data
	enum { IDD = IDD_BATTLEPIC };
	CButton	m_palette4;
	CButton	m_palette3;
	CButton	m_palette2;
	CButton	m_palette1;

	unsigned int CHARBATTLEPALETTE_ASSIGNMENT1 = (unsigned int)-1; // bank 0C
	unsigned int CHARBATTLEPALETTE_ASSIGNMENT2 = (unsigned int)-1; // bank 0F
	int CHARBATTLEPIC_OFFSET = -1;                                 // bank 09
	unsigned int CHARBATTLEPALETTE_OFFSET = (unsigned int)-1;      // bank 0F
	int BINBANK09DATA_OFFSET = -1;
	int BINBANK09GFXDATA_OFFSET = -1;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	// Generated message map functions
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPalette1();
	afx_msg void OnPalette2();
	afx_msg void OnPalette3();
	afx_msg void OnPalette4();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnExportbitmap();
	afx_msg void OnImportbitmap();
	DECLARE_MESSAGE_MAP()
};


#endif // !defined(AFX_BATTLEPIC_H__E2CFFD07_1ED2_456B_90F8_471E3D1EA4DB__INCLUDED_)
