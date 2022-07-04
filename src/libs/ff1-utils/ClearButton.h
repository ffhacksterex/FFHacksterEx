#pragma once

#include <gdiplus.h>

// CClearButton
// Subclasses buttons to give them owner-draw customizations.
// If the ownerdraw and transparent styles are set, the button shows it's parent as its background.
// If the ownerdraw style and and clientedge exstyle are set, the client edge is replaced with WS_BORDER.

class CClearButton : public CButton
{
	DECLARE_DYNAMIC(CClearButton)

public:
	CClearButton();
	CClearButton(UINT resid, CString restype);
	virtual ~CClearButton();

	BOOL CreateButton(UINT id, CString caption, CRect rect, CWnd* parent);
	void SetButtonImage(HINSTANCE hinst, UINT resid, CString restype);

protected:
	UINT m_resid;
	CString m_restype;
	BOOL m_bUseNotifyForLeftClick;

	bool m_tracking = false;
	bool m_inclient = false;
	Gdiplus::Bitmap * m_bmp;

	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT pdi);

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnEnable(BOOL bEnable);
};
