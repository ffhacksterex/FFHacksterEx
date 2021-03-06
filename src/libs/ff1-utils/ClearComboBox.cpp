// ClearComboBox.cpp : implementation file
//
#include "stdafx.h"
#include "ClearComboBox.h"
#include "ODUtils.h"


// CClearComboInbeddedEdit

IMPLEMENT_DYNAMIC(CClearComboInbeddedEdit, CEdit)

CClearComboInbeddedEdit::CClearComboInbeddedEdit()
{
}

CClearComboInbeddedEdit::~CClearComboInbeddedEdit()
{
}

BEGIN_MESSAGE_MAP(CClearComboInbeddedEdit, CRichEditCtrl)
	ON_WM_HSCROLL()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CClearComboInbeddedEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
	Invalidate();
}

void CClearComboInbeddedEdit::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);
	Invalidate();
}

void CClearComboInbeddedEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	auto combo = dynamic_cast<CComboBox*>(GetParent());
	if (combo != nullptr) {
		if (nChar == VK_ESCAPE) {
			combo->SendMessage(CB_SHOWDROPDOWN, FALSE);
		}
		else if (nChar == VK_RETURN) {
			auto sel = combo->GetCurSel();
			combo->SendMessage(CB_SHOWDROPDOWN, FALSE);
			if (sel != CB_ERR)
				combo->SetCurSel(sel);
		}
		combo->Invalidate();
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}



// #################################################################################
// #################################################################################
// CClearComboBox

IMPLEMENT_DYNAMIC(CClearComboBox, CComboBox)

CClearComboBox::CClearComboBox()
{
}

CClearComboBox::~CClearComboBox()
{
}

void CClearComboBox::SubclassEdit()
{
	if ((GetStyle() & CBS_DROPDOWN) == CBS_DROPDOWN) {
		// Alas, this isn't full owner-draw implementation, just enough for this app.
		// Only works with dropedits, but drawing gets funky if the user can edit the strings.
		// Set the edit portion to readonly.
		COMBOBOXINFO info = { 0 };
		info.cbSize = sizeof(info);
		if (GetComboBoxInfo(&info)) {
			auto *pedit = (CEdit*)CWnd::FromHandlePermanent(info.hwndItem);
			if (pedit != nullptr)
				pedit->SetReadOnly(TRUE);
		}
	}
}


BEGIN_MESSAGE_MAP(CClearComboBox, CComboBox)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_CTLCOLOR()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CClearComboBox::OnCbnDropdown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, &CClearComboBox::OnCbnCloseup)
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_MESSAGE(CB_SETCURSEL, &CClearComboBox::OnSetCurSel)
END_MESSAGE_MAP()


void CClearComboBox::PreSubclassWindow()
{
	CComboBox::PreSubclassWindow();
	SubclassEdit();
}


// CClearComboBox message handlers

int CClearComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	SubclassEdit();
	return 0;
}

LRESULT CClearComboBox::OnSetCurSel(WPARAM wparam, LPARAM lparam)
{
	UNREFERENCED_PARAMETER(wparam);
	UNREFERENCED_PARAMETER(lparam);
	Invalidate();
	return Default();
}

void CClearComboBox::OnPaint()
{
	CPaintDC dc(this);
	auto dcstate = dc.SaveDC();

	CRect rcclip;
	GetClientRect(&rcclip);
	rcclip.DeflateRect(1, 1);

	bool drawbrushed = ODUtils::HasAllStyles(GetExStyle(), WS_EX_TRANSPARENT);
	auto brush = drawbrushed ? OnCtlColor(&dc, this, CTLCOLOR_STATIC) : NULL;
	if (brush != NULL) {
		dc.SelectStockObject(NULL_PEN);
		dc.SelectObject(brush);
		dc.Rectangle(&rcclip);
	}
	else {
		dc.FillSolidRect(&rcclip, GetSysColor(COLOR_WINDOW));
	}

	CRect rcarrow = rcclip;
	rcarrow.InflateRect(1, 1);
	rcarrow.left = rcclip.right - GetSystemMetrics(SM_CXVSCROLL);

	// If there's a selection, draw it
	if (GetCurSel() != CB_ERR) {
		CString text;
		GetLBText(GetCurSel(), text);

		// Pinch the text in from the border a bit, and use the scroll arrows for the button
		CRect rctext = rcclip;
		rctext.DeflateRect(4, 2);
		rctext.right -= (GetSystemMetrics(SM_CXVSCROLL));
		dc.SelectObject(GetFont());

		// Draw highlighted when the control is selected and the list is NOT dropped
		auto focused = GetFocus() == this;
		auto dropped = GetDroppedState() == TRUE;
		if (!IsWindowEnabled()) {
			//REFACTOR - should disabled controls use a non-transparent background
			dc.SetBkMode(TRANSPARENT);
			dc.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
		}
		else if (focused && !dropped) {
			dc.SetBkMode(OPAQUE);
			dc.SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
			dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
		}
		else {
			dc.SetBkMode(TRANSPARENT);
			dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		}

		/// Draw the text
		if ((GetStyle() & CBS_DROPDOWNLIST) == 0) {
			//BUG - display issue, need to update when text is scrolled
			auto startat = m_item.CharFromPos(rctext.TopLeft());
			text = text.Mid(startat);
		}

		dc.DrawText(text, &rctext, DT_SINGLELINE | DT_VCENTER);
	}

	// Draw the button arroe down if not dropped, up if dropped
	auto dfcsarrow = GetDroppedState() ? DFCS_SCROLLUP : DFCS_SCROLLDOWN;
	dc.DrawFrameControl(&rcarrow, DFC_SCROLL, DFCS_FLAT | dfcsarrow);

	dc.RestoreDC(dcstate);
}

void CClearComboBox::OnNcPaint()
{
	CWindowDC dc(this);
	CRect rcclip;
	dc.GetClipBox(&rcclip);

	auto dcstate = dc.SaveDC();
	dc.SelectStockObject(BLACK_PEN);
	dc.SelectStockObject(NULL_BRUSH);
	dc.Rectangle(&rcclip);
	dc.RestoreDC(dcstate);
}

HBRUSH CClearComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0) {
		CString cs;
		cs.Format(_T("Unknown control class '%s' cannot handle WM_CTLCOLOR\n"), classname);
		OutputDebugString(cs);
		return hbr;
	}
	if (_tcsicmp(classname, _T("COMBOLBOX")) == 0)
		return hbr;

	return ODUtils::GetODCtlColorBrush(this, pDC);
}

void CClearComboBox::OnCbnDropdown()
{
	RedrawWindow(nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
}

void CClearComboBox::OnCbnCloseup()
{
	RedrawWindow(nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
}

BOOL CClearComboBox::OnEraseBkgnd(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
	return TRUE;
}

void CClearComboBox::OnSetFocus(CWnd* pOldWnd)
{
	CComboBox::OnSetFocus(pOldWnd);
	Invalidate();
}

void CClearComboBox::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);
	Invalidate();
}