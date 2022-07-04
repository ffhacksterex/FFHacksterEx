#pragma once

#include "BorderedListCtrl.h"

// CListCtrlHideColumns
// A listbox that doesn't allow 0-width columns to resize, effectively hiding them.
// Minor usage note, the "hidden" column can't be resized, so click/drag slightly left
// of the column divider to work around it.

class CListCtrlHideColumns : public CBorderedListCtrl
{
	DECLARE_DYNAMIC(CListCtrlHideColumns)
public:
	CListCtrlHideColumns();
	virtual ~CListCtrlHideColumns();

protected:
	bool IsColumnVisible(int nCol);

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnSetColumnWidth(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnHeaderDividerDblClick(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL SetColumnWidthAuto(int nCol, bool includeHeader = false);
	afx_msg void OnHdnBegintrack(NMHDR *pNMHDR, LRESULT *pResult);
};
