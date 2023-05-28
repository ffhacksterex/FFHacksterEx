// CustomTool.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "CustomTool.h"
#include <FFH2Project.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomTool dialog

CCustomTool::CCustomTool(CWnd* pParent /*= nullptr */)
	: CFFBaseDlg(CCustomTool::IDD, pParent)
{
}

void CCustomTool::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCustomTool, CFFBaseDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCustomTool message handlers

BOOL CCustomTool::OnInitDialog() 
{
	CFFBaseDlg::OnInitDialog();

	FFH_THROW_NULL_PROJECT(Proj2, "Custom Tool");
	FFH_THROW_OLD_FFHACKSTERPROJ(dat);
	
	rcTiles.SetRect(75,15,75 + 256,15 + 128);
	rcTool.SetRect(15,15,15 + 48,15 + 48);
	redpen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	slot = 0;
	for(int co = 0; co < 9; co++)
		toolarray[co] = Proj2->session.smartTools[tool][co];
	
	return 1;
}

void CCustomTool::OnPaint() 
{
	CPaintDC dc(this);
	CPen* origpen = dc.SelectObject(&redpen);

	int coY, coX, co;
	CPoint pt(rcTiles.left,rcTiles.top);
	for(coY = 0, co = 0; coY < 8; coY++, pt.y += 16){
	for(coX = 0, pt.x = rcTiles.left; coX < 16; coX++, pt.x += 16,co++)
		m_tiles->Draw(&dc,co,pt,ILD_NORMAL);}

	pt.x = rcTool.left; pt.y = rcTool.top;
	for(coY = 0, co = 0; coY < 3; coY++, pt.y += 16){
	for(coX = 0, pt.x = rcTool.left; coX < 3; coX++, pt.x += 16, co++)
		m_tiles->Draw(&dc,toolarray[co],pt,ILD_NORMAL);}

	pt.x = rcTool.left + ((slot % 3) << 4);
	pt.y = rcTool.top + ((slot / 3) << 4);
	dc.MoveTo(pt); pt.x += 15;
	dc.LineTo(pt); pt.y += 15;
	dc.LineTo(pt); pt.x -= 15;
	dc.LineTo(pt); pt.y -= 15;
	dc.LineTo(pt);

	dc.SelectObject(origpen);
}

void CCustomTool::OnLButtonDown(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	if(PtInRect(rcTool,pt)){
		pt.x = (pt.x - rcTool.left) >> 4;
		pt.y = (pt.y - rcTool.top) >> 4;
		slot = (pt.y * 3) + pt.x;
		InvalidateRect(rcTool,0);}
	else if(PtInRect(rcTiles,pt)){
		pt.x = (pt.x - rcTiles.left) >> 4;
		pt.y = (pt.y - rcTiles.top) & 0xF0;
		toolarray[slot] = BYTE(pt.y + pt.x);
		slot = (slot + 1) % 9;
		InvalidateRect(rcTool,0);}
}

void CCustomTool::OnOK() 
{
	for(int co = 0; co < 9; co++)
		Proj2->session.smartTools[tool][co] = toolarray[co];
	CFFBaseDlg::OnOK();
}