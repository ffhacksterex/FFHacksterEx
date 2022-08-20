// SimpleImageButton.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleImageButton.h"
#include "ui_helpers.h"
#include <imaging_helpers.h>

// CSimpleImageButton

IMPLEMENT_DYNAMIC(CSimpleImageButton, CButton)

CSimpleImageButton::CSimpleImageButton(UINT idimage, COLORREF selcolor)
	: SelColor((selcolor & 0x80000000) ? GetSysColor(COLOR_HIGHLIGHT) : selcolor)
{
	m_restype = "PNG"; //TODO - for now, just PNG, add support for other types as needed.
	m_idimage = idimage;
	m_pgbm = Imaging::ImageLoad(AfxGetResourceHandle(), m_idimage, m_restype);
	if (m_pgbm != nullptr) {
		m_hw = m_pgbm->GetWidth();
		m_hh = m_pgbm->GetHeight();
	}
}

CSimpleImageButton::~CSimpleImageButton()
{
	if (m_pgbm != nullptr) delete m_pgbm;
}


BEGIN_MESSAGE_MAP(CSimpleImageButton, CButton)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CSimpleImageButton message handlers

void CSimpleImageButton::do_paint(CDC& dc)
{
	CRect client = Ui::GetClientRect(this);

	// Background
	auto pt = Ui::GetControlCursorPos(this);
	bool hot = client.PtInRect(pt) == TRUE;
	auto bkcolor = hot ? GetSysColor(COLOR_HOTLIGHT) : dc.GetBkColor();
	dc.FillSolidRect(client, bkcolor);

	// Image
	//TODO - maintain aspect ratio when stretching
	Gdiplus::Rect grcbmp{ 0, 0, m_hw, m_hh };
	Gdiplus::Rect gclient{ client.left, client.top, client.Width(), client.Height() };
	Gdiplus::Rect grc = gclient;
	grc.Inflate(-2, -2);
	grc.Intersect(gclient);
	Gdiplus::Graphics g(dc.GetSafeHdc());
	g.DrawImage(m_pgbm, grc, grcbmp.X, grcbmp.Y, grcbmp.Width, grcbmp.Height,
		Gdiplus::Unit::UnitPixel);

	// Border and focus rect
	bool checked = GetCheck() == BST_CHECKED;
	auto bdrcolor = checked ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor(COLOR_ACTIVECAPTION);
	dc.Draw3dRect(client, bdrcolor, bdrcolor);

	if (GetFocus() == this) {
		client.InflateRect(-3, -3);
		dc.DrawFocusRect(&client);
	}
}

void CSimpleImageButton::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CButton::OnPaint() for painting messages
	do_paint(dc);
}
