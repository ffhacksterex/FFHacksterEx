// MapsOrig.cpp : implementation file
//

#include "stdafx.h"
#include "MapsOrig.h"
#include <AppSettings.h>
#include <collection_helpers.h>
#include <core_exceptions.h>
#include <editor_label_functions.h>
#include <FFH2Project.h>
#include <general_functions.h>
#include <draw_functions.h>
#include <imaging_helpers.h>
#include <ingame_text_functions.h>
#include <ini_functions.h>
#include <io_functions.h>
#include <path_functions.h>
#include <string_functions.h>
#include <ui_helpers.h>
#include <ui_prompts.h>
#include <AsmFiles.h>
#include <DRAW_STRUCT.h>
#include <EntriesLoader.h>
#include <GameSerializer.h>
#include <ValueDataAccessor.h>

#include "Loading.h"
#include "Coords.h"
#include "CustomTool.h"
#include "Mapman.h"
#include "NESPalette.h"
#include "NewLabel.h"
#include "Tint.h"
#include "TileEdit.h"

#include <SpriteDialogueSettings.h>
#include "editors_common.h"

using namespace Editors;
using namespace Editorlabels;
using namespace Imaging;
using namespace Ini;
using namespace Ingametext;
using namespace Io;
using namespace Ui;
using namespace Strings;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace {
	constexpr auto FFH_MAP_FILTER = "FFHackster Standard Map (*.ffh;*.ffsmap)|*.ffh;*.ffsmap|All Files (*.*)|*.*||";
	constexpr auto FFH_MAP_EXT = "ffsmap";

	CCoords coords_dlg;
}


/////////////////////////////////////////////////////////////////////////////
// CMapsOrig dialog

CMapsOrig::CMapsOrig(CWnd* pParent /*= nullptr */)
	: CEditorWithBackground(CMapsOrig::IDD, pParent)
{
}

void CMapsOrig::DoDataExchange(CDataExchange* pDX)
{
	CEditorWithBackground::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIEWCOORDS, m_viewcoords);
	DDX_Control(pDX, IDC_KAB, m_kab);
	DDX_Control(pDX, IDC_CUSTOMTOOL, m_customtool);
	DDX_Control(pDX, IDC_SHOWLASTCLICK, m_showlastclick);
	DDX_Control(pDX, IDC_HOVERING, m_hovering);
	DDX_Control(pDX, IDC_LASTCLICK, m_lastclick);
	DDX_Control(pDX, IDC_SPRITEGRAPHIC, m_spritegraphic);
	DDX_Control(pDX, IDC_STILL, m_still);
	DDX_Control(pDX, IDC_SPRITECOORDY, m_spritecoordy);
	DDX_Control(pDX, IDC_SPRITECOORDX, m_spritecoordx);
	DDX_Control(pDX, IDC_SPRITE, m_sprite);
	DDX_Control(pDX, IDC_INROOM, m_inroom);
	DDX_Control(pDX, IDC_SPRITE_LIST, m_sprite_list);
	DDX_Control(pDX, IDC_EDITLABEL, m_editlabel);
	DDX_Control(pDX, IDC_WARP, m_warp);
	DDX_Control(pDX, IDC_TEXT_LIST, m_text_list);
	DDX_Control(pDX, IDC_TELEPORT_LIST, m_teleport_list);
	DDX_Control(pDX, IDC_TELEPORT, m_teleport);
	DDX_Control(pDX, IDC_TCITEM_PRICE, m_tcitem_price);
	DDX_Control(pDX, IDC_TCITEM_LIST, m_tcitem_list);
	DDX_Control(pDX, IDC_TC_LIST, m_tc_list);
	DDX_Control(pDX, IDC_TC, m_tc);
	DDX_Control(pDX, IDC_SPECIAL_LIST, m_special_list);
	DDX_Control(pDX, IDC_SPECIAL, m_special);
	DDX_Control(pDX, IDC_SHOP_LIST, m_shop_list);
	DDX_Control(pDX, IDC_SHOP, m_shop);
	DDX_Control(pDX, IDC_MOVE, m_move);
	DDX_Control(pDX, IDC_FIGHT_LIST, m_fight_list);
	DDX_Control(pDX, IDC_FIGHT, m_fight);
	DDX_Control(pDX, IDC_TILESET, m_tileset);
	DDX_Control(pDX, IDC_PROBABILITY8, m_probability8);
	DDX_Control(pDX, IDC_PROBABILITY7, m_probability7);
	DDX_Control(pDX, IDC_PROBABILITY6, m_probability6);
	DDX_Control(pDX, IDC_PROBABILITY5, m_probability5);
	DDX_Control(pDX, IDC_PROBABILITY4, m_probability4);
	DDX_Control(pDX, IDC_PROBABILITY3, m_probability3);
	DDX_Control(pDX, IDC_PROBABILITY2, m_probability2);
	DDX_Control(pDX, IDC_PROBABILITY1, m_probability1);
	DDX_Control(pDX, IDC_FORMATION8, m_formation8);
	DDX_Control(pDX, IDC_FORMATION7, m_formation7);
	DDX_Control(pDX, IDC_FORMATION6, m_formation6);
	DDX_Control(pDX, IDC_FORMATION5, m_formation5);
	DDX_Control(pDX, IDC_FORMATION4, m_formation4);
	DDX_Control(pDX, IDC_FORMATION3, m_formation3);
	DDX_Control(pDX, IDC_FORMATION2, m_formation2);
	DDX_Control(pDX, IDC_FORMATION1, m_formation1);
	DDX_Control(pDX, IDC_BATTLE8, m_battle8);
	DDX_Control(pDX, IDC_BATTLE7, m_battle7);
	DDX_Control(pDX, IDC_BATTLE6, m_battle6);
	DDX_Control(pDX, IDC_BATTLE5, m_battle5);
	DDX_Control(pDX, IDC_BATTLE4, m_battle4);
	DDX_Control(pDX, IDC_BATTLE3, m_battle3);
	DDX_Control(pDX, IDC_BATTLE2, m_battle2);
	DDX_Control(pDX, IDC_BATTLE1, m_battle1);
	DDX_Control(pDX, IDC_VSCROLL, m_vscroll);
	DDX_Control(pDX, IDC_HSCROLL, m_hscroll);
	DDX_Control(pDX, IDC_SHOWROOMS, m_showrooms);
	DDX_Control(pDX, IDC_MAPLIST, m_maplist);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDC_STATIC_MAP, m_mapstatic);
	DDX_Control(pDX, IDC_STATIC_TILES, m_tilestatic);
	DDX_Control(pDX, IDC_STATIC_MAPPALETTES, m_palettestatic);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
	DDX_Control(pDX, IDC_MAPS_ENCOUNTERRATE, m_encounterrateedit);
}


BEGIN_MESSAGE_MAP(CMapsOrig, CEditorWithBackground)
	ON_WM_PAINT()
	ON_LBN_SELCHANGE(IDC_MAPLIST, OnSelchangeMaplist)
	ON_BN_CLICKED(IDC_SHOWROOMS, OnShowrooms)
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_FIGHT, OnFight)
	ON_BN_CLICKED(IDC_SHOP, OnShop)
	ON_BN_CLICKED(IDC_SPECIAL, OnSpecial)
	ON_BN_CLICKED(IDC_TC, OnTc)
	ON_BN_CLICKED(IDC_WARP, OnWarp)
	ON_BN_CLICKED(IDC_TELEPORT, OnTeleport)
	ON_BN_CLICKED(IDC_MOVE, OnMove)
	ON_CBN_SELCHANGE(IDC_TC_LIST, OnSelchangeTcList)
	ON_CBN_SELCHANGE(IDC_TCITEM_LIST, OnSelchangeTcitemList)
	ON_CBN_SELCHANGE(IDC_TILESET, OnSelchangeTileset)
	ON_CBN_SELCHANGE(IDC_SPRITE_LIST, OnSelchangeSpriteList)
	ON_BN_CLICKED(IDC_EDITSPRITEGFX, OnEditspritegfx)
	ON_BN_CLICKED(IDC_STILL, OnStill)
	ON_BN_CLICKED(IDC_INROOM, OnInroom)
	ON_EN_CHANGE(IDC_SPRITECOORDX, OnChangeSpritecoordx)
	ON_EN_CHANGE(IDC_SPRITECOORDY, OnChangeSpritecoordy)
	ON_CBN_SELCHANGE(IDC_SPRITE, OnSelchangeSprite)
	ON_CBN_SELCHANGE(IDC_SPRITEGRAPHIC, OnSelchangeSpritegraphic)
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_SHOWLASTCLICK, OnShowlastclick)
	ON_BN_CLICKED(IDC_CUSTOMTOOL, OnCustomtool)
	ON_BN_CLICKED(IDC_FINDKAB, OnFindkab)
	ON_BN_CLICKED(IDC_EDIT_SPRITE_LABEL, OnEditSpriteLabel)
	ON_BN_CLICKED(IDC_EDITGFXLABEL, OnEditgfxlabel)
	ON_BN_CLICKED(IDC_MAPLABEL, OnMaplabel)
	ON_BN_CLICKED(IDC_EDITLABEL, OnEditlabel)
	ON_BN_CLICKED(IDC_TILESETLABEL, OnTilesetlabel)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_VIEWCOORDS, OnViewcoords)
	ON_CBN_SELCHANGE(IDC_TELEPORT_LIST, OnSelchangeTeleportList)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_MAP, OnMapImport)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_MAP, OnMapExport)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMapsOrig message handlers

