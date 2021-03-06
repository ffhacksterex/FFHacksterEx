// BorderedListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "BorderedListCtrl.h"


// CBorderedListCtrl

IMPLEMENT_DYNAMIC(CBorderedListCtrl, CListCtrl)

CBorderedListCtrl::CBorderedListCtrl()
{
}

CBorderedListCtrl::~CBorderedListCtrl()
{
}


BEGIN_MESSAGE_MAP(CBorderedListCtrl, CListCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CBorderedListCtrl message handlers


int CBorderedListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ((lpCreateStruct->dwExStyle & WS_EX_CLIENTEDGE) == WS_EX_CLIENTEDGE) {
		lpCreateStruct->dwExStyle &= ~WS_EX_CLIENTEDGE;
		lpCreateStruct->style |= WS_BORDER;
	}

	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CBorderedListCtrl::PreSubclassWindow()
{
	if ((GetExStyle() & WS_EX_CLIENTEDGE) == WS_EX_CLIENTEDGE) {
		ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
		ModifyStyle(0, WS_BORDER);
	}

	__super::PreSubclassWindow();
}