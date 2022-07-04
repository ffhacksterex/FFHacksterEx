#if !defined(AFX_MINIMAP_H__9A93F35D_2E6D_4F7C_8BC4_F23373EB2E79__INCLUDED_)
#define AFX_MINIMAP_H__9A93F35D_2E6D_4F7C_8BC4_F23373EB2E79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MiniMap.h : header file
//

#include "resource_subeditors.h"
#include <FFBaseDlg.h>
class CFFHacksterProject;
class ICoordMap;

/////////////////////////////////////////////////////////////////////////////
// CMiniMap dialog

class CMiniMap : public CFFBaseDlg
{
// Construction
public:
	CMiniMap(CWnd* pParent = nullptr);   // standard constructor

	CFFHacksterProject* cart;
	int Context;

	BYTE* Map;
	BYTE* palette;
	BYTE PalAssign[128];

	CRect rcOld;
	CRect rcNew;

	void FixRects(CPoint);
	void UpdateCur();
	void UpdateAll();

	bool mousedown;

	ICoordMap* OVmap;

// Dialog Data
	enum { IDD = IDD_MINIMAP };

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual BOOL OnInitDialog();

// Implementation
	// Generated message map functions
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_MINIMAP_H__9A93F35D_2E6D_4F7C_8BC4_F23373EB2E79__INCLUDED_)
