#include "stdafx.h"
#include "ClearListBox.h"
#include "ODUtils.h"
#include "ui_helpers.h"


IMPLEMENT_DYNAMIC(CClearListBox, CListBox)

CClearListBox::CClearListBox()
{
}


CClearListBox::~CClearListBox()
{
}


BOOL CClearListBox::IsOwnerDraw()
{
	auto style = GetStyle();
	return ((style & LBS_OWNERDRAWFIXED) == LBS_OWNERDRAWFIXED) ||
		((style & LBS_OWNERDRAWVARIABLE) == LBS_OWNERDRAWVARIABLE);
}


void CClearListBox::Draw(CDC & dc)
{
	CRect rccli;
	GetClientRect(&rccli);
	rccli.InflateRect(1, 1); // compensate for the borders

	// DRAW THE BACKGROUND
	//TODO - clean up the bottom edge of the control
	auto drawdefault = true;
	if (GetExStyle() & WS_EX_TRANSPARENT) {
		auto oldorg = dc.GetBrushOrg();
		auto brush = ODUtils::GetODCtlColorBrush(this, &dc);
		if (brush != NULL) {
			auto oldpen = dc.SelectStockObject(NULL_PEN);
			auto oldbr = dc.SelectObject(brush);

			dc.Rectangle(&rccli);

			dc.SelectObject(oldbr);
			dc.SelectObject(oldpen);
			drawdefault = false;
		}
		dc.SetBrushOrg(oldorg);
	}
	if (drawdefault) {
		dc.FillSolidRect(&rccli, GetSysColor(COLOR_WINDOW));
	}

	// DRAW THE ITEMS
	auto leading = dc.GetTextExtent(CString(" ")).cx / 2;
	auto index = GetTopIndex();
	auto count = GetCount();
	if (index != LB_ERR && count > 0) {
		//REFACTOR - with this method, try using each item's rect instead of using GetItemHeight()

		auto oldfont = dc.SelectObject(GetFont());
		auto oldcolor = dc.GetBkColor();
		auto oldmode = dc.SetBkMode(TRANSPARENT);

		LOGFONT lf = { 0 };
		GetFont()->GetLogFont(&lf);

		bool disabled = !IsWindowEnabled();
		bool focused = GetFocus() == this;
		CRect rect(0, 0, rccli.Width(), GetItemHeight(0));
		for (int i = index; i < count; ++i) {
			if (rect.top > rccli.Height()) break;

			GetItemRect(i, &rect);

			CString text;
			GetText(i, text);

			auto oldbr = dc.SelectStockObject(HOLLOW_BRUSH);
			auto selected = GetSel(i);
			auto colorindex = COLOR_WINDOWTEXT;

			// BACKGROUND
			if (disabled) {
				if (selected) dc.FillSolidRect(&rect, GetSysColor(COLOR_INACTIVECAPTION));
				colorindex = selected ? COLOR_INACTIVECAPTIONTEXT : COLOR_GRAYTEXT;
			}
			else if (focused || !UseInactiveColors) {
				if (selected) dc.FillSolidRect(&rect, GetSysColor(COLOR_HIGHLIGHT));
				colorindex = selected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT;
			}
			else {
				if (selected) dc.FillSolidRect(&rect, GetSysColor(COLOR_INACTIVECAPTION));
				colorindex = selected ? COLOR_INACTIVECAPTIONTEXT : COLOR_WINDOWTEXT;
			}

			CRect rctext(rect);
			rctext.left += leading;
			dc.SetTextColor(GetSysColor(colorindex));
			dc.DrawText(text, &rctext, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
			dc.SelectObject(oldbr);
		}

		if (focused) {
			auto selindex = GetCurSel();
			if (selindex == LB_ERR) selindex = 0;
			CRect rcfocus;
			GetItemRect(selindex, &rcfocus);
			dc.DrawFocusRect(&rcfocus);
		}

		dc.SetBkMode(oldmode);
		dc.SetBkColor(oldcolor);
		dc.SelectObject(oldfont);
	}
}


int CClearListBox::CalcBaseItemHeight()
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(GetFont());
	TEXTMETRIC tm;
	VERIFY(dc.GetTextMetrics(&tm));
	dc.SelectObject(pOldFont);

	auto height = tm.tmHeight + tm.tmExternalLeading;

	return __max(height, 10);
}


