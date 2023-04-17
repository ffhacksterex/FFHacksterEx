// OverworldMap.cpp : implementation file
// new implementation

#include "stdafx.h"
#include "OverworldMap.h"
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
// COverworldMap dialog

COverworldMap::COverworldMap(CWnd* pParent /*= nullptr */)
	: CEditorWithBackground(COverworldMap::IDD, pParent)
{
}

void COverworldMap::DoDataExchange(CDataExchange* pDX)
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
	DDX_Control(pDX, IDC_STATIC_BACKDROP, m_backdropstatic);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
	DDX_Control(pDX, IDC_OW_LAND_ENCOUNTERRATE, m_landencrateedit);
	DDX_Control(pDX, IDC_OW_SEA_ENCOUNTERRATE2, m_seaencrateedit);
	DDX_Control(pDX, IDC_BTN_DRAWTOOLS_PEN, m_penbutton);
	DDX_Control(pDX, IDC_BTN_DRAWTOOLS_BLOCK, m_blockbutton);
	DDX_Control(pDX, IDC_BTN_DRAWTOOLS_CUSTOM1, m_custom1button);
	DDX_Control(pDX, IDC_BTN_DRAWTOOLS_CUSTOM2, m_custom2button);
	DDX_Control(pDX, IDC_BTN_DRAWTOOLS_CUSTOM3, m_custom3button);
	DDX_Control(pDX, IDC_STATIC_INFRA_MAPPANEL, m_mappanel);
	DDX_Control(pDX, IDC_BUTTON_POPOUT, m_popoutbutton);
}


BEGIN_MESSAGE_MAP(COverworldMap, CEditorWithBackground)
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
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_MAP, &COverworldMap::OnBnClickedButtonImportMap)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_MAP, &COverworldMap::OnBnClickedButtonExportMap)
	ON_MESSAGE(WMA_DRAWTOOLBNCLICK, OnDrawToolBnClick)
	ON_BN_CLICKED(IDC_BUTTON_POPOUT, &COverworldMap::OnBnClickedButtonPopout)
	ON_MESSAGE(WMA_SHOWFLOATINGMAP, &COverworldMap::OnShowFloatMap)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COverworldMap message handlers


void COverworldMap::DoHScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
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

void COverworldMap::DoVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	if (m_popoutcreated) {
		//N.B. - see the note in OnVHScroll, similar logic here.
		int iPos = ((INT)nPos);
		int newval = (iPos + 8) * 100 / 255;
		m_popoutmap.ScrollByPercentage(SB_VERT, newval);
	}
	handle_vscroll(nSBCode, nPos * m_tiledims.cy, pScrollBar);
}

void COverworldMap::SendKeydown(WPARAM wparam, LPARAM lparam)
{
	this->PostMessage(WM_KEYDOWN, wparam, lparam);
}

void COverworldMap::HandleLButtonDown(UINT nFlags, CPoint point)
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

