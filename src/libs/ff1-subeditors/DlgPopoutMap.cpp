// SubDlgPopoutMap.cpp : implementation file
//

#include "stdafx.h"
#include "DlgPopoutMap.h"
#include "resource_subeditors.h"
#include "afxdialogex.h"
#include "IMapEditor.h"
#include <FFBaseApp.h>
#include <ui_helpers.h>
#include <algorithm>
#include <afxglobals.h>

//DUPE - IDCR_TOOLBTN
#define IDCR_TOOLBTN 1

// CDlgPopoutMap dialog

IMPLEMENT_DYNAMIC(CDlgPopoutMap, CDialogEx)

CDlgPopoutMap::CDlgPopoutMap(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_POPOUT_MAP, pParent)
{
}

CDlgPopoutMap::~CDlgPopoutMap()
{
}

BEGIN_MESSAGE_MAP(CDlgPopoutMap, CDialogEx)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_NCHITTEST()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_CUSTOMTOOL, &CDlgPopoutMap::OnClickedCustomtool)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_MAP, &CDlgPopoutMap::OnClickedButtonImportMap)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_MAP, &CDlgPopoutMap::OnClickedButtonExportMap)
	ON_MESSAGE(WMA_DRAWTOOLBNCLICK, &CDlgPopoutMap::OnDrawToolBnClick)
	ON_WM_ERASEBKGND()
	ON_WM_EXITSIZEMOVE()
END_MESSAGE_MAP()

void CDlgPopoutMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DISPLAYAREA, m_displaystatic);
	DDX_Control(pDX, IDC_STATIC_BUTTONANCHOR, m_buttonanchor);
	DDX_Control(pDX, IDC_CUSTOMTOOL, m_customizebutton);
	DDX_Control(pDX, IDC_SB_HORZ, m_hscroll);
	DDX_Control(pDX, IDC_SB_VERT, m_vscroll);
}

BOOL CDlgPopoutMap::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_ddxdone = true;
	return TRUE;
}


// Public implementation

BOOL CDlgPopoutMap::CreateModeless(IMapEditor* editor, CSize mapdims, CSize tiledims, CWnd* parent)
{
	ASSERT(editor != nullptr);
	ASSERT(mapdims.cx > 0);
	ASSERT(mapdims.cy > 0);
	ASSERT(tiledims.cx > 0);
	ASSERT(tiledims.cy > 0);

	if (editor != nullptr) {
		if (Create(IDD_POPOUT_MAP, parent)) {
			Editor = editor;
			Mapsize = mapdims;
			Tilesize = tiledims;
			return TRUE;
		}
	}
	return FALSE;
}

void CDlgPopoutMap::UpdateControls()
{
	update_tool_index();
}

void CDlgPopoutMap::ScrollByPercentage(int nBar, int percent)
{
	ASSERT(is_valid());
	if (!is_valid()) return;

	// This is a percentage of the map size, NOT the scroll offset.
	CScrollBar* bar = nullptr;
	auto rcdisp = get_display_area();
	int halfdisp = 0;
	int mapextent = 0;
	if (nBar == SB_HORZ) {
		bar = &m_hscroll;
		halfdisp = (rcdisp.Width() / 2);
		mapextent = Mapsize.cx * Tilesize.cx;
	}
	else {
		ASSERT(nBar == SB_VERT);
		bar = &m_vscroll;
		halfdisp = (rcdisp.Height() / 2);
		mapextent = Mapsize.cy * Tilesize.cy;
	}

	if (bar != nullptr) {
		SCROLLINFO info;
		bar->GetScrollInfo(&info, SIF_ALL);
		int limit = info.nMax;
		int mappos = percent * mapextent / 100;
		int diff = mapextent - limit;
		int newpos = mappos - diff;

		newpos += halfdisp; // center on the clicked point if possible
		if (newpos < 0)
			newpos = 0;
		if (newpos > limit)
			newpos = limit;

		bar->SetScrollPos(newpos);
		invalidate_display_area();
		Editor->HandleAfterScroll(get_scroll_pos(), rcdisp);
	}
}

