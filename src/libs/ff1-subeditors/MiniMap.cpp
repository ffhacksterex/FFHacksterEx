// MiniMap.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "ICoordMap.h"
#include "MiniMap.h"
#include "FFHacksterProject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMiniMap dialog


CMiniMap::CMiniMap(CWnd* pParent /*= nullptr */)
	: CFFBaseDlg(CMiniMap::IDD, pParent)
	, Context(-1)
{
}

void CMiniMap::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMiniMap, CFFBaseDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMiniMap message handlers

const COLORREF RED = RGB(255, 0, 0);

void CMiniMap::OnPaint() 
{
	CPaintDC dc(this);
	int coX, coY, co = 0;
	for (coY = 0; coY < 256; coY++) {
		for (coX = 0; coX < 256; coX++, co++)
			dc.SetPixelV(coX, coY, cart->Palette[0][palette[(PalAssign[Map[co]] << 2) + 3]]);
	}
	dc.Draw3dRect(rcNew, RED, RED);
}

void CMiniMap::SetFocusRect(int left, int top, int right, int bottom)
{
	rcNew.SetRect(left, top, right, bottom);
	rcOld = rcNew;
}

void CMiniMap::SetFocusRect(CRect rect)
{
	SetFocusRect(rect.left, rect.top, rect.right, rect.bottom);
}

void CMiniMap::UpdateFocusRect(int left, int top, int right, int bottom)
{
	rcNew.SetRect(left, top, right, bottom);
	InvalidateRect(rcNew, 0);
	InvalidateRect(rcOld, 0);
	rcOld = rcNew;
}

void CMiniMap::UpdateFocusRect(CPoint pt, CSize sz)
{
	UpdateFocusRect(pt.x, pt.y, pt.x + sz.cx, pt.y + sz.cy);
}

void CMiniMap::UpdateFocusRect(CRect rect)
{
	UpdateFocusRect(rect.left, rect.top, rect.right, rect.bottom);
}

void CMiniMap::OnCancel() 
{
	OVmap->Cancel(Context);
}

void CMiniMap::OnLButtonDown(UINT nFlags, CPoint pt) 
{
	mousedown = 1;
	OnMouseMove(nFlags,pt);
}

void CMiniMap::UpdateAll()
{InvalidateRect(nullptr,0);}
void CMiniMap::UpdateCur()
{InvalidateRect(rcNew,0);}

void CMiniMap::OnMouseMove(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	if(mousedown){
		//N.B. - since the main map is fixed at 16x16, the "-8"s below
		//		attempt to center the scrolling done by the minimap.
		//		For now, keep this intact, as the overworld map has a
		//		workaround to this for the popout map.
		OVmap->DoHScroll(SB_THUMBTRACK, pt.x - 8, nullptr);
		OVmap->DoVScroll(SB_THUMBTRACK, pt.y - 8, nullptr);
	}

	CString msg("Minimap");
	if (pt.x < 256 && pt.y < 256) msg.AppendFormat("   X: %d  Y: %d", pt.x, pt.y);
	SetWindowText(msg);
}

void CMiniMap::OnLButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	mousedown = 0;
}

BOOL CMiniMap::OnInitDialog() 
{
	CFFBaseDlg::OnInitDialog();
	
	mousedown = 0;
	
	return 1;
}
