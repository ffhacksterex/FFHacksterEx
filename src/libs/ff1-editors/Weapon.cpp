// Weapon.cpp : implementation file
//

#include "stdafx.h"
#include "Weapon.h"
#include "collection_helpers.h"
#include <copypaste_helpers.h>
#include <core_exceptions.h>
#include "draw_functions.h"
#include "editor_label_functions.h"
#include <FFH2Project.h>
#include "general_functions.h"
#include "imaging_helpers.h"
#include "ingame_text_functions.h"
#include "ini_functions.h"
#include "io_functions.h"
#include "string_functions.h"
#include "ui_helpers.h"
#include <ValueDataAccessor.h>
#include "AsmFiles.h"
#include "GameSerializer.h"
#include "NewLabel.h"
#include "WepMagGraphic.h"

using namespace Editorlabels;
using namespace Ingametext;
using namespace Ini;
using namespace Io;
using namespace Ui;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWeapon dialog
#define BaseClass CEditorWithBackground

CWeapon::CWeapon(CWnd* pParent /*= nullptr */)
	: BaseClass(CWeapon::IDD, pParent)
{
}

void CWeapon::DoDataExchange(CDataExchange* pDX)
{
	BaseClass::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ELEM8, m_elem8);
	DDX_Control(pDX, IDC_ELEM7, m_elem7);
	DDX_Control(pDX, IDC_ELEM6, m_elem6);
	DDX_Control(pDX, IDC_ELEM5, m_elem5);
	DDX_Control(pDX, IDC_ELEM3, m_elem3);
	DDX_Control(pDX, IDC_ELEM4, m_elem4);
	DDX_Control(pDX, IDC_ELEM2, m_elem2);
	DDX_Control(pDX, IDC_ELEM1, m_elem1);
	DDX_Control(pDX, IDC_EDITGFX, m_editgfx);
	DDX_Control(pDX, IDC_WEAPONLIST, m_weaponlist);
	DDX_Control(pDX, IDC_USE9, m_use9);
	DDX_Control(pDX, IDC_USE8, m_use8);
	DDX_Control(pDX, IDC_USE7, m_use7);
	DDX_Control(pDX, IDC_USE6, m_use6);
	DDX_Control(pDX, IDC_USE5, m_use5);
	DDX_Control(pDX, IDC_USE4, m_use4);
	DDX_Control(pDX, IDC_USE3, m_use3);
	DDX_Control(pDX, IDC_USE2, m_use2);
	DDX_Control(pDX, IDC_USE12, m_use12);
	DDX_Control(pDX, IDC_USE11, m_use11);
	DDX_Control(pDX, IDC_USE10, m_use10);
	DDX_Control(pDX, IDC_USE1, m_use1);
	DDX_Control(pDX, IDC_UNKNOWN, m_critrate);
	DDX_Control(pDX, IDC_SPELLCAST, m_spellcast);
	DDX_Control(pDX, IDC_PRICE, m_price);
	DDX_Control(pDX, IDC_HIT, m_hit);
	DDX_Control(pDX, IDC_GFX, m_gfx);
	DDX_Control(pDX, IDC_DAMAGE, m_damage);
	DDX_Control(pDX, IDC_CAT8, m_cat8);
	DDX_Control(pDX, IDC_CAT7, m_cat7);
	DDX_Control(pDX, IDC_CAT6, m_cat6);
	DDX_Control(pDX, IDC_CAT5, m_cat5);
	DDX_Control(pDX, IDC_CAT4, m_cat4);
	DDX_Control(pDX, IDC_CAT3, m_cat3);
	DDX_Control(pDX, IDC_CAT2, m_cat2);
	DDX_Control(pDX, IDC_CAT1, m_cat1);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDC_SAVE, m_savebutton);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDC_EDITLABEL, m_editlabelbutton);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
}


BEGIN_MESSAGE_MAP(CWeapon, BaseClass)
	ON_LBN_SELCHANGE(IDC_WEAPONLIST, OnSelchangeWeaponlist)
	ON_BN_CLICKED(IDC_EDITLABEL, OnEditlabel)
	ON_WM_LBUTTONDOWN()
	ON_CBN_SELCHANGE(IDC_GFX, OnSelchangeGfx)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_EDITGFX, OnEditgfx)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWeapon message handlers

