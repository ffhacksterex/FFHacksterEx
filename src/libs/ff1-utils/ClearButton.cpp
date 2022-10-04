// ClearButton.cpp : implementation file
//

#include "stdafx.h"
#include "ClearButton.h"
#include "ui_helpers.h"
#include "window_messages.h"
#include "imaging_helpers.h"
#include "BackgroundMixin.h"
#include "ODUtils.h"

using namespace Ui;
using namespace Imaging;

// CClearButton

namespace {
	CPen s_blackpen;
	CPen s_whitepen;
	CPen s_okpen;
	CPen s_applypen;
	CPen s_cancelpen;

	bool s_initialized = []() {
		s_blackpen.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_WINDOWTEXT));
		s_whitepen.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_GRAYTEXT));
		return true;
	}();
}


IMPLEMENT_DYNAMIC(CClearButton, CButton)


CClearButton::CClearButton()
	: m_bmp(nullptr)
	, m_bUseNotifyForLeftClick(TRUE)
{
}

CClearButton::CClearButton(UINT resid, CString restype)
	: m_resid(resid)
	, m_restype(restype)
	, m_bUseNotifyForLeftClick(TRUE)
{
}

CClearButton::~CClearButton()
{
	if (m_bmp != nullptr) delete m_bmp;
}

void CClearButton::SetEndWithEllipsis(bool set)
{
	m_endellipsis = set;
	InvalidateRect(nullptr, 0);
}

bool CClearButton::EndWithEllipsis() const
{
	return m_endellipsis;
}

BOOL CClearButton::CreateButton(UINT id, CString caption, CRect rect, CWnd* parent)
{
	return CreateEx(WS_EX_CLIENTEDGE | WS_EX_TRANSPARENT, "BUTTON", caption,
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT | BS_CENTER | BS_VCENTER | BS_OWNERDRAW,
		rect, parent, id);
}

void CClearButton::SetButtonImage(HINSTANCE hinst, UINT resid, CString restype)
{
	if (m_bmp != nullptr) delete m_bmp;
	m_resid = resid;
	m_restype = restype;
	m_bmp = ImageLoad(hinst, m_resid, m_restype);
}