void CClearListBox::HandleSelchange()
{
	Invalidate();
	UpdateWindow();
}


BEGIN_MESSAGE_MAP(CClearListBox, CListBox)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CClearListBox message handlers



void CClearListBox::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	// Don't do anything here, but we need to avoid the assert
}


BOOL CClearListBox::OnEraseBkgnd(CDC* pDC)
{
	if (IsOwnerDraw()) {
		return TRUE;
	}
	else {
		return CListBox::OnEraseBkgnd(pDC);
	}
}


void CClearListBox::OnPaint()
{
	if (IsOwnerDraw()) {
		CPaintDC dc(this);
		auto dcstate = dc.SaveDC();
		dc.SelectObject(GetFont());
		dc.SetBkMode(TRANSPARENT);

		CRect rccli;
		GetClientRect(&rccli);

		// DRAW THE BACKGROUND
		CBitmap membmp;
		membmp.CreateCompatibleBitmap(&dc, rccli.Width(), rccli.Height());

		// DRAW THE VISIBLE ITEMS (DOUBLE-BUFFERED)
		CDC memdc;
		memdc.CreateCompatibleDC(&dc);

		// Draw to an offscreen bitmap, then draw that bitmap to the client area
		auto oldbmp = memdc.SelectObject(&membmp);
		Draw(memdc);
		dc.BitBlt(0, 0, rccli.Width(), rccli.Height(), &memdc, 0, 0, SRCCOPY);
		memdc.SelectObject(oldbmp);

		dc.RestoreDC(dcstate);
	}
	else {
		CListBox::OnPaint();
	}
}


void CClearListBox::OnKillFocus(CWnd* pNewWnd)
{
	CListBox::OnKillFocus(pNewWnd);
	Invalidate();
}


void CClearListBox::OnSetFocus(CWnd* pOldWnd)
{
	CListBox::OnSetFocus(pOldWnd);
	Invalidate();
}

void CClearListBox::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SetRedraw(FALSE);
	CListBox::OnHScroll(nSBCode, nPos, pScrollBar);
	SetRedraw(TRUE);

	RedrawWindow(nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
}


void CClearListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SetRedraw(FALSE);
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
	SetRedraw(TRUE);

	RedrawWindow(nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
}


void CClearListBox::OnLbnSelchange()
{
	HandleSelchange();
	Ui::SendLbnSelchangeToParent(this);
}


int CClearListBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//DEVNOTE - this control currently requires strings to be stored in the list box
	ASSERT(lpCreateStruct->style & LBS_HASSTRINGS);

	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetItemHeight(0, CalcBaseItemHeight());

	return 0;
}


void CClearListBox::PreSubclassWindow()
{
	//DEVNOTE - this control currently requires strings to be stored in the list box
	ASSERT(GetStyle() & LBS_HASSTRINGS);

	SetItemHeight(0, CalcBaseItemHeight());

	ModifyStyle(0, WS_BORDER);
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

	CListBox::PreSubclassWindow();
}


BOOL CClearListBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	SetRedraw(FALSE);
	auto result = CListBox::OnMouseWheel(nFlags, zDelta, pt);
	SetRedraw(TRUE);

	RedrawWindow(nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
	return result;
}


void CClearListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	SetRedraw(FALSE);
	CListBox::OnMouseMove(nFlags, point);
	SetRedraw(TRUE);

	RedrawWindow(nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
}

void CClearListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonUp(nFlags, point);
	Invalidate();
}


void CClearListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
	Invalidate();
}