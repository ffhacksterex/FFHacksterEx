#pragma once

// CBorderedListCtrl

class CBorderedListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CBorderedListCtrl)

public:
	CBorderedListCtrl();
	virtual ~CBorderedListCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PreSubclassWindow();
};
