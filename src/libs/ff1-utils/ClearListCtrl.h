#pragma once

// CClearListCtrl

class CClearListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CClearListCtrl)

public:
	CClearListCtrl();
	virtual ~CClearListCtrl();

protected:
	void InvalidateIndex(int index);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