void CMapsOrig::LoadOffsets()
{
	ffh::acc::ValueDataAccessor d(*Proj2);
	ITEMPRICE_OFFSET = d.get<int>("ITEMPRICE_OFFSET");
	MAPSPRITEPATTERNTABLE_COUNT = d.get<int>("MAPSPRITEPATTERNTABLE_COUNT");
	MAPSPRITEPATTERNTABLE_OFFSET = d.get<int>("MAPSPRITEPATTERNTABLE_OFFSET");
	MAPSPRITE_PICASSIGNMENT = d.get<int>("MAPSPRITE_PICASSIGNMENT");
	MAPTILESET_ASSIGNMENT = d.get<int>("MAPTILESET_ASSIGNMENT");
	MAPPALETTE_OFFSET = d.get<int>("MAPPALETTE_OFFSET");
	TILESETPATTERNTABLE_OFFSET = d.get<int>("TILESETPATTERNTABLE_OFFSET");
	TILESETPALETTE_ASSIGNMENT = d.get<int>("TILESETPALETTE_ASSIGNMENT");
	TILESETPATTERNTABLE_ASSIGNMENT = d.get<int>("TILESETPATTERNTABLE_ASSIGNMENT");
	TILESET_TILEDATA = d.get<int>("TILESET_TILEDATA");
	MAP_END = d.get<int>("MAP_END");
	MAP_START = d.get<int>("MAP_START");
	MAP_COUNT = d.get<int>("MAP_COUNT");
	MAP_OFFSET = d.get<int>("MAP_OFFSET");
	MAP_PTRADD = d.get<int>("MAP_PTRADD");
	NNTELEPORT_COUNT = d.get<int>("NNTELEPORT_COUNT");
	ONTELEPORT_COUNT = d.get<int>("ONTELEPORT_COUNT");
	MAPSPRITE_COUNT = d.get<int>("MAPSPRITE_COUNT");
	MAPSPRITE_OFFSET = d.get<int>("MAPSPRITE_OFFSET");
	BATTLEDOMAIN_OFFSET = d.get<int>("BATTLEDOMAIN_OFFSET");
	BATTLEPROBABILITY_OFFSET = d.get<int>("BATTLEPROBABILITY_OFFSET");
	TREASURE_OFFSET = d.get<int>("TREASURE_OFFSET");
	MAPBATTLERATE_OFFSET = d.get<int>("MAPBATTLERATE_OFFSET");

	BANK0A_OFFSET = d.get<int>("BANK0A_OFFSET");
	BANK00_OFFSET = d.get<int>("BANK00_OFFSET");
	BANK02_OFFSET = d.get<int>("BANK02_OFFSET");
	BANK03_OFFSET = d.get<int>("BANK03_OFFSET");
	BANK04_OFFSET = d.get<int>("BANK04_OFFSET");
	BANK05_OFFSET = d.get<int>("BANK05_OFFSET");
	BANK06_OFFSET = d.get<int>("BANK06_OFFSET");
	BANK07_OFFSET = d.get<int>("BANK07_OFFSET");
	BINBANK01DATA_OFFSET = d.get<int>("BINBANK01DATA_OFFSET");
	BINPRICEDATA_OFFSET = d.get<int>("BINPRICEDATA_OFFSET");
}

void CMapsOrig::LoadRom()
{
	Proj2->ClearROM();
	if (Proj2->IsRom()) {
		Proj2->LoadROM();
	}
	else if (Proj2->IsAsm()) {
		GameSerializer ser(*Proj2);
		// Instead of writing to the entire buffer, just write to the parts we need
		// Note that tile data spans from bank 3 through the end of bank 6.
		ser.LoadAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.LoadAsmBin(BANK_00, BANK00_OFFSET);
		ser.LoadAsmBin(BANK_02, BANK02_OFFSET);
		ser.LoadAsmBin(BANK_03, BANK03_OFFSET);
		ser.LoadAsmBin(BANK_04, BANK04_OFFSET);
		ser.LoadAsmBin(BANK_05, BANK05_OFFSET);
		ser.LoadAsmBin(BANK_06, BANK06_OFFSET);
		ser.LoadAsmBin(BANK_07, BANK07_OFFSET);
		ser.LoadAsmBin(BIN_BANK01DATA, BINBANK01DATA_OFFSET);
		ser.LoadAsmBin(BIN_BATTLEDOMAINDATA, BATTLEDOMAIN_OFFSET);
		ser.LoadAsmBin(BIN_PRICEDATA, BINPRICEDATA_OFFSET);
		ser.LoadAsmBin(BIN_MAPENCOUNTERRATES, MAPBATTLERATE_OFFSET);
		ser.LoadInline(ASM_0F, { { asmlabel, "lut_FormationWeight", { BATTLEPROBABILITY_OFFSET } } });
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, Proj2->info.type);
	}
}

void CMapsOrig::SaveRom()
{
	if (Proj2->IsRom()) {
		Proj2->SaveROM();
	}
	else if (Proj2->IsAsm()) {
		GameSerializer ser(*Proj2);
		// Instead of writing to the entire buffer, just write to the parts we need
		// Note that tile data spans from bank 3 through the end of bank 6.
		ser.SaveAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.SaveAsmBin(BANK_00, BANK00_OFFSET);
		ser.SaveAsmBin(BANK_02, BANK02_OFFSET);
		ser.SaveAsmBin(BANK_03, BANK03_OFFSET);
		ser.SaveAsmBin(BANK_04, BANK04_OFFSET);
		ser.SaveAsmBin(BANK_05, BANK05_OFFSET);
		ser.SaveAsmBin(BANK_06, BANK06_OFFSET);
		ser.SaveAsmBin(BANK_07, BANK07_OFFSET);
		ser.SaveAsmBin(BIN_BANK01DATA, BINBANK01DATA_OFFSET);
		ser.SaveAsmBin(BIN_BATTLEDOMAINDATA, BATTLEDOMAIN_OFFSET);
		ser.SaveAsmBin(BIN_PRICEDATA, BINPRICEDATA_OFFSET);
		ser.SaveAsmBin(BIN_MAPENCOUNTERRATES, MAPBATTLERATE_OFFSET);
		ser.SaveInline(ASM_0F, { { asmlabel, "lut_FormationWeight", { BATTLEPROBABILITY_OFFSET } } });
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, Proj2->info.type);
	}
}

