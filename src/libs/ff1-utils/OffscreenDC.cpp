#include "stdafx.h"
#include "OffscreenDC.h"

COffscreenDC::COffscreenDC(CDC& targetdc, CRect area)
	: m_targetdc(targetdc)
	, m_area(area)
{
	// Create a DC and bitmap compatible with the source DC.
	// "Compatible" means it has a matching graphics format.
	m_compat.CreateCompatibleDC(&m_targetdc);
	m_bmp.CreateCompatibleBitmap(&m_targetdc, m_area.Width(), m_area.Height());
	m_oldbmp = m_compat.SelectObject(&m_bmp);

	// Attach a region to ensure we don't draw outside the specified area.
	m_cliprgn.CreateRectRgn(m_area.left, m_area.top, m_area.right, m_area.bottom);
	m_compat.SelectClipRgn(&m_cliprgn);
}

COffscreenDC::~COffscreenDC()
{
	// Select the bitmap out of the compat DC; destructors will clean up the rest.
	m_compat.SelectObject(m_oldbmp);
}

CDC& COffscreenDC::GetDrawingDC()
{
	return m_compat;
}

void COffscreenDC::Update()
{
	auto save = m_targetdc.SaveDC();
	CRgn rgn;
	rgn.CreateRectRgn(m_area.left, m_area.top, m_area.right, m_area.bottom);
	m_targetdc.SelectClipRgn(&rgn);
	m_targetdc.BitBlt(m_area.left, m_area.top, m_area.Width(), m_area.Height(),
		&m_compat,
		0, 0,
		SRCCOPY);
	m_targetdc.RestoreDC(save);
}