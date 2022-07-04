// Magic.cpp : implementation file
//

#include "stdafx.h"
#include "Magic.h"
#include "AsmFiles.h"
#include "collection_helpers.h"
#include "DRAW_STRUCT.h"
#include "draw_functions.h"
#include "editor_label_functions.h"
#include "editor_label_defaults.h"
#include "FFHacksterProject.h"
#include "GameSerializer.h"
#include "general_functions.h"
#include "imaging_helpers.h"
#include "ingame_text_functions.h"
#include "ini_functions.h"
#include "io_functions.h"
#include "path_functions.h"
#include "string_functions.h"
#include "ui_helpers.h"
#include "DlgPasteTargets.h"
#include "MagicEditorSettings.h"
#include "MagicEditorSettingsDlg.h"
#include "NewLabel.h"
#include "WepMagGraphic.h"

using namespace Editorlabels;
using namespace Imaging;
using namespace Ingametext;
using namespace Ini;
using namespace Io;
using namespace Ui;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define MAGIC_COPY 1
#define MAGIC_PASTE 2

#define PASTE_ACCURACY 1
#define PASTE_EFFECTIVITY 2
#define PASTE_ELEMENTS 4
#define PASTE_TARGETING 8
#define PASTE_EFFECT 16
#define PASTE_WEAPONGFX 32
#define PASTE_WEAPONPAL 64
#define PASTE_BYTE7 128
#define PASTE_BTLMSG 256
#define PASTE_PRICE 512
#define PASTE_PERMISSIONS 1024

#define CONTEXT_CHECKED 1
#define CONTEXT_UNCHECKED 2


/////////////////////////////////////////////////////////////////////////////
// CMagic dialog
#define BaseClass CEditorWithBackground


CMagic::CMagic(CWnd* pParent /*= nullptr */)
	: CEditorWithBackground(CMagic::IDD, pParent)
{
}

void CMagic::DoDataExchange(CDataExchange* pDX)
{
	CEditorWithBackground::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTMINBOX, m_outminbox);
	DDX_Control(pDX, IDC_OUTMAXBOX, m_outmaxbox);
	DDX_Control(pDX, IDC_OUTMIN, m_outmin);
	DDX_Control(pDX, IDC_OUTMAX, m_outmax);
	DDX_Control(pDX, IDC_OUTBATTLELIST, m_outbattlelist);
	DDX_Control(pDX, IDC_EDITGFX, m_editgfx);
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
	DDX_Control(pDX, IDC_BATTLEMESSAGE, m_battlemessage);
	DDX_Control(pDX, IDC_PRICE, m_price);
	DDX_Control(pDX, IDC_UNKNOWN, m_accuracy);
	DDX_Control(pDX, IDC_TARGET5, m_target5);
	DDX_Control(pDX, IDC_TARGET4, m_target4);
	DDX_Control(pDX, IDC_TARGET3, m_target3);
	DDX_Control(pDX, IDC_TARGET2, m_target2);
	DDX_Control(pDX, IDC_TARGET1, m_target1);
	DDX_Control(pDX, IDC_MAGICLIST, m_magiclist);
	DDX_Control(pDX, IDC_GFX, m_gfx);
	DDX_Control(pDX, IDC_ELEM8, m_elem8);
	DDX_Control(pDX, IDC_ELEM7, m_elem7);
	DDX_Control(pDX, IDC_ELEM6, m_elem6);
	DDX_Control(pDX, IDC_ELEM5, m_elem5);
	DDX_Control(pDX, IDC_ELEM4, m_elem4);
	DDX_Control(pDX, IDC_ELEM3, m_elem3);
	DDX_Control(pDX, IDC_ELEM2, m_elem2);
	DDX_Control(pDX, IDC_ELEM1, m_elem1);
	DDX_Control(pDX, IDC_EFFECTBOX, m_effectbox);
	DDX_Control(pDX, IDC_EFFECT8, m_effect8);
	DDX_Control(pDX, IDC_EFFECT7, m_effect7);
	DDX_Control(pDX, IDC_EFFECT6, m_effect6);
	DDX_Control(pDX, IDC_EFFECT5, m_effect5);
	DDX_Control(pDX, IDC_EFFECT4, m_effect4);
	DDX_Control(pDX, IDC_EFFECT3, m_effect3);
	DDX_Control(pDX, IDC_EFFECT2, m_effect2);
	DDX_Control(pDX, IDC_EFFECT1, m_effect1);
	DDX_Control(pDX, IDC_EFF9, m_eff9);
	DDX_Control(pDX, IDC_EFF8, m_eff8);
	DDX_Control(pDX, IDC_EFF7, m_eff7);
	DDX_Control(pDX, IDC_EFF6, m_eff6);
	DDX_Control(pDX, IDC_EFF5, m_eff5);
	DDX_Control(pDX, IDC_EFF4, m_eff4);
	DDX_Control(pDX, IDC_EFF3, m_eff3);
	DDX_Control(pDX, IDC_EFF2, m_eff2);
	DDX_Control(pDX, IDC_EFF17, m_eff17);
	DDX_Control(pDX, IDC_EFF16, m_eff16);
	DDX_Control(pDX, IDC_EFF15, m_eff15);
	DDX_Control(pDX, IDC_EFF14, m_eff14);
	DDX_Control(pDX, IDC_EFF13, m_eff13);
	DDX_Control(pDX, IDC_EFF12, m_eff12);
	DDX_Control(pDX, IDC_EFF11, m_eff11);
	DDX_Control(pDX, IDC_EFF10, m_eff10);
	DDX_Control(pDX, IDC_EFF1, m_eff1);
	DDX_Control(pDX, IDC_EDITLABEL, m_editlabel);
	DDX_Control(pDX, IDC_DAMAGEBOX, m_damagebox);
	DDX_Control(pDX, IDC_DAMAGE, m_damage);
	DDX_Control(pDX, IDC_MAGIC_STATIC_BYTE7, m_staticByte7);
	DDX_Control(pDX, IDC_MAGIC_EDIT_BYTE7, m_editByte7);
	DDX_Control(pDX, IDC_CLASSES_SETTINGS, m_settingsbutton);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDC_SAVE, m_savebutton);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
}