BOOL CMapsOrig::OnInitDialog() 
{
	Proj2 = Proj2;
	CEditorWithBackground::OnInitDialog();

	try {
		if (Proj2 == nullptr) throw std::runtime_error("no project was specified for this editor");
		if (Enloader == nullptr) throw std::runtime_error("No entry loader was specified for this editor");

		this->LoadOffsets();
		this->LoadRom();

		Sprite_Coords.resize(MAPSPRITE_COUNT);
		Sprite_InRoom.resize(MAPSPRITE_COUNT);
		Sprite_StandStill.resize(MAPSPRITE_COUNT);
		Sprite_Value.resize(MAPSPRITE_COUNT);

		coords_dlg.Context = Coords;
		coords_dlg.STparent = this;
		coords_dlg.Proj2 = Proj2;
		coords_dlg.Create(IDD_COORDS, this);
		coords_dlg.IsOV = 0;
		coords_dlg.Boot();
		m_viewcoords.SetCheck(BootToTeleportFollowup);
		OnViewcoords();

		mousedown = 0;
		IF_NOHANDLE(m_tools).Create(25, 25, ILC_COLOR4, 8, 0);
		IF_NOHANDLE(redpen).CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		IF_NOHANDLE(m_sprites).Create(16, 16, ILC_COLOR16 + ILC_MASK, 16, 0);

		cur_tool = 0;

		CBitmap bmp;
		bmp.LoadBitmap(IDB_DRAWINGTOOLS);
		m_tools.Add(&bmp, RGB(0, 0, 0)); bmp.DeleteObject();

		ScrollOffset.x = 0;
		ScrollOffset.y = 0;
		m_hscroll.SetScrollRange(0, 48);
		m_vscroll.SetScrollRange(0, 48);

		CRect rcpos;
		GetControlRect(&m_showlastclick, &rcpos);
		rcTools.SetRect(0, 0, 100, 25);
		rcTools.OffsetRect(rcpos.left, rcpos.bottom + 3);

		GetControlRect(&m_tilestatic, &rcpos);
		rcTiles.SetRect(0, 0, 256, 128);
		rcTiles.OffsetRect(rcpos.left + 8, rcpos.top + 16);

		GetControlRect(&m_mapstatic, &rcpos);
		rcMap.SetRect(0, 0, 256, 256);
		rcMap.OffsetRect(rcpos.left + 8, rcpos.top + 16);

		GetControlRect(&m_palettestatic, &rcpos);
		rcPalettes.SetRect(0, 0, 256, 32);
		rcPalettes.OffsetRect(rcpos.left + 4, rcpos.top + 4);
		rcPalettes2.SetRect(0, 0, 128, 32);
		rcPalettes2.OffsetRect(rcPalettes.left, rcPalettes.bottom);

		LoadCombo(m_tcitem_list, Enloader->LoadTreasureItemEntries(*Proj2, true));

		LoadListBox(m_maplist, Labels2::LoadMapLabels(*Proj2));
		for (auto wnd : { &m_battle1, &m_battle2, &m_battle3,&m_battle4,&m_battle5,&m_battle6,&m_battle7,&m_battle8 })
			LoadCombo(*wnd, Labels2::LoadBattleLabels(*Proj2));

		CSpriteDialogueSettings dlgstgs(*Proj2);
		LoadCombo(m_text_list, (dlgstgs.ShowActualText ? LoadGameTextEntries(*Proj2, true) : Labels2::LoadTextLabels(*Proj2, true)));

		LoadCombo(m_tileset, Labels2::LoadTilesetLabels(*Proj2));
		LoadCombo(m_fight_list, Labels2::LoadBattleLabels(*Proj2));
		m_fight_list.InsertString(0, "--Random Encounters--");

		LoadCombo(m_shop_list, Labels2::LoadShopLabels(*Proj2));
		LoadCombo(m_tc_list, Labels2::LoadTreasureLabels(*Proj2));
		LoadCombo(m_special_list, Labels2::LoadSpecialTileLabels(*Proj2));
		LoadCombo(m_teleport_list, Labels2::LoadNNTeleportLabels(*Proj2) + Labels2::LoadNOTeleportLabels(*Proj2));
		LoadCombo(m_sprite_list, Labels2::LoadOnScreenSpriteLabels(*Proj2));
		LoadCombo(m_spritegraphic, Labels2::LoadSpriteGraphicLabels(*Proj2));
		LoadCombo(m_sprite, Labels2::LoadSpriteLabels(*Proj2, true));

		m_showlastclick.SetCheck(Proj2->session.showLastClick);

		cur_tc = -1;
		cur_tcitem = -1;
		cur_map = -1;
		cur_tile = 0;
		cur_tileset = 0;

		m_sprite_list.SetCurSel(0);
		m_tileset.SetCurSel(0);
		m_maplist.SetCurSel(0);

		if (BootToTeleportFollowup) {
			m_maplist.SetCurSel(Proj2->TeleportFollowup[Proj2->curFollowup][0]);
			ptLastClick.x = Proj2->TeleportFollowup[Proj2->curFollowup][1];
			ptLastClick.y = Proj2->TeleportFollowup[Proj2->curFollowup][2];
			OnHScroll(5, ptLastClick.x - 8, &m_hscroll);
			OnVScroll(5, ptLastClick.y - 8, &m_vscroll);
		}

		OnSelchangeMaplist();

		m_banner.Set(this, COLOR_BLACK, COLOR_ORANGE, "Standard Maps");
	}
	catch (std::exception & ex) {
		return Ui::AbortFail(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortFail(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}

void CMapsOrig::ReloadSprites(CProgressCtrl* m_prog)
{
	while(m_sprites.GetImageCount()) m_sprites.Remove(0);
	CPaintDC dc(this);
	CDC mDC; mDC.CreateCompatibleDC(&dc);
	CBitmap bmp; bmp.CreateCompatibleBitmap(&dc,16,16);
	CBitmap dummy; dummy.CreateCompatibleBitmap(&dc,1,1);
	mDC.SelectObject(&bmp);

	int offset, co;
	CPoint pt;

	for(co = 0; co < MAPSPRITEPATTERNTABLE_COUNT; co++){
		offset = MAPSPRITEPATTERNTABLE_OFFSET + (co << 8);
		DrawTile(&mDC,0,0,Proj2,offset     ,SpritePalette[0]);
		DrawTile(&mDC,8,0,Proj2,offset + 16,SpritePalette[0]);
		DrawTile(&mDC,0,8,Proj2,offset + 32,SpritePalette[1]);
		DrawTile(&mDC,8,8,Proj2,offset + 48,SpritePalette[1]);
		mDC.SelectObject(&dummy);
		m_sprites.Add(&bmp,Proj2->palette[0][0x40]);
		mDC.SelectObject(&bmp);
		m_prog->OffsetPos(1);
	}

	mDC.DeleteDC();
	dummy.DeleteObject();
	bmp.DeleteObject();
}

void CMapsOrig::ReloadImages(CProgressCtrl* m_prog)
{
	if(!Proj2->OK_tiles[cur_map]){
		Proj2->GetStandardTiles(cur_map,false).Create(16, 16, ILC_COLOR16, 64, 0);
		Proj2->GetStandardTiles(cur_map,true).Create(16, 16, ILC_COLOR16, 64, 0);
		CPaintDC dc(this);
		CDC mDC; mDC.CreateCompatibleDC(&dc);
		CBitmap bmp; bmp.CreateCompatibleBitmap(&dc,16,16);
		CBitmap dummy; dummy.CreateCompatibleBitmap(&dc,1,1);
		mDC.SelectObject(&bmp);

		int offset;
		int co;
		int pal;
		int temp = (cur_tileset << 11) + TILESETPATTERNTABLE_OFFSET;
		const auto& tints = Proj2->session.tintTiles;

		for(int showrm = 0; showrm < 2; showrm++){
		for(co = 0; co < 128; co++){
			pal = Proj2->ROM[TILESETPALETTE_ASSIGNMENT + co + (cur_tileset << 7)] & 3;
			offset = TILESETPATTERNTABLE_ASSIGNMENT + co + (cur_tileset << 9);

			DrawTile(&mDC,0,0,Proj2,temp + (Proj2->ROM[offset] << 4),MapPalette[showrm][pal],tints[cur_tileset + 1][co]);
			DrawTile(&mDC,8,0,Proj2,temp + (Proj2->ROM[offset + 128] << 4),MapPalette[showrm][pal],tints[cur_tileset + 1][co]);
			DrawTile(&mDC,0,8,Proj2,temp + (Proj2->ROM[offset + 256] << 4),MapPalette[showrm][pal],tints[cur_tileset + 1][co]);
			DrawTile(&mDC,8,8,Proj2,temp + (Proj2->ROM[offset + 384] << 4),MapPalette[showrm][pal],tints[cur_tileset + 1][co]);

			mDC.SelectObject(&dummy);
			Proj2->GetStandardTiles(cur_map,showrm).Add(&bmp,RGB(255,1,255));
			mDC.SelectObject(&bmp);
			m_prog->OffsetPos(1);
		}}

		mDC.DeleteDC();
		bmp.DeleteObject();
		dummy.DeleteObject();
		Proj2->OK_tiles[cur_map] = 1;
	}
	else m_prog->OffsetPos(256);
}

void CMapsOrig::LoadValues()
{
	//load the tileset
	cur_tileset = Proj2->ROM[MAPTILESET_ASSIGNMENT + cur_map];
	m_tileset.SetCurSel(cur_tileset);

	//load the palettes
	int offset = MAPPALETTE_OFFSET + (cur_map * 0x30) + 0x10;
	int co;
	for(co = 0; co < 8; co++, offset++) ControlPalette[co] = Proj2->ROM[offset];
	for(co = 0; co < 8; co++, offset++) SpritePalette[0][co] = Proj2->ROM[offset];
	SpritePalette[0][0] = 0x40; SpritePalette[1][0] = 0x40;
	offset -= 0x20;
	for(co = 0; co < 0x20; co++, offset++){
		if(co == 0x10) offset += 0x10;
		if((co & 3) == 0) MapPalette[0][0][co] = ControlPalette[0];
		else MapPalette[0][0][co] = Proj2->ROM[offset];}

	CLoading dlg; dlg.Create(IDD_LOADING,this);
	dlg.m_progress.SetRange(0,286);
	dlg.m_progress.SetPos(0);
	dlg.ShowWindow(1);

	ReloadImages(&dlg.m_progress);
	ReloadSprites(&dlg.m_progress);

	dlg.ShowWindow(0);

	BYTE temp;
	Maps::DecompressMap(*Proj2, cur_map, MAP_OFFSET, MAP_PTRADD, DecompressedMap);

	//load the 16 sprites
	offset = MAPSPRITE_OFFSET + (cur_map * 0x30);
	for(auto coX = 0; coX < MAPSPRITE_COUNT; offset += 3, coX++){
		Sprite_Value[coX] = Proj2->ROM[offset];
		temp = Proj2->ROM[offset + 1];
		Sprite_Coords[coX].x = temp & 0x3F;
		Sprite_InRoom[coX] = (temp & 0x80) != 0;
		Sprite_StandStill[coX] = (temp & 0x40) != 0;
		Sprite_Coords[coX].y = Proj2->ROM[offset + 2];}

	OnSelchangeSpriteList();

	//load the domains
	offset = BATTLEDOMAIN_OFFSET + 0x200 + (cur_map << 3);
	m_battle1.SetCurSel(Proj2->ROM[offset] & 0x7F); m_formation1.SetCheck(Proj2->ROM[offset] & 0x80); offset += 1;
	m_battle2.SetCurSel(Proj2->ROM[offset] & 0x7F); m_formation2.SetCheck(Proj2->ROM[offset] & 0x80); offset += 1;
	m_battle3.SetCurSel(Proj2->ROM[offset] & 0x7F); m_formation3.SetCheck(Proj2->ROM[offset] & 0x80); offset += 1;
	m_battle4.SetCurSel(Proj2->ROM[offset] & 0x7F); m_formation4.SetCheck(Proj2->ROM[offset] & 0x80); offset += 1;
	m_battle5.SetCurSel(Proj2->ROM[offset] & 0x7F); m_formation5.SetCheck(Proj2->ROM[offset] & 0x80); offset += 1;
	m_battle6.SetCurSel(Proj2->ROM[offset] & 0x7F); m_formation6.SetCheck(Proj2->ROM[offset] & 0x80); offset += 1;
	m_battle7.SetCurSel(Proj2->ROM[offset] & 0x7F); m_formation7.SetCheck(Proj2->ROM[offset] & 0x80); offset += 1;
	m_battle8.SetCurSel(Proj2->ROM[offset] & 0x7F); m_formation8.SetCheck(Proj2->ROM[offset] & 0x80);

	offset = BATTLEPROBABILITY_OFFSET;
	int buffer[8] = {0,0,0,0,0,0,0,0};
	for(co = 0; co < 64; co++)
		buffer[Proj2->ROM[offset + co]] += 1;
	CString text;
	CEdit* m_edit[8] = {&m_probability1,&m_probability2,&m_probability3,&m_probability4,
		&m_probability5,&m_probability6,&m_probability7,&m_probability8};
	for(co = 0; co < 8; co++){
		text.Format("%d",buffer[co]);
		m_edit[co]->SetWindowText(text);}

	offset = MAPBATTLERATE_OFFSET + cur_map + 1; // skip first entry (overworld)
	Ui::SetControlInt(m_encounterrateedit, (int)Proj2->ROM[offset]);

	//load tile data
	if(BootToTeleportFollowup){
		BootToTeleportFollowup = 0;
		cur_tile = DecompressedMap[ptLastClick.y][ptLastClick.x];}
	LoadTileData();

	OnFindkab();
}

void CMapsOrig::LoadTileData()
{
	int offset = TILESET_TILEDATA + (cur_tileset << 8) + (cur_tile << 1);
	BYTE temp = Proj2->ROM[offset];
	BYTE byte2 = Proj2->ROM[offset + 1];

	m_teleport_list.SetCurSel(byte2); 
	m_text_list.SetCurSel(byte2);
	m_fight_list.SetCurSel((byte2 + 1) % 0x81);
	m_shop_list.SetCurSel(byte2 - 1);
	m_tc_list.SetCurSel(byte2);

	cur_tiledata = 0;

	if(!(temp & 1)) cur_tiledata += 1;

	switch(temp & 0xC0){
	case 0x40: cur_tiledata += 0x20; break;
	case 0x80: cur_tiledata += 0x40; break;
	case 0xC0: cur_tiledata += 0x40; m_teleport_list.SetCurSel(byte2 + NNTELEPORT_COUNT); break;}

	if(temp == 0x0A) cur_tiledata += 0x02;
	if(((temp & 0x3E) == 0x02) && byte2 != 0) cur_tiledata += 0x04;
	if((temp & 0x3E) == 0x08) cur_tiledata += 0x08;

	if(!(cur_tiledata & 0x04)){
		cur_tiledata += 0x10;
		switch(temp & 0x3F){
		case 0x03: case 0x02: byte2 = 0; break;
		case 0x06: byte2 = 1; break;
		case 0x05: byte2 = 2; break;
		case 0x0E: byte2 = 3; break;
		case 0x10: byte2 = 4; break;
		case 0x13: byte2 = 5; break;
		case 0x18: byte2 = 6; break;
		case 0x1A: byte2 = 7; break;
		case 0x1C: byte2 = 8; break;
		case 0x1E: byte2 = 9; break;
		case 0x0C: byte2 = 10; break;
		case 0x14: byte2 = 11; break;
		case 0x16: byte2 = 12; break;
		default: cur_tiledata -= 0x10;}
		m_special_list.SetCurSel(byte2);}

	if(m_text_list.GetCurSel() == -1) m_text_list.SetCurSel(0);
	if(m_fight_list.GetCurSel() == -1) m_fight_list.SetCurSel(0);
	if(m_shop_list.GetCurSel() == -1) m_shop_list.SetCurSel(0);
	if(m_tc_list.GetCurSel() == -1) m_tc_list.SetCurSel(0);
	if(m_special_list.GetCurSel() == -1) m_special_list.SetCurSel(0);
	if(m_teleport_list.GetCurSel() == -1) m_teleport_list.SetCurSel(0);

	if(cur_tiledata & 0x40)
		OnSelchangeTeleportList();

	cur_tc = -1;
	OnSelchangeTcList();

	UpdateTileData();
}

void CMapsOrig::StoreValues()
{
	StoreTileData();

	//store the tileset
	Proj2->ROM[MAPTILESET_ASSIGNMENT + cur_map] = (BYTE)cur_tileset;

	//store the palettes
	int offset = MAPPALETTE_OFFSET + (cur_map * 0x30) + 0x10;
	int co;
	for (co = 0; co < 8; co++, offset++) Proj2->ROM[offset] = ControlPalette[co];
	for (co = 0; co < 8; co++, offset++) Proj2->ROM[offset] = SpritePalette[0][co];
	Proj2->ROM[offset - 8] = 0x0F; Proj2->ROM[offset - 4] = 0x0F;
	offset -= 0x20;
	for (co = 0; co < 0x20; co++, offset++) {
		if (co == 0x10) offset += 0x10;
		Proj2->ROM[offset] = MapPalette[0][0][co];
	}

	//store the 16 sprites
	offset = MAPSPRITE_OFFSET + (cur_map * 0x30);
	BYTE temp = 0;
	for (co = 0; co < MAPSPRITE_COUNT; offset += 3, co++) {
		Proj2->ROM[offset] = (BYTE)Sprite_Value[co];
		temp = (BYTE)Sprite_Coords[co].x;
		if (Sprite_InRoom[co]) temp |= 0x80;
		if (Sprite_StandStill[co]) temp |= 0x40;
		Proj2->ROM[offset + 1] = temp;
		Proj2->ROM[offset + 2] = (BYTE)Sprite_Coords[co].y;
	}

	//store the domains
	int ref = BATTLEDOMAIN_OFFSET + 0x200 + (cur_map << 3);

	Proj2->ROM[ref] = (BYTE)(m_battle1.GetCurSel() + (m_formation1.GetCheck() << 7));
	Proj2->ROM[ref + 1] = (BYTE)(m_battle2.GetCurSel() + (m_formation2.GetCheck() << 7));
	Proj2->ROM[ref + 2] = (BYTE)(m_battle3.GetCurSel() + (m_formation3.GetCheck() << 7));
	Proj2->ROM[ref + 3] = (BYTE)(m_battle4.GetCurSel() + (m_formation4.GetCheck() << 7));
	Proj2->ROM[ref + 4] = (BYTE)(m_battle5.GetCurSel() + (m_formation5.GetCheck() << 7));
	Proj2->ROM[ref + 5] = (BYTE)(m_battle6.GetCurSel() + (m_formation6.GetCheck() << 7));
	Proj2->ROM[ref + 6] = (BYTE)(m_battle7.GetCurSel() + (m_formation7.GetCheck() << 7));
	Proj2->ROM[ref + 7] = (BYTE)(m_battle8.GetCurSel() + (m_formation8.GetCheck() << 7));

	int hold[8] = { 0,0,0,0,0,0,0,0 };
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
	int bigco;
	for (bigco = 0; bigco < 8; bigco++) {
		for (co = 0; co < hold[bigco] && ref < max; co++, ref++)
			Proj2->ROM[ref] = (BYTE)bigco;
	}

	for (;ref < max; ref++)
		Proj2->ROM[ref] = 0;

	offset = MAPBATTLERATE_OFFSET + cur_map + 1; // skip first entry (overworld)
	Proj2->ROM[offset] = (unsigned char)(Ui::GetControlInt(m_encounterrateedit) & 0xFF);

	OnFindkab();
	if (kab < 0)
		THROWEXCEPTION(std::runtime_error, "Not enough KAB to store map data.");

	CompressMap();
}

void CMapsOrig::StoreTileData()
{
	StoreTC();
	
	int offset = TILESET_TILEDATA + (cur_tileset << 8) + (cur_tile << 1);
	BYTE temp = 0;
	int byte2 = m_text_list.GetCurSel();

	if(!(cur_tiledata & 0x01)) temp = 1;
	if(cur_tiledata & 0x02){
		temp |= 0x0A; byte2 = (m_fight_list.GetCurSel() + 0x80) % 0x81;}
	if(cur_tiledata & 0x04){
		temp |= 0x02; byte2 = m_shop_list.GetCurSel() + 1;}
	if(cur_tiledata & 0x08){
		temp |= 0x08; byte2 = m_tc_list.GetCurSel();}
	if(cur_tiledata & 0x10){
		switch(m_special_list.GetCurSel()){
		case 0: temp = 0x03; break;
		case 1: temp = 0x06; break;
		case 2: temp = 0x05; break;
		case 3: temp = 0x0E; break;
		case 4: temp = 0x10; break;
		case 5: temp = 0x13; break;
		case 6: temp = 0x18; break;
		case 7: temp = 0x1A; break;
		case 8: temp = 0x1C; break;
		case 9: temp = 0x1E; break;
		case 10: temp = 0x0C; break;
		case 11: temp = 0x14; break;
		case 12: temp = 0x16; break;}}
	if(cur_tiledata & 0x20)
		temp |= 0x40;
	if(cur_tiledata & 0x40){
		temp |= 0x80;
		byte2 = m_teleport_list.GetCurSel();
		if(byte2 > NNTELEPORT_COUNT){
			temp |= 0x40;
			byte2 -= NNTELEPORT_COUNT;}}

	Proj2->ROM[offset] = temp;
	Proj2->ROM[offset + 1] = (BYTE)byte2;
}

void CMapsOrig::OnPaint() 
{
	CPaintDC dc(this);
	CPen* origpen = dc.SelectObject(&redpen);
	int coX, coY, tile, coy, cox;
	CPoint pt;
	bool room = m_showrooms.GetCheck() == 1;

	//Draw the Tiles on the screen
	for(coY = 0, tile = 0, pt.y = rcTiles.top; coY < 0x08; coY++, pt.y += 16){
	for(coX = 0, pt.x = rcTiles.left; coX < 0x10; coX++, pt.x += 16, tile++)
		Proj2->GetStandardTiles(cur_map,room).Draw(&dc,tile,pt,ILD_NORMAL);}
	pt.x = ((cur_tile & 0x0F) << 4) + rcTiles.left;
	pt.y = (cur_tile & 0xF0) + rcTiles.top;
	dc.MoveTo(pt); pt.x += 15;
	dc.LineTo(pt); pt.y += 15;
	dc.LineTo(pt); pt.x -= 15;
	dc.LineTo(pt); pt.y -= 15;
	dc.LineTo(pt);

	//Draw the map
	for(coY = 0, pt.y = rcMap.top, coy = ScrollOffset.y; coY < 0x10; coY++, pt.y += 16, coy++){
	for(coX = 0, pt.x = rcMap.left, cox = ScrollOffset.x; coX < 0x10; coX++, pt.x += 16, cox++)
		Proj2->GetStandardTiles(cur_map,room).Draw(&dc,DecompressedMap[coy][cox],pt,ILD_NORMAL);}
	CRect rcTemp = rcToolRect; rcTemp.NormalizeRect(); rcTemp.bottom += 1; rcTemp.right += 1;
	CPoint copt;
	if(mousedown == 1){
		switch(cur_tool){
		case 0: break;
		case 1:{			//fill
			coY = rcTemp.top - ScrollOffset.y; coX = rcTemp.left - ScrollOffset.x;
			copt.y = rcTemp.bottom - ScrollOffset.y; copt.x = rcTemp.right - ScrollOffset.x;
			tile = coX;
			for(; coY < copt.y; coY++){
			for(coX = tile; coX < copt.x; coX++){
				pt.x = rcMap.left + (coX << 4); pt.y = rcMap.top + (coY << 4);
				Proj2->GetStandardTiles(cur_map,room).Draw(&dc,cur_tile,pt,ILD_NORMAL);}}
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
	if(Proj2->session.showLastClick){
		pt.x = ((ptLastClick.x - ScrollOffset.x) << 4) + rcMap.left;
		pt.y = ((ptLastClick.y - ScrollOffset.y) << 4) + rcMap.top;
		if(PtInRect(rcMap,pt)){
			dc.MoveTo(pt); pt.x += 15;
			dc.LineTo(pt); pt.y += 15;
			dc.LineTo(pt); pt.x -= 15;
			dc.LineTo(pt); pt.y -= 15;
			dc.LineTo(pt);}
	}

	//Draw the sprites
	CRect rc(0,0,16,16);
	for(coX = 0; coX < MAPSPRITE_COUNT; coX++){
		if(!Sprite_Value[coX]) continue;
		if(room != Sprite_InRoom[coX]) continue;
		pt.x = Sprite_Coords[coX].x - ScrollOffset.x;
		pt.y = Sprite_Coords[coX].y - ScrollOffset.y;
		if(!PtInRect(rc,pt)) continue;
		pt.x = (pt.x << 4) + rcMap.left;
		pt.y = (pt.y << 4) + rcMap.top;
		m_sprites.Draw(&dc,Proj2->ROM[MAPSPRITE_PICASSIGNMENT + Sprite_Value[coX]],pt,ILD_TRANSPARENT);
	}

	//Draw the palettes
	CBrush br;
	rc.top = rcPalettes.top; rc.bottom = rc.top + 16;
	for(coY = 0; coY < 2; coY++,rc.top += 16, rc.bottom += 16){
		rc.left = rcPalettes.left; rc.right = rc.left + 16;
		for(coX = 0; coX < 16; coX++, rc.left += 16, rc.right += 16){
			br.CreateSolidBrush(Proj2->palette[0][MapPalette[coY][0][coX]]);
			dc.FillRect(rc,&br);
			br.DeleteObject();}}
	rc = rcPalettes2; rc.right = rc.left + 16; rc.bottom = rc.top + 16;
	for(coX = 0; coX < 8; coX++, rc.left += 16, rc.right += 16){
		if(!(coX & 3)) continue;
		br.CreateSolidBrush(Proj2->palette[0][SpritePalette[0][coX]]);
		dc.FillRect(rc,&br);
		br.DeleteObject();}
	rc = rcPalettes2; rc.right = rc.left + 16; rc.bottom = rc.top + 16;
	rc.top += 16; rc.bottom += 16;
	for(coX = 0; coX < 8; coX++, rc.left += 16, rc.right += 16){
		br.CreateSolidBrush(Proj2->palette[0][ControlPalette[coX]]);
		dc.FillRect(rc,&br);
		br.DeleteObject();}

	//Draw the tools
	pt.x = rcTools.left; pt.y = rcTools.top;
	m_tools.Draw(&dc,0 + ((cur_tool == 0) * 4),pt,ILD_NORMAL); pt.x += 25;
	m_tools.Draw(&dc,1 + ((cur_tool == 1) * 4),pt,ILD_NORMAL); pt.x += 25;
	m_tools.Draw(&dc,2 + ((cur_tool == 5) * 4),pt,ILD_NORMAL); pt.x += 25;
	m_tools.Draw(&dc,3 + ((cur_tool == 6) * 4),pt,ILD_NORMAL);

	dc.SelectObject(origpen);

	m_banner.Render(dc, 8, 8);
}

void CMapsOrig::OnSelchangeMaplist() 
{
	if (cur_map != -1) {
		try {
			StoreValues();
		} catch(...) {
			if (AfxMessageBox("WARNING!!!\nNot enough KAB to compress map.\n"
				"If you continue, changes you made to this map will NOT be saved.\nAre you sure you want to continue?",
				MB_ICONQUESTION | MB_YESNO) == IDNO) {
				m_maplist.SetCurSel(cur_map);
				return;
			}
		}
	}

	cur_map = m_maplist.GetCurSel();

	LoadValues();

	InvalidateRect(rcTiles,0);
	InvalidateRect(rcMap,0);
	InvalidateRect(rcPalettes,0);
	InvalidateRect(rcPalettes2,0);
}

void CMapsOrig::OnShowrooms() 
{InvalidateRect(rcTiles,0); InvalidateRect(rcMap,0);}

void CMapsOrig::OnLButtonDown(UINT nFlags, CPoint pt)
{
	mousedown = 0;
	int tile;
	if(PtInRect(rcMap,pt)){
		if(coords_dlg.m_mouseclick.GetCheck()){
			OnRButtonDown(nFlags,pt);
			return;}
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
		pt.x = (pt.x - rcTiles.left) >> 4;
		pt.y = (pt.y - rcTiles.top) & 0xF0;
		StoreTileData();
		cur_tile = pt.x + pt.y;
		LoadTileData();
		InvalidateRect(rcTiles,0);}
	else if(PtInRect(rcPalettes,pt)){
		pt.x = (pt.x - rcPalettes.left) >> 4;
		pt.y = (pt.y - rcPalettes.top) & 0xF0;
		tile = pt.x + (pt.y << 1);
		if(!(tile & 0x03)) tile = 0;

		CNESPalette dlg;
		dlg.Proj2 = Proj2;
		int temp = MAPPALETTE_OFFSET + (cur_map * 0x30) + tile;
		if(!tile) temp += 0x18;
		dlg.color = &Proj2->ROM[temp];
		if(dlg.DoModal() == IDOK){
			MapPalette[pt.y >= 0x10][0][pt.x] = *dlg.color;
			if (!tile) {
				tile = *dlg.color;
				ControlPalette[0] = (BYTE)tile;
				for (int co = 0; co <= 28; co += 4)
					MapPalette[0][0][co] = (BYTE)tile;
			}
			UpdatePics();
			InvalidateRect(rcPalettes,0);
			InvalidateRect(rcPalettes2,0);
		}
	}
	else if(PtInRect(rcPalettes2,pt)){
		pt.x = (pt.x - rcPalettes2.left) >> 4;
		pt.y = ((pt.y - rcPalettes2.top) & 0xF0) >> 1;
		tile = pt.x + pt.y;
		if(tile == 0 || tile == 4) return;
		if(tile >= 8) tile -= 8;
		else tile += 8;

		CNESPalette dlg;
		dlg.Proj2 = Proj2;
		dlg.color = &Proj2->ROM[MAPPALETTE_OFFSET + (cur_map * 0x30) + tile + 0x10];
		if(dlg.DoModal() == IDOK){
			if(tile < 8) ControlPalette[tile] = *dlg.color;
			else SpritePalette[0][tile - 8] = *dlg.color;
			if (!tile) {
				tile = *dlg.color;
				ControlPalette[0] = (BYTE)tile;
				for (int co = 0; co <= 28; co += 4)
					MapPalette[0][0][co] = (BYTE)tile;
			}
			UpdatePics();
			InvalidateRect(rcPalettes,0);
			InvalidateRect(rcPalettes2,0);
		}
	}
	else if(PtInRect(rcTools,pt)){
		cur_tool = (pt.x - rcTools.left) / 25;
		if(cur_tool > 1) cur_tool += 3;
		InvalidateRect(rcTools,0);
		m_customtool.EnableWindow(cur_tool > 1);}
	else {
		HandleLbuttonDrag(this);
	}
}

void CMapsOrig::OnMouseMove(UINT nFlags, CPoint pt)
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
				Sprite_Coords[mousedown - 2] = ptHover;
				text.Format("%X",ptHover.x); m_spritecoordx.SetWindowText(text);
				text.Format("%X",ptHover.y); m_spritecoordy.SetWindowText(text);
				InvalidateRect(rcMap,0);}
		}
	}
	else{
		if(ptHover.x != -1){
			m_hovering.SetWindowText(text);
			ptHover.x = -1;}
	}
}

void CMapsOrig::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
	if(ScrollOffset.x > 48) ScrollOffset.x = 48;

	m_hscroll.SetScrollPos(ScrollOffset.x);
	InvalidateRect(rcMap,FALSE);
}

void CMapsOrig::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
	if(ScrollOffset.y > 48) ScrollOffset.y = 48;

	m_vscroll.SetScrollPos(ScrollOffset.y);
	InvalidateRect(rcMap,FALSE);
}

