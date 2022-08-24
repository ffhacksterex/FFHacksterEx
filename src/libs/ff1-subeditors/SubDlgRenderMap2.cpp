// SubDlgRenderMap2.cpp : implementation file
//

#include "stdafx.h"
#include "SubDlgRenderMap2.h"
#include "resource_subeditors.h"
#include "afxdialogex.h"


// CSubDlgRenderMap2 dialog

IMPLEMENT_DYNAMIC(CSubDlgRenderMap2, CDialogEx)

CSubDlgRenderMap2::CSubDlgRenderMap2(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SUBDLG_RENDERMAP2, pParent)
{
}

CSubDlgRenderMap2::~CSubDlgRenderMap2()
{
}

BOOL CSubDlgRenderMap2::CreateOverControl(CWnd* pParent, CWnd* pOver)
{
	return 0;
}

void CSubDlgRenderMap2::SetSource(IMapEditor* owner, CSize tiledims)
{
}

void CSubDlgRenderMap2::InvalidateFrame()
{
}

void CSubDlgRenderMap2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSubDlgRenderMap2, CDialogEx)
END_MESSAGE_MAP()

BOOL CSubDlgRenderMap2::OnInitDialog()
{
	return 0;
}


// Internal implementation

BOOL CSubDlgRenderMap2::init()
{
	return 0;
}

int CSubDlgRenderMap2::handle_scroll(UINT nBar, UINT nSBCode, UINT nPos)
{
	return 0;
}

void CSubDlgRenderMap2::handle_sizing()
{
}

void CSubDlgRenderMap2::handle_sizing(int clientx, int clienty)
{
}


// CSubDlgRenderMap2 overrides and message handlers

int CSubDlgRenderMap2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}

void CSubDlgRenderMap2::OnSize(UINT nType, int cx, int cy)
{
}