void COverworldMap::HandleLButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);

	if (mousedown) {
		rcToolRect.NormalizeRect();
		int coY, coX, temp, co;
		bool draw;
		switch (cur_tool) {
		case 0:
			invalidate_maps();
			break;
		case 1: {			//fill
			for (coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++) {
				for (coX = rcToolRect.left; coX <= rcToolRect.right; coX++)
					DecompressedMap[coY][coX] = (BYTE)cur_tile;
			}
			invalidate_maps();
		}break;
		default: {			//smarttools
			temp = cur_tool - 2;
			//flood fill
			for (coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++) {
				for (coX = rcToolRect.left; coX <= rcToolRect.right; coX++)
					DecompressedMap[coY][coX] = cart->SmartTools[temp][4];
			}
			//"smart" top edge
			coY = rcToolRect.top;
			for (coX = rcToolRect.left; coX <= rcToolRect.right; coX++) {
				draw = 1;
				if (coY) {
					for (co = 0; co < 6 && draw; co++) {
						if (DecompressedMap[coY - 1][coX] == cart->SmartTools[temp][co]) draw = 0;
					}
				}
				if (draw) DecompressedMap[coY][coX] = cart->SmartTools[temp][1];
			}
			//"smart" bottom edge
			coY = rcToolRect.bottom;
			for (coX = rcToolRect.left; coX <= rcToolRect.right; coX++) {
				draw = 1;
				if (coY < 255) {
					for (co = 3; co < 9 && draw; co++) {
						if (DecompressedMap[coY + 1][coX] == cart->SmartTools[temp][co]) draw = 0;
					}
				}
				if (draw) DecompressedMap[coY][coX] = cart->SmartTools[temp][7];
			}
			//"smart" left edge
			coX = rcToolRect.left;
			for (coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++) {
				draw = 1;
				if (coX) {
					for (co = 0; co < 8 && draw; co++) {
						if (co % 3 == 2) co++;
						if (DecompressedMap[coY][coX - 1] == cart->SmartTools[temp][co]) draw = 0;
					}
				}
				if (draw) DecompressedMap[coY][coX] = cart->SmartTools[temp][3];
			}
			//"smart" right edge
			coX = rcToolRect.right;
			for (coY = rcToolRect.top; coY <= rcToolRect.bottom; coY++) {
				draw = 1;
				if (coX < 255) {
					for (co = 1; co < 9 && draw; co++) {
						if (co % 3 == 0) co++;
						if (DecompressedMap[coY][coX + 1] == cart->SmartTools[temp][co]) draw = 0;
					}
				}
				if (draw) DecompressedMap[coY][coX] = cart->SmartTools[temp][5];
			}
			//"smart" NW corner
			co = 0;
			draw = 1;
			if (rcToolRect.left) {
				coY = DecompressedMap[rcToolRect.top][rcToolRect.left - 1];
				for (coX = 0; draw && coX < 8; coX++) {
					if (coX % 3 == 2) coX++;
					if (coY == cart->SmartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) co = 1;
			draw = 1;
			if (rcToolRect.top) {
				coY = DecompressedMap[rcToolRect.top - 1][rcToolRect.left];
				for (coX = 0; draw && coX < 6; coX++) {
					if (coY == cart->SmartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) {
				if (co == 1) co = 4;
				else co = 3;
			}
			DecompressedMap[rcToolRect.top][rcToolRect.left] = cart->SmartTools[temp][co];
			//"smart" SW corner
			co = 6;
			draw = 1;
			if (rcToolRect.left) {
				coY = DecompressedMap[rcToolRect.bottom][rcToolRect.left - 1];
				for (coX = 0; draw && coX < 8; coX++) {
					if (coX % 3 == 2) coX++;
					if (coY == cart->SmartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) co = 7;
			draw = 1;
			if (rcToolRect.bottom < 255) {
				coY = DecompressedMap[rcToolRect.bottom + 1][rcToolRect.left];
				for (coX = 3; draw && coX < 9; coX++) {
					if (coY == cart->SmartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) {
				if (co == 7) co = 4;
				else co = 3;
			}
			DecompressedMap[rcToolRect.bottom][rcToolRect.left] = cart->SmartTools[temp][co];
			//"smart" NE corner
			co = 2;
			draw = 1;
			if (rcToolRect.right < 255) {
				coY = DecompressedMap[rcToolRect.top][rcToolRect.right + 1];
				for (coX = 1; draw && coX < 9; coX++) {
					if (coX % 3 == 0) coX++;
					if (coY == cart->SmartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) co = 1;
			draw = 1;
			if (rcToolRect.top) {
				coY = DecompressedMap[rcToolRect.top - 1][rcToolRect.right];
				for (coX = 0; draw && coX < 6; coX++) {
					if (coY == cart->SmartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) {
				if (co == 1) co = 4;
				else co = 5;
			}
			DecompressedMap[rcToolRect.top][rcToolRect.right] = cart->SmartTools[temp][co];
			//"smart" SE corner
			co = 8;
			draw = 1;
			if (rcToolRect.right < 255) {
				coY = DecompressedMap[rcToolRect.bottom][rcToolRect.right + 1];
				for (coX = 1; draw && coX < 9; coX++) {
					if (coX % 3 == 0) coX++;
					if (coY == cart->SmartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) co = 7;
			draw = 1;
			if (rcToolRect.bottom < 255) {
				coY = DecompressedMap[rcToolRect.bottom + 1][rcToolRect.right];
				for (coX = 3; draw && coX < 9; coX++) {
					if (coY == cart->SmartTools[temp][coX]) draw = 0;
				}
			}
			if (!draw) {
				if (co == 7) co = 4;
				else co = 5;
			}
			DecompressedMap[rcToolRect.bottom][rcToolRect.right] = cart->SmartTools[temp][co];

			invalidate_maps();
		}break;
		}
		if (m_minimap.GetCheck()) minimap.UpdateCur();
		OnFindKAB();
	}
}

void COverworldMap::HandleLButtonDblClk(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	// By default, does nothing on the map
}

void COverworldMap::HandleRButtonDown(UINT nFlags, CPoint pt)
{
	StoreTileData();
	OnFindKAB();
	cur_tile = DecompressedMap[pt.y][pt.x];
	UpdateClick(pt);
	InvalidateRect(rcTiles, 0);
	LoadTileData();
	if (cur_tool > 1) {
		CheckRadioButton(m_penbutton.GetDlgCtrlID(), m_custom2button.GetDlgCtrlID(),
			m_blockbutton.GetDlgCtrlID());
		cur_tool = m_blockbutton.GetToolIndex();
		m_customizetool.EnableWindow(FALSE);
		m_popoutmap.UpdateControls();
	}
	if (m_showlastclick.GetCheck()) invalidate_maps();

	//if they clicked on a sprite... adjust the Sprite Editor accordingly
	for (int co = 0; co < 5; co++) {
		if (misccoords[co] == pt) {
			mousedown = (BYTE)(co + 2);
			m_misccoords.SetCurSel(co);
			OnSelchangeMisccoords();
			break;
		}
	}
}

void COverworldMap::HandleRButtonUp(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	// By default, does nothing on the map, pass fix_map_point(point) to any handler
}

void COverworldMap::HandleRButtonDblClk(UINT nFlags, CPoint point)
{
	UNREFERENCED_PARAMETER(nFlags);
	auto ref = DecompressedMap[point.y][point.x];
	if (ref != -1)
		apply_tile_tint(ref);
}

void COverworldMap::HandleMouseMove(UINT nFlags, CPoint newhover)
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
			}break;
			default: {		//fill / Smarttools
				rcToolRect.right = ptHover.x;
				rcToolRect.bottom = ptHover.y;
				invalidate_maps();
			}break;
			}
			UpdateClick(ptHover);
			OnFindKAB();
		}
		else if (mousedown) {
			UpdateClick(ptHover);
			OnFindKAB();
			misccoords[mousedown - 2] = ptHover;
			text.Format("%X", ptHover.x); m_miscx.SetWindowText(text);
			text.Format("%X", ptHover.y); m_miscy.SetWindowText(text);
			invalidate_maps();
		}
	}
}

void COverworldMap::HandleAfterScroll(CPoint scrolloffset, CRect displayarea)
{
	UNREFERENCED_PARAMETER(displayarea);
	minimap.UpdateFocusRect(make_minimap_rect(scrolloffset));
}

void COverworldMap::HandleMapImport()
{
	OnMapImport();
}

void COverworldMap::HandleMapExport()
{
	OnMapExport();
}

bool COverworldMap::HandleCustomizeTool()
{
	CCustomTool dlg;
	dlg.dat = cart;
	dlg.m_tiles = &cart->m_overworldtiles;
	dlg.tool = cur_tool - 2;
	auto result = dlg.DoModal();
	return result == IDOK;
}

void COverworldMap::RenderMapEx(CDC& dc, CRect displayarea, CPoint scrolloff, CSize tiledims)
{
	paint_map_elements(dc, displayarea, scrolloff, tiledims);
}

int COverworldMap::GetCurrentToolIndex() const
{
	return cur_tool;
}

void COverworldMap::SetMouseDown(int imousedown)
{
	mousedown = imousedown;
}

int COverworldMap::GetMouseDown() const
{
	return mousedown;
}

void COverworldMap::DoMinimap()
{
	OnMinimap();
}

void COverworldMap::DoViewcoords()
{
	OnViewcoords();
}

void COverworldMap::LoadOffsets()
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

void COverworldMap::LoadRom()
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

void COverworldMap::SaveRom()
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

BOOL COverworldMap::OnInitDialog()
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

		m_toolbuttons = { &m_penbutton, &m_blockbutton, &m_custom1button, &m_custom2button, &m_custom3button };
		init_popout_map_window();

		minimap.cart = cart;minimap.Context = Minimap;
		minimap.OVmap = this;
		minimap.Create(IDD_MINIMAP, this);
		minimap.ShowWindow(0);
		minimap.Map = DecompressedMap[0];
		minimap.palette = palette[0];
		minimap.SetFocusRect(0, 0, 16, 16); // default, will change when invoked

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

		m_gridcolor = RGB(0, 0, 255);
		IF_NOHANDLE(redpen).CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		IF_NOHANDLE(toolBrush).CreateSolidBrush(RGB(128, 64, 255));

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

		GetControlRect(&m_palettestatic, &rcpos);
		rcPalette.SetRect(0, 0, 256, 32);
		rcPalette.OffsetRect(rcpos.left + 4, rcpos.top + 4);

		m_penbutton.SetCheck(BST_CHECKED);
		cur_tool = m_penbutton.GetToolIndex();

		auto maxes = calc_scroll_maximums();
		m_hscroll.SetScrollRange(0, maxes.cx);
		m_vscroll.SetScrollRange(0, maxes.cy);
		ScrollOffset.x = 0;
		ScrollOffset.y = 0;

		probabilitychanged = 1;
		ptHover.x = -1;
		ptLastClick.x = -1;
		ptLastClick.y = 0;
		ptDomain.x = 0;
		ptDomain.y = 0;

		build_domain_rects();
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

BOOL COverworldMap::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
	case WM_KEYDOWN:
		switch (pMsg->wParam) {
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
			break;
		}
		break;
	}
	return CEditorWithBackground::PreTranslateMessage(pMsg);
}

void COverworldMap::OnCancel()
{
	if (m_popoutcreated && m_popoutmap.IsWindowVisible()) {
		m_popoutmap.PostMessage(WM_KEYDOWN, VK_ESCAPE);
		return;
	}
	CEditorWithBackground::OnCancel();
}

void COverworldMap::LoadTileData()
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

void COverworldMap::StoreTileData()
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

void COverworldMap::DecompressMap()
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

void COverworldMap::ReloadGraphics()
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

void COverworldMap::ReloadSpriteGraphics(CProgressCtrl* m_prog)
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

void COverworldMap::InitBackdrops()
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

void COverworldMap::build_domain_rects()
{
	// We'll store the domain rectangles.
	// The popout map can display more than one domain
	// if it's large enough.
	m_domainrects.clear();
	CSize sizeofdomain = {
		m_mapsize.cx / m_domaincounts.cx * m_tiledims.cx,
		m_mapsize.cy / m_domaincounts.cy * m_tiledims.cy
	};
	for (int row = 0; row < m_domaincounts.cy; ++row)
	{
		CRect rcdomain{ 0, row * sizeofdomain.cy, sizeofdomain.cx, sizeofdomain.cy };
		for (int col = 0; col < m_domaincounts.cx; ++col)
		{
			m_domainrects.push_back(rcdomain);
			rcdomain.OffsetRect(sizeofdomain.cx, 0);
		}
	}
}

void COverworldMap::invalidate_maps()
{
	InvalidateRect(rcMap, 0);
	m_popoutmap.InvalidateMap();
}

void COverworldMap::handle_hscroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

	if (m_minimap.GetCheck()) {
		if (!m_popoutmap.IsWindowVisible()) {
			// No adjustment needed, popout and embedded maps are never shown at the same time
			minimap.UpdateFocusRect(make_minimap_rect(ScrollOffset));
		}
	}
	invalidate_maps();
}

void COverworldMap::handle_vscroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UNREFERENCED_PARAMETER(pScrollBar);

	bool controlDown = IsKeyDown(VK_CONTROL);
	switch (nSBCode) {
	case SB_LINEUP: ScrollOffset.y -= 1 * Ui::MultiplyIf(controlDown, 8); break;
	case SB_LINEDOWN: ScrollOffset.y += 1 * Ui::MultiplyIf(controlDown, 8); break;
	case SB_PAGEUP: ScrollOffset.y -= 16 * Ui::MultiplyIf(controlDown, 2); break;
	case SB_PAGEDOWN: ScrollOffset.y += 16 * Ui::MultiplyIf(controlDown, 2); break;
	case SB_THUMBTRACK: ScrollOffset.y = nPos; break;
	}
	auto maxes = calc_scroll_maximums();
	if (ScrollOffset.y < 0) ScrollOffset.y = 0;
	if (ScrollOffset.y > maxes.cy) ScrollOffset.y = maxes.cy;

	m_vscroll.SetScrollPos(ScrollOffset.y);

	if (m_minimap.GetCheck()) {
		if (!m_popoutmap.IsWindowVisible()) {
			// No adjustment needed, popout and embedded maps are never shown at the same time
			minimap.UpdateFocusRect(make_minimap_rect(ScrollOffset));
		}
	}
	invalidate_maps();
}

void COverworldMap::OnPaint()
{
	CPaintDC dc(this);
	handle_paint(dc);
}

void COverworldMap::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	handle_hscroll(nSBCode, nPos, pScrollBar);
}

void COverworldMap::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	handle_vscroll(nSBCode, nPos, pScrollBar);
}

void COverworldMap::OnLButtonDown(UINT nFlags, CPoint pt)
{
	if(PtInRect(rcMap,pt)){
		HandleLButtonDown(nFlags, fix_map_point(pt));
	}
	else if(PtInRect(rcTiles,pt)){
		StoreTileData();
		pt.x = (pt.x - rcTiles.left) >> 4;
		pt.y = (pt.y - rcTiles.top) & 0xF0;
		cur_tile = pt.y + pt.x;
		InvalidateRect(rcTiles,0);
		LoadTileData();
	}
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
			InvalidateRect(rcPalette,0);
			invalidate_maps();
		}
	}
	else {
		HandleLbuttonDrag(this);
	}
}

