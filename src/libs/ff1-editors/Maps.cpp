// Maps.cpp : implementation file
// New implementation

#include "stdafx.h"
#include "resource_editors.h"
#include "Maps.h"
#include <AppSettings.h>
#include <collection_helpers.h>
#include <core_exceptions.h>
#include <editor_label_functions.h>
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
#include <FFH2Project.h>
#include <GameSerializer.h>

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
#include <ValueDataAccessor.h>
#include <dva_primitives.h>

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
// CMaps dialog

CMaps::CMaps(CWnd* pParent /*= nullptr */)
	: CEditorWithBackground(CMaps::IDD, pParent)
{
}

void CMaps::DoDataExchange(CDataExchange* pDX)
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
	DDX_Control(pDX, IDC_BTN_DRAWTOOLS_PEN, m_penbutton);
	DDX_Control(pDX, IDC_BTN_DRAWTOOLS_BLOCK, m_blockbutton);
	DDX_Control(pDX, IDC_BTN_DRAWTOOLS_CUSTOM1, m_custom1button);
	DDX_Control(pDX, IDC_BTN_DRAWTOOLS_CUSTOM2, m_custom2button);
	DDX_Control(pDX, IDC_STATIC_INFRA_MAPPANEL, m_mappanel);
	DDX_Control(pDX, IDC_BUTTON_POPOUT, m_popoutbutton);
}


BEGIN_MESSAGE_MAP(CMaps, CEditorWithBackground)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_LBN_SELCHANGE(IDC_MAPLIST, OnSelchangeMaplist)
	ON_BN_CLICKED(IDC_SHOWROOMS, OnShowrooms)
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
	ON_BN_CLICKED(IDC_SHOWLASTCLICK, OnShowlastclick)
	ON_BN_CLICKED(IDC_CUSTOMTOOL, OnCustomtool)
	ON_BN_CLICKED(IDC_FINDKAB, OnFindkab)
	ON_BN_CLICKED(IDC_EDIT_SPRITE_LABEL, OnEditSpriteLabel)
	ON_BN_CLICKED(IDC_EDITGFXLABEL, OnEditgfxlabel)
	ON_BN_CLICKED(IDC_MAPLABEL, OnMaplabel)
	ON_BN_CLICKED(IDC_EDITLABEL, OnEditlabel)
	ON_BN_CLICKED(IDC_TILESETLABEL, OnTilesetlabel)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_VIEWCOORDS, OnViewcoords)
	ON_CBN_SELCHANGE(IDC_TELEPORT_LIST, OnSelchangeTeleportList)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_MAP, OnMapImport)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_MAP, OnMapExport)
	ON_BN_CLICKED(IDC_BUTTON_POPOUT, &CMaps::OnBnClickedButtonPopout)
	ON_MESSAGE(WMA_SHOWFLOATINGMAP, &CMaps::OnShowFloatMap)
	ON_MESSAGE(WMA_DRAWTOOLBNCLICK, &CMaps::OnDrawToolBnClick)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMaps message handlers

void CMaps::LoadOffsets()
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

void CMaps::LoadRom()
{
	cart->ClearROM();
	if (cart->IsRom()) {
		cart->LoadROM();
	}
	else if (cart->IsAsm()) {
		GameSerializer ser(*cart);
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
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, cart->info.type);
	}
}

void CMaps::SaveRom()
{
	if (cart->IsRom()) {
		cart->SaveROM();
	}
	else if (cart->IsAsm()) {
		GameSerializer ser(*cart);
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
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, cart->info.type);
	}
}

BOOL CMaps::OnInitDialog()
{
	cart = Proj2;
	CEditorWithBackground::OnInitDialog();

	FFH_THROW_NULL_PROJECT(cart, "Standard Maps Editor");//TODO - specify editor name in ctor and let base class use it

	try {
		if (cart == nullptr) throw std::runtime_error("no project was specified for this editor");
		if (Enloader == nullptr) throw std::runtime_error("No entry loader was specified for this editor");

		this->LoadOffsets();
		this->LoadRom();

		Sprite_Coords.resize(MAPSPRITE_COUNT);
		Sprite_InRoom.resize(MAPSPRITE_COUNT);
		Sprite_StandStill.resize(MAPSPRITE_COUNT);
		Sprite_Value.resize(MAPSPRITE_COUNT);

		coords_dlg.Context = Coords;
		coords_dlg.STparent = this;
		coords_dlg.Proj2= cart;
		coords_dlg.Create(IDD_COORDS, this);
		coords_dlg.IsOV = 0;
		coords_dlg.Boot();
		m_viewcoords.SetCheck(BootToTeleportFollowup);
		OnViewcoords();

		mousedown = 0;
		IF_NOHANDLE(redpen).CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		IF_NOHANDLE(toolBrush).CreateSolidBrush(RGB(128, 64, 255));
		IF_NOHANDLE(m_sprites).Create(16, 16, ILC_COLOR16 + ILC_MASK, 16, 0);

		m_penbutton.SetCheck(BST_CHECKED);
		cur_tool = m_penbutton.GetToolIndex();

		auto maxes = calc_scroll_maximums();
		m_hscroll.SetScrollRange(0, maxes.cx);
		m_vscroll.SetScrollRange(0, maxes.cy);
		ScrollOffset.x = 0;
		ScrollOffset.y = 0;
		ScrollOffset.x = 0;
		ScrollOffset.y = 0;

		CRect rcpos;
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

		LoadCombo(m_tcitem_list, Enloader->LoadTreasureItemEntries(*cart, true));

		LoadListBox(m_maplist, Labels2::LoadMapLabels(*cart));
		for (auto wnd : { &m_battle1, &m_battle2, &m_battle3,&m_battle4,&m_battle5,&m_battle6,&m_battle7,&m_battle8 })
			LoadCombo(*wnd, Labels2::LoadBattleLabels(*cart));

		CSpriteDialogueSettings dlgstgs(*cart);
		LoadCombo(m_text_list, (dlgstgs.ShowActualText ? LoadGameTextEntries(*cart, true) : Labels2::LoadTextLabels(*cart, true)));

		LoadCombo(m_tileset, Labels2::LoadTilesetLabels(*cart));
		LoadCombo(m_fight_list, Labels2::LoadBattleLabels(*cart));
		m_fight_list.InsertString(0, "--Random Encounters--");

		LoadCombo(m_shop_list, Labels2::LoadShopLabels(*cart));
		LoadCombo(m_tc_list, Labels2::LoadTreasureLabels(*cart));
		LoadCombo(m_special_list, Labels2::LoadSpecialTileLabels(*cart));
		LoadCombo(m_teleport_list, Labels2::LoadNNTeleportLabels(*cart) + Labels2::LoadNOTeleportLabels(*cart));
		LoadCombo(m_sprite_list, Labels2::LoadOnScreenSpriteLabels(*cart));
		LoadCombo(m_spritegraphic, Labels2::LoadSpriteGraphicLabels(*cart));
		LoadCombo(m_sprite, Labels2::LoadSpriteLabels(*cart, true));

		m_showlastclick.SetCheck(cart->session.showLastClick);

		cur_tc = -1;
		cur_tcitem = -1;
		cur_map = -1;
		cur_tile = 0;
		cur_tileset = 0;

		m_sprite_list.SetCurSel(0);
		m_tileset.SetCurSel(0);
		m_maplist.SetCurSel(0);

		m_toolbuttons = {&m_penbutton, &m_blockbutton, &m_custom1button, &m_custom2button};
		init_popout_map_window();

		if (BootToTeleportFollowup) {
			m_maplist.SetCurSel(cart->TeleportFollowup[cart->curFollowup][0]);
			ptLastClick.x = cart->TeleportFollowup[cart->curFollowup][1];
			ptLastClick.y = cart->TeleportFollowup[cart->curFollowup][2];
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

BOOL CMaps::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		switch(pMsg->wParam) {
		case VK_F6:
			if (m_popoutcreated && m_popoutmap.IsWindowVisible()) {
				m_popoutmap.SetActiveWindow();
				return TRUE;
			}
			break;
		case VK_F7:
			if (m_popoutcreated) {
				bool in = m_popoutmap.IsWindowVisible() == TRUE;
				PopMapDialog(in);
				return TRUE;
			}
		}
	}
	return CEditorWithBackground::PreTranslateMessage(pMsg);
}

void CMaps::OnCancel()
{
	if (m_popoutcreated && m_popoutmap.IsWindowVisible()) {
		m_popoutmap.PostMessage(WM_KEYDOWN, VK_ESCAPE);
		return;
	}
	CEditorWithBackground::OnCancel();
}

void CMaps::ReloadSprites(CProgressCtrl* m_prog)
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
		DrawTile(&mDC,0,0,cart,offset     ,SpritePalette[0]);
		DrawTile(&mDC,8,0,cart,offset + 16,SpritePalette[0]);
		DrawTile(&mDC,0,8,cart,offset + 32,SpritePalette[1]);
		DrawTile(&mDC,8,8,cart,offset + 48,SpritePalette[1]);
		mDC.SelectObject(&dummy);
		m_sprites.Add(&bmp,Proj2->palette[0][0x40]);
		mDC.SelectObject(&bmp);
		m_prog->OffsetPos(1);
	}

	mDC.DeleteDC();
	dummy.DeleteObject();
	bmp.DeleteObject();
}

