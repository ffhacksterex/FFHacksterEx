// InplaceEdit.cpp : implementation file
//

#include "stdafx.h"
#include "InplaceEdit.h"
#include "afxdialogex.h"


// CInplaceEdit dialog

IMPLEMENT_DYNAMIC(CInplaceEdit, CDialogEx)

CInplaceEdit::CInplaceEdit(CWnd* pParent /*= nullptr */)
	: CDialogEx(IDD_INPLACE_EDIT, pParent)
	, m_bOkInvoked(false)
	, m_bCancelled(false)
{
}

CInplaceEdit::~CInplaceEdit()
{
}

void CInplaceEdit::InitEdit(CRect rcitem, CString text)
{
	SetPosition(rcitem);
	ClearData();
	m_edit.SetWindowText(text);
	m_edit.SetSel(0, -1);
}

void CInplaceEdit::ClearData()
{
	m_edit.Clear();
	m_bCancelled = false;
	m_bOkInvoked = false;
}

void CInplaceEdit::SetPosition(CRect rc)
{
	CRect curbounds(0, 0, 0, 0);
	GetWindowRect(curbounds);
	int height = curbounds.Height(); // don't try to change the dialog height
	SetWindowPos(nullptr, rc.left, rc.top, rc.Width(), height, SWP_NOZORDER | SWP_SHOWWINDOW);
}

void CInplaceEdit::SetText(CString text, bool selectall)
{
	m_edit.SetWindowText(text);
	if (selectall) m_edit.SetSel(0, -1);
}

CString CInplaceEdit::GetText()
{
	CString text;
	m_edit.GetWindowText(text);
	return text;
}

void CInplaceEdit::Cancel()
{
	OnCancel();
}

void CInplaceEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INPLACE_EDITBOX, m_edit);
}

//N.B. - when this control is used as a modeless dialog, this will only be called after the Create() function is called.
//       Call ClearData() before each invocation.
BOOL CInplaceEdit::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CInplaceEdit::OnOK()
{
	// Whether the edit is OK or not, hide the control if OnOK is called by any means.
	if (!m_bOkInvoked) {
		m_bOkInvoked = true; // prevent multiple messages from invoking more than one call (e.g. Enter key followed by Killfocus)
		if (OKFunc != nullptr) OKFunc(GetText());

		ShowWindow(SW_HIDE);
		CDialogEx::OnOK();
	}
}

void CInplaceEdit::OnCancel()
{
	m_bCancelled = true;
	CDialogEx::OnCancel();
}


BEGIN_MESSAGE_MAP(CInplaceEdit, CDialogEx)
	ON_WM_SIZE()
	ON_EN_KILLFOCUS(IDC_INPLACE_EDITBOX, &CInplaceEdit::OnKillfocusInplaceEditbox)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// CInplaceEdit message handlers

void CInplaceEdit::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (IsWindow(m_edit.GetSafeHwnd())) {
		m_edit.SetWindowPos(nullptr, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
	}
}

void CInplaceEdit::OnSetFocus(CWnd* pOldWnd)
{
	CDialogEx::OnSetFocus(pOldWnd);

	m_edit.SetFocus();
}

void CInplaceEdit::OnKillfocusInplaceEditbox()
{
	if (GetFocus() != this) {
		if (!m_bCancelled)
			OnOK();
	}
}

void CInplaceEdit::OnKillFocus(CWnd* pNewWnd)
{
	CDialogEx::OnKillFocus(pNewWnd);

	if (GetFocus() == &m_edit) {
		if (!m_bCancelled)
			OnOK();
	}
}