BOOL CWeapon::OnInitDialog()
{
	BaseClass::OnInitDialog();

	ShowWindow(SW_HIDE);

	try {
		this->LoadOffsets();
		this->LoadRom();

		LoadCaptions(std::vector<CWnd*>{ &m_elem1, &m_elem2, &m_elem3, &m_elem4, &m_elem5, &m_elem6, &m_elem7, &m_elem8 }, Labels2::LoadElementLabels(*Proj2));
		LoadCaptions(std::vector<CWnd*>{ &m_cat1, &m_cat2, &m_cat3, &m_cat4, &m_cat5, &m_cat6, &m_cat7, &m_cat8 }, Labels2::LoadEnemyCategoryLabels(*Proj2));

		LoadListBox(m_weaponlist, LoadWeaponEntries(*Proj2));
		LoadCombo(m_spellcast, LoadMagicEntries(*Proj2) + LoadAttackEntries(*Proj2));
		m_spellcast.InsertString(0, "--None--");

		std::vector<CWnd*> classlists = {
			&m_use1,&m_use2,&m_use3,&m_use4,&m_use5,&m_use6,
			&m_use7,&m_use8,&m_use9,&m_use10,&m_use11,&m_use12 };
		ASSERT(classlists.size() == (size_t)CLASS_COUNT);
		classlists.resize(CLASS_COUNT);
		LoadCaptions(classlists, LoadClassEntries(*Proj2));

		LoadCombo(m_gfx, Labels2::LoadWepMagicLabels(*Proj2));
		m_gfx.InsertString(0, "--None--");

		// Position the palette and graphic rectangles relative to the Edit Gfx label button
		CRect rcgfx;
		GetDlgItem(IDC_EDITLABEL)->GetWindowRect(&rcgfx);
		ScreenToClient(&rcgfx);
		rcgfx.OffsetRect(0, rcgfx.Height() + 8);

		rcPalette.SetRect(rcgfx.left, rcgfx.top, rcgfx.left + 256, rcgfx.top + 16);
		rcGraphic = rcPalette;
		rcGraphic.left = rcGraphic.right - 32;
		rcGraphic.top -= 48;
		rcGraphic.bottom -= 32;
		rcFinger = rcPalette;
		rcFinger.top += 16;
		rcFinger.bottom += 16;
		cur_pal = 0;

		m_graphics.Create(32, 32, ILC_COLOR16, 1, 0);

		cur = -1;
		m_weaponlist.SetCurSel(0);
		OnSelchangeWeaponlist();

		m_banner.Set(this, RGB(0, 0, 0), RGB(255, 32, 64), "Weapons");
		SetODDefButtonID(IDOK);
	}
	catch (std::exception & ex) {
		return Ui::AbortInitDialog(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortInitDialog(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}

void CWeapon::LoadOffsets()
{
	ffh::acc::ValueDataAccessor v(*Proj2);
	CLASS_COUNT = v.get<int>("CLASS_COUNT");
	WEAPON_COUNT = v.get<int>("WEAPON_COUNT");
	WEAPON_OFFSET = v.get<int>("WEAPON_OFFSET");
	WEAPON_BYTES = v.get<int>("WEAPON_BYTES");
	WEAPONPRICE_OFFSET = v.get<int>("WEAPONPRICE_OFFSET");
	WEAPONPERMISSIONS_OFFSET = v.get<int>("WEAPONPERMISSIONS_OFFSET");
	WEAPONMAGICGRAPHIC_OFFSET = v.get<int>("WEAPONMAGICGRAPHIC_OFFSET");

	BANK0A_OFFSET = v.get<int>("BANK0A_OFFSET");
	BINBANK09GFXDATA_OFFSET = v.get<int>("BINBANK09GFXDATA_OFFSET");
	BINPRICEDATA_OFFSET = v.get<int>("BINPRICEDATA_OFFSET");
}

void CWeapon::LoadRom()
{
	Proj2->ClearROM();
	// Now load the data
	if (Proj2->IsRom()) {
		Proj2->LoadROM();
	}
	else if (Proj2->IsAsm()) {
		GameSerializer ser(*Proj2);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.LoadAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.LoadAsmBin(BIN_BANK09GFXDATA, BINBANK09GFXDATA_OFFSET);
		ser.LoadAsmBin(BIN_WEAPONDATA, WEAPON_OFFSET);
		ser.LoadAsmBin(BIN_PRICEDATA, BINPRICEDATA_OFFSET);
		ser.LoadInline(ASM_0E, { { asmlabel, LUT_WEAPONPERMISSIONS, {WEAPONPERMISSIONS_OFFSET} } });
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, Proj2->info.type);
	}
}

void CWeapon::SaveRom()
{
	if (Proj2->IsRom()) {
		Proj2->SaveROM();
	}
	else if (Proj2->IsAsm()) {
		GameSerializer ser(*Proj2);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.SaveAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.SaveAsmBin(BIN_BANK09GFXDATA, BINBANK09GFXDATA_OFFSET);
		ser.SaveAsmBin(BIN_WEAPONDATA, WEAPON_OFFSET);
		ser.SaveAsmBin(BIN_PRICEDATA, BINPRICEDATA_OFFSET);
		ser.SaveInline(ASM_0E, { { asmlabel, LUT_WEAPONPERMISSIONS,{ WEAPONPERMISSIONS_OFFSET } } });
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, Proj2->info.type);
	}
}

void CWeapon::LoadValues()
{
	int temp;
	CString text;
	int offset = WEAPON_OFFSET + (cur * WEAPON_BYTES);

	text.Format("%d", Proj2->ROM[offset]);
	m_hit.SetWindowText(text);

	text.Format("%d", Proj2->ROM[offset + 1]);
	m_damage.SetWindowText(text);

	//REMOVE - critrate no longer expressed as hex
	//text.Format("%X", Proj2->ROM[offset + 2]);
	//if (text.GetLength() == 1) text = "0" + text;
	text.Format("%d", Proj2->ROM[offset + 2]);
	m_critrate.SetWindowText(text);

	temp = Proj2->ROM[offset + 3];
	if (temp >= 0x43) temp -= 0x02;
	m_spellcast.SetCurSel(temp);

	temp = Proj2->ROM[offset + 4];
	SetCheckFlags(temp, std::vector<CStrikeCheck*>{ &m_elem1, &m_elem2, &m_elem3, &m_elem4, &m_elem5, &m_elem6, &m_elem7, &m_elem8 });

	temp = Proj2->ROM[offset + 5];
	SetCheckFlags(temp, std::vector<CStrikeCheck*>{ &m_cat1,&m_cat2,&m_cat3,&m_cat4,&m_cat5,&m_cat6,&m_cat7,&m_cat8 });

	temp = Proj2->ROM[offset + 6];
	if (temp > 0) {
		temp -= 0x80;
		temp >>= 2;
		temp += 1;
		m_gfx.SetCurSel(temp);
	}
	else m_gfx.SetCurSel(0);

	m_editgfx.EnableWindow(m_gfx.GetCurSel());

	cur_pal = Proj2->ROM[offset + 7] - 0x20;

	offset = WEAPONPRICE_OFFSET + (cur << 1);
	temp = Proj2->ROM[offset] + (Proj2->ROM[offset + 1] << 8);
	text.Format("%d", temp);
	m_price.SetWindowText(text);

	//N.B. - the check associates class 0 with the highest bit, and equips if flags are cleared.
	//	Therefore, bitwise-flip the value, then loop the classes in reverse order.
	offset = WEAPONPERMISSIONS_OFFSET + (cur << 1);
	temp = Proj2->ROM[offset] + (Proj2->ROM[offset + 1] << 8);
	temp = (~temp & 0xFFF);
	SetCheckFlags(temp, std::vector<CStrikeCheck*>{ &m_use12, &m_use11, &m_use10, &m_use9, &m_use8, &m_use7, &m_use6, &m_use5, &m_use4, &m_use3, &m_use2, &m_use1});

	ResetGraphicList();
	InvalidateRect(rcFinger);
}

void CWeapon::StoreValues()
{
	int temp;
	CString text;
	int offset = WEAPON_OFFSET + (cur * WEAPON_BYTES);

	m_hit.GetWindowText(text); temp = StringToInt(text);
	if (temp > 0xFF) temp = 0xFF; Proj2->ROM[offset] = (BYTE)temp;

	m_damage.GetWindowText(text); temp = StringToInt(text);
	if (temp > 0xFF) temp = 0xFF; Proj2->ROM[offset + 1] = (BYTE)temp;

	m_critrate.GetWindowText(text); temp = StringToInt(text);
	//REMOVE - critrate no longer expressed as hex
	//temp = StringToInt_HEX(text);
	if (temp > 0xFF) temp = 0xFF; Proj2->ROM[offset + 2] = (BYTE)temp;

	temp = m_spellcast.GetCurSel();
	if (temp >= 0x41) temp += 0x02;
	Proj2->ROM[offset + 3] = (BYTE)temp;

	temp = GetCheckFlags(std::vector<CStrikeCheck*>{ &m_elem1, &m_elem2, &m_elem3, &m_elem4, &m_elem5, &m_elem6, &m_elem7, &m_elem8 });
	Proj2->ROM[offset + 4] = (BYTE)temp;

	temp = GetCheckFlags(std::vector<CStrikeCheck*>{ &m_cat1, &m_cat2, &m_cat3, &m_cat4, &m_cat5, &m_cat6, &m_cat7, &m_cat8 });
	Proj2->ROM[offset + 5] = (BYTE)temp;

	temp = m_gfx.GetCurSel() << 2;
	if (temp) temp += 0x7C;
	Proj2->ROM[offset + 6] = (BYTE)temp;

	Proj2->ROM[offset + 7] = (BYTE)(cur_pal + 0x20);

	offset = WEAPONPRICE_OFFSET + (cur << 1);
	m_price.GetWindowText(text);
	temp = StringToInt(text); if (temp > 0xFFFF) temp = 0xFFFF;
	Proj2->ROM[offset] = temp & 0xFF;
	Proj2->ROM[offset + 1] = (BYTE)(temp >> 8);

	//N.B. - the check associates class 0 with the highest bit, and equips if flags are cleared.
	//	Therefore, bitwise-flip the value, then loop the classes in reverse order.
	temp = GetCheckFlags(std::vector<CStrikeCheck*>{ &m_use12, &m_use11, &m_use10, &m_use9, &m_use8, &m_use7, &m_use6, &m_use5, &m_use4, &m_use3, &m_use2, &m_use1});
	temp = (~temp & 0xFFF);
	offset = WEAPONPERMISSIONS_OFFSET + (cur << 1);
	Proj2->ROM[offset] = temp & 0xFF;
	Proj2->ROM[offset + 1] = (BYTE)(temp >> 8);
}

void CWeapon::PaintClient(CDC & dc)
{
	CRect rcpalborder = rcPalette;
	rcpalborder.InflateRect(2, 2);
	auto oldpen = dc.SelectStockObject(BLACK_PEN);
	auto oldbr = dc.SelectStockObject(HOLLOW_BRUSH);
	dc.Rectangle(&rcpalborder);
	dc.SelectObject(oldbr);
	dc.SelectObject(oldpen);

	CBrush br;
	CRect temp = rcPalette;
	temp.right = temp.left + 16;
	for (int co = 0; co < 16; co++, temp.left += 16, temp.right += 16) {
		br.CreateSolidBrush(Proj2->palette[0][32 + co]);
		dc.FillRect(temp, &br);
		br.DeleteObject();
	}

	CPoint pt(rcFinger.left + (cur_pal << 4), rcFinger.top);
	Proj2->Finger.Draw(&dc, 1, pt, ILD_TRANSPARENT);

	if (m_gfx.GetCurSel()) {
		pt.x = rcGraphic.left;
		pt.y = rcGraphic.top;
		m_graphics.Draw(&dc, 0, pt, ILD_TRANSPARENT);
	}
	else {
		br.CreateSolidBrush(Proj2->palette[0][0x0F]);
		dc.FillRect(rcGraphic, &br);
		br.DeleteObject();
	}
}

void CWeapon::ResetGraphicList()
{
	while(m_graphics.GetImageCount()) m_graphics.Remove(0);
	CBitmap bmp;
	CPaintDC dc(this);
	CDC mDC; mDC.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc,32,32);
	mDC.SelectObject(&bmp);

	int offset = WEAPONMAGICGRAPHIC_OFFSET + ((m_gfx.GetCurSel() - 1) << 6);

	BYTE palette[4] = { 0x0F, (BYTE)(0x20 + cur_pal),(BYTE)(0x10 + cur_pal),(BYTE)cur_pal };

	BYTE coX, coY;
	for(coY = 0; coY < 32; coY += 16){
	for(coX = 0; coX < 32; coX += 16, offset += 16){
		DrawTileScale(&mDC,coX,coY,Proj2,offset,palette,2);}}

	mDC.DeleteDC();
	m_graphics.Add(&bmp,RGB(0,0,0));
	bmp.DeleteObject();

	InvalidateRect(rcGraphic,0);
}

