// TileEdit.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "TileEdit.h"
#include "NESPalette.h"
#include "FFHacksterProject.h"
#include "DRAW_STRUCT.h"
#include "draw_functions.h"
#include "general_functions.h"
#include "string_functions.h"
#include "ini_functions.h"
#include "io_functions.h"
#include <ui_helpers.h>
#include "AsmFiles.h"
#include "GameSerializer.h"

using namespace Ini;
using namespace Io;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTileEdit dialog


CTileEdit::CTileEdit(CWnd* pParent /*= nullptr */)
	: CSaveableDialog(CTileEdit::IDD, pParent)
{
}

void CTileEdit::DoDataExchange(CDataExchange* pDX)
{
	CSaveableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SMALL, m_small);
	DDX_Control(pDX, IDC_LARGE, m_large);
	DDX_Control(pDX, IDC_SHOWROOM, m_showroom);
}

void CTileEdit::LoadRom()
{
	OVERWORLDPALETTE_ASSIGNMENT = ReadHex(cart->ValuesPath, "OVERWORLDPALETTE_ASSIGNMENT");
	OVERWORLDPALETTE_OFFSET = ReadHex(cart->ValuesPath, "OVERWORLDPALETTE_OFFSET");
	OVERWORLDPATTERNTABLE_ASSIGNMENT = ReadHex(cart->ValuesPath, "OVERWORLDPATTERNTABLE_ASSIGNMENT");
	OVERWORLDPATTERNTABLE_OFFSET = ReadHex(cart->ValuesPath, "OVERWORLDPATTERNTABLE_OFFSET");
	TILESETPATTERNTABLE_OFFSET = ReadHex(cart->ValuesPath, "TILESETPATTERNTABLE_OFFSET");
	TILESETPALETTE_ASSIGNMENT = ReadHex(cart->ValuesPath, "TILESETPALETTE_ASSIGNMENT");
	TILESETPATTERNTABLE_ASSIGNMENT = ReadHex(cart->ValuesPath, "TILESETPATTERNTABLE_ASSIGNMENT");
	BANK03_OFFSET = ReadHex(cart->ValuesPath, "BANK03_OFFSET");

	if (cart->IsRom()) {
		// All of ROM has already been loaded by the caller, no need to reload it.
	}
	else if (cart->IsAsm()) {
		GameSerializer ser(*cart);
		if (Invoker == Maps) {
			// parent loads the needed banks
		}
		else if (Invoker == Overworld) {
			// overworld doesn't load bank 3
			ser.LoadAsmBin(BANK_03, BANK03_OFFSET);
		}
		else {
			THROWEXCEPTION(std::runtime_error, "Unknown invoker: should only be invoked to load either Overworld or Standard Map tile data.");
		}
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, (LPCSTR)cart->ProjectTypeName);
	}
}

void CTileEdit::SaveRom()
{
	if (cart->IsRom()) {
		save_binary(cart->WorkRomPath, cart->ROM);
	}
	else if (cart->IsAsm()) {
		GameSerializer ser(*cart);
		// Instead of writing to the entire buffer, just write to the parts we need
		// Note that tile data spans from bank 3 through the end of bank 6.
		if (Invoker == Maps) {
			// parent loads the needed banks
		}
		else if (Invoker == Overworld) {
			// overworld doesn't save bank 3
			ser.SaveAsmBin(BANK_03, BANK03_OFFSET);
		}
		else {
			THROWEXCEPTION(std::runtime_error, "Unknown invoker: should only be invoked to save either Overworld or Standard Map tile data.");
		}
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, (LPCSTR)cart->ProjectTypeName);
	}
}

