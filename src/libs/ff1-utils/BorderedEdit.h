#pragma once

// CBorderedEdit

class CBorderedEdit : public CEdit
{
	DECLARE_DYNAMIC(CBorderedEdit)

public:
	CBorderedEdit();
	virtual ~CBorderedEdit();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PreSubclassWindow();
};
