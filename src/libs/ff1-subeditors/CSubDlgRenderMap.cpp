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

void CSubDlgRenderMap::InvalidateFrame()
{
	CRect rc{ Ui::GetClientRect(this) };
	m_frame.InvalidateRect(rc);
}

// Internal implementation

BOOL CSubDlgRenderMap::init()
{
	return TRUE;
}

int CSubDlgRenderMap::handle_scroll(UINT nBar, UINT nSBCode, UINT nPos)
{
	return Ui::HandleClientScroll(this, nBar, nSBCode, nPos, 8, 2);
}

void CSubDlgRenderMap::handle_sizing()
{
	auto client = Ui::GetClientRect(this);
	handle_sizing(client.Width(), client.Height());
}

void CSubDlgRenderMap::handle_sizing(int clientx, int clienty)
{
	const auto& state = m_frame.GetRenderState();
	auto ctlrc = Ui::GetControlRect(&m_frame);
	Ui::SetClientScroll(this, SB_HORZ, ctlrc, clientx, state.tiledims.cx + m_rmbasept.x);
	Ui::SetClientScroll(this, SB_VERT, ctlrc, clienty, state.tiledims.cy + m_rmbasept.y);
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