BEGIN_MESSAGE_MAP(CMagic, CEditorWithBackground)
	ON_LBN_SELCHANGE(IDC_MAGICLIST, OnSelchangeMagiclist)
	ON_WM_LBUTTONDOWN()
	ON_CBN_SELCHANGE(IDC_GFX, OnSelchangeGfx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_TARGET1, OnTarget1)
	ON_BN_CLICKED(IDC_TARGET2, OnTarget2)
	ON_BN_CLICKED(IDC_TARGET3, OnTarget3)
	ON_BN_CLICKED(IDC_TARGET4, OnTarget4)
	ON_BN_CLICKED(IDC_TARGET5, OnTarget5)
	ON_BN_CLICKED(IDC_EFF1, OnEff1)
	ON_BN_CLICKED(IDC_EFF10, OnEff10)
	ON_BN_CLICKED(IDC_EFF11, OnEff11)
	ON_BN_CLICKED(IDC_EFF12, OnEff12)
	ON_BN_CLICKED(IDC_EFF13, OnEff13)
	ON_BN_CLICKED(IDC_EFF14, OnEff14)
	ON_BN_CLICKED(IDC_EFF15, OnEff15)
	ON_BN_CLICKED(IDC_EFF16, OnEff16)
	ON_BN_CLICKED(IDC_EFF17, OnEff17)
	ON_BN_CLICKED(IDC_EFF2, OnEff2)
	ON_BN_CLICKED(IDC_EFF3, OnEff3)
	ON_BN_CLICKED(IDC_EFF4, OnEff4)
	ON_BN_CLICKED(IDC_EFF5, OnEff5)
	ON_BN_CLICKED(IDC_EFF6, OnEff6)
	ON_BN_CLICKED(IDC_EFF7, OnEff7)
	ON_BN_CLICKED(IDC_EFF8, OnEff8)
	ON_BN_CLICKED(IDC_EFF9, OnEff9)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_EDITLABEL, OnEditlabel)
	ON_BN_CLICKED(IDC_EDITGFX, OnEditgfx)
	ON_CBN_SELCHANGE(IDC_OUTBATTLELIST, OnSelchangeOutbattlelist)
	ON_EN_CHANGE(IDC_OUTMAX, OnChangeOutmax)
	ON_EN_CHANGE(IDC_OUTMIN, OnChangeOutmin)
	ON_CBN_EDITCHANGE(IDC_OUTMAXBOX, OnEditchangeOutmaxbox)
	ON_CBN_EDITCHANGE(IDC_OUTMINBOX, OnEditchangeOutminbox)
	ON_CBN_SELCHANGE(IDC_OUTMAXBOX, OnSelchangeOutmaxbox)
	ON_CBN_SELCHANGE(IDC_OUTMINBOX, OnSelchangeOutminbox)
	ON_BN_CLICKED(IDC_CLASSES_SETTINGS, &CMagic::OnBnClickedClassesSettings)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMagic message handlers

BOOL CMagic::OnInitDialog()
{
	CEditorWithBackground::OnInitDialog();

	try {
		LoadRom();

		haltwrite = 1;

		int co;
		for (co = 0; co < 9; co++) {
			m_outminbox.InsertString(co, mintext[co]);
			m_outmaxbox.InsertString(co, maxtext[co]);
		}

		m_graphics.Create(32, 32, ILC_COLOR16, 1, 0);
		LoadCaptions(std::vector<CWnd*>{ &m_elem1, &m_elem2, &m_elem3, &m_elem4, &m_elem5, &m_elem6, &m_elem7, &m_elem8 }, LoadElementLabels(*Project));
		LoadCustomizedControls();
		LoadListBox(m_magiclist, LoadMagicEntries(*Project) + LoadPotionEntries(*Project));
		LoadCombo(m_battlemessage, LoadBattleMessageEntries(*Project));

		// Load out-of-battle data and controls
		if (Project->IsRom()) {
			LoadCombo(m_outbattlelist, LoadOutOfBattleMagicEntries(*Project));
			m_oobmagicoffsets = { MG_CURE, MG_CUR2, MG_CUR3, MG_CUR4, MG_HEAL, MG_HEL3, MG_HEL2, MG_PURE, MG_LIFE, MG_LIF2, MG_WARP, MG_SOFT, MG_EXIT };
			ASSERT(m_oobmagicoffsets.size() == (size_t)OOBMAGIC_COUNT);

			m_oobmagicranges = {
				{ MGRANGEMIN_CURE, MGRANGEMAX_CURE },
				{ MGRANGEMIN_CUR2, MGRANGEMAX_CUR2 },
				{ MGRANGEMIN_CUR3, MGRANGEMAX_CUR3 },
				{ MGRANGEMIN_CUREP, MGRANGEMAX_CUREP },
				{ MGRANGEMIN_HEAL, MGRANGEMAX_HEAL },
				{ MGRANGEMIN_HEL2, MGRANGEMAX_HEL2 },
				{ MGRANGEMIN_HEL3, MGRANGEMAX_HEL3 },
			};
			ASSERT(m_oobmagicranges.size() == (size_t)OOBMAGICRANGE_COUNT);
		}

		m_outbattlelist.InsertString(0, "--none--");

		std::vector<CWnd*> classlists = {
			&m_use1,&m_use2,&m_use3,&m_use4,&m_use5,&m_use6,
			&m_use7,&m_use8,&m_use9,&m_use10,&m_use11,&m_use12 };
		ASSERT(classlists.size() == (size_t)CLASS_COUNT);
		classlists.resize(CLASS_COUNT);

		LoadCaptions(classlists, LoadClassEntries(*Project));
		LoadCombo(m_gfx, LoadWepMagicLabels(*Project));

		m_gfx.InsertString(0, "--None--");
		m_battlemessage.InsertString(0, "--None--");

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

		cur = -1;
		m_magiclist.SetCurSel(0);

		OnSelchangeMagiclist();

		m_banner.Set(this, COLOR_BLACK, COLOR_FFBLUE, "Magic");

		SetTimer(1, 400, nullptr);
		m_initialized = true;
	}
	catch (std::exception & ex) {
		return Ui::AbortInitDialog(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortInitDialog(this, "An unexpected exception prevented game data from loading.");
	}

	return 1;
}

void CMagic::LoadCode()
{
	m_outbattlelist.ShowWindow(SW_HIDE);
	m_outmin.ShowWindow(SW_HIDE);
	m_outmax.ShowWindow(SW_HIDE);
	m_outminbox.ShowWindow(SW_HIDE);
	m_outmaxbox.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_OBMAGIC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_OBMAG_BASE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_OBMAG_PLUS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_OBMAG_RANGE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_OBMAGICASM)->ShowWindow(SW_SHOW);
}

