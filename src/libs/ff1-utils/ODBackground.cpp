#include "stdafx.h"
#include "ODBackground.h"


CODBackground::CODBackground()
{
}


CODBackground::~CODBackground()
{
}


BOOL CODBackground::UpdateBackground(CWnd * pwnd)
{
	m_bmp.DeleteObject();
	m_brush.DeleteObject();
	m_dims.SetSize(0, 0);

	auto parent = pwnd->GetParent();
	if (parent != nullptr) {
		CRect Rect;
		pwnd->GetClientRect(&Rect);

		// Translate the client area to a screen rect
		pwnd->ClientToScreen(&Rect);
		parent->ScreenToClient(&Rect);

		CDC * dcparent = parent->GetDC();
		m_dims.cx = Rect.Width();
		m_dims.cy = Rect.Height();

		m_bmp.CreateCompatibleBitmap(dcparent, Rect.Width(), Rect.Height());

		CDC memdc;
		memdc.CreateCompatibleDC(dcparent);
		CBitmap *oldbmp = memdc.SelectObject(&m_bmp);
		memdc.BitBlt(0, 0, Rect.Width(), Rect.Height(),
			dcparent, Rect.left, Rect.top, SRCCOPY);

		memdc.SelectObject(oldbmp);
		parent->ReleaseDC(dcparent);

		m_brush.CreatePatternBrush(&m_bmp);
	}

	return m_bmp.GetSafeHandle() != NULL;
}

BOOL CODBackground::UpdateThisBackground(CWnd * pwnd)
{
	m_bmp.DeleteObject();
	m_brush.DeleteObject();
	m_dims.SetSize(0, 0);

	// Copy this window's background

	auto parent = pwnd;
	if (parent != nullptr) {
		CRect Rect;
		pwnd->GetClientRect(&Rect);

		// Translate the client area to a screen rect
		pwnd->ClientToScreen(&Rect);
		parent->ScreenToClient(&Rect);

		CDC * dcparent = parent->GetDC();
		m_dims.cx = Rect.Width();
		m_dims.cy = Rect.Height();

		m_bmp.CreateCompatibleBitmap(dcparent, Rect.Width(), Rect.Height());

		CDC memdc;
		memdc.CreateCompatibleDC(dcparent);
		CBitmap *oldbmp = memdc.SelectObject(&m_bmp);
		memdc.BitBlt(0, 0, Rect.Width(), Rect.Height(),
			dcparent, Rect.left, Rect.top, SRCCOPY);

		memdc.SelectObject(oldbmp);
		parent->ReleaseDC(dcparent);

		m_brush.CreatePatternBrush(&m_bmp);
	}

	return m_bmp.GetSafeHandle() != NULL;
}