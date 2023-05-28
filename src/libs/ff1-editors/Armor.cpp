// Armor.cpp : implementation file
//

#include "stdafx.h"
#include "Armor.h"
#include "AsmFiles.h"
#include "collection_helpers.h"
#include <core_exceptions.h>
#include <ValueDataAccessor.h>
#include <dva_primitives.h>
#include <editor_label_functions.h>
#include <FFH2Project.h>
#include "GameSerializer.h"
#include "general_functions.h"
#include "imaging_helpers.h"
#include "ingame_text_functions.h"
#include "ini_functions.h"
#include "io_functions.h"
#include "string_functions.h"
#include "ui_helpers.h"
#include "copypaste_helpers.h"
#include <DlgPasteTargets.h>

using namespace Ingametext;
using namespace Ini;
using namespace Io;
using namespace Editorlabels;
using namespace Ui;
using namespace Imaging;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArmor dialog
#define BaseClass CEditorWithBackground


CArmor::CArmor(CWnd* pParent /*= nullptr */)
	: BaseClass(CArmor::IDD, pParent)
{
}

void CArmor::DoDataExchange(CDataExchange* pDX)
{
	BaseClass::DoDataExchange(pDX);
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
	DDX_Control(pDX, IDC_PRICE, m_price);
	DDX_Control(pDX, IDC_EVADE, m_evade);
	DDX_Control(pDX, IDC_DEF8, m_def8);
	DDX_Control(pDX, IDC_DEF7, m_def7);
	DDX_Control(pDX, IDC_DEF6, m_def6);
	DDX_Control(pDX, IDC_DEF5, m_def5);
	DDX_Control(pDX, IDC_DEF4, m_def4);
	DDX_Control(pDX, IDC_DEF3, m_def3);
	DDX_Control(pDX, IDC_DEF2, m_def2);
	DDX_Control(pDX, IDC_DEF1, m_def1);
	DDX_Control(pDX, IDC_DEF, m_def);
	DDX_Control(pDX, IDC_SPELLCAST, m_spellcast);
	DDX_Control(pDX, IDC_ARMORTYPE, m_armortype);
	DDX_Control(pDX, IDC_ARMORLIST, m_armorlist);
	DDX_Control(pDX, IDC_STATIC1, m_static1);
	DDX_Control(pDX, IDC_STATIC2, m_static2);
	DDX_Control(pDX, IDC_STATIC3, m_static3);
	DDX_Control(pDX, IDC_STATIC4, m_static4);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDC_SAVE, m_savebutton);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
}


BEGIN_MESSAGE_MAP(CArmor, BaseClass)
	ON_LBN_SELCHANGE(IDC_ARMORLIST, OnSelchangeArmorlist)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArmor message handlers