void CMagic::LoadRom()
{
	Project->ClearROM();

	CLASS_COUNT = ReadDec(Project->ValuesPath, "CLASS_COUNT");
	MAGIC_OFFSET = ReadHex(Project->ValuesPath, "MAGIC_OFFSET");
	MAGIC_BYTES = ReadDec(Project->ValuesPath, "MAGIC_BYTES");
	MAGIC_COUNT = ReadDec(Project->ValuesPath, "MAGIC_COUNT");
	MAGICPRICE_OFFSET = ReadHex(Project->ValuesPath, "MAGICPRICE_OFFSET");
	BATTLEMESSAGETEXT_START = ReadHex(Project->ValuesPath, "BATTLEMESSAGETEXT_START");
	NOTHINGHAPPENS_OFFSET = ReadHex(Project->ValuesPath, "NOTHINGHAPPENS_OFFSET");
	MAGICPERMISSIONS_OFFSET = ReadHex(Project->ValuesPath, "MAGICPERMISSIONS_OFFSET");
	BATTLEMESSAGE_OFFSET = ReadHex(Project->ValuesPath, "BATTLEMESSAGE_OFFSET");
	WEAPONMAGICGRAPHIC_OFFSET = ReadHex(Project->ValuesPath, "WEAPONMAGICGRAPHIC_OFFSET");

	OOBMAGIC_COUNT = ReadDec(Project->ValuesPath, "OOBMAGIC_COUNT");
	auto mgstartnode = find(Project->m_varmap, std::string("MG_START"));
	MG_START = mgstartnode.result ? mgstartnode.value : ReadHex(Project->ValuesPath, "MG_START");
	MG_CURE = ReadHex(Project->ValuesPath, "MG_CURE");
	MG_CUR2 = ReadHex(Project->ValuesPath, "MG_CUR2");
	MG_CUR3 = ReadHex(Project->ValuesPath, "MG_CUR3");
	MG_CUR4 = ReadHex(Project->ValuesPath, "MG_CUR4");
	MG_HEAL = ReadHex(Project->ValuesPath, "MG_HEAL");
	MG_HEL3 = ReadHex(Project->ValuesPath, "MG_HEL3");
	MG_HEL2 = ReadHex(Project->ValuesPath, "MG_HEL2");
	MG_PURE = ReadHex(Project->ValuesPath, "MG_PURE");
	MG_LIFE = ReadHex(Project->ValuesPath, "MG_LIFE");
	MG_LIF2 = ReadHex(Project->ValuesPath, "MG_LIF2");
	MG_WARP = ReadHex(Project->ValuesPath, "MG_WARP");
	MG_SOFT = ReadHex(Project->ValuesPath, "MG_SOFT");
	MG_EXIT = ReadHex(Project->ValuesPath, "MG_EXIT");

	OOBMAGICRANGE_COUNT = ReadDec(Project->ValuesPath, "OOBMAGICRANGE_COUNT");
	MGRANGEMIN_CURE = ReadHex(Project->ValuesPath, "MGRANGEMIN_CURE");
	MGRANGEMAX_CURE = ReadHex(Project->ValuesPath, "MGRANGEMAX_CURE");
	MGRANGEMIN_CUR2 = ReadHex(Project->ValuesPath, "MGRANGEMIN_CUR2");
	MGRANGEMAX_CUR2 = ReadHex(Project->ValuesPath, "MGRANGEMAX_CUR2");
	MGRANGEMIN_CUR3 = ReadHex(Project->ValuesPath, "MGRANGEMIN_CUR3");
	MGRANGEMAX_CUR3 = ReadHex(Project->ValuesPath, "MGRANGEMAX_CUR3");
	MGRANGEMIN_CUREP = ReadHex(Project->ValuesPath, "MGRANGEMIN_CUREP");
	MGRANGEMAX_CUREP = ReadHex(Project->ValuesPath, "MGRANGEMAX_CUREP"); //N.B. - not actually used by vanilla FF1, it's identical to MIN_CUREP here
	MGRANGEMIN_HEAL = ReadHex(Project->ValuesPath, "MGRANGEMIN_HEAL");
	MGRANGEMAX_HEAL = ReadHex(Project->ValuesPath, "MGRANGEMAX_HEAL");
	MGRANGEMIN_HEL2 = ReadHex(Project->ValuesPath, "MGRANGEMIN_HEL2");
	MGRANGEMAX_HEL2 = ReadHex(Project->ValuesPath, "MGRANGEMAX_HEL2");
	MGRANGEMIN_HEL3 = ReadHex(Project->ValuesPath, "MGRANGEMIN_HEL3");
	MGRANGEMAX_HEL3 = ReadHex(Project->ValuesPath, "MGRANGEMAX_HEL3");

	BANK0A_OFFSET = ReadHex(Project->ValuesPath, "BANK0A_OFFSET");
	BINBANK09GFXDATA_OFFSET = ReadHex(Project->ValuesPath, "BINBANK09GFXDATA_OFFSET");
	BINPRICEDATA_OFFSET = ReadHex(Project->ValuesPath, "BINPRICEDATA_OFFSET");

	// Now load the data
	if (Project->IsRom()) {
		load_binary(Project->WorkRomPath, Project->ROM);
	}
	else if (Project->IsAsm()) {
		GameSerializer ser(*Project);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.LoadAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.LoadAsmBin(BIN_BANK09GFXDATA, BINBANK09GFXDATA_OFFSET);
		ser.LoadAsmBin(BIN_MAGICDATA, MAGIC_OFFSET);
		ser.LoadAsmBin(BIN_PRICEDATA, BINPRICEDATA_OFFSET);
		ser.LoadAsmBin(BIN_BATTLEMESSAGES, BATTLEMESSAGETEXT_START);
		ser.LoadAsmBin(BIN_NOTHINGHAPPENS, NOTHINGHAPPENS_OFFSET);
		ser.LoadInline(ASM_0C, { { asmlabel, "lut_MagicBattleMessages", { BATTLEMESSAGE_OFFSET } } });
		ser.LoadInline(ASM_0E, { { asmtable, "lut_MagicPermissions", { MAGICPERMISSIONS_OFFSET } } });
		LoadCode();
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, (LPCSTR)Project->ProjectTypeName);
	}
}

