// SubDlgDynaButtons.cpp : implementation file
//

#include "stdafx.h"
#include "SubDlgDynaButtons.h"
#include "afxdialogex.h"
#include "resource.h"
#include <ui_helpers.h>
#include <window_messages.h>

#define DLG_ID IDD_SUB_DYNABUTTONS
#define IDC_DYNA IDC_DYNABUTTON_EDITORS
#define DYNA_LAST_ID (IDC_DYNABUTTON_EDITORS + 1000)

// CSubDlgDynaButtons dialog

IMPLEMENT_DYNAMIC(CSubDlgDynaButtons, CDialogEx)

CSubDlgDynaButtons::CSubDlgDynaButtons(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SUB_DYNABUTTONS, pParent)
{
}

CSubDlgDynaButtons::~CSubDlgDynaButtons()
{
}

void CSubDlgDynaButtons::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSubDlgDynaButtons, CDialogEx)
	ON_WM_SIZE()
	ON_MESSAGE(FFTTM_SHOWDESC, &CSubDlgDynaButtons::OnFfttmShowDesc)
	ON_NOTIFY_RANGE(NM_RCLICK, IDC_DYNA, DYNA_LAST_ID, &CSubDlgDynaButtons::OnNmRclickActionButton)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_DYNA, DYNA_LAST_ID, &CSubDlgDynaButtons::OnBnClickedActionButton)
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BOOL CSubDlgDynaButtons::CreateOverControl(CWnd* pctltoreplace)
{
	auto parent = pctltoreplace->GetParent();
	ASSERT(parent != nullptr);
	if (Create(DLG_ID, parent)) {
		m_buttonfont = GetFont();
		if (Ui::SeatControl(this, pctltoreplace))
			return TRUE;
	}
	return FALSE;
}

void CSubDlgDynaButtons::UseFixedButtonSize(CSize size)
{
	m_usedynabuttonsize = false;
	m_fixedbuttonsize = size;
}

void CSubDlgDynaButtons::UseDynamicButtonSize()
{
	m_usedynabuttonsize = true;
}

void CSubDlgDynaButtons::SetButtonFont(CFont* font)
{
	m_buttonfont = font != nullptr ? font : GetFont();
}

namespace {
	const UINT styles = WS_VISIBLE | WS_BORDER | WS_TABSTOP | 
		BS_FLAT | BS_OWNERDRAW | BS_CENTER | BS_VCENTER;
	const CSize margin = { 4,4 };
	const CSize padding = { 16,16 };
}

