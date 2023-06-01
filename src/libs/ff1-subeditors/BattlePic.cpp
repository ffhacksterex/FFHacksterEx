// BattlePic.cpp : implementation file
//

#include "stdafx.h"
#include "BattlePic.h"
#include "AsmFiles.h"
#include <core_exceptions.h>
#include <FFH2Project.h>
#include <AppSettings.h>
#include "GameSerializer.h"
#include "NESPalette.h"
#include "draw_functions.h"
#include "general_functions.h"
#include "ini_functions.h"
#include "string_functions.h"
#include "ui_helpers.h"
#include <ValueDataAccessor.h>

using namespace Ini;
using namespace Ui;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBattlePic dialog

CBattlePic::CBattlePic(CWnd* pParent /*= nullptr */)
	: CSaveableDialog(CBattlePic::IDD, pParent)
{
}

void CBattlePic::DoDataExchange(CDataExchange* pDX)
{
	CSaveableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PALETTE4, m_palette4);
	DDX_Control(pDX, IDC_PALETTE3, m_palette3);
	DDX_Control(pDX, IDC_PALETTE2, m_palette2);
	DDX_Control(pDX, IDC_PALETTE1, m_palette1);
}


BEGIN_MESSAGE_MAP(CBattlePic, CSaveableDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_PALETTE1, OnPalette1)
	ON_BN_CLICKED(IDC_PALETTE2, OnPalette2)
	ON_BN_CLICKED(IDC_PALETTE3, OnPalette3)
	ON_BN_CLICKED(IDC_PALETTE4, OnPalette4)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_COMMAND(ID_EXPORTBITMAP, OnExportbitmap)
	ON_COMMAND(ID_IMPORTBITMAP, OnImportbitmap)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBattlePic message handlers

const BYTE ConstPicFormation[39] = {	//3 x 13 pic formation
 0, 1, 0, 1, 8, 9,14,15,20,21,255,255,255,
 2, 3, 2, 3,10,11,16,17,22,23,26,27,28,
 4, 5, 6, 7,12,13,18,19,24,25,29,30,31};


void CBattlePic::LoadRom()
{
	ffh::acc::ValueDataAccessor v(*Proj2);
	CHARBATTLEPALETTE_ASSIGNMENT1 = v.get<int>("CHARBATTLEPALETTE_ASSIGNMENT1");
	CHARBATTLEPALETTE_ASSIGNMENT2 = v.get<int>("CHARBATTLEPALETTE_ASSIGNMENT2");
	CHARBATTLEPIC_OFFSET = v.get<int>("CHARBATTLEPIC_OFFSET");
	CHARBATTLEPALETTE_OFFSET = v.get<int>("CHARBATTLEPALETTE_OFFSET");
	BINBANK09DATA_OFFSET = v.get<int>("BINBANK09DATA_OFFSET");
	BINBANK09GFXDATA_OFFSET = v.get<int>("BINBANK09GFXDATA_OFFSET");

	// In-memory operations ignore loading and saving to the file(s) and assume that the client has intiialized ROM
	// and will handle the loading and saving details.
	if (InMemory)
		return;

	// Now load the data
	if (Proj2->IsRom()) {
	}
	else if (Proj2->IsAsm()) {
		GameSerializer ser(*Proj2);
		// Instead of reading to the entire buffer, just read to the parts we need
		ser.LoadInline(ASM_0C, { { asmlabel, LUT_INBATTLECHARPALETTEASSIGN, { CHARBATTLEPALETTE_ASSIGNMENT1 } } });
		ser.LoadInline(ASM_0F, { { asmlabel, LUT_BATTLESPRITEPALETTES, { CHARBATTLEPALETTE_OFFSET } },
			{ asmlabel, LUT_CLASSBATSPRPALETTE, { CHARBATTLEPALETTE_ASSIGNMENT2 } } });
		ser.LoadAsmBin(BIN_BANK09DATA, BINBANK09DATA_OFFSET);
		ser.LoadAsmBin(BIN_BANK09GFXDATA, BINBANK09GFXDATA_OFFSET);
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, Proj2->info.type);
	}
}

void CBattlePic::SaveRom()
{
	// In-memory operations ignore loading and saving to the file(s) and assume that the client has intiialized ROM
	// and will handle the loading and saving details.
	if (InMemory)
		return;

	if (Proj2->IsRom()) {
	}
	else if (Proj2->IsAsm()) {
		GameSerializer ser(*Proj2);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.SaveInline(ASM_0C, { { asmlabel, LUT_INBATTLECHARPALETTEASSIGN, { CHARBATTLEPALETTE_ASSIGNMENT1 } } });
		ser.SaveInline(ASM_0F, { { asmlabel, LUT_BATTLESPRITEPALETTES, { CHARBATTLEPALETTE_OFFSET } },
			{ asmlabel, LUT_CLASSBATSPRPALETTE, { CHARBATTLEPALETTE_ASSIGNMENT2 } } });
		ser.SaveAsmBin(BIN_BANK09DATA, BINBANK09DATA_OFFSET);
		ser.SaveAsmBin(BIN_BANK09GFXDATA, BINBANK09GFXDATA_OFFSET);
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, Proj2->info.type);
	}
}

