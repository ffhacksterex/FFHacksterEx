// DlgInputLine.cpp : implementation file
//

#include "stdafx.h"
#include "DlgInputLine.h"
#include "afxdialogex.h"
#include "resource_coredefs.h"


// CDlgInputLine dialog

IMPLEMENT_DYNAMIC(CDlgInputLine, CDialogEx)

CDlgInputLine::CDlgInputLine(CWnd* pParent /*= nullptr */)
	: CDialogEx(IDD_INPUTLINE, pParent)
{
}

CDlgInputLine::~CDlgInputLine()
{
}

void CDlgInputLine::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INPUTLINE_EDIT, m_inlineedit);
	DDX_Control(pDX, IDC_INPUTLINE_LABEL, m_label);
}


BEGIN_MESSAGE_MAP(CDlgInputLine, CDialogEx)
END_MESSAGE_MAP()


// CDlgInputLine message handlers

BOOL CDlgInputLine::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_label.SetWindowText(Label);
	m_inlineedit.SetWindowText(Value);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgInputLine::OnOK()
{
	m_inlineedit.GetWindowText(Value);
	CDialogEx::OnOK();
}