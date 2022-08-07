#pragma once

#include <ClearButton.h>

#define WMA_DRAWTOOLBNCLICK (WM_APP + 1)

// CDrawingToolButton

class CDrawingToolButton : public CButton
{
	DECLARE_DYNAMIC(CDrawingToolButton)

public:
	CDrawingToolButton(UINT idimage, int index, COLORREF selcolor = 0x80000000);
	virtual ~CDrawingToolButton();

	int GetToolIndex() const;
	COLORREF SelColor;

protected:
	int m_index = -1;
	CBitmap m_image;
	int m_hw = 0;
	int m_hh = 0;
	int m_idimage;
	Gdiplus::Bitmap * m_pgbm;

	void do_paint(CDC& dc);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClicked();
	afx_msg void OnPaint();
};