void CMaps::ReloadImages(CProgressCtrl* m_prog)
{
	if(!cart->OK_tiles[cur_map]){
		cart->GetStandardTiles(cur_map,false).Create(16, 16, ILC_COLOR16, 64, 0);
		cart->GetStandardTiles(cur_map,true).Create(16, 16, ILC_COLOR16, 64, 0);
		CPaintDC dc(this);
		CDC mDC; mDC.CreateCompatibleDC(&dc);
		CBitmap bmp; bmp.CreateCompatibleBitmap(&dc,16,16);
		CBitmap dummy; dummy.CreateCompatibleBitmap(&dc,1,1);
		mDC.SelectObject(&bmp);

		int offset;
		int co;
		int pal;
		int temp = (cur_tileset << 11) + TILESETPATTERNTABLE_OFFSET;

		for(int showrm = 0; showrm < 2; showrm++){
		for(co = 0; co < 128; co++){
				pal = cart->ROM[TILESETPALETTE_ASSIGNMENT + co + (cur_tileset << 7)] & 3;
				offset = TILESETPATTERNTABLE_ASSIGNMENT + co + (cur_tileset << 9);

				DrawTile(&mDC,0,0,cart,temp + (cart->ROM[offset] << 4),MapPalette[showrm][pal],cart->session.tintTiles[cur_tileset + 1][co]);
				DrawTile(&mDC,8,0,cart,temp + (cart->ROM[offset + 128] << 4),MapPalette[showrm][pal],cart->session.tintTiles[cur_tileset + 1][co]);
				DrawTile(&mDC,0,8,cart,temp + (cart->ROM[offset + 256] << 4),MapPalette[showrm][pal],cart->session.tintTiles[cur_tileset + 1][co]);
				DrawTile(&mDC,8,8,cart,temp + (cart->ROM[offset + 384] << 4),MapPalette[showrm][pal],cart->session.tintTiles[cur_tileset + 1][co]);

				mDC.SelectObject(&dummy);
				cart->GetStandardTiles(cur_map,showrm).Add(&bmp,RGB(255,1,255));
				mDC.SelectObject(&bmp);
				m_prog->OffsetPos(1);
			}}

		mDC.DeleteDC();
		bmp.DeleteObject();
		dummy.DeleteObject();
		cart->OK_tiles[cur_map] = 1;
	}
	else m_prog->OffsetPos(256);
}

void CMaps::LoadValues()
{
	//load the tileset
	cur_tileset = cart->ROM[MAPTILESET_ASSIGNMENT + cur_map];
	m_tileset.SetCurSel(cur_tileset);

	//load the palettes
	int offset = MAPPALETTE_OFFSET + (cur_map * 0x30) + 0x10;
	int co;
	for(co = 0; co < 8; co++, offset++) ControlPalette[co] = cart->ROM[offset];
	for(co = 0; co < 8; co++, offset++) SpritePalette[0][co] = cart->ROM[offset];
	SpritePalette[0][0] = 0x40; SpritePalette[1][0] = 0x40;
	offset -= 0x20;
	for(co = 0; co < 0x20; co++, offset++){
		if(co == 0x10) offset += 0x10;
		if((co & 3) == 0) MapPalette[0][0][co] = ControlPalette[0];
		else MapPalette[0][0][co] = cart->ROM[offset];}

	CLoading dlg; dlg.Create(IDD_LOADING,this);
	dlg.m_progress.SetRange(0,286);
	dlg.m_progress.SetPos(0);
	dlg.ShowWindow(1);

	ReloadImages(&dlg.m_progress);
	ReloadSprites(&dlg.m_progress);

	dlg.ShowWindow(0);

	BYTE temp;
	Maps::DecompressMap(*Project, cur_map, MAP_OFFSET, MAP_PTRADD, DecompressedMap);

	//load the 16 sprites
	offset = MAPSPRITE_OFFSET + (cur_map * 0x30);
	for(auto coX = 0; coX < MAPSPRITE_COUNT; offset += 3, coX++){
		Sprite_Value[coX] = cart->ROM[offset];
		temp = cart->ROM[offset + 1];
		Sprite_Coords[coX].x = temp & 0x3F;
		Sprite_InRoom[coX] = (temp & 0x80) != 0;
		Sprite_StandStill[coX] = (temp & 0x40) != 0;
		Sprite_Coords[coX].y = cart->ROM[offset + 2];}

	OnSelchangeSpriteList();

	//load the battle formations (from domain data).
	offset = BATTLEDOMAIN_OFFSET + 0x200 + (cur_map << 3);
	m_battle1.SetCurSel(cart->ROM[offset] & 0x7F); m_formation1.SetCheck(cart->ROM[offset] & 0x80); offset += 1;
	m_battle2.SetCurSel(cart->ROM[offset] & 0x7F); m_formation2.SetCheck(cart->ROM[offset] & 0x80); offset += 1;
	m_battle3.SetCurSel(cart->ROM[offset] & 0x7F); m_formation3.SetCheck(cart->ROM[offset] & 0x80); offset += 1;
	m_battle4.SetCurSel(cart->ROM[offset] & 0x7F); m_formation4.SetCheck(cart->ROM[offset] & 0x80); offset += 1;
	m_battle5.SetCurSel(cart->ROM[offset] & 0x7F); m_formation5.SetCheck(cart->ROM[offset] & 0x80); offset += 1;
	m_battle6.SetCurSel(cart->ROM[offset] & 0x7F); m_formation6.SetCheck(cart->ROM[offset] & 0x80); offset += 1;
	m_battle7.SetCurSel(cart->ROM[offset] & 0x7F); m_formation7.SetCheck(cart->ROM[offset] & 0x80); offset += 1;
	m_battle8.SetCurSel(cart->ROM[offset] & 0x7F); m_formation8.SetCheck(cart->ROM[offset] & 0x80);

	offset = BATTLEPROBABILITY_OFFSET;
	int buffer[8] = {0,0,0,0,0,0,0,0};
	for(co = 0; co < 64; co++)
		buffer[cart->ROM[offset + co]] += 1;
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

void CMaps::LoadTileData()
{
	int offset = TILESET_TILEDATA + (cur_tileset << 8) + (cur_tile << 1);
	BYTE temp = cart->ROM[offset];
	BYTE byte2 = cart->ROM[offset + 1];

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

void CMaps::StoreValues()
{
	StoreTileData();

	//store the tileset
	cart->ROM[MAPTILESET_ASSIGNMENT + cur_map] = (BYTE)cur_tileset;

	//store the palettes
	int offset = MAPPALETTE_OFFSET + (cur_map * 0x30) + 0x10;
	int co;
	for (co = 0; co < 8; co++, offset++) cart->ROM[offset] = ControlPalette[co];
	for (co = 0; co < 8; co++, offset++) cart->ROM[offset] = SpritePalette[0][co];
	cart->ROM[offset - 8] = 0x0F; cart->ROM[offset - 4] = 0x0F;
	offset -= 0x20;
	for (co = 0; co < 0x20; co++, offset++) {
		if (co == 0x10) offset += 0x10;
		cart->ROM[offset] = MapPalette[0][0][co];
	}

	//store the 16 sprites
	offset = MAPSPRITE_OFFSET + (cur_map * 0x30);
	BYTE temp = 0;
	for (co = 0; co < MAPSPRITE_COUNT; offset += 3, co++) {
		cart->ROM[offset] = (BYTE)Sprite_Value[co];
		temp = (BYTE)Sprite_Coords[co].x;
		if (Sprite_InRoom[co]) temp |= 0x80;
		if (Sprite_StandStill[co]) temp |= 0x40;
		cart->ROM[offset + 1] = temp;
		cart->ROM[offset + 2] = (BYTE)Sprite_Coords[co].y;
	}

	//store the domains
	int ref = BATTLEDOMAIN_OFFSET + 0x200 + (cur_map << 3);

	cart->ROM[ref] = (BYTE)(m_battle1.GetCurSel() + (m_formation1.GetCheck() << 7));
	cart->ROM[ref + 1] = (BYTE)(m_battle2.GetCurSel() + (m_formation2.GetCheck() << 7));
	cart->ROM[ref + 2] = (BYTE)(m_battle3.GetCurSel() + (m_formation3.GetCheck() << 7));
	cart->ROM[ref + 3] = (BYTE)(m_battle4.GetCurSel() + (m_formation4.GetCheck() << 7));
	cart->ROM[ref + 4] = (BYTE)(m_battle5.GetCurSel() + (m_formation5.GetCheck() << 7));
	cart->ROM[ref + 5] = (BYTE)(m_battle6.GetCurSel() + (m_formation6.GetCheck() << 7));
	cart->ROM[ref + 6] = (BYTE)(m_battle7.GetCurSel() + (m_formation7.GetCheck() << 7));
	cart->ROM[ref + 7] = (BYTE)(m_battle8.GetCurSel() + (m_formation8.GetCheck() << 7));

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
			cart->ROM[ref] = (BYTE)bigco;
	}

	for (;ref < max; ref++)
		cart->ROM[ref] = 0;

	offset = MAPBATTLERATE_OFFSET + cur_map + 1; // skip first entry (overworld)
	Proj2->ROM[offset] = (unsigned char)(Ui::GetControlInt(m_encounterrateedit) & 0xFF);

	OnFindkab();
	if (kab < 0)
		THROWEXCEPTION(std::runtime_error, "Not enough KAB to store map data.");

	CompressMap();
}

void CMaps::StoreTileData()
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

	cart->ROM[offset] = temp;
	cart->ROM[offset + 1] = (BYTE)byte2;
}

