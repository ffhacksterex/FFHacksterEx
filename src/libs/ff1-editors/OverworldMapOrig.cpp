// OverworldMapOrig.cpp : implementation file
//

#include "stdafx.h"
#include "OverworldMapOrig.h"
#include <AppSettings.h>
#include "FFHacksterProject.h"
#include "collection_helpers.h"
#include "general_functions.h"
#include "ini_functions.h"
#include "string_functions.h"
#include "ingame_text_functions.h"
#include "io_functions.h"
#include "editor_label_functions.h"
#include "draw_functions.h"
#include <path_functions.h>
#include "AsmFiles.h"
#include "GameSerializer.h"
#include "DRAW_STRUCT.h"
#include "ui_helpers.h"
#include <ui_prompts.h>
#include "imaging_helpers.h"
#include "NESPalette.h"
#include "Loading.h"
#include "Coords.h"
#include "CustomTool.h"
#include "Tint.h"
#include "Backdrop.h"
#include "TileEdit.h"
#include "MiniMap.h"
#include "NewLabel.h"

using namespace Editorlabels;
using namespace Imaging;
using namespace Ini;
using namespace Io;
using namespace Ui;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace {
	constexpr auto FFH_MAP_FILTER = "FFHackster Overworld Map (*.ffm;*.ffomap)|*.ffm;*.ffomap|All Files (*.*)|*.*||";
	constexpr auto FFH_MAP_EXT = "ffomap";

	CMiniMap minimap;
	CCoords coord_dlg;
}


/////////////////////////////////////////////////////////////////////////////
// COverworldMapOrig dialog

COverworldMapOrig::COverworldMapOrig(CWnd* pParent /*= nullptr */)
	: CEditorWithBackground(COverworldMapOrig::IDD, pParent)
{
}

void COverworldMapOrig::DoDataExchange(CDataExchange* pDX)
{
	CEditorWithBackground::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIEWCOORDS, m_viewcoords);
	DDX_Control(pDX, IDC_MINIMAP, m_minimap);
	DDX_Control(pDX, IDC_MISCY, m_miscy);
	DDX_Control(pDX, IDC_MISCX, m_miscx);
	DDX_Control(pDX, IDC_MISCCOORDS, m_misccoords);
	DDX_Control(pDX, IDC_CUSTOMIZETOOL, m_customizetool);
	DDX_Control(pDX, IDC_KAB, m_kab);
	DDX_Control(pDX, IDC_BACKDROP, m_backdroplist);
	DDX_Control(pDX, IDC_SHOWLASTCLICK, m_showlastclick);
	DDX_Control(pDX, IDC_RANDOMENCOUNTER, m_randomencounter);
	DDX_Control(pDX, IDC_PROBABILITY8, m_probability8);
	DDX_Control(pDX, IDC_PROBABILITY7, m_probability7);
	DDX_Control(pDX, IDC_PROBABILITY6, m_probability6);
	DDX_Control(pDX, IDC_PROBABILITY5, m_probability5);
	DDX_Control(pDX, IDC_PROBABILITY4, m_probability4);
	DDX_Control(pDX, IDC_PROBABILITY3, m_probability3);
	DDX_Control(pDX, IDC_PROBABILITY2, m_probability2);
	DDX_Control(pDX, IDC_PROBABILITY1, m_probability1);
	DDX_Control(pDX, IDC_LASTCLICK, m_lastclick);
	DDX_Control(pDX, IDC_HOVERING, m_hovering);
	DDX_Control(pDX, IDC_FORMATION8, m_form8);
	DDX_Control(pDX, IDC_FORMATION7, m_form7);
	DDX_Control(pDX, IDC_FORMATION6, m_form6);
	DDX_Control(pDX, IDC_FORMATION5, m_form5);
	DDX_Control(pDX, IDC_FORMATION4, m_form4);
	DDX_Control(pDX, IDC_FORMATION3, m_form3);
	DDX_Control(pDX, IDC_FORMATION2, m_form2);
	DDX_Control(pDX, IDC_FORMATION1, m_form1);
	DDX_Control(pDX, IDC_DRAWGRID, m_drawgrid);
	DDX_Control(pDX, IDC_DOMAIN, m_domain);
	DDX_Control(pDX, IDC_BATTLE8, m_battle8);
	DDX_Control(pDX, IDC_BATTLE7, m_battle7);
	DDX_Control(pDX, IDC_BATTLE6, m_battle6);
	DDX_Control(pDX, IDC_BATTLE5, m_battle5);
	DDX_Control(pDX, IDC_BATTLE4, m_battle4);
	DDX_Control(pDX, IDC_BATTLE3, m_battle3);
	DDX_Control(pDX, IDC_BATTLE2, m_battle2);
	DDX_Control(pDX, IDC_BATTLE1, m_battle1);
	DDX_Control(pDX, IDC_FIGHT_RIVER, m_fight_river);
	DDX_Control(pDX, IDC_FIGHT_OCEAN, m_fight_ocean);
	DDX_Control(pDX, IDC_FIGHT_NORMAL, m_fight_normal);
	DDX_Control(pDX, IDC_FIGHT_NONE, m_fight_none);
	DDX_Control(pDX, IDC_TELEPORTBOX, m_teleportbox);
	DDX_Control(pDX, IDC_TELEPORT, m_teleport);
	DDX_Control(pDX, IDC_RAISEAIRSHIP, m_raiseairship);
	DDX_Control(pDX, IDC_MOVE_WALK, m_move_walk);
	DDX_Control(pDX, IDC_MOVE_SHIP, m_move_ship);
	DDX_Control(pDX, IDC_MOVE_CANOE, m_move_canoe);
	DDX_Control(pDX, IDC_MOVE_AIRSHIP, m_move_airship);
	DDX_Control(pDX, IDC_FOREST, m_forest);
	DDX_Control(pDX, IDC_DOCKSHIP, m_dockship);
	DDX_Control(pDX, IDC_CHIME, m_chime);
	DDX_Control(pDX, IDC_CARAVAN, m_caravan);
	DDX_Control(pDX, IDC_VSCROLL, m_vscroll);
	DDX_Control(pDX, IDC_HSCROLL, m_hscroll);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDC_STATIC_MAPS, m_mapstatic);
	DDX_Control(pDX, IDC_STATIC_TILES, m_tilestatic);
	DDX_Control(pDX, IDC_STATIC_MAPPALETTES, m_palettestatic);
	DDX_Control(pDX, IDC_STATIC_TOOLS, m_toolstatic);
	DDX_Control(pDX, IDC_STATIC_BACKDROP, m_backdropstatic);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
	DDX_Control(pDX, IDC_OW_LAND_ENCOUNTERRATE, m_landencrateedit);
	DDX_Control(pDX, IDC_OW_SEA_ENCOUNTERRATE2, m_seaencrateedit);
}


BEGIN_MESSAGE_MAP(COverworldMapOrig, CEditorWithBackground)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_EDITLABEL, OnEditlabel)
	ON_CBN_SELCHANGE(IDC_BACKDROP, OnSelchangeBackdrop)
	ON_WM_MOUSEMOVE()
	ON_EN_CHANGE(IDC_PROBABILITY1, OnChangeProbability)
	ON_BN_CLICKED(IDC_FIGHT_NONE, OnFightNone)
	ON_BN_CLICKED(IDC_FIGHT_NORMAL, OnFightNormal)
	ON_BN_CLICKED(IDC_FIGHT_OCEAN, OnFightOcean)
	ON_BN_CLICKED(IDC_FIGHT_RIVER, OnFightRiver)
	ON_BN_CLICKED(IDC_SHOWLASTCLICK, OnShowlastclick)
	ON_BN_CLICKED(IDC_DRAWGRID, OnDrawgrid)
	ON_BN_CLICKED(IDC_TELEPORT, OnTeleport)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_FINDKAB, OnFindKAB)
	ON_BN_CLICKED(IDC_CUSTOMIZETOOL, OnCustomizetool)
	ON_BN_CLICKED(IDC_CARAVAN, OnCaravan)
	ON_BN_CLICKED(IDC_CHIME, OnChime)
	ON_BN_CLICKED(IDC_RAISEAIRSHIP, OnRaiseairship)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_WM_RBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_EDITGRAPHICS, OnEditgraphics)
	ON_WM_LBUTTONDBLCLK()
	ON_CBN_SELCHANGE(IDC_MISCCOORDS, OnSelchangeMisccoords)
	ON_EN_CHANGE(IDC_MISCX, OnChangeMiscx)
	ON_EN_CHANGE(IDC_MISCY, OnChangeMiscy)
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_MINIMAP, OnMinimap)
	ON_BN_CLICKED(IDC_VIEWCOORDS, OnViewcoords)
	ON_EN_CHANGE(IDC_PROBABILITY2, OnChangeProbability)
	ON_EN_CHANGE(IDC_PROBABILITY3, OnChangeProbability)
	ON_EN_CHANGE(IDC_PROBABILITY4, OnChangeProbability)
	ON_EN_CHANGE(IDC_PROBABILITY5, OnChangeProbability)
	ON_EN_CHANGE(IDC_PROBABILITY6, OnChangeProbability)
	ON_EN_CHANGE(IDC_PROBABILITY7, OnChangeProbability)
	ON_EN_CHANGE(IDC_PROBABILITY8, OnChangeProbability)
	ON_CBN_SELCHANGE(IDC_TELEPORTBOX, OnSelchangeTeleportbox)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_MAP, &COverworldMapOrig::OnBnClickedButtonImportMap)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_MAP, &COverworldMapOrig::OnBnClickedButtonExportMap)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COverworldMapOrig message handlers


