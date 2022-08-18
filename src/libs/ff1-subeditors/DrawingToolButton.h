#pragma once

#define WMA_DRAWTOOLBNCLICK ((WM_APP) + 1)		// LPARAM = tool index

typedef struct sToolButtonData {
	UINT resid;
	CString restype;
	int param;
} sMapDlgButton;

// CDrawingToolButton

class CDrawingToolButton : public CButton
{
	DECLARE_DYNAMIC(CDrawingToolButton)

public:
	CDrawingToolButton(UINT idimage, int index, COLORREF selcolor = 0x80000000);
	virtual ~CDrawingToolButton();

	int GetToolIndex() const;
	COLORREF SelColor;
	sToolButtonData GetSpec() const;
	void SuppressClickHandler(bool suppress);

protected:
	bool m_suppressclick = false;
	UINT m_idimage;
	CString m_restype;
	int m_index = -1;
	CBitmap m_image;
	int m_hw = 0;
	int m_hh = 0;
	Gdiplus::Bitmap * m_pgbm;

	void do_paint(CDC& dc);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClicked();
	afx_msg void OnPaint();
};
