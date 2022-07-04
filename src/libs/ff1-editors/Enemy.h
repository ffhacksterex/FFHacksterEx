#if !defined(AFX_ENEMY_H__2FB3AFEC_3AF8_4739_A37B_C71C985332CE__INCLUDED_)
#define AFX_ENEMY_H__2FB3AFEC_3AF8_4739_A37B_C71C985332CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Enemy.h : header file
//

#include <EditorWithBackground.h>
#include <DataListDialog.h>
#include "EnemyBattleUsageData.h"
#include <assembly_types.h>
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// CEnemy dialog

class CEnemy : public CEditorWithBackground
{
// Construction
public:
	CEnemy(CWnd* pParent = nullptr);   // standard constructor

protected:
	bool m_usagedataon = false;
	int cur;
	int cur_ai;
	bool m_wasflags = false;

	int ENEMY_OFFSET = -1;
	int ENEMY_BYTES = -1;
	int AI_OFFSET = -1;
	int AI_COUNT = -1;
	int ENEMYTEXT_OFFSET = -1;
	int BATTLE_OFFSET = -1;
	int BATTLE_BYTES = -1;

	int BANK0A_OFFSET = -1;
	int BINENEMYDATA_OFFSET = -1; //TODO - this is identical to ENEMY_OFFSET
	int BINAIDATA_OFFSET = -1;
	int BINBATTLEFORMATIONS_OFFSET = -1;

	int BINBATTLEPALETTES_OFFSET = -1; // used by subeditors
	int BINCHAOSTSA_OFFSET = -1;
	int BINFIENDTSA_OFFSET = -1;
	int BANK00_OFFSET = -1;
	int BANK07_OFFSET = -1;

	stdstringvector m_asmroutinenames;
	talknamevectormap m_asmroutinemap;
	EnemyBattleUsageData m_usedata;
	CDataListDialog m_dlgdatalist;

	bool UpdateUsageData(int enemyindex);
	static CString UsageDataFormatter(CFFHacksterProject& proj, const sUseData& u);

	void LoadCustomizedControls();
	void LoadRom();
	virtual void SaveRom();
	void LoadValues();
	virtual void StoreValues();
	void LoadAI();
	void StoreAI();
	virtual void PaintClient(CDC & dc);

	unsigned char GetByte15HaxValue();
	unsigned char GetByte15ValueFromFlags();
	void SetByte15FlagsFromValue(unsigned char value);

// Dialog Data
	enum { IDD = IDD_ENEMY };
	CStrikeCheck	m_atkelem8;
	CStrikeCheck	m_atkelem7;
	CStrikeCheck	m_atkelem6;
	CStrikeCheck	m_atkelem5;
	CStrikeCheck	m_atkelem4;
	CStrikeCheck	m_atkelem3;
	CStrikeCheck	m_atkelem2;
	CStrikeCheck	m_atkelem1;
	CClearButton m_editlabel;
	CStrikeCheck	m_weak8;
	CStrikeCheck	m_weak7;
	CStrikeCheck	m_weak6;
	CStrikeCheck	m_weak5;
	CStrikeCheck	m_weak4;
	CStrikeCheck	m_weak3;
	CStrikeCheck	m_weak2;
	CStrikeCheck	m_weak1;
	CClearEdit	m_unknown;
	CClearEdit	m_str;
	CStrikeCheck	m_res8;
	CStrikeCheck	m_res7;
	CStrikeCheck	m_res6;
	CStrikeCheck	m_res5;
	CStrikeCheck	m_res4;
	CStrikeCheck	m_res3;
	CStrikeCheck	m_res2;
	CStrikeCheck	m_res1;
	CClearEdit	m_morale;
	CClearEdit	m_magdef;
	CClearEdit	m_hp;
	CClearEdit	m_hits;
	CClearEdit	m_hit;
	CClearEdit	m_gold;
	CClearEdit	m_exp;
	CClearListBox	m_enemylist;
	CStrikeCheck	m_eff8;
	CStrikeCheck	m_eff7;
	CStrikeCheck	m_eff6;
	CStrikeCheck	m_eff5;
	CStrikeCheck	m_eff4;
	CStrikeCheck	m_eff3;
	CStrikeCheck	m_eff2;
	CStrikeCheck	m_eff1;
	CClearEdit	m_def;
	CClearEdit	m_critical;
	CStrikeCheck	m_cat8;
	CStrikeCheck	m_cat7;
	CStrikeCheck	m_cat6;
	CStrikeCheck	m_cat5;
	CStrikeCheck	m_cat4;
	CStrikeCheck	m_cat3;
	CStrikeCheck	m_cat2;
	CStrikeCheck	m_cat1;
	CClearEdit	m_ai_magicrate;
	CClearComboBox	m_ai_magic8;
	CClearComboBox	m_ai_magic7;
	CClearComboBox	m_ai_magic6;
	CClearComboBox	m_ai_magic5;
	CClearComboBox	m_ai_magic4;
	CClearComboBox	m_ai_magic3;
	CClearComboBox	m_ai_magic2;
	CClearComboBox	m_ai_magic1;
	CClearEdit	m_ai_abilityrate;
	CClearComboBox	m_ai_ability4;
	CClearComboBox	m_ai_ability3;
	CClearComboBox	m_ai_ability2;
	CClearComboBox	m_ai_ability1;
	CClearComboBox	m_ai;
	CClearEdit	m_agility;
	CStatic m_groupByte15;
	CStatic m_groupWeak;
	CStatic m_groupAttack;
	CStatic m_statAttackElem;
	CStatic m_groupAi;
	CCloseButton m_closebutton;
	CClearButton m_settingsbutton = { IDPNG_GEARSM, "PNG" };
	CClearButton m_savebutton;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton;
	CClearButton m_editgfxbutton;
	CHelpbookButton m_helpbookbutton;
	CClearButton m_viewuse;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelchangeEnemylist();
	afx_msg void OnSelchangeAi();
	afx_msg void OnEditlabel();
	afx_msg void OnEditpic();
	afx_msg void OnBnClickedSettings();
	afx_msg void OnBnClickedEnemyBtnViewuse();
};

#endif // !defined(AFX_ENEMY_H__2FB3AFEC_3AF8_4739_A37B_C71C985332CE__INCLUDED_)