BEGIN_MESSAGE_MAP(CClearButton, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	ON_WM_NCPAINT()
	ON_WM_ENABLE()
END_MESSAGE_MAP()


// CClearButton message handlers

void CClearButton::DrawItem(LPDRAWITEMSTRUCT pdi)
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
	bool disabled = (pdi->itemState & ODS_DISABLED) != 0;

	// CALCULATE TEXT ALIGNMENT
	using namespace ODUtils;
	int align = 0;
	if (HasAllStyles(style, BS_CENTER))
		align |= DT_CENTER;
	else if (HasAllStyles(style, BS_LEFT))
		align |= DT_LEFT;
	else if (HasAllStyles(style, BS_RIGHT))
		align |= DT_RIGHT;

	if (HasAllStyles(style, BS_VCENTER))
		align |= DT_VCENTER;
	else if (HasAllStyles(style, BS_TOP))
		align |= DT_TOP;
	else if (HasAllStyles(style, BS_BOTTOM))
		align |= DT_BOTTOM;

	if (!HasAllStyles(style, BS_MULTILINE))
		align |= DT_SINGLELINE;
	else
		align |= DT_WORDBREAK;

	if (m_endellipsis)
		align |= DT_END_ELLIPSIS;

	if (!transparent) {
		CRect rcfill(client);
		rcfill.InflateRect(-1, -1);
		dc.FillSolidRect(&rcfill, GetSysColor(COLOR_BTNFACE));
	}

	bool imageonly = (style & BS_BITMAP) == BS_BITMAP;
	if (m_bmp == nullptr) {
		imageonly = false; // this is only done to avoid blank buttons if no valid image was set
	}
	else {
		// GDI+ can render PNGs with translucency (32-bit).
		//REFACTOR - to do this with GDI, the AlphaBlend function could be used instead of GDI+.
		using namespace Gdiplus;
		Graphics g(dc);
		int w = m_bmp->GetWidth();
		int h = m_bmp->GetHeight();
		int cw = client.Width();
		int ch = client.Height();
		//N.B. - currently, no clipping is done
		int x = (cw - w) / 2;
		int y = (ch - h) / 2;
		g.DrawImage(m_bmp, x, y, m_bmp->GetWidth(), m_bmp->GetHeight());
	}

	if (!imageonly) {
		CString text;
		GetWindowText(text);

		CRect rctext = client;
		rctext.InflateRect(-1, -1);
		if (selected)
			rctext.OffsetRect(1, 1);
		if (transparent)
			dc.SetBkMode(TRANSPARENT);
		if (disabled)
			dc.SetTextColor(GetSysColor(COLOR_GRAYTEXT));

		//FUTURE - do buttons need to expand tabs (DT_EXPANDTABS)?
		if (!HasAllStyles(align, DT_SINGLELINE) && HasAllStyles(align, DT_VCENTER)) {
			//N.B. - it seems like DT_VCENTER and DT_MULTILINE aren't working together, so
			//		if both are specified, manually do the vertical centering *sigh*
			// Center the text rect inside the client area.
			CRect rccalc = rctext;
			dc.DrawText(text, &rccalc, align | DT_CALCRECT);

			// Vertically offset the original rcrect, NOT rccalc
			int addoffset = (rctext.Height() - rccalc.Height()) / 2;
			rctext.OffsetRect(0, addoffset);
		}
		dc.DrawText(text, &rctext, align);
	}

	CImage* pimg = nullptr;
	if (selected)  pimg = &GetMouseOverSelImg();
	else if (m_inclient) pimg = &GetMouseOverHotImg();
	
	if (!disabled) {
		// If either mouse button is down, apply a higher alpha value to make the overlay color slightly more opaque (i.e. darker)
		BYTE alpha = ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_RBUTTON) & 0x8000)) ? 0x80 : 0x50;
		if (pimg && *pimg) pimg->AlphaBlend(dc.GetSafeHdc(), client, GetMouseOverImgRect(), alpha);
	}

	if (!disabled) {
		if (pdi->itemState & ODS_FOCUS) {
			CRect focus(client);
			focus.InflateRect(-3, -3);
			dc.DrawFocusRect(&focus);
		}
	}

	//BORDERS - REMOVE THESE AND SIMPLY CHECK FOR WS_BORDER
	//if ((style & WS_BORDER) || (exstyle & (WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)))

	//if (style & WS_BORDER)
	//{
	//	//REFACTOR - CONSIDER A LIGHT-GRAY BACKGROUND WITH MID-GRAY TEXT TO DENOTE THE DISABLED STATE
	//	//		It's kind of hard to see as it is...
	//	if (disabled)
	//		dc.SelectObject(&s_whitepen);
	//	else
	//		dc.SelectObject(&s_blackpen);

	//	dc.SelectStockObject(HOLLOW_BRUSH);
	//	dc.Rectangle(&client);
	//}

	//if (style & WS_BORDER)
	//{
	//	static CPen pen;
	//	if (!pen.GetSafeHandle())
	//		pen.CreatePen(PS_SOLID, 2, RGB(32, 64, 192));
	//	dc.SelectObject(&pen);
	//	//if (disabled)
	//	//	dc.SelectObject(&s_whitepen);
	//	//else
	//	//	dc.SelectObject(&s_blackpen);

	//	dc.SelectStockObject(HOLLOW_BRUSH);
	//	CRect rc = client;
	//	rc.InflateRect(1, 1);
	//	dc.Rectangle(&rc);
	//}

	dc.RestoreDC(dcstate);
	dc.Detach();
}

int CClearButton::OnCreate(LPCREATESTRUCT)
{
	if (m_resid != 0 && !m_restype.IsEmpty())
		SetButtonImage(AfxGetResourceHandle(), m_resid, m_restype);
	return 0;
}

HBRUSH CClearButton::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	return ODUtils::GetODCtlColorBrush(this, pDC);
}

void CClearButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	CButton::OnLButtonDown(nFlags, point);
	Invalidate();
}

void CClearButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	CButton::OnLButtonUp(nFlags, point);
	CRect client = Ui::GetClientRect(this);
	if (client.PtInRect(point)) {
		Invalidate();
		//REFACTOR - maybe ditch the NM_CLICK and use BN_COMMAND exclusively for left-clicks
		//		Is it possible that WM_NOTIFY style is workingnow that some bugs have been fixed?
		if (m_bUseNotifyForLeftClick)
			NotifyParent(this, NM_CLICK);
		else
			CommandParent(this, BN_CLICKED); //TODO - why does this trigger an ASSERT?
	}
}

void CClearButton::OnRButtonDown(UINT nFlags, CPoint point)
{
	CButton::OnRButtonDown(nFlags, point);
	Invalidate();
}

