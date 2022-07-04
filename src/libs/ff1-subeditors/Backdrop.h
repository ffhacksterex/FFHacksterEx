#if !defined(AFX_BACKDROP_H__898EF119_9925_4571_BE79_E1DA5D53D11B__INCLUDED_)
#define AFX_BACKDROP_H__898EF119_9925_4571_BE79_E1DA5D53D11B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Backdrop.h : header file
//

#include <SaveableDialog.h>
#include "DRAW_STRUCT.h"
#include <vector>
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// CBackdrop dialog

//DEVNOTE - for now. this class is a strict sub-editor, so it operates on the Project->ROM and DOES NOT load
//          from or save to files.
class CBackdrop : public CSaveableDialog
{
// Construction
public:
	CBackdrop(CWnd* pParent = nullptr);   // standard constructor

	bool NeedsRefresh() const;

	CFFHacksterProject* Project;
	BYTE backdrop;

	BYTE bitmap[0x400];
	BYTE palette[4];
	DRAW_STRUCT draw;
	BYTE PicFormation[32];

protected:
// Dialog Data
	enum { IDD = IDD_BACKDROPGRAPHIC };

	int BATTLEBACKDROPPALETTE_OFFSET = -1;
	int BATTLEPATTERNTABLE_OFFSET = -1;

	bool m_bNeedsRefesh = false;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
	void LoadRom();
	void SaveRom();
	void StoreValues();

	// Generated message map functions
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnExportbitmap();
	afx_msg void OnImportbitmap();
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_BACKDROP_H__898EF119_9925_4571_BE79_E1DA5D53D11B__INCLUDED_)
