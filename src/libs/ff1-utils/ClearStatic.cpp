// ClearStatic.cpp : implementation file
//

#include "stdafx.h"
#include "ClearStatic.h"


// CClearStatic

IMPLEMENT_DYNAMIC(CClearStatic, CStatic)

CClearStatic::CClearStatic()
{
}

CClearStatic::~CClearStatic()
{
}


BEGIN_MESSAGE_MAP(CClearStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CClearStatic message handlers

void CClearStatic::OnPaint()
{
	if (!IsWindowVisible()) return;

	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CStatic::OnPaint() for painting messages

	CRect client;
	GetClientRect(&client);

	CString text;
	GetWindowText(text);

	auto style = GetStyle();
	auto exstyle = GetExStyle();

	bool transperent = (exstyle & WS_EX_TRANSPARENT) != 0;
	int align = 0;
	if (style & SS_RIGHT) align |= DT_RIGHT;
	else if (style & SS_CENTER) align |= DT_CENTER;
	if (style & SS_CENTERIMAGE)
		align |= DT_SINGLELINE | DT_VCENTER;

	auto dcstate = dc.SaveDC();
	dc.SelectObject(GetFont());
	dc.SetBkMode(TRANSPARENT);
	if (!transperent) {
		//FUTURE - until theming is implemented, FillSolidRect() will have to do
		dc.FillSolidRect(&client, GetSysColor(COLOR_BTNFACE)); // or dc.GetBkColor()
	}

	dc.DrawText(text, &client, align | DT_EXPANDTABS); //FUTURE - auto wrapping isn't automatically supported yet
	dc.RestoreDC(dcstate);
}
