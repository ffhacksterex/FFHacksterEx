#pragma once

#include "ClearButton.h"

// CClearCheckBox

class CClearCheckBox : public CButton
{
	DECLARE_DYNAMIC(CClearCheckBox)

public:
	CClearCheckBox();
	virtual ~CClearCheckBox();

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

using CClearCheck = CClearCheckBox;