void CClearButton::OnRButtonUp(UINT nFlags, CPoint point)
{
	CButton::OnRButtonUp(nFlags, point);
	CRect client = Ui::GetClientRect(this);
	if (client.PtInRect(point)) {
		Invalidate();
		NotifyParent(this, NM_RCLICK);
	}
}

void CClearButton::OnKillFocus(CWnd* pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);
	Invalidate();
	if (::IsWindow(pNewWnd->GetSafeHwnd())) pNewWnd->Invalidate();
}

void CClearButton::OnSetFocus(CWnd* pOldWnd)
{
	CButton::OnSetFocus(pOldWnd);
	Invalidate();
	if (::IsWindow(pOldWnd->GetSafeHwnd())) pOldWnd->Invalidate();
}

void CClearButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CButton::OnMouseMove(nFlags, point);
	if (!m_tracking) {
		// Block while setting up the tracking, and if it fails, clear the blocking flag.
		m_tracking = true;
		if (!SetTrackMouseEvent(this, TME_LEAVE)) {
			m_tracking = false;
		}
		else {
			GetParent()->SendMessage(FFTTM_SHOWDESC, 0, (LPARAM)this);
		}
	}

	bool wasin = m_inclient;
	CRect client;
	GetClientRect(&client);
	m_inclient = client.PtInRect(point) == TRUE;
	if (m_inclient != wasin) {
		Invalidate();
	}
}

void CClearButton::OnMouseLeave()
{
	CButton::OnMouseLeave();
	if (m_tracking || m_inclient) Invalidate();
	m_tracking = false;
	m_inclient = false;
	GetParent()->SendMessage(FFTTM_SHOWDESC, 0, 0);
}

void CClearButton::OnDestroy()
{
	CButton::OnDestroy();
	m_tracking = false;
	m_inclient = false;
}

void CClearButton::PreSubclassWindow()
{
	CButton::PreSubclassWindow();

	//BORDERS - replace either client or static edge with a simple WS_BORDER.
	// If it's ownerdrawn AND clientedge, remove clientedge and add a border

	//if ((GetStyle() & BS_OWNERDRAW) && (GetExStyle() & WS_EX_CLIENTEDGE)) {
	//	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	//	AddThinBorder(this);
	//}

	//DEVNOTE - dialog editor doens't provide a Border property for buttons.
	//		Since I'm suporting neither client nor static edges, if either style
	//		is speciifed, remove it and set WS_BORDER instead.
	if ((GetStyle() & BS_OWNERDRAW)) {
		if (GetExStyle() & (WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)) {
			ModifyStyle(0, WS_BORDER);
			ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_DRAWFRAME);
		}
	}

	if (m_resid != 0 && !m_restype.IsEmpty())
		SetButtonImage(AfxGetResourceHandle(), m_resid, m_restype);
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

BOOL CClearButton::PreTranslateMessage(MSG* pMsg)
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

	return CButton::PreTranslateMessage(pMsg);
}

void CClearButton::OnNcPaint()
{
	//REFACTOR - allow shaded modal buttons
	//static const auto GetPen = [](CWnd& wnd) {
	//	auto id = wnd.GetDlgCtrlID();
	//	switch (id) {
	//	case IDOK: return &s_okpen;
	//	case IDCANCEL: return &s_cancelpen;
	//	case IDC_SAVE : return &s_applypen;
	//	}
	//	return &s_blackpen;
	//};

	// TODO: Add your message handler code here
	// Do not call CButton::OnNcPaint() for painting messages
	// Draw the border according to enabled or disabled state.
	if (GetStyle() & WS_BORDER) {
		CWindowDC dc(this);
		auto rc = Ui::GetNonClientRect(*this);
		//REFACTOR - allow shaded modal buttons?
		//auto pbr = !this->IsWindowEnabled() ? &s_whitepen : GetPen(*this);
		auto pbr = this->IsWindowEnabled() ? &s_blackpen : &s_whitepen;
		auto oldpen = dc.SelectObject(pbr);
		auto oldbr = dc.SelectStockObject(HOLLOW_BRUSH);
		dc.Rectangle(&rc);
		dc.SelectObject(oldbr);
		dc.SelectObject(oldpen);
	}
}

void CClearButton::OnEnable(BOOL bEnable)
{
	CButton::OnEnable(bEnable);
	Invalidate();
	RedrawWindow(nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
}