void COverworldMap::OnLButtonUp(UINT nFlags, CPoint point)
{
	HandleLButtonUp(nFlags, fix_map_point(point));
	mousedown = 0;
}

void COverworldMap::OnLButtonDblClk(UINT nFlags, CPoint pt)
{
	UNREFERENCED_PARAMETER(nFlags);

	if (PtInRect(rcTiles, pt)) {
		CTileEdit dlg;
		dlg.Invoker = CTileEdit::Overworld;
		dlg.cart = cart;
		dlg.tileset = 0;
		dlg.tile = (BYTE)cur_tile;
		dlg.pal[0] = palette[0];
		OnSave();
		if (dlg.DoModal() == IDOK) {
			minimap.PalAssign[cur_tile] = cart->ROM[OVERWORLDPALETTE_ASSIGNMENT + cur_tile] & 3;
			if (m_minimap.GetCheck()) minimap.UpdateAll();
			cart->m_overworldtiles.DeleteImageList();
			cart->OK_overworldtiles = 0;
			ReloadGraphics();
			InvalidateRect(rcPalette, 0);
			InvalidateRect(rcTiles, 0);
			invalidate_maps();
		}
	}
}

void COverworldMap::OnRButtonDown(UINT nFlags, CPoint pt)
{
	mousedown = 0;
	if (PtInRect(rcTiles, pt)) {
		OnLButtonDown(nFlags, pt);
	}
	else if (PtInRect(rcMap, pt)) {
		HandleRButtonDown(nFlags, fix_map_point(pt));
	}
}

