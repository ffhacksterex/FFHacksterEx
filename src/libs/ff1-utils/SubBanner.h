#pragma once

#include <gdiplus.h>
#include <atlimage.h>

class CSubBanner
{
public:
	CSubBanner(CWnd* pparent = nullptr);
	~CSubBanner();

	CString Text;
	COLORREF Start;
	COLORREF End;
	COLORREF StripeColor;

	void Reparent(CWnd* pparent);
	void Set(CWnd* pparent, COLORREF start, COLORREF end, CString text);
	int GetHeight() const;
	void Render(CDC & dc, int x, int y, CRect * pupdatedrect = nullptr);

protected:
	CWnd * pwnd;
	CFont m_font;
	int fontheight;
};
