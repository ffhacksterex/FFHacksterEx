// CDrawingToolButton.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "DrawingToolButton.h"
#include <ui_helpers.h>
#include <imaging_helpers.h>

// CDrawingToolButton

IMPLEMENT_DYNAMIC(CDrawingToolButton, CButton)

CDrawingToolButton::CDrawingToolButton(UINT idimage, int index, COLORREF selcolor)
	: m_index(index)
	, SelColor((selcolor & 0x80000000) ? GetSysColor(COLOR_HIGHLIGHT) : selcolor)
{
	m_idimage = idimage;
	m_pgbm = Imaging::ImageLoad(AfxGetResourceHandle(), m_idimage, "PNG");
	if (m_pgbm!= nullptr) {
		m_hw = m_pgbm->GetWidth();
		m_hh = m_pgbm->GetHeight();
	}
}

CDrawingToolButton::~CDrawingToolButton()
{
}

int CDrawingToolButton::GetToolIndex() const
{
	return m_index;
}


BEGIN_MESSAGE_MAP(CDrawingToolButton, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &CDrawingToolButton::OnBnClicked)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CDrawingToolButton::do_paint(CDC& dc)
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

// CDrawingToolButton message handlers

void CDrawingToolButton::OnBnClicked()
{
	GetParent()->SendMessage(WMA_DRAWTOOLBNCLICK, 0, m_index);
}

void CDrawingToolButton::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CButton::OnPaint() for painting messages
	do_paint(dc);
}