void CMapsOrig::OnRButtonDblClk(UINT nFlags, CPoint pt)
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
	if(ref != -1){
		int old = Proj2->session.tintTiles[cur_tileset + 1][ref];
		CTint dlg;
		dlg.tintvalue = old;
		dlg.m_tintvariant = Proj2->session.tintVariant;
		if(dlg.DoModal() == IDOK){
			
			Proj2->OK_tiles[cur_map] = 0;
			Proj2->GetStandardTiles(cur_map,0).DeleteImageList();
			Proj2->GetStandardTiles(cur_map,1).DeleteImageList();

			Proj2->session.tintTiles[cur_tileset + 1][ref] = (BYTE)dlg.tintvalue;
			if(Proj2->session.tintVariant != dlg.m_tintvariant){
				Proj2->session.tintVariant = (BYTE)dlg.m_tintvariant;
				Proj2->ReTintPalette();}

			CLoading dlgmaps;
			dlgmaps.Create(IDD_LOADING,this);
			dlgmaps.m_progress.SetRange(0,128);
			dlgmaps.m_progress.SetPos(0);
			dlgmaps.ShowWindow(1);

			ReloadImages(&dlgmaps.m_progress);

			dlgmaps.ShowWindow(0);
			InvalidateRect(rcTiles,0);
			InvalidateRect(rcMap,0);}
	}
}

