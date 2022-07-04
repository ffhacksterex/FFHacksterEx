#pragma once


// CColorStatic

class CColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CColorStatic)

public:
	CColorStatic();
	virtual ~CColorStatic();

	void SetColor(COLORREF newcolor);
	COLORREF GetColor();

protected:
	COLORREF m_color;
	CBrush m_brush;

	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnPaint();
};
