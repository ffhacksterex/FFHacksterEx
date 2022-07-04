// WaitingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WaitingDlg.h"
#include "afxdialogex.h"
#include "ui_helpers.h"
#include "window_messages.h"

// CWaitingDlg dialog

IMPLEMENT_DYNAMIC(CWaitingDlg, CDialogEx)

CWaitingDlg::CWaitingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD, pParent)
{
}

CWaitingDlg::~CWaitingDlg()
{
}

void CWaitingDlg::SetMessage(CString message)
{
	m_messagestatic.SetWindowText(message);
}

void CWaitingDlg::Init()
{
	if (!IsWindow(GetSafeHwnd())) {
		if (!Create(IDD))
			throw Ui::ui_exception(EXCEPTIONPOINT, "Unable to initialize the waiting progress dialog.");
	}

	Ui::CenterToParent(this);
	ShowWindow(TRUE);
}

void CWaitingDlg::DoAddSteps(int numtoadd)
{
	UNREFERENCED_PARAMETER(numtoadd);
}

void CWaitingDlg::DoStep()
{
}

void CWaitingDlg::DoSetProgressText(LPCSTR text)
{
	SetMessage(text);
}

void CWaitingDlg::DoSetStatusText(LPCSTR text)
{
	UNREFERENCED_PARAMETER(text);
	//FUTURE - for now, setting the status text from here is not supported
}

void CWaitingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_messagestatic);
}


BEGIN_MESSAGE_MAP(CWaitingDlg, CDialogEx)
	ON_REGISTERED_MESSAGE(RWM_FFHACKSTEREX_SHOWWAITMESSAGE, &CWaitingDlg::OnFFHacksterExShowWaitMessage)
END_MESSAGE_MAP()


// CWaitingDlg message handlers

LRESULT CWaitingDlg::OnFFHacksterExShowWaitMessage(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	if (lParam != NULL) SetMessage((LPCSTR)lParam);
	return 0;
}