void CMagic::SaveRom()
{
	if (Project->IsRom()) {
		save_binary(Project->WorkRomPath, Project->ROM);
	}
	else if (Project->IsAsm()) {
		GameSerializer ser(*Project);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.SaveAsmBin(BIN_BANK09GFXDATA, BINBANK09GFXDATA_OFFSET);
		ser.SaveAsmBin(BIN_MAGICDATA, MAGIC_OFFSET);
		ser.SaveAsmBin(BIN_PRICEDATA, BINPRICEDATA_OFFSET);
		ser.SaveAsmBin(BIN_BATTLEMESSAGES, BATTLEMESSAGETEXT_START);
		ser.SaveAsmBin(BIN_NOTHINGHAPPENS, NOTHINGHAPPENS_OFFSET);
		ser.SaveInline(ASM_0C, { { asmlabel, "lut_MagicBattleMessages", { BATTLEMESSAGE_OFFSET } } });
		ser.SaveInline(ASM_0E, { { asmtable, "lut_MagicPermissions", { MAGICPERMISSIONS_OFFSET } } });
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, (LPCSTR)Project->ProjectTypeName);
	}
}

void CMagic::LoadValues()
{
	CString text;
	int temp;
	int offset = MAGIC_OFFSET + (MAGIC_BYTES * cur);

	temp = Project->ROM[offset];
	if (temp < 0x10) text.Format("0%X", temp);
	else text.Format("%X", temp);
	m_accuracy.SetWindowText(text);

	temp = Project->ROM[offset + 2];
	SetCheckFlags(temp, std::vector<CStrikeCheck*>{ &m_elem1, &m_elem2, &m_elem3, &m_elem4, &m_elem5, &m_elem6, &m_elem7, &m_elem8 });

	UpdateTarget(Project->ROM[offset + 3]);

	temp = Project->ROM[offset + 5];
	if (temp > 0) {
		temp -= 0x80;
		temp >>= 2;
		temp += 1;
		m_gfx.SetCurSel(temp);
	}
	else m_gfx.SetCurSel(0);

	temp = Project->ROM[offset + 4];
	UpdateEffect(temp);
	if (UsesChecks(temp)) {
		temp = Project->ROM[offset + 1];
		SetCheckFlags(temp, std::vector<CStrikeCheck*>{ &m_effect1, &m_effect2, &m_effect3, &m_effect4, &m_effect5, &m_effect6, &m_effect7, &m_effect8 });
	}
	else {
		text.Format("%d", Project->ROM[offset + 1]);
		m_damage.SetWindowText(text);
	}

	cur_pal = Project->ROM[offset + 6] - 0x20;

	text.Format("%d", Project->ROM[offset + 7]);
	m_editByte7.SetWindowText(text);

	offset = MAGICPRICE_OFFSET + (cur << 1);
	temp = Project->ROM[offset] + (Project->ROM[offset + 1] << 8);
	text.Format("%d", temp);
	m_price.SetWindowText(text);

	// Each class' permission list is 8 bytes long, so read the same bit in each list by stepping forward by 8 bytes for each class.
	offset = (cur >> 3) + MAGICPERMISSIONS_OFFSET;
	temp = 1 << (7 - (cur & 7));
	std::vector<CStrikeCheck*> vchecks{ &m_use1, &m_use2, &m_use3, &m_use4, &m_use5, &m_use6, &m_use7, &m_use8,
		&m_use9, &m_use10, &m_use11, &m_use12};
	for (int step = 0, off = 0; step < 12; ++step, off += 0x08) {
		bool set = !(Project->ROM[offset + off] & temp);
		vchecks[step]->SetCheck(set ? BST_CHECKED : BST_UNCHECKED);
		vchecks[step]->Invalidate();
	}

	m_battlemessage.SetCurSel(Project->ROM[BATTLEMESSAGE_OFFSET + cur]);

	m_editgfx.EnableWindow(m_gfx.GetCurSel());

	offset = cur + MG_START;
	temp = -1;

	m_outbattlelist.SetCurSel(0);
	if (Project->IsRom()) {
		for (int co = 0; co < OOBMAGIC_COUNT; co++) {
			if (Project->ROM[m_oobmagicoffsets[co]] == offset) {
				m_outbattlelist.SetCurSel(co + 1);
				temp = co;
				break;
			}
		}
	}

	bool hide = cur < MAGIC_COUNT;
	m_use1.EnableWindow(hide);
	m_use2.EnableWindow(hide);
	m_use3.EnableWindow(hide);
	m_use4.EnableWindow(hide);
	m_use5.EnableWindow(hide);
	m_use6.EnableWindow(hide);
	m_use7.EnableWindow(hide);
	m_use8.EnableWindow(hide);
	m_use9.EnableWindow(hide);
	m_use10.EnableWindow(hide);
	m_use11.EnableWindow(hide);
	m_use12.EnableWindow(hide);
	m_battlemessage.EnableWindow(hide);
	m_price.EnableWindow(hide);
	m_outbattlelist.EnableWindow(hide);

	OnSelchangeOutbattlelist();

	ResetGraphicList();
	InvalidateRect(rcFinger);

	//TODO - how to reduce flickering here?
	InvalidateChild(&m_outminbox);
	InvalidateChild(&m_outmaxbox);
	InvalidateChild(&m_outbattlelist);
	InvalidateChild(&m_battlemessage);
	InvalidateChild(&m_gfx);
}