void CMaps::OnPaint()
{
	CPaintDC dc(this);
	handle_paint(dc);
}

void CMaps::OnSelchangeMaplist()
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
	InvalidateRect(rcPalettes,0);
	InvalidateRect(rcPalettes2,0);
	invalidate_maps();
}

void CMaps::OnShowrooms()
{
	m_showingrooms = Ui::GetCheckValue(m_showrooms);
	InvalidateRect(rcTiles, 0);
	invalidate_maps();
}

void CMaps::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	handle_hscroll(nSBCode, nPos, pScrollBar);
}

void CMaps::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	handle_vscroll(nSBCode, nPos, pScrollBar);
}

void CMaps::OnLButtonDown(UINT nFlags, CPoint pt)
{
	mousedown = 0;
	int tile;
	if (PtInRect(rcMap, pt)) {
		auto fixedpt = fix_map_point(pt);
		if (coords_dlg.m_mouseclick.GetCheck())
			HandleRButtonDown(nFlags, fixedpt);
		else
			HandleLButtonDown(nFlags, fixedpt);
	}
	else if (PtInRect(rcTiles, pt)) {
		pt.x = (pt.x - rcTiles.left) >> 4;
		pt.y = (pt.y - rcTiles.top) & 0xF0;
		StoreTileData();
		cur_tile = pt.x + pt.y;
		LoadTileData();
		InvalidateRect(rcTiles, 0);
	}
	else if (PtInRect(rcPalettes, pt)) {
		pt.x = (pt.x - rcPalettes.left) >> 4;
		pt.y = (pt.y - rcPalettes.top) & 0xF0;
		tile = pt.x + (pt.y << 1);
		if (!(tile & 0x03)) tile = 0;

		CNESPalette dlg;
		dlg.Proj2 = cart;
		int temp = MAPPALETTE_OFFSET + (cur_map * 0x30) + tile;
		if (!tile) temp += 0x18;
		dlg.color = &cart->ROM[temp];
		if (dlg.DoModal() == IDOK) {
			MapPalette[pt.y >= 0x10][0][pt.x] = *dlg.color;
			if (!tile) {
				tile = *dlg.color;
				ControlPalette[0] = (BYTE)tile;
				for (int co = 0; co <= 28; co += 4)
					MapPalette[0][0][co] = (BYTE)tile;
			}
			UpdatePics();
			InvalidateRect(rcPalettes, 0);
			InvalidateRect(rcPalettes2, 0);
		}
	}
	else if (PtInRect(rcPalettes2, pt)) {
		pt.x = (pt.x - rcPalettes2.left) >> 4;
		pt.y = ((pt.y - rcPalettes2.top) & 0xF0) >> 1;
		tile = pt.x + pt.y;
		if (tile == 0 || tile == 4) return;
		if (tile >= 8) tile -= 8;
		else tile += 8;

		CNESPalette dlg;
		dlg.Proj2 = cart;
		dlg.color = &cart->ROM[MAPPALETTE_OFFSET + (cur_map * 0x30) + tile + 0x10];
		if (dlg.DoModal() == IDOK) {
			if (tile < 8) ControlPalette[tile] = *dlg.color;
			else SpritePalette[0][tile - 8] = *dlg.color;
			if (!tile) {
				tile = *dlg.color;
				ControlPalette[0] = (BYTE)tile;
				for (int co = 0; co <= 28; co += 4)
					MapPalette[0][0][co] = (BYTE)tile;
			}
			UpdatePics();
			InvalidateRect(rcPalettes, 0);
			InvalidateRect(rcPalettes2, 0);
		}
	}
	else {
		HandleLbuttonDrag(this);
	}
}

void CMaps::OnLButtonUp(UINT nFlags, CPoint pt)
{
	HandleLButtonUp(nFlags, fix_map_point(pt));
	mousedown = 0;
}

void CMaps::OnLButtonDblClk(UINT nFlags, CPoint pt)
{
	UNREFERENCED_PARAMETER(nFlags);

	if (PtInRect(rcTiles, pt)) {
		CTileEdit dlg;
		dlg.Invoker = CTileEdit::Maps;
		dlg.Proj2 = Proj2;
		dlg.tileset = (BYTE)(cur_tileset + 1);
		dlg.tile = (BYTE)cur_tile;
		dlg.pal[0] = MapPalette[0][0];
		dlg.pal[1] = MapPalette[1][0];
		OnSave();
		if (dlg.DoModal() == IDOK) {
			cart->OK_tiles[cur_map] = 0;
			cart->GetStandardTiles(cur_map, 0).DeleteImageList();
			cart->GetStandardTiles(cur_map, 1).DeleteImageList();
			CLoading load; load.Create(IDD_LOADING, this);
			load.m_progress.SetRange(0, 124);
			load.m_progress.SetPos(0);
			load.ShowWindow(1);
			ReloadImages(&load.m_progress);
			load.ShowWindow(0);

			InvalidateRect(rcPalettes, 0);
			InvalidateRect(rcTiles, 0);
			invalidate_maps();
		}
	}
}

void CMaps::OnRButtonDown(UINT nFlags, CPoint pt)
{
	mousedown = 0;
	if (PtInRect(rcTiles, pt)) {
		OnLButtonDown(nFlags, pt);
	}
	else if (PtInRect(rcMap, pt)) {
		HandleRButtonDown(nFlags, fix_map_point(pt));
	}
}

void CMaps::OnRButtonUp(UINT nFlags, CPoint point)
{
	HandleRButtonUp(nFlags, fix_map_point(point));
	mousedown = 0;
}

void CMaps::OnRButtonDblClk(UINT nFlags, CPoint pt)
{
	UNREFERENCED_PARAMETER(nFlags);

	int ref = -1;
	if (PtInRect(rcMap, pt)) {
		HandleRButtonDblClk(nFlags, fix_map_point(pt));
	}
	else if(PtInRect(rcTiles,pt)){
		pt.x = (pt.x - rcTiles.left) >> 4;
		pt.y = (pt.y - rcTiles.top) & 0xF0;
		ref = pt.x + pt.y;}
	if(ref != -1){
		int old = cart->session.tintTiles[cur_tileset + 1][ref];
		CTint dlg;
		dlg.tintvalue = old;
		dlg.m_tintvariant = cart->session.tintVariant;
		if (dlg.DoModal() == IDOK) {

			cart->OK_tiles[cur_map] = 0;
			cart->GetStandardTiles(cur_map, 0).DeleteImageList();
			cart->GetStandardTiles(cur_map, 1).DeleteImageList();

			cart->session.tintTiles[cur_tileset + 1][ref] = (BYTE)dlg.tintvalue;
			if (cart->session.tintVariant != dlg.m_tintvariant) {
				cart->session.tintVariant = (BYTE)dlg.m_tintvariant;
				cart->ReTintPalette();
			}

			CLoading dlgmaps;
			dlgmaps.Create(IDD_LOADING, this);
			dlgmaps.m_progress.SetRange(0, 128);
			dlgmaps.m_progress.SetPos(0);
			dlgmaps.ShowWindow(1);

			ReloadImages(&dlgmaps.m_progress);

			dlgmaps.ShowWindow(0);
			InvalidateRect(rcTiles, 0);
			invalidate_maps();
		}
	}
}

