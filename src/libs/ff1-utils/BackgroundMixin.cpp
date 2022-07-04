#include "stdafx.h"
#include "BackgroundMixin.h"
#include "ui_helpers.h"
#include "window_messages.h"

using namespace Ui;

CBackgroundMixin::CBackgroundMixin()
	: m_pimage(&m_nullimage)
	, m_pbrush(&m_nullbrush)
{
}

CBackgroundMixin::CBackgroundMixin(UINT resid, bool enable)
	: CBackgroundMixin()
{
	SetBackgroundResImage(resid, enable);
}

CBackgroundMixin::~CBackgroundMixin()
{
	Release();
}

void CBackgroundMixin::SetBackColor(COLORREF color)
{
	BackgroundColor = color;
}

void CBackgroundMixin::ClearBackColor()
{
	BackgroundColor = 0x80000000;
}

bool CBackgroundMixin::HasBackColor()
{
	// If the high-byte has a value, then the color can't be displayed by GDI
	return (BackgroundColor & 0xFF000000) != 0;
}

bool CBackgroundMixin::SetBackgroundResImage(UINT resid, bool enable)
{
	UNREFERENCED_PARAMETER(resid);
	UNREFERENCED_PARAMETER(enable);
	Release();

//IMAGES - RESTORE, LOAD WILL FAIL IF NO IMAGES ARE PRESENT
	//CBitmap tempbmp;
	//if (!LoadPNG(tempbmp, AfxGetResourceHandle(), resid)) {
	//	if (!tempbmp.LoadBitmap(resid)) {
	//		m_enabled = false;
	//		return false;
	//	}
	//}

	//CBitmap & img = m_liveimage;
	//CBrush & bru = m_livebrush;

	//img.Attach(tempbmp.Detach());
	//bru.CreatePatternBrush(CBitmap::FromHandle((HBITMAP)img.GetSafeHandle()));

	//EnableImage(enable);
	return true;
}

void CBackgroundMixin::Release()
{
	m_pbrush = nullptr;
	m_pimage = nullptr;
	m_liveimage.DeleteObject();
	m_livebrush.DeleteObject();
	m_nullimage.DeleteObject();
	m_nullbrush.DeleteObject();
}

void CBackgroundMixin::EnableImage(bool enable)
{
	m_enabled = enable;
	m_pimage = m_enabled ? &m_liveimage : &m_nullimage;
	m_pbrush = m_enabled ? &m_livebrush : &m_nullbrush;

	auto wnd = dynamic_cast<CWnd*>(this);
	if (wnd != nullptr && IsWindow(wnd->GetSafeHwnd()))
		wnd->Invalidate();
}

void CBackgroundMixin::DrawBackgroundColor(CDC & dc)
{
	auto wnd = dynamic_cast<CWnd*>(this);
	if (wnd != nullptr) {
		auto color = HasBackColor() ? BackgroundColor : GetSysColor(COLOR_WINDOW);
		CRect client;
		wnd->GetClientRect(&client);
		dc.FillSolidRect(&client, color);
	}
}

void CBackgroundMixin::DrawBackgroundImage(CDC & dc)
{
	ASSERT(m_pimage != nullptr);
	if (m_pimage == nullptr)
		return;

	CBitmap & m_backimage = *m_pimage;

	auto wnd = dynamic_cast<CWnd*>(this);
	if (wnd != nullptr && m_backimage.GetSafeHandle()) {
		BITMAP bm = { 0 };
		m_backimage.GetObject(sizeof(bm), &bm);

		CDC memdc;
		memdc.CreateCompatibleDC(&dc);

		CRect client;
		wnd->GetClientRect(&client);

		auto oldbmp = memdc.SelectObject(&m_backimage);
		dc.BitBlt(DrawLocation.x, DrawLocation.y, bm.bmWidth, bm.bmHeight, &memdc, 0, 0, SRCCOPY);
		memdc.SelectObject(oldbmp);
		memdc.DeleteDC();
	}
}

void CBackgroundMixin::PaintBackground(CDC & dc)
{
	if (!m_enabled) return;

	DrawBackgroundColor(dc);
	DrawBackgroundImage(dc);
}

void CBackgroundMixin::PaintClient(CDC & dc)
{
	UNREFERENCED_PARAMETER(dc);
}

CBitmap * CBackgroundMixin::GetBackgroundImage()
{
	ASSERT(m_pimage != nullptr);

	return m_enabled && m_pimage != nullptr ? m_pimage : NULL;
}

HBRUSH CBackgroundMixin::GetBackgroundBrush()
{
	ASSERT(m_pbrush != nullptr);

	return m_enabled && m_pbrush != nullptr ? (HBRUSH)m_pbrush->GetSafeHandle() : NULL;
}