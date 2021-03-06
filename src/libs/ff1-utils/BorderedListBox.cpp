// CBorderedListBox.cpp : implementation file
//
#include "stdafx.h"
#include "BorderedListBox.h"

// CBorderedListBox

IMPLEMENT_DYNAMIC(CBorderedListBox, CListBox)

CBorderedListBox::CBorderedListBox()
{
}

CBorderedListBox::~CBorderedListBox()
{
}


BEGIN_MESSAGE_MAP(CBorderedListBox, CListBox)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CBorderedListBox message handlers


int CBorderedListBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ((lpCreateStruct->dwExStyle & WS_EX_CLIENTEDGE) == WS_EX_CLIENTEDGE) {
		lpCreateStruct->dwExStyle &= ~WS_EX_CLIENTEDGE;
		lpCreateStruct->style |= WS_BORDER;
	}

	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CBorderedListBox::PreSubclassWindow()
{
	if ((GetExStyle() & WS_EX_CLIENTEDGE) == WS_EX_CLIENTEDGE) {
		ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
		ModifyStyle(0, WS_BORDER);
	}

	CListBox::PreSubclassWindow();
}