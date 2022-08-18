#pragma once
// Maps.h : header file

#include <EditorWithBackground.h>
#include "ICoordMap.h"
#include "IMapEditor.h"
#include <vector>
#include "afxwin.h"
#include <SimpleImageButton.h>
#include <DrawingToolButton.h>
#include <FloatingMapDlg.h>
class CFFHacksterProject;
class CEntriesLoader;

/////////////////////////////////////////////////////////////////////////////
// CMaps dialog

class CMaps : public CEditorWithBackground,
			public ICoordMap,
			public IMapEditor
{
	// Construction
public:
	CMaps(CWnd* pParent = nullptr);   // standard constructor

	enum CancelContext { Coords };

	bool BootToTeleportFollowup;
	CEntriesLoader* Enloader = nullptr;

	// Inherited via ICoordMap
	virtual void UpdateTeleportLabel(int areaid, int type) override;
	virtual void Cancel(int context) override;
	virtual POINT GetLastClick() override;
	virtual int GetCurMap() override;
	virtual void TeleportHere(int mapindex, int x, int y) override;
	virtual void DoHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void DoVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	// Inherited from IMapEditor
	virtual void HandleLButtonDown(UINT nFlags, CPoint point);
	virtual void HandleLButtonUp(UINT nFlags, CPoint point);
	virtual void HandleLButtonDblClk(UINT nFlags, CPoint point);
	virtual void HandleRButtonDown(UINT nFlags, CPoint pt);
	virtual void HandleRButtonUp(UINT nFlags, CPoint pt);
	virtual void HandleRButtonDblClk(UINT nFlags, CPoint point);
	virtual void HandleMouseMove(UINT nFlags, CPoint newhover);
	virtual void HandleMapImport();
	virtual void HandleMapExport();
	virtual bool HandleCustomizeTool();

protected:
	CFFHacksterProject* cart = nullptr; //FUTURE - replace cart with Project and remove references to cart

	void UpdateTeleportLabel(int,bool);
	void DoViewcoords();
	void DoOK();
	void DoSelchangeMaplist();
	void ApplyTileTint(int ref);

	int cur_map;
	int cur_tileset;
	int cur_tile;
	int cur_tc;
	int cur_tcitem;
	int cur_tiledata;
	int cur_tool;
	BYTE mousedown;
	bool m_showingrooms = false;
	bool m_firstpopoutdone = false;
	bool m_popoutcreated = false;
	std::vector<CDrawingToolButton*> m_toolbuttons;

	CPoint ptLastClick;
	CButton	m_viewcoords;
	
	BYTE DecompressedMap[0x40][0x40];
	CImageList m_sprites;
	CPen redpen;
	CRect rcTiles;
	CRect rcMap;
	CRect rcPalettes;
	CRect rcPalettes2;
	CRect rcToolRect;
	CPoint ScrollOffset;
	std::vector<CPoint> Sprite_Coords; // MAPSPRITE_COUNT;
	std::vector<bool> Sprite_InRoom; // MAPSPRITE_COUNT;
	std::vector<bool> Sprite_StandStill; // MAPSPRITE_COUNT;
	std::vector<short> Sprite_Value; // MAPSPRITE_COUNT;

	int kab;

	CPoint ptHover;
	void UpdateClick(CPoint);
	CPoint fix_map_point(CPoint point);

	void LoadRom();
	virtual void SaveRom();
	void LoadValues();
	virtual void StoreValues();
	void LoadTileData();
	void StoreTileData();
	void CompressMap();
	int GetByteCount(int,bool);

	void LoadTC();
	void StoreTC();
	void LoadTCItem();
	void StoreTCItem();

	void ReloadImages(CProgressCtrl*);
	void ReloadSprites(CProgressCtrl*);

	void UpdateTileData();
	void UpdatePics();

	void init_popout_map_window();
	void PopMapDialog(bool in);

	BYTE MapPalette[2][4][4];
	BYTE SpritePalette[2][4];
	BYTE ControlPalette[8];

	// Dialog Data
	enum { IDD = IDD_MAPS_NEW };
	CBorderedListBox	m_maplist;
	CClearEdit m_encounterrateedit;
	CStatic	m_kab;
	CButton	m_customtool;
	CButton	m_showlastclick;
	CStatic	m_hovering;
	CStatic	m_lastclick;
	CComboBox	m_spritegraphic;
	CButton	m_still;
	CClearEdit	m_spritecoordy;
	CClearEdit	m_spritecoordx;
	CComboBox	m_sprite;
	CButton	m_inroom;
	CComboBox	m_sprite_list;
	CButton	m_editlabel;
	CButton	m_warp;
	CComboBox	m_text_list;
	CComboBox	m_teleport_list;
	CButton	m_teleport;
	CClearEdit	m_tcitem_price;
	CComboBox	m_tcitem_list;
	CComboBox	m_tc_list;
	CButton	m_tc;
	CComboBox	m_special_list;
	CButton	m_special;
	CComboBox	m_shop_list;
	CButton	m_shop;
	CButton	m_move;
	CComboBox	m_fight_list;
	CButton	m_fight;
	CComboBox	m_tileset;
	CClearEdit	m_probability8;
	CClearEdit	m_probability7;
	CClearEdit	m_probability6;
	CClearEdit	m_probability5;
	CClearEdit	m_probability4;
	CClearEdit	m_probability3;
	CClearEdit	m_probability2;
	CClearEdit	m_probability1;
	CButton	m_formation8;
	CButton	m_formation7;
	CButton	m_formation6;
	CButton	m_formation5;
	CButton	m_formation4;
	CButton	m_formation3;
	CButton	m_formation2;
	CButton	m_formation1;
	CComboBox	m_battle8;
	CComboBox	m_battle7;
	CComboBox	m_battle6;
	CComboBox	m_battle5;
	CComboBox	m_battle4;
	CComboBox	m_battle3;
	CComboBox	m_battle2;
	CComboBox	m_battle1;
	CScrollBar	m_vscroll;
	CScrollBar	m_hscroll;
	CButton	m_showrooms;
	CStatic m_mapstatic;
	CStatic m_tilestatic;
	CStatic m_palettestatic;
	CDrawingToolButton m_penbutton{ IDB_PNG_DRAWTOOL_PEN, 0 };
	CDrawingToolButton m_blockbutton{ IDB_PNG_DRAWTOOL_BLOCK, 1 };
	CDrawingToolButton m_custom1button{ IDB_PNG_DRAWTOOL_CUSTOM1, 5 };
	CDrawingToolButton m_custom2button{ IDB_PNG_DRAWTOOL_CUSTOM2, 6 };
	CStatic m_mappanel;
	CSimpleImageButton m_popoutbutton{ IDB_PNG_SCREEENSIZE_INCREASE };
	CCloseButton m_closebutton;
	CHelpbookButton m_helpbookbutton;

	CSubBanner m_banner;
	CFloatingMapDlg m_mapdlg;

	int ITEMPRICE_OFFSET = -1;
	int MAPSPRITEPATTERNTABLE_COUNT = -1;
	int MAPSPRITEPATTERNTABLE_OFFSET = -1;
	int MAPSPRITE_PICASSIGNMENT = -1;
	int MAPTILESET_ASSIGNMENT = -1;
	int MAPPALETTE_OFFSET = -1;
	int TILESETPATTERNTABLE_OFFSET = -1;
	int TILESETPALETTE_ASSIGNMENT = -1;
	int TILESETPATTERNTABLE_ASSIGNMENT = -1;
	int TILESET_TILEDATA = -1;
	int MAP_END = -1;
	int MAP_START = -1;
	int MAP_COUNT = -1;
	int MAP_OFFSET = -1;
	int MAP_PTRADD = -1;
	int NNTELEPORT_COUNT = -1;
	int ONTELEPORT_COUNT = -1;
	int MAPSPRITE_COUNT = -1;
	int MAPSPRITE_OFFSET = -1;
	int BATTLEDOMAIN_OFFSET = -1;
	unsigned int BATTLEPROBABILITY_OFFSET = (unsigned int)-1;
	int TREASURE_OFFSET = -1;
	unsigned int MAPBATTLERATE_OFFSET = (unsigned int)-1;

	int BANK0A_OFFSET = -1;
	int BANK00_OFFSET = -1;
	int BANK02_OFFSET = -1;
	int BANK03_OFFSET = -1;
	int BANK04_OFFSET = -1;
	int BANK05_OFFSET = -1;
	int BANK06_OFFSET = -1;
	int BANK07_OFFSET = -1;
	int BINBANK01DATA_OFFSET = -1;
	int BINPRICEDATA_OFFSET = -1;

	// Overrides
		// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Implementation
		// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnSelchangeMaplist();
	afx_msg void OnShowrooms();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFight();
	afx_msg void OnShop();
	afx_msg void OnSpecial();
	afx_msg void OnTc();
	afx_msg void OnWarp();
	afx_msg void OnTeleport();
	afx_msg void OnMove();
	afx_msg void OnSelchangeTcList();
	afx_msg void OnSelchangeTcitemList();
	afx_msg void OnSelchangeTileset();
	afx_msg void OnSelchangeSpriteList();
	afx_msg void OnEditspritegfx();
	afx_msg void OnStill();
	afx_msg void OnInroom();
	afx_msg void OnChangeSpritecoordx();
	afx_msg void OnChangeSpritecoordy();
	afx_msg void OnSelchangeSprite();
	afx_msg void OnSelchangeSpritegraphic();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnShowlastclick();
	afx_msg void OnCustomtool();
	afx_msg void OnFindkab();
	afx_msg void OnEditSpriteLabel();
	afx_msg void OnEditgfxlabel();
	afx_msg void OnMaplabel();
	afx_msg void OnEditlabel();
	afx_msg void OnTilesetlabel();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMapExport();
	afx_msg void OnMapImport();
	afx_msg void OnViewcoords();
	afx_msg void OnSelchangeTeleportList();
	afx_msg LRESULT OnDrawToolBnClick(WPARAM wparam, LPARAM lparam);
	afx_msg void OnBnClickedButtonPopout();
	afx_msg LRESULT OnShowFloatMap(WPARAM wparam, LPARAM lparam);
};
