// ColorStatic.cpp : implementation file
//

#include "stdafx.h"
#include "ColorStatic.h"


// CColorStatic

IMPLEMENT_DYNAMIC(CColorStatic, CStatic)

CColorStatic::CColorStatic()
{
}

CColorStatic::~CColorStatic()
{
}

void CColorStatic::SetColor(COLORREF newcolor)
{
	m_brush.DeleteObject();
	m_brush.CreateSolidBrush(newcolor);
	m_color = newcolor;
	Invalidate();
}

COLORREF CColorStatic::GetColor()
{
	return m_color;
}


BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CColorStatic message handlers


HBRUSH CColorStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
	UNREFERENCED_PARAMETER(pDC);

	if (nCtlColor == CTLCOLOR_STATIC) {
		return (HBRUSH)m_brush.GetSafeHandle();
	}
	return NULL;
}

void CColorStatic::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
}


void CColorStatic::OnPaint()
{
	CPaintDC dc(this);
	CRect client;
	GetClientRect(&client);
	dc.FillSolidRect(&client, m_color);
	auto oldbr = dc.SelectStockObject(HOLLOW_BRUSH);
	dc.Rectangle(&client);
	dc.SelectObject(oldbr);
}