#pragma once

#include "ClearButton.h"

// CInfoButton

class CInfoButton : public CClearButton
{
	DECLARE_DYNAMIC(CInfoButton)

public:
	CInfoButton();
	virtual ~CInfoButton();

protected:
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
