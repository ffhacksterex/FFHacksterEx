// StrikeCheck.cpp : implementation file
//

#include "stdafx.h"
#include "StrikeCheck.h"
#include "ui_helpers.h"
#include "window_messages.h"
#include "imaging_helpers.h"

using namespace Ui;
using namespace Imaging;

// CStrikeCheck

IMPLEMENT_DYNAMIC(CStrikeCheck, CClearButton)

BOOL CStrikeCheck::RenderAsCheckbox = TRUE;
COLORREF CStrikeCheck::SharedCheckedColor = RGB(64, 128, 255);
CFont* CStrikeCheck::SharedCheckedFont = nullptr;

CStrikeCheck::CStrikeCheck()
{
	m_checkstate = BST_UNCHECKED;
	m_bUseNotifyForLeftClick = FALSE;
}

CStrikeCheck::~CStrikeCheck()
{
}

void CStrikeCheck::SetOverrideColor(COLORREF color)
{
	//DEVNOTE - COLORREF doesn't allow the high byte to be set, so if it's set, take that to mean "clear the override"
	UseOverriddenColor = (color != 0x80000000);
	UncheckedColor = UseOverriddenColor ? (color & 0x00FFFFFF) : RGB(0, 0, 0);
}

void CStrikeCheck::SetOverrideFont(CFont * font)
{
	UseOverriddenFont = (font != nullptr);
	UncheckedFont = font;
}


BEGIN_MESSAGE_MAP(CStrikeCheck, CClearButton)
	ON_WM_LBUTTONUP()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_MESSAGE(BM_SETCHECK, OnSetCheck)
	ON_MESSAGE(BM_GETCHECK, OnGetCheck)
END_MESSAGE_MAP()


// CStrikeCheck message handlers

void CStrikeCheck::Toggle()
{
	auto waschecked = GetCheck() == BST_CHECKED;
	SetCheck(waschecked ? BST_UNCHECKED : BST_CHECKED);
	InvalidateRect(nullptr);
}

bool CStrikeCheck::IsOwnerDraw() const
{
	return (GetStyle() & (BS_OWNERDRAW)) == BS_OWNERDRAW;
}

void CStrikeCheck::DrawItem(LPDRAWITEMSTRUCT pdi)
{
	if (!IsWindowVisible()) return;

	CRect client;
	GetClientRect(&client);

	CDC dc;
	dc.Attach(pdi->hDC);
	auto dcstate = dc.SaveDC();

	auto style = GetStyle();
	auto exstyle = GetExStyle();

	CPoint offset = { 0,0 };
	bool selected = (pdi->itemState & ODS_SELECTED) != 0;
	bool transparent = (exstyle & WS_EX_TRANSPARENT) != 0;
	bool enabled = (pdi->itemState & ODS_DISABLED) == 0;
	auto align = ConvertStyleToAlignment(style);

	if (!transparent) {
		CRect rcfill(client);
		rcfill.InflateRect(-1, -1);
		dc.FillSolidRect(&rcfill, GetSysColor(COLOR_BTNFACE));
	}

	const auto disabledcolor = RGB(128, 128, 128);
	CRect rctext = client;

	CString text;
	GetWindowText(text);

	auto oldtextcolor = dc.GetTextColor();
	bool checked = GetCheck() == BST_CHECKED;

	if (selected)
		rctext.OffsetRect(1, 1);
	if (transparent)
		dc.SetBkMode(TRANSPARENT);

	// Set the color and - if overridden - any overrides
	auto textcolor = disabledcolor;
	if (enabled) {
		textcolor = checked ? CStrikeCheck::SharedCheckedColor : RGB(0, 0, 0);

		// If overriding, switch to the font and/or overridden color now
		if (UseOverriddenColor) textcolor = this->SharedCheckedColor;
		if (UseOverriddenFont) dc.SelectObject(SharedCheckedFont);
	}

	CRect rcstrike = rctext;
	dc.DrawText(text, &rcstrike, align | DT_CALCRECT);

	auto tempbkmode = dc.GetBkMode();
	if (enabled && checked) {
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(RGB(128, 128, 128));
		rcstrike.OffsetRect(1, 1);
		//dc.DrawText(text, &rcstrike, align);
		rcstrike.OffsetRect(-1, -1);
	}
	dc.SetTextColor(textcolor);
	dc.DrawText(text, &rcstrike, align);
	dc.SetTextColor(oldtextcolor);
	dc.SetBkMode(tempbkmode);

	if (!checked) {
		CPen pen;
		pen.CreatePen(PS_SOLID, 1, textcolor);
		auto oldpen = dc.SelectObject(pen);
		auto y = rcstrike.top + (rcstrike.Height() / 2);
		dc.MoveTo(rcstrike.left, y);
		dc.LineTo(rcstrike.right, y);
		dc.SelectObject(oldpen);
	}

	// Don't do focus/hover if disabled
	if (enabled) {
		CImage* pimg = nullptr;
		if (selected)  pimg = &GetMouseOverSelImg();
		else if (m_inclient) pimg = &GetMouseOverHotImg();
		if (pimg && *pimg) pimg->AlphaBlend(dc.GetSafeHdc(), client, GetMouseOverImgRect(), 0x70);

		if (pdi->itemState & ODS_FOCUS) {
			CRect focus(rctext);
			dc.DrawFocusRect(&focus);
		}
	}

	// Draw the border (use a hollow brush to avoid overwriting the interior we just drew).
	if ((style & WS_BORDER) || (exstyle & (WS_EX_CLIENTEDGE | WS_EX_STATICEDGE))) {
		dc.SelectStockObject(HOLLOW_BRUSH);
		if (enabled) {
			dc.SelectStockObject(BLACK_PEN);
			dc.Rectangle(&client);
		}
		else {
			dc.SelectStockObject(DC_PEN);
			auto olddccolor = dc.SetDCPenColor(disabledcolor);
			dc.Rectangle(&client);
			dc.SetDCPenColor(olddccolor);
		}
	}

	dc.RestoreDC(dcstate);
	dc.Detach();
}

