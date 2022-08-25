#pragma once
// Magic.h : header file

#include <EditorWithBackground.h>
#include <vector>
#include "afxlinkctrl.h"
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// CMagic dialog

class CMagic : public CEditorWithBackground
{
// Construction
public:
	CMagic(CWnd* pParent = nullptr);   // standard constructor

	int cur;
	int cur_pal;
	int cur_targ;
	int cur_eff;
	bool cur_graphic;

protected:
	virtual void LoadOffsets();
	virtual void LoadRom();
	virtual void SaveRom();
	virtual void LoadValues();
	virtual void StoreValues();
	virtual void PaintClient(CDC& dc);

	bool m_initialized = false;
	CRect rcPalette;
	CRect rcFinger;
	CRect rcGraphic;

	CImageList m_graphics;
	CMenu m_magicmenu;

	void LoadCustomizedControls();
	int UsesChecks(int);
	void ResetGraphicList();

	void ShowHideOutOfBattleControls();
	void UpdateTarget(int);
	void UpdateEffect(int);

	void ChangeOutBattles(bool);

	void CopySpell(int classindex);
	void HandleMagicListContextMenu(CWnd* pWnd, CPoint point);
	void PasteSpell(int srcindex, int destindex, int flags);

	bool haltwrite;
	std::vector<int> m_oobmagicoffsets;
	std::vector<std::vector<int>> m_oobmagicranges;

	int m_copiedspell = -1;

	int CLASS_COUNT = -1;
	int MAGIC_OFFSET = -1;
	int MAGIC_BYTES = -1;
	int MAGIC_COUNT = -1;
	int MAGICPRICE_OFFSET = -1;
	int BATTLEMESSAGETEXT_START = -1;
	int NOTHINGHAPPENS_OFFSET = -1;
	unsigned int MAGICPERMISSIONS_OFFSET = (unsigned int)-1;
	unsigned int BATTLEMESSAGE_OFFSET = (unsigned int)-1;
	int WEAPONMAGICGRAPHIC_OFFSET = -1;

	int OOBMAGIC_COUNT = -1;
	int MG_START = -1;
	int MG_CURE = -1;
	int MG_CUR2 = -1;
	int MG_CUR3 = -1;
	int MG_CUR4 = -1;
	int MG_HEAL = -1;
	int MG_HEL3 = -1;
	int MG_HEL2 = -1;
	int MG_PURE = -1;
	int MG_LIFE = -1;
	int MG_LIF2 = -1;
	int MG_WARP = -1;
	int MG_SOFT = -1;
	int MG_EXIT = -1;

	int OOBMAGICRANGE_COUNT = -1;
	int MGRANGEMIN_CURE = -1;
	int MGRANGEMAX_CURE = -1;
	int MGRANGEMIN_CUR2 = -1;
	int MGRANGEMAX_CUR2 = -1;
	int MGRANGEMIN_CUR3 = -1;
	int MGRANGEMAX_CUR3 = -1;
	int MGRANGEMIN_CUREP = -1;
	int MGRANGEMAX_CUREP = -1;
	int MGRANGEMIN_HEAL = -1;
	int MGRANGEMAX_HEAL = -1;
	int MGRANGEMIN_HEL2 = -1;
	int MGRANGEMAX_HEL2 = -1;
	int MGRANGEMIN_HEL3 = -1;
	int MGRANGEMAX_HEL3 = -1;

	int BANK0A_OFFSET = -1;
	int BINBANK09GFXDATA_OFFSET = -1;
	int BINPRICEDATA_OFFSET = -1;

