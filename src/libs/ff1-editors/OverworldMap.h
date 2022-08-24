#pragma once
// OverworldMap.h : header file

#include <EditorWithBackground.h>
#include "ICoordMap.h"
#include "IMapEditor.h"
#include <vector>
#include "afxwin.h"
#include <DrawingToolButton.h>
#include <DrawingToolButton.h>
#include <DlgPopoutMap.h>
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// COverworldMap dialog

class COverworldMap : public CEditorWithBackground,
					public ICoordMap,
					public IMapEditor
{
	// Construction
public:
	COverworldMap(CWnd* pParent = nullptr);   // standard constructor

	enum CancelContext { Coords, Minimap };

	bool BootToTeleportFollowup;
	bool ForceCenteringOnMapSwitch = false; //N.B. - for now, this is always false

	LRESULT OnDrawToolBnClick(WPARAM wparam, LPARAM lparam);

	// Inherited via CSaveableDialog
	virtual bool DoSave();

	// Inherited via ICoordMap
	virtual void UpdateTeleportLabel(int areaid, int type) override;
	virtual void Cancel(int context) override;
	virtual POINT GetLastClick() override;
	virtual int GetCurMap() override;
	virtual void TeleportHere(int mapindex, int x, int y) override;
	virtual void DoHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void DoVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void SendKeydown(WPARAM wparam, LPARAM lparam);

	// Inherited from IMapEditor
	virtual void HandleLButtonDown(UINT nFlags, CPoint point);
	virtual void HandleLButtonUp(UINT nFlags, CPoint point);
	virtual void HandleLButtonDblClk(UINT nFlags, CPoint point);
	virtual void HandleRButtonDown(UINT nFlags, CPoint pt);
	virtual void HandleRButtonUp(UINT nFlags, CPoint pt);
	virtual void HandleRButtonDblClk(UINT nFlags, CPoint point);
	virtual void HandleMouseMove(UINT nFlags, CPoint newhover);
	virtual void HandleAfterScroll(CPoint scrolloffset, CRect displayarea);
	virtual void HandleMapImport();
	virtual void HandleMapExport();
	virtual bool HandleCustomizeTool();
	virtual void RenderMapEx(CDC& dc, CRect displayarea, CPoint scrolloff, CSize tiledims);
	virtual int GetCurrentToolIndex() const;
	virtual void SetMouseDown(int imousedown);
	virtual int GetMouseDown() const;

protected:
	CFFHacksterProject* cart = nullptr; //FUTURE - replace cart with Project and remove references to cart
	CImageList m_sprites;
	CImageList m_backdrop;
	CPen redpen;
	CPen bluepen;
	CBrush toolBrush;
	CRect rcTiles;
	CRect rcMap;
	CRect rcBackdrop;
	CRect rcToolRect;
	CRect rcPalette;

	CSubBanner m_banner;
	CDlgPopoutMap m_popoutmap;
	CPoint ptHover;
	CPoint ptLastClick;
	CPoint ptDomain;

	BYTE DecompressedMap[256][256];
	BYTE palette[2][16];
	CPoint ScrollOffset;
	int cur_tile;
	int cur_tool;
	bool probabilitychanged;
	BYTE mousedown;
	int kab;
	CSize m_mapsize = { 256, 256 };
	CSize m_tiledims = { 16,16 };
	CSize m_minmapsize = { 16,16 };
	bool m_firstpopoutdone = false;
	bool m_popoutcreated = false;
	std::vector<CDrawingToolButton*> m_toolbuttons;

	CPoint misccoords[5];

	void UpdateTeleportLabel(int);
	void DoMinimap();
	void DoViewcoords();

	virtual void LoadRom();
	virtual void SaveRom();
	void LoadTileData();
	void StoreTileData();

	void DecompressMap();
	void CompressMap();
	void ReloadGraphics();
	void ReloadSpriteGraphics(CProgressCtrl*);
	void InitBackdrops();
	void UpdateClick(CPoint);
	void LoadDomain();
	void StoreDomain();
	void UpdateFight(int);
	void UpdateMisc(int);
	void StoreMiscCoords();

	void init_popout_map_window();
	void PopMapDialog(bool in);

	CPoint fix_map_point(CPoint point);
	CRect make_minimap_rect(CPoint point);
	CSize calc_scroll_maximums();
	CSize get_scroll_limits();
	void apply_tile_tint(int ref);
	CRect get_display_area();
	void handle_hscroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void handle_vscroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void handle_paint(CDC& dc);
	void paint_map_elements(CDC& dc, CRect displayarea, CPoint scrolloff, CSize tiledims);
	void sync_map_positions(bool popin);

	// Dialog Data
	enum { IDD = IDD_OVERWORLDMAP_NEW };
	CClearEdit m_landencrateedit;
	CClearEdit m_seaencrateedit;
	CButton	m_viewcoords;
	CButton	m_minimap;
	CClearEdit	m_miscy;
	CClearEdit	m_miscx;
	CComboBox	m_misccoords;
	CButton	m_customizetool;
	CStatic	m_kab;
	CComboBox	m_backdroplist;
	CButton	m_showlastclick;
	CButton	m_randomencounter;
	CClearEdit	m_probability8;
	CClearEdit	m_probability7;
	CClearEdit	m_probability6;
	CClearEdit	m_probability5;
	CClearEdit	m_probability4;
	CClearEdit	m_probability3;
	CClearEdit	m_probability2;
	CClearEdit	m_probability1;
	CStatic	m_lastclick;
	CStatic	m_hovering;
	CButton	m_form8;
	CButton	m_form7;
	CButton	m_form6;
	CButton	m_form5;
	CButton	m_form4;
	CButton	m_form3;
	CButton	m_form2;
	CButton	m_form1;
	CButton	m_drawgrid;
	CStatic	m_domain;
	CComboBox	m_battle8;
	CComboBox	m_battle7;
	CComboBox	m_battle6;
	CComboBox	m_battle5;
	CComboBox	m_battle4;
	CComboBox	m_battle3;
	CComboBox	m_battle2;
	CComboBox	m_battle1;
	CButton	m_fight_river;
	CButton	m_fight_ocean;
	CButton	m_fight_normal;
	CButton	m_fight_none;
	CComboBox	m_teleportbox;
	CButton	m_teleport;
	CButton	m_raiseairship;
	CButton	m_move_walk;
	CButton	m_move_ship;
	CButton	m_move_canoe;
	CButton	m_move_airship;
	CButton	m_forest;
	CButton	m_dockship;
	CButton	m_chime;
	CButton	m_caravan;
	CScrollBar	m_vscroll;
	CScrollBar	m_hscroll;
	CStatic m_mapstatic;
	CStatic m_tilestatic;
	CStatic m_palettestatic;
	CStatic m_backdropstatic;
	CDrawingToolButton m_penbutton{ IDB_PNG_DRAWTOOL_PEN, 0 };
	CDrawingToolButton m_blockbutton{ IDB_PNG_DRAWTOOL_BLOCK, 1 };
	CDrawingToolButton m_custom1button{ IDB_PNG_GREENTREE, 2 };
	CDrawingToolButton m_custom2button{ IDB_PNG_BLUEDROP, 3 };
	CDrawingToolButton m_custom3button{ IDB_PNG_BROWNMOUNTAIN, 4 };
	CStatic m_mappanel;
	CSimpleImageButton m_popoutbutton{ IDB_PNG_SCREEENSIZE_INCREASE };
	CCloseButton m_closebutton;
	CHelpbookButton m_helpbookbutton;

	std::vector<int> m_misccoordoffsets;

	int TRANSPARENTCOLOR = -1;
	int COORD_STARTING_OFFSET = -1;
	int COORD_SHIP_OFFSET = -1;
	int COORD_AIRSHIP_OFFSET = -1;
	int COORD_BRIDGE_OFFSET = -1;
	int COORD_CANAL_OFFSET = -1;
	int OVERWORLDPALETTE_ASSIGNMENT = -1;
	int OVERWORLDPALETTE_OFFSET = -1;
	int OVERWORLD_TILEDATA = -1;
	int OVERWORLDMAP_OFFSET = -1;
	int OVERWORLDMAP_PTRADD = -1;
	int OVERWORLDPATTERNTABLE_ASSIGNMENT = -1;
	int OVERWORLDPATTERNTABLE_OFFSET = -1;
	int BATTLEBACKDROP_COUNT = -1;
	int BATTLEBACKDROPASSIGNMENT_OFFSET = -1;
	int MAPMANPALETTE_OFFSET = -1;
	int MAPMANGRAPHIC_OFFSET = -1;
	int BATTLEPATTERNTABLE_OFFSET = -1;
	int BATTLEBACKDROPPALETTE_OFFSET = -1;
	int BATTLEDOMAIN_OFFSET = -1;
	unsigned int BATTLEPROBABILITY_OFFSET = (unsigned int)-1;
	int OVERWORLDMAP_END = -1;
	int OVERWORLDMAP_START = -1;
	unsigned int OWBATTLERATE_LAND_OFFSET = (unsigned int)-1;
	unsigned int OWBATTLERATE_SEA_OFFSET = (unsigned int)-1;

	int BANK00_OFFSET = -1;
	int BANK02_OFFSET = -1;
	int BANK07_OFFSET = -1;
	int BANK08_OFFSET = -1;
	int BINBANK01DATA_OFFSET = -1;

	// Overrides
		// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();

	// Implementation
		// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnEditlabel();
	afx_msg void OnSelchangeBackdrop();
	afx_msg void OnChangeProbability();
	afx_msg void OnFightNone();
	afx_msg void OnFightNormal();
	afx_msg void OnFightOcean();
	afx_msg void OnFightRiver();
	afx_msg void OnShowlastclick();
	afx_msg void OnDrawgrid();
	afx_msg void OnTeleport();
	afx_msg void OnFindKAB();
	afx_msg void OnCustomizetool();
	afx_msg void OnCaravan();
	afx_msg void OnChime();
	afx_msg void OnRaiseairship();
	afx_msg void OnEditgraphics();
	afx_msg void OnSelchangeMisccoords();
	afx_msg void OnChangeMiscx();
	afx_msg void OnChangeMiscy();
	afx_msg void OnMapExport();
	afx_msg void OnMapImport();
	afx_msg void OnMinimap();
	afx_msg void OnViewcoords();
	afx_msg void OnSelchangeTeleportbox();
	afx_msg void OnBnClickedButtonImportMap();
	afx_msg void OnBnClickedButtonExportMap();
	afx_msg void OnBnClickedButtonPopout();
	afx_msg LRESULT OnShowFloatMap(WPARAM wparam, LPARAM lparam);
};