void CMagic::StoreValues()
{
	int temp;
	CString text;
	int offset = MAGIC_OFFSET + (MAGIC_BYTES * cur);

	m_accuracy.GetWindowText(text); temp = StringToInt_HEX(text);
	if (temp > 0xFF) temp = 0xFF;
	Project->ROM[offset] = (BYTE)temp;

	if (UsesChecks(cur_eff)) {
		temp = 0;
		temp = GetCheckFlags(std::vector<CStrikeCheck*>{ &m_effect1, &m_effect2, &m_effect3, &m_effect4, &m_effect5, &m_effect6, &m_effect7, &m_effect8 });
	}
	else {
		m_damage.GetWindowText(text); temp = StringToInt(text);
		if (temp > 0xFF) temp = 0xFF;
	}
	Project->ROM[offset + 1] = (BYTE)temp;

	temp = 0;
	temp = GetCheckFlags(std::vector<CStrikeCheck*>{ &m_elem1, &m_elem2, &m_elem3, &m_elem4, &m_elem5, &m_elem6, &m_elem7, &m_elem8 });
	Project->ROM[offset + 2] = (BYTE)temp;

	Project->ROM[offset + 3] = (BYTE)cur_targ;
	Project->ROM[offset + 4] = (BYTE)cur_eff;

	temp = m_gfx.GetCurSel();
	if (temp) {
		temp -= 1;
		temp <<= 2;
		temp += 0x80;
	}
	Project->ROM[offset + 5] = (BYTE)temp;
	Project->ROM[offset + 6] = (BYTE)(cur_pal + 0x20);

	m_editByte7.GetWindowText(text);
	temp = atol(text) & 0xff;
	Project->ROM[offset + 7] = (BYTE)temp;

	offset = MAGICPRICE_OFFSET + (cur << 1);
	m_price.GetWindowText(text); temp = StringToInt(text);
	if (temp > 0xFFFF) temp = 0xFFFF;
	Project->ROM[offset] = temp & 0xFF;
	Project->ROM[offset + 1] = (BYTE)(temp >> 8);

	offset = (cur >> 3) + MAGICPERMISSIONS_OFFSET;
	std::vector<CStrikeCheck*> vchecks{ &m_use1, &m_use2, &m_use3, &m_use4, &m_use5, &m_use6, &m_use7, &m_use8,
		&m_use9, &m_use10, &m_use11, &m_use12 };
	for (int co = 0, off = 0; co < (int)vchecks.size(); ++co, off += 0x08) {
		bool checked = GetCheckValue(*vchecks[co]);
		int bit = 1 << (7 - (cur & 7));  // slot 0 is the most significant bit, slot 7 is the least
		temp = Project->ROM[offset + off];
		if (!checked) bit = 0;           // when flipped, this preserves the "can't equip" bit
		temp &= ~bit;

		Project->ROM[offset + off] = 0xFF & temp;
	}
	int co;

	Project->ROM[BATTLEMESSAGE_OFFSET + cur] = (BYTE)m_battlemessage.GetCurSel();

	if (Project->IsRom()) {
		temp = cur + MG_START;
		for (co = 0; co < OOBMAGIC_COUNT; co++) {
			if (m_outbattlelist.GetCurSel() == (co + 1))
				Project->ROM[m_oobmagicoffsets[co]] = (BYTE)temp;
			else if (Project->ROM[m_oobmagicoffsets[co]] == temp)
				Project->ROM[m_oobmagicoffsets[co]] = 0xFF;
		}
	}
}

void CMagic::PaintClient(CDC & dc)
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
		br.CreateSolidBrush(Project->Palette[0][32 + co]);
		dc.FillRect(temp, &br);
		br.DeleteObject();
	}

	CPoint pt(rcFinger.left + (cur_pal << 4), rcFinger.top);
	if (cur_pal < 0) pt.x = rcFinger.left;
	Project->Finger.Draw(&dc, 1, pt, ILD_TRANSPARENT);

	pt.x = rcGraphic.left;
	pt.y = rcGraphic.top;
	m_graphics.Draw(&dc, cur_graphic, pt, ILD_TRANSPARENT);
}

void CMagic::OnSelchangeMagiclist()
{
	if (cur != -1)
		StoreValues();
	cur = m_magiclist.GetCurSel();
	LoadValues();
}

void CMagic::UpdateTarget(int temp)
{
	cur_targ = temp;
	m_target1.SetCheck(temp == 0x01);
	m_target2.SetCheck(temp == 0x02);
	m_target3.SetCheck(temp == 0x04);
	m_target4.SetCheck(temp == 0x08);
	m_target5.SetCheck(temp == 0x10);
}

namespace {
	dataintnodevector PrefixNodes(dataintnodevector dvec, CString prefix)
	{
		for (auto & node : dvec) {
			node.name = prefix + node.name;
		}
		return dvec;
	}
}

