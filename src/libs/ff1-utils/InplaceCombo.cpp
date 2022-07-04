// InplaceCombo.cpp : implementation file
//

#include "stdafx.h"
#include "InplaceCombo.h"
#include "general_functions.h"
#include "ui_helpers.h"

using namespace Ui;

// CInplaceCombo dialog

IMPLEMENT_DYNAMIC(CInplaceCombo, CDialogEx)

CInplaceCombo::CInplaceCombo(CWnd* pParent /*= nullptr */)
	: CDialogEx(IDD_INPLACE_COMBO, pParent)
	, m_bOked(false)
	, m_bCancelled(false)
	, OKFunc(nullptr)
{
}

CInplaceCombo::~CInplaceCombo()
{
}

void CInplaceCombo::SetPosition(CRect & rc)
{
	CRect curbounds(0, 0, 0, 0);
	GetWindowRect(curbounds);

	auto border = GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CYEDGE);
	CRect rccombo = GetControlRect(&m_combo);
	SetWindowPos(nullptr, rc.left, rc.top - border, rc.Width(), rccombo.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);
}

void CInplaceCombo::SetDropWidth(int width)
{
	if (IsWindow(m_combo.GetSafeHwnd())) {
		if (width <= 0) {
			CRect rc(0, 0, 0, 0);
			m_combo.GetWindowRect(rc);
			width = rc.Width();
		}
		m_combo.SetDroppedWidth(width);
	}
}

void CInplaceCombo::ClearData()
{
	m_combo.ResetContent();
	m_bCancelled = false;
	m_bOked = false;
}

void CInplaceCombo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO, m_combo);
}

BOOL CInplaceCombo::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CInplaceCombo::OnOK()
{
	if ((OKFunc != nullptr) && !m_bOked) {
		m_bOked = true;
		int sel = m_combo.GetCurSel();
		DWORD_PTR data = m_combo.GetItemData(sel);
		OKFunc(sel, data);
	}
	CDialogEx::OnOK();
}

void CInplaceCombo::OnCancel()
{
	m_bCancelled = true;
	CDialogEx::OnCancel();
}


BEGIN_MESSAGE_MAP(CInplaceCombo, CDialogEx)
	ON_WM_SIZE()
	ON_CBN_KILLFOCUS(IDC_COMBO, &CInplaceCombo::OnKillfocusCombo)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


// CInplaceCombo message handlers

void CInplaceCombo::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (IsWindow(m_combo.GetSafeHwnd())) {
		m_combo.SetWindowPos(nullptr, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
	}
}

void CInplaceCombo::OnSetFocus(CWnd* pOldWnd)
{
	CDialogEx::OnSetFocus(pOldWnd);
	m_combo.SetFocus();
}

void CInplaceCombo::OnKillfocusCombo()
{
	if (GetFocus() == this) {
		LogMsg << "user moved focus from combo to dialog, refocus the combo." << std::endl;
	}
	else {
		ShowWindow(SW_HIDE);
		if (!m_bCancelled)
			OnOK();
	}
}

void CInplaceCombo::OnKillFocus(CWnd* pNewWnd)
{
	CDialogEx::OnKillFocus(pNewWnd);

	if (GetFocus() == &m_combo) {
		LogMsg << "dialog gave focus to the combo, don't hide the dialog" << std::endl;
	} 
	else {
		ShowWindow(SW_HIDE);
		if (!m_bCancelled)
			OnOK();
	}
}