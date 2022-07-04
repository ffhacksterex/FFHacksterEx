#include "stdafx.h"
#include "MainBanner.h"
#include "FFBaseApp.h"

CMainBanner::CMainBanner(CWnd * pparent)
	: Start(RGB(0,0,0))
	, End(RGB(0, 127, 255))
	, m_pwnd(nullptr)
{
	SetParent(pparent);
}

CMainBanner::~CMainBanner()
{
}

void CMainBanner::SetParent(CWnd* pparent)
{
	m_pwnd = pparent;

	auto bannerfont = AfxGetFFBaseApp()->GetFonts().GetBannerFont();
	if (bannerfont != nullptr) {
		LOGFONT lf = { 0 };
		bannerfont->GetLogFont(&lf);
		m_font.DeleteObject();
		m_font.CreateFontIndirect(&lf);
	}
	auto subfont = AfxGetFFBaseApp()->GetFonts().GetSubbannerFont();
	if (subfont != nullptr) {
		LOGFONT lf = { 0 };
		subfont->GetLogFont(&lf);
		m_subfont.DeleteObject();
		m_subfont.CreateFontIndirect(&lf);
	}
}


#define DORENDER_CALCONLY true
#define DORENDER_CALCDRAW false

void CMainBanner::Render(CDC & dc, int x, int y, CRect * pupdatedrect)
{
	CRect rcdummy;
	if (pupdatedrect == nullptr)
		pupdatedrect = &rcdummy;
	DoRender(dc, x, y, *pupdatedrect, DORENDER_CALCDRAW);
}

void CMainBanner::Calc(CDC & dc, int x, int y, CRect & updatedrect)
{
	DoRender(dc, x, y, updatedrect, DORENDER_CALCONLY);
}

void CMainBanner::DoRender(CDC & dc, int x, int y, CRect & updatedrect, bool calconly)
{
	if (Text.IsEmpty()) return;

	auto dcstate = dc.SaveDC();
	dc.SelectObject(m_font);
	dc.SetBkMode(TRANSPARENT);

	CRect rctext = { x, y, x + 1, y + 1 };

	CString & thetext = Text;

	if (Start == End) {
		dc.SetTextColor(Start);
		dc.DrawText(thetext, rctext, DT_CALCRECT);
		if (!calconly)
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
			if (!calconly)
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

	//REFACTOR - REMOVE THE LINE, MOVE THE TEXT UP
	//	Hide the line for now, doesn't look quite right.
	//	ALso, redice the space between the banner and subtext.
	//	Move subtext a couple pixels to the right to better align with the larger banner text.
	auto updistance = rctext.Height() * 20 / 100;
	rctext.OffsetRect(2, -updistance);

	// Draw a line between banner and sub-banner
	//CRect rcline(rctext);
	//dc.MoveTo(rcline.left, rcline.bottom);
	//if (!calconly)
	//	dc.LineTo(rcline.right, rcline.bottom); 
	//else
	//	dc.MoveTo(rcline.right, rcline.bottom);
	//rctext.UnionRect(&rctext, &rcline);

	// Draw the sub-text with the end color from above
	//REFACTOR - use the start color instead
	dc.SetTextColor(Start);
	CRect rcsub(rctext);
	CSize si = dc.GetTextExtent(Subtext);
	rcsub.OffsetRect(0, si.cy);
	dc.SelectObject(&m_subfont);
	dc.DrawText(Subtext, rcsub, DT_CALCRECT);
	if (!calconly)
		dc.DrawText(Subtext, rcsub, 0);
	rctext.UnionRect(&rctext, &rcsub);

	updatedrect = rctext;

	dc.RestoreDC(dcstate);
}