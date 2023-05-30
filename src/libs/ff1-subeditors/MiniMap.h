#if !defined(AFX_MINIMAP_H__9A93F35D_2E6D_4F7C_8BC4_F23373EB2E79__INCLUDED_)
#define AFX_MINIMAP_H__9A93F35D_2E6D_4F7C_8BC4_F23373EB2E79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MiniMap.h : header file
//

#include "resource_subeditors.h"
#include <FFBaseDlg.h>
class ICoordMap;

/////////////////////////////////////////////////////////////////////////////
// CMiniMap dialog

class CMiniMap : public CFFBaseDlg
{
// Construction
public:
	CMiniMap(CWnd* pParent = nullptr);   // standard constructor

	FFH2Project* Proj2 = nullptr;
	int Context;
	ICoordMap* OVmap;
	BYTE* Map;
	BYTE* palette;
	BYTE PalAssign[128];
	CSize Tilesize = { 16,16 };

	void SetFocusRect(int left, int top, int right, int bottom);
	void SetFocusRect(CRect rect);
	void UpdateFocusRect(int left, int top, int right, int bottom);
	void UpdateFocusRect(CPoint pt, CSize sz);
	void UpdateFocusRect(CRect rect);
	void UpdateCur();
	void UpdateAll();

protected:
	CRect rcOld;
	CRect rcNew;
	bool mousedown;

// Dialog Data
	enum { IDD = IDD_MINIMAP };

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
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#endif // !defined(AFX_MINIMAP_H__9A93F35D_2E6D_4F7C_8BC4_F23373EB2E79__INCLUDED_)