void COverworldMapOrig::DoHScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	OnHScroll(nSBCode, nPos, pScrollBar);
}

void COverworldMapOrig::DoVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	OnVScroll(nSBCode, nPos, pScrollBar);
}

void COverworldMapOrig::DoMinimap()
{
	OnMinimap();
}

void COverworldMapOrig::DoViewcoords()
{
	OnViewcoords();
}

void COverworldMapOrig::LoadOffsets()
{
	TRANSPARENTCOLOR = ReadRgb(cart->ValuesPath, "TRANSPARENTCOLOR");
	OVERWORLDPALETTE_ASSIGNMENT = ReadHex(cart->ValuesPath, "OVERWORLDPALETTE_ASSIGNMENT");
	OVERWORLDPALETTE_OFFSET = ReadHex(cart->ValuesPath, "OVERWORLDPALETTE_OFFSET");
	OVERWORLD_TILEDATA = ReadHex(cart->ValuesPath, "OVERWORLD_TILEDATA");
	OVERWORLDMAP_OFFSET = ReadHex(cart->ValuesPath, "OVERWORLDMAP_OFFSET");
	OVERWORLDMAP_PTRADD = ReadHex(cart->ValuesPath, "OVERWORLDMAP_PTRADD");
	OVERWORLDPATTERNTABLE_ASSIGNMENT = ReadHex(cart->ValuesPath, "OVERWORLDPATTERNTABLE_ASSIGNMENT");
	OVERWORLDPATTERNTABLE_OFFSET = ReadHex(cart->ValuesPath, "OVERWORLDPATTERNTABLE_OFFSET");
	BATTLEBACKDROP_COUNT = ReadDec(cart->ValuesPath, "BATTLEBACKDROP_COUNT");
	BATTLEBACKDROPASSIGNMENT_OFFSET = ReadHex(cart->ValuesPath, "BATTLEBACKDROPASSIGNMENT_OFFSET");
	MAPMANPALETTE_OFFSET = ReadHex(cart->ValuesPath, "MAPMANPALETTE_OFFSET");
	MAPMANGRAPHIC_OFFSET = ReadHex(cart->ValuesPath, "MAPMANGRAPHIC_OFFSET");
	BATTLEPATTERNTABLE_OFFSET = ReadHex(cart->ValuesPath, "BATTLEPATTERNTABLE_OFFSET");
	BATTLEBACKDROPPALETTE_OFFSET = ReadHex(cart->ValuesPath, "BATTLEBACKDROPPALETTE_OFFSET");
	BATTLEDOMAIN_OFFSET = ReadHex(cart->ValuesPath, "BATTLEDOMAIN_OFFSET");
	BATTLEPROBABILITY_OFFSET = ReadHex(cart->ValuesPath, "BATTLEPROBABILITY_OFFSET");
	OVERWORLDMAP_END = ReadHex(cart->ValuesPath, "OVERWORLDMAP_END");
	OVERWORLDMAP_START = ReadHex(cart->ValuesPath, "OVERWORLDMAP_START");
	OWBATTLERATE_LAND_OFFSET = ReadHex(cart->ValuesPath, "OWBATTLERATE_LAND_OFFSET");
	OWBATTLERATE_SEA_OFFSET = ReadHex(cart->ValuesPath, "OWBATTLERATE_SEA_OFFSET");

	COORD_STARTING_OFFSET = ReadHex(cart->ValuesPath, "COORD_STARTING_OFFSET");
	COORD_SHIP_OFFSET = ReadHex(cart->ValuesPath, "COORD_SHIP_OFFSET");
	COORD_AIRSHIP_OFFSET = ReadHex(cart->ValuesPath, "COORD_AIRSHIP_OFFSET");
	COORD_BRIDGE_OFFSET = ReadHex(cart->ValuesPath, "COORD_BRIDGE_OFFSET");
	COORD_CANAL_OFFSET = ReadHex(cart->ValuesPath, "COORD_CANAL_OFFSET");

	BANK00_OFFSET = ReadHex(cart->ValuesPath, "BANK00_OFFSET");
	BANK02_OFFSET = ReadHex(cart->ValuesPath, "BANK02_OFFSET");
	BANK07_OFFSET = ReadHex(cart->ValuesPath, "BANK07_OFFSET");
	BANK08_OFFSET = ReadHex(cart->ValuesPath, "BANK08_OFFSET");
	BINBANK01DATA_OFFSET = ReadHex(cart->ValuesPath, "BINBANK01DATA_OFFSET");
}

void COverworldMapOrig::LoadRom()
{
	cart->ClearROM();
	if (cart->IsRom()) {
		load_binary(cart->WorkRomPath, cart->ROM);
	}
	else if (cart->IsAsm()) {
		GameSerializer ser(*cart);
		// Instead of writing to the entire buffer, just write to the parts we need
		// Note that tile data spans from bank 3 through the end of bank 6.
		ser.LoadAsmBin(BANK_00, BANK00_OFFSET);
		ser.LoadAsmBin(BANK_02, BANK02_OFFSET);
		ser.LoadAsmBin(BANK_07, BANK07_OFFSET);
		ser.LoadAsmBin(BANK_08, BANK08_OFFSET);
		ser.LoadAsmBin(BIN_BANK01DATA, BINBANK01DATA_OFFSET);
		ser.LoadAsmBin(BIN_BATTLEDOMAINDATA, BATTLEDOMAIN_OFFSET);
		ser.LoadInline(ASM_0F, {
			{ asmopval, "op_OwBattleRateLand", { OWBATTLERATE_LAND_OFFSET} },
			{ asmopval, "op_OwBattleRateSea", { OWBATTLERATE_SEA_OFFSET} },
			{ asmlabel, "lut_FormationWeight", { BATTLEPROBABILITY_OFFSET } }
		});
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, (LPCSTR)cart->ProjectTypeName);
	}
}

void COverworldMapOrig::SaveRom()
{
	if (cart->IsRom()) {
		save_binary(cart->WorkRomPath, cart->ROM);
	}
	else if (cart->IsAsm()) {
		GameSerializer ser(*cart);
		// Instead of writing to the entire buffer, just write to the parts we need
		// Note that tile data spans from bank 3 through the end of bank 6.
		ser.SaveAsmBin(BANK_00, BANK00_OFFSET);
		ser.SaveAsmBin(BANK_02, BANK02_OFFSET);
		ser.SaveAsmBin(BANK_07, BANK07_OFFSET);
		ser.SaveAsmBin(BANK_08, BANK08_OFFSET);
		ser.SaveAsmBin(BIN_BANK01DATA, BINBANK01DATA_OFFSET);
		ser.SaveAsmBin(BIN_BATTLEDOMAINDATA, BATTLEDOMAIN_OFFSET);
		ser.SaveInline(ASM_0F, {
			{ asmopval, "op_OwBattleRateLand", { OWBATTLERATE_LAND_OFFSET} },
			{ asmopval, "op_OwBattleRateSea", { OWBATTLERATE_SEA_OFFSET} },
			{ asmlabel, "lut_FormationWeight", { BATTLEPROBABILITY_OFFSET } }
		});
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, (LPCSTR)cart->ProjectTypeName);
	}
}

