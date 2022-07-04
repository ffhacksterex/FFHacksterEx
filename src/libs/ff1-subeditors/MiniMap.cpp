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

void CMiniMap::OnPaint() 
{
	CPaintDC dc(this);
	CPen redpen; redpen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	CPen* oldpen = dc.SelectObject(&redpen);
	int coX, coY, co = 0;
	for(coY = 0; coY < 256; coY++){
	for(coX = 0; coX < 256; coX++, co++)
		dc.SetPixelV(coX,coY,cart->Palette[0][palette[(PalAssign[Map[co]] << 2) + 3]]);}

	dc.MoveTo(rcNew.TopLeft()); dc.LineTo(rcNew.left + 15,rcNew.top); dc.LineTo(rcNew.left + 15,rcNew.top + 15);
	dc.LineTo(rcNew.left,rcNew.top + 15); dc.LineTo(rcNew.TopLeft());

	dc.SelectObject(oldpen);
	redpen.DeleteObject();
}

void CMiniMap::FixRects(CPoint pt)
{
	rcNew.SetRect(pt.x,pt.y,pt.x + 16,pt.y + 16);
	InvalidateRect(rcNew,0);
	InvalidateRect(rcOld,0);
	rcOld = rcNew;
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
		OVmap->DoHScroll(5,pt.x - 8, nullptr);
		OVmap->DoVScroll(5,pt.y - 8, nullptr);
	}
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