void CMapsOrig::OnRButtonDown(UINT nFlags, CPoint pt)
{
	mousedown = 0;
	if(PtInRect(rcTiles,pt)) OnLButtonDown(nFlags,pt);
	else if(PtInRect(rcMap,pt)){
		StoreTileData();
		pt.x = ((pt.x - rcMap.left) >> 4) + ScrollOffset.x;
		pt.y = ((pt.y - rcMap.top) >> 4) + ScrollOffset.y;
		UpdateClick(pt);
		InvalidateRect(rcTiles,0);
		if(m_showlastclick.GetCheck()) InvalidateRect(rcMap,0);
		if(coords_dlg.m_mouseclick.GetCheck()){
			coords_dlg.m_coord_l.SetCurSel(m_maplist.GetCurSel());
			coords_dlg.OnSelchangeCoordL();
			coords_dlg.InputCoords(pt);
			return;}
		cur_tile = DecompressedMap[pt.y][pt.x];
		LoadTileData();
		if(cur_tool > 1){
			cur_tool = 1;
			m_customtool.EnableWindow(0);
			InvalidateRect(rcTools,0);}

		//if they clicked on a sprite... adjust the Sprite Editor accordingly
		for(int co = 0; co < MAPSPRITE_COUNT; co++){
			if(!Sprite_Value[co]) continue;
			if(Sprite_Coords[co] == pt){
				mousedown = (BYTE)(co + 2);
				m_sprite_list.SetCurSel(co);
				OnSelchangeSpriteList();
				break;}}
	}
}