void COverworldMap::OnRButtonUp(UINT nFlags, CPoint point)
{
	HandleRButtonUp(nFlags, fix_map_point(point));
	mousedown = (BYTE)0;
}

void COverworldMap::OnRButtonDblClk(UINT nFlags, CPoint pt)
{
	int ref = -1;
	if (PtInRect(rcMap, pt)) {
		HandleRButtonDblClk(nFlags, fix_map_point(pt));
	}
	else if (PtInRect(rcTiles, pt)) {
		pt.x = (pt.x - rcTiles.left) >> 4;
		pt.y = (pt.y - rcTiles.top) & 0xF0;
		ref = pt.x + pt.y;
	}

	if (ref != -1) {
		apply_tile_tint(ref);
	}
}

void COverworldMap::OnMouseMove(UINT nFlags, CPoint pt)
{
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

void COverworldMap::OnEditlabel()
{
	int temp = m_backdroplist.GetCurSel();
	ChangeLabel(*cart, -1, LoadBackdropLabel(*cart, temp), WriteBackdropLabel, temp, nullptr, &m_backdroplist);
}

void COverworldMap::OnSelchangeBackdrop() 
{InvalidateRect(rcBackdrop);}

void COverworldMap::UpdateClick(CPoint pt)
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

void COverworldMap::LoadDomain()
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

void COverworldMap::StoreDomain()
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

void COverworldMap::OnChangeProbability() 
{probabilitychanged = 1;}

void COverworldMap::OnFightNone() 
{UpdateFight(0);}
void COverworldMap::OnFightNormal() 
{UpdateFight(1);}
void COverworldMap::OnFightOcean() 
{UpdateFight(2);}
void COverworldMap::OnFightRiver() 
{UpdateFight(3);}
void COverworldMap::UpdateFight(int update)
{
	m_fight_none.SetCheck(update == 0);
	m_fight_normal.SetCheck(update == 1);
	m_fight_ocean.SetCheck(update == 2);
	m_fight_river.SetCheck(update == 3);
}
void COverworldMap::OnCaravan() 
{UpdateMisc(m_caravan.GetCheck());}
void COverworldMap::OnChime() 
{UpdateMisc(m_chime.GetCheck() << 1);}
void COverworldMap::OnRaiseairship() 
{UpdateMisc(m_raiseairship.GetCheck() << 2);}
void COverworldMap::UpdateMisc(int update)
{
	m_caravan.SetCheck(update == 1);
	m_chime.SetCheck(update == 2);
	m_raiseairship.SetCheck(update == 4);
}
void COverworldMap::OnShowlastclick()
{
	cart->ShowLastClick = (m_showlastclick.GetCheck() != 0);
	invalidate_maps();
}
void COverworldMap::OnDrawgrid()
{
	cart->DrawDomainGrid = (m_drawgrid.GetCheck() != 0);
	invalidate_maps();
}
void COverworldMap::OnTeleport() 
{
	bool teleport = m_teleport.GetCheck() != 0;
	m_fight_none.EnableWindow(!teleport);
	m_fight_normal.EnableWindow(!teleport);
	m_fight_river.EnableWindow(!teleport);
	m_fight_ocean.EnableWindow(!teleport);
	m_teleportbox.ShowWindow(teleport);
}

void COverworldMap::OnFindKAB() 
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

void COverworldMap::OnCustomizetool()
{
	HandleCustomizeTool();
}

void COverworldMap::CompressMap()
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

void COverworldMap::OnEditgraphics()
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

void COverworldMap::OnSelchangeMisccoords() 
{
	CString text;
	text.Format("%X",misccoords[m_misccoords.GetCurSel()].x);
	m_miscx.SetWindowText(text);
	text.Format("%X",misccoords[m_misccoords.GetCurSel()].y);
	m_miscy.SetWindowText(text);
}

void COverworldMap::StoreMiscCoords()
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

void COverworldMap::init_popout_map_window()
{
	if (m_popoutmap.CreateModeless(this, { 256,256 }, { 16,16 }, this))
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

void COverworldMap::PopMapDialog(bool in)
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

	// The focus rect size depends on which map is visible.
	BOOL popvis = m_popoutmap.IsWindowVisible();
	auto scroff = popvis ? m_popoutmap.GetScrollOffset() : ScrollOffset;
	auto minidims = popvis ? m_popoutmap.GetMiniMapDims() : m_minmapsize;
	minimap.UpdateFocusRect(make_minimap_rect(scroff));

	std::vector<UINT> ids{ IDHELPBOOK, IDCANCEL2, IDC_SAVE, IDOK, IDCANCEL };
	int slide = -diff;
	for (const auto& id : ids) {
		auto pwnd = GetDlgItem(id);
		Ui::MoveControlBy(pwnd, slide, 0);
	}
}

CPoint COverworldMap::fix_map_point(CPoint point)
{
	CPoint fixedpt;
	fixedpt.x = ((point.x - rcMap.left) + ScrollOffset.x) / 16; //TODO - display tile dims are hardcoded here as 16x16
	fixedpt.y = ((point.y - rcMap.top) + ScrollOffset.y) / 16;
	return fixedpt;
}

CRect COverworldMap::make_minimap_rect(CPoint point)
{
	// Here, the minimap rect position is base on tiles.
	// Its size is based on displayarea size.
	BOOL popvis = m_popoutmap.IsWindowVisible();
	auto mapdims = popvis ? m_popoutmap.GetMiniMapDims() : m_minmapsize;
	auto tiledims = popvis ? m_popoutmap.GetTileDims() : m_tiledims;
	CRect rc;
	rc.left = point.x / tiledims.cx;
	rc.right = rc.left + mapdims.cx;
	rc.top = point.y / tiledims.cy;
	rc.bottom = rc.top + mapdims.cy;
	return rc;
}

CSize COverworldMap::calc_scroll_maximums()
{
	//TODO - hardcoded for now, change to a calculation...
	return { 240 * m_tiledims.cx, 240 * m_tiledims.cy };
}

CSize COverworldMap::get_scroll_limits()
{
	return CSize{
		m_hscroll.GetScrollLimit(),
		m_vscroll.GetScrollLimit()
	};
}

void COverworldMap::apply_tile_tint(int ref)
{
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
		invalidate_maps();
	}
}

