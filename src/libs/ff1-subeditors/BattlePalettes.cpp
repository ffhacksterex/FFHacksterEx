// BattlePalettes.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "BattlePalettes.h"
#include <ValueDataAccessor.h>
#include <dva_primitives.h>
#include <FFH2Project.h>
#include "general_functions.h"
#include "ini_functions.h"
#include "string_functions.h"

using namespace Ini;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBattlePalettes dialog


CBattlePalettes::CBattlePalettes(CWnd* pParent /*= nullptr */)
	: CFFBaseDlg(CBattlePalettes::IDD, pParent)
{
}


void CBattlePalettes::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ORIGPAL, m_origpal);
	DDX_Control(pDX, IDC_NEWPAL, m_newpal);
}


BEGIN_MESSAGE_MAP(CBattlePalettes, CFFBaseDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBattlePalettes message handlers

BOOL CBattlePalettes::OnInitDialog() 
{
	CFFBaseDlg::OnInitDialog();

	SWITCH_OLDFFH_PTR_CHECK(Project);
	MUST_SPECIFY_PROJECT(Proj2, "BattlePalettes subeditor");

	ffh::acc::ValueDataAccessor d(*Proj2);
	BATTLEPALETTE_OFFSET = d.get<int>("BATTLEPALETTE_OFFSET");

	drawrect.left = 30;
	drawrect.top = 75;
	drawrect.right = drawrect.left + 0x200;
	drawrect.bottom = drawrect.top + 0x80;

	rcOldPal.top = 10;
	rcOldPal.left = 115;
	rcOldPal.bottom = rcOldPal.top + 16;
	rcOldPal.right = rcOldPal.left + 48;

	rcNewPal = rcOldPal;
	rcNewPal.top += 20;
	rcNewPal.bottom += 20;

	newpal = oldpal;
	CString text;
	if(oldpal < 0x10) text.Format("[0%X]",oldpal);
	else text.Format("[%X]",oldpal);
	m_origpal.SetWindowText(text);
	m_newpal.SetWindowText(text);
	
	return 1;
}

void CBattlePalettes::OnPaint() 
{
	CPaintDC dc(this);

	CBrush br;
	CRect rc;
	rc.left = drawrect.left;
	rc.top = drawrect.top;
	rc.right = rc.left + 16;
	rc.bottom = rc.top + 16;

	int coY, coX, co, co2;
	co = BATTLEPALETTE_OFFSET;
	for(coY = 0; coY < 8; coY++, rc.top += 16, rc.bottom += 16, rc.left = drawrect.left,rc.right = rc.left + 16){
	for(coX = 0; coX < 8; coX++, co += 4, rc.left += 16, rc.right += 16){
		for(co2 = 1; co2 < 4; co2++, rc.left += 16, rc.right += 16){
			br.CreateSolidBrush(Proj2->palette[0][Proj2->ROM[co + co2]]);
			dc.FillRect(rc,&br);
			br.DeleteObject();
		}
	}}

	CPoint pt = FindCurPoint();
	Proj2->Finger.Draw(&dc,0,pt,ILD_TRANSPARENT);

	rc.left = rcOldPal.left;
	rc.top = rcOldPal.top;
	rc.right = rc.left + 16;
	rc.bottom = rc.top + 16;
	co = BATTLEPALETTE_OFFSET + 1 + (oldpal << 2);
	for(co2 = 0; co2 < 3; co2++, co++, rc.left += 16, rc.right += 16){
		br.CreateSolidBrush(Proj2->palette[0][Proj2->ROM[co]]);
		dc.FillRect(rc,&br);
		br.DeleteObject();}

	rc.left = rcNewPal.left;
	rc.top = rcNewPal.top;
	rc.right = rc.left + 16;
	rc.bottom = rc.top + 16;
	co = BATTLEPALETTE_OFFSET + 1 + (newpal << 2);
	for(co2 = 0; co2 < 3; co2++, co++, rc.left += 16, rc.right += 16){
		br.CreateSolidBrush(Proj2->palette[0][Proj2->ROM[co]]);
		dc.FillRect(rc,&br);
		br.DeleteObject();}
}

CPoint CBattlePalettes::FindCurPoint()
{
	CPoint pt;
	pt.x = drawrect.left - 16 + ((newpal & 0x07) << 6);
	pt.y = drawrect.top + ((newpal & 0xF8) << 1);
	return pt;
}

void CBattlePalettes::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UNREFERENCED_PARAMETER(nFlags);

	if(PtInRect(drawrect,point)){
		point.x = (point.x - drawrect.left) >> 6;
		point.y = (point.y - drawrect.top) >> 4;
		int temp = newpal;

		CRect rc;
		CPoint pt = FindCurPoint();
		rc.SetRect(pt.x,pt.y,pt.x + 16,pt.y + 16);
		newpal = (point.y << 3) + point.x;
		if(temp == newpal) return;

		InvalidateRect(rc);
		pt = FindCurPoint();
		rc.SetRect(pt.x,pt.y,pt.x + 16,pt.y + 16);
		InvalidateRect(rc);

		CString text;
		if(newpal < 0x10) text.Format("[0%X]",newpal);
		else text.Format("[%X]",newpal);
		m_newpal.SetWindowText(text);
		InvalidateRect(rcNewPal,0);
	}
}