BOOL CTileEdit::OnInitDialog()
{
	CSaveableDialog::OnInitDialog();

	try {
		if (Invoker == None) {
			AfxMessageBox("An invoker must be specified, either OverworldMap or Maps.", MB_ICONERROR);
			CSaveableDialog::OnCancel();
			return 0;
		}

		LoadRom();

		redpen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		int offset;
		if (tileset) offset = TILESETPATTERNTABLE_ASSIGNMENT + ((tileset - 1) << 9) + tile;
		else offset = OVERWORLDPATTERNTABLE_ASSIGNMENT + tile;

		int co;
		for (co = 0; co < 4; co++, offset += 0x80)
			PicFormation[co] = cart->ROM[offset];

		if (tileset) offset = TILESETPALETTE_ASSIGNMENT + ((tileset - 1) << 7) + tile;
		else offset = OVERWORLDPALETTE_ASSIGNMENT + tile;
		palassign = cart->ROM[offset] & 3;

		int co2;
		for (co2 = 0; co2 < 2; co2++) {
			for (co = 0; co < 16; co++)
				palette[co2][co] = pal[co2][co];
			if (!tileset) break;
		}

		draw.bitmap = bitmap;
		draw.PicFormation = PicFormation;
		draw.palmax = 4;
		draw.Width = 2;
		draw.Height = 2;
		if (tileset) draw.Max = 0x80;
		else draw.Max = 0x100;
		draw.rcPalette.SetRect(35, 97, 35 + 48, 97 + 64);
		draw.rcFinger = draw.rcPalette;
		draw.rcFinger.top = draw.rcPalette.bottom;
		draw.rcFinger.bottom = draw.rcPalette.bottom + 16;
		draw.rcCloseup.SetRect(103, 28, 103 + 128, 28 + 128);
		draw.rcGraphic.SetRect(51, 28, 51 + 32, 28 + 32);
		curblock = 0;
		draw.curblock = PicFormation[curblock];
		draw.curpal = 1;
		draw.PalFormation = nullptr;
		draw.mousedown = 0;

		rcPatternTables.SetRect(253, 28, 253 + 256, 28 + draw.Max);
		rcFinger = draw.rcPalette;
		rcFinger.left -= 16; rcFinger.right = draw.rcPalette.left;

		if (tileset) offset = TILESETPATTERNTABLE_OFFSET + ((tileset - 1) << 11);
		else offset = OVERWORLDPATTERNTABLE_OFFSET;
		Draw_ROM_Buffer(cart, offset, &draw);

		m_showroom.ShowWindow(tileset != 0);
		m_small.ShowWindow(tileset != 0);
		m_large.ShowWindow(tileset == 0);
	}
	catch (std::exception & ex) {
		return Ui::AbortFail(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortFail(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}


BEGIN_MESSAGE_MAP(CTileEdit, CSaveableDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_SHOWROOM, OnShowroom)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_SAVE, OnSave)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTileEdit message handlers

void CTileEdit::OnPaint() 
{
	CPaintDC dc(this);
	CPen* oldpen = dc.SelectObject(&redpen);
	
	BYTE* thispal = &palette[m_showroom.GetCheck()][palassign << 2];
	Draw_DrawPatternTables(&dc,&draw,cart,thispal,rcPatternTables,draw.Max,1);
	Draw_DrawPalette(&dc,draw.rcPalette.left,draw.rcPalette.top,cart,&palette[m_showroom.GetCheck()][0]);
	Draw_DrawPalette(&dc,draw.rcPalette.left,draw.rcPalette.top + 16,cart,&palette[m_showroom.GetCheck()][4]);
	Draw_DrawPalette(&dc,draw.rcPalette.left,draw.rcPalette.top + 32,cart,&palette[m_showroom.GetCheck()][8]);
	Draw_DrawPalette(&dc,draw.rcPalette.left,draw.rcPalette.top + 48,cart,&palette[m_showroom.GetCheck()][12]);
	Draw_DrawFinger(&dc,&draw,cart);
	Draw_DrawCloseup(&dc,&draw,cart,thispal);
	Draw_DrawGraphic(&dc,&draw,cart,thispal,0,0);

	CPoint pt(rcFinger.left,rcFinger.top + (palassign << 4));
	cart->Finger.Draw(&dc,0,pt,ILD_TRANSPARENT);

	pt.x = ((curblock & 1) << 4) + draw.rcGraphic.left;
	pt.y = ((curblock & 2) << 3) + draw.rcGraphic.top;
	dc.MoveTo(pt); dc.LineTo(pt.x + 15,pt.y); dc.LineTo(pt.x + 15,pt.y + 15);
	dc.LineTo(pt.x,pt.y + 15); dc.LineTo(pt);

	dc.SelectObject(oldpen);
}

void CTileEdit::OnShowroom()
{
	InvalidateRect(draw.rcPalette,0);
	InvalidateRect(draw.rcGraphic,0);
	InvalidateRect(draw.rcCloseup,0);
	InvalidateRect(rcPatternTables,0);
}

void CTileEdit::OnLButtonDown(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(pt);

	BYTE redraw = 0;
	if(PtInRect(draw.rcPalette,pt)){
		palassign = BYTE((pt.y - draw.rcPalette.top) >> 4);
		draw.curpal = BYTE((pt.x - draw.rcPalette.left) >> 4) + 1;
		redraw = 3;}
	if(PtInRect(draw.rcCloseup,pt)){
		Draw_ButtonDown(&draw,pt,0);
		redraw = 1;}
	if(PtInRect(draw.rcGraphic,pt)){
		curblock = (BYTE)((((pt.y - draw.rcGraphic.top) & 0xF0) >> 3) + 
			       (((pt.x - draw.rcGraphic.left) & 0xF0) >> 4));
		draw.curblock = PicFormation[curblock];
		redraw = 1;}
	if(PtInRect(rcPatternTables,pt)){
		draw.curblock = BYTE(((pt.y - rcPatternTables.top) & 0xF0) + 
			            ((pt.x - rcPatternTables.left) >> 4));
		PicFormation[curblock] = draw.curblock;
		redraw = 1;}

	if(redraw & 1){
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);
		InvalidateRect(rcPatternTables,0);}
	if(redraw & 2){
		InvalidateRect(rcFinger);
		InvalidateRect(draw.rcFinger);}
}

void CTileEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	draw.mousedown = 0;
}
void CTileEdit::OnRButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	draw.mousedown = 0;
}

