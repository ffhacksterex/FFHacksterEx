#pragma once

#include "ClearButton.h"

// CCloseButton

class CCloseButton : public CClearButton
{
	DECLARE_DYNAMIC(CCloseButton)

public:
	CCloseButton();
	virtual ~CCloseButton();

protected:
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
