// Backdrop.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "Backdrop.h"
#include <AppSettings.h>
#include "AsmFiles.h"
#include <core_exceptions.h>
#include <draw_functions.h>
#include <FFH2Project.h>
#include <GameSerializer.h>
#include <general_functions.h>
#include <ini_functions.h>
#include <io_functions.h>
#include <string_functions.h>
#include <ui_helpers.h>
#include <ValueDataAccessor.h>

#include "NESPalette.h"

using namespace Ini;
using namespace Io;
using namespace Ui;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CBackdrop dialog


CBackdrop::CBackdrop(CWnd* pParent /*=nullptr*/)
	: CSaveableDialog(CBackdrop::IDD, pParent)
{
}

bool CBackdrop::NeedsRefresh() const
{
	return m_bNeedsRefesh;
}

void CBackdrop::DoDataExchange(CDataExchange* pDX)
{
	CSaveableDialog::DoDataExchange(pDX);
}

const BYTE ConstPicFormation[32] = {
	0, 1, 2, 3, 2, 3, 0, 1,
	4, 5, 6, 7, 6, 7, 4, 5,
	8, 9,10,11,10,11, 8, 9,
	12,13,14,15,14,15,12,13 };

BOOL CBackdrop::OnInitDialog()
{
	CSaveableDialog::OnInitDialog();

	try {
		LoadRom();

		for (BYTE co = 0; co < 32; co++)
			PicFormation[co] = ConstPicFormation[co];

		draw.bitmap = bitmap;
		draw.PicFormation = PicFormation;
		draw.Width = 8;
		draw.Height = 4;
		draw.Max = 16;
		draw.palmax = 4;
		draw.curblock = 0;
		draw.curpal = 1;
		draw.rcGraphic.SetRect(20, 26, 20 + 128, 26 + 64);
		draw.rcCloseup.SetRect(20, 112, 20 + 128, 112 + 128);
		draw.rcPalette.SetRect(165, 26, 165 + 48, 26 + 16);
		draw.rcFinger = draw.rcPalette;
		draw.rcFinger.top += 16; draw.rcFinger.bottom += 16;
		draw.PalFormation = nullptr;
		draw.mousedown = 0;

		int offset = BATTLEBACKDROPPALETTE_OFFSET + (backdrop << 2);
		for (BYTE co = 0; co < 4; co++, offset++)
			palette[co] = Proj2->ROM[offset];
		palette[0] = 0x0F;

		Draw_ROM_Buffer(Proj2, BATTLEPATTERNTABLE_OFFSET + 0x10 + (backdrop << 11), &draw);
	}
	catch (std::exception & ex) {
		return AbortInitDialog(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return AbortInitDialog(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}

void CBackdrop::LoadRom()
{
	ffh::acc::ValueDataAccessor d(*Proj2);
	BATTLEBACKDROPPALETTE_OFFSET = d.get<int>("BATTLEBACKDROPPALETTE_OFFSET");
	BATTLEPATTERNTABLE_OFFSET = d.get<int>("BATTLEPATTERNTABLE_OFFSET");

	if (!Proj2->IsRom() && !Proj2->IsAsm()) {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, Proj2->info.type);
	}
}

void CBackdrop::SaveRom()
{
	if (!Proj2->IsRom() && !Proj2->IsAsm()) {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, Proj2->info.type);
	}
	m_bNeedsRefesh = true; // if it saved, then we need to refresh the caller
}

void CBackdrop::StoreValues()
{
	int offset = BATTLEBACKDROPPALETTE_OFFSET + (backdrop << 2);
	for (BYTE co = 0; co < 4; co++, offset++)
		Proj2->ROM[offset] = palette[co];

	Draw_Buffer_ROM(Proj2, BATTLEPATTERNTABLE_OFFSET + 0x10 + (backdrop << 11), &draw);
}


BEGIN_MESSAGE_MAP(CBackdrop, CSaveableDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_COMMAND(ID_EXPORTBITMAP, OnExportbitmap)
	ON_COMMAND(ID_IMPORTBITMAP, OnImportbitmap)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBackdrop message handlers


void CBackdrop::OnPaint() 
{
	CPaintDC dc(this);
	Draw_DrawAll(&dc,&draw,Proj2,palette);
}

void CBackdrop::OnLButtonDown(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	BYTE ret = Draw_ButtonDown(&draw,pt,0);
	if(ret & 1){
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);}
	if(ret & 2)
		InvalidateRect(draw.rcFinger);
}

void CBackdrop::OnLButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	draw.mousedown = 0;
}

void CBackdrop::OnRButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	draw.mousedown = 0;
}

void CBackdrop::OnRButtonDown(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	BYTE ret = Draw_ButtonDown(&draw,pt,1);
	if(ret & 1){
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);}
	if(ret & 2)
		InvalidateRect(draw.rcFinger);
}

void CBackdrop::OnMouseMove(UINT nFlags, CPoint point) 
{
	UNREFERENCED_PARAMETER(nFlags);

	if(Draw_MouseMove(&draw,point)){
		InvalidateRect(draw.rcCloseup,0);
		InvalidateRect(draw.rcGraphic,0);}
}

void CBackdrop::OnLButtonDblClk(UINT nFlags, CPoint pt) 
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

void CBackdrop::OnExportbitmap()
{
	Draw_ExportToBmp(&draw, Proj2, palette, FOLDERPREF(Proj2->AppSettings, PrefImageImportExportFolder));
}

void CBackdrop::OnImportbitmap() 
{
	Draw_ImportFromBmp(&draw,Proj2,palette, FOLDERPREF(Proj2->AppSettings, PrefImageImportExportFolder));
	InvalidateRect(draw.rcGraphic,0);
	InvalidateRect(draw.rcCloseup,0);
}