#pragma once

#include "ClearButton.h"

// CHelpbookButton

class CHelpbookButton : public CClearButton
{
	DECLARE_DYNAMIC(CHelpbookButton)

public:
	CHelpbookButton();
	virtual ~CHelpbookButton();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


