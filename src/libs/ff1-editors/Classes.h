#pragma once
// Classes.h : header file

#include <EditorWithBackground.h>
#include "vector_types.h"
#include "map_types.h"
#include "afxwin.h"
class CFFHacksterProject;
class GameSerializer;

/////////////////////////////////////////////////////////////////////////////
// CClasses dialog

class CClasses : public CEditorWithBackground
{
// Construction
protected:
	CClasses(UINT idd, CWnd* pParent = nullptr);
public:
	CClasses(CWnd* pParent = nullptr);   // standard constructor

	bool WantsToReload() const;

protected:
	CStrikeCheck* ar_1m[4];
	CStrikeCheck* ar_2m[4];
	CStrikeCheck* ar_3m[4];
	CStrikeCheck* ar_4m[4];
	CStrikeCheck* ar_5m[4];
	CStrikeCheck* ar_6m[4];
	CStrikeCheck* ar_7m[4];
	CStrikeCheck* ar_8m[4];
	CStrikeCheck* ar_strong[4];
	CStrikeCheck* ar_str[4];
	CStrikeCheck* ar_agi[4];
	CStrikeCheck* ar_int[4];
	CStrikeCheck* ar_vit[4];
	CStrikeCheck* ar_luc[4];
	CEdit* ar_exp[4];
	CStatic* ar_level[4];
	CStatic* ar_totalhp[4];
	CStatic* ar_totalstr[4];
	CStatic* ar_totalint[4];
	CStatic* ar_totalvit[4];
	CStatic* ar_totalluc[4];
	CStatic* ar_totalagi[4];
	CStatic* ar_totalmagic[4];

	int cur_class;
	short level_offset;

	short TotalHP[49];
	short TotalStr[49];
	short TotalInt[49];
	short TotalVit[49];
	short TotalLuc[49];
	short TotalAgi[49];
	BYTE TotalMagic[49][8];
	short ViewScale;

	unsigned int CLASS_COUNT = (unsigned int)-1;
	unsigned int CLASS_OFFSET = (unsigned int)-1;
	unsigned int CLASS_BYTES = (unsigned int)-1;
	unsigned int HITPCTINCREASE_OFFSET = (unsigned int)-1;
	unsigned int MAGDEFINCREASE_OFFSET = (unsigned int)-1;
	unsigned int STARTINGMP_OFFSET = (unsigned int)-1;
	unsigned int MPCAP_OFFSET = (unsigned int)-1;
	unsigned int MAGICMIN_OFFSET = (unsigned int)-1;
	unsigned int MAGICMAX_OFFSET = (unsigned int)-1;
	unsigned int HPMAX_OFFSET = (unsigned int)-1;
	unsigned int HPRANGEMIN_OFFSET = (unsigned int)-1;
	unsigned int HPRANGEMAX_OFFSET = (unsigned int)-1;
	unsigned int LEVELUP_OFFSET = (unsigned int)-1;
	unsigned int EXP_OFFSET = (unsigned int)-1;
	unsigned int MAPMANGRAPHIC_OFFSET = (unsigned int)-1;
	
	unsigned int LOCKFIGHTERTHIEF_OFFSET = (unsigned int)-1;
	unsigned int HOLDMPCODE_OFFSET = (unsigned int)-1;
	unsigned int HOLDMPCLASS1_OFFSET = (unsigned int)-1;
	unsigned int HOLDMPCLASS2_OFFSET = (unsigned int)-1;
	unsigned int HOLDMPCLASSID2_OFFSET = (unsigned int)-1;
	unsigned int CAPMPCODE_OFFSET = (unsigned int)-1;
	unsigned int CAPMPCLASS1_OFFSET = (unsigned int)-1;
	unsigned int CAPMPCLASSID1_OFFSET = (unsigned int)-1;
	unsigned int CAPMPCLASS2_OFFSET = (unsigned int)-1;
	unsigned int CAPMPCLASSID2_OFFSET = (unsigned int)-1;
	unsigned int CAPMPLIMIT_OFFSET = (unsigned int)-1;
	unsigned int BBMACLASS1STATS_OFFSET = (unsigned int)-1;
	unsigned int BBMACLASS1UNADJ_OFFSET = (unsigned int)-1;
	unsigned int BBMACLASS1READJ_OFFSET = (unsigned int)-1;
	unsigned int BBMACLASS1HITS_OFFSET = (unsigned int)-1;
	unsigned int BBMACLASS2STATS_OFFSET = (unsigned int)-1;
	unsigned int BBMACLASS2UNADJ_OFFSET = (unsigned int)-1;
	unsigned int BBMACLASS2READJ_OFFSET = (unsigned int)-1;
	unsigned int BBMACLASS2HITS_OFFSET = (unsigned int)-1;

	int BANK0A_OFFSET = -1;
	int BANK00_OFFSET = -1;
	int BANK02_OFFSET = -1;
	int BINLEVELUPDATA_OFFSET = -1;

