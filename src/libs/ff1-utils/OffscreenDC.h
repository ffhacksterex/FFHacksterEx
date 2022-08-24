#pragma once
#include <afxwin.h>

// Use this to perform offscreen drawing to a memory bitmap,
// then use Update() to draw the bitmap to the target DC.

class COffscreenDC : public CDC
{
public:
	COffscreenDC(CDC& targetdc, CRect area);
	virtual ~COffscreenDC();

	CDC& GetDrawingDC();
	void Update();

private:
	CDC& m_targetdc;
	const CRect m_area;
	CBitmap m_bmp;
	CDC m_compat;
	CRgn m_cliprgn;
	CBitmap* m_oldbmp = nullptr;
};