void CMagic::UpdateEffect(int temp)
{
	cur_eff = temp;
	m_eff1.SetCheck(temp == 0x00);
	m_eff2.SetCheck(temp == 0x01);
	m_eff3.SetCheck(temp == 0x02);
	m_eff4.SetCheck(temp == 0x03);
	m_eff5.SetCheck(temp == 0x04);
	m_eff6.SetCheck(temp == 0x05);
	m_eff7.SetCheck(temp == 0x07);
	m_eff8.SetCheck(temp == 0x08);
	m_eff9.SetCheck(temp == 0x09);
	m_eff10.SetCheck(temp == 0x0A);
	m_eff11.SetCheck(temp == 0x0C);
	m_eff12.SetCheck(temp == 0x0D);
	m_eff13.SetCheck(temp == 0x0E);
	m_eff14.SetCheck(temp == 0x0F);
	m_eff15.SetCheck(temp == 0x10);
	m_eff16.SetCheck(temp == 0x11);
	m_eff17.SetCheck(temp == 0x12);

	CString text[8];
	int co;

	for(co = 0; co < 8; co++) text[co] = "Unknown";
	std::vector<CWnd*> m_effect = 
	{
		&m_effect1,&m_effect2,&m_effect3,&m_effect4,
		&m_effect5,&m_effect6,&m_effect7,&m_effect8};

	temp = UsesChecks(temp);

	CString prefix;
	switch(temp){
	case 2:
		prefix = "Cures ";
		// flow into next cases
	case 1:
	case 4:
		LoadCaptions(m_effect, PrefixNodes(LoadAilEffectLabels(*Project), prefix));
		break;
	case 3:
		//LoadCaptions(m_effect, LoadAilEffectLabels(*Project));
		LoadCaptions(m_effect, LoadElementLabels(*Project));
		break;
	}

	//BUG - need to change these to SW_ values (ternary operator)
	m_damagebox.ShowWindow(!temp);
	m_damage.ShowWindow(!temp);
	m_effectbox.ShowWindow(temp != 0);

	for(co = 0; co < 8; co++){
		m_effect[co]->ShowWindow(temp != 0);}
}

void CMagic::LoadCustomizedControls()
{
	CMagicEditorSettings stgs(*Project);
	m_staticByte7.SetWindowText(stgs.Byte7Name);
}

int CMagic::UsesChecks(int temp)
{
	switch(temp){
	case 0x03: return 1; break;
	case 0x08: return 2; break;
	case 0x0A: return 3; break;
	case 0x12: return 4; break;
	default: return 0; break;}
}

void CMagic::ResetGraphicList()
{
	while(m_graphics.GetImageCount()) m_graphics.Remove(0);
	CBitmap bmp;
	CPaintDC dc(this);
	CDC mDC; mDC.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc,64,32);
	mDC.SelectObject(&bmp);

	if(m_gfx.GetCurSel()){
		int offset = WEAPONMAGICGRAPHIC_OFFSET + ((m_gfx.GetCurSel() - 1) << 6);
		BYTE palette[4];
		palette[0] = 0x0F;
		palette[1] = (BYTE)(0x20 + cur_pal);
		palette[2] = (BYTE)(0x10 + cur_pal);
		palette[3] = 0x30;

		BYTE co, coX, coY;
		for(co = 0; co < 64; co += 32){
		for(coY = 0; coY < 32; coY += 16){
		for(coX = 0; coX < 32; coX += 16, offset += 16){
			DrawTileScale(&mDC,co + coX,coY,Project,offset,palette,2);}}}
	}
	else{
		CRect rec(0,0,64,32);
		CBrush br; br.CreateSolidBrush(Project->Palette[0][0x0F]);
		mDC.FillRect(rec,&br);
		br.DeleteObject();}
	mDC.DeleteDC();
	m_graphics.Add(&bmp,RGB(0,0,0));
	bmp.DeleteObject();

	InvalidateRect(rcGraphic,0);
}

void CMagic::OnLButtonDown(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	if(PtInRect(rcPalette,pt)){
		cur_pal = (pt.x - rcPalette.left) >> 4;
		ResetGraphicList();
		InvalidateRect(rcFinger);
	}
	HandleLbuttonDrag(this);
}

void CMagic::OnSelchangeGfx() 
{ResetGraphicList(); InvalidateRect(rcPalette,0); InvalidateRect(rcFinger,0);
m_editgfx.EnableWindow(m_gfx.GetCurSel());}

void CMagic::OnTimer(UINT_PTR nIDEvent) 
{
	UNREFERENCED_PARAMETER(nIDEvent);

	cur_graphic = !cur_graphic;
	InvalidateRect(rcGraphic,0);
}

void CMagic::OnTarget1() 
{UpdateTarget(0x01);}
void CMagic::OnTarget2() 
{UpdateTarget(0x02);}
void CMagic::OnTarget3() 
{UpdateTarget(0x04);}
void CMagic::OnTarget4() 
{UpdateTarget(0x08);}
void CMagic::OnTarget5() 
{UpdateTarget(0x10);}
void CMagic::OnEff1()
{UpdateEffect(0x00);}
void CMagic::OnEff2()
{UpdateEffect(0x01);}
void CMagic::OnEff3()
{UpdateEffect(0x02);}
void CMagic::OnEff4()
{UpdateEffect(0x03);}
void CMagic::OnEff5()
{UpdateEffect(0x04);}
void CMagic::OnEff6()
{UpdateEffect(0x05);}
void CMagic::OnEff7()
{UpdateEffect(0x07);}
void CMagic::OnEff8()
{UpdateEffect(0x08);}
void CMagic::OnEff9()
{UpdateEffect(0x09);}
void CMagic::OnEff10()
{UpdateEffect(0x0A);}
void CMagic::OnEff11()
{UpdateEffect(0x0C);}
void CMagic::OnEff12()
{UpdateEffect(0x0D);}
void CMagic::OnEff13()
{UpdateEffect(0x0E);}
void CMagic::OnEff14()
{UpdateEffect(0x0F);}
void CMagic::OnEff15()
{UpdateEffect(0x10);}
void CMagic::OnEff16()
{UpdateEffect(0x11);}
void CMagic::OnEff17()
{UpdateEffect(0x12);}