BOOL COverworldMapOrig::OnInitDialog()
{
	cart = Project;
	CEditorWithBackground::OnInitDialog();

	try {
		this->LoadOffsets();
		this->LoadRom();

		m_misccoordoffsets.push_back(COORD_STARTING_OFFSET);
		m_misccoordoffsets.push_back(COORD_SHIP_OFFSET);
		m_misccoordoffsets.push_back(COORD_AIRSHIP_OFFSET);
		m_misccoordoffsets.push_back(COORD_BRIDGE_OFFSET);
		m_misccoordoffsets.push_back(COORD_CANAL_OFFSET);

		int co;

		coord_dlg.cart = cart;
		coord_dlg.Context = Coords;
		coord_dlg.Create(IDD_COORDS, this);
		coord_dlg.IsOV = 1;
		coord_dlg.OVparent = this;
		coord_dlg.Boot();
		m_viewcoords.SetCheck(BootToTeleportFollowup);
		OnViewcoords();

		minimap.cart = cart;minimap.Context = Minimap;
		minimap.OVmap = this;
		minimap.Create(IDD_MINIMAP, this);
		minimap.ShowWindow(0);
		minimap.Map = DecompressedMap[0];
		minimap.palette = palette[0];
		minimap.SetFocusRect(0, 0, 16, 16);
		//minimap.rcNew.SetRect(0, 0, 16, 16);
		//minimap.rcOld.SetRect(0, 0, 16, 16);

		for (co = 0; co < 128; co++)
			minimap.PalAssign[co] = cart->ROM[OVERWORLDPALETTE_ASSIGNMENT + co] & 3;

		LoadCombo(m_backdroplist, LoadBackdropLabels(*cart));
		const auto battleLabels = LoadBattleLabels(*cart);
		for (auto wnd : { &m_battle1, &m_battle2, &m_battle3,&m_battle4,&m_battle5,&m_battle6,&m_battle7,&m_battle8 })
			LoadCombo(*wnd, battleLabels);

		LoadCombo(m_teleportbox, LoadONTeleportLabels(*cart));
		m_teleportbox.SetCurSel(0);

		for (co = 0; co < 32; co++) {
			if (!(co & 3))
				palette[0][co] = cart->ROM[OVERWORLDPALETTE_OFFSET + 0x10];
			else
				palette[0][co] = cart->ROM[OVERWORLDPALETTE_OFFSET + co];
		}

		for (co = 0; co < 5; co++) {
			LoadCombo(m_misccoords, LoadMiscCoordLabels(*cart));
			misccoords[co].x = cart->ROM[m_misccoordoffsets[co]];
			misccoords[co].y = cart->ROM[m_misccoordoffsets[co] + 1];
		}
		misccoords[0].x += 7;
		misccoords[0].y += 7;
		m_misccoords.SetCurSel(0);
		OnSelchangeMisccoords();

		IF_NOHANDLE(redpen).CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		IF_NOHANDLE(bluepen).CreatePen(PS_SOLID, 1, RGB(0, 0, 255));

		CRect rcpos;
		GetControlRect(&m_backdropstatic, &rcpos);
		rcBackdrop.SetRect(0, 0, 128, 32);
		rcBackdrop.OffsetRect(rcpos.left + 8, rcpos.top + 18);

		GetControlRect(&m_tilestatic, &rcpos);
		rcTiles.SetRect(0, 0, 256, 128);
		rcTiles.OffsetRect(rcpos.left + 8, rcpos.top + 16);

		GetControlRect(&m_mapstatic, &rcpos);
		rcMap.SetRect(0, 0, 256, 256);
		rcMap.OffsetRect(rcpos.left + 8, rcpos.top + 16);

		GetControlRect(&m_toolstatic, &rcpos);
		rcTools.SetRect(0, 0, 125, 25);
		rcTools.OffsetRect(rcpos.left, rcpos.top);

		GetControlRect(&m_palettestatic, &rcpos);
		rcPalette.SetRect(0, 0, 256, 32);
		rcPalette.OffsetRect(rcpos.left + 4, rcpos.top + 4);

		m_hscroll.SetScrollRange(0, 240);
		m_vscroll.SetScrollRange(0, 240);
		ScrollOffset.x = 0;
		ScrollOffset.y = 0;

		probabilitychanged = 1;
		ptHover.x = -1;
		ptLastClick.x = -1;
		ptLastClick.y = 0;
		ptDomain.x = 0;
		ptDomain.y = 0;

		LoadDomain();
		CPoint pt(0, 0);
		UpdateClick(pt);

		cur_tool = 0;
		cur_tile = 0;
		mousedown = 0;
		LoadTileData();

		InitBackdrops();

		IF_NOHANDLE(m_sprites).Create(16, 16, ILC_COLOR16 + ILC_MASK, 5, 0);

		ReloadGraphics();

		IF_NOHANDLE(m_tools).Create(25, 25, ILC_COLOR4, 10, 0);

		CBitmap bmp;
		bmp.LoadBitmap(IDB_ADVDRAWINGTOOLS);
		m_tools.Add(&bmp, RGB(0, 0, 0)); bmp.DeleteObject();

		DecompressMap();

		m_showlastclick.SetCheck(cart->ShowLastClick);
		m_drawgrid.SetCheck(cart->DrawDomainGrid);

		Ui::SetControlInt(m_landencrateedit, Project->ROM[OWBATTLERATE_LAND_OFFSET]);
		Ui::SetControlInt(m_seaencrateedit, Project->ROM[OWBATTLERATE_SEA_OFFSET]);

		if (BootToTeleportFollowup) {
			ptLastClick.x = cart->TeleportFollowup[cart->curFollowup][1];
			ptLastClick.y = cart->TeleportFollowup[cart->curFollowup][2];
			OnHScroll(5, ptLastClick.x - 8, &m_hscroll);
			OnVScroll(5, ptLastClick.y - 8, &m_vscroll);
			cur_tile = DecompressedMap[ptLastClick.y][ptLastClick.x];
			LoadTileData();
		}
		BootToTeleportFollowup = 0;

		m_banner.Set(this, COLOR_BLACK, COLOR_FFBLUE, "Overworld Map");
	}
	catch (std::exception & ex) {
		return Ui::AbortInitDialog(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortInitDialog(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}

void COverworldMapOrig::LoadTileData()
{
	int temp = OVERWORLD_TILEDATA + (cur_tile << 1);
	BYTE data[2];
	data[0] = cart->ROM[temp];
	data[1] = cart->ROM[temp + 1];

	temp = data[0] & 0xC0;
	m_caravan.SetCheck(temp == 0x80);
	m_chime.SetCheck(temp == 0x40);
	m_raiseairship.SetCheck(temp == 0xC0);

	m_dockship.SetCheck(data[0] & 0x20);
	m_forest.SetCheck(data[0] & 0x10);
	m_move_airship.SetCheck(!(data[0] & 0x08));
	m_move_ship.SetCheck(!(data[0] & 0x04));
	m_move_canoe.SetCheck(!(data[0] & 0x02));
	m_move_walk.SetCheck(!(data[0] & 0x01));

	bool teleport = (data[1] & 0x80) != 0;
	m_teleport.SetCheck(teleport);
	OnTeleport();
	if(teleport){
		m_fight_none.SetCheck(1);
		m_fight_normal.SetCheck(0);
		m_fight_river.SetCheck(0);
		m_fight_ocean.SetCheck(0);
		m_teleportbox.SetCurSel(data[1] - 0x80);
	}
	else{
		temp = data[1] & 0x43;
		m_fight_none.SetCheck(temp < 0x40);
		m_fight_normal.SetCheck(temp == 0x40);
		m_fight_ocean.SetCheck(temp == 0x42);
		m_fight_river.SetCheck(temp == 0x41 || temp == 0x43);}

	m_backdroplist.SetCurSel(cart->ROM[BATTLEBACKDROPASSIGNMENT_OFFSET + cur_tile] & 0x0F);
	if(m_teleportbox.GetCurSel() == -1) m_teleportbox.SetCurSel(0);

	OnSelchangeTeleportbox();

	InvalidateRect(rcBackdrop,0);
}

void COverworldMapOrig::StoreTileData()
{
	int temp = OVERWORLD_TILEDATA + (cur_tile << 1);
	BYTE data[2] = {0,0};

	if(m_caravan.GetCheck()) data[0] |= 0x80;
	if(m_chime.GetCheck()) data[0] |= 0x40;
	if(m_raiseairship.GetCheck()) data[0] |= 0xC0;

	if(m_dockship.GetCheck()) data[0] |= 0x20;
	if(m_forest.GetCheck()) data[0] |= 0x10;
	if(!m_move_airship.GetCheck()) data[0] |= 0x08;
	if(!m_move_ship.GetCheck()) data[0] |= 0x04;
	if(!m_move_canoe.GetCheck()) data[0] |= 0x02;
	if(!m_move_walk.GetCheck()) data[0] |= 0x01;

	if(m_teleport.GetCheck()){
		data[1] = (BYTE)(0x80 + m_teleportbox.GetCurSel());}
	else{
		if(m_fight_normal.GetCheck()) data[1] = 0x40;
		if(m_fight_ocean.GetCheck()) data[1] = 0x42;
		if(m_fight_river.GetCheck()) data[1] = 0x41;
	}
	
	cart->ROM[temp] = data[0];
	cart->ROM[temp + 1] = data[1];

	cart->ROM[BATTLEBACKDROPASSIGNMENT_OFFSET + cur_tile] = (BYTE)m_backdroplist.GetCurSel();
}

void COverworldMapOrig::DecompressMap()
{
	// This takes the compressed map from the ROM and loads it into DecompressedMap[][]
	int offset, ptr;
	int coY, coX, co, run;
	BYTE temp;

	ptr = OVERWORLDMAP_OFFSET;
	for(coY = 0; coY < 256; coY++, ptr += 2){
		offset = cart->ROM[ptr] + (cart->ROM[ptr + 1] << 8) + OVERWORLDMAP_PTRADD;
		for(coX = 0; coX < 256; offset++){
			temp = cart->ROM[offset];
			run = cart->ROM[offset + 1];
			if(temp < 0x80){
				DecompressedMap[coY][coX] = temp;
				coX++;}
			else if(temp == 0xFF || run == 0xFF){
				for(; coX < 256; coX++)
					DecompressedMap[coY][coX] = 0;}
			else{
				offset += 1;
				temp -= 0x80;
				if(!run) run = 256;
				for(co = 0; co < run && coX < 256; co++, coX++)
					DecompressedMap[coY][coX] = temp;
			}
		}
	}

	OnFindKAB();
}

void COverworldMapOrig::ReloadGraphics()
{
	CLoading dlg; dlg.Create(IDD_LOADING,this);
	dlg.m_progress.SetRange(0,133);
	dlg.m_progress.SetPos(0);
	dlg.ShowWindow(1);

	if(cart->OK_overworldtiles)
	{
		dlg.m_progress.SetRange(0,5);
	}
	else{
		cart->m_overworldtiles.Create(16,16,ILC_COLOR16,128,0);
		CBitmap bmp;
		CBitmap dummy;
		CPaintDC dc(this);
		CDC mDC;
		mDC.CreateCompatibleDC(&dc);
		bmp.CreateCompatibleBitmap(&dc,16,16);
		dummy.CreateCompatibleBitmap(&dc,1,1);
		mDC.SelectObject(&bmp);

		BYTE usepalette;
		int offset;
		
		for(short imagecount = 0; imagecount < 128; imagecount += 1){
			usepalette = (cart->ROM[OVERWORLDPALETTE_ASSIGNMENT + imagecount] & 3) << 2;
			offset = OVERWORLDPATTERNTABLE_ASSIGNMENT + imagecount;

			DrawTile(&mDC,0,0,cart,OVERWORLDPATTERNTABLE_OFFSET + (cart->ROM[offset] << 4),&palette[0][usepalette],cart->TintTiles[0][imagecount]);
			DrawTile(&mDC,8,0,cart,OVERWORLDPATTERNTABLE_OFFSET + (cart->ROM[offset + 128] << 4),&palette[0][usepalette],cart->TintTiles[0][imagecount]);
			DrawTile(&mDC,0,8,cart,OVERWORLDPATTERNTABLE_OFFSET + (cart->ROM[offset + 256] << 4),&palette[0][usepalette],cart->TintTiles[0][imagecount]);
			DrawTile(&mDC,8,8,cart,OVERWORLDPATTERNTABLE_OFFSET + (cart->ROM[offset + 384] << 4),&palette[0][usepalette],cart->TintTiles[0][imagecount]);

			mDC.SelectObject(&dummy);
			cart->m_overworldtiles.Add(&bmp,TRANSPARENTCOLOR);
			mDC.SelectObject(&bmp);
			dlg.m_progress.OffsetPos(1);
		}

		mDC.DeleteDC();
		bmp.DeleteObject();
		dummy.DeleteObject();
		cart->OK_overworldtiles = 1;
	}

	ReloadSpriteGraphics(&dlg.m_progress);
	dlg.ShowWindow(0);
	dlg.DestroyWindow();
}

void COverworldMapOrig::ReloadSpriteGraphics(CProgressCtrl* m_prog)
{
	while(m_sprites.GetImageCount()) m_sprites.Remove(0);
	BYTE FixedPals[4][4] = { 0 };
	int co;
	for(co = 0; co < 16; co++)
		FixedPals[0][co] = palette[1][co];
	for(co = 0; co < 16; co += 4)
		FixedPals[0][co] = 0x40;
	FixedPals[0][2] = cart->ROM[MAPMANPALETTE_OFFSET];
	FixedPals[1][2] = cart->ROM[MAPMANPALETTE_OFFSET + 1];
	int offsets[5] = {MAPMANGRAPHIC_OFFSET,0x9E50,0x9F90,0x9C50,0x9C90};
	BYTE palassign[5] = {0,2,2,3,3};

	CBitmap bmp;
	CBitmap dummy;
	CPaintDC dc(this);
	CDC mDC;
	mDC.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc,16,16);
	dummy.CreateCompatibleBitmap(&dc,1,1);
	mDC.SelectObject(&bmp);

	for(co = 0; co < 5; co++){
		DrawTile(&mDC,0,0,cart,offsets[co],FixedPals[palassign[co]],0);
		DrawTile(&mDC,8,0,cart,offsets[co] + 16,FixedPals[palassign[co]],0);
		if(!co) palassign[0] = 1;
		DrawTile(&mDC,0,8,cart,offsets[co] + 32,FixedPals[palassign[co]],0);
		DrawTile(&mDC,8,8,cart,offsets[co] + 48,FixedPals[palassign[co]],0);
		mDC.SelectObject(&dummy);
		m_sprites.Add(&bmp,TRANSPARENTCOLOR);
		mDC.SelectObject(&bmp);
		m_prog->OffsetPos(1);
	}
	mDC.DeleteDC();
	bmp.DeleteObject();
	dummy.DeleteObject();
}

void COverworldMapOrig::InitBackdrops()
{
	IF_NOHANDLE(m_backdrop).Create(64,32,ILC_COLOR16,16,0);
	CBitmap bmp;
	CBitmap dummy;
	CPaintDC dc(this);
	CDC mDC;
	mDC.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc,64,32);
	dummy.CreateCompatibleBitmap(&dc,1,1);
	mDC.SelectObject(&bmp);

	int offset = BATTLEPATTERNTABLE_OFFSET + 0x10;
	int paloffset = BATTLEBACKDROPPALETTE_OFFSET;

	BYTE bigco, coY, coX;
	int slapX[4] = {48,48,16,16};
	CPoint pt;

	for(bigco = 0; bigco < BATTLEBACKDROP_COUNT; bigco++, paloffset += 4, offset += 0x700){
		for(coY = 0; coY < 32; coY += 8){
		for(coX = 0; coX < 32; coX += 8, offset += 16){
			DrawTile(&mDC,coX,coY,cart,offset,&cart->ROM[paloffset]);
			DrawTile(&mDC,coX + slapX[coX >> 3],coY,cart,offset,&cart->ROM[paloffset]);}}
		mDC.SelectObject(&dummy);
		m_backdrop.Add(&bmp,TRANSPARENTCOLOR);
		mDC.SelectObject(&bmp);
	}

	mDC.DeleteDC();
	bmp.DeleteObject();
	dummy.DeleteObject();
}

void COverworldMapOrig::OnPaint() 
{
	CPaintDC dc(this);
	CPen* origpen = dc.SelectObject(&redpen);
	int coX, coY, tile;
	CPoint pt;

	//Draw the Tiles on the screen
	for(coY = 0, tile = 0, pt.y = rcTiles.top; coY < 0x08; coY++, pt.y += 16){
	for(coX = 0, pt.x = rcTiles.left; coX < 0x10; coX++, pt.x += 16, tile++)
		cart->m_overworldtiles.Draw(&dc,tile,pt,ILD_NORMAL);}
	pt.x = ((cur_tile & 0x0F) << 4) + rcTiles.left;
	pt.y = (cur_tile & 0xF0) + rcTiles.top;
	dc.MoveTo(pt); pt.x += 15;
	dc.LineTo(pt); pt.y += 15;
	dc.LineTo(pt); pt.x -= 15;
	dc.LineTo(pt); pt.y -= 15;
	dc.LineTo(pt);

	//Draw the map!
	CPoint copt;
	CRect rcTemp = rcToolRect;
	rcTemp.NormalizeRect(); rcTemp.right += 1; rcTemp.bottom += 1;
	copt = ScrollOffset;
	for(coY = 0, pt.y = rcMap.top; coY < 16; coY++, pt.y += 16, copt.y += 1){
	for(coX = 0, pt.x = rcMap.left, copt.x = ScrollOffset.x; coX < 16; coX++, pt.x += 16, copt.x += 1){
		if(!(mousedown && cur_tool && PtInRect(rcTemp,copt))){
			cart->m_overworldtiles.Draw(&dc,DecompressedMap[copt.y][copt.x],pt,ILD_NORMAL);}}}
	if(mousedown){
		switch(cur_tool){
		case 0: break;
		case 1:{			//fill
			coY = rcTemp.top - ScrollOffset.y; coX = rcTemp.left - ScrollOffset.x;
			copt.y = rcTemp.bottom - ScrollOffset.y; copt.x = rcTemp.right - ScrollOffset.x;
			tile = coX;
			for(; coY < copt.y; coY++){
			for(coX = tile; coX < copt.x; coX++){
				pt.x = rcMap.left + (coX << 4); pt.y = rcMap.top + (coY << 4);
				cart->m_overworldtiles.Draw(&dc,cur_tile,pt,ILD_NORMAL);}}
			   }break;
		default:{
			CBrush br; br.CreateSolidBrush(RGB(128,64,255));
			rcTemp.left = ((rcTemp.left - ScrollOffset.x) << 4) + rcMap.left;
			rcTemp.right = ((rcTemp.right - ScrollOffset.x) << 4) + rcMap.left;
			rcTemp.top = ((rcTemp.top - ScrollOffset.y) << 4) + rcMap.top;
			rcTemp.bottom = ((rcTemp.bottom - ScrollOffset.y) << 4) + rcMap.top;
			dc.FillRect(rcTemp,&br);
			br.DeleteObject();
				}break;
		}
	}
	if(cart->ShowLastClick){
		pt.x = ((ptLastClick.x - ScrollOffset.x) << 4) + rcMap.left;
		pt.y = ((ptLastClick.y - ScrollOffset.y) << 4) + rcMap.top;
		if(PtInRect(rcMap,pt)){
			dc.MoveTo(pt); pt.x += 15;
			dc.LineTo(pt); pt.y += 15;
			dc.LineTo(pt); pt.x -= 15;
			dc.LineTo(pt); pt.y -= 15;
			dc.LineTo(pt);}
	}
	if(cart->DrawDomainGrid){
		dc.SelectObject(&bluepen);
		CPoint is;
		CPoint goo;
		goo.x = (ScrollOffset.x + 15) >> 4;
		goo.y = (ScrollOffset.y + 15) >> 4;
		if(!(goo.x & 1)){
			is.x = ScrollOffset.x & 0x0F;
			if(is.x) is.x = ((16 - is.x) << 4) + rcMap.left;
			else is.x = (is.x << 4) + rcMap.left;
			dc.MoveTo(is.x,rcMap.top); dc.LineTo(is.x,rcMap.bottom);}
		if(!(goo.y & 1)){
			is.y = ScrollOffset.y & 0x0F;
			if(is.y) is.y = ((16 - is.y) << 4) + rcMap.top;
			else is.y = (is.y << 4) + rcMap.top;
			dc.MoveTo(rcMap.left,is.y); dc.LineTo(rcMap.right,is.y);}
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(RGB(0,0,255));
		CString text;
		text.Format("%d,%d",goo.x >> 1,goo.y >> 1);
		if(!(goo.x % 2 && goo.y % 2))
			dc.TextOut(is.x,is.y,text);}
	
	//Draw the sprites
	CRect rc(0,0,16,16);
	for(coX = 0; coX < 5; coX++){
		pt.x = misccoords[coX].x - ScrollOffset.x;
		pt.y = misccoords[coX].y - ScrollOffset.y;
		if(!PtInRect(rc,pt)) continue;
		pt.x = (pt.x << 4) + rcMap.left;
		pt.y = (pt.y << 4) + rcMap.top;
		m_sprites.Draw(&dc,coX,pt,ILD_TRANSPARENT);
	}

	//Draw the backdrop
	pt.x = rcBackdrop.left;
	pt.y = rcBackdrop.top;
	tile = m_backdroplist.GetCurSel();
	m_backdrop.Draw(&dc,tile,pt,ILD_NORMAL); pt.x += 64;
	m_backdrop.Draw(&dc,tile,pt,ILD_NORMAL);

	//Draw the tools
	pt.x = rcTools.left; pt.y = rcTools.top;
	m_tools.Draw(&dc,0 + ((cur_tool == 0) * 5),pt,ILD_NORMAL); pt.x += 25;
	m_tools.Draw(&dc,1 + ((cur_tool == 1) * 5),pt,ILD_NORMAL); pt.x += 25;
	m_tools.Draw(&dc,2 + ((cur_tool == 2) * 5),pt,ILD_NORMAL); pt.x += 25;
	m_tools.Draw(&dc,3 + ((cur_tool == 3) * 5),pt,ILD_NORMAL); pt.x += 25;
	m_tools.Draw(&dc,4 + ((cur_tool == 4) * 5),pt,ILD_NORMAL);

	//Draw the palette!
	CBrush br;
	rc = rcPalette;
	rc.right = rc.left + 16; rc.bottom = rc.top + 16;
	for(pt.y = 0; pt.y < 2; pt.y += 1, rc.top += 16, rc.bottom += 16, rc.left -= 256, rc.right -= 256){
	for(pt.x = 0; pt.x < 16; pt.x++, rc.left += 16, rc.right += 16){
		br.CreateSolidBrush(cart->Palette[0][palette[pt.y][pt.x]]);
		dc.FillRect(rc,&br);
		br.DeleteObject();}}

	dc.SelectObject(origpen);

	m_banner.Render(dc, 8, 8);
}

void COverworldMapOrig::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UNREFERENCED_PARAMETER(pScrollBar);

	switch(nSBCode){
	case 0: ScrollOffset.x -= 1; break;
	case 1: ScrollOffset.x += 1; break;
	case 2: ScrollOffset.x -= 16; break;
	case 3: ScrollOffset.x += 16; break;
	case 5: ScrollOffset.x = nPos; break;
	}
	if(ScrollOffset.x < 0) ScrollOffset.x = 0;
	if(ScrollOffset.x > 240) ScrollOffset.x = 240;

	m_hscroll.SetScrollPos(ScrollOffset.x);
	//if(m_minimap.GetCheck()) minimap.FixRects(ScrollOffset);
	if (m_minimap.GetCheck()) minimap.UpdateFocusRect(ScrollOffset, m_minmapsize);
	InvalidateRect(rcMap,FALSE);
}

