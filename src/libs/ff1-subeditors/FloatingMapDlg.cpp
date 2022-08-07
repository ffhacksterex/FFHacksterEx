// FloatingMapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "FloatingMapDlg.h"
#include "afxdialogex.h"


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
}

BEGIN_MESSAGE_MAP(CFloatingMapDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


void CFloatingMapDlg::HandleClose()
{
	// Send the message to the owner.
	// By default, the owner is the parent
	CWnd* owner = GetOwner();
	CWnd* parent = GetParent();
	CWnd* wnd = owner == nullptr ? owner : parent;
	wnd->SendMessage(WMA_SHOWFLOATINGMAP, 0, (LPARAM)1);
}

// CFloatingMapDlg message handlers

void CFloatingMapDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialogEx::OnPaint() for painting messages
}

void CFloatingMapDlg::OnClose()
{
	HandleClose();
}

void CFloatingMapDlg::OnCancel()
{
	HandleClose();
}

void CFloatingMapDlg::OnOK()
{
	HandleClose();
}