BOOL CBattlePic::OnInitDialog() 
{
	CSaveableDialog::OnInitDialog();

	try {
		if (Proj2 == nullptr) THROWEXCEPTION(std::runtime_error, "A valid FFH2017 project must be specified before editing battle sprites.");
		if (Class == -1) THROWEXCEPTION(std::runtime_error, "A class most be specified before its battle sprites can be edited.");

		LoadRom();

		for (BYTE co = 0; co < 39; co++)
			PicFormation[co] = ConstPicFormation[co];

		draw.bitmap = bitmap;
		draw.PicFormation = PicFormation;
		draw.Width = 13;
		draw.Height = 3;
		draw.Max = 32;
		draw.palmax = 4;
		draw.curblock = 0;
		draw.curpal = 1;
		draw.rcGraphic.SetRect(20, 27, 20 + 208, 27 + 48);
		draw.rcCloseup.SetRect(20, 93, 20 + 128, 93 + 128);
		draw.rcPalette.SetRect(162, 93, 162 + 48, 93 + 16);
		draw.rcFinger = draw.rcPalette;
		draw.rcFinger.top += 16; draw.rcFinger.bottom += 16;
		draw.PalFormation = nullptr;
		draw.mousedown = 0;

		palette[0] = 0x0F;

		PalAssign = 255;
		UpdatePalAssignment(Proj2->ROM[CHARBATTLEPALETTE_ASSIGNMENT1 + Class] & 0x03);

		Draw_ROM_Buffer(Proj2, CHARBATTLEPIC_OFFSET + (Class << 9), &draw);
	}
	catch (std::exception & ex) {
		return Ui::AbortInitDialog(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortInitDialog(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}

void CBattlePic::ReLoadPalette()
{
	int offset = CHARBATTLEPALETTE_OFFSET + (PalAssign << 2) + 1;
	for(int co = 1; co < 4; offset++, co++)
		palette[co] = Proj2->ROM[offset];
}

void CBattlePic::StorePalette()
{
	int offset = CHARBATTLEPALETTE_OFFSET + (PalAssign << 2) + 1;
	for(int co = 1; co < 4; offset++, co++)
		Proj2->ROM[offset] = palette[co];
}

void CBattlePic::OnPaint() 
{
	CPaintDC dc(this);
	Draw_DrawAll(&dc,&draw,Proj2,palette);
}

void CBattlePic::OnLButtonDown(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	BYTE ret = Draw_ButtonDown(&draw,pt,0);
	if(ret & 1){
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);}
	if(ret & 2)
		InvalidateRect(draw.rcFinger);
}

void CBattlePic::OnRButtonDown(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	BYTE ret = Draw_ButtonDown(&draw,pt,1);
	if(ret & 1){
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);}
	if(ret & 2)
		InvalidateRect(draw.rcFinger);
}

void CBattlePic::OnRButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	draw.mousedown = 0;
}
void CBattlePic::OnLButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	draw.mousedown = 0;
}

void CBattlePic::OnMouseMove(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	if(Draw_MouseMove(&draw,pt)){
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);}
}

void CBattlePic::UpdatePalAssignment(BYTE update)
{
	if(PalAssign != 255) StorePalette();
	PalAssign = update;
	m_palette1.SetCheck(update == 0);
	m_palette2.SetCheck(update == 1);
	m_palette3.SetCheck(update == 2);
	m_palette4.SetCheck(update == 3);
	ReLoadPalette();
	InvalidateRect(draw.rcPalette,0);
	InvalidateRect(draw.rcGraphic,0);
	InvalidateRect(draw.rcCloseup,0);
}
void CBattlePic::OnPalette1() 
{UpdatePalAssignment(0);}
void CBattlePic::OnPalette2() 
{UpdatePalAssignment(1);}
void CBattlePic::OnPalette3() 
{UpdatePalAssignment(2);}
void CBattlePic::OnPalette4() 
{UpdatePalAssignment(3);}

void CBattlePic::OnLButtonDblClk(UINT nFlags, CPoint pt)
{
	UNREFERENCED_PARAMETER(nFlags);

	if(PtInRect(draw.rcPalette,pt)){
		CNESPalette dlg;
		dlg.Proj2 = Proj2;
		dlg.color = &palette[draw.curpal];
		if(dlg.DoModal() == IDOK){
			InvalidateRect(draw.rcPalette,0);
			InvalidateRect(draw.rcGraphic,0);
			InvalidateRect(draw.rcCloseup,0);}
	}
}

void CBattlePic::StoreValues()
{
	Draw_Buffer_ROM(Proj2,CHARBATTLEPIC_OFFSET + (Class << 9),&draw);

	Proj2->ROM[CHARBATTLEPALETTE_ASSIGNMENT1 + Class] = PalAssign;
	Proj2->ROM[CHARBATTLEPALETTE_ASSIGNMENT2 + Class] = PalAssign;

	StorePalette();
}

void CBattlePic::OnExportbitmap()
{
	Draw_ExportToBmp(&draw, Proj2, palette, FOLDERPREF(Proj2->AppSettings, PrefImageImportExportFolder));
}

void CBattlePic::OnImportbitmap() 
{
	Draw_ImportFromBmp(&draw, Proj2, palette, FOLDERPREF(Proj2->AppSettings, PrefImageImportExportFolder));
	InvalidateRect(draw.rcGraphic,0);
	InvalidateRect(draw.rcCloseup,0);
}