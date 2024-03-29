// Mapman.cpp : implementation file
//

#include "stdafx.h"
#include "Mapman.h"
#include "NESPalette.h"
#include "FFHacksterProject.h"
#include <AppSettings.h>
#include "general_functions.h"
#include "ini_functions.h"
#include "io_functions.h"
#include "string_functions.h"
#include "draw_functions.h"
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
// CMapman dialog


CMapman::CMapman(CWnd* pParent /*= nullptr */)
	: CSaveableDialog(CMapman::IDD, pParent)
{
}

void CMapman::DoDataExchange(CDataExchange* pDX)
{
	CSaveableDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMapman, CSaveableDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_COMMAND(ID_EXPORTBITMAP, OnExportbitmap)
	ON_COMMAND(ID_IMPORTBITMAP, OnImportbitmap)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMapman message handlers

const BYTE ConstPicFormation[16] = {
 0, 1, 4, 5, 8, 9,12,13,
 2, 3, 6, 7,10,11,14,15};


void CMapman::LoadRom()
{
		OVERWORLDPALETTE_OFFSET = ReadHex(cart->ValuesPath, "OVERWORLDPALETTE_OFFSET");
		MAPMANPALETTE_OFFSET = ReadHex(cart->ValuesPath, "MAPMANPALETTE_OFFSET");
		BANK00_OFFSET = ReadHex(cart->ValuesPath, "BANK00_OFFSET");


	// In-memory operations ignore loading and saving to the file(s) and assume that the client has intiialized ROM
	// and will handle the loading and saving details.
	if (InMemory)
		return;

	// Now load the data
	if (cart->IsRom()) {
	}
	else if (cart->IsAsm()) {
		GameSerializer ser(*cart);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.LoadAsmBin(BANK_00, BANK00_OFFSET);
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, (LPCSTR)cart->ProjectTypeName);
	}
}

void CMapman::SaveRom()
{
	// In-memory operations ignore loading and saving to the file(s) and assume that the client has intiialized ROM
	// and will handle the loading and saving details.
	if (InMemory)
		return;

	if (cart->IsRom()) {
	}
	else if (cart->IsAsm()) {
		GameSerializer ser(*cart);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.SaveAsmBin(BANK_00, BANK00_OFFSET);
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, (LPCSTR)cart->ProjectTypeName);
	}
}