	const CString mintext[9] = { "0","1","2","4","8","16","32","64","128" };
	const CString maxtext[9] = { "0","1","3","7","15","31","63","127","255" };

// Dialog Data
	enum { IDD = IDD_MAGIC };
	CComboBox	m_outminbox;
	CComboBox	m_outmaxbox;
	CClearEdit	m_outmin;
	CClearEdit	m_outmax;
	CComboBox	m_outbattlelist;
	CClearButton	m_editgfx;
	CStrikeCheck	m_use9;
	CStrikeCheck	m_use8;
	CStrikeCheck	m_use7;
	CStrikeCheck	m_use6;
	CStrikeCheck	m_use5;
	CStrikeCheck	m_use4;
	CStrikeCheck	m_use3;
	CStrikeCheck	m_use2;
	CStrikeCheck	m_use12;
	CStrikeCheck	m_use11;
	CStrikeCheck	m_use10;
	CStrikeCheck	m_use1;
	CComboBox	m_battlemessage;
	CClearEdit	m_price;
	CClearEdit	m_accuracy;
	CButton	m_target5;
	CButton	m_target4;
	CButton	m_target3;
	CButton	m_target2;
	CButton	m_target1;
	CClearListBox	m_magiclist;
	CComboBox	m_gfx;
	CStrikeCheck	m_elem8;
	CStrikeCheck	m_elem7;
	CStrikeCheck	m_elem6;
	CStrikeCheck	m_elem5;
	CStrikeCheck	m_elem4;
	CStrikeCheck	m_elem3;
	CStrikeCheck	m_elem2;
	CStrikeCheck	m_elem1;
	CButton	m_effectbox;
	CStrikeCheck	m_effect8;
	CStrikeCheck	m_effect7;
	CStrikeCheck	m_effect6;
	CStrikeCheck	m_effect5;
	CStrikeCheck	m_effect4;
	CStrikeCheck	m_effect3;
	CStrikeCheck	m_effect2;
	CStrikeCheck	m_effect1;
	CButton	m_eff9;
	CButton	m_eff8;
	CButton	m_eff7;
	CButton	m_eff6;
	CButton	m_eff5;
	CButton	m_eff4;
	CButton	m_eff3;
	CButton	m_eff2;
	CButton	m_eff17;
	CButton	m_eff16;
	CButton	m_eff15;
	CButton	m_eff14;
	CButton	m_eff13;
	CButton	m_eff12;
	CButton	m_eff11;
	CButton	m_eff10;
	CButton	m_eff1;
	CClearButton	m_editlabel;
	CClearButton	m_damagebox;
	CClearEdit	m_damage;
	CStatic m_staticByte7;
	CClearEdit m_editByte7;
	CClearButton m_settingsbutton = { IDPNG_GEARSM, "PNG" };
	CCloseButton m_closebutton;
	CClearButton m_savebutton;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton;
	CHelpbookButton m_helpbookbutton;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
	// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelchangeMagiclist();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeGfx();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnTarget1();
	afx_msg void OnTarget2();
	afx_msg void OnTarget3();
	afx_msg void OnTarget4();
	afx_msg void OnTarget5();
	afx_msg void OnEff1();
	afx_msg void OnEff10();
	afx_msg void OnEff11();
	afx_msg void OnEff12();
	afx_msg void OnEff13();
	afx_msg void OnEff14();
	afx_msg void OnEff15();
	afx_msg void OnEff16();
	afx_msg void OnEff17();
	afx_msg void OnEff2();
	afx_msg void OnEff3();
	afx_msg void OnEff4();
	afx_msg void OnEff5();
	afx_msg void OnEff6();
	afx_msg void OnEff7();
	afx_msg void OnEff8();
	afx_msg void OnEff9();
	afx_msg void OnEditlabel();
	afx_msg void OnEditgfx();
	afx_msg void OnSelchangeOutbattlelist();
	afx_msg void OnChangeOutmax();
	afx_msg void OnChangeOutmin();
	afx_msg void OnEditchangeOutmaxbox();
	afx_msg void OnEditchangeOutminbox();
	afx_msg void OnSelchangeOutmaxbox();
	afx_msg void OnSelchangeOutminbox();
	afx_msg void OnBnClickedClassesSettings();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
