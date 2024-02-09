#pragma once
// Battle.h : header file

#include <EditorWithBackground.h>
#include <DataListDialog.h>
#include "EnemyBattleUsageData.h"
#include <RomAsmMapping_t.h>
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// CBattle dialog

class CBattle : public CEditorWithBackground
{
	// Construction
public:
	CBattle(CWnd* pParent = nullptr);   // standard constructor

protected:
	virtual void LoadOffsets();
	virtual void LoadRom();
	virtual void SaveRom();
	virtual void LoadValues();
	virtual void StoreValues();
	virtual void PaintClient(CDC& dc);

	RomAsmGropuedMappingRefs m_groupedmappings;
	bool m_usagedataon = false;
	int cur;
	bool form;
	short view;
	int battletype;
	int picassignment[4];
	bool paletteassignment[4];
	int qtymin[4];
	int qtymax[4];
	int pal[2];
	CImageList m_enemies[4];

	CRect rcPal[2];
	CRect rcPreview;

	void DrawSmallEnemies(CDC*,int);
	void DrawLargeEnemies(CDC*,int);

	bool UpdateUsageData(int btlindex);

	static CString UsageDataFormatter(CFFHacksterProject& proj, const sUseData& u);

	void UpdatePalettes();
	void UpdateBattleType(int,bool redraw = 1);
	void UpdatePicAssignment(int,int,bool redraw = 1);
	void UpdatePaletteAssignment(int,int,bool redraw = 1);
	void UpdateForm(bool);
	void UpdateView(int);
	void UpdateType(int);
	void ReloadGraphics();
	void LoadFiendChaosPic(int,int,bool);

	int BATTLE_COUNT = -1;
	int BATTLE_OFFSET = -1; // bank0B
	int BATTLE_BYTES = -1;
	int BATTLEPATTERNTABLE_COUNT = -1;
	int ENEMYTEXT_OFFSET = -1;  // bank0A
	int ENEMYTEXT_PTRADD = -1;
	int ENEMY_COUNT = -1;
	int CHAOSDRAW_TABLE = -1; // bank0B (bin) BIN_BATTLEFORMATIONS
	int CHAOSPALETTE_TABLE = -1;
	int FIENDDRAW_SHIFT = -1;
	int FIENDDRAW_TABLE = -1;
	int FIENDPALETTE_TABLE = -1;
	int BATTLEPALETTE_OFFSET = -1; // bank0C (inline) BIN_BATTLEPALETTES
	int BATTLEPATTERNTABLE_OFFSET = -1; // bank07
	int TRANSPARENTCOLOR = -1;
	int BATTLELABEL_SIZE = -1;
	int BATTLEPATTERNTABLELABEL_SIZE = -1;

	int BANK_SIZE = -1;
	int BANK07_OFFSET = -1;
	int BANK08_OFFSET = -1;
	int BANK0A_OFFSET = -1;
	int BINPRICEDATA_OFFSET = -1;
	int BINFIENDTSA_OFFSET = -1;
	int BINCHAOSTSA_OFFSET = -1;
	int BINBATTLEPALETTES_OFFSET = -1;
	int BINBATTLEFORMATIONS_OFFSET = -1;
	int BINENEMYNAMES_OFFSET = -1;

	int BANK00_OFFSET = -1;   // used by subeditors

	EnemyBattleUsageData m_usedata;
	CDataListDialog m_dlgdatalist;

	// Dialog Data
	enum { IDD = IDD_BATTLE };
	CButton	m_viewmin;
	CButton	m_viewmean;
	CButton	m_viewmax;
	CButton	m_form2;
	CButton	m_form1;
	CButton	m_type5;
	CButton	m_type4;
	CButton	m_type3;
	CButton	m_type2;
	CButton	m_type1;
	CClearEdit	m_suprised;
	CClearEdit	m_qtymin_4;
	CClearEdit	m_qtymin_3;
	CClearEdit	m_qtymin_2;
	CClearEdit	m_qtymin_1;
	CClearEdit	m_qtymax_4;
	CClearEdit	m_qtymax_3;
	CClearEdit	m_qtymax_2;
	CClearEdit	m_qtymax_1;
	CButton	m_picd_4;
	CButton	m_picd_3;
	CButton	m_picd_2;
	CButton	m_picd_1;
	CButton	m_picc_4;
	CButton	m_picc_3;
	CButton	m_picc_2;
	CButton	m_picc_1;
	CButton	m_picb_4;
	CButton	m_picb_3;
	CButton	m_picb_2;
	CButton	m_picb_1;
	CButton	m_pica_4;
	CButton	m_pica_3;
	CButton	m_pica_2;
	CButton	m_pica_1;
	CComboBox	m_patterntables;
	CStatic	m_palette2;
	CStatic	m_palette1;
	CButton	m_palb_4;
	CButton	m_palb_3;
	CButton	m_palb_2;
	CButton	m_palb_1;
	CButton	m_pala_4;
	CButton	m_pala_3;
	CButton	m_pala_2;
	CButton	m_pala_1;
	CButton	m_norun;
	CComboBox	m_enemy4;
	CComboBox	m_enemy3;
	CComboBox	m_enemy2;
	CComboBox	m_enemy1;
	CClearListBox	m_battlelist;
	CStatic m_previewstatic;
	CClearButton m_viewuse;
	CClearButton m_editlabelbutton;
	CClearButton m_patternlabelbutton;
	CClearButton m_editpatternbutton;
	CClearButton m_changepal1button;
	CClearButton m_editpal2button;
	CClearButton m_savebutton;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton;
	CClearButton m_settingsbutton = { IDPNG_GEARSM, "PNG" };
	CCloseButton m_closebutton;
	CHelpbookButton m_helpbookbutton;

	// Overrides
		// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	// Implementation
		// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelchangeBattlelist();
	afx_msg void UpdateQtyMin();
	afx_msg void UpdateQtyMax();
	afx_msg void OnViewmax();
	afx_msg void OnViewmean();
	afx_msg void OnViewmin();
	afx_msg void OnForm1();
	afx_msg void OnForm2();
	afx_msg void OnSelchangePatterntables();
	afx_msg void OnType1();
	afx_msg void OnType2();
	afx_msg void OnType3();
	afx_msg void OnType4();
	afx_msg void OnType5();
	afx_msg void OnPala1();
	afx_msg void OnPala2();
	afx_msg void OnPala3();
	afx_msg void OnPala4();
	afx_msg void OnPalb1();
	afx_msg void OnPalb2();
	afx_msg void OnPalb3();
	afx_msg void OnPalb4();
	afx_msg void OnPica1();
	afx_msg void OnPica2();
	afx_msg void OnPica3();
	afx_msg void OnPica4();
	afx_msg void OnPicb1();
	afx_msg void OnPicb2();
	afx_msg void OnPicb3();
	afx_msg void OnPicb4();
	afx_msg void OnPicc1();
	afx_msg void OnPicc2();
	afx_msg void OnPicc3();
	afx_msg void OnPicc4();
	afx_msg void OnPicd1();
	afx_msg void OnPicd2();
	afx_msg void OnPicd3();
	afx_msg void OnPicd4();
	afx_msg void OnEditbatlabel();
	afx_msg void OnEditpatlabel();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChangepal1();
	afx_msg void OnChangepal2();
	afx_msg void OnEditgraphic();
	afx_msg void OnBnClickedBattleSettings();
	afx_msg void OnBnClickedViewUsage();
};