void CMapsOrig::UpdatePics()
{
	CLoading dlg; dlg.Create(IDD_LOADING,this);
	dlg.m_progress.SetRange(0,286);
	dlg.m_progress.SetPos(0);
	dlg.ShowWindow(1);
	
	Proj2->OK_tiles[cur_map] = 0;
	Proj2->GetStandardTiles(cur_map,0).DeleteImageList();
	Proj2->GetStandardTiles(cur_map,1).DeleteImageList();

	ReloadImages(&dlg.m_progress);
	ReloadSprites(&dlg.m_progress);

	dlg.ShowWindow(0);
	InvalidateRect(rcTiles,0);
	InvalidateRect(rcMap,0);
}

void CMapsOrig::UpdateTileData()
{
	if(cur_tiledata & 0x77) cur_tiledata |= 0x01;

	m_move.SetCheck(cur_tiledata & 0x01);
	m_fight.SetCheck(cur_tiledata & 0x02);
	m_shop.SetCheck(cur_tiledata & 0x04);
	m_tc.SetCheck(cur_tiledata & 0x08);
	m_special.SetCheck(cur_tiledata & 0x10);
	m_warp.SetCheck(cur_tiledata & 0x20);
	m_teleport.SetCheck(cur_tiledata & 0x40);

	m_move.EnableWindow(!(cur_tiledata & 0x76));
	m_fight.EnableWindow(!(cur_tiledata & 0x7C));
	m_fight_list.ShowWindow((cur_tiledata & 0x02) != 0);
	m_shop.EnableWindow(!(cur_tiledata & 0x7A));
	m_shop_list.ShowWindow((cur_tiledata & 0x04) != 0);
	m_tc.EnableWindow(!(cur_tiledata & 0x76));
	m_tc_list.ShowWindow((cur_tiledata & 0x08) != 0);
	m_tcitem_list.ShowWindow((cur_tiledata & 0x08) != 0);
	m_tcitem_price.ShowWindow((cur_tiledata & 0x08) != 0);
	m_special.EnableWindow(!(cur_tiledata & 0x0E));
	m_special_list.ShowWindow((cur_tiledata & 0x10) != 0);
	m_warp.EnableWindow(!(cur_tiledata & 0x4E));
	m_teleport.EnableWindow(!(cur_tiledata & 0x2E));
	m_teleport_list.ShowWindow((cur_tiledata & 0x40) != 0);

	//TODO - clean up these old-style uses of bitwise and logical operators together.
	//	They produce warnings with the newer generations of compilers.
	m_text_list.ShowWindow(!(cur_tiledata & 0x6E));
	m_editlabel.ShowWindow((cur_tiledata & 0x08) != 0);
}

void CMapsOrig::OnMove() 
{cur_tiledata ^= 0x01;
UpdateTileData();}
void CMapsOrig::OnFight() 
{cur_tiledata ^= 0x02;
UpdateTileData();}
void CMapsOrig::OnShop() 
{cur_tiledata ^= 0x04;
UpdateTileData();}
void CMapsOrig::OnTc() 
{cur_tiledata ^= 0x08;
UpdateTileData();}
void CMapsOrig::OnSpecial() 
{cur_tiledata ^= 0x10;
UpdateTileData();}
void CMapsOrig::OnWarp() 
{cur_tiledata ^= 0x20;
UpdateTileData();}
void CMapsOrig::OnTeleport() 
{cur_tiledata ^= 0x40;
UpdateTileData();}

void CMapsOrig::OnSelchangeTcList() 
{
	if(cur_tc != -1) StoreTC();
	cur_tc = m_tc_list.GetCurSel();
	LoadTC();
}

void CMapsOrig::OnSelchangeTcitemList() 
{
	if(cur_tcitem != -1) StoreTCItem();
	cur_tcitem = m_tcitem_list.GetCurSel();
	LoadTCItem();
}

void CMapsOrig::StoreTC()
{
	StoreTCItem();
	Proj2->ROM[TREASURE_OFFSET + cur_tc] = (BYTE)(cur_tcitem + 1);
}

void CMapsOrig::LoadTC()
{
	cur_tcitem = Proj2->ROM[TREASURE_OFFSET + cur_tc] - 1;
	m_tcitem_list.SetCurSel(cur_tcitem);
	LoadTCItem();
}

void CMapsOrig::StoreTCItem()
{
	int offset = ITEMPRICE_OFFSET + (cur_tcitem << 1);
	int temp;
	CString text; m_tcitem_price.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFFFF) temp = 0xFFFF;
	Proj2->ROM[offset] = temp & 0xFF;
	Proj2->ROM[offset + 1] = (BYTE)(temp >> 8);
}

void CMapsOrig::LoadTCItem()
{
	int offset = ITEMPRICE_OFFSET + (cur_tcitem << 1);
	CString text; text.Format("%d",Proj2->ROM[offset] + (Proj2->ROM[offset + 1] << 8));
	m_tcitem_price.SetWindowText(text);
}

void CMapsOrig::OnSelchangeTileset()
{
	StoreTileData();
	cur_tileset = m_tileset.GetCurSel();
	CLoading dlg; dlg.Create(IDD_LOADING,this);
	dlg.m_progress.SetRange(0,286);
	dlg.m_progress.SetPos(0);
	dlg.ShowWindow(1);
	
	Proj2->OK_tiles[cur_map] = 0;
	Proj2->GetStandardTiles(cur_map,0).DeleteImageList();
	Proj2->GetStandardTiles(cur_map,1).DeleteImageList();

	ReloadImages(&dlg.m_progress);
	ReloadSprites(&dlg.m_progress);

	dlg.ShowWindow(0);
	InvalidateRect(rcTiles,0);
	InvalidateRect(rcMap,0);
	LoadTileData();
}

void CMapsOrig::OnSelchangeSpriteList()
{
	int temp = m_sprite_list.GetCurSel();
	m_inroom.SetCheck(Sprite_InRoom[temp]);
	m_still.SetCheck(Sprite_StandStill[temp]);
	CString text; text.Format("%X",Sprite_Coords[temp].x);
	m_spritecoordx.SetWindowText(text);
	text.Format("%X",Sprite_Coords[temp].y);
	m_spritecoordy.SetWindowText(text);
	m_sprite.SetCurSel(Sprite_Value[temp]);
	m_spritegraphic.SetCurSel(Proj2->ROM[MAPSPRITE_PICASSIGNMENT + Sprite_Value[temp]]);
}