void CSubDlgDynaButtons::Add(UINT id, int eventindex, CString text)
{
	//TODO - handle variable width buttons
	//	maybe make this an option in app settings
	//DEVNOTE - if we allow variable widths, then we'll likely need
	//		to specify padding here.

	// Add after the most recent window
	// Move to a new row if it's beyond the bounds of the right client rect edge
	CRect client = Ui::GetClientRect(this);
	CRect rcbutton;
	auto pwndlast = get_last_child();

	if (pwndlast != nullptr) {
		auto rclast = Ui::GetControlRect(pwndlast);
		rcbutton = rclast;
		bool horzok = (rcbutton.right + rclast.Width() + margin.cx) < client.right;
		int dx = horzok ? rclast.Width() + margin.cx : -rclast.left + m_buttonorg.x;
		int dy = horzok ? 0 : rclast.Height() + margin.cy;
		rcbutton.OffsetRect(dx, dy);
	}
	else {
		// This is the first button, establish the origin
		if (m_usedynabuttonsize) {
			rcbutton = { margin.cx, margin.cy, margin.cx, margin.cy };
			m_buttonorg = rcbutton.TopLeft();
		}
		else {
			auto bpr = client.Width() / (m_fixedbuttonsize.cx + margin.cx);
			auto extent = bpr * (m_fixedbuttonsize.cx + margin.cx);
			auto diff = client.Width() - extent;
			rcbutton = { diff / 2, margin.cy, diff / 2, margin.cy };
			m_buttonorg = rcbutton.TopLeft();
		}
	}

	rcbutton.right = rcbutton.left + m_fixedbuttonsize.cx;
	rcbutton.bottom = rcbutton.top + m_fixedbuttonsize.cy;

	auto newbutton = new CClearButton();
	newbutton->Create(text, styles, rcbutton, this, id);
	newbutton->SetFont(m_buttonfont);
	newbutton->SetWindowPos(pwndlast, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	newbutton->ModifyStyleEx(0, WS_EX_TRANSPARENT);
	SetWindowLongPtr(newbutton->GetSafeHwnd(), GWLP_USERDATA, eventindex);
	m_buttons.push_back(newbutton);

	if (!m_supresslayout) handle_size();
}

void CSubDlgDynaButtons::ClearButtons()
{
	for (CWnd* pbtn : m_buttons) {
		pbtn->DestroyWindow();
		delete pbtn;
	}
	m_buttons.clear();
}

void CSubDlgDynaButtons::SuppressLayout(bool suppress)
{
	auto was = m_supresslayout;
	m_supresslayout = suppress;
	if (!m_supresslayout && was)
		handle_size();
}

bool CSubDlgDynaButtons::SuppressingLayout() const
{
	return m_supresslayout;
}


// Internal implementation

CWnd* CSubDlgDynaButtons::get_last_child()
{
	auto child = this->GetWindow(GW_CHILD);
	auto pwndlast = child != nullptr ? child->GetNextWindow(GW_HWNDLAST) : nullptr;
	return pwndlast;
}

void CSubDlgDynaButtons::handle_size()
{
	auto client = Ui::GetClientRect(this);
	handle_size(client.Width(), client.Height());
}

void CSubDlgDynaButtons::handle_size(int clientx, int clienty)
{
	auto plast = get_last_child();
	auto bottom = (plast != nullptr) ? Ui::GetControlRect(plast).bottom : 0;
	CRect area = { 0, 0, 1, bottom + margin.cy };
	bool shown = Ui::SetClientScroll(this, SB_VERT, area, clienty);
	if (shown)
		this->ModifyStyle(0, WS_BORDER, SWP_DRAWFRAME);
	else
		this->ModifyStyle(WS_BORDER, 0, SWP_DRAWFRAME);
}

void CSubDlgDynaButtons::slide_buttons(int amount)
{
	for (auto pbtn : m_buttons) {
		Ui::MoveControlBy(pbtn, 0, amount);
	}
}


// CSubDlgDynaButtons message handlers

void CSubDlgDynaButtons::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (IsWindow(this->GetSafeHwnd()))
		handle_size(cx, cy);
}

void CSubDlgDynaButtons::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int oldpos = GetScrollPos(SB_VERT);
	int curpos = Ui::HandleClientScroll(this, SB_VERT, nSBCode, nPos);
	slide_buttons(oldpos - curpos); // inverted, negative value scrolls controls up
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
	if (nSBCode == SB_PAGEUP) InvalidateRect(nullptr);
}

BOOL CSubDlgDynaButtons::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// invert the wheel so scrolling toward user moves down the list
	auto delta = -zDelta / WHEEL_DELTA;
	if (delta != 0) {
		SCROLLINFO info;
		GetScrollInfo(SB_VERT, &info, SIF_ALL);
		int oldpos = info.nPos;
		int newpos = oldpos + delta;
		int max = info.nMax - info.nPage + 1;
		if (newpos < info.nMin) newpos = info.nMin;
		if (newpos > max) newpos = max;
		int diff = oldpos - newpos;
		if (diff != 0) {
			SetScrollPos(SB_VERT, newpos);
			slide_buttons(oldpos - newpos);
		}
	}
	return FALSE;
	//return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

LRESULT CSubDlgDynaButtons::OnFfttmShowDesc(WPARAM wparam, LPARAM lparam)
{
	return GetParent()->SendMessage(FFTTM_SHOWDESC, wparam, lparam);
}

void CSubDlgDynaButtons::OnNmRclickActionButton(UINT id, NMHDR* pNotify, LRESULT* result)
{
	UNREFERENCED_PARAMETER(pNotify);
	auto btn = GetDlgItem(id);
	auto index = GetWindowLongPtr(btn->GetSafeHwnd(), GWLP_USERDATA);
	GetParent()->SendMessage(FFEDIT_RCLICK, index);
	*result = 0;
}

void CSubDlgDynaButtons::OnBnClickedActionButton(UINT id)
{
	auto btn = GetDlgItem(id);
	auto index = GetWindowLongPtr(btn->GetSafeHwnd(), GWLP_USERDATA);
	GetParent()->SendMessage(FFEDIT_LCLICK, index);
}

BOOL CSubDlgDynaButtons::OnEraseBkgnd(CDC* pDC)
{
	auto rc = Ui::GetClientRect(this);
	pDC->FillSolidRect(rc, RGB(255, 255, 255));
	return TRUE;
}