void CWeapon::HandleWeaponListContextMenu(CWnd* pWnd, CPoint point)
{
	using namespace copypaste_helpers;
	auto optionnames = mfcstringvector{ "Name", "Hit%", "Damage", "Critical%", "Spell", "Element",
		"Category", "Graphic", "Palette", "Price", "Equip" };
	auto result = InvokeCopySwap(m_weaponlist, point, m_selitem, optionnames);
	switch (result.selcmd) {
	case ID_FFH_COPY:
		m_selitem = result.copyindex;
		break;
	case ID_FFH_PASTE:
	case ID_FFH_SWAP:
	{
		bool swap = result.selcmd == ID_FFH_SWAP;
		auto thisitem = result.thisindex;
		auto& flags = result.flags;
		boolvector wepflags(WEAPON_BYTES);
		std::copy_n(cbegin(flags) + 1, WEAPON_BYTES, begin(wepflags));

		CopySwapBytes(swap, Proj2->ROM, m_selitem, thisitem, WEAPON_OFFSET, WEAPON_BYTES, 0, wepflags);
		if (flags[0]) DoCopySwapName(swap, m_selitem, thisitem);
		if (flags[9]) CopySwapBuffer(swap, Proj2->ROM, m_selitem, thisitem, WEAPONPRICE_OFFSET, 2, 0, 2);
		if (flags[10]) CopySwapBuffer(swap, Proj2->ROM, m_selitem, thisitem, WEAPONPERMISSIONS_OFFSET, 2, 0, 2);

		m_weaponlist.SetCurSel(cur = thisitem);
		LoadValues();
		break;
	}
	default:
		if (!result.message.IsEmpty())
			throw std::runtime_error((LPCSTR)result.message);
		break;
	}
}

