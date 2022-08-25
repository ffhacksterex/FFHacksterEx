// StartingItems.cpp : implementation file
//

#include "stdafx.h"
#include "resource_editors.h"
#include "StartingItems.h"
#include "afxdialogex.h"
#include "ui_helpers.h"
#include "GameSerializer.h"
#include "AsmFiles.h"
#include "general_functions.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "ingame_text_functions.h"
#include "io_functions.h"
#include "ui_helpers.h"
#include "imaging_helpers.h"

using namespace Ini;
using namespace Ingametext;
using namespace Ui;
using namespace Imaging;
using namespace Io;

// CStartingItems dialog
#define BaseClass CEditorWithBackground


IMPLEMENT_DYNAMIC(CStartingItems, BaseClass)

CStartingItems::CStartingItems(CWnd* pParent /*= nullptr */)
	: BaseClass(IDD_STARTINGITEMS, pParent)
{
}

CStartingItems::~CStartingItems()
{
}

void CStartingItems::DoDataExchange(CDataExchange* pDX)
{
	BaseClass::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STARTITEMS_COMBO_BRIDGE, m_comboBridge);
	DDX_Control(pDX, IDC_STARTITEMS_CHECK_HASCANOEFLAG, m_checkHasCanoe);
	DDX_Control(pDX, IDC_STARTITEMS_EDIT_CONSUM1, m_comumableedit1);
	DDX_Control(pDX, IDC_STARTITEMS_EDIT_CONSUM2, m_comumableedit2);
	DDX_Control(pDX, IDC_STARTITEMS_EDIT_CONSUM3, m_comumableedit3);
	DDX_Control(pDX, IDC_STARTITEMS_EDIT_CONSUM4, m_comumableedit4);
	DDX_Control(pDX, IDC_STARTITEMS_EDIT_CONSUM5, m_comumableedit5);
	DDX_Control(pDX, IDC_STARTITEMS_EDIT_CONSUM6, m_comumableedit6);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDC_SAVE, m_savebutton);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
	DDX_Control(pDX, IDC_STARTITEMS_CHECK_SHIPVIS, m_checkHasShip);
	DDX_Control(pDX, IDC_STARTITEMS_CHECK_AIRSHIPVIS, m_checkHasAirship);
	DDX_Control(pDX, IDC_STARTITEMS_CHECK_BRIDGEVIS, m_checkHasBridge);
	DDX_Control(pDX, IDC_STARTITEMS_CHECK_CANALVIS, m_checkHasCanal);
}

