#pragma once


// CClearEdit
//REFACTOR - multiline edit controls don't redraw properly when scrolling
//		(the client area doesn't repaint properly).
//		There are currently no multiline edits that are transparent,
//		so the problem isn't currently visible.
class CClearEdit : public CEdit
{
	DECLARE_DYNAMIC(CClearEdit)

public:
	CClearEdit();
	virtual ~CClearEdit();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
