// FloatingMapDlg.cpp : implementation file

#include "stdafx.h"
#include "resource_subeditors.h"
#include "FloatingMapDlg.h"
#include "afxdialogex.h"
#include "IMapEditor.h"
#include <ui_helpers.h>
#include <FFHacksterProject.h>
#include <FFBaseApp.h>

#define IDCR_TOOLBTN 1

// CFloatingMapDlg dialog

IMPLEMENT_DYNAMIC(CFloatingMapDlg, CDialogEx)

CFloatingMapDlg::CFloatingMapDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FLOATING_MAP, pParent)
{
}

CFloatingMapDlg::~CFloatingMapDlg()
{
}

void CFloatingMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_RENDERMAP, m_subdlgover);
	DDX_Control(pDX, IDC_STATIC_BUTTONANCHOR, m_buttonanchor);
	DDX_Control(pDX, IDC_CUSTOMTOOL, m_customizebutton);
}

BEGIN_MESSAGE_MAP(CFloatingMapDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WMA_DRAWTOOLBNCLICK, &CFloatingMapDlg::OnDrawToolBnClick)
	ON_BN_CLICKED(IDC_CUSTOMTOOL, &CFloatingMapDlg::OnClickedCustomtool)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_MAP, &CFloatingMapDlg::OnClickedButtonImportMap)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_MAP, &CFloatingMapDlg::OnClickedButtonExportMap)
	ON_WM_NCHITTEST()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// Public implementation

BOOL CFloatingMapDlg::Init(const sRenderMapState& state, const std::vector<sMapDlgButton>& buttons)
{
	if (set_render_state(state)) {
		if (set_buttons(buttons))
			return TRUE;
	}
	return FALSE;
}

void CFloatingMapDlg::UpdateControls()
{
	UpdateToolIndex();
}

void CFloatingMapDlg::InvalidateMap()
{
	m_subdlg.InvalidateFrame();
}

void CFloatingMapDlg::UpdateToolIndex()
{
	auto index = *m_subdlg.GetRenderState().cur_tool;
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


// Internal Implementation

bool CFloatingMapDlg::set_render_state(const sRenderMapState& state)
{
	m_subdlg.SetRenderState(state);
	return m_subdlg.GetRenderState().IsValid();
}

// Specifies button images for either 4 or 5 buttons.
bool CFloatingMapDlg::set_buttons(const std::vector<sMapDlgButton>& buttons)
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
		UpdateToolIndex();
		set = true;
	}
	else {
		AfxMessageBox(_T("Not all of the drawing tool buttons were initialized."));
	}
	return set;
}

void CFloatingMapDlg::handle_close()
{
	// Send the message to the owner (which defaults to the parent).
	CWnd* owner = GetOwner();
	CWnd* parent = GetParent();
	CWnd* wnd = owner != nullptr ? owner : parent;
	wnd->SendMessage(WMA_SHOWFLOATINGMAP, (WPARAM)0);
}

CRect CFloatingMapDlg::get_sizer_rect(bool client)
{
	auto rc = client ? Ui::GetClientRect(this) : Ui::GetWindowRect(this);
	rc.left = rc.right - GetSystemMetrics(SM_CXVSCROLL);
	rc.top = rc.bottom - GetSystemMetrics(SM_CYHSCROLL);
	return rc;
}

void CFloatingMapDlg::handle_sizing(int clientx, int clienty)
{
	if (IsWindow(m_subdlgover.GetSafeHwnd())) {
		auto rc = Ui::GetControlRect(&m_subdlgover);
		m_subdlg.SetWindowPos(nullptr, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);

		auto rcsizeh = get_sizer_rect(true);
		auto rcsizev = rcsizeh;
		rcsizeh.left = 0;
		rcsizev.top = 0;
		InvalidateRect(rcsizeh);
		InvalidateRect(rcsizev);
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

LRESULT CFloatingMapDlg::OnDrawToolBnClick(WPARAM wparam, LPARAM lparam)
{
	// The button sends it to this control (its parent), forward it
	// to the map editor (the parent of this dialog).
	UNREFERENCED_PARAMETER(wparam);
	GetParent()->SendMessage(WMA_DRAWTOOLBNCLICK, wparam, lparam);
	m_customizebutton.EnableWindow(lparam > 1);
	return 0;
}

void CFloatingMapDlg::OnClickedCustomtool()
{
	if (m_subdlg.GetRenderState().owner->HandleCustomizeTool()) {
		UpdateToolIndex();
	}
}

void CFloatingMapDlg::OnClickedButtonImportMap()
{
	m_subdlg.GetRenderState().owner->HandleMapImport();
}

void CFloatingMapDlg::OnClickedButtonExportMap()
{
	m_subdlg.GetRenderState().owner->HandleMapExport();
}


LRESULT CFloatingMapDlg::OnNcHitTest(CPoint point)
{
	auto rc = get_sizer_rect(false);
	if (rc.PtInRect(point))
		return HTBOTTOMRIGHT;
	return CDialogEx::OnNcHitTest(point);
}


void CFloatingMapDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialogEx::OnPaint() for painting messages
	auto rc = get_sizer_rect(true);
	dc.DrawFrameControl(rc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
}