void CMapsOrig::OnEditspritegfx()
{
	OnSave();
	CMapman dlg;
	dlg.Proj2 = Proj2;
	dlg.graphicoffset = MAPSPRITEPATTERNTABLE_OFFSET + (m_spritegraphic.GetCurSel() << 8);
	dlg.paletteoffset = MAPPALETTE_OFFSET + (cur_map * 0x30) + 0x18;
	dlg.DoModal();
}

void CMapsOrig::OnStill() 
{Sprite_StandStill[m_sprite_list.GetCurSel()] = m_still.GetCheck() != 0;}

void CMapsOrig::OnInroom() 
{
	Sprite_InRoom[m_sprite_list.GetCurSel()] = m_inroom.GetCheck() != 0;
	InvalidateRect(rcMap,0);
}

void CMapsOrig::OnChangeSpritecoordx() 
{
	CString text; int number;
	m_spritecoordx.GetWindowText(text); number = StringToInt_HEX(text);
	if(number > 0x3F) number = 0x3F;
	Sprite_Coords[m_sprite_list.GetCurSel()].x = number;
	InvalidateRect(rcMap,0);
}

void CMapsOrig::OnChangeSpritecoordy() 
{
	CString text; int number;
	m_spritecoordy.GetWindowText(text); number = StringToInt_HEX(text);
	if(number > 0x3F) number = 0x3F;
	Sprite_Coords[m_sprite_list.GetCurSel()].y = number;
	InvalidateRect(rcMap,0);
}

void CMapsOrig::OnSelchangeSprite() 
{
	Sprite_Value[m_sprite_list.GetCurSel()] = (short)m_sprite.GetCurSel();
	m_spritegraphic.SetCurSel(Proj2->ROM[MAPSPRITE_PICASSIGNMENT + m_sprite.GetCurSel()]);
	InvalidateRect(rcMap,0);
}

void CMapsOrig::OnSelchangeSpritegraphic()
{
	Proj2->ROM[MAPSPRITE_PICASSIGNMENT + Sprite_Value[m_sprite_list.GetCurSel()]] = (BYTE)m_spritegraphic.GetCurSel();
	InvalidateRect(rcMap, 0);
}

void CMapsOrig::UpdateClick(CPoint pt)
{
	ptLastClick = pt;
	CString text; text.Format("%X,%X",pt.x,pt.y);
	m_lastclick.SetWindowText(text);
}

void CMapsOrig::OnRButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);

	mousedown = 0;
}

void CMapsOrig::OnLButtonUp(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(pt);

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
			InvalidateRect(rcMap,0);
			   }break;
		default:{			//smarttools
			temp = cur_tool - 2 + (cur_tileset << 1);
			//flood fill
			for(coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++){
			for(coX = rcToolRect.left; coX <= rcToolRect.right; coX++)
				DecompressedMap[coY][coX] = Proj2->session.smartTools[temp][4];}
			//"smart" top edge
			coY = rcToolRect.top;
			for(coX = rcToolRect.left; coX <= rcToolRect.right; coX++){
				draw = 1;
				if(coY){ for(co = 0; co < 6 && draw; co++){
					if(DecompressedMap[coY - 1][coX] == Proj2->session.smartTools[temp][co]) draw = 0;}}
				if(draw) DecompressedMap[coY][coX] = Proj2->session.smartTools[temp][1];}
			//"smart" bottom edge
			coY = rcToolRect.bottom;
			for(coX = rcToolRect.left; coX <= rcToolRect.right; coX++){
				draw = 1;
				if(coY < 255){ for(co = 3; co < 9 && draw; co++){
					if(DecompressedMap[coY + 1][coX] == Proj2->session.smartTools[temp][co]) draw = 0;}}
				if(draw) DecompressedMap[coY][coX] = Proj2->session.smartTools[temp][7];}
			//"smart" left edge
			coX = rcToolRect.left;
			for(coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++){
				draw = 1;
				if(coX){ for(co = 0; co < 8 && draw; co++){
					if(co % 3 == 2) co++;
					if(DecompressedMap[coY][coX - 1] == Proj2->session.smartTools[temp][co]) draw = 0;}}
				if(draw) DecompressedMap[coY][coX] = Proj2->session.smartTools[temp][3];}
			//"smart" right edge
			coX = rcToolRect.right;
			for(coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++){
				draw = 1;
				if(coX < 255){ for(co = 1; co < 9 && draw; co++){
					if(co % 3 == 0) co++;
					if(DecompressedMap[coY][coX + 1] == Proj2->session.smartTools[temp][co]) draw = 0;}}
				if(draw) DecompressedMap[coY][coX] = Proj2->session.smartTools[temp][5];}
			//"smart" NW corner
			co = 0;
			draw = 1;
			if(rcToolRect.left){
				coY = DecompressedMap[rcToolRect.top][rcToolRect.left - 1];
				for(coX = 0; draw && coX < 8; coX++){
					if(coX % 3 == 2) coX++;
					if(coY == Proj2->session.smartTools[temp][coX]) draw = 0;}}
			if(!draw) co = 1;
			draw = 1;
			if(rcToolRect.top){
				coY = DecompressedMap[rcToolRect.top - 1][rcToolRect.left];
				for(coX = 0; draw && coX < 6; coX++){
					if(coY == Proj2->session.smartTools[temp][coX]) draw = 0;}}
			if(!draw){
				if(co == 1) co = 4;
				else co = 3;}
			DecompressedMap[rcToolRect.top][rcToolRect.left] = Proj2->session.smartTools[temp][co];
			//"smart" SW corner
			co = 6;
			draw = 1;
			if(rcToolRect.left){
				coY = DecompressedMap[rcToolRect.bottom][rcToolRect.left - 1];
				for(coX = 0; draw && coX < 8; coX++){
					if(coX % 3 == 2) coX++;
					if(coY == Proj2->session.smartTools[temp][coX]) draw = 0;}}
			if(!draw) co = 7;
			draw = 1;
			if(rcToolRect.bottom < 255){
				coY = DecompressedMap[rcToolRect.bottom + 1][rcToolRect.left];
				for(coX = 3; draw && coX < 9; coX++){
					if(coY == Proj2->session.smartTools[temp][coX]) draw = 0;}}
			if(!draw){
				if(co == 7) co = 4;
				else co = 3;}
			DecompressedMap[rcToolRect.bottom][rcToolRect.left] = Proj2->session.smartTools[temp][co];
			//"smart" NE corner
			co = 2;
			draw = 1;
			if(rcToolRect.right < 255){
				coY = DecompressedMap[rcToolRect.top][rcToolRect.right + 1];
				for(coX = 1; draw && coX < 9; coX++){
					if(coX % 3 == 0) coX++;
					if(coY == Proj2->session.smartTools[temp][coX]) draw = 0;}}
			if(!draw) co = 1;
			draw = 1;
			if(rcToolRect.top){
				coY = DecompressedMap[rcToolRect.top - 1][rcToolRect.right];
				for(coX = 0; draw && coX < 6; coX++){
					if(coY == Proj2->session.smartTools[temp][coX]) draw = 0;}}
			if(!draw){
				if(co == 1) co = 4;
				else co = 5;}
			DecompressedMap[rcToolRect.top][rcToolRect.right] = Proj2->session.smartTools[temp][co];
			//"smart" SE corner
			co = 8;
			draw = 1;
			if(rcToolRect.right < 255){
				coY = DecompressedMap[rcToolRect.bottom][rcToolRect.right + 1];
				for(coX = 1; draw && coX < 9; coX++){
					if(coX % 3 == 0) coX++;
					if(coY == Proj2->session.smartTools[temp][coX]) draw = 0;}}
			if(!draw) co = 7;
			draw = 1;
			if(rcToolRect.bottom < 255){
				coY = DecompressedMap[rcToolRect.bottom + 1][rcToolRect.right];
				for(coX = 3; draw && coX < 9; coX++){
					if(coY == Proj2->session.smartTools[temp][coX]) draw = 0;}}
			if(!draw){
				if(co == 7) co = 4;
				else co = 5;}
			DecompressedMap[rcToolRect.bottom][rcToolRect.right] = Proj2->session.smartTools[temp][co];

			InvalidateRect(rcMap,0);
				}break;

		}
	}
	mousedown = 0;
}
void CMapsOrig::OnShowlastclick() 
{Proj2->session.showLastClick = (m_showlastclick.GetCheck() != 0); InvalidateRect(rcMap,0);}

void CMapsOrig::OnCustomtool()
{
	CCustomTool dlg;
	dlg.Proj2 = Proj2;
	dlg.m_tiles = &Proj2->GetStandardTiles(cur_map,m_showrooms.GetCheck());
	dlg.tool = cur_tool - 2 + (cur_tileset << 1);
	dlg.DoModal();
}

void CMapsOrig::OnFindkab() 
{
	kab = MAP_END - MAP_START;
	int co;
	for(co = 0; co < MAP_COUNT; co++)
		kab -= GetByteCount(co,(co != cur_map));

	CString text;
	text.Format("%d",kab); m_kab.SetWindowText(text);
}

