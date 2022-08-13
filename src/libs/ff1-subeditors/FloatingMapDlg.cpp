// FloatingMapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "FloatingMapDlg.h"
#include "afxdialogex.h"
#include <ui_helpers.h>
#include <FFHacksterProject.h>
#include <FFBaseApp.h>


// CFloatingMapDlg dialog

IMPLEMENT_DYNAMIC(CFloatingMapDlg, CDialogEx)

CFloatingMapDlg::CFloatingMapDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FLOATING_MAP, pParent)
{
}

CFloatingMapDlg::~CFloatingMapDlg()
{
}

BEGIN_MESSAGE_MAP(CFloatingMapDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// Public implementation

void CFloatingMapDlg::SetRenderState(const sRenderMapState& state)
{
	m_subdlg.SetRenderState(state);
}

// Specifies button images for either 4 or 5 buttons.
bool CFloatingMapDlg::SetButtons(const std::vector<sMapDlgButton> & buttons)
{
	bool set = false;
	for (auto & pbtn : m_buttons) pbtn->DestroyWindow();
	m_buttons.clear();

	const UINT style = WS_CHILD | WS_VISIBLE | WS_TABSTOP |
		BS_FLAT | BS_AUTORADIOBUTTON | BS_PUSHLIKE;
	auto rc = Ui::GetControlRect(&m_buttonanchor);
	CWnd* prev = &m_buttonanchor;
	UINT ctlid = 1;
	for (const auto& b : buttons) {
		auto btn = std::make_shared<CDrawingToolButton>(b.resid, b.param);
		if (btn && btn->Create(nullptr, style, rc, this, ctlid++)) {
			btn->SetWindowPos(prev, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			m_buttons.push_back(btn);
			rc.OffsetRect(rc.Width() + 8, 0);
			prev = btn.get();
		}
	}

	ASSERT(m_buttons.size() == buttons.size());
	if (m_buttons.size() == buttons.size()) {
		//m_buttons[*State->cur_tool]->SetCheck(BST_CHECKED);
		m_buttons[0]->ModifyStyle(0, WS_GROUP);
		set = true;
	}
	else {
		AfxMessageBox("Not all of the drawing tool buttons were initialized.");
	}
	return set;
}

void CFloatingMapDlg::InvalidateMap()
{
	m_subdlg.Invalidate();
}

void CFloatingMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_RENDERMAP, m_subdlgover);
	DDX_Control(pDX, IDC_STATIC_BUTTONANCHOR, m_buttonanchor);
}


// Internal Implementation

void CFloatingMapDlg::handle_close()
{
	// Send the message to the owner (which defaults to the parent).
	CWnd* owner = GetOwner();
	CWnd* parent = GetParent();
	CWnd* wnd = owner != nullptr ? owner : parent;
	wnd->SendMessage(WMA_SHOWFLOATINGMAP, 0, (LPARAM)1);
}

void CFloatingMapDlg::handle_sizing(int clientx, int clienty)
{
	if (IsWindow(m_subdlgover.GetSafeHwnd())) {
		auto rc = Ui::GetControlRect(&m_subdlgover);
		m_subdlg.SetWindowPos(nullptr, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);
	}
}


// CFloatingMapDlg overrides and message handlers

BOOL CFloatingMapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_subdlg.CreateOverControl(this, &m_subdlgover);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CFloatingMapDlg::OnCancel()
{
	handle_close();
}

void CFloatingMapDlg::OnOK()
{
	handle_close();
}

void CFloatingMapDlg::OnClose()
{
	handle_close();
}

int CFloatingMapDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

void CFloatingMapDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	handle_sizing(cx, cy);
}

BOOL CFloatingMapDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		switch(pMsg->wParam) {
		case VK_ESCAPE:
			handle_close();
			return TRUE;
		case VK_F6:
			GetParent()->SetActiveWindow();
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
