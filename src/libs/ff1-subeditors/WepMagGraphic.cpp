// WepMagGraphic.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "WepMagGraphic.h"
#include "FFHacksterProject.h"
#include "general_functions.h"
#include "ini_functions.h"
#include "string_functions.h"
#include "draw_functions.h"
#include <ui_helpers.h>
#include "AsmFiles.h"
#include "GameSerializer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Ini;

/////////////////////////////////////////////////////////////////////////////
// CWepMagGraphic dialog


CWepMagGraphic::CWepMagGraphic(CWnd* pParent /*= nullptr */)
	: CSaveableDialog(CWepMagGraphic::IDD, pParent)
{
}

void CWepMagGraphic::DoDataExchange(CDataExchange* pDX)
{
	CSaveableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WEAPON, m_weapon);
	DDX_Control(pDX, IDC_MAGIC, m_magic);
}


BEGIN_MESSAGE_MAP(CWepMagGraphic, CSaveableDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_COMMAND(ID_EXPORTBITMAP, OnExportbitmap)
	ON_COMMAND(ID_IMPORTBITMAP, OnImportbitmap)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWepMagGraphic message handlers

const BYTE ConstPicFormationMag[8] = {
0,1,4,5,
2,3,6,7};

const BYTE ConstPicFormationWep[4] = {
0,1,
2,3};


void CWepMagGraphic::LoadRom()
{
	WEAPONMAGICGRAPHIC_OFFSET = ReadHex(Project->ValuesPath, "WEAPONMAGICGRAPHIC_OFFSET");
	BINBANK09GFXDATA_OFFSET = ReadHex(Project->ValuesPath, "BINBANK09GFXDATA_OFFSET");

	// Now load the data
	if (Project->IsRom()) {
	}
	else if (Project->IsAsm()) {
		//DEVNOTE - not needed, all clients currently load BIN_BANK09GFXDATA/BINBANK09GFXDATA_OFFSET
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, (LPCSTR)Project->ProjectTypeName);
	}
}

void CWepMagGraphic::SaveRom()
{
	if (Project->IsRom()) {
	}
	else if (Project->IsAsm()) {
		//DEVNOTE - not needed, all clients currently save BIN_BANK09GFXDATA/BINBANK09GFXDATA_OFFSET
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, (LPCSTR)Project->ProjectTypeName);
	}
}

void CWepMagGraphic::StoreValues()
{
	Draw_Buffer_ROM(Project, WEAPONMAGICGRAPHIC_OFFSET + (graphic << 6), &draw);
}

BOOL CWepMagGraphic::OnInitDialog() 
{
	CSaveableDialog::OnInitDialog();

	try {
		LoadRom();

		m_magic.ShowWindow(!IsWeapon);
		m_weapon.ShowWindow(IsWeapon);

		draw.bitmap = bitmap;
		if (IsWeapon) {
			for (BYTE co = 0; co < 4; co++)
				PicFormation[co] = ConstPicFormationWep[co];
			draw.Width = 2;
			draw.Max = 4;
		}
		else {
			for (BYTE co = 0; co < 8; co++)
				PicFormation[co] = ConstPicFormationMag[co];
			draw.Width = 4;
			draw.Max = 8;
		}

		draw.PicFormation = PicFormation;
		draw.Height = 2;
		draw.palmax = 4;
		draw.curblock = 0;
		draw.curpal = 1;
		draw.rcGraphic.SetRect(20, 25, 20 + 32, 25 + 32);
		if (!IsWeapon) draw.rcGraphic.right += 32;
		draw.rcCloseup.SetRect(20, 130, 20 + 128, 130 + 128);
		draw.rcPalette.SetRect(20, 80, 20 + 48, 80 + 16);
		draw.rcFinger = draw.rcPalette;
		draw.rcFinger.top += 16;
		draw.rcFinger.bottom += 16;
		draw.PalFormation = nullptr;
		draw.mousedown = 0;
		palette[0] = 0x0F;
		palette[1] = 0x20 + paletteref;
		palette[2] = 0x10 + paletteref;
		if (IsWeapon) palette[3] = paletteref;
		else palette[3] = 0x30;

		Draw_ROM_Buffer(Project, WEAPONMAGICGRAPHIC_OFFSET + (graphic << 6), &draw);

		if (!m_caption.IsEmpty()) SetWindowText(m_caption);
	}
	catch (std::exception & ex) {
		return Ui::AbortFail(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortFail(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}

void CWepMagGraphic::OnPaint() 
{
	CPaintDC dc(this);
	Draw_DrawAll(&dc,&draw,Project,palette);
}

void CWepMagGraphic::OnLButtonDown(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	BYTE ref = Draw_ButtonDown(&draw,pt,0);
	if(ref & 1){
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);}
	if(ref & 2)
		InvalidateRect(draw.rcFinger);
}

void CWepMagGraphic::OnRButtonDown(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	BYTE ref = Draw_ButtonDown(&draw,pt,1);
	if(ref & 1){
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);}
	if(ref & 2)
		InvalidateRect(draw.rcFinger);
}
void CWepMagGraphic::OnLButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	draw.mousedown = 0;
}
void CWepMagGraphic::OnRButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	draw.mousedown = 0;
}

void CWepMagGraphic::OnMouseMove(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	if(Draw_MouseMove(&draw,pt)){
		InvalidateRect(draw.rcGraphic,0);
		InvalidateRect(draw.rcCloseup,0);}
}

void CWepMagGraphic::SetCaption(CString caption)
{
	m_caption = caption;
}

void CWepMagGraphic::OnExportbitmap() 
{Draw_ExportToBmp(&draw,Project,palette);}

void CWepMagGraphic::OnImportbitmap() 
{
	Draw_ImportFromBmp(&draw,Project,palette);
	InvalidateRect(draw.rcGraphic,0);
	InvalidateRect(draw.rcCloseup,0);
}