void CMaps::OnMouseMove(UINT nFlags, CPoint pt)
{
	UNREFERENCED_PARAMETER(nFlags);

	CString text = "";
	if (PtInRect(rcMap, pt)) {
		HandleMouseMove(nFlags, fix_map_point(pt));
	}
	else {
		if (ptHover.x != -1) {
			m_hovering.SetWindowText(text);
			ptHover.x = -1;
		}
	}
}

void CMaps::UpdatePics()
{
	CLoading dlg; dlg.Create(IDD_LOADING,this);
	dlg.m_progress.SetRange(0,286);
	dlg.m_progress.SetPos(0);
	dlg.ShowWindow(1);

	cart->OK_tiles[cur_map] = 0;
	cart->GetStandardTiles(cur_map,0).DeleteImageList();
	cart->GetStandardTiles(cur_map,1).DeleteImageList();

	ReloadImages(&dlg.m_progress);
	ReloadSprites(&dlg.m_progress);

	dlg.ShowWindow(0);
	InvalidateRect(rcTiles,0);
	invalidate_maps();
}

void CMaps::UpdateTileData()
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

void CMaps::OnMove()
{cur_tiledata ^= 0x01;
UpdateTileData();}
void CMaps::OnFight()
{cur_tiledata ^= 0x02;
UpdateTileData();}
void CMaps::OnShop()
{cur_tiledata ^= 0x04;
UpdateTileData();}
void CMaps::OnTc()
{cur_tiledata ^= 0x08;
UpdateTileData();}
void CMaps::OnSpecial()
{cur_tiledata ^= 0x10;
UpdateTileData();}
void CMaps::OnWarp()
{cur_tiledata ^= 0x20;
UpdateTileData();}
void CMaps::OnTeleport()
{cur_tiledata ^= 0x40;
UpdateTileData();}

void CMaps::OnSelchangeTcList()
{
	if(cur_tc != -1) StoreTC();
	cur_tc = m_tc_list.GetCurSel();
	LoadTC();
}

void CMaps::OnSelchangeTcitemList()
{
	if(cur_tcitem != -1) StoreTCItem();
	cur_tcitem = m_tcitem_list.GetCurSel();
	LoadTCItem();
}

void CMaps::StoreTC()
{
	StoreTCItem();
	cart->ROM[TREASURE_OFFSET + cur_tc] = (BYTE)(cur_tcitem + 1);
}

void CMaps::LoadTC()
{
	cur_tcitem = cart->ROM[TREASURE_OFFSET + cur_tc] - 1;
	m_tcitem_list.SetCurSel(cur_tcitem);
	LoadTCItem();
}

void CMaps::StoreTCItem()
{
	int offset = ITEMPRICE_OFFSET + (cur_tcitem << 1);
	int temp;
	CString text; m_tcitem_price.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFFFF) temp = 0xFFFF;
	cart->ROM[offset] = temp & 0xFF;
	cart->ROM[offset + 1] = (BYTE)(temp >> 8);
}

void CMaps::LoadTCItem()
{
	int offset = ITEMPRICE_OFFSET + (cur_tcitem << 1);
	CString text; text.Format("%d",cart->ROM[offset] + (cart->ROM[offset + 1] << 8));
	m_tcitem_price.SetWindowText(text);
}

void CMaps::OnSelchangeTileset()
{
	StoreTileData();
	cur_tileset = m_tileset.GetCurSel();
	CLoading dlg; dlg.Create(IDD_LOADING,this);
	dlg.m_progress.SetRange(0,286);
	dlg.m_progress.SetPos(0);
	dlg.ShowWindow(1);

	cart->OK_tiles[cur_map] = 0;
	cart->GetStandardTiles(cur_map,0).DeleteImageList();
	cart->GetStandardTiles(cur_map,1).DeleteImageList();

	ReloadImages(&dlg.m_progress);
	ReloadSprites(&dlg.m_progress);

	dlg.ShowWindow(0);
	InvalidateRect(rcTiles,0);
	invalidate_maps();
	LoadTileData();
}

void CMaps::OnSelchangeSpriteList()
{
	int temp = m_sprite_list.GetCurSel();
	m_inroom.SetCheck(Sprite_InRoom[temp]);
	m_still.SetCheck(Sprite_StandStill[temp]);
	CString text; text.Format("%X",Sprite_Coords[temp].x);
	m_spritecoordx.SetWindowText(text);
	text.Format("%X",Sprite_Coords[temp].y);
	m_spritecoordy.SetWindowText(text);
	m_sprite.SetCurSel(Sprite_Value[temp]);
	m_spritegraphic.SetCurSel(cart->ROM[MAPSPRITE_PICASSIGNMENT + Sprite_Value[temp]]);
}

void CMaps::OnEditspritegfx()
{
	OnSave();
	CMapman dlg;
	dlg.Proj2 = Proj2;
	dlg.graphicoffset = MAPSPRITEPATTERNTABLE_OFFSET + (m_spritegraphic.GetCurSel() << 8);
	dlg.paletteoffset = MAPPALETTE_OFFSET + (cur_map * 0x30) + 0x18;
	dlg.DoModal();
}

void CMaps::OnStill()
{Sprite_StandStill[m_sprite_list.GetCurSel()] = m_still.GetCheck() != 0;}

void CMaps::OnInroom()
{
	Sprite_InRoom[m_sprite_list.GetCurSel()] = m_inroom.GetCheck() != 0;
	invalidate_maps();
}

void CMaps::OnChangeSpritecoordx()
{
	CString text; int number;
	m_spritecoordx.GetWindowText(text); number = StringToInt_HEX(text);
	if(number > 0x3F) number = 0x3F;
	Sprite_Coords[m_sprite_list.GetCurSel()].x = number;
	invalidate_maps();
}

void CMaps::OnChangeSpritecoordy()
{
	CString text; int number;
	m_spritecoordy.GetWindowText(text); number = StringToInt_HEX(text);
	if(number > 0x3F) number = 0x3F;
	Sprite_Coords[m_sprite_list.GetCurSel()].y = number;
	invalidate_maps();
}

void CMaps::OnSelchangeSprite()
{
	Sprite_Value[m_sprite_list.GetCurSel()] = (short)m_sprite.GetCurSel();
	m_spritegraphic.SetCurSel(cart->ROM[MAPSPRITE_PICASSIGNMENT + m_sprite.GetCurSel()]);
	invalidate_maps();
}

void CMaps::OnSelchangeSpritegraphic()
{
	cart->ROM[MAPSPRITE_PICASSIGNMENT + Sprite_Value[m_sprite_list.GetCurSel()]] = (BYTE)m_spritegraphic.GetCurSel();
	invalidate_maps();
}

void CMaps::UpdateClick(CPoint pt)
{
	ptLastClick = pt;
	CString text; text.Format("%X,%X",pt.x,pt.y);
	m_lastclick.SetWindowText(text);
}

CPoint CMaps::fix_map_point(CPoint point)
{
	CPoint fixedpt;
	fixedpt.x = ((point.x - rcMap.left) + ScrollOffset.x) / m_displaytilecounts.cx;
	fixedpt.y = ((point.y - rcMap.top) + ScrollOffset.y) / m_displaytilecounts.cy;
	return fixedpt;
}

void CMaps::OnShowlastclick()
{
	cart->session.showLastClick = (m_showlastclick.GetCheck() != 0);
	invalidate_maps();
}

void CMaps::OnCustomtool()
{
	HandleCustomizeTool();
}

void CMaps::OnFindkab()
{
	kab = MAP_END - MAP_START;
	int co;
	for(co = 0; co < MAP_COUNT; co++)
		kab -= GetByteCount(co,(co != cur_map));

	CString text;
	text.Format("%d",kab); m_kab.SetWindowText(text);
}