BOOL CMapman::OnInitDialog() 
{
	CSaveableDialog::OnInitDialog();

	try {
		LoadRom();

		BYTE co;
		for (co = 0; co < 16; co++)
			PicFormation[co] = ConstPicFormation[co];
		for (co = 0; co < 16; co++) PalFormation[co] = (co >= 8);

		draw.bitmap = bitmap;
		draw.PicFormation = PicFormation;
		draw.Width = 8;
		draw.Height = 2;
		draw.Max = 16;
		draw.palmax = 8;
		draw.curblock = 0;
		draw.curpal = 1;
		draw.rcGraphic.SetRect(20, 28, 20 + 128, 28 + 32);
		draw.rcCloseup.SetRect(20, 80, 20 + 128, 80 + 128);
		draw.rcPalette.SetRect(165, 28, 165 + 48, 28 + 32);
		draw.rcFinger = draw.rcPalette;
		draw.rcFinger.top += 32; draw.rcFinger.bottom += 32;
		draw.PalFormation = PalFormation;
		draw.mousedown = 0;

		Draw_ROM_Buffer(cart, graphicoffset, &draw);

		int offset;
		if (paletteoffset < 0) {
			//we must load character palettes
			//looks like we'll load the overworl palette as a background
			offset = OVERWORLDPALETTE_OFFSET + 0x10;
			for (co = 0; co < 8; co++, offset++)
				palette[co] = cart->ROM[offset];

			//then replace the third color (indexes 2 and 6) with the individual character palette colors,
			//which represent top and bottom.
			offset = ((-paletteoffset - 1) << 1) + MAPMANPALETTE_OFFSET;
			palette[2] = cart->ROM[offset];
			palette[6] = cart->ROM[offset + 1];
		}
		else {
			offset = paletteoffset;
			for (co = 0; co < 8; co++, offset++)
				palette[co] = cart->ROM[offset];
		}
		palette[0] = 0x40;
		palette[4] = 0x40;
	}
	catch (std::exception & ex) {
		return Ui::AbortInitDialog(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortInitDialog(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}

void CMapman::OnPaint() 
{
	CPaintDC dc(this);
	Draw_DrawGraphic(&dc,&draw,cart,palette,1,1);
	Draw_DrawFinger(&dc,&draw,cart);
	Draw_DrawCloseup(&dc,&draw,cart,&palette[(draw.curblock & 2) << 1]);
	Draw_DrawPalette(&dc,draw.rcPalette.left,draw.rcPalette.top,cart,palette);
	Draw_DrawPalette(&dc,draw.rcPalette.left,draw.rcPalette.top + 16,cart,&palette[4]);
}

void CMapman::OnLButtonDown(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	BYTE ret = Draw_ButtonDown(&draw,pt,0);
	if(ret & 1){
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);}
	if(ret & 2)
		InvalidateRect(draw.rcFinger);
}

void CMapman::OnRButtonDown(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	BYTE ret = Draw_ButtonDown(&draw,pt,1);
	if(ret & 1){
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);}
	if(ret & 2)
		InvalidateRect(draw.rcFinger);
}

void CMapman::OnLButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	draw.mousedown = 0;
}
void CMapman::OnRButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	draw.mousedown = 0;
}

void CMapman::OnMouseMove(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);
	if(Draw_MouseMove(&draw,pt)){
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);}
}

void CMapman::OnLButtonDblClk(UINT nFlags, CPoint pt)
{
	UNREFERENCED_PARAMETER(nFlags);
	if(PtInRect(draw.rcPalette,pt)){
		pt.x = (pt.x - draw.rcPalette.left) >> 4;
		pt.y = (pt.y > (draw.rcPalette.top + 16)) << 2;
		CNESPalette dlg;
		dlg.cart = cart;
		dlg.color = &palette[pt.y + pt.x + 1];
		if(dlg.DoModal() == IDOK){
			InvalidateRect(draw.rcCloseup,0);
			InvalidateRect(draw.rcPalette,0);
			InvalidateRect(draw.rcGraphic,0);
	}}
}

void CMapman::StoreValues()
{
	Draw_Buffer_ROM(cart,graphicoffset,&draw);
	
	int offset;
	if (paletteoffset < 0) {
		//we must load character palettes
		offset = OVERWORLDPALETTE_OFFSET + 0x10;
		cart->ROM[offset + 1] = palette[1];
		cart->ROM[offset + 3] = palette[3];
		cart->ROM[offset + 5] = palette[5];
		cart->ROM[offset + 7] = palette[7];
		offset = ((-paletteoffset - 1) << 1) + MAPMANPALETTE_OFFSET;
		cart->ROM[offset] = palette[2];
		cart->ROM[offset + 1] = palette[6];
	}
	else {
		offset = paletteoffset;
		for(BYTE co = 0; co < 8; co++, offset++){
			if(!(co & 3)) continue;
			cart->ROM[offset] = palette[co];}
	}
}

void CMapman::OnExportbitmap()
{
	Draw_ExportToBmp(&draw, cart, palette, FOLDERPREF(cart->AppSettings, PrefImageImportExportFolder));
}

void CMapman::OnImportbitmap() 
{
	Draw_ImportFromBmp(&draw,cart,palette, FOLDERPREF(cart->AppSettings, PrefImageImportExportFolder));
	InvalidateRect(draw.rcGraphic,0);
	InvalidateRect(draw.rcCloseup,0);
}