void CDlgPopoutMap::ScrollToPos(int nBar, int mappos, bool center)
{
	ASSERT(is_valid());
	if (!is_valid()) return;

	CScrollBar* bar = nullptr;
	auto rcdisp = get_display_area();
	int halfdisp = 0;
	if (nBar == SB_HORZ) {
		bar = &m_hscroll;
		halfdisp = (rcdisp.Width() / 2);
	}
	else {
		ASSERT(nBar == SB_VERT);
		bar = &m_vscroll;
		halfdisp = (rcdisp.Height() / 2);
	}

	if (bar != nullptr) {
		SCROLLINFO info;
		bar->GetScrollInfo(&info, SIF_ALL);
		int limit = info.nMax;
		int newpos = center ? mappos - halfdisp : mappos;

		if (newpos < 0)
			newpos = 0;
		if (newpos > limit)
			newpos = limit;

		bar->SetScrollPos(newpos);
		invalidate_display_area();
		Editor->HandleAfterScroll(get_scroll_pos(), rcdisp);
	}
}

CPoint CDlgPopoutMap::GetMapPos() const
{
	auto rc = get_display_area();
	auto scrpos = get_scroll_pos();
	CPoint pos{
		scrpos.x + (rc.Width()/2),
		scrpos.y + (rc.Height()/2)
	};
	return pos;
}

CPoint CDlgPopoutMap::GetScrollOffset() const
{
	return get_scroll_pos();
}

CSize CDlgPopoutMap::GetScrollLimits() const
{
	return CSize{
		m_hscroll.GetScrollLimit(),
		m_vscroll.GetScrollLimit()
	};
}

CSize CDlgPopoutMap::GetTileDims() const
{
	return Tilesize;
}

CSize CDlgPopoutMap::GetMiniMapDims() const
{
	// This is the visible area divided by tiledims
	auto rcdisp = get_display_area();
	CSize sz = {
		rcdisp.Width() / Tilesize.cx,
		rcdisp.Height() / Tilesize.cy
	};
	return sz;
}

//CPoint CDlgPopoutMap::GetScrollPercentages() const
//{
//	//CSize extent = { Mapsize.cx * Tilesize.cx, Mapsize.cy * Tilesize.cy };
//	CSize extent = { m_hscroll.GetScrollLimit(), m_vscroll.GetScrollLimit() };
//	CPoint pos = get_scroll_pos();
//	CSize pct{
//		pos.x * 100 / extent.cx,
//		pos.y * 100 / extent.cy
//	};
//	return pct;
//}

bool CDlgPopoutMap::SetButtons(const std::vector<sMapDlgButton>& buttons)
{
	return set_buttons(buttons);
}


// Internal implementation

bool CDlgPopoutMap::is_valid() const
{
	return
		(Editor != nullptr) &&
		(Mapsize.cx > 0 && Mapsize.cy > 0) &&
		(Tilesize.cx > 0 && Tilesize.cy > 0);
}