BOOL CArmor::OnInitDialog() 
{
	BaseClass::OnInitDialog();

	try {
		this->LoadOffsets();
		this->LoadRom();

		std::vector<CWnd*> classlists = {
			&m_use1,&m_use2,&m_use3,&m_use4,&m_use5,&m_use6,
			&m_use7,&m_use8,&m_use9,&m_use10,&m_use11,&m_use12 };
		ASSERT(classlists.size() == (size_t)CLASS_COUNT);

		LoadCaptions(std::vector<CWnd*>{ &m_def1, & m_def2, & m_def3, & m_def4, & m_def5, & m_def6, & m_def7, & m_def8 }, Labels2::LoadElementLabels(*Proj2));

		LoadListBox(m_armorlist, LoadArmorEntries(*Proj2));
		LoadCombo(m_spellcast, LoadMagicEntries(*Proj2) + LoadAttackEntries(*Proj2));
		m_spellcast.InsertString(0, "--None--");
		LoadCombo(m_armortype, Labels2::LoadArmorTypes(*Proj2));
		LoadCaptions(classlists, LoadClassEntries(*Proj2));

		cur = -1;
		m_armorlist.SetCurSel(0);
		OnSelchangeArmorlist();

		m_banner.Set(this, COLOR_BLACK, COLOR_FFRED, "Armor");
		SetODDefButtonID(IDOK);
	}
	catch (std::exception & ex) {
		return AbortInitDialog(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return AbortInitDialog(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}

void CArmor::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (pWnd == &m_armorlist) HandleArmorListContextMenu(pWnd, point);
}

void CArmor::OnSelchangeArmorlist() 
{
	if(cur != -1) StoreValues();

	cur = m_armorlist.GetCurSel();

	LoadValues();
}

void CArmor::LoadOffsets()
{
	ffh::acc::ValueDataAccessor d(*Proj2);
	CLASS_COUNT = d.get<int>("CLASS_COUNT");
	ARMORTEXT_OFFSET = d.get<int>("ARMORTEXT_OFFSET");
	BASICTEXT_PTRADD = d.get<int>("BASICTEXT_PTRADD");
	ARMOR_COUNT = d.get<int>("ARMOR_COUNT");
	ATTACKTEXT_OFFSET = d.get<int>("ATTACKTEXT_OFFSET");
	ATTACKTEXT_PTRADD = d.get<int>("ATTACKTEXT_PTRADD");
	ATTACK_COUNT = d.get<int>("ATTACK_COUNT");
	MAGICTEXT_OFFSET = d.get<int>("MAGICTEXT_OFFSET");
	MAGIC_COUNT = d.get<int>("MAGIC_COUNT");
	CLASSTEXT_OFFSET = d.get<int>("CLASSTEXT_OFFSET");
	ARMOR_OFFSET = d.get<int>("ARMOR_OFFSET");
	ARMOR_BYTES = d.get<int>("ARMOR_BYTES");
	ARMORPRICE_OFFSET = d.get<int>("ARMORPRICE_OFFSET");
	ARMORPERMISSIONS_OFFSET = d.get<int>("ARMORPERMISSIONS_OFFSET");
	ARMORTYPE_OFFSET = d.get<int>("ARMORTYPE_OFFSET");
	BANK0A_OFFSET = d.get<int>("BANK0A_OFFSET");
	BINPRICEDATA_OFFSET = d.get<int>("BINPRICEDATA_OFFSET");
}

void CArmor::LoadRom()
{
	Proj2->ClearROM();
	if (Proj2->IsRom())
	{
		Proj2->LoadROM();
	}
	else if (Proj2->IsAsm())
	{
		GameSerializer ser(*Proj2);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.LoadAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.LoadAsmBin(BIN_ARMORDATA, ARMOR_OFFSET);
		ser.LoadAsmBin(BIN_PRICEDATA, BINPRICEDATA_OFFSET);
		ser.LoadInline(ASM_0E, {
			{ asmlabel, "lut_ArmorTypes",{ ARMORTYPE_OFFSET } },
			{ asmlabel, LUT_ARMORPERMISSIONS,{ ARMORPERMISSIONS_OFFSET } },
			});
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, Proj2->info.type);
	}
}

void CArmor::SaveRom()
{
	if (Proj2->IsRom()) {
		Proj2->SaveROM();
	}
	else if (Proj2->IsAsm()) {
		GameSerializer ser(*Proj2);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.SaveAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.SaveAsmBin(BIN_ARMORDATA, ARMOR_OFFSET);
		ser.SaveAsmBin(BIN_PRICEDATA, BINPRICEDATA_OFFSET);
		ser.SaveInline(ASM_0E, {
			{ asmlabel, "lut_ArmorTypes",{ ARMORTYPE_OFFSET } },
			{ asmlabel, LUT_ARMORPERMISSIONS,{ ARMORPERMISSIONS_OFFSET } }
			});
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, Proj2->info.type);
	}
}

void CArmor::LoadValues()
{
	int offset = ARMOR_OFFSET + (ARMOR_BYTES * cur);
	int temp;
	CString text;
	const auto& rom = Proj2->ROM;

	temp = rom[offset];
	text.Format("%d",temp);
	m_evade.SetWindowText(text);

	temp = rom[offset + 1];
	text.Format("%d",temp);
	m_def.SetWindowText(text);

	temp = rom[offset + 2];
	SetCheckFlags(temp, std::vector<CStrikeCheck*>{ &m_def1, &m_def2, &m_def3, &m_def4, &m_def5, &m_def6, &m_def7, &m_def8 });

	temp = rom[offset + 3];
	if(temp >= 0x43) temp -= 0x02;
	m_spellcast.SetCurSel(temp);

	offset = ARMORPRICE_OFFSET + (cur << 1);
	temp = rom[offset] + (rom[offset + 1] << 8);
	text.Format("%d",temp);
	m_price.SetWindowText(text);

	offset = ARMORTYPE_OFFSET + cur;
	m_armortype.SetCurSel(rom[offset]);

	offset = ARMORPERMISSIONS_OFFSET + (cur << 1);
	temp = rom[offset] + (rom[offset + 1] << 8);
	//N.B. - the check associates class 0 with the highest bit, and equips if flags are cleared.
	//	Therefore, bitwise-flip the value, then loop the classes in reverse order.
	temp = (~temp & 0xFFF);
	SetCheckFlags(temp, std::vector<CStrikeCheck*>{ &m_use12, &m_use11, &m_use10, &m_use9, &m_use8, &m_use7, &m_use6, &m_use5, &m_use4, &m_use3, &m_use2, &m_use1});
}