void CMagic::OnEditlabel() 
{
	int temp = m_gfx.GetCurSel();
	ChangeLabel(*Project, -1, LoadWepMagicLabel(*Project, temp - 1), WriteWepMagicLabel, temp, nullptr, &m_gfx);
}

void CMagic::OnEditgfx()
{
	KillTimer(1);
	CWepMagGraphic dlg;
	dlg.Project = Project;
	dlg.paletteref = (BYTE)cur_pal;
	dlg.graphic = (short)(m_gfx.GetCurSel() - 1);
	dlg.IsWeapon = 0;
	dlg.DoModal();
	InvalidateRect(rcPalette,0);
	ResetGraphicList();
	SetTimer(1,400, nullptr);
}

void CMagic::OnSelchangeOutbattlelist() 
{
	if (!Project->IsRom()) return; // only handle out-of-battle in ROM projects

	haltwrite = 1;
	m_outmin.SetWindowText("");
	m_outmax.SetWindowText("");
	CString text = "";
	int temp = m_outbattlelist.GetCurSel() - 1;
	if((temp >= 0) && (temp <= 2)){
		m_outminbox.ShowWindow(1);
		m_outmaxbox.ShowWindow(1);
		m_outmin.ShowWindow(0);
		m_outmax.ShowWindow(0);
		text.Format("%d",Project->ROM[m_oobmagicranges[temp][0]]);
		m_outminbox.SetWindowText(text);
		text.Format("%d",Project->ROM[m_oobmagicranges[temp][1]]);
		m_outmaxbox.SetWindowText(text);
	}
	else if((temp >= 4) && (temp <= 6)){
		m_outminbox.ShowWindow(0);
		m_outmaxbox.ShowWindow(0);
		m_outmin.ShowWindow(1);
		m_outmax.ShowWindow(1);
		m_outmin.EnableWindow(1);
		m_outmax.EnableWindow(1);
		text.Format("%d",Project->ROM[m_oobmagicranges[temp][0]]);
		m_outmin.SetWindowText(text);
		text.Format("%d",Project->ROM[m_oobmagicranges[temp][1]]);
		m_outmax.SetWindowText(text);
	}
	else if(cur == MAGIC_COUNT){
		m_outminbox.ShowWindow(0);
		m_outmaxbox.ShowWindow(0);
		m_outmin.ShowWindow(1);
		m_outmax.ShowWindow(1);
		m_outmin.EnableWindow(1);
		m_outmax.EnableWindow(0);
		m_outmax.SetWindowText(text);
		text.Format("%d",Project->ROM[m_oobmagicranges[3][0]]);
		m_outmin.SetWindowText(text);
	}
	else{
		m_outmin.ShowWindow(1);
		m_outmax.ShowWindow(1);
		m_outminbox.ShowWindow(0);
		m_outmaxbox.ShowWindow(0);
		m_outmin.EnableWindow(0);
		m_outmax.EnableWindow(0);}
	haltwrite = 0;
}

void CMagic::OnChangeOutmax() 
{ChangeOutBattles(1);}
void CMagic::OnChangeOutmin() 
{ChangeOutBattles(0);}

void CMagic::ChangeOutBattles(bool minmax)
{
	if (!Project->IsRom()) return; // only handle out-of-battle in ROM projects

	if(haltwrite) return;
	int temp = m_outbattlelist.GetCurSel() - 1;

	if(cur == MAGIC_COUNT) temp = 3;
	else if(temp < 0 || temp == 3 || temp > 6) return;

	CString text;
	if(temp < 3){
		if(minmax) m_outmaxbox.GetWindowText(text);
		else m_outminbox.GetWindowText(text);}
	else{
		if(minmax) m_outmax.GetWindowText(text);
		else m_outmin.GetWindowText(text);}

	int value = StringToInt(text); if(value > 255) value = 255;
	Project->ROM[m_oobmagicranges[temp][minmax]] = (BYTE)value;
}

void CMagic::CopySpell(int classindex)
{
	m_copiedspell = classindex;
}

void CMagic::HandleMagicListContextMenu(CWnd * pWnd, CPoint point)
{
	CPoint listpoint = point;
	BOOL bOutside = FALSE;
	m_magiclist.ScreenToClient(&listpoint);
	auto selspell = m_magiclist.ItemFromPoint(listpoint, bOutside);
	if (selspell == LB_ERR) return;

	auto BuildPasteText = [](CString op, CString srcspellname, CString destspellname) {
		if (!op.IsEmpty()) op += " ";
		CString cs;
		cs.Format("Paste %sfrom %s to %s", (LPCSTR)op, (LPCSTR)srcspellname, (LPCSTR)destspellname);
		return cs;
	};

	CString strcopiedspell;
	CString strselected;
	m_magiclist.GetText(selspell, strselected);
	m_magicmenu.AppendMenu(MF_STRING, MAGIC_COPY, "&Copy " + strselected);
	if (m_copiedspell != -1 && m_copiedspell != (int)selspell) {
		// if pasting, the clicked index is actually the destination
		m_magiclist.GetText(m_copiedspell, strcopiedspell);
		m_magicmenu.AppendMenu(MF_STRING, MAGIC_PASTE, "&" + BuildPasteText("", strcopiedspell, strselected) + "...");
	}

	auto selop = m_magicmenu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, point.x, point.y, pWnd);
	if (selop != 0) {
		if (selop == MAGIC_COPY) {
			CopySpell(selspell);
		}
		if (selop == MAGIC_PASTE) {
			ASSERT(m_copiedspell != -1);

			CMagicEditorSettings stgs(*Project);
			CString strdestclass;
			m_magiclist.GetText(selspell, strdestclass);
			auto targets = std::vector<PasteTarget>{
				{ "Accuracy", true },
				{ "Effectivity", true },
				{ "Elements", true },
				{ "Targeting", true },
				{ "Effect", true },
				{ "Weapon graphic", true },
				{ "Weapon palette", true },
				{ stgs.Byte7Name, true },
				{ "Battle Message", true },
				{ "Price", true },
				{ "Permissions", true },
			};
			CDlgPasteTargets dlg(this);
			dlg.Title = BuildPasteText("", strcopiedspell, strselected);
			dlg.SetTargets(targets);
			if (dlg.DoModal() == IDOK) {
				unsigned long flags = 0;
				for (auto st = 0u; st < targets.size(); ++st) {
					auto val = (dlg.IsChecked(st) ? 1 : 0) << st;
					flags |= val;
				}
				PasteSpell(m_copiedspell, selspell, flags);
			}
		}
	}
}

