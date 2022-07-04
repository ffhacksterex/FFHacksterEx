// ClearEdit.cpp : implementation file
//
#include "stdafx.h"
#include "ClearEdit.h"
#include "window_messages.h"
#include "ui_helpers.h"


// CClearEdit

IMPLEMENT_DYNAMIC(CClearEdit, CEdit)

CClearEdit::CClearEdit()
{
}

CClearEdit::~CClearEdit()
{
}


BEGIN_MESSAGE_MAP(CClearEdit, CEdit)
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CHAR()
END_MESSAGE_MAP()


// CClearEdit message handlers


void CClearEdit::OnNcPaint()
{
	if (!IsWindowVisible()) return;

	auto style = GetWindowLong(m_hWnd, GWL_STYLE);
	auto exstyle = GetExStyle();
	auto pdc = GetWindowDC();
	auto dcstate = pdc->SaveDC();

	CBrush br;
	if (exstyle & WS_EX_TRANSPARENT)
		br.CreateStockObject(HOLLOW_BRUSH);
	else
		br.CreateSysColorBrush(COLOR_WINDOW);
	pdc->SelectObject(&br);

	CPen pen;
	if ((style & WS_BORDER) || (exstyle & WS_EX_CLIENTEDGE))
		pen.CreateStockObject(BLACK_PEN);
	else
		pen.CreateStockObject(NULL_PEN);
	pdc->SelectObject(&pen);

	CRect rc;
	GetWindowRect(&rc);
	rc.OffsetRect(-rc.left, -rc.top);

	//REFACTOR - I think this check prevented non-bordered controls from repainting.
	//		since I don't use any in this project, the problem isn't visible.
	//		verify that it's not needed and remove it.
	//		However, there's still a problem with the non-client area not
	//		refreshing for multiline controls.
	//if ((style & WS_BORDER) || (exstyle & WS_EX_CLIENTEDGE))
	pdc->Rectangle(&rc);

	pdc->RestoreDC(dcstate);
	ReleaseDC(pdc);
}


void CClearEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	UNREFERENCED_PARAMETER(bCalcValidRects);

	RECT & rc = lpncsp->rgrc[0];
	int origcy = rc.bottom - rc.top;

	auto pdc = GetWindowDC();
	auto oldfont = pdc->SelectObject(GetFont());
	//REFACTOR - verify use of the actual text (instead of single line text) to calculate extent
	//auto sitext = pdc->GetTextExtent("Wy");
	CString text = Ui::GetControlText(*this);
	auto sitext = pdc->GetTextExtent(text);
	auto diff = (origcy - sitext.cy) / 2;
	pdc->SelectObject(oldfont);
	ReleaseDC(pdc);

	rc.top += diff;
	rc.bottom -= diff;
	rc.left += 1;
	rc.right -= 1;
}


BOOL CClearEdit::OnEraseBkgnd(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);

	return TRUE;
}


void CClearEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	CEdit::OnMouseMove(nFlags, point);
}