int CMaps::GetByteCount(int map,bool compressed)
{
	int count = 0;
	int co, temp;
	if(compressed)
	{
		temp = MAP_OFFSET + (map << 1);
		co = MAP_PTRADD + cart->ROM[temp] + (cart->ROM[temp + 1] << 8);
		for(;cart->ROM[co] != 0xFF; co++, count++);
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

void CMaps::CompressMap()
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
	int mapstart = cart->ROM[mapptr] + (cart->ROM[mapptr + 1] << 8) + MAP_PTRADD;
	int co, temp;
	if(shift > 0){
		for(co = MAP_END - 1; co >= mapstart; co--)
			cart->ROM[co] = cart->ROM[co - shift];}
	if(shift < 0){
		for(co = mapstart; co < MAP_END; co++)
			cart->ROM[co] = cart->ROM[co - shift];}

	//shift pointers
	if(shift != 0){
		for(mapptr += 2; mapptr < mapptrend; mapptr += 2){
			temp = cart->ROM[mapptr] + (cart->ROM[mapptr + 1] << 8);
			temp += shift;
			cart->ROM[mapptr] = temp & 0xFF;
			cart->ROM[mapptr + 1] = (BYTE)(temp >> 8);}
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
			cart->ROM[mapstart] = (BYTE)ThisRun;
			mapstart += 1;}
		else if(RunLength == 255){
			cart->ROM[mapstart] = (BYTE)ThisRun;
			cart->ROM[mapstart + 1] = (BYTE)(ThisRun | 0x80);
			cart->ROM[mapstart + 2] = 254;
			mapstart += 3;}
		else{
			cart->ROM[mapstart] = (BYTE)(ThisRun | 0x80);
			cart->ROM[mapstart + 1] = (BYTE)RunLength;
			if(RunLength == 256) cart->ROM[mapstart + 1] = 0;
			mapstart += 2;}
	}
	cart->ROM[mapstart] = 0xFF;
}

void CMaps::OnEditSpriteLabel()
{
	int temp = m_sprite.GetCurSel();
	//ChangeLabel(*cart, -1, LoadSpriteLabel(*cart, temp), WriteSpriteLabel, temp, nullptr, &m_sprite); //TODO  -implement
}

void CMaps::OnEditgfxlabel()
{
	int temp = m_spritegraphic.GetCurSel();
	//ChangeLabel(*cart, -1, LoadSpriteGraphicLabel(*cart, temp), WriteSpriteGraphicLabel, temp, nullptr, &m_spritegraphic);
}

void CMaps::OnMaplabel()
{
	int themap = m_maplist.GetCurSel();
	//ChangeLabel(*cart, -1, LoadMapLabel(*cart, themap), WriteMapLabel, themap, &m_maplist, nullptr); //TODO - implement

	int temp = coords_dlg.m_coord_l.GetCurSel();
	coords_dlg.m_coord_l.DeleteString(themap);
	coords_dlg.m_coord_l.InsertString(themap, Labels2::LoadMapLabel(*cart, themap).name);
	coords_dlg.m_coord_l.SetCurSel(temp);
}

void CMaps::OnEditlabel()
{
	int temp = m_tc_list.GetCurSel();
	//ChangeLabel(*cart, -1, LoadTreasureLabel(*cart, temp), WriteTreasureLabel, temp, nullptr, &m_tc_list); //TODO - implement
}

void CMaps::OnTilesetlabel()
{
	int temp = m_tileset.GetCurSel();
	//ChangeLabel(*cart, -1, LoadTilesetLabel(*cart, temp), WriteTilesetLabel, temp, nullptr, &m_tileset);
}

void CMaps::OnMapExport()
{
	CString text = Labels2::LoadMapLabel(*cart, cur_map).name + " Map." + CString(FFH_MAP_EXT);
	CString filename = Paths::Combine({ FOLDERPREF(Proj2->AppSettings, PrefMapImportExportFolder), text });
	auto result = Ui::SaveFilePromptExt(this, FFH_MAP_FILTER, FFH_MAP_EXT, "Export Standard Map", filename);
	if (!result) return;

	FILE* file = fopen(result.value, "w+b");
	if(file == nullptr){
		AfxMessageBox("Error saving map", MB_ICONERROR);
		return;}
	fwrite(DecompressedMap,1,0x1000,file);
	fclose(file);
}

void CMaps::OnMapImport()
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
	invalidate_maps();
}

void CMaps::OnViewcoords()
{
	coords_dlg.ShowWindow(m_viewcoords.GetCheck());
}

void CMaps::UpdateTeleportLabel(int arid, bool NNTele)
{
	int temp = m_teleport_list.GetCurSel();
	if (NNTele) {
		m_teleport_list.DeleteString(arid);
		m_teleport_list.InsertString(arid, Labels2::LoadNNTeleportLabel(*cart, arid).name);
	}
	else {
		m_teleport_list.DeleteString(arid + NNTELEPORT_COUNT);
		m_teleport_list.InsertString(arid + NNTELEPORT_COUNT, Labels2::LoadNOTeleportLabel(*cart, arid).name);
	}
	m_teleport_list.SetCurSel(temp);
}

void CMaps::DoHScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	if (m_popoutcreated)
	{
		//N.B. - minimap subtracts 8 in an attempt to center the viewpoint
		// For now, we workaround that by adding back the 8 that was subtracted
		// and let the popout map handle centering directly.
		int iPos = ((INT)nPos);
		int newval = (iPos + 8) * 100 / 255; // convert to percentage
		m_popoutmap.ScrollByPercentage(SB_HORZ, newval);
	}
	handle_hscroll(nSBCode, nPos * m_tiledims.cx, pScrollBar);
}

void CMaps::DoVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	if (m_popoutcreated) {
		//N.B. - see the note in OnVHScroll, similar logic here.
		int iPos = ((INT)nPos);
		int newval = (iPos + 8) * 100 / 255;
		m_popoutmap.ScrollByPercentage(SB_VERT, newval);
	}
	handle_vscroll(nSBCode, nPos * m_tiledims.cy, pScrollBar);
}

void CMaps::SendKeydown(WPARAM wparam, LPARAM lparam)
{
	this->PostMessage(WM_KEYDOWN, wparam, lparam);
}

//DEVNOTE - for the HandleXXXX button methods,
//		CPoint is a MAP position, NOT a pixel coordinate, where
//		x = column
//		y = row
//		It's assumed that the caller already made this
//		adjustment by calling fix_map_point(point) to convert
//		from pixels to map position.

void CMaps::HandleLButtonDown(UINT nFlags, CPoint point)
{
	switch (cur_tool) {
	case 0: {		//pencil
		mousedown = 1;
		UpdateClick(point);
		DecompressedMap[point.y][point.x] = (BYTE)cur_tile;
		invalidate_maps();
	}break;
	default: {		//fill/smarttools
		mousedown = 1;
		UpdateClick(point);
		rcToolRect.SetRect(point.x, point.y, point.x, point.y);
		invalidate_maps();
	}break;
	}
}