void CMagic::PasteSpell(int srcindex, int destindex, int flags)
{
	int srcoffset = MAGIC_OFFSET + (MAGIC_BYTES * srcindex);
	int dstoffset = MAGIC_OFFSET + (MAGIC_BYTES * destindex);
	if (flags & PASTE_ACCURACY)
		Project->ROM[dstoffset] = Project->ROM[srcoffset];
	if (flags & PASTE_EFFECTIVITY)
		Project->ROM[dstoffset+1] = Project->ROM[srcoffset+1];
	if (flags & PASTE_ELEMENTS)
		Project->ROM[dstoffset+2] = Project->ROM[srcoffset+2];
	if (flags & PASTE_TARGETING)
		Project->ROM[dstoffset+3] = Project->ROM[srcoffset+3];
	if (flags & PASTE_EFFECT)
		Project->ROM[dstoffset+4] = Project->ROM[srcoffset+4];
	if (flags & PASTE_WEAPONGFX)
		Project->ROM[dstoffset+5] = Project->ROM[srcoffset+5];
	if (flags & PASTE_WEAPONPAL)
		Project->ROM[dstoffset+6] = Project->ROM[srcoffset+6];
	if (flags & PASTE_BYTE7)
		Project->ROM[dstoffset+7] = Project->ROM[srcoffset+7];

	if (flags & PASTE_PRICE) {
		int srcprice = MAGICPRICE_OFFSET + (srcindex << 1);
		int dstprice = MAGICPRICE_OFFSET + (destindex << 1);
		Project->ROM[dstprice] = Project->ROM[srcprice];
		Project->ROM[dstprice + 1] = Project->ROM[srcprice + 1];
	}

	if (flags & PASTE_BTLMSG) {
		Project->ROM[BATTLEMESSAGE_OFFSET + destindex] = Project->ROM[BATTLEMESSAGE_OFFSET + srcindex];
	}

	if (flags & PASTE_PERMISSIONS) {
		// Get the source and destination spell permisssions indices
		int srcperms = (int)((srcindex >> 3) + MAGICPERMISSIONS_OFFSET);
		int dstperms = (int)((destindex >> 3) + MAGICPERMISSIONS_OFFSET);

		BYTE srcmask = 1 << (7 - (srcindex & 7));                         // used to isolate the bit we want to copy from the source byte
		BYTE dstmask = 1 << (7 - (destindex & 7));                        // used to isolate the bit we want to overwrite in the dest byte
		BYTE invmask = ~dstmask;                                          // used to isolate the bits we PRESERVE in the dest byte

		for (int i = 0; i <= 0x58; i += 0x08) {
			Project->ROM[dstperms + i] = (Project->ROM[dstperms + i] & invmask) | (((Project->ROM[srcperms + i] & srcmask) != 0) ? dstmask : 0);

			//N.B. - the one-liner above copies the state of the source bit to the destination bit.
			//       the only difference between this and the code in LoadRom/SaveRom is that this code isn't translating
			//       the bits to check boxes. It does the following steps in one line:

			//bool on = (Project->ROM[srcperms + i] & srcmask) != 0;           // is the source bit set? if so, this class can't cast the spell
			//BYTE destmask = Project->ROM[dstperms + i] & invmask;            // these are the actual bits we will preserve
			//BYTE destadd = on ? dstmask : 0;                              // if the source bit is set, turn on the dest bit
			//BYTE newdestbyte = destmask | destadd;                        // now combine the preserved bits with the destadd and save them to ROM
			//Project->ROM[dstperms + i] = newdestbyte;                        // save as the new dest byte in ROM
		}
	}

	if (destindex == cur) {
		LoadValues();
	}
}

void CMagic::OnEditchangeOutmaxbox() 
{ChangeOutBattles(1);}
void CMagic::OnEditchangeOutminbox() 
{ChangeOutBattles(0);}

void CMagic::OnSelchangeOutmaxbox() 
{
	if(m_outmaxbox.GetCurSel() != -1){
		m_outmaxbox.SetWindowText(maxtext[m_outmaxbox.GetCurSel()]);
		ChangeOutBattles(1);}
}

void CMagic::OnSelchangeOutminbox() 
{
	if(m_outminbox.GetCurSel() != -1){
		m_outminbox.SetWindowText(mintext[m_outminbox.GetCurSel()]);
		ChangeOutBattles(0);}
}

void CMagic::OnBnClickedClassesSettings()
{
	CMagicEditorSettingsDlg dlg(this);
	dlg.m_proj = Project;
	if (dlg.DoModal() == IDOK) {
		LoadCustomizedControls();
	}
}

void CMagic::OnContextMenu(CWnd* pWnd, CPoint point)
{
	m_magicmenu.DestroyMenu();
	if (!m_magicmenu.CreatePopupMenu()) return;

	if (pWnd == &m_magiclist) {
		RedrawScope redraw(this);
		HandleMagicListContextMenu(pWnd, point);
	}
}