bool CDlgPopoutMap::set_buttons(const std::vector<sMapDlgButton>& buttons)
{
	bool set = false;
	for (auto& pbtn : m_buttons) pbtn->DestroyWindow();
	m_buttons.clear();

	const UINT style = WS_CHILD | WS_VISIBLE | WS_TABSTOP |
		BS_FLAT | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_NOTIFY;
	auto rc = Ui::GetControlRect(&m_buttonanchor);
	CWnd* prev = &m_buttonanchor;
	UINT ctlid = IDCR_TOOLBTN;
	for (const auto& b : buttons) {
		auto btn = std::make_shared<CDrawingToolButton>(b.resid, b.param);
		if (btn && btn->Create(nullptr, style, rc, this, ctlid++)) {
			btn->SetWindowPos(prev, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			m_buttons.push_back(btn);
			rc.OffsetRect(rc.Width() + 4, 0);
			prev = btn.get();
		}
	}

	// Place the Customize button 4 px right of the last button
	m_customizebutton.SetWindowPos(nullptr, rc.left, rc.top, 0, 0,
		SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);

	ASSERT(m_buttons.size() == buttons.size());
	if (m_buttons.size() == buttons.size()) {
		m_buttons[0]->ModifyStyle(0, WS_GROUP);
		update_tool_index();
		set = true;
	}
	else {
		AfxMessageBox(_T("Not all of the drawing tool buttons were initialized."));
	}
	return set;
}

void CDlgPopoutMap::update_tool_index()
{
	auto index = Editor->GetCurrentToolIndex();
	auto iter = std::find_if(cbegin(m_buttons), cend(m_buttons),
		[index](const std::shared_ptr<CDrawingToolButton>& btn) {
			return btn->GetToolIndex() == index;
		});
	if (iter != cend(m_buttons)) {
		auto idcheck = (*iter)->GetDlgCtrlID();
		CheckRadioButton(IDCR_TOOLBTN, IDCR_TOOLBTN + (UINT)m_buttons.size(), idcheck);
		m_customizebutton.EnableWindow(index > 1);
	}
}

CRect CDlgPopoutMap::get_sizer_rect(bool client)
{
	auto rc = client ? Ui::GetClientRect(this) : Ui::GetWindowRect(this);
	rc.left = rc.right - GetSystemMetrics(SM_CXVSCROLL);
	rc.top = rc.bottom - GetSystemMetrics(SM_CYHSCROLL);
	return rc;
}

CRect CDlgPopoutMap::GetDisplayArea() const
{
	return get_display_area();
}

CPoint CDlgPopoutMap::get_scroll_pos() const
{
	CPoint pt{ m_hscroll.GetScrollPos(), m_vscroll.GetScrollPos() };
	return pt;
}

CRect CDlgPopoutMap::get_display_area() const
{
	auto rc = Ui::GetControlRect(const_cast<CStatic*>(&m_displaystatic));
	return rc;
}

CPoint CDlgPopoutMap::fix_map_point(CPoint point)
{
	auto rcMap = this->get_display_area();
	auto scrpos = get_scroll_pos();
	CPoint pt{
		((point.x - rcMap.left) + scrpos.x) / Tilesize.cx,
		((point.y - rcMap.top) + scrpos.y) / Tilesize.cy
	};
	return pt;
}

void CDlgPopoutMap::invalidate_display_area()
{
	auto rc = get_display_area();
	InvalidateRect(rc);
}

void CDlgPopoutMap::handle_close()
{
	// Send the message to the owner (which defaults to the parent).
	CWnd* owner = GetOwner();
	CWnd* parent = GetParent();
	CWnd* wnd = owner != nullptr ? owner : parent;
	wnd->SendMessage(WMA_SHOWFLOATINGMAP, (WPARAM)0);
}

void CDlgPopoutMap::handle_sizing(int clientx, int clienty)
{
	if (!m_ddxdone) return;

	InvalidateRect(nullptr);

	CRect maparea{ 0, 0, Mapsize.cx * Tilesize.cx, Mapsize.cy * Tilesize.cy };
	auto client = Ui::GetControlRect(&m_displaystatic);
	Ui::SetContainedScroll(&m_hscroll, SB_HORZ, maparea, client.Width(), Tilesize.cx);
	Ui::SetContainedScroll(&m_vscroll, SB_VERT, maparea, client.Height(), Tilesize.cy);
}


// CDlgPopoutMap message handlers

void CDlgPopoutMap::OnCancel()
{
	handle_close();
}

void CDlgPopoutMap::OnOK()
{
	handle_close();
}

BOOL CDlgPopoutMap::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
		case VK_ESCAPE:
		case VK_F7:
			handle_close();
			return TRUE;
		case VK_F6:
			GetParent()->SetActiveWindow();
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

int CDlgPopoutMap::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	//ELJNOTE - I give up, another reason why Win32 Desktop UI is so terrible.
	// 		this is waaaaaaay too much work to get just have an X close button
	//		without a system menu.
	// 		Removing system menu items also removes corresponding SC_XXXX commands,
	// 		e.g. removing the SC_MOVE menu item ("Move") disables the ability
	//			to move the dialog using the caption bar.
	// 		I'll just specify the app icon here and call it a day.
	SetIcon(AfxGetFFBaseApp()->GetLargeIcon(), TRUE);
	return 0;
}