void COverworldMapOrig::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UNREFERENCED_PARAMETER(pScrollBar);

	switch(nSBCode){
	case 0: ScrollOffset.y -= 1; break;
	case 1: ScrollOffset.y += 1; break;
	case 2: ScrollOffset.y -= 16; break;
	case 3: ScrollOffset.y += 16; break;
	case 5: ScrollOffset.y = nPos; break;
	}
	if(ScrollOffset.y < 0) ScrollOffset.y = 0;
	if(ScrollOffset.y > 240) ScrollOffset.y = 240;

	m_vscroll.SetScrollPos(ScrollOffset.y);
	//if(m_minimap.GetCheck()) minimap.FixRects(ScrollOffset);
	if (m_minimap.GetCheck()) minimap.UpdateFocusRect(ScrollOffset, m_minmapsize);
	InvalidateRect(rcMap,FALSE);
}

void COverworldMapOrig::OnLButtonDown(UINT nFlags, CPoint pt)
{
	UNREFERENCED_PARAMETER(nFlags);

	if(PtInRect(rcMap,pt)){
		CPoint fixedpt;
		fixedpt.x = ((pt.x - rcMap.left) >> 4) + ScrollOffset.x;
		fixedpt.y = ((pt.y - rcMap.top) >> 4) + ScrollOffset.y;
		switch(cur_tool){
		case 0:{		//pencil
			mousedown = 1;
			UpdateClick(fixedpt);
			DecompressedMap[fixedpt.y][fixedpt.x] = (BYTE)cur_tile;
			InvalidateRect(rcMap,0);
			   }break;
		default:{		//fill/smarttools
			mousedown = 1;
			UpdateClick(fixedpt);
			rcToolRect.SetRect(fixedpt.x,fixedpt.y,fixedpt.x,fixedpt.y);
			InvalidateRect(rcMap,0);
			   }break;

		}
	}
	else if(PtInRect(rcTiles,pt)){
		StoreTileData();
		pt.x = (pt.x - rcTiles.left) >> 4;
		pt.y = (pt.y - rcTiles.top) & 0xF0;
		cur_tile = pt.y + pt.x;
		InvalidateRect(rcTiles,0);
		LoadTileData();}
	else if(PtInRect(rcTools,pt)){
		cur_tool = (short)((pt.x - rcTools.left) / 25);
		InvalidateRect(rcTools,0);
		m_customizetool.EnableWindow(cur_tool > 1);}
	else if(PtInRect(rcPalette,pt)){
		pt.y = (pt.y - rcPalette.top) >> 4;
		pt.x = (pt.x - rcPalette.left) >> 4;
		CNESPalette dlg;
		dlg.cart = cart;
		dlg.color = &palette[pt.y][pt.x];
		if(dlg.DoModal() == IDOK){
			if(!(pt.x & 3)){
				for(pt.y = 0; pt.y < 32; pt.y += 4)
					palette[0][pt.y] = *dlg.color;
			}
			else if(m_minimap.GetCheck() && ((pt.x & 3) == 3))
				minimap.UpdateAll();
			cart->m_overworldtiles.DeleteImageList();
			cart->OK_overworldtiles = 0;
			ReloadGraphics();
			InvalidateRect(rcTiles,0);
			InvalidateRect(rcMap,0);
			InvalidateRect(rcPalette,0);
		}
	}
	else {
		HandleLbuttonDrag(this);
	}
}