void CWeapon::DoCopySwapName(bool swap, int srcitem, int dstitem)
{
	try {
		Ingametext::PasteSwapStringBytes(swap, *Proj2, WEAPONS, srcitem, dstitem);

		CString srcname = LoadWeaponEntry(*Proj2, srcitem + 1).name.Trim();
		CString dstname = LoadWeaponEntry(*Proj2, dstitem + 1).name.Trim();

		// Now, reload the class names in the list box
		Ui::ReplaceString(m_weaponlist, srcitem, srcname);
		Ui::ReplaceString(m_weaponlist, dstitem, dstname);
	}
	catch (std::exception& ex) {
		throw std::runtime_error("Copy/Swap operation failed: " + std::string(ex.what()));
	}
}

void CWeapon::OnSelchangeWeaponlist() 
{
	if(cur != -1) StoreValues();

	cur = m_weaponlist.GetCurSel();

	LoadValues();
}

void CWeapon::OnEditlabel() 
{
	int temp = m_gfx.GetCurSel();
	//ChangeLabel(*Proj2, -1, LoadWepMagicLabel(*Proj2, temp - 1), WriteWepMagicLabel, temp, nullptr, &m_gfx); //TODO - ChangeLabel
}

void CWeapon::OnLButtonDown(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	if(PtInRect(rcPalette,pt)){
		cur_pal = (pt.x - rcPalette.left) >> 4;
		ResetGraphicList();
		InvalidateRect(rcFinger);
	}
	HandleLbuttonDrag(this);
}

void CWeapon::OnSelchangeGfx() 
{
	ResetGraphicList();
	m_editgfx.EnableWindow(m_gfx.GetCurSel());
}

void CWeapon::OnEditgfx()
{
	CWepMagGraphic dlg;
	dlg.Proj2 = Proj2;
	dlg.paletteref = (BYTE)cur_pal;
	dlg.graphic = (BYTE)(m_gfx.GetCurSel() - 1);
	dlg.IsWeapon = 1;
	dlg.DoModal();
	InvalidateRect(rcPalette,0);
	ResetGraphicList();
}

void CWeapon::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (pWnd == &m_weaponlist) HandleWeaponListContextMenu(pWnd, point);
}
