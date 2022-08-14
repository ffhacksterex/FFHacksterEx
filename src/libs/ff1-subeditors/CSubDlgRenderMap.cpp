// CSubDlgRenderMap.cpp : implementation file
// Handle scrolling here.

#include "stdafx.h"
#include "resource_subeditors.h"
#include "CSubDlgRenderMap.h"
#include "afxdialogex.h"
#include <ui_helpers.h>

// CSubDlgRenderMap dialog

#define DLGID IDD_SUBDLG_RENDERMAP

IMPLEMENT_DYNAMIC(CSubDlgRenderMap, CDialogEx)

CSubDlgRenderMap::CSubDlgRenderMap(CWnd* pParent /*=nullptr*/)
	: CDialogEx(DLGID, pParent)
{
}

CSubDlgRenderMap::~CSubDlgRenderMap()
{
}

void CSubDlgRenderMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_FRAME, m_frame);
}

BEGIN_MESSAGE_MAP(CSubDlgRenderMap, CDialogEx)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// Create this subdialog and position over the bounds of pOver.
// pOver is typically a static or frame used as a placeholder on dialogs
// to set up the position of this subdialog.
BOOL CSubDlgRenderMap::CreateOverControl(CWnd* pParent, CWnd* pOver)
{
	ASSERT(pParent != nullptr);
	ASSERT(pOver != nullptr);
	if (pParent != nullptr && pOver != nullptr) {
		ASSERT(pOver->GetParent() == pParent);
		if (Create(DLGID, pParent)) {
			auto rc = Ui::GetControlRect(pOver);
			SetWindowPos(pOver, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);
			return TRUE;
		}
	}
	return FALSE;
}

void CSubDlgRenderMap::SetRenderState(const sRenderMapState& state)
{
	m_frame.SetRenderState(state);
	handle_sizing();
}

const sRenderMapState& CSubDlgRenderMap::GetRenderState() const
{
	return m_frame.GetRenderState();
}

// Internal implementation

BOOL CSubDlgRenderMap::init()
{
	return TRUE;
}

int CSubDlgRenderMap::handle_scroll(UINT nBar, UINT nSBCode, UINT nPos)
{
	int curpos = GetScrollPos(nBar);
	int limit = GetScrollLimit(nBar);
	switch (nSBCode)
	{
	case SB_LEFT: // == SB_TOP
		curpos = 0;
		break;
	case SB_RIGHT: // == SB_BOTTOM
		curpos = limit;
		break;
	case SB_ENDSCROLL:
		break;
	case SB_LINELEFT: // == SB_LINEUP
		if (curpos > 0) --curpos;
		break;
	case SB_LINERIGHT: // == SB_LINEDOWN
		if (curpos < limit) ++curpos;
		break;
	case SB_PAGELEFT: // == SB_PAGEUP
	{
		SCROLLINFO info;
		GetScrollInfo(nBar, &info, SIF_ALL);
		if (curpos > 0) curpos = max(0, curpos - (int)info.nPage);
	}
	break;
	case SB_PAGERIGHT: // == SB_PAGEDOWN
	{
		SCROLLINFO info;
		GetScrollInfo(nBar, &info, SIF_ALL);
		if (curpos < limit) curpos = min(limit, curpos + (int)info.nPage);
	}
	break;
	case SB_THUMBPOSITION:
		curpos = nPos;	// nPos is the absolute position at the end of the drag.
		break;
	case SB_THUMBTRACK:
		curpos = nPos;  // nPos is the specified position where the scroll box is currently.
		break;          // This occurs when dragging the scroll box with the mouse.
	}

	SetScrollPos(nBar, curpos);
	return curpos;
}

void CSubDlgRenderMap::handle_sizing()
{
	auto client = Ui::GetClientRect(this);
	handle_sizing(client.Width(), client.Height());
}

void CSubDlgRenderMap::handle_sizing(int clientx, int clienty)
{
	//DEVNOTE - I went with a static local since other methods don't need to call this.
	static const auto local_set_scroll = [](CWnd* wnd, int nBar, const CRect & ctlrc,
		int extent, int tilespan)
	{
		// Does the control's extent exceeds the host client area's extent?
		int ctlw = nBar == SB_HORZ ? ctlrc.Width() : ctlrc.Height();
		if (ctlw > extent) {
			// Yes, so show the bar and set it's range
			int bump = nBar == SB_HORZ ? GetSystemMetrics(SM_CXVSCROLL) : GetSystemMetrics(SM_CYHSCROLL);
			//auto scmax = (ctlw - extent) + 16; //TODO - add getter for tile dims to m_rendermap
			auto scmax = (ctlw - extent) + bump + tilespan;
			wnd->EnableScrollBarCtrl(nBar, TRUE);
			SCROLLINFO si = { 0 };
			wnd->GetScrollInfo(nBar, &si, SIF_ALL);
			si.nPage = 16;
			si.nMin = 0;
			si.nMax = scmax;
			si.nPos = (si.nPos > scmax) ? scmax : si.nPos;
			wnd->SetScrollInfo(nBar, &si, SIF_ALL);
		} else {
			wnd->EnableScrollBarCtrl(nBar, FALSE);
		}
	};

	const auto & state = m_frame.GetRenderState();
	auto ctlrc = Ui::GetControlRect(&m_frame);
	local_set_scroll(this, SB_HORZ, ctlrc, clientx, state.tiledims.cx + m_rmbasept.x);
	local_set_scroll(this, SB_VERT, ctlrc, clienty, state.tiledims.cy + m_rmbasept.y);
}



// CSubDlgRenderMap overrides and message handlers

void CSubDlgRenderMap::PreSubclassWindow()
{
	VERIFY(init());
	CDialogEx::PreSubclassWindow();
}

BOOL CSubDlgRenderMap::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	auto ctlrc = Ui::GetControlRect(&m_frame);
	m_rmbasept = ctlrc.TopLeft();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

int CSubDlgRenderMap::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return init() ? 0 : -1;
}

void CSubDlgRenderMap::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int curpos = handle_scroll(SB_HORZ, nSBCode, nPos);
	int newpos = m_rmbasept.x - curpos;
	auto ctlpos = Ui::GetControlRect(&m_frame);
	m_frame.SetWindowPos(nullptr, newpos, ctlpos.top, -1, -1, SWP_NOZORDER | SWP_NOSIZE);
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSubDlgRenderMap::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int curpos = handle_scroll(SB_VERT, nSBCode, nPos);
	int newpos = m_rmbasept.y - curpos;
	auto ctlpos = Ui::GetControlRect(&m_frame);
	m_frame.SetWindowPos(nullptr, ctlpos.left, newpos, -1, -1, SWP_NOZORDER | SWP_NOSIZE);
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSubDlgRenderMap::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (IsWindow(m_frame.GetSafeHwnd())) {
		handle_sizing(cx, cy);
	}
}
