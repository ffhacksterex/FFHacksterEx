#pragma once

#include "ClearButton.h"

// CClearRadio

class CClearRadio : public CClearButton
{
	DECLARE_DYNAMIC(CClearRadio)

public:
	CClearRadio();
	virtual ~CClearRadio();

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