int CMapsOrig::GetByteCount(int map,bool compressed)
{
	int count = 0;
	int co, temp;
	if(compressed)
	{
		temp = MAP_OFFSET + (map << 1);
		co = MAP_PTRADD + Proj2->ROM[temp] + (Proj2->ROM[temp + 1] << 8);
		for(;Proj2->ROM[co] != 0xFF; co++, count++);
	}
	else
	{
		int RunLength, ThisRun;
		for(co = 0; co < 0x1000;){
			ThisRun = DecompressedMap[0][co]; RunLength = 0;

			if(ThisRun != 0x7F){
				while(DecompressedMap[0][co] == ThisRun && RunLength < 256){
					co++; RunLength++;}}
			else{ co++; RunLength = 1;}

			if(RunLength == 1) count += 1;
			else if(RunLength == 255) count += 3;
			else count += 2;}
	}
	return count + 1;
}

void CMapsOrig::CompressMap()
{
	/************************************************
	WARNING!!!!

	Before calling this function MAKE SURE kab >= 0
	If there is not enough KAB, map data will be lost
	************************************************/

	//find the shift in sizes
	int shift = GetByteCount(cur_map,0) - GetByteCount(cur_map,1);

	//shift maps accordingly
	int mapptr = MAP_OFFSET + (cur_map << 1);
	int mapptrend = MAP_OFFSET + (MAP_COUNT << 1);
	int mapstart = Proj2->ROM[mapptr] + (Proj2->ROM[mapptr + 1] << 8) + MAP_PTRADD;
	int co, temp;
	if(shift > 0){
		for(co = MAP_END - 1; co >= mapstart; co--)
			Proj2->ROM[co] = Proj2->ROM[co - shift];}
	if(shift < 0){
		for(co = mapstart; co < MAP_END; co++)
			Proj2->ROM[co] = Proj2->ROM[co - shift];}

	//shift pointers
	if(shift != 0){
		for(mapptr += 2; mapptr < mapptrend; mapptr += 2){
			temp = Proj2->ROM[mapptr] + (Proj2->ROM[mapptr + 1] << 8);
			temp += shift;
			Proj2->ROM[mapptr] = temp & 0xFF;
			Proj2->ROM[mapptr + 1] = (BYTE)(temp >> 8);}
	}

	//compress and insert the new map
	int ThisRun, RunLength;
	for(co = 0; co < 0x1000;){
		ThisRun = DecompressedMap[0][co]; RunLength = 0;

		if(ThisRun != 0x7F){
			while(DecompressedMap[0][co] == ThisRun && RunLength < 256){
				co++; RunLength++;}}
		else{ co++; RunLength = 1;}

		if(RunLength == 1){
			Proj2->ROM[mapstart] = (BYTE)ThisRun;
			mapstart += 1;}
		else if(RunLength == 255){
			Proj2->ROM[mapstart] = (BYTE)ThisRun;
			Proj2->ROM[mapstart + 1] = (BYTE)(ThisRun | 0x80);
			Proj2->ROM[mapstart + 2] = 254;
			mapstart += 3;}
		else{
			Proj2->ROM[mapstart] = (BYTE)(ThisRun | 0x80);
			Proj2->ROM[mapstart + 1] = (BYTE)RunLength;
			if(RunLength == 256) Proj2->ROM[mapstart + 1] = 0;
			mapstart += 2;}
	}
	Proj2->ROM[mapstart] = 0xFF;
}

void CMapsOrig::OnEditSpriteLabel() 
{
	int temp = m_sprite.GetCurSel();
	//ChangeLabel(*Proj2, -1, LoadSpriteLabel(*Proj2, temp), WriteSpriteLabel, temp, nullptr, &m_sprite); //TODO - ChangeLabel
}

void CMapsOrig::OnEditgfxlabel() 
{
	int temp = m_spritegraphic.GetCurSel();
	//ChangeLabel(*Proj2, -1, LoadSpriteGraphicLabel(*Proj2, temp), WriteSpriteGraphicLabel, temp, nullptr, &m_spritegraphic);
}

void CMapsOrig::OnMaplabel()
{
	int themap = m_maplist.GetCurSel();
	//ChangeLabel(*Proj2, -1, LoadMapLabel(*Proj2, themap), WriteMapLabel, themap, &m_maplist, nullptr);

	int temp = coords_dlg.m_coord_l.GetCurSel();
	coords_dlg.m_coord_l.DeleteString(themap);
	coords_dlg.m_coord_l.InsertString(themap, Labels2::LoadMapLabel(*Proj2, themap).name);
	coords_dlg.m_coord_l.SetCurSel(temp);
}

void CMapsOrig::OnEditlabel() 
{
	int temp = m_tc_list.GetCurSel();
	//ChangeLabel(*Proj2, -1, LoadTreasureLabel(*Proj2, temp), WriteTreasureLabel, temp, nullptr, &m_tc_list);
}

void CMapsOrig::OnTilesetlabel() 
{
	int temp = m_tileset.GetCurSel();
	//ChangeLabel(*Proj2, -1, LoadTilesetLabel(*Proj2, temp), WriteTilesetLabel, temp, nullptr, &m_tileset);
}

void CMapsOrig::OnLButtonDblClk(UINT nFlags, CPoint pt) 
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(pt);

	if(PtInRect(rcTiles,pt)){
		CTileEdit dlg;
		dlg.Invoker = CTileEdit::Maps;
		dlg.Proj2 = Proj2;
		dlg.tileset = (BYTE)(cur_tileset + 1);
		dlg.tile = (BYTE)cur_tile;
		dlg.pal[0] = MapPalette[0][0];
		dlg.pal[1] = MapPalette[1][0];
		OnSave();
		if(dlg.DoModal() == IDOK){
			Proj2->OK_tiles[cur_map] = 0;
			Proj2->GetStandardTiles(cur_map,0).DeleteImageList();
			Proj2->GetStandardTiles(cur_map,1).DeleteImageList();
			CLoading load; load.Create(IDD_LOADING,this);
			load.m_progress.SetRange(0,124);
			load.m_progress.SetPos(0);
			load.ShowWindow(1);
			ReloadImages(&load.m_progress);
			load.ShowWindow(0);

			InvalidateRect(rcPalettes,0);
			InvalidateRect(rcTiles,0);
			InvalidateRect(rcMap,0);}
	}
}

void CMapsOrig::OnMapExport() 
{
	CString text = Labels2::LoadMapLabel(*Proj2, cur_map).name + " Map." + CString(FFH_MAP_EXT);
	CString filename = Paths::Combine({ FOLDERPREF(Proj2->AppSettings, PrefMapImportExportFolder) , text });
	auto result = Ui::SaveFilePromptExt(this, FFH_MAP_FILTER, FFH_MAP_EXT, "Export Standard Map", filename);
	if (!result) return;

	FILE* file = fopen(result.value, "w+b");
	if(file == nullptr){
		AfxMessageBox("Error saving map", MB_ICONERROR);
		return;}
	fwrite(DecompressedMap,1,0x1000,file);
	fclose(file);
}

void CMapsOrig::OnMapImport() 
{
	auto result = OpenFilePromptExt(this, FFH_MAP_FILTER, FFH_MAP_EXT, "Import Standard Map",
		FOLDERPREF(Proj2->AppSettings, PrefMapImportExportFolder));
	if (!result) return;

	FILE* file = fopen(result.value, "r+b");
	if(file == nullptr){
		AfxMessageBox("Error loading map", MB_ICONERROR);
		return;}
	fread(DecompressedMap,1,0x1000,file);
	fclose(file);
	InvalidateRect(rcMap,0);
}

void CMapsOrig::OnViewcoords()
{
	coords_dlg.ShowWindow(m_viewcoords.GetCheck());
}

void CMapsOrig::UpdateTeleportLabel(int arid, bool NNTele)
{
	int temp = m_teleport_list.GetCurSel();
	if (NNTele) {
		m_teleport_list.DeleteString(arid);
		m_teleport_list.InsertString(arid, Labels2::LoadNNTeleportLabel(*Proj2, arid).name);
	}
	else {
		m_teleport_list.DeleteString(arid + NNTELEPORT_COUNT);
		m_teleport_list.InsertString(arid + NNTELEPORT_COUNT, Labels2::LoadNOTeleportLabel(*Proj2, arid).name);
	}
	m_teleport_list.SetCurSel(temp);
}

void CMapsOrig::DoHScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMapsOrig::DoVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	OnVScroll(nSBCode, nPos, pScrollBar);
}

void CMapsOrig::DoViewcoords()
{
	OnViewcoords();
}

void CMapsOrig::DoOK()
{
	OnOK();
}

void CMapsOrig::DoSelchangeMaplist()
{
	OnSelchangeMaplist();
}

void CMapsOrig::OnSelchangeTeleportList()
{
	if(!coords_dlg.m_mouseclick.GetCheck()){
		coords_dlg.m_teleportlist.SetCurSel(m_teleport_list.GetCurSel() + ONTELEPORT_COUNT);
		coords_dlg.OnSelchangeTeleportlist();}
}

void CMapsOrig::UpdateTeleportLabel(int areaid, int type)
{
	this->UpdateTeleportLabel(areaid, type == 1);
}

void CMapsOrig::Cancel(int context)
{
	if (context == Coords) {
		m_viewcoords.SetCheck(0);
		DoViewcoords();
	}
}

POINT CMapsOrig::GetLastClick()
{
	return ptLastClick;
}

int CMapsOrig::GetCurMap()
{
	return cur_map;
}

void CMapsOrig::TeleportHere(int mapindex, int x, int y)
{
	if (mapindex == 0xFF) {
		BootToTeleportFollowup = 1;
		DoOK();
	}
	else {
		ptLastClick = CPoint(x, y);
		DoHScroll(5, x - 8, nullptr);
		DoVScroll(5, y - 8, nullptr);
		m_maplist.SetCurSel(mapindex);
		BootToTeleportFollowup = 1;
		DoSelchangeMaplist();
	}
}