#pragma once

// CClearStatic

class CClearStatic : public CStatic
{
	DECLARE_DYNAMIC(CClearStatic)

public:
	CClearStatic();
	virtual ~CClearStatic();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
};
