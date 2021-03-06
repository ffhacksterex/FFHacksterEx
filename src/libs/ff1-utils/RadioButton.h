#pragma once


// CRadioButton
// This class is purely semantic, and is used to identify checkboxes ctreated in the dialog editor.
// Note that this class does not wotk with Owner Drawn checkboxes because Ownr Draw overwrites the
// necessary flags to distinguish the check box from other button types.
// When used with a non-checkbox or an owner draw button, the debug library ASSERTs.

class CRadioButton : public CButton
{
	DECLARE_DYNAMIC(CRadioButton)

public:
	CRadioButton();
	virtual ~CRadioButton();

protected:
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