	size_t WEAPON_COUNT = (size_t)-1;
	size_t WEAPONPERMISSIONS_OFFSET = (size_t)-1;
	size_t ARMOR_COUNT = (size_t)-1;
	size_t ARMORPERMISSIONS_OFFSET = (size_t)-1;
	size_t MAGIC_COUNT = (size_t)-1;
	size_t MAGICPERMISSIONS_OFFSET = (size_t)-1;
	size_t SPELLLEVEL_COUNT = (size_t)-1;

	// Battle sprite offsets
	unsigned int CHARBATTLEPALETTE_ASSIGNMENT1 = (unsigned int)-1; // bank 0C
	unsigned int CHARBATTLEPIC_OFFSET = (unsigned int)-1;          // bank 09
	unsigned int CHARBATTLEPALETTE_ASSIGNMENT2 = (unsigned int)-1; // bank 0F
	unsigned int CHARBATTLEPALETTE_OFFSET = (unsigned int)-1;      // bank 0F
	int BINBANK09DATA_OFFSET = -1;
	int BINBANK09GFXDATA_OFFSET = -1;

	// Mapman sprite offset
	int OVERWORLDPALETTE_OFFSET = -1;
	int MAPMANPALETTE_OFFSET = -1;

	unsigned int BATTLESPRITETILE_COUNT = (unsigned int)-1;
	unsigned int BATTLESPRITETILE_BYTES = (unsigned int)-1;
	unsigned int MAPMANSPRITETILE_COUNT = (unsigned int)-1;
	unsigned int MAPMANSPRITETILE_BYTES = (unsigned int)-1;

	bool m_bRequestReload = false;
	bool m_IgnoreMPRange = false;
	bool m_bAllowHoldMpEdits = false;

	bytevector m_holdmpbytes;
	bytevector m_capmpbytes;

	mfcstrintmap m_constantmap;
	CMenu m_classesmenu;
	int m_copiedclass = -1;
	bool m_pastewarned = false;

	void InitButtons();

	virtual void ReadOffsets();
	virtual void LoadRom();
	virtual void SaveRom();
	virtual void LoadValues();
	virtual void StoreValues();
	virtual void LoadClassEngineData(GameSerializer & ser);
	virtual void SaveClassEngineData(GameSerializer & ser);
	virtual int GetLevelOffset(int classid) const;

	virtual void PaintClient(CDC & dc);

	void DisplayRunningTotals();
	void ScrollXPViewBy(int offset);

	stdstringvector GetClassIdVector(unsigned char classid);

	void CopySwapBytes(bool swapping, size_t srcoffset, size_t destoffset, size_t start, size_t count);
	void CopySwapBits16(bool swapping, size_t baseoffset, size_t srcindex, size_t destindex, size_t bits, size_t start, size_t count);
	void CopyClass(int classindex);
	bool WarnCancelPaste();
	virtual void PasteStartInfo(int srcindex, int destindex, int flags);
	virtual void PasteLevelInfo(int srcindex, int destindex, int flags);
	virtual void PasteSpriteAndPaletteInfo(int srcindex, int destindex, int flags);
	virtual void PasteUsables(int srcindex, int destindex, int flags);
	virtual void HandleClassListContextMenu(CWnd* pWnd, CPoint point);
	virtual void HandleSpellCheckContextMenu(CWnd* pWnd, CPoint point);
	virtual void HandleSaivlCheckContextMenu(CWnd* pWnd, CPoint point);
	virtual bool IsSpellChargeCheck(CWnd* pWnd);
	virtual bool IsSaivlChargeCheck(CWnd * pWnd);
	virtual void ChangeSpellCheckGroup(int group, bool checked);
	virtual void ChangeSaivlCheckGroup(int group, bool checked);