BOOL CStartingItems::OnInitDialog()
{
	BaseClass::OnInitDialog();

	try {
		this->LoadOffsets();
		this->LoadRom();

		m_itemids = { IDC_STARTITEMS_CHECK_1,IDC_STARTITEMS_CHECK_2,IDC_STARTITEMS_CHECK_3,IDC_STARTITEMS_CHECK_4,
			IDC_STARTITEMS_CHECK_5,IDC_STARTITEMS_CHECK_6,IDC_STARTITEMS_CHECK_7,IDC_STARTITEMS_CHECK_8,
			IDC_STARTITEMS_CHECK_9,IDC_STARTITEMS_CHECK_10,IDC_STARTITEMS_CHECK_11,IDC_STARTITEMS_CHECK_12,
			IDC_STARTITEMS_CHECK_13,IDC_STARTITEMS_CHECK_14,IDC_STARTITEMS_CHECK_15,IDC_STARTITEMS_CHECK_16,
			IDC_STARTITEMS_CHECK_17
		};
		m_comsumeids = { IDC_STARTITEMS_EDIT_CONSUM1, IDC_STARTITEMS_EDIT_CONSUM2, IDC_STARTITEMS_EDIT_CONSUM3,
			IDC_STARTITEMS_EDIT_CONSUM4,IDC_STARTITEMS_EDIT_CONSUM5,IDC_STARTITEMS_EDIT_CONSUM6 };
		m_consumelabelids = { IDC_STARTITEMS_STATIC_CONSUM1, IDC_STARTITEMS_STATIC_CONSUM2, IDC_STARTITEMS_STATIC_CONSUM3,
			IDC_STARTITEMS_STATIC_CONSUM4,IDC_STARTITEMS_STATIC_CONSUM5,IDC_STARTITEMS_STATIC_CONSUM6 };

		AddEntry(m_comboBridge, "Upon stepping on the bridge (normal setting)", 0);
		AddEntry(m_comboBridge, "When overworld first displays", 1);
		AddEntry(m_comboBridge, "Never", 0x80);

		LoadValues();

		m_banner.Set(this, RGB(0, 0, 0), RGB(255, 32, 64), "Starting Items");
	}
	catch (std::exception & ex) {
		return Ui::AbortInitDialog(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortInitDialog(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CStartingItems::LoadOffsets()
{
	HASCANOE_OFFSET = ReadHex(Project->ValuesPath, "HASCANOE_OFFSET");
	SHIPVIS_OFFSET = ReadHex(Project->ValuesPath, "SHIPVIS_OFFSET");
	AIRSHIPVIS_OFFSET = ReadHex(Project->ValuesPath, "AIRSHIPVIS_OFFSET");
	BRIDGEVIS_OFFSET = ReadHex(Project->ValuesPath, "BRIDGEVIS_OFFSET");
	CANALVIS_OFFSET = ReadHex(Project->ValuesPath, "CANALVIS_OFFSET");

	BRIDGESCENE_OFFSET = ReadHex(Project->ValuesPath, "BRIDGESCENE_OFFSET");
	STARTINGITEMS_OFFSET = ReadHex(Project->ValuesPath, "STARTINGITEMS_OFFSET");
	STARTINGCONSUMABLES_OFFSET = ReadHex(Project->ValuesPath, "STARTINGCONSUMABLES_OFFSET");

	BANK00_OFFSET = ReadHex(Project->ValuesPath, "BANK00_OFFSET");
	BANK0A_OFFSET = ReadHex(Project->ValuesPath, "BANK0A_OFFSET");
}

void CStartingItems::LoadRom()
{
	Project->ClearROM();
	if (Project->IsRom()) {
		load_binary(Project->WorkRomPath, Project->ROM);
	}
	else if (Project->IsAsm()) {
		GameSerializer ser(*Project);
		ser.LoadAsmBin(BANK_00, BANK00_OFFSET);
		ser.LoadAsmBin(BANK_0A, BANK0A_OFFSET);
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, (LPCSTR)Project->ProjectTypeName);
	}
}

void CStartingItems::SaveRom()
{
	if (Project->IsRom()) {
		save_binary(Project->WorkRomPath, Project->ROM);
	}
	else if (Project->IsAsm()) {
		GameSerializer ser(*Project);
		ser.SaveAsmBin(BANK_00, BANK00_OFFSET);
		ser.SaveAsmBin(BANK_0A, BANK0A_OFFSET);
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, (LPCSTR)Project->ProjectTypeName);
	}
}

void CStartingItems::LoadValues()
{
	int selindex = FindIndexByData(m_comboBridge, Project->ROM[BRIDGESCENE_OFFSET]);
	m_comboBridge.SetCurSel(__max(0, selindex));

	SetCheckValue(m_checkHasCanoe, Project->ROM[HASCANOE_OFFSET] == 1);
	SetCheckValue(m_checkHasShip, Project->ROM[SHIPVIS_OFFSET] == 1);
	SetCheckValue(m_checkHasAirship, Project->ROM[AIRSHIPVIS_OFFSET] == 1);
	SetCheckValue(m_checkHasBridge, Project->ROM[BRIDGEVIS_OFFSET] == 1);
	SetCheckValue(m_checkHasCanal, Project->ROM[CANALVIS_OFFSET] == 1);

	int offset = STARTINGITEMS_OFFSET;
	for (size_t st = 0; st < m_itemids.size(); ++st) {
		auto id = m_itemids[st];
		CButton* check = (CButton*)GetDlgItem(id);
		if (check != nullptr) {
			bool checked = Project->ROM[offset + st] > 0;
			SetCheckValue(*check, checked);

			CString itemname = LoadItemEntry(*Project, (int)st + 1).name.TrimRight();
			if (!itemname.IsEmpty()) check->SetWindowText(itemname);
		}
	}

	offset = STARTINGCONSUMABLES_OFFSET;
	for (size_t st = 0; st < m_comsumeids.size(); ++st) {
		auto id = m_comsumeids[st];
		CEdit* edit = (CEdit*)GetDlgItem(id);
		if (edit != nullptr) {
			int value = Project->ROM[offset + st];
			CString fmt;
			fmt.Format("%d", value);
			edit->SetWindowText(fmt);
		}

		auto labelid = m_consumelabelids[st];
		CStatic* stat = (CStatic*)GetDlgItem(labelid);
		if (stat != nullptr) {
			int nameindex = (offset + (int)st + 1) - STARTINGITEMS_OFFSET;
			CString itemname = LoadItemEntry(*Project, nameindex).name.TrimRight();
			if (!itemname.IsEmpty()) stat->SetWindowText(itemname);
		}
	}
}

void CStartingItems::StoreValues()
{
	auto seldata = GetSelectedItemData(m_comboBridge);
	if (seldata != -1)
		Project->ROM[BRIDGESCENE_OFFSET] = (unsigned char)seldata;

	Project->ROM[HASCANOE_OFFSET] = GetCheckValue(m_checkHasCanoe) ? 1 : 0;
	Project->ROM[SHIPVIS_OFFSET] = GetCheckValue(m_checkHasShip) ? 1 : 0;
	Project->ROM[AIRSHIPVIS_OFFSET] = GetCheckValue(m_checkHasAirship) ? 1 : 0;
	Project->ROM[BRIDGEVIS_OFFSET] = GetCheckValue(m_checkHasBridge) ? 1 : 0;
	Project->ROM[CANALVIS_OFFSET] = GetCheckValue(m_checkHasCanal) ? 1 : 0;

	int offset = STARTINGITEMS_OFFSET;
	for (size_t st = 0; st < m_itemids.size(); ++st) {
		auto id = m_itemids[st];
		CButton* check = (CButton*)GetDlgItem(id);
		if (check != nullptr) {
			bool checked = GetCheckValue(*check);
			Project->ROM[offset + st] = checked ? 1 : 0;
		}
	}

	offset = STARTINGCONSUMABLES_OFFSET;
	for (size_t st = 0; st < m_comsumeids.size(); ++st) {
		auto id = m_comsumeids[st];
		CEdit* edit = (CEdit*)GetDlgItem(id);
		if (edit != nullptr) {
			CString fmt;
			edit->GetWindowText(fmt);
			int value = atol(fmt);
			if (value > 99) value = 99;
			else if (value < 0) value = 0;
			Project->ROM[offset + st] = (unsigned char)value;
		}
	}
}


BEGIN_MESSAGE_MAP(CStartingItems, BaseClass)
	ON_BN_CLICKED(IDC_SAVE, &CStartingItems::OnSave)
END_MESSAGE_MAP()


// CStartingItems message handlers