void CMaps::HandleLButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);

	if (mousedown) {
		rcToolRect.NormalizeRect();
		int coY, coX, temp, co;
		bool draw;
		switch (cur_tool) {
		case 0: break;
		case 1: {			//fill
			for (coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++) {
				for (coX = rcToolRect.left; coX <= rcToolRect.right; coX++)
					DecompressedMap[coY][coX] = (BYTE)cur_tile;
			}
			invalidate_maps();
		}break;

		default: {			//smarttools
			temp = cur_tool - 2 + (cur_tileset << 1);
			//flood fill
			for (coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++) {
				for (coX = rcToolRect.left; coX <= rcToolRect.right; coX++)
					DecompressedMap[coY][coX] = Proj2->session.smartTools[temp][4];
			}
			//"smart" top edge
			coY = rcToolRect.top;
			for (coX = rcToolRect.left; coX <= rcToolRect.right; coX++) {
				draw = 1;
				if (coY) {
					for (co = 0; co < 6 && draw; co++) {
						if (DecompressedMap[coY - 1][coX] == Proj2->session.smartTools[temp][co]) draw = 0;
					}
				}
				if (draw) DecompressedMap[coY][coX] = Proj2->session.smartTools[temp][1];
			}
			//"smart" bottom edge
			coY = rcToolRect.bottom;
			for (coX = rcToolRect.left; coX <= rcToolRect.right; coX++) {
				draw = 1;
				if (coY < 255) {
					for (co = 3; co < 9 && draw; co++) {
						if (DecompressedMap[coY + 1][coX] == Proj2->session.smartTools[temp][co]) draw = 0;
					}
				}
				if (draw) DecompressedMap[coY][coX] = Proj2->session.smartTools[temp][7];
			}
			//"smart" left edge
			coX = rcToolRect.left;
			for (coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++) {
				draw = 1;
				if (coX) {
					for (co = 0; co < 8 && draw; co++) {
						if (co % 3 == 2) co++;
						if (DecompressedMap[coY][coX - 1] == Proj2->session.smartTools[temp][co]) draw = 0;
					}
				}
				if (draw) DecompressedMap[coY][coX] = Proj2->session.smartTools[temp][3];
			}
			//"smart" right edge
			coX = rcToolRect.right;
			for (coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++) {
				draw = 1;
				if (coX < 255) {
					for (co = 1; co < 9 && draw; co++) {
						if (co % 3 == 0) co++;
						if (DecompressedMap[coY][coX + 1] == Proj2->session.smartTools[temp][co]) draw = 0;
					}
				}
				if (draw) DecompressedMap[coY][coX] = Proj2->session.smartTools[temp][5];
			}
			//"smart" NW corner
			co = 0;
			draw = 1;
			if (rcToolRect.left) {
				coY = DecompressedMap[rcToolRect.top][rcToolRect.left - 1];
				for (coX = 0; draw && coX < 8; coX++) {
					if (coX % 3 == 2) coX++;
					if (coY == Proj2->session.smartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) co = 1;
			draw = 1;
			if (rcToolRect.top) {
				coY = DecompressedMap[rcToolRect.top - 1][rcToolRect.left];
				for (coX = 0; draw && coX < 6; coX++) {
					if (coY == Proj2->session.smartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) {
				if (co == 1) co = 4;
				else co = 3;
			}
			DecompressedMap[rcToolRect.top][rcToolRect.left] = Proj2->session.smartTools[temp][co];
			//"smart" SW corner
			co = 6;
			draw = 1;
			if (rcToolRect.left) {
				coY = DecompressedMap[rcToolRect.bottom][rcToolRect.left - 1];
				for (coX = 0; draw && coX < 8; coX++) {
					if (coX % 3 == 2) coX++;
					if (coY == Proj2->session.smartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) co = 7;
			draw = 1;
			if (rcToolRect.bottom < 255) {
				coY = DecompressedMap[rcToolRect.bottom + 1][rcToolRect.left];
				for (coX = 3; draw && coX < 9; coX++) {
					if (coY == Proj2->session.smartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) {
				if (co == 7) co = 4;
				else co = 3;
			}
			DecompressedMap[rcToolRect.bottom][rcToolRect.left] = Proj2->session.smartTools[temp][co];
			//"smart" NE corner
			co = 2;
			draw = 1;
			if (rcToolRect.right < 255) {
				coY = DecompressedMap[rcToolRect.top][rcToolRect.right + 1];
				for (coX = 1; draw && coX < 9; coX++) {
					if (coX % 3 == 0) coX++;
					if (coY == Proj2->session.smartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) co = 1;
			draw = 1;
			if (rcToolRect.top) {
				coY = DecompressedMap[rcToolRect.top - 1][rcToolRect.right];
				for (coX = 0; draw && coX < 6; coX++) {
					if (coY == Proj2->session.smartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) {
				if (co == 1) co = 4;
				else co = 5;
			}
			DecompressedMap[rcToolRect.top][rcToolRect.right] = Proj2->session.smartTools[temp][co];
			//"smart" SE corner
			co = 8;
			draw = 1;
			if (rcToolRect.right < 255) {
				coY = DecompressedMap[rcToolRect.bottom][rcToolRect.right + 1];
				for (coX = 1; draw && coX < 9; coX++) {
					if (coX % 3 == 0) coX++;
					if (coY == Proj2->session.smartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) co = 7;
			draw = 1;
			if (rcToolRect.bottom < 255) {
				coY = DecompressedMap[rcToolRect.bottom + 1][rcToolRect.right];
				for (coX = 3; draw && coX < 9; coX++) {
					if (coY == Proj2->session.smartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) {
				if (co == 7) co = 4;
				else co = 5;
			}
			DecompressedMap[rcToolRect.bottom][rcToolRect.right] = Proj2->session.smartTools[temp][co];

			invalidate_maps();
		}break;
		}
		OnFindkab();
	}
}

void CMaps::HandleLButtonDblClk(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	// Currently, left dblclk does nothing on the map.
}

void CMaps::HandleRButtonDown(UINT nFlags, CPoint pt)
{
	UNREFERENCED_PARAMETER(nFlags);

	StoreTileData();
	OnFindkab();
	UpdateClick(pt);
	InvalidateRect(rcTiles, 0);
	if (m_showlastclick.GetCheck()) {
		invalidate_maps();
	}
	if (coords_dlg.m_mouseclick.GetCheck()) {
		coords_dlg.m_coord_l.SetCurSel(m_maplist.GetCurSel());
		coords_dlg.OnSelchangeCoordL();
		coords_dlg.InputCoords(pt);
		return;
	}
	cur_tile = DecompressedMap[pt.y][pt.x];
	LoadTileData();
	if (cur_tool > 1) {
		CheckRadioButton(m_penbutton.GetDlgCtrlID(), m_custom2button.GetDlgCtrlID(),
			m_blockbutton.GetDlgCtrlID());
		cur_tool = m_blockbutton.GetToolIndex();
		m_customtool.EnableWindow(FALSE);
		m_popoutmap.UpdateControls();
	}

	//if they clicked on a sprite... adjust the Sprite Editor accordingly
	for (int co = 0; co < MAPSPRITE_COUNT; co++) {
		if (!Sprite_Value[co]) continue;
		if (Sprite_Coords[co] == pt) {
			mousedown = (BYTE)(co + 2);
			m_sprite_list.SetCurSel(co);
			OnSelchangeSpriteList();
			break;
		}
	}
}

void CMaps::HandleRButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	// By default, does nothing on the map, pass fix_map_point(point) to any handler
}

void CMaps::HandleRButtonDblClk(UINT nFlags, CPoint pt)
{
	UNREFERENCED_PARAMETER(nFlags);

	int ref = DecompressedMap[pt.y][pt.x];
	if (ref != -1)
		ApplyTileTint(ref);
}

void CMaps::HandleMouseMove(UINT nFlags, CPoint newhover)
{
	UNREFERENCED_PARAMETER(nFlags);

	if (ptHover != newhover) {
		ptHover = newhover;
		CString text;
		text.Format("%X,%X", ptHover.x, ptHover.y);
		m_hovering.SetWindowText(text);
		if (mousedown == 1) {
			switch (cur_tool) {
			case 0: {		//pencil
				DecompressedMap[ptHover.y][ptHover.x] = (BYTE)cur_tile;
				invalidate_maps();
				break;
			}
			default: {		//fill / Smarttools
				rcToolRect.right = ptHover.x;
				rcToolRect.bottom = ptHover.y;
				invalidate_maps();
				break;
			}
			}
			UpdateClick(ptHover);
			OnFindkab();
		}
		else if (mousedown) {
			UpdateClick(ptHover);
			OnFindkab();
			Sprite_Coords[mousedown - 2] = ptHover;
			text.Format("%X", ptHover.x);
			m_spritecoordx.SetWindowText(text);
			text.Format("%X", ptHover.y);
			m_spritecoordy.SetWindowText(text);
			invalidate_maps();
		}
	}
}

void CMaps::HandleAfterScroll(CPoint scrolloffset, CRect displayarea)
{
	UNREFERENCED_PARAMETER(scrolloffset);
	UNREFERENCED_PARAMETER(displayarea);
}

void CMaps::HandleMapImport()
{
	OnMapImport();
}

void CMaps::HandleMapExport()
{
	OnMapExport();
}

bool CMaps::HandleCustomizeTool()
{
	CCustomTool dlg;
	dlg.Proj2 = Proj2;
	dlg.m_tiles = &cart->GetStandardTiles(cur_map, m_showrooms.GetCheck());
	dlg.tool = cur_tool - 2 + (cur_tileset << 1);
	auto result = dlg.DoModal();
	return result == IDOK;
}

void CMaps::RenderMapEx(CDC& dc, CRect displayarea, CPoint scrolloff, CSize tiledims)
{
	paint_map_elements(dc, displayarea, scrolloff, tiledims);
}

int CMaps::GetCurrentToolIndex() const
{
	return cur_tool;
}

void CMaps::SetMouseDown(int imousedown)
{
	mousedown = imousedown;
}

int CMaps::GetMouseDown() const
{
	return mousedown;
}

void CMaps::DoViewcoords()
{
	OnViewcoords();
}

void CMaps::DoOK()
{
	OnOK();
}

void CMaps::DoSelchangeMaplist()
{
	OnSelchangeMaplist();
}

void CMaps::ApplyTileTint(int ref)
{
	int old = cart->session.tintTiles[cur_tileset + 1][ref];
	CTint dlg;
	dlg.tintvalue = old;
	dlg.m_tintvariant = cart->session.tintVariant;
	if (dlg.DoModal() == IDOK) {

		cart->OK_tiles[cur_map] = 0;
		cart->GetStandardTiles(cur_map, 0).DeleteImageList();
		cart->GetStandardTiles(cur_map, 1).DeleteImageList();

		cart->session.tintTiles[cur_tileset + 1][ref] = (BYTE)dlg.tintvalue;
		if (cart->session.tintVariant != dlg.m_tintvariant) {
			cart->session.tintVariant = (BYTE)dlg.m_tintvariant;
			cart->ReTintPalette();
		}

		CLoading dlgmaps;
		dlgmaps.Create(IDD_LOADING, this);
		dlgmaps.m_progress.SetRange(0, 128);
		dlgmaps.m_progress.SetPos(0);
		dlgmaps.ShowWindow(1);

		ReloadImages(&dlgmaps.m_progress);

		dlgmaps.ShowWindow(0);
		InvalidateRect(rcTiles, 0);
		invalidate_maps();
	}
}

void CMaps::OnSelchangeTeleportList()
{
	if(!coords_dlg.m_mouseclick.GetCheck()){
		coords_dlg.m_teleportlist.SetCurSel(m_teleport_list.GetCurSel() + ONTELEPORT_COUNT);
		coords_dlg.OnSelchangeTeleportlist();}
}

void CMaps::UpdateTeleportLabel(int areaid, int type)
{
	this->UpdateTeleportLabel(areaid, type == 1);
}

void CMaps::Cancel(int context)
{
	if (context == Coords) {
		m_viewcoords.SetCheck(0);
		DoViewcoords();
	}
}

POINT CMaps::GetLastClick()
{
	return ptLastClick;
}

int CMaps::GetCurMap()
{
	return cur_map;
}

void CMaps::TeleportHere(int mapindex, int x, int y)
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

void CMaps::init_popout_map_window()
{
	//TODO - tile dims are hardcoded as 16 x 16 here
	if (m_popoutmap.CreateModeless(this, m_mapdims, { 16,16 }, this))
	{
		std::vector<sMapDlgButton> buttons(m_toolbuttons.size());
		std::transform(cbegin(m_toolbuttons), cend(m_toolbuttons), begin(buttons),
			[](const CDrawingToolButton* srcbtn) { return srcbtn->GetSpec(); });
		VERIFY(m_popoutmap.SetButtons(buttons));
		m_popoutcreated = true;
	}
	else {
		AfxMessageBox(_T("Unable to initialize the popout map window."));
		m_popoutbutton.EnableWindow(FALSE);
	}
}

void CMaps::PopMapDialog(bool in)
{
	// Show/Hide the popout map dialog
	// Shrink/Grow the main dialog to hide/show the its embedded map area
	// Slide the relevant controls left/right as required

	auto rc = Ui::GetControlRect(&m_mappanel);
	int diff = rc.Width() * (in ? -1 : 1);
	Ui::ShrinkWindow(this, diff, 0);

	if (!in) {
		if (!m_firstpopoutdone) {
			// on initial popout, size the dialog over the portion of the
			// dialog that was hidden.
			m_firstpopoutdone = true;
			auto dlgrc = Ui::GetWindowRect(this);
			auto left = dlgrc.right - GetSystemMetrics(SM_CXSIZEFRAME) + 1;
			CRect poprc{
				left,
				dlgrc.top,
				left + diff + GetSystemMetrics(SM_CXSIZEFRAME),
				dlgrc.bottom + GetSystemMetrics(SM_CYSIZEFRAME)
			};
			m_popoutmap.MoveWindow(poprc);
		}
		m_popoutmap.UpdateControls();
	}

	m_popoutmap.ShowWindow(in ? SW_HIDE : SW_SHOW);
	m_popoutbutton.EnableWindow(in ? 1 : 0);

	sync_map_positions(in);
	if (in) {
		m_popoutbutton.SetFocus();
	}
	else {
		m_popoutmap.PostMessage(WM_SETFOCUS); //DEVNOTE - don't use SetFocus()/SendMessage for this
	}

	std::vector<UINT> ids{ IDHELPBOOK, IDCANCEL2, IDC_SAVE, IDOK, IDCANCEL };
	int slide = -diff;
	for (const auto& id : ids) {
		auto pwnd = GetDlgItem(id);
		Ui::MoveControlBy(pwnd, slide, 0);
	}
}

CSize CMaps::calc_scroll_maximums()
{
	//TODO - screen tile counts (cols and rows) are hardcoded as 16 x 16 here
	return { (m_mapdims.cx - 16) * m_tiledims.cx, (m_mapdims.cy - 16) * m_tiledims.cy };
}

CSize CMaps::get_scroll_limits()
{
	return CSize{
		m_hscroll.GetScrollLimit(),
		m_vscroll.GetScrollLimit()
	};
}

CRect CMaps::get_display_area()
{
	return rcMap;
}

void CMaps::invalidate_maps()
{
	InvalidateRect(rcMap, 0);
	m_popoutmap.InvalidateMap();
}

void CMaps::handle_hscroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UNREFERENCED_PARAMETER(pScrollBar);

	bool controlDown = IsKeyDown(VK_CONTROL);
	switch (nSBCode) {
	case SB_LINELEFT: ScrollOffset.x -= 1 * Ui::MultiplyIf(controlDown, 8); break;
	case SB_LINERIGHT: ScrollOffset.x += 1 * Ui::MultiplyIf(controlDown, 8); break;
	case SB_PAGELEFT: ScrollOffset.x -= 16 * Ui::MultiplyIf(controlDown, 2); break;
	case SB_PAGERIGHT: ScrollOffset.x += 16 * Ui::MultiplyIf(controlDown, 2); break;
	case SB_THUMBTRACK: ScrollOffset.x = nPos; break;
	}
	auto maxes = calc_scroll_maximums();
	if (ScrollOffset.x < 0) ScrollOffset.x = 0;
	if (ScrollOffset.x > maxes.cy) ScrollOffset.x = maxes.cy;

	m_hscroll.SetScrollPos(ScrollOffset.x);
	invalidate_maps();
}

void CMaps::handle_vscroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UNREFERENCED_PARAMETER(pScrollBar);

	bool controlDown = IsKeyDown(VK_CONTROL);
	switch (nSBCode) {
	case SB_LINEUP: ScrollOffset.y -= 1 * Ui::MultiplyIf(controlDown, 8); break;
	case SB_LINEDOWN: ScrollOffset.y += 1 * Ui::MultiplyIf(controlDown, 8); break;
	case SB_PAGEUP: ScrollOffset.y -= 16 * Ui::MultiplyIf(controlDown, 1); break;
	case SB_PAGEDOWN: ScrollOffset.y += 16 * Ui::MultiplyIf(controlDown, 1); break;
	case SB_THUMBTRACK: ScrollOffset.y = nPos; break;
	}
	auto maxes = calc_scroll_maximums();
	if (ScrollOffset.y < 0) ScrollOffset.y = 0;
	if (ScrollOffset.y > maxes.cy) ScrollOffset.y = maxes.cy;

	m_vscroll.SetScrollPos(ScrollOffset.y);
	invalidate_maps();
}

void CMaps::handle_paint(CDC& dc)
{
	CPen* origpen = dc.SelectObject(&redpen);
	int coX = -1, coY = -1, tile = -1;
	CPoint pt;
	bool room = m_showrooms.GetCheck() == 1;

	//Draw the Source Tiles on the screen (8 rows of 16 tiles)
	for(coY = 0, tile = 0, pt.y = rcTiles.top; coY < 0x08; coY++, pt.y += 16){
	for(coX = 0, pt.x = rcTiles.left; coX < 0x10; coX++, pt.x += 16, tile++)
			cart->GetStandardTiles(cur_map,room).Draw(&dc,tile,pt,ILD_NORMAL);}
	pt.x = ((cur_tile & 0x0F) << 4) + rcTiles.left;
	pt.y = (cur_tile & 0xF0) + rcTiles.top;
	dc.MoveTo(pt); pt.x += 15;
	dc.LineTo(pt); pt.y += 15;
	dc.LineTo(pt); pt.x -= 15;
	dc.LineTo(pt); pt.y -= 15;
	dc.LineTo(pt);

	{
		// Draw the map elements (map, sprites, last click rect).
		CSize tiledims = { 16,16 };
		CPoint scrolloff = ScrollOffset;
		CDC compat;
		compat.CreateCompatibleDC(&dc);
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dc, rcMap.Width() + tiledims.cx, rcMap.Height() + tiledims.cy);
		auto oldbmp = compat.SelectObject(&bmp);
		CRect client = { 0,0,rcMap.Width() + tiledims.cx, rcMap.Height() };
		compat.FillSolidRect(client, RGB(255, 255, 255));
		paint_map_elements(compat, rcMap, scrolloff, tiledims);

		// Draw the bitmap to the window's display area.
		// To ensure we only draw to that area, set it as the clip region.
		CRgn rgn;
		rgn.CreateRectRgn(rcMap.left, rcMap.top, rcMap.right, rcMap.bottom);
		compat.SelectClipRgn(&rgn);

		auto save = dc.SaveDC();
		dc.BitBlt(rcMap.left, rcMap.top, rcMap.Width(), rcMap.Height(),
			&compat,
			0, 0,
			SRCCOPY);
		dc.RestoreDC(save);
		compat.SelectObject(oldbmp);
	}

	//Draw the palettes
	CBrush br;
	CRect rc(0, 0, 16, 16);
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

	dc.SelectObject(origpen);
	m_banner.Render(dc, 8, 8);
}

void CMaps::paint_map_elements(CDC& dc, CRect displayrect, CPoint scrolloff, CSize tiledims)
{
	ASSERT(tiledims.cx > 0);
	ASSERT(tiledims.cy > 0);
	if (tiledims.cx <= 0 || tiledims.cy <= 0) return;

	CRect displayarea = { 0, 0, displayrect.Width(), displayrect.Height() };
	CSize gridvis = { displayarea.Width() / tiledims.cx, displayarea.Height() / tiledims.cy };
	CPoint gridanchor = { scrolloff.x / tiledims.cx, scrolloff.y / tiledims.cy };
	int coX, coY, tile;
	CPoint pt;
	CPoint copt;
	CRect rcTemp = rcToolRect;
	rcTemp.NormalizeRect(); rcTemp.right += 1; rcTemp.bottom += 1;

	// If we aren't aligned on a tile boundary, then the first col/row is partially drawn.
	// Back up the start of drawing to accomodate this.
	CPoint tileoff = { scrolloff.x % tiledims.cx, scrolloff.y % tiledims.cy };
	displayarea.left -= tileoff.x;
	displayarea.top -= tileoff.y;
	if (tileoff.x) ++gridvis.cx;
	if (tileoff.y) ++gridvis.cy;

	// If not at the right col or bottom row, overdraw by 1 tile.
	// This enables cheap partial tile rendering; the clipping region will clean it up.
	if ((gridanchor.x + gridvis.cx) < m_mapdims.cx)
		++gridvis.cx;
	if ((gridanchor.y + gridvis.cy) < m_mapdims.cy)
		++gridvis.cy;

	bool room = m_showrooms.GetCheck() == 1;
	copt = gridanchor;
	for (coY = 0, pt.y = displayarea.top; coY < gridvis.cy; coY++, pt.y += tiledims.cy, copt.y += 1) {
		for (coX = 0, pt.x = displayarea.left, copt.x = gridanchor.x; coX < gridvis.cx; coX++, pt.x += tiledims.cx, copt.x += 1) {
			if (!(mousedown && cur_tool && PtInRect(rcTemp, copt))) {
				cart->GetStandardTiles(cur_map, room).Draw(&dc, DecompressedMap[copt.y][copt.x], pt, ILD_NORMAL);
			}
		}
	}

	if (mousedown) {
		switch (cur_tool) {
		case 0: break;
		case 1: {			//fill
			coY = rcTemp.top - gridanchor.y;
			coX = rcTemp.left - gridanchor.x;
			copt.y = rcTemp.bottom - gridanchor.y;
			copt.x = rcTemp.right - gridanchor.x;
			tile = coX;
			for (; coY < copt.y; coY++) {
				for (coX = tile; coX < copt.x; coX++) {
					pt.x = displayarea.left + (coX << 4); pt.y = displayarea.top + (coY << 4);
					cart->m_overworldtiles.Draw(&dc, cur_tile, pt, ILD_NORMAL); //TODO - this is a bug, use standard tiles instead
					//	add an issue to address this so it can be checked in testing
					//	it should do the following instead of using the overworld tiles:
					//cart->GetStandardTiles(cur_map, room).Draw(&dc, cur_tile, pt, ILD_NORMAL);
				}
			}
		}break;
		default: {
			// rcTemp is in map coords, translate to pixels
			auto& br = toolBrush;
			rcTemp.left = ((rcTemp.left - gridanchor.x) * tiledims.cx) + displayarea.left;
			rcTemp.right = ((rcTemp.right - gridanchor.x) * tiledims.cx) + displayarea.left;
			rcTemp.top = ((rcTemp.top - gridanchor.y) * tiledims.cy) + displayarea.top;
			rcTemp.bottom = ((rcTemp.bottom - gridanchor.y) * tiledims.cy) + displayarea.top;
			dc.FillRect(rcTemp, &br);
		}break;
		}
	}

	// Draw the sprites
	CRect rcscreen(0,0,16,16); // tile coords, not pixels
	for(coX = 0; coX < MAPSPRITE_COUNT; coX++){
		// Draw the sprite if it's defined, matches the current room setting, and is in the display rect.
		if(!Sprite_Value[coX]) continue;
		if(room != Sprite_InRoom[coX]) continue;

		pt.x = ((Sprite_Coords[coX].x - gridanchor.x) * tiledims.cx) + displayarea.left;
		pt.y = ((Sprite_Coords[coX].y - gridanchor.y) * tiledims.cy) + displayarea.top;
		if(!PtInRect(displayarea, pt)) continue;

		m_sprites.Draw(&dc,cart->ROM[MAPSPRITE_PICASSIGNMENT + Sprite_Value[coX]],pt,ILD_TRANSPARENT);
	}

	if (cart->session.showLastClick) {
		// Draw the rect with our pen if it's in the display rect.
		pt.x = ((ptLastClick.x - gridanchor.x) * tiledims.cx) + displayarea.left;
		pt.y = ((ptLastClick.y - gridanchor.y) * tiledims.cy) + displayarea.top;
		if (PtInRect(displayarea, pt)) {
			auto oldpen = dc.SelectObject(&redpen);
			dc.MoveTo(pt); pt.x += tiledims.cx;
			dc.LineTo(pt); pt.y += tiledims.cy;
			dc.LineTo(pt); pt.x -= tiledims.cx;
			dc.LineTo(pt); pt.y -= tiledims.cy;
			dc.LineTo(pt);
			dc.SelectObject(oldpen);
		}
	}
}

void CMaps::sync_map_positions(bool popin)
{
	ASSERT(m_popoutcreated);
	if (!m_popoutcreated) return;

	auto embpos = ScrollOffset;
	auto poppos = m_popoutmap.GetScrollOffset();
	auto emblim = get_scroll_limits();
	auto poplim = m_popoutmap.GetScrollLimits();
	if (popin) {
		ASSERT(!m_popoutmap.IsWindowVisible());
		// popout map to embedded
		POINTF newoff = {
			poppos.x * emblim.cx / (float)poplim.cx,
			poppos.y * emblim.cy / (float)poplim.cy
		};
		handle_hscroll(SB_THUMBTRACK, (int)std::round(newoff.x), nullptr);
		handle_vscroll(SB_THUMBTRACK, (int)std::round(newoff.y), nullptr);
	}
	else {
		ASSERT(m_popoutmap.IsWindowVisible());
		// embedded map to popout
		POINTF newoff = {
			embpos.x * poplim.cx / (float)emblim.cx,
			embpos.y * poplim.cy / (float)emblim.cy
		};
		m_popoutmap.ScrollToPos(SB_HORZ, (int)std::round(newoff.x), ForceCenteringOnMapSwitch);
		m_popoutmap.ScrollToPos(SB_VERT, (int)std::round(newoff.y), ForceCenteringOnMapSwitch);
	}
}

void CMaps::OnBnClickedButtonPopout()
{
	PopMapDialog(false);
}

LRESULT CMaps::OnDrawToolBnClick(WPARAM wparam, LPARAM lparam)
{
	UNREFERENCED_PARAMETER(wparam);
	cur_tool = (int)lparam;
	m_customtool.EnableWindow(cur_tool > 1);
	return 0;
}

LRESULT CMaps::OnShowFloatMap(WPARAM wparam, LPARAM lparam)
{
	UNREFERENCED_PARAMETER(lparam);
	bool in = (wparam == 0);

	if (in) {
		// We know the tool index, just need to check the corresponding button.
		auto iter = std::find_if(cbegin(m_toolbuttons), cend(m_toolbuttons),
			[this](const CDrawingToolButton* btn) { return btn->GetToolIndex() == cur_tool; });
		if (iter != cend(m_toolbuttons)) {
			CheckRadioButton(m_penbutton.GetDlgCtrlID(), m_custom2button.GetDlgCtrlID(),
				(*iter)->GetDlgCtrlID());
		}
	}

	PopMapDialog(in);
	return 0;
}