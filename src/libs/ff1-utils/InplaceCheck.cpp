// InplaceCheck.cpp : implementation file
//

#include "stdafx.h"
#include "InplaceCheck.h"
#include "afxdialogex.h"


// CInplaceCheck dialog

IMPLEMENT_DYNAMIC(CInplaceCheck, CDialogEx)

CInplaceCheck::CInplaceCheck(CWnd* pParent /*= nullptr */)
	: CDialogEx(IDD_INPLACECHECK, pParent)
	, m_bOked(false)
	, m_bCancelled(false)
	, m_bNoName(false)
	, m_bChecked(false)
{
}

CInplaceCheck::~CInplaceCheck()
{
}

void CInplaceCheck::InitEdit(CRect rcitem, CString text, bool checked)
{
	SetPosition(rcitem);
	ClearData();

	m_bChecked = checked;
	m_bNoName = text.IsEmpty();
	if (m_bNoName) text = m_bChecked ? "true" : "false";
	m_check.SetWindowText(text);
	m_check.SetCheck(m_bChecked ? BST_CHECKED : BST_UNCHECKED);
}

void CInplaceCheck::ClearData()
{
	m_bCancelled = false;
	m_bOked = false;
	m_bChecked = false;
}

void CInplaceCheck::SetPosition(CRect rc)
{
	CRect curbounds(0, 0, 0, 0);
	GetWindowRect(curbounds);
	int height = curbounds.Height(); // don't try to change the dialog height
	SetWindowPos(nullptr, rc.left, rc.top, rc.Width(), height, SWP_NOZORDER | SWP_SHOWWINDOW);
}

bool CInplaceCheck::IsChecked() const
{
	return m_bChecked;
}

void CInplaceCheck::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INLINEBOOL_CHECK, m_check);
}

BOOL CInplaceCheck::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CInplaceCheck::OnOK()
{
	CDialogEx::OnOK();
	m_bChecked = m_check.GetCheck() == BST_CHECKED;
	if ((OKFunc != nullptr) && !m_bOked) {
		m_bOked = true;
		OKFunc(IsChecked());
	}
}

void CInplaceCheck::OnCancel()
{
	m_bCancelled = true;
	CDialogEx::OnCancel();
}


BEGIN_MESSAGE_MAP(CInplaceCheck, CDialogEx)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_BN_KILLFOCUS(IDC_INLINEBOOL_CHECK, &CInplaceCheck::OnKillfocusInlineboolCheck)
	ON_BN_CLICKED(IDC_INLINEBOOL_CHECK, &CInplaceCheck::OnClickedInlineboolCheck)
END_MESSAGE_MAP()


// CInplaceCheck message handlers

void CInplaceCheck::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (IsWindow(m_check.GetSafeHwnd())) {
		m_check.SetWindowPos(nullptr, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
	}
}

void CInplaceCheck::OnSetFocus(CWnd* pOldWnd)
{
	CDialogEx::OnSetFocus(pOldWnd);
	m_check.SetFocus();
}

void CInplaceCheck::OnKillfocusInlineboolCheck()
{
	if (GetFocus() == this) {
		LogMsg << "user moved focus from edit to dialog, refocus the edit." << std::endl;
	}
	else {
		ShowWindow(SW_HIDE);
		if (!m_bCancelled)
			OnOK();
	}
}

void CInplaceCheck::OnKillFocus(CWnd* pNewWnd)
{
	CDialogEx::OnKillFocus(pNewWnd);

	if (GetFocus() == &m_check) {
		LogMsg << "dialog gave focus to the edit, don't hide the dialog" << std::endl;
	}
	else {
		ShowWindow(SW_HIDE);
		if (!m_bCancelled)
			OnOK();
	}
}

void CInplaceCheck::OnClickedInlineboolCheck()
{
	if (m_bNoName) {
		auto checked = m_check.GetCheck() == BST_CHECKED;
		CString text = checked ? "true" : "false";
		m_check.SetWindowText(text);
	}
}