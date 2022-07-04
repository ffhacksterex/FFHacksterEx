#pragma once

#include "ODBackground.h"

class CClearListBox : public CListBox
{
	DECLARE_DYNAMIC(CClearListBox)

public:
	CClearListBox();
	~CClearListBox();

	bool UseInactiveColors = true;

protected:
	CODBackground m_bg;

	BOOL IsOwnerDraw();

	virtual void Draw(CDC & dc);
	virtual int CalcBaseItemHeight();
	virtual void HandleSelchange();  // descendants should call the base class
	virtual void DrawItem(LPDRAWITEMSTRUCT pdi);
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLbnSelchange();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