CRect COverworldMap::get_display_area()
{
	return rcMap;
}

void COverworldMap::handle_paint(CDC& dc)
{
	CPen* origpen = dc.SelectObject(&redpen);
	int coX, coY, tile;
	CPoint pt;

	//Draw the Source Tiles on the screen (8 rows of 16 tiles)
	for (coY = 0, tile = 0, pt.y = rcTiles.top; coY < 0x08; coY++, pt.y += 16) {
		for (coX = 0, pt.x = rcTiles.left; coX < 0x10; coX++, pt.x += 16, tile++)
			cart->m_overworldtiles.Draw(&dc, tile, pt, ILD_NORMAL);
	}
	pt.x = ((cur_tile & 0x0F) << 4) + rcTiles.left;
	pt.y = (cur_tile & 0xF0) + rcTiles.top;
	dc.MoveTo(pt); pt.x += 15;
	dc.LineTo(pt); pt.y += 15;
	dc.LineTo(pt); pt.x -= 15;
	dc.LineTo(pt); pt.y -= 15;
	dc.LineTo(pt);

	{
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

	//Draw the backdrop
	pt.x = rcBackdrop.left;
	pt.y = rcBackdrop.top;
	tile = m_backdroplist.GetCurSel();
	m_backdrop.Draw(&dc, tile, pt, ILD_NORMAL); pt.x += 64;
	m_backdrop.Draw(&dc, tile, pt, ILD_NORMAL);

	//Draw the palette!
	CBrush br;
	CRect rc(0, 0, 16, 16);
	rc = rcPalette;
	rc.right = rc.left + 16; rc.bottom = rc.top + 16;
	for (pt.y = 0; pt.y < 2; pt.y += 1, rc.top += 16, rc.bottom += 16, rc.left -= 256, rc.right -= 256) {
		for (pt.x = 0; pt.x < 16; pt.x++, rc.left += 16, rc.right += 16) {
			br.CreateSolidBrush(cart->Palette[0][palette[pt.y][pt.x]]);
			dc.FillRect(rc, &br);
			br.DeleteObject();
		}
	}

	dc.SelectObject(origpen);
	m_banner.Render(dc, 8, 8);
}

void COverworldMap::paint_map_elements(CDC& dc, CRect displayrect, CPoint scrolloff, CSize tiledims)
{
	ASSERT(tiledims.cx > 0);
	ASSERT(tiledims.cy > 0);
	if (tiledims.cx <= 0 || tiledims.cy <= 0) return;

	CRect displayarea = { 0, 0, displayrect.Width(), displayrect.Height() };
	CSize gridvis = {displayarea.Width() / tiledims.cx, displayarea.Height() / tiledims.cy};
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
	if ((gridanchor.x + gridvis.cx) < m_mapsize.cx)
		++gridvis.cx;
	if ((gridanchor.y + gridvis.cy) < m_mapsize.cy)
		++gridvis.cy;

	copt = gridanchor;
	for (coY = 0, pt.y = displayarea.top; coY < gridvis.cy; coY++, pt.y += tiledims.cy, copt.y += 1) {
		for (coX = 0, pt.x = displayarea.left, copt.x = gridanchor.x; coX < gridvis.cx; coX++, pt.x += tiledims.cx, copt.x += 1) {
			if (!(mousedown && cur_tool && PtInRect(rcTemp, copt))) {
				cart->m_overworldtiles.Draw(&dc, DecompressedMap[copt.y][copt.x], pt, ILD_NORMAL);
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
					cart->m_overworldtiles.Draw(&dc, cur_tile, pt, ILD_NORMAL);
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
	if (cart->ShowLastClick) {
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
	if (cart->DrawDomainGrid) {
		auto oldbkmode = dc.SetBkMode(TRANSPARENT);
		auto oldtxcolor = dc.SetTextColor(m_gridcolor);
		CString text;

		// Create a translucent square behind each domain label.
		BLENDFUNCTION bf = { 0 };
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.AlphaFormat = 0; //N.B. - DO NOT set this to AC_SRC_ALPHA in this case
		bf.SourceConstantAlpha = 192;
		CRect rcblend{ 0, 0, m_tiledims.cx * 2, m_tiledims.cy };
		CDC dcblend;
		CBitmap bmblend;
		dcblend.CreateCompatibleDC(&dc);
		bmblend.CreateCompatibleBitmap(&dc, rcblend.Width(), rcblend.Height());
		dcblend.SelectObject(&bmblend);
		dcblend.FillSolidRect(&rcblend, COLOR_WHITE);

		CPoint ptmouse = Ui::GetControlCursorPos(this);
		ptmouse.Offset(-displayrect.left, -displayrect.top);

		for (size_t irc = 0, row = 0; row < m_domaincounts.cy; ++row, +irc) {
			for (size_t col = 0; col < m_domaincounts.cx; ++col, ++irc) {
				// Translate to displayarea coordinates
				CRect rcdomain = m_domainrects[irc];
				rcdomain.OffsetRect(-scrolloff.x, -scrolloff.y);

				// Draw the rectangle ...
				dc.Draw3dRect(rcdomain, m_gridcolor, m_gridcolor);

				// ... but only render text if not currently drawing under it.
				CRect rcextent{ rcdomain.TopLeft(), m_tiledims };
				rcextent.InflateRect(0, 0, m_tiledims.cx, 0);
				BOOL undermouse = rcextent.PtInRect(ptmouse);
				if (!(mousedown && undermouse)) {
					dc.AlphaBlend(rcextent.left, rcextent.top, rcextent.Width(), rcextent.Height(),
						&dcblend, 0, 0, rcblend.Width(), rcblend.Height(), bf);
					text.Format(_T("%d,%d"), col, row);
					dc.DrawText(text, rcextent, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
			}
		}
		dc.SetTextColor(oldtxcolor);
		dc.SetBkMode(oldbkmode);
	}

	//Draw the sprites
	CRect rc(0, 0, gridvis.cx, gridvis.cy);
	for (coX = 0; coX < 5; coX++) {
		pt.x = misccoords[coX].x - gridanchor.x;
		pt.y = misccoords[coX].y - gridanchor.y;
		if (!PtInRect(rc, pt)) continue;
		pt.x = (pt.x * tiledims.cx) + displayarea.left;
		pt.y = (pt.y * tiledims.cy) + displayarea.top;
		m_sprites.Draw(&dc, coX, pt, ILD_TRANSPARENT);
	}
}

void COverworldMap::sync_map_positions(bool popin)
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
			poppos.x* emblim.cx / (float)poplim.cx,
			poppos.y* emblim.cy / (float)poplim.cy
		};
		handle_hscroll(SB_THUMBTRACK, (int)std::round(newoff.x), nullptr);
		handle_vscroll(SB_THUMBTRACK, (int)std::round(newoff.y), nullptr);
	}
	else {
		ASSERT(m_popoutmap.IsWindowVisible());
		// embedded map to popout
		POINTF newoff = {
			embpos.x* poplim.cx / (float)emblim.cx,
			embpos.y* poplim.cy / (float)emblim.cy
		};
		m_popoutmap.ScrollToPos(SB_HORZ, (int)std::round(newoff.x), ForceCenteringOnMapSwitch);
		m_popoutmap.ScrollToPos(SB_VERT, (int)std::round(newoff.y), ForceCenteringOnMapSwitch);
	}
}

bool COverworldMap::DoSave()
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

void COverworldMap::OnChangeMiscx() 
{
	CString text; int number;
	m_miscx.GetWindowText(text); number = StringToInt_HEX(text);
	if(number > 0xFF) number = 0xFF;
	misccoords[m_misccoords.GetCurSel()].x = number;
	invalidate_maps();
}

void COverworldMap::OnChangeMiscy() 
{
	CString text; int number;
	m_miscy.GetWindowText(text); number = StringToInt_HEX(text);
	if(number > 0xFF) number = 0xFF;
	misccoords[m_misccoords.GetCurSel()].y = number;
	invalidate_maps();
}

void COverworldMap::OnMapExport() 
{
	CString text = "Overworld Map." + CString(FFH_MAP_EXT);
	CString filename = Paths::Combine({ FOLDERPREF(Project->AppSettings,PrefMapImportExportFolder), text });
	auto result = Ui::SaveFilePromptExt(this, FFH_MAP_FILTER, FFH_MAP_EXT, "Export Overworld Map", filename);
	if (!result) return;

	FILE* file = fopen(result.value, "w+b");
	if(file == nullptr){
		AfxMessageBox("Error saving map", MB_ICONERROR);
		return;}
	fwrite(DecompressedMap,1,0x10000,file);
	fclose(file);
}

void COverworldMap::OnMapImport() 
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
	invalidate_maps();
}

void COverworldMap::OnMinimap()
{
	if (m_minimap.GetCheck()) {
		// The focus rect size depends on which map is visible.
		BOOL popvis = m_popoutmap.IsWindowVisible();
		auto scroff = popvis ? m_popoutmap.GetScrollOffset() : ScrollOffset;
		auto minidims = popvis ? m_popoutmap.GetMiniMapDims() : m_minmapsize;
		minimap.SetFocusRect(make_minimap_rect(scroff));
	}
	minimap.ShowWindow(m_minimap.GetCheck());
}

void COverworldMap::OnViewcoords()
{
	coord_dlg.ShowWindow(m_viewcoords.GetCheck());
}

void COverworldMap::UpdateTeleportLabel(int arid)
{
	int temp = m_teleportbox.GetCurSel();
	m_teleportbox.DeleteString(arid);
	m_teleportbox.InsertString(arid, LoadONTeleportLabel(*cart, arid).name);
	m_teleportbox.SetCurSel(temp);
}

void COverworldMap::OnSelchangeTeleportbox()
{
	if(!coord_dlg.m_mouseclick.GetCheck()){
		coord_dlg.m_teleportlist.SetCurSel(m_teleportbox.GetCurSel());
		coord_dlg.OnSelchangeTeleportlist();}
}

void COverworldMap::OnBnClickedButtonImportMap()
{
	OnMapImport();
}

void COverworldMap::OnBnClickedButtonExportMap()
{
	OnMapExport();
}

void COverworldMap::OnBnClickedButtonPopout()
{
	PopMapDialog(false);
}

LRESULT COverworldMap::OnDrawToolBnClick(WPARAM wparam, LPARAM lparam)
{
	UNREFERENCED_PARAMETER(wparam);
	cur_tool = (int)lparam;
	m_customizetool.EnableWindow(cur_tool > 1);
	return 0;
}

void COverworldMap::UpdateTeleportLabel(int areaid, int type)
{
	UNREFERENCED_PARAMETER(type);

	this->UpdateTeleportLabel(areaid);
}

void COverworldMap::Cancel(int context)
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

POINT COverworldMap::GetLastClick()
{
	return ptLastClick;
}

int COverworldMap::GetCurMap()
{
	return -1;
}

void COverworldMap::TeleportHere(int mapindex, int x, int y)
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

LRESULT COverworldMap::OnShowFloatMap(WPARAM wparam, LPARAM lparam)
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