void CDlgPopoutMap::OnClose()
{
	handle_close();
}

void CDlgPopoutMap::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	handle_sizing(cx, cy);
}

LRESULT CDlgPopoutMap::OnNcHitTest(CPoint point)
{
	auto rc = get_sizer_rect(false);
	if (rc.PtInRect(point))
		return HTBOTTOMRIGHT;
	return CDialogEx::OnNcHitTest(point);
}

void CDlgPopoutMap::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialogEx::OnPaint() for painting messages

	auto client = Ui::GetClientRect(this);
	auto displayarea = Ui::GetControlRect(&m_displaystatic);

	// SIGH.... to reduce flicker, use offscreen drawing.
	// Create a compatible bitmap, select it into a compatible DC.
	// "Compatible" here means compatible with the color format of the paint DC.
	// This is an awful lot of boilerplate for a relatively simple operation.
	CDC compat;
	compat.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, displayarea.Width(), displayarea.Height());
	auto oldbmp = compat.SelectObject(&bmp);

	// Erase the background, then tell the attached editor to render the map.
	compat.FillSolidRect(client, RGB(255, 255, 255));
	auto scrolloff = get_scroll_pos();
	CRect drawrect = { 0, 0, displayarea.Width(), displayarea.Height() };
	Editor->RenderMapEx(compat, drawrect, scrolloff, Tilesize);

	// Draw the bitmap to the window's display area.
	// To ensure we only draw to that area, set it as the clip region.
	CRgn rgn;
	rgn.CreateRectRgn(displayarea.left, displayarea.top, displayarea.right, displayarea.bottom);
	compat.SelectClipRgn(&rgn);

	auto save = dc.SaveDC();
	dc.BitBlt(displayarea.left, displayarea.top, displayarea.Width(), displayarea.Height(),
		&compat,
		0, 0,
		SRCCOPY);
	dc.RestoreDC(save);
	compat.SelectObject(oldbmp);

	//TODO - might remove this frame border
	displayarea.InflateRect(1, 1);
	dc.Draw3dRect(displayarea, RGB(0, 0, 0), RGB(0, 0, 0));

	auto rc = get_sizer_rect(true);
	dc.DrawFrameControl(rc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
}

void CDlgPopoutMap::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Ui::HandleContainedScroll(&m_hscroll, SB_HORZ, nSBCode, nPos, 8, 2);
	invalidate_display_area();
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	Editor->HandleAfterScroll(get_scroll_pos(), get_display_area());
}

void CDlgPopoutMap::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Ui::HandleContainedScroll(&m_vscroll, SB_VERT, nSBCode, nPos, 8, 2);
	invalidate_display_area();
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
	Editor->HandleAfterScroll(get_scroll_pos(), get_display_area());
}

void CDlgPopoutMap::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialogEx::OnLButtonDown(nFlags, point);
	if (!is_valid()) return;

	Editor->SetMouseDown(0);
	auto pt = fix_map_point(point);
	Editor->HandleLButtonDown(nFlags, pt);
	invalidate_display_area();
}

void CDlgPopoutMap::OnLButtonUp(UINT nFlags, CPoint point)
{
	ASSERT(is_valid());
	CDialogEx::OnLButtonUp(nFlags, point);
	if (!is_valid()) return;

	auto pt = fix_map_point(point);
	Editor->HandleLButtonUp(nFlags, pt);
	invalidate_display_area();
	Editor->SetMouseDown(0);
}

