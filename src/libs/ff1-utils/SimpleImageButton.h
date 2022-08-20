#pragma once

// CSimpleImageButton

class CSimpleImageButton : public CButton
{
	DECLARE_DYNAMIC(CSimpleImageButton)

public:
	CSimpleImageButton(UINT idimage, COLORREF selcolor = 0x80000000);
	virtual ~CSimpleImageButton();

	COLORREF SelColor;

protected:
	UINT m_idimage;
	CString m_restype;
	CBitmap m_image;
	int m_hw = 0;
	int m_hh = 0;
	Gdiplus::Bitmap* m_pgbm;

	void do_paint(CDC& dc);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};
