#if !defined(AFX_OVERWORLDMAP_H__D4F4EFA3_060E_443F_94F9_797904E48555__INCLUDED_)
#define AFX_OVERWORLDMAP_H__D4F4EFA3_060E_443F_94F9_797904E48555__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OverworldMap.h : header file
//

#include <EditorWithBackground.h>
#include "ICoordMap.h"
#include <vector>
#include "afxwin.h"
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// COverworldMap dialog

class COverworldMap : public CEditorWithBackground, public ICoordMap
{
// Construction
public:
	COverworldMap(CWnd* pParent = nullptr);   // standard constructor

	enum CancelContext { Coords, Minimap };

	bool BootToTeleportFollowup;

	// Inherited via ICoordMap
	virtual void UpdateTeleportLabel(int areaid, int type) override;
	virtual void Cancel(int context) override;
	virtual POINT GetLastClick() override;
	virtual int GetCurMap() override;
	virtual void TeleportHere(int mapindex, int x, int y) override;

	virtual void DoHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void DoVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

protected:
	CFFHacksterProject* cart = nullptr; //FUTURE - replace cart with Project and remove references to cart
	CImageList m_sprites;
	CImageList m_backdrop;
	CImageList m_tools;
	CPen redpen;
	CPen bluepen;
	CRect rcTiles;
	CRect rcMap;
	CRect rcBackdrop;
	CRect rcTools;
	CRect rcToolRect;
	CRect rcPalette;

	CSubBanner m_banner;
	CPoint ptHover;
	CPoint ptLastClick;
	CPoint ptDomain;

	BYTE DecompressedMap[256][256];
	BYTE palette[2][16];
	CPoint ScrollOffset;
	int cur_tile;
	short cur_tool;
	bool probabilitychanged;
	BYTE mousedown;
	int kab;

	CPoint misccoords[5];

	void UpdateTeleportLabel(int);
	void DoMinimap();
	void DoViewcoords();

	void LoadRom();
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

	virtual bool DoSave();

// Dialog Data
	enum { IDD = IDD_OVERWORLDMAP };
	CClearEdit m_landencrateedit;
	CClearEdit m_seaencrateedit;
	CButton	m_viewcoords;
	CButton	m_minimap;
	CClearEdit	m_miscy;
	CClearEdit	m_miscx;
	CClearComboBox	m_misccoords;
	CButton	m_customizetool;
	CStatic	m_kab;
	CClearComboBox	m_backdroplist;
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
	CClearComboBox	m_battle8;
	CClearComboBox	m_battle7;
	CClearComboBox	m_battle6;
	CClearComboBox	m_battle5;
	CClearComboBox	m_battle4;
	CClearComboBox	m_battle3;
	CClearComboBox	m_battle2;
	CClearComboBox	m_battle1;
	CButton	m_fight_river;
	CButton	m_fight_ocean;
	CButton	m_fight_normal;
	CButton	m_fight_none;
	CClearComboBox	m_teleportbox;
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
	CStatic m_toolstatic;
	CStatic m_backdropstatic;
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

// Implementation
	// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEditlabel();
	afx_msg void OnSelchangeBackdrop();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChangeProbability();
	afx_msg void OnFightNone();
	afx_msg void OnFightNormal();
	afx_msg void OnFightOcean();
	afx_msg void OnFightRiver();
	afx_msg void OnShowlastclick();
	afx_msg void OnDrawgrid();
	afx_msg void OnTeleport();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnFindKAB();
	afx_msg void OnCustomizetool();
	afx_msg void OnCaravan();
	afx_msg void OnChime();
	afx_msg void OnRaiseairship();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEditgraphics();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeMisccoords();
	afx_msg void OnChangeMiscx();
	afx_msg void OnChangeMiscy();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMapExport();
	afx_msg void OnMapImport();
	afx_msg void OnMinimap();
	afx_msg void OnViewcoords();
	afx_msg void OnSelchangeTeleportbox();
	afx_msg void OnBnClickedButtonImportMap();
	afx_msg void OnBnClickedButtonExportMap();
};

#endif // !defined(AFX_OVERWORLDMAP_H__D4F4EFA3_060E_443F_94F9_797904E48555__INCLUDED_)
