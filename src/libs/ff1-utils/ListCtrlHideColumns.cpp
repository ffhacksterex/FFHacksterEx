// ListCtrlHideColumns.cpp : implementation file

#include "stdafx.h"
#include "ListCtrlHideColumns.h"


// CListCtrlHideColumns

IMPLEMENT_DYNAMIC(CListCtrlHideColumns, CListCtrl)

CListCtrlHideColumns::CListCtrlHideColumns()
{
}

CListCtrlHideColumns::~CListCtrlHideColumns()
{
}


BEGIN_MESSAGE_MAP(CListCtrlHideColumns, CListCtrl)
	ON_MESSAGE(LVM_SETCOLUMNWIDTH, OnSetColumnWidth)
	ON_NOTIFY_EX(HDN_DIVIDERDBLCLICK, 0, OnHeaderDividerDblClick)
	ON_WM_KEYDOWN()
	ON_NOTIFY(HDN_BEGINTRACKA, 0, &CListCtrlHideColumns::OnHdnBegintrack)
	ON_NOTIFY(HDN_BEGINTRACKW, 0, &CListCtrlHideColumns::OnHdnBegintrack)
END_MESSAGE_MAP()


// CListCtrlHideColumns message handlers

LRESULT CListCtrlHideColumns::OnSetColumnWidth(WPARAM wParam, LPARAM lParam)
{
	// Check that column is allowed to be resized
	int nCol = (int)wParam;
	if (!IsColumnVisible(nCol))
		return FALSE;

	// Let the CListCtrl handle the event
	return DefWindowProc(LVM_SETCOLUMNWIDTH, wParam, lParam);
}

BOOL CListCtrlHideColumns::OnHeaderDividerDblClick(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pResult);

	NMHEADER* pNMH = (NMHEADER*)pNMHDR;
	SetColumnWidthAuto(pNMH->iItem);
	return TRUE;    // Don't let parent handle the event
}

void CListCtrlHideColumns::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_ADD:    // CTRL + NumPlus (Auto size all columns)
			if (GetKeyState(VK_CONTROL) < 0)
			{
				// Special handling to avoid showing "hidden" columns
				SetColumnWidthAuto(-1);
				return;
			}
			break;
	}
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CListCtrlHideColumns::SetColumnWidthAuto(int nCol, bool includeHeader)
{
	if (nCol == -1) {
		for (int i = 0; i < GetHeaderCtrl()->GetItemCount(); ++i)
		{
			SetColumnWidthAuto(i, includeHeader);
		}
		return TRUE;
	}
	else {
		if (includeHeader)
			return SetColumnWidth(nCol, LVSCW_AUTOSIZE_USEHEADER);
		else
			return SetColumnWidth(nCol, LVSCW_AUTOSIZE);
	}
}

bool CListCtrlHideColumns::IsColumnVisible(int nCol)
{
	return GetColumnWidth(nCol) > 0;
}

void CListCtrlHideColumns::OnHdnBegintrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	*pResult = 0;

	int nCol = (int)phdr->iItem;
	if (!IsColumnVisible(nCol)) {
		*pResult = TRUE;    // Block resize
	}
}