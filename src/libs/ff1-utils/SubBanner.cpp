#include "stdafx.h"
#include "SubBanner.h"
#include "FFBaseApp.h"

CSubBanner::CSubBanner(CWnd * pparent)
	: Start(RGB(0, 0, 0))
	, End(RGB(0, 127, 255))
	, pwnd(nullptr)
	, fontheight(0)
{
	Reparent(pparent);
}

CSubBanner::~CSubBanner()
{
}

void CSubBanner::Reparent(CWnd * pparent)
{
	pwnd = pparent;

	auto bannerfont = AfxGetFFBaseApp()->GetFonts().GetBannerFont();
	if (bannerfont != nullptr) {
		LOGFONT lf = { 0 };
		bannerfont->GetLogFont(&lf);
		lf.lfHeight = lf.lfHeight * 60 / 100;
		m_font.DeleteObject();
		m_font.CreateFontIndirect(&lf);
	}
}

void CSubBanner::Set(CWnd* pparent, COLORREF start, COLORREF end, CString text)
{
	Start = start;
	End = end;
	Text = text;
	Reparent(pparent);
}

int CSubBanner::GetHeight() const
{
	return fontheight;
}

void CSubBanner::Render(CDC & dc, int x, int y, CRect * pupdatedrect)
{
	if (Text.IsEmpty()) return;

	auto dcstate = dc.SaveDC();
	dc.SelectObject(m_font);
	dc.SetBkMode(TRANSPARENT);

	CString & thetext = Text;

	CRect rctext = { x, y, x + 1, y + 1 };

	if (Start == End) {
		dc.SetTextColor(Start);
		dc.DrawText(thetext, rctext, DT_CALCRECT);
		dc.DrawText(thetext, rctext, 0);
	}
	else {
		// Calculate the step size for each color using the number of characters in the string.
		int length = thetext.GetLength();
		float rstep = (GetRValue(End) - GetRValue(Start)) / (float)length;
		float gstep = (GetGValue(End) - GetGValue(Start)) / (float)length;
		float bstep = (GetBValue(End) - GetBValue(Start)) / (float)length;
		float red = GetRValue(Start);
		float green = GetGValue(Start);
		float blue = GetBValue(Start);

		// Now, start drawing from here and go character-by-character
		char sztext[2] = { 0 };
		CRect rcletter(rctext);
		for (int st = 0; st < length; ++st) {
			// stick the character into a working nulltermed string
			sztext[0] = thetext[st];

			// set the color and draw the character
			dc.SetTextColor(RGB(red, green, blue));
			dc.DrawText(sztext, rcletter, DT_CALCRECT);
			dc.DrawText(sztext, rcletter, 0);

			// step the drawing position
			CSize size = dc.GetTextExtent(sztext, 1);
			rcletter.OffsetRect(size.cx, 0);

			// step the colors
			red += rstep;
			green += gstep;
			blue += bstep;
			rctext.UnionRect(&rctext, &rcletter);
		}
	}

	if (pupdatedrect) *pupdatedrect = rctext;

	dc.RestoreDC(dcstate);
}