	virtual bool UsingHoldMP(int slot);
	virtual bool UsingCapMP(int slot);
	virtual bool UsingMPRange();
	virtual bool UsingBBMA(int slot);

// Dialog Data
	enum { IDD = IDD_CLASSES };
	CStatic m_groupHoldMpMax;
	CClearEdit m_editHoldMpMax;
	CClearEdit	m_hitpctup;
	CButton	m_mean;
	CStatic	m_totalvit4;
	CStatic	m_totalvit3;
	CStatic	m_totalvit2;
	CStatic	m_totalvit1;
	CStatic	m_totalstr4;
	CStatic	m_totalstr3;
	CStatic	m_totalstr2;
	CStatic	m_totalstr1;
	CStatic	m_totalmag4;
	CStatic	m_totalmag3;
	CStatic	m_totalmag2;
	CStatic	m_totalmag1;
	CStatic	m_totalluc4;
	CStatic	m_totalluc3;
	CStatic	m_totalluc2;
	CStatic	m_totalluc1;
	CStatic	m_totalint4;
	CStatic	m_totalint3;
	CStatic	m_totalint2;
	CStatic	m_totalint1;
	CStatic	m_totalhp4;
	CStatic	m_totalhp3;
	CStatic	m_totalhp2;
	CStatic	m_totalhp1;
	CStatic	m_totalagi4;
	CStatic	m_totalagi3;
	CStatic	m_totalagi2;
	CStatic	m_totalagi1;
	CStrikeCheck	m_strong4;
	CStrikeCheck	m_strong3;
	CStrikeCheck	m_strong2;
	CStrikeCheck	m_strong1;
	CStrikeCheck	m_str4;
	CStrikeCheck	m_str3;
	CStrikeCheck	m_str2;
	CStrikeCheck	m_str1;
	CStrikeCheck	m_agi4;
	CStrikeCheck	m_agi3;
	CStrikeCheck	m_agi2;
	CStrikeCheck	m_agi1;
	CStrikeCheck	m_int4;
	CStrikeCheck	m_int3;
	CStrikeCheck	m_int2;
	CStrikeCheck	m_int1;
	CStrikeCheck	m_vit4;
	CStrikeCheck	m_vit2;
	CStrikeCheck	m_vit3;
	CStrikeCheck	m_vit1;
	CStrikeCheck	m_luc4;
	CStrikeCheck	m_luc3;
	CStrikeCheck	m_luc2;
	CStrikeCheck	m_luc1;
	CStrikeCheck	m_8m4;
	CStrikeCheck	m_8m3;
	CStrikeCheck	m_8m2;
	CStrikeCheck	m_8m1;
	CStrikeCheck	m_7m4;
	CStrikeCheck	m_7m3;
	CStrikeCheck	m_7m2;
	CStrikeCheck	m_7m1;
	CStrikeCheck	m_6m4;
	CStrikeCheck	m_6m3;
	CStrikeCheck	m_6m2;
	CStrikeCheck	m_6m1;
	CStrikeCheck	m_5m4;
	CStrikeCheck	m_5m3;
	CStrikeCheck	m_5m2;
	CStrikeCheck	m_5m1;
	CStrikeCheck	m_4m4;
	CStrikeCheck	m_4m3;
	CStrikeCheck	m_4m2;
	CStrikeCheck	m_4m1;
	CStrikeCheck	m_3m4;
	CStrikeCheck	m_3m3;
	CStrikeCheck	m_3m2;
	CStrikeCheck	m_3m1;
	CStrikeCheck	m_2m4;
	CStrikeCheck	m_2m3;
	CStrikeCheck	m_2m2;
	CStrikeCheck	m_2m1;
	CStrikeCheck	m_1m4;
	CStrikeCheck	m_1m3;
	CStrikeCheck	m_1m2;
	CStrikeCheck	m_1m1;
	CScrollBar	m_scroll;
	CButton	m_min;
	CButton	m_max;
	CClearEdit	m_magic;
	CStatic	m_level4;
	CStatic	m_level3;
	CStatic	m_level2;
	CStatic	m_level1;
	CClearEdit	m_hprangemin;
	CClearEdit	m_hprangemax;
	CClearEdit	m_exp4;
	CClearEdit	m_exp3;
	CClearEdit	m_exp2;
	CClearEdit	m_exp1;
	CClearEdit	m_vit;
	CClearEdit	m_classid;
	CClearEdit	m_str;
	CClearEdit	m_startinghp;
	CClearEdit	m_magdef;
	CClearEdit	m_luck;
	CClearEdit	m_int;
	CClearEdit	m_hit;
	CClearEdit	m_evade;
	CClearListBox	m_classlist;
	CClearEdit	m_damage;
	CClearEdit	m_agility;
	CClearEdit m_editMpMaxAllChars;
	CClearEdit m_editMagDefUp;
	CComboBox m_comboclass2;
	CComboBox m_comboclass1;
	CComboBox m_combopost2;
	CComboBox m_combopost1;
	CComboBox m_mpmincombo;
	CComboBox m_mpmaxcombo;
	CComboBox m_bbmacombo1;
	CComboBox m_bbmacombo2;
	CComboBox m_2xhitscombo1;
	CComboBox m_2xhitscombo2;
	CClearEdit m_maxhpedit;
	CInfoButton m_infobutton;
	CClearButton m_settingsbutton = { IDPNG_GEARSM, "PNG" };
	CCloseButton m_closebutton;
	CClearButton m_savebutton;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton;
	CClearButton m_mapmanpicbutton;
	CClearButton m_editpatpicbutton;
	CHelpbookButton m_helpbookbutton;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
	// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelchangeClasslist();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMax();
	afx_msg void OnMin();
	afx_msg void RecalculateRunningTotals();
	afx_msg void OnChangeHprangemax();
	afx_msg void OnChangeHprangemin();
	afx_msg void OnCheckBox();
	afx_msg void OnMean();
	afx_msg void OnBattlepic();
	afx_msg void OnMapmanpic();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnCbnSelchangeComboMprangemin();
	afx_msg void OnCbnSelchangeComboMprangemax();
	afx_msg void OnBnClickedClassesInfo();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnCbnSelchangeClasscombo1();
	afx_msg void OnCbnSelchangeClasscombo2();
	afx_msg void OnCbnSelchangePostcombo1();
	afx_msg void OnCbnSelchangePostcombo2();
	afx_msg void OnBnClickedClassesSettings();
};