void CDlgPopoutMap::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	ASSERT(is_valid());
	CDialogEx::OnLButtonDblClk(nFlags, point);
	if (!is_valid()) return;

	auto pt = fix_map_point(point);
	Editor->HandleLButtonDblClk(nFlags, pt);
	invalidate_display_area();
}

void CDlgPopoutMap::OnRButtonDown(UINT nFlags, CPoint point)
{
	CDialogEx::OnRButtonDown(nFlags, point);
	if (!is_valid()) return;

	Editor->SetMouseDown(0);
	auto pt = fix_map_point(point);
	Editor->HandleRButtonDown(nFlags, pt);
	invalidate_display_area();
}

void CDlgPopoutMap::OnRButtonUp(UINT nFlags, CPoint point)
{
	ASSERT(is_valid());
	CDialogEx::OnRButtonUp(nFlags, point);
	if (!is_valid()) return;

	auto pt = fix_map_point(point);
	Editor->HandleRButtonUp(nFlags, pt);
	invalidate_display_area();
	Editor->SetMouseDown(0);
}

void CDlgPopoutMap::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	ASSERT(is_valid());
	CDialogEx::OnRButtonDblClk(nFlags, point);
	if (!is_valid()) return;

	auto pt = fix_map_point(point);
	Editor->HandleRButtonDblClk(nFlags, pt);
	invalidate_display_area();
}

void CDlgPopoutMap::OnMouseMove(UINT nFlags, CPoint point)
{
	CDialogEx::OnMouseMove(nFlags, point);
	if (!is_valid()) return;

	auto pt = fix_map_point(point);
	Editor->HandleMouseMove(nFlags, pt);

	if (Editor->GetMouseDown()) {
		auto rcwnd = Ui::GetControlRect(&m_displaystatic);
		if (!rcwnd.PtInRect(point)) {
			Editor->SetMouseDown(0);
		}
		invalidate_display_area();
	}
}

void CDlgPopoutMap::OnClickedCustomtool()
{
	if (Editor != nullptr)
		Editor->HandleCustomizeTool();
}

void CDlgPopoutMap::OnClickedButtonImportMap()
{
	if (Editor != nullptr)
		Editor->HandleMapImport();
}

void CDlgPopoutMap::OnClickedButtonExportMap()
{
	if (Editor != nullptr)
		Editor->HandleMapExport();
}

LRESULT CDlgPopoutMap::OnDrawToolBnClick(WPARAM wparam, LPARAM lparam)
{
	// The button sends it to this control (its parent), forward it
	// to the map editor (the parent of this dialog).
	UNREFERENCED_PARAMETER(wparam);
	GetParent()->SendMessage(WMA_DRAWTOOLBNCLICK, wparam, lparam);
	m_customizebutton.EnableWindow(lparam > 1);
	return 0;
}

BOOL CDlgPopoutMap::OnEraseBkgnd(CDC* pDC)
{
	auto client = Ui::GetClientRect(this);
	auto rcdisp = get_display_area();

	// The displayarea's background is erased during offscreen drawing.
	// Build a region that excludes the display rect.
	CRgn rgn, rgdisp;
	rgn.CreateRectRgn(client.left, client.top, client.right, client.bottom);
	rgdisp.CreateRectRgn(rcdisp.left, rcdisp.top, rcdisp.right, rcdisp.bottom);
	
	// Erase the rest of the client area to the background color.
	auto save = pDC->SaveDC();
	pDC->SelectClipRgn(&rgn);
	pDC->SelectClipRgn(&rgdisp, RGN_DIFF); // exclude the displayarea
	pDC->FillSolidRect(client, RGB(255, 255, 255));
	pDC->RestoreDC(save);
	return TRUE;
}

void CDlgPopoutMap::OnExitSizeMove()
{
	// We only want this update to happen once, after sizing has finished.
	CDialogEx::OnExitSizeMove();
	InvalidateRect(nullptr);
	Editor->HandleAfterScroll(get_scroll_pos(), get_display_area());
}