void CStrikeCheck::PreSubclassWindow()
{
	// This class is intended to allow check boxes to display using a strikeout instead of a check image.
	// So if the button has either check box style, add the ownerdraw style before subclassing it.
	// That prevents us from having to set the style for every control in every dialogue box that wants to use it.

	if (GetStyle() & (BS_CHECKBOX))
	{
		if (!CStrikeCheck::RenderAsCheckbox)
			ModifyStyle(0, BS_OWNERDRAW);
	}
	CClearButton::PreSubclassWindow();
}

void CStrikeCheck::OnLButtonUp(UINT nFlags, CPoint point)
{
	Toggle();
	CClearButton::OnLButtonUp(nFlags, point);
	// don't call the base class here

	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
}


void CStrikeCheck::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	// don't call the base class here

	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
}

void CStrikeCheck::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//REFACTOR - not sure if this is still needed
	CClearButton::OnChar(nChar, nRepCnt, nFlags);

	UNREFERENCED_PARAMETER(nChar);
	UNREFERENCED_PARAMETER(nRepCnt);
	UNREFERENCED_PARAMETER(nFlags);
}

void CStrikeCheck::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_SPACE) {
		Toggle();
	}
	// don't call the base class here

	UNREFERENCED_PARAMETER(nChar);
	UNREFERENCED_PARAMETER(nRepCnt);
	UNREFERENCED_PARAMETER(nFlags);
}

void CStrikeCheck::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// don't call the base class here

	UNREFERENCED_PARAMETER(nChar);
	UNREFERENCED_PARAMETER(nRepCnt);
	UNREFERENCED_PARAMETER(nFlags);
}

LRESULT CStrikeCheck::OnSetCheck(WPARAM wparam, LPARAM lparam)
{
	if (IsOwnerDraw()) {
		m_checkstate = (int)wparam;
		InvalidateRect(nullptr);
	}

	UNREFERENCED_PARAMETER(wparam);
	UNREFERENCED_PARAMETER(lparam);
	return Default();
}

LRESULT CStrikeCheck::OnGetCheck(WPARAM wparam, LPARAM lparam)
{
	if (IsOwnerDraw()) {
		return m_checkstate;
	}

	UNREFERENCED_PARAMETER(wparam);
	UNREFERENCED_PARAMETER(lparam);
	return Default();
}

// Another workaround...
// Owner draw buttons (like this) that aren't tied to commands
// don't trigger a dialog's default button the way standard
// buttons do.
// So when ENTER/RETURN is pressed, we'll trigger the default button if:
// 1) our parent is a CDialog-derived class
// 2) we get a default ID value that isn't reserved.
// It appears to take quite a bit more work than this
// to properly (and generally) make owner draw buttons
// handle this scenario. For now, this quick-n-dirty
// workaround will have to do.

BOOL CStrikeCheck::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_RETURN) {
			::DispatchMessage(pMsg);
			auto pdlg = DYNAMIC_DOWNCAST(CDialog, GetParent());
			auto defid = (pdlg != nullptr) ? pdlg->GetDefID() : 0;

			// avoid the dozen reserved dialog control IDs (IDOK, etc.)
			auto pctl = defid > 12 ? pdlg->GetDlgItem(defid) : nullptr;
			if (defid > 0)
				pdlg->PostMessage(WM_COMMAND, MAKEWPARAM(defid, BN_CLICKED), (LPARAM)pctl);
			return true;
		}
		break;
	}
	return CClearButton::PreTranslateMessage(pMsg);
}
