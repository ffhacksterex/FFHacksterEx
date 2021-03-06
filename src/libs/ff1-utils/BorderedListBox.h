#pragma once


// CBorderedListBox
// The dialog editor maps WS_BORDER to WS_EX_CLIENTEDGE, so you won't get the thin border
// without the 3D shadow effect by subclassing the window to CListBox.
// This class intercepts and reverses the mapping, restoring WS_BORDER.
// If you want WS_EX_CLIENTEDGE, then don't use this class, stickwith CListBox.

class CBorderedListBox : public CListBox
{
	DECLARE_DYNAMIC(CBorderedListBox)

public:
	CBorderedListBox();
	virtual ~CBorderedListBox();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PreSubclassWindow();
};


