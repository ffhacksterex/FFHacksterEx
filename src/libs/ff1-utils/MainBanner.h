#pragma once

class CMainBanner
{
public:
	CMainBanner(CWnd * pparent = nullptr);
	~CMainBanner();

	CString Text;
	CString Subtext;
	COLORREF Start;
	COLORREF End;

	void SetParent(CWnd* pparent);
	void Render(CDC & dc, int x, int y, CRect * pupdatedrect = nullptr);
	void Calc(CDC & dc, int x, int y, CRect & pupdatedrect);

protected:
	CWnd * m_pwnd;
	CFont m_font;
	CFont m_subfont;

	void DoRender(CDC & dc, int x, int y, CRect & pupdatedrect, bool calconly);
};