void CArmor::StoreValues()
{
	int offset = ARMOR_OFFSET + (ARMOR_BYTES * cur);
	int temp;
	CString text;
	auto & rom = Proj2->ROM;

	m_evade.GetWindowText(text);
	temp = StringToInt(text); if(temp > 0xFF) temp = 0xFF;
	rom[offset] = (BYTE)temp;

	m_def.GetWindowText(text);
	temp = StringToInt(text); if(temp > 0xFF) temp = 0xFF;
	rom[offset + 1] = (BYTE)temp;

	temp = GetCheckFlags(std::vector<CStrikeCheck*>{ &m_def1, &m_def2, &m_def3, &m_def4, &m_def5, &m_def6, &m_def7, &m_def8 });
	rom[offset + 2] = (BYTE)temp;

	temp = m_spellcast.GetCurSel();
	if(temp >= 0x41) temp += 0x02;
	rom[offset + 3] = (BYTE)temp;

	offset = ARMORPRICE_OFFSET + (cur << 1);
	m_price.GetWindowText(text);
	temp = StringToInt(text); if(temp > 0xFFFF) temp = 0xFFFF;
	rom[offset] = temp & 0xFF;
	rom[offset + 1] = (BYTE)(temp >> 8);

	offset = ARMORTYPE_OFFSET + cur;
	temp = m_armortype.GetCurSel();
	rom[offset] = (BYTE)temp;
	
	//N.B. - the check associates class 0 with the highest bit, and equips if flags are cleared.
	//	Therefore, loop the classes in reverse order, then bitwise-flip the value.
	temp = GetCheckFlags(std::vector<CStrikeCheck*>{ &m_use12, &m_use11, &m_use10, &m_use9, &m_use8, &m_use7, &m_use6, &m_use5, &m_use4, &m_use3, &m_use2, &m_use1});
	temp = (~temp & 0xFFF);

	offset = ARMORPERMISSIONS_OFFSET + (cur << 1);
	rom[offset] = temp & 0xFF;
	rom[offset + 1] = (BYTE)(temp >> 8);
}

void CArmor::HandleArmorListContextMenu(CWnd* pWnd, CPoint point)
{
	using namespace copypaste_helpers;
	auto optionnames = mfcstringvector{ "Name", "Evade", "Absorb", "Resist", "Spell", "Price", "Equip", "Armor Type" };
	auto result = InvokeCopySwap(m_armorlist, point, m_selitem, optionnames);
	switch (result.selcmd) {
	case ID_FFH_COPY:
		m_selitem = result.copyindex;
		break;
	case ID_FFH_PASTE:
	case ID_FFH_SWAP:
	{
		bool swap = result.selcmd == ID_FFH_SWAP;
		auto thisitem = result.thisindex;
		auto & flags = result.flags;
		boolvector armorflags(ARMOR_BYTES);
		std::copy_n(cbegin(flags) + 1, ARMOR_BYTES, begin(armorflags));

		auto & rom = Proj2->ROM;
		CopySwapBytes(swap, rom, m_selitem, thisitem, ARMOR_OFFSET, ARMOR_BYTES, 0, armorflags);
		if (flags[0]) DoCopySwapName(swap, m_selitem, thisitem);
		if (flags[5]) CopySwapBuffer(swap, rom, m_selitem, thisitem, ARMORPRICE_OFFSET, 2, 0, 2);
		if (flags[6]) CopySwapBuffer(swap, rom, m_selitem, thisitem, ARMORPERMISSIONS_OFFSET, 2, 0, 2);
		if (flags[7]) CopySwapBuffer(swap, rom, m_selitem, thisitem, ARMORTYPE_OFFSET, 1, 0, 1);

		m_armorlist.SetCurSel(cur = thisitem);
		LoadValues();
		break;
	}
	default:
		if (!result.message.IsEmpty())
			throw std::runtime_error((LPCSTR)result.message);
		break;
	}
}

void CArmor::DoCopySwapName(bool swap, int srcitem, int dstitem)
{
	try {
		Ingametext::PasteSwapStringBytes(swap, *Proj2, ARMOR, srcitem, dstitem);

		CString srcname = LoadArmorEntry(*Proj2, srcitem + 1).name.Trim();
		CString dstname = LoadArmorEntry(*Proj2, dstitem + 1).name.Trim();

		// Now, reload the class names in the list box
		Ui::ReplaceString(m_armorlist, srcitem, srcname);
		Ui::ReplaceString(m_armorlist, dstitem, dstname);
	} catch(std::exception & ex) {
		throw std::runtime_error("Copy/Swap operation failed: " + std::string(ex.what()));
	}
}