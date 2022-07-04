#pragma once

class CODBackground
{
public:
	CODBackground();
	~CODBackground();

	CBitmap m_bmp;
	CBrush m_brush;
	CSize m_dims;

	BOOL UpdateBackground(CWnd* pwnd);
	BOOL UpdateThisBackground(CWnd* pwnd);
};
