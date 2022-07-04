// ClearListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ClearListCtrl.h"


// CClearListCtrl

IMPLEMENT_DYNAMIC(CClearListCtrl, CListCtrl)

CClearListCtrl::CClearListCtrl()
{

}

CClearListCtrl::~CClearListCtrl()
{
}

void CClearListCtrl::InvalidateIndex(int index)
{
	CRect rcitem;
	GetItemRect(index, &rcitem, LVIR_BOUNDS);
	rcitem.bottom += 128;
	InvalidateRect(&rcitem);
}


BEGIN_MESSAGE_MAP(CClearListCtrl, CListCtrl)
	ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CClearListCtrl message handlers


void CClearListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
	Invalidate();
}

void CClearListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	Invalidate();
}

void CClearListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListCtrl::OnLButtonDown(nFlags, point);
	Invalidate();
}


BOOL CClearListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	Invalidate();
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}


BOOL CClearListCtrl::OnEraseBkgnd(CDC* pDC)
{
	return CListCtrl::OnEraseBkgnd(pDC);
}