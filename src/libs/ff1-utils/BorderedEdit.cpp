// BorderedEdit.cpp : implementation file
//
#include "stdafx.h"
#include "BorderedEdit.h"


// CBorderedEdit

IMPLEMENT_DYNAMIC(CBorderedEdit, CEdit)

CBorderedEdit::CBorderedEdit()
{
}

CBorderedEdit::~CBorderedEdit()
{
}


BEGIN_MESSAGE_MAP(CBorderedEdit, CEdit)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CBorderedEdit message handlers


int CBorderedEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ((lpCreateStruct->dwExStyle & WS_EX_CLIENTEDGE) == WS_EX_CLIENTEDGE) { //REFACTOR - do this after __super and call the helper described below
		lpCreateStruct->dwExStyle &= ~WS_EX_CLIENTEDGE;
		lpCreateStruct->style |= WS_BORDER;
	}

	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CBorderedEdit::PreSubclassWindow()
{
	if ((GetExStyle() & WS_EX_CLIENTEDGE) == WS_EX_CLIENTEDGE) { //REFACTOR - put this into a helper
		ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
		ModifyStyle(0, WS_BORDER);
	}

	__super::PreSubclassWindow();
}