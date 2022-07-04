#if !defined(AFX_MAPMAN_H__BFC01627_E214_4EF1_A1FA_19C33C4061FB__INCLUDED_)
#define AFX_MAPMAN_H__BFC01627_E214_4EF1_A1FA_19C33C4061FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Mapman.h : header file
//

#include <SaveableDialog.h>
#include "DRAW_STRUCT.h"
#include "resource_subeditors.h"
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// CMapman dialog

class CMapman : public CSaveableDialog
{
// Construction
public:
	CMapman(CWnd* pParent = nullptr);   // standard constructor

	CFFHacksterProject* cart = nullptr;
	bool InMemory = false;
	int graphicoffset;
	int paletteoffset;

	BYTE bitmap[0x400];
	BYTE palette[8];
	DRAW_STRUCT draw;
	BYTE PicFormation[16];
	BYTE PalFormation[16];

	void LoadRom();
	virtual void SaveRom();
	virtual void StoreValues();

// Dialog Data
	enum { IDD = IDD_MAPMAN };

protected:
	int OVERWORLDPALETTE_OFFSET = -1;
	int MAPMANPALETTE_OFFSET = -1;
	int BANK00_OFFSET = -1;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
	// Generated message map functions
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnExportbitmap();
	afx_msg void OnImportbitmap();
};

#endif // !defined(AFX_MAPMAN_H__BFC01627_E214_4EF1_A1FA_19C33C4061FB__INCLUDED_)