void CTileEdit::OnRButtonDown(UINT nFlags, CPoint pt)
{
	if(PtInRect(draw.rcCloseup,pt)){
		Draw_ButtonDown(&draw,pt,1);
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);
		InvalidateRect(rcPatternTables,0);}
	else OnLButtonDown(nFlags,pt);
}

void CTileEdit::OnMouseMove(UINT nFlags, CPoint pt)
{
	UNREFERENCED_PARAMETER(nFlags);
	if(Draw_MouseMove(&draw,pt)){
		InvalidateRect(rcPatternTables,0);
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);}
}

void CTileEdit::OnLButtonDblClk(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);
	if(PtInRect(draw.rcPalette,pt)){
		CNESPalette dlg;
		dlg.cart = cart;
		dlg.color = &palette[m_showroom.GetCheck()][(palassign << 2) + draw.curpal];
		if(dlg.DoModal() == IDOK){
			InvalidateRect(draw.rcPalette,0);
			InvalidateRect(draw.rcGraphic,0);
			InvalidateRect(rcPatternTables,0);
			InvalidateRect(draw.rcCloseup,0);}
	}
}

void CTileEdit::StoreValues()
{
	int offset;
	if(tileset) offset = TILESETPATTERNTABLE_ASSIGNMENT + ((tileset - 1) << 9) + tile;
	else offset = OVERWORLDPATTERNTABLE_ASSIGNMENT + tile;

	int co;
	for(co = 0; co < 4; co++, offset += 0x80)
		cart->ROM[offset] = PicFormation[co];

	if(tileset) offset = TILESETPALETTE_ASSIGNMENT + ((tileset - 1) << 7) + tile;
	else offset = OVERWORLDPALETTE_ASSIGNMENT + tile;
	cart->ROM[offset] = (palassign << 6) + (palassign << 4) + (palassign << 2) + (palassign);

	if(tileset) offset = TILESETPATTERNTABLE_OFFSET + ((tileset - 1) << 11);
	else offset = OVERWORLDPATTERNTABLE_OFFSET;
	Draw_Buffer_ROM(cart,offset,&draw);
	
	int co2;
	for(co2 = 0; co2 < 2; co2++){
		for(co = 0; co < 16; co++)
			pal[co2][co] = palette[co2][co];
		if(!tileset) break;}
}