void COverworldMapOrig::OnRButtonDown(UINT nFlags, CPoint pt)
{
	mousedown = 0;
	if(PtInRect(rcTiles,pt)) OnLButtonDown(nFlags,pt);
	else if(PtInRect(rcMap,pt)){
		StoreTileData();
		pt.x = ((pt.x - rcMap.left) >> 4) + ScrollOffset.x;
		pt.y = ((pt.y - rcMap.top) >> 4) + ScrollOffset.y;
		cur_tile = DecompressedMap[pt.y][pt.x];
		UpdateClick(pt);
		InvalidateRect(rcTiles,0);
		LoadTileData();
		if(cur_tool > 1){
			cur_tool = 1;
			InvalidateRect(rcTools,0);}
		if(m_showlastclick.GetCheck()) InvalidateRect(rcMap,0);

		
		//if they clicked on a sprite... adjust the Sprite Editor accordingly
		for(int co = 0; co < 5; co++){
			if(misccoords[co] == pt){
				mousedown = (BYTE)(co + 2);
				m_misccoords.SetCurSel(co);
				OnSelchangeMisccoords();
				break;}}
	}
}

void COverworldMapOrig::OnEditlabel()
{
	int temp = m_backdroplist.GetCurSel();
	ChangeLabel(*cart, -1, LoadBackdropLabel(*cart, temp), WriteBackdropLabel, temp, nullptr, &m_backdroplist);
}

void COverworldMapOrig::OnSelchangeBackdrop() 
{InvalidateRect(rcBackdrop);}

void COverworldMapOrig::OnMouseMove(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);

	CString text = "";
	if(PtInRect(rcMap,pt)){
		CPoint newhover;
		newhover.x = ((pt.x - rcMap.left) >> 4) + ScrollOffset.x;
		newhover.y = ((pt.y - rcMap.top) >> 4) + ScrollOffset.y;
		if(ptHover != newhover){
			ptHover = newhover;
			text.Format("%X,%X",ptHover.x,ptHover.y);
			m_hovering.SetWindowText(text);
			if(mousedown == 1){
				switch(cur_tool){
				case 0:{		//pencil
					DecompressedMap[ptHover.y][ptHover.x] = (BYTE)cur_tile;
					InvalidateRect(rcMap,0);
					   }break;
				default:{		//fill / Smarttools
					rcToolRect.right = ptHover.x;
					rcToolRect.bottom = ptHover.y;
					InvalidateRect(rcMap,0);
					   }break;
				}
				UpdateClick(ptHover);
			}
			else if(mousedown){
				UpdateClick(ptHover);
				misccoords[mousedown - 2] = ptHover;
				text.Format("%X",ptHover.x); m_miscx.SetWindowText(text);
				text.Format("%X",ptHover.y); m_miscy.SetWindowText(text);
				InvalidateRect(rcMap,0);}
		}
	}
	else{
		if(ptHover.x != -1){
			m_hovering.SetWindowText(text);
			ptHover.x = -1;}
	}
}

void COverworldMapOrig::UpdateClick(CPoint pt)
{
	CString text;
	if(ptLastClick == pt) return;

	ptLastClick = pt;
	pt.x >>= 5;
	pt.y >>= 5;

	if(pt != ptDomain){
		StoreDomain();
		ptDomain = pt;
		LoadDomain();
		text.Format("%X,%X - ",ptDomain.x,ptDomain.y);
		if(ptDomain.y > 3) text += "S";
		else text += "N";
		m_domain.SetWindowText(text);
		text = "Domain:  " + text;
		m_randomencounter.SetWindowText(text);
	}
	text.Format("%X,%X",ptLastClick.x,ptLastClick.y);
	m_lastclick.SetWindowText(text);
}

void COverworldMapOrig::LoadDomain()
{
	int ref = (ptDomain.y << 3) + ptDomain.x;
	ref = BATTLEDOMAIN_OFFSET + (ref << 3);

	m_battle1.SetCurSel(cart->ROM[ref] & 0x7F);
	m_form1.SetCheck(cart->ROM[ref] & 0x80);
	m_battle2.SetCurSel(cart->ROM[ref + 1] & 0x7F);
	m_form2.SetCheck(cart->ROM[ref + 1] & 0x80);
	m_battle3.SetCurSel(cart->ROM[ref + 2] & 0x7F);
	m_form3.SetCheck(cart->ROM[ref + 2] & 0x80);
	m_battle4.SetCurSel(cart->ROM[ref + 3] & 0x7F);
	m_form4.SetCheck(cart->ROM[ref + 3] & 0x80);
	m_battle5.SetCurSel(cart->ROM[ref + 4] & 0x7F);
	m_form5.SetCheck(cart->ROM[ref + 4] & 0x80);
	m_battle6.SetCurSel(cart->ROM[ref + 5] & 0x7F);
	m_form6.SetCheck(cart->ROM[ref + 5] & 0x80);
	m_battle7.SetCurSel(cart->ROM[ref + 6] & 0x7F);
	m_form7.SetCheck(cart->ROM[ref + 6] & 0x80);
	m_battle8.SetCurSel(cart->ROM[ref + 7] & 0x7F);
	m_form8.SetCheck(cart->ROM[ref + 7] & 0x80);

	if(probabilitychanged){
		probabilitychanged = 0;
		ref = BATTLEPROBABILITY_OFFSET;
		int buffer[8] = {0,0,0,0,0,0,0,0};
		int co;
		for(co = 0; co < 64; co++)
			buffer[cart->ROM[ref + co]] += 1;
		CString text;
		CEdit* m_edit[8] = {&m_probability1,&m_probability2,&m_probability3,&m_probability4,
			&m_probability5,&m_probability6,&m_probability7,&m_probability8};
		for(co = 0; co < 8; co++){
			text.Format("%d",buffer[co]);
			m_edit[co]->SetWindowText(text);}
	}
}

void COverworldMapOrig::StoreDomain()
{
	int ref = (ptDomain.y << 3) + ptDomain.x;
	ref = BATTLEDOMAIN_OFFSET + (ref << 3);

	cart->ROM[ref] = (BYTE)(m_battle1.GetCurSel() + (m_form1.GetCheck() << 7));
	cart->ROM[ref + 1] = (BYTE)(m_battle2.GetCurSel() + (m_form2.GetCheck() << 7));
	cart->ROM[ref + 2] = (BYTE)(m_battle3.GetCurSel() + (m_form3.GetCheck() << 7));
	cart->ROM[ref + 3] = (BYTE)(m_battle4.GetCurSel() + (m_form4.GetCheck() << 7));
	cart->ROM[ref + 4] = (BYTE)(m_battle5.GetCurSel() + (m_form5.GetCheck() << 7));
	cart->ROM[ref + 5] = (BYTE)(m_battle6.GetCurSel() + (m_form6.GetCheck() << 7));
	cart->ROM[ref + 6] = (BYTE)(m_battle7.GetCurSel() + (m_form7.GetCheck() << 7));
	cart->ROM[ref + 7] = (BYTE)(m_battle8.GetCurSel() + (m_form8.GetCheck() << 7));

	if(probabilitychanged){
		int hold[8] = {0,0,0,0,0,0,0,0};
		CString text;
		ref = BATTLEPROBABILITY_OFFSET;

		m_probability1.GetWindowText(text); hold[0] = StringToInt(text);
		m_probability2.GetWindowText(text); hold[1] = StringToInt(text);
		m_probability3.GetWindowText(text); hold[2] = StringToInt(text);
		m_probability4.GetWindowText(text); hold[3] = StringToInt(text);
		m_probability5.GetWindowText(text); hold[4] = StringToInt(text);
		m_probability6.GetWindowText(text); hold[5] = StringToInt(text);
		m_probability7.GetWindowText(text); hold[6] = StringToInt(text);
		m_probability8.GetWindowText(text); hold[7] = StringToInt(text);

		int max = ref + 64;
		int co;
		int bigco;
		for(bigco = 0; bigco < 8; bigco++){
			for(co = 0; co < hold[bigco] && ref < max; co++, ref++)
				cart->ROM[ref] = (BYTE)bigco;}

		for(;ref < max; ref++)
			cart->ROM[ref] = 0;

		LoadDomain();
	}
}

void COverworldMapOrig::OnChangeProbability() 
{probabilitychanged = 1;}

void COverworldMapOrig::OnFightNone() 
{UpdateFight(0);}
void COverworldMapOrig::OnFightNormal() 
{UpdateFight(1);}
void COverworldMapOrig::OnFightOcean() 
{UpdateFight(2);}
void COverworldMapOrig::OnFightRiver() 
{UpdateFight(3);}
void COverworldMapOrig::UpdateFight(int update)
{
	m_fight_none.SetCheck(update == 0);
	m_fight_normal.SetCheck(update == 1);
	m_fight_ocean.SetCheck(update == 2);
	m_fight_river.SetCheck(update == 3);
}
void COverworldMapOrig::OnCaravan() 
{UpdateMisc(m_caravan.GetCheck());}
void COverworldMapOrig::OnChime() 
{UpdateMisc(m_chime.GetCheck() << 1);}
void COverworldMapOrig::OnRaiseairship() 
{UpdateMisc(m_raiseairship.GetCheck() << 2);}
void COverworldMapOrig::UpdateMisc(int update)
{
	m_caravan.SetCheck(update == 1);
	m_chime.SetCheck(update == 2);
	m_raiseairship.SetCheck(update == 4);
}
void COverworldMapOrig::OnShowlastclick() 
{cart->ShowLastClick = (m_showlastclick.GetCheck() != 0); InvalidateRect(rcMap,0);}
void COverworldMapOrig::OnDrawgrid() 
{cart->DrawDomainGrid = (m_drawgrid.GetCheck() != 0); InvalidateRect(rcMap,0);}
void COverworldMapOrig::OnTeleport() 
{
	bool teleport = m_teleport.GetCheck() != 0;
	m_fight_none.EnableWindow(!teleport);
	m_fight_normal.EnableWindow(!teleport);
	m_fight_river.EnableWindow(!teleport);
	m_fight_ocean.EnableWindow(!teleport);
	m_teleportbox.ShowWindow(teleport);
}

void COverworldMapOrig::OnLButtonUp(UINT nFlags, CPoint point) 
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);

	if(mousedown){
		rcToolRect.NormalizeRect();
		int coY, coX, temp, co;
		bool draw;
		switch(cur_tool){
		case 0: break;
		case 1:{			//fill
			for(coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++){
			for(coX = rcToolRect.left; coX <= rcToolRect.right; coX++)
				DecompressedMap[coY][coX] = (BYTE)cur_tile;}
			   }break;
		default:{			//smarttools
			temp = cur_tool - 2;
			//flood fill
			for(coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++){
			for(coX = rcToolRect.left; coX <= rcToolRect.right; coX++)
				DecompressedMap[coY][coX] = cart->SmartTools[temp][4];}
			//"smart" top edge
			coY = rcToolRect.top;
			for(coX = rcToolRect.left; coX <= rcToolRect.right; coX++){
				draw = 1;
				if(coY){ for(co = 0; co < 6 && draw; co++){
					if(DecompressedMap[coY - 1][coX] == cart->SmartTools[temp][co]) draw = 0;}}
				if(draw) DecompressedMap[coY][coX] = cart->SmartTools[temp][1];}
			//"smart" bottom edge
			coY = rcToolRect.bottom;
			for(coX = rcToolRect.left; coX <= rcToolRect.right; coX++){
				draw = 1;
				if(coY < 255){ for(co = 3; co < 9 && draw; co++){
					if(DecompressedMap[coY + 1][coX] == cart->SmartTools[temp][co]) draw = 0;}}
				if(draw) DecompressedMap[coY][coX] = cart->SmartTools[temp][7];}
			//"smart" left edge
			coX = rcToolRect.left;
			for(coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++){
				draw = 1;
				if(coX){ for(co = 0; co < 8 && draw; co++){
					if(co % 3 == 2) co++;
					if(DecompressedMap[coY][coX - 1] == cart->SmartTools[temp][co]) draw = 0;}}
				if(draw) DecompressedMap[coY][coX] = cart->SmartTools[temp][3];}
			//"smart" right edge
			coX = rcToolRect.right;
			for(coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++){
				draw = 1;
				if(coX < 255){ for(co = 1; co < 9 && draw; co++){
					if(co % 3 == 0) co++;
					if(DecompressedMap[coY][coX + 1] == cart->SmartTools[temp][co]) draw = 0;}}
				if(draw) DecompressedMap[coY][coX] = cart->SmartTools[temp][5];}
			//"smart" NW corner
			co = 0;
			draw = 1;
			if(rcToolRect.left){
				coY = DecompressedMap[rcToolRect.top][rcToolRect.left - 1];
				for(coX = 0; draw && coX < 8; coX++){
					if(coX % 3 == 2) coX++;
					if(coY == cart->SmartTools[temp][coX]) draw = 0;}}
			if(!draw) co = 1;
			draw = 1;
			if(rcToolRect.top){
				coY = DecompressedMap[rcToolRect.top - 1][rcToolRect.left];
				for(coX = 0; draw && coX < 6; coX++){
					if(coY == cart->SmartTools[temp][coX]) draw = 0;}}
			if(!draw){
				if(co == 1) co = 4;
				else co = 3;}
			DecompressedMap[rcToolRect.top][rcToolRect.left] = cart->SmartTools[temp][co];
			//"smart" SW corner
			co = 6;
			draw = 1;
			if(rcToolRect.left){
				coY = DecompressedMap[rcToolRect.bottom][rcToolRect.left - 1];
				for(coX = 0; draw && coX < 8; coX++){
					if(coX % 3 == 2) coX++;
					if(coY == cart->SmartTools[temp][coX]) draw = 0;}}
			if(!draw) co = 7;
			draw = 1;
			if(rcToolRect.bottom < 255){
				coY = DecompressedMap[rcToolRect.bottom + 1][rcToolRect.left];
				for(coX = 3; draw && coX < 9; coX++){
					if(coY == cart->SmartTools[temp][coX]) draw = 0;}}
			if(!draw){
				if(co == 7) co = 4;
				else co = 3;}
			DecompressedMap[rcToolRect.bottom][rcToolRect.left] = cart->SmartTools[temp][co];
			//"smart" NE corner
			co = 2;
			draw = 1;
			if(rcToolRect.right < 255){
				coY = DecompressedMap[rcToolRect.top][rcToolRect.right + 1];
				for(coX = 1; draw && coX < 9; coX++){
					if(coX % 3 == 0) coX++;
					if(coY == cart->SmartTools[temp][coX]) draw = 0;}}
			if(!draw) co = 1;
			draw = 1;
			if(rcToolRect.top){
				coY = DecompressedMap[rcToolRect.top - 1][rcToolRect.right];
				for(coX = 0; draw && coX < 6; coX++){
					if(coY == cart->SmartTools[temp][coX]) draw = 0;}}
			if(!draw){
				if(co == 1) co = 4;
				else co = 5;}
			DecompressedMap[rcToolRect.top][rcToolRect.right] = cart->SmartTools[temp][co];
			//"smart" SE corner
			co = 8;
			draw = 1;
			if(rcToolRect.right < 255){
				coY = DecompressedMap[rcToolRect.bottom][rcToolRect.right + 1];
				for(coX = 1; draw && coX < 9; coX++){
					if(coX % 3 == 0) coX++;
					if(coY == cart->SmartTools[temp][coX]) draw = 0;}}
			if(!draw) co = 7;
			draw = 1;
			if(rcToolRect.bottom < 255){
				coY = DecompressedMap[rcToolRect.bottom + 1][rcToolRect.right];
				for(coX = 3; draw && coX < 9; coX++){
					if(coY == cart->SmartTools[temp][coX]) draw = 0;}}
			if(!draw){
				if(co == 7) co = 4;
				else co = 5;}
			DecompressedMap[rcToolRect.bottom][rcToolRect.right] = cart->SmartTools[temp][co];

			InvalidateRect(rcMap);
				}break;
		}
		if(m_minimap.GetCheck()) minimap.UpdateCur();
	}
	mousedown = 0;
}

void COverworldMapOrig::OnFindKAB() 
{
	kab = 0;
	int coX, coY, coRecycle;
	int ThisRun, RunLength;
	bool recycle;
	for(coY = 0; coY < 256; coY++){
		recycle = 0;
		for(coRecycle = 0; coRecycle < coY && !recycle; coRecycle++){
			recycle = 1;
			for(coX = 0; coX < 256 && recycle; coX++)
				recycle = (DecompressedMap[coY][coX] == DecompressedMap[coRecycle][coX]);
		}
		if(!recycle){
			for(coX = 0; coX < 256;){
				ThisRun = DecompressedMap[coY][coX]; RunLength = 0;
				if(ThisRun != 0x7F){		//can't have a run of 0x7F b/c that calls 0xFF
					while(DecompressedMap[coY][coX] == ThisRun && coX < 256){
						RunLength++;
						coX++;}}
				else{ RunLength += 1; coX++;}
				if(RunLength == 1) kab += 1;
				else if(RunLength == 255) kab += 3;	//can't have a run of 255, b/c that is 0xFF
				else kab += 2;}
			kab += 1;	//for the 0xFF break at the end of each line
		}
	}

	kab = (OVERWORLDMAP_END - OVERWORLDMAP_START) - kab;
	CString text;
	text.Format("%d",kab);
	m_kab.SetWindowText(text);

}

void COverworldMapOrig::OnCustomizetool()
{
	CCustomTool dlg;
	dlg.dat = cart;
	dlg.m_tiles = &cart->m_overworldtiles;
	dlg.tool = cur_tool - 2;
	dlg.DoModal();
}

void COverworldMapOrig::CompressMap()
{
	int ThisRun, RunLength;
	int coY, coX, co, coR;
	bool recycle;

	co = OVERWORLDMAP_START;
	for(coY = 0; coY < 256; coY++){
		//check to see if we can recycle a line
		recycle = 0;
		for(coR = 0; coR < coY && !recycle; coR++){
			recycle = 1;
			for(coX = 0; coX < 256 && recycle; coX++)
				recycle = (DecompressedMap[coR][coX] == DecompressedMap[coY][coX]);}
		if(recycle){
			coR--;
			cart->ROM[OVERWORLDMAP_OFFSET + (coY << 1)] = cart->ROM[OVERWORLDMAP_OFFSET + (coR << 1)];
			cart->ROM[OVERWORLDMAP_OFFSET + (coY << 1) + 1] = cart->ROM[OVERWORLDMAP_OFFSET + (coR << 1) + 1];}
		else {
			cart->ROM[OVERWORLDMAP_OFFSET + (coY << 1)] = (co - OVERWORLDMAP_PTRADD) & 0xFF;
			cart->ROM[OVERWORLDMAP_OFFSET + (coY << 1) + 1] = (BYTE)((co - OVERWORLDMAP_PTRADD) >> 8);
			coX = 0;
			while (coX < 256) {
				ThisRun = DecompressedMap[coY][coX]; RunLength = 0;
				if (ThisRun == 0x7F) {
					RunLength = 1;
					coX++;
				}
				else {
					for (;coX < 256 && ThisRun == DecompressedMap[coY][coX]; coX++, RunLength++); // empty loop body
				}
				if (RunLength == 1) {
					cart->ROM[co] = (BYTE)ThisRun;
					co++;
				}
				else if (RunLength == 0xFF) {
					cart->ROM[co] = (BYTE)(ThisRun + 0x80);
					cart->ROM[co + 1] = 0xFE;
					cart->ROM[co + 2] = (BYTE)ThisRun;
					co += 3;
				}
				else {
					if (RunLength == 256) RunLength = 0;
					cart->ROM[co] = (BYTE)(ThisRun + 0x80);
					cart->ROM[co + 1] = (BYTE)RunLength;
					co += 2;
				}
			}
			cart->ROM[co] = 0xFF;
			co++;
		}
	}

	//Also, store the palette here as well... might as well  :P
	for(co = 0; co < 32; co++)
		cart->ROM[co + OVERWORLDPALETTE_OFFSET] = palette[0][co];
}

void COverworldMapOrig::OnRButtonDblClk(UINT nFlags, CPoint pt)
{
	UNREFERENCED_PARAMETER(nFlags);

	int ref = -1;
	if(PtInRect(rcMap,pt)){
		pt.x = ((pt.x - rcMap.left) >> 4) + ScrollOffset.x;
		pt.y = ((pt.y - rcMap.top) >> 4) + ScrollOffset.y;
		ref = DecompressedMap[pt.y][pt.x];}
	else if(PtInRect(rcTiles,pt)){
		pt.x = (pt.x - rcTiles.left) >> 4;
		pt.y = (pt.y - rcTiles.top) & 0xF0;
		ref = pt.x + pt.y;}

	if (ref != -1) {
		int old = cart->TintTiles[0][ref];
		CTint dlg;
		dlg.tintvalue = old;
		dlg.m_tintvariant = cart->TintVariant;
		if (dlg.DoModal() == IDOK) {
			cart->TintTiles[0][ref] = (BYTE)dlg.tintvalue;
			if (cart->TintVariant != dlg.m_tintvariant) {
				cart->TintVariant = (BYTE)dlg.m_tintvariant;
				cart->ReTintPalette();
			}
			cart->m_overworldtiles.DeleteImageList();
			cart->OK_overworldtiles = 0;
			ReloadGraphics();
			InvalidateRect(rcTiles, 0);
			InvalidateRect(rcMap, 0);
		}
	}
}

void COverworldMapOrig::OnEditgraphics()
{
	CBackdrop dlg;
	dlg.Project = cart;
	dlg.backdrop = (BYTE)m_backdroplist.GetCurSel();
	//DEVNOTE - since the user can click Save, then Cancel, checking for IDOK isn't sufficient
	//          to register a redraw of the backdrop. NeedsRefresh() tracks Save/Cancel and OK.
	dlg.DoModal();
	if (dlg.NeedsRefresh()) {
		m_backdrop.DeleteImageList();
		InitBackdrops();
		InvalidateRect(nullptr, 0);
	}
}

void COverworldMapOrig::OnLButtonDblClk(UINT nFlags, CPoint pt)
{
	UNREFERENCED_PARAMETER(nFlags);

	if(PtInRect(rcTiles,pt)){
		CTileEdit dlg;
		dlg.Invoker = CTileEdit::Overworld;
		dlg.cart = cart;
		dlg.tileset = 0;
		dlg.tile = (BYTE)cur_tile;
		dlg.pal[0] = palette[0];
		OnSave();
		if(dlg.DoModal() == IDOK){
			minimap.PalAssign[cur_tile] = cart->ROM[OVERWORLDPALETTE_ASSIGNMENT + cur_tile] & 3;
			if(m_minimap.GetCheck()) minimap.UpdateAll();
			cart->m_overworldtiles.DeleteImageList();
			cart->OK_overworldtiles = 0;
			ReloadGraphics();
			InvalidateRect(rcPalette,0);
			InvalidateRect(rcTiles,0);
			InvalidateRect(rcMap,0);
		}
	}
}

void COverworldMapOrig::OnSelchangeMisccoords() 
{
	CString text;
	text.Format("%X",misccoords[m_misccoords.GetCurSel()].x);
	m_miscx.SetWindowText(text);
	text.Format("%X",misccoords[m_misccoords.GetCurSel()].y);
	m_miscy.SetWindowText(text);
}

void COverworldMapOrig::StoreMiscCoords()
{
	misccoords[0].x -= 7;
	misccoords[0].y -= 7;
	if(misccoords[0].x < 0) misccoords[0].x = 0;
	if(misccoords[0].y < 0) misccoords[0].y = 0;
	for(int co = 0; co < 5; co++){
		cart->ROM[m_misccoordoffsets[co]] = (BYTE)misccoords[co].x;
		cart->ROM[m_misccoordoffsets[co] + 1] = (BYTE)misccoords[co].y;}
	misccoords[0].x += 7;
	misccoords[0].y += 7;
}

bool COverworldMapOrig::DoSave()
{
	try {
		CWaitCursor wait;
		OnFindKAB();
		if (kab < 0)
			THROWEXCEPTION(std::runtime_error, "Map is too complex to save.\nDid not save to ROM.");

		Project->ROM[OWBATTLERATE_LAND_OFFSET] = (unsigned char)(Ui::GetControlInt(m_landencrateedit) & 0xFF);
		Project->ROM[OWBATTLERATE_SEA_OFFSET] = (unsigned char)(Ui::GetControlInt(m_seaencrateedit) & 0xFF);

		StoreMiscCoords();
		StoreTileData();
		StoreDomain();
		CompressMap();
		SaveRom();
		return true;
	}
	catch (std::exception & ex) {
		AfxMessageBox("Unable to save ROM:\n" + CString(ex.what()), MB_ICONERROR);
	}
	catch (...) {
		AfxMessageBox("Unable to save ROM.", MB_ICONERROR);
	}
	return false;
}

void COverworldMapOrig::OnChangeMiscx() 
{
	CString text; int number;
	m_miscx.GetWindowText(text); number = StringToInt_HEX(text);
	if(number > 0xFF) number = 0xFF;
	misccoords[m_misccoords.GetCurSel()].x = number;
	InvalidateRect(rcMap,0);
}

void COverworldMapOrig::OnChangeMiscy() 
{
	CString text; int number;
	m_miscy.GetWindowText(text); number = StringToInt_HEX(text);
	if(number > 0xFF) number = 0xFF;
	misccoords[m_misccoords.GetCurSel()].y = number;
	InvalidateRect(rcMap,0);
}

void COverworldMapOrig::OnRButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	mousedown = (BYTE)0;
}

void COverworldMapOrig::OnMapExport() 
{
	CString text = "Overworld Map." + CString(FFH_MAP_EXT);
	CString filename = Paths::Combine({ FOLDERPREF(Project->AppSettings, PrefMapImportExportFolder) , text });
	auto result = Ui::SaveFilePromptExt(this, FFH_MAP_FILTER, FFH_MAP_EXT, "Export Overworld Map", filename);
	if (!result) return;

	FILE* file = fopen(result.value, "w+b");
	if(file == nullptr){
		AfxMessageBox("Error saving map", MB_ICONERROR);
		return;}
	fwrite(DecompressedMap,1,0x10000,file);
	fclose(file);
}

void COverworldMapOrig::OnMapImport() 
{
	auto result = OpenFilePromptExt(this, FFH_MAP_FILTER, FFH_MAP_EXT, "Import Overworld Map",
		FOLDERPREF(Project->AppSettings, PrefMapImportExportFolder));
	if (!result) return;

	FILE* file = fopen(result.value, "r+b");
	if(file == nullptr){
		AfxMessageBox("Error loading map", MB_ICONERROR);
		return;}
	fread(DecompressedMap,1,0x10000,file);
	fclose(file);
	InvalidateRect(rcMap,0);
}

void COverworldMapOrig::OnMinimap()
{
	if (m_minimap.GetCheck()) {
		minimap.SetFocusRect(ScrollOffset.x, ScrollOffset.y, ScrollOffset.x + 16, ScrollOffset.y + 16);
		//minimap.rcNew.SetRect(ScrollOffset.x, ScrollOffset.y, ScrollOffset.x + 16, ScrollOffset.y + 16);
		//minimap.rcOld = minimap.rcNew;
	}
	minimap.ShowWindow(m_minimap.GetCheck());
}

void COverworldMapOrig::OnViewcoords()
{
	coord_dlg.ShowWindow(m_viewcoords.GetCheck());
}

void COverworldMapOrig::UpdateTeleportLabel(int arid)
{
	int temp = m_teleportbox.GetCurSel();
	m_teleportbox.DeleteString(arid);
	m_teleportbox.InsertString(arid, LoadONTeleportLabel(*cart, arid).name);
	m_teleportbox.SetCurSel(temp);
}

void COverworldMapOrig::OnSelchangeTeleportbox()
{
	if(!coord_dlg.m_mouseclick.GetCheck()){
		coord_dlg.m_teleportlist.SetCurSel(m_teleportbox.GetCurSel());
		coord_dlg.OnSelchangeTeleportlist();}
}

void COverworldMapOrig::OnBnClickedButtonImportMap()
{
	OnMapImport();
}

void COverworldMapOrig::OnBnClickedButtonExportMap()
{
	OnMapExport();
}

void COverworldMapOrig::UpdateTeleportLabel(int areaid, int type)
{
	UNREFERENCED_PARAMETER(type);

	this->UpdateTeleportLabel(areaid);
}

void COverworldMapOrig::Cancel(int context)
{
	if (context == Coords) {
		m_viewcoords.SetCheck(0);
		DoViewcoords();
	}
	else if (context == Minimap) {
		m_minimap.SetCheck(0);
		DoMinimap();
	}
}

POINT COverworldMapOrig::GetLastClick()
{
	return ptLastClick;
}

int COverworldMapOrig::GetCurMap()
{
	return -1;
}

void COverworldMapOrig::TeleportHere(int mapindex, int x, int y)
{
	if (mapindex == 0xFF) {
		ptLastClick = CPoint(x, y);
		DoHScroll(5, x - 8, nullptr);
		DoVScroll(5, y - 8, nullptr);
		cur_tile = DecompressedMap[y][x];
		LoadTileData();
	}
	else {
		BootToTeleportFollowup = 1;
		OnOK();
	}
}