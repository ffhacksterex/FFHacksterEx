// Classes.cpp : implementation file
//
#include "stdafx.h"
#include "Classes.h"
#include "AsmFiles.h"
#include "collection_helpers.h"
#include "DRAW_STRUCT.h"
#include "FFHacksterProject.h"
#include "GameSerializer.h"
#include "general_functions.h"
#include "imaging_helpers.h"
#include "ingame_text_functions.h"
#include "ini_functions.h"
#include "io_functions.h"
#include "path_functions.h"
#include "std_assembly.h"
#include "std_project_properties.h"
#include "type_support.h"
#include "ui_helpers.h"

#include "BattlePic.h"
#include "ClassesEditorSettingsDlg.h"
#include "ClassesEditorSettings.h"
#include "DlgPasteTargets.h"
#include "Mapman.h"
#include "WaitingDlg.h"

using namespace Imaging;
using namespace Ini;
using namespace Ingametext;
using namespace Io;
using namespace std_assembly::shared;
using namespace Types;
using namespace Ui;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CLASS_COPY 1
#define CLASS_PASTE 2

#define PASTE_SAIVLDATA 1
#define PASTE_LEVELDATA 2
#define PASTE_BATTLEGFX 4
#define PASTE_MAPMANGFX 8
#define PASTE_MAPMANPAL 16

#define CONTEXT_CHECKED 1
#define CONTEXT_UNCHECKED 2

#define HoldMPCode "HoldMPCode"
#define CapMPCode "CapMPCode"


/////////////////////////////////////////////////////////////////////////////
// CClasses dialog
#define BaseClass CEditorWithBackground

//PROTECTED
CClasses::CClasses(UINT idd, CWnd* pParent)
	: BaseClass(idd, pParent)
{
}

CClasses::CClasses(CWnd* pParent /*= nullptr */)
	: BaseClass(CClasses::IDD, pParent)
{
}

bool CClasses::WantsToReload() const
{
	return m_bRequestReload;
}

void CClasses::DoDataExchange(CDataExchange* pDX)
{
	BaseClass::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HITPCTUP, m_hitpctup);
	DDX_Control(pDX, IDC_MEAN, m_mean);
	DDX_Control(pDX, IDC_VIT4, m_vit4);
	DDX_Control(pDX, IDC_VIT2, m_vit2);
	DDX_Control(pDX, IDC_VIT3, m_vit3);
	DDX_Control(pDX, IDC_VIT1, m_vit1);
	DDX_Control(pDX, IDC_TOTALVIT4, m_totalvit4);
	DDX_Control(pDX, IDC_TOTALVIT3, m_totalvit3);
	DDX_Control(pDX, IDC_TOTALVIT2, m_totalvit2);
	DDX_Control(pDX, IDC_TOTALVIT1, m_totalvit1);
	DDX_Control(pDX, IDC_TOTALSTR4, m_totalstr4);
	DDX_Control(pDX, IDC_TOTALSTR3, m_totalstr3);
	DDX_Control(pDX, IDC_TOTALSTR2, m_totalstr2);
	DDX_Control(pDX, IDC_TOTALSTR1, m_totalstr1);
	DDX_Control(pDX, IDC_TOTALMAG4, m_totalmag4);
	DDX_Control(pDX, IDC_TOTALMAG3, m_totalmag3);
	DDX_Control(pDX, IDC_TOTALMAG2, m_totalmag2);
	DDX_Control(pDX, IDC_TOTALMAG1, m_totalmag1);
	DDX_Control(pDX, IDC_TOTALLUC4, m_totalluc4);
	DDX_Control(pDX, IDC_TOTALLUC3, m_totalluc3);
	DDX_Control(pDX, IDC_TOTALLUC2, m_totalluc2);
	DDX_Control(pDX, IDC_TOTALLUC1, m_totalluc1);
	DDX_Control(pDX, IDC_TOTALINT4, m_totalint4);
	DDX_Control(pDX, IDC_TOTALINT3, m_totalint3);
	DDX_Control(pDX, IDC_TOTALINT2, m_totalint2);
	DDX_Control(pDX, IDC_TOTALINT1, m_totalint1);
	DDX_Control(pDX, IDC_TOTALHP4, m_totalhp4);
	DDX_Control(pDX, IDC_TOTALHP3, m_totalhp3);
	DDX_Control(pDX, IDC_TOTALHP2, m_totalhp2);
	DDX_Control(pDX, IDC_TOTALHP1, m_totalhp1);
	DDX_Control(pDX, IDC_TOTALAGI4, m_totalagi4);
	DDX_Control(pDX, IDC_TOTALAGI3, m_totalagi3);
	DDX_Control(pDX, IDC_TOTALAGI2, m_totalagi2);
	DDX_Control(pDX, IDC_TOTALAGI1, m_totalagi1);
	DDX_Control(pDX, IDC_STRONG4, m_strong4);
	DDX_Control(pDX, IDC_STRONG3, m_strong3);
	DDX_Control(pDX, IDC_STRONG2, m_strong2);
	DDX_Control(pDX, IDC_STRONG1, m_strong1);
	DDX_Control(pDX, IDC_STR4, m_str4);
	DDX_Control(pDX, IDC_STR3, m_str3);
	DDX_Control(pDX, IDC_STR2, m_str2);
	DDX_Control(pDX, IDC_STR1, m_str1);
	DDX_Control(pDX, IDC_SCROLL, m_scroll);
	DDX_Control(pDX, IDC_MIN, m_min);
	DDX_Control(pDX, IDC_MAX, m_max);
	DDX_Control(pDX, IDC_MAGIC, m_magic);
	DDX_Control(pDX, IDC_LUC4, m_luc4);
	DDX_Control(pDX, IDC_LUC3, m_luc3);
	DDX_Control(pDX, IDC_LUC2, m_luc2);
	DDX_Control(pDX, IDC_LUC1, m_luc1);
	DDX_Control(pDX, IDC_LEVEL4, m_level4);
	DDX_Control(pDX, IDC_LEVEL3, m_level3);
	DDX_Control(pDX, IDC_LEVEL2, m_level2);
	DDX_Control(pDX, IDC_LEVEL1, m_level1);
	DDX_Control(pDX, IDC_INT4, m_int4);
	DDX_Control(pDX, IDC_INT3, m_int3);
	DDX_Control(pDX, IDC_INT2, m_int2);
	DDX_Control(pDX, IDC_INT1, m_int1);
	DDX_Control(pDX, IDC_HPRANGEMIN, m_hprangemin);
	DDX_Control(pDX, IDC_HPRANGEMAX, m_hprangemax);
	DDX_Control(pDX, IDC_EXP4, m_exp4);
	DDX_Control(pDX, IDC_EXP3, m_exp3);
	DDX_Control(pDX, IDC_EXP2, m_exp2);
	DDX_Control(pDX, IDC_EXP1, m_exp1);
	DDX_Control(pDX, IDC_AGI4, m_agi4);
	DDX_Control(pDX, IDC_AGI3, m_agi3);
	DDX_Control(pDX, IDC_AGI2, m_agi2);
	DDX_Control(pDX, IDC_AGI1, m_agi1);
	DDX_Control(pDX, IDC_8M4, m_8m4);
	DDX_Control(pDX, IDC_8M3, m_8m3);
	DDX_Control(pDX, IDC_8M2, m_8m2);
	DDX_Control(pDX, IDC_8M1, m_8m1);
	DDX_Control(pDX, IDC_7M4, m_7m4);
	DDX_Control(pDX, IDC_7M3, m_7m3);
	DDX_Control(pDX, IDC_7M2, m_7m2);
	DDX_Control(pDX, IDC_7M1, m_7m1);
	DDX_Control(pDX, IDC_6M4, m_6m4);
	DDX_Control(pDX, IDC_6M3, m_6m3);
	DDX_Control(pDX, IDC_6M2, m_6m2);
	DDX_Control(pDX, IDC_6M1, m_6m1);
	DDX_Control(pDX, IDC_5M4, m_5m4);
	DDX_Control(pDX, IDC_5M3, m_5m3);
	DDX_Control(pDX, IDC_5M2, m_5m2);
	DDX_Control(pDX, IDC_5M1, m_5m1);
	DDX_Control(pDX, IDC_4M4, m_4m4);
	DDX_Control(pDX, IDC_4M3, m_4m3);
	DDX_Control(pDX, IDC_4M2, m_4m2);
	DDX_Control(pDX, IDC_4M1, m_4m1);
	DDX_Control(pDX, IDC_3M4, m_3m4);
	DDX_Control(pDX, IDC_3M3, m_3m3);
	DDX_Control(pDX, IDC_3M2, m_3m2);
	DDX_Control(pDX, IDC_3M1, m_3m1);
	DDX_Control(pDX, IDC_2M4, m_2m4);
	DDX_Control(pDX, IDC_2M3, m_2m3);
	DDX_Control(pDX, IDC_2M2, m_2m2);
	DDX_Control(pDX, IDC_2M1, m_2m1);
	DDX_Control(pDX, IDC_1M4, m_1m4);
	DDX_Control(pDX, IDC_1M3, m_1m3);
	DDX_Control(pDX, IDC_1M2, m_1m2);
	DDX_Control(pDX, IDC_1M1, m_1m1);
	DDX_Control(pDX, IDC_VIT, m_vit);
	DDX_Control(pDX, IDC_UNKNOWN, m_classid);
	DDX_Control(pDX, IDC_STR, m_str);
	DDX_Control(pDX, IDC_STARTINGHP, m_startinghp);
	DDX_Control(pDX, IDC_MAGDEF, m_magdef);
	DDX_Control(pDX, IDC_LUCK, m_luck);
	DDX_Control(pDX, IDC_INT, m_int);
	DDX_Control(pDX, IDC_HIT, m_hit);
	DDX_Control(pDX, IDC_EVADEPCT, m_evade);
	DDX_Control(pDX, IDC_CLASSLIST, m_classlist);
	DDX_Control(pDX, IDC_BASEDAMAGE, m_damage);
	DDX_Control(pDX, IDC_AGILITY, m_agility);
	DDX_Control(pDX, IDC_CLASS_GROUP_HOLDMPMAX, m_groupHoldMpMax);
	DDX_Control(pDX, IDC_CLASS_EDIT_HOLDMPMAX, m_editHoldMpMax);
	DDX_Control(pDX, IDC_CLASS_EDIT_MPMAX, m_editMpMaxAllChars);
	DDX_Control(pDX, IDC_CLASSES_MAGDEFUP, m_editMagDefUp);
	DDX_Control(pDX, IDC_COMBO_MPRANGEMIN, m_mpmincombo);
	DDX_Control(pDX, IDC_COMBO_MPRANGEMAX, m_mpmaxcombo);
	DDX_Control(pDX, IDC_CLASSES_INFO, m_infobutton);
	DDX_Control(pDX, IDC_CLASSCOMBO1, m_comboclass1);
	DDX_Control(pDX, IDC_CLASSCOMBO2, m_comboclass2);
	DDX_Control(pDX, IDC_POSTCOMBO1, m_combopost1);
	DDX_Control(pDX, IDC_POSTCOMBO2, m_combopost2);
	DDX_Control(pDX, IDC_BBMA1, m_bbmacombo1);
	DDX_Control(pDX, IDC_BBMA2, m_bbmacombo2);
	DDX_Control(pDX, IDC_2XHITS1, m_2xhitscombo1);
	DDX_Control(pDX, IDC_2XHITS2, m_2xhitscombo2);
	DDX_Control(pDX, IDC_HPMAX, m_maxhpedit);
	DDX_Control(pDX, IDC_MAPMANPIC, m_mapmanpicbutton);
	DDX_Control(pDX, IDC_BATTLEPIC, m_editpatpicbutton);
	DDX_Control(pDX, IDC_CLASSES_SETTINGS, m_settingsbutton);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDC_SAVE, m_savebutton);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
}


BEGIN_MESSAGE_MAP(CClasses, BaseClass)
	ON_LBN_SELCHANGE(IDC_CLASSLIST, OnSelchangeClasslist)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_MAX, OnMax)
	ON_BN_CLICKED(IDC_MIN, OnMin)
	ON_EN_CHANGE(IDC_STARTINGHP, RecalculateRunningTotals)
	ON_EN_CHANGE(IDC_HPRANGEMAX, OnChangeHprangemax)
	ON_EN_CHANGE(IDC_HPRANGEMIN, OnChangeHprangemin)
	ON_BN_CLICKED(IDC_1M1, OnCheckBox)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_MEAN, OnMean)
	ON_BN_CLICKED(IDC_BATTLEPIC, OnBattlepic)
	ON_EN_CHANGE(IDC_STR, RecalculateRunningTotals)
	ON_EN_CHANGE(IDC_VIT, RecalculateRunningTotals)
	ON_EN_CHANGE(IDC_AGILITY, RecalculateRunningTotals)
	ON_EN_CHANGE(IDC_LUCK, RecalculateRunningTotals)
	ON_EN_CHANGE(IDC_INT, RecalculateRunningTotals)
	ON_EN_CHANGE(IDC_MAGIC, RecalculateRunningTotals)
	ON_EN_CHANGE(IDC_CLASS_EDIT_HOLDMPMAX, RecalculateRunningTotals)
	ON_EN_CHANGE(IDC_CLASS_EDIT_MPMAX, RecalculateRunningTotals)
	ON_BN_CLICKED(IDC_1M2, OnCheckBox)
	ON_BN_CLICKED(IDC_1M3, OnCheckBox)
	ON_BN_CLICKED(IDC_1M4, OnCheckBox)
	ON_BN_CLICKED(IDC_2M1, OnCheckBox)
	ON_BN_CLICKED(IDC_2M2, OnCheckBox)
	ON_BN_CLICKED(IDC_2M3, OnCheckBox)
	ON_BN_CLICKED(IDC_2M4, OnCheckBox)
	ON_BN_CLICKED(IDC_3M1, OnCheckBox)
	ON_BN_CLICKED(IDC_3M2, OnCheckBox)
	ON_BN_CLICKED(IDC_3M3, OnCheckBox)
	ON_BN_CLICKED(IDC_3M4, OnCheckBox)
	ON_BN_CLICKED(IDC_4M1, OnCheckBox)
	ON_BN_CLICKED(IDC_4M2, OnCheckBox)
	ON_BN_CLICKED(IDC_4M3, OnCheckBox)
	ON_BN_CLICKED(IDC_4M4, OnCheckBox)
	ON_BN_CLICKED(IDC_5M1, OnCheckBox)
	ON_BN_CLICKED(IDC_5M2, OnCheckBox)
	ON_BN_CLICKED(IDC_5M3, OnCheckBox)
	ON_BN_CLICKED(IDC_5M4, OnCheckBox)
	ON_BN_CLICKED(IDC_6M1, OnCheckBox)
	ON_BN_CLICKED(IDC_6M2, OnCheckBox)
	ON_BN_CLICKED(IDC_6M3, OnCheckBox)
	ON_BN_CLICKED(IDC_6M4, OnCheckBox)
	ON_BN_CLICKED(IDC_7M1, OnCheckBox)
	ON_BN_CLICKED(IDC_7M2, OnCheckBox)
	ON_BN_CLICKED(IDC_7M3, OnCheckBox)
	ON_BN_CLICKED(IDC_7M4, OnCheckBox)
	ON_BN_CLICKED(IDC_8M1, OnCheckBox)
	ON_BN_CLICKED(IDC_8M2, OnCheckBox)
	ON_BN_CLICKED(IDC_8M3, OnCheckBox)
	ON_BN_CLICKED(IDC_8M4, OnCheckBox)
	ON_BN_CLICKED(IDC_AGI1, OnCheckBox)
	ON_BN_CLICKED(IDC_AGI2, OnCheckBox)
	ON_BN_CLICKED(IDC_AGI3, OnCheckBox)
	ON_BN_CLICKED(IDC_AGI4, OnCheckBox)
	ON_BN_CLICKED(IDC_STR1, OnCheckBox)
	ON_BN_CLICKED(IDC_STR2, OnCheckBox)
	ON_BN_CLICKED(IDC_STR3, OnCheckBox)
	ON_BN_CLICKED(IDC_STR4, OnCheckBox)
	ON_BN_CLICKED(IDC_INT1, OnCheckBox)
	ON_BN_CLICKED(IDC_INT2, OnCheckBox)
	ON_BN_CLICKED(IDC_INT3, OnCheckBox)
	ON_BN_CLICKED(IDC_INT4, OnCheckBox)
	ON_BN_CLICKED(IDC_LUC1, OnCheckBox)
	ON_BN_CLICKED(IDC_LUC2, OnCheckBox)
	ON_BN_CLICKED(IDC_LUC3, OnCheckBox)
	ON_BN_CLICKED(IDC_LUC4, OnCheckBox)
	ON_BN_CLICKED(IDC_VIT1, OnCheckBox)
	ON_BN_CLICKED(IDC_VIT2, OnCheckBox)
	ON_BN_CLICKED(IDC_VIT3, OnCheckBox)
	ON_BN_CLICKED(IDC_VIT4, OnCheckBox)
	ON_BN_CLICKED(IDC_STRONG1, OnCheckBox)
	ON_BN_CLICKED(IDC_STRONG2, OnCheckBox)
	ON_BN_CLICKED(IDC_STRONG3, OnCheckBox)
	ON_BN_CLICKED(IDC_STRONG4, OnCheckBox)
	ON_BN_CLICKED(IDC_MAPMANPIC, OnMapmanpic)
	ON_WM_MOUSEWHEEL()
	ON_CBN_SELCHANGE(IDC_COMBO_MPRANGEMIN, &CClasses::OnCbnSelchangeComboMprangemin)
	ON_CBN_SELCHANGE(IDC_COMBO_MPRANGEMAX, &CClasses::OnCbnSelchangeComboMprangemax)
	ON_BN_CLICKED(IDC_CLASSES_INFO, &CClasses::OnBnClickedClassesInfo)
	ON_WM_CONTEXTMENU()
	ON_CBN_SELCHANGE(IDC_CLASSCOMBO1, &CClasses::OnCbnSelchangeClasscombo1)
	ON_CBN_SELCHANGE(IDC_CLASSCOMBO2, &CClasses::OnCbnSelchangeClasscombo2)
	ON_CBN_SELCHANGE(IDC_POSTCOMBO1, &CClasses::OnCbnSelchangePostcombo1)
	ON_CBN_SELCHANGE(IDC_POSTCOMBO2, &CClasses::OnCbnSelchangePostcombo2)
	ON_BN_CLICKED(IDC_CLASSES_SETTINGS, &CClasses::OnBnClickedClassesSettings)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CClasses message handlers

BOOL CClasses::OnInitDialog() 
{
	BaseClass::OnInitDialog();
	
	m_bRequestReload = false;

	try {
		InitButtons();
		LoadRom();

		ar_1m[0] = &m_1m1;ar_1m[1] = &m_1m2;ar_1m[2] = &m_1m3;ar_1m[3] = &m_1m4;ar_2m[0] = &m_2m1;
		ar_2m[1] = &m_2m2;ar_2m[2] = &m_2m3;ar_2m[3] = &m_2m4;ar_3m[0] = &m_3m1;ar_3m[1] = &m_3m2;
		ar_3m[2] = &m_3m3;ar_3m[3] = &m_3m4;ar_4m[0] = &m_4m1;ar_4m[1] = &m_4m2;ar_4m[2] = &m_4m3;
		ar_4m[3] = &m_4m4;ar_5m[0] = &m_5m1;ar_5m[1] = &m_5m2;ar_5m[2] = &m_5m3;ar_5m[3] = &m_5m4;
		ar_6m[0] = &m_6m1;ar_6m[1] = &m_6m2;ar_6m[2] = &m_6m3;ar_6m[3] = &m_6m4;ar_7m[0] = &m_7m1;
		ar_7m[1] = &m_7m2;ar_7m[2] = &m_7m3;ar_7m[3] = &m_7m4;ar_8m[0] = &m_8m1;ar_8m[1] = &m_8m2;
		ar_8m[2] = &m_8m3;ar_8m[3] = &m_8m4;ar_strong[0] = &m_strong1;ar_strong[1] = &m_strong2;
		ar_strong[2] = &m_strong3;ar_strong[3] = &m_strong4;
		ar_str[0] = &m_str1;ar_str[1] = &m_str2;ar_str[2] = &m_str3;ar_str[3] = &m_str4;
		ar_agi[0] = &m_agi1;ar_agi[1] = &m_agi2;ar_agi[2] = &m_agi3;ar_agi[3] = &m_agi4;
		ar_int[0] = &m_int1;ar_int[1] = &m_int2;ar_int[2] = &m_int3;ar_int[3] = &m_int4;
		ar_vit[0] = &m_vit1;ar_vit[1] = &m_vit2;ar_vit[2] = &m_vit3;ar_vit[3] = &m_vit4;
		ar_luc[0] = &m_luc1;ar_luc[1] = &m_luc2;ar_luc[2] = &m_luc3;ar_luc[3] = &m_luc4;
		ar_exp[0] = &m_exp1;ar_exp[1] = &m_exp2;ar_exp[2] = &m_exp3;ar_exp[3] = &m_exp4;
		ar_level[0] = &m_level1;ar_level[1] = &m_level2;ar_level[2] = &m_level3;ar_level[3] = &m_level4;

		ar_totalhp[0] = &m_totalhp1;ar_totalhp[1] = &m_totalhp2;ar_totalhp[2] = &m_totalhp3;ar_totalhp[3] = &m_totalhp4;
		ar_totalstr[0] = &m_totalstr1;ar_totalstr[1] = &m_totalstr2;ar_totalstr[2] = &m_totalstr3;ar_totalstr[3] = &m_totalstr4;
		ar_totalagi[0] = &m_totalagi1;ar_totalagi[1] = &m_totalagi2;ar_totalagi[2] = &m_totalagi3;ar_totalagi[3] = &m_totalagi4;
		ar_totalint[0] = &m_totalint1;ar_totalint[1] = &m_totalint2;ar_totalint[2] = &m_totalint3;ar_totalint[3] = &m_totalint4;
		ar_totalvit[0] = &m_totalvit1;ar_totalvit[1] = &m_totalvit2;ar_totalvit[2] = &m_totalvit3;ar_totalvit[3] = &m_totalvit4;
		ar_totalluc[0] = &m_totalluc1;ar_totalluc[1] = &m_totalluc2;ar_totalluc[2] = &m_totalluc3;ar_totalluc[3] = &m_totalluc4;
		ar_totalmagic[0] = &m_totalmag1;ar_totalmagic[1] = &m_totalmag2;ar_totalmagic[2] = &m_totalmag3;ar_totalmagic[3] = &m_totalmag4;

		LoadListBox(m_classlist, LoadClassEntries(*Project));
		ViewScale = 1;
		m_max.SetCheck(0);
		m_min.SetCheck(0);
		m_mean.SetCheck(1);

		level_offset = 0;
		m_scroll.SetScrollRange(0, 45);
		m_scroll.SetScrollPos(0);

		cur_class = -1;
		m_classlist.SetCurSel(0);
		OnSelchangeClasslist();
	}
	catch (std::exception & ex) {
		return AbortFail(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return AbortFail(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}

void CClasses::InitButtons()
{
	m_banner.Set(this, RGB(0, 0, 0), RGB(255, 32, 64), "Classes");
}

void CClasses::LoadRom()
{
	CWaitCursor wait;

	Project->ClearROM();

	CLASS_COUNT = ReadDec(Project->ValuesPath, "CLASS_COUNT");
	CLASS_OFFSET = ReadHex(Project->ValuesPath, "CLASS_OFFSET");
	CLASS_BYTES = ReadDec(Project->ValuesPath, "CLASS_BYTES");
	HITPCTINCREASE_OFFSET = ReadHex(Project->ValuesPath, "HITPCTINCREASE_OFFSET");
	MAGDEFINCREASE_OFFSET = ReadHex(Project->ValuesPath, "MAGDEFINCREASE_OFFSET");
	STARTINGMP_OFFSET = ReadHex(Project->ValuesPath, "STARTINGMP_OFFSET");
	MPCAP_OFFSET = ReadHex(Project->ValuesPath, "MPCAP_OFFSET");
	MAGICMIN_OFFSET = ReadHex(Project->ValuesPath, "MAGICMIN_OFFSET");
	MAGICMAX_OFFSET = ReadHex(Project->ValuesPath, "MAGICMAX_OFFSET");
	HPMAX_OFFSET = ReadHex(Project->ValuesPath, "HPMAX_OFFSET");
	HPRANGEMIN_OFFSET = ReadHex(Project->ValuesPath, "HPRANGEMIN_OFFSET");
	HPRANGEMAX_OFFSET = ReadHex(Project->ValuesPath, "HPRANGEMAX_OFFSET");
	LEVELUP_OFFSET = ReadHex(Project->ValuesPath, "LEVELUP_OFFSET");
	EXP_OFFSET = ReadHex(Project->ValuesPath, "EXP_OFFSET");
	MAPMANGRAPHIC_OFFSET = ReadHex(Project->ValuesPath, "MAPMANGRAPHIC_OFFSET");

	HOLDMPCODE_OFFSET = ReadHex(Project->ValuesPath, "HOLDMPCODE_OFFSET");
	HOLDMPCLASS1_OFFSET = ReadHex(Project->ValuesPath, "HOLDMPCLASS1_OFFSET");
	HOLDMPCLASS2_OFFSET = ReadHex(Project->ValuesPath, "HOLDMPCLASS2_OFFSET");
	HOLDMPCLASSID2_OFFSET = ReadHex(Project->ValuesPath, "HOLDMPCLASSID2_OFFSET");

	CAPMPCODE_OFFSET = ReadHex(Project->ValuesPath, "CAPMPCODE_OFFSET");
	CAPMPCLASS1_OFFSET = ReadHex(Project->ValuesPath, "CAPMPCLASS1_OFFSET");
	CAPMPCLASSID1_OFFSET = ReadHex(Project->ValuesPath, "CAPMPCLASSID1_OFFSET");
	CAPMPCLASS2_OFFSET = ReadHex(Project->ValuesPath, "CAPMPCLASS2_OFFSET");
	CAPMPCLASSID2_OFFSET = ReadHex(Project->ValuesPath, "CAPMPCLASSID2_OFFSET");
	CAPMPLIMIT_OFFSET = ReadHex(Project->ValuesPath, "CAPMPLIMIT_OFFSET");

	BANK0A_OFFSET = ReadHex(Project->ValuesPath, "BANK0A_OFFSET");
	BANK00_OFFSET = ReadHex(Project->ValuesPath, "BANK00_OFFSET");
	BANK02_OFFSET = ReadHex(Project->ValuesPath, "BANK02_OFFSET");

	BBMACLASS1STATS_OFFSET = ReadHex(Project->ValuesPath, "BBMACLASS1STATS_OFFSET");
	BBMACLASS1UNADJ_OFFSET = ReadHex(Project->ValuesPath, "BBMACLASS1UNADJ_OFFSET");
	BBMACLASS1READJ_OFFSET = ReadHex(Project->ValuesPath, "BBMACLASS1READJ_OFFSET");
	BBMACLASS1HITS_OFFSET = ReadHex(Project->ValuesPath, "BBMACLASS1HITS_OFFSET");
	BBMACLASS2STATS_OFFSET = ReadHex(Project->ValuesPath, "BBMACLASS2STATS_OFFSET");
	BBMACLASS2UNADJ_OFFSET = ReadHex(Project->ValuesPath, "BBMACLASS2UNADJ_OFFSET");
	BBMACLASS2READJ_OFFSET = ReadHex(Project->ValuesPath, "BBMACLASS2READJ_OFFSET");
	BBMACLASS2HITS_OFFSET = ReadHex(Project->ValuesPath, "BBMACLASS2HITS_OFFSET");

	CHARBATTLEPALETTE_ASSIGNMENT1 = ReadHex(Project->ValuesPath, "CHARBATTLEPALETTE_ASSIGNMENT1");
	CHARBATTLEPALETTE_ASSIGNMENT2 = ReadHex(Project->ValuesPath, "CHARBATTLEPALETTE_ASSIGNMENT2");
	CHARBATTLEPIC_OFFSET = ReadHex(Project->ValuesPath, "CHARBATTLEPIC_OFFSET");
	CHARBATTLEPALETTE_OFFSET = ReadHex(Project->ValuesPath, "CHARBATTLEPALETTE_OFFSET");
	BINBANK09DATA_OFFSET = ReadHex(Project->ValuesPath, "BINBANK09DATA_OFFSET");
	BINBANK09GFXDATA_OFFSET = ReadHex(Project->ValuesPath, "BINBANK09GFXDATA_OFFSET");

	OVERWORLDPALETTE_OFFSET = ReadHex(Project->ValuesPath, "OVERWORLDPALETTE_OFFSET");
	MAPMANPALETTE_OFFSET = ReadHex(Project->ValuesPath, "MAPMANPALETTE_OFFSET");

	BATTLESPRITETILE_COUNT = ReadDec(Project->ValuesPath, "BATTLESPRITETILE_COUNT");
	BATTLESPRITETILE_BYTES = ReadDec(Project->ValuesPath, "BATTLESPRITETILE_BYTES");
	MAPMANSPRITETILE_COUNT = ReadDec(Project->ValuesPath, "MAPMANSPRITETILE_COUNT");
	MAPMANSPRITETILE_BYTES = ReadDec(Project->ValuesPath, "MAPMANSPRITETILE_BYTES");

	// Now load the data
	GameSerializer ser(*Project);

	if (Project->IsRom()) {
		load_binary(Project->WorkRomPath, Project->ROM);
		LoadClassEngineData(ser);
	}
	else if (Project->IsAsm()) {
		// Instead of writing to the entire buffer, just write to the parts we need
		CWaitingDlg waiting(this);
		waiting.Init();
		waiting.SetMessage("Loading classes' data...");
		ser.MainWindow = &waiting;

		ser.LoadAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.LoadAsmBin(BANK_00, BANK00_OFFSET);
		ser.LoadAsmBin(BANK_02, BANK02_OFFSET);
		ser.LoadAsmBin(BIN_LEVELUPDATA, LEVELUP_OFFSET);
		ser.LoadAsmBin(BIN_BANK09DATA, BINBANK09DATA_OFFSET);
		ser.LoadAsmBin(BIN_BANK09GFXDATA, BINBANK09GFXDATA_OFFSET);
		ser.LoadInline(ASM_0B, {
			{ asmlabel, "lut_LvlUpHitRateBonus", { HITPCTINCREASE_OFFSET } },
			{ asmlabel, "data_MaxHPPlusOne", { HPMAX_OFFSET }},
			{ asmlabel, "lut_ExpToAdvance", { EXP_OFFSET } },
			{ asmlabel, "lut_LvlUpMagDefBonus", { MAGDEFINCREASE_OFFSET } },
			{ asmopval, "op_MpCap", { MPCAP_OFFSET } },
			{ asmopval, "op_HpStrongRangeMin", { HPRANGEMIN_OFFSET } },
			{ asmopval, "op_HpStrongRangeMax", { HPRANGEMAX_OFFSET } },
		});

		// Battle sprites
		ser.LoadInline(ASM_0C, { { asmlabel, LUT_INBATTLECHARPALETTEASSIGN, { CHARBATTLEPALETTE_ASSIGNMENT1 }} });
		ser.LoadInline(ASM_0F, {
			{ asmopval, "op_StartingMp", {STARTINGMP_OFFSET} },
			{ asmlabel, LUT_BATTLESPRITEPALETTES, {CHARBATTLEPALETTE_OFFSET} },
			{ asmlabel, LUT_CLASSBATSPRPALETTE, {CHARBATTLEPALETTE_ASSIGNMENT2} }
		});

		waiting.SetMessage("Loading class engine data...");
		LoadClassEngineData(ser);
		waiting.DestroyWindow();
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, (LPCSTR)Project->ProjectTypeName);
	}
}

void CClasses::SaveRom()
{
	CWaitCursor wait;

	GameSerializer ser(*Project);

	if (Project->IsRom()) {
		SaveClassEngineData(ser);
		save_binary(Project->WorkRomPath, Project->ROM);
	}
	else if (Project->IsAsm()) {
		// Instead of writing to the entire buffer, just write to the parts we need
		CWaitingDlg waiting(this);
		waiting.Init();
		waiting.SetMessage("Saving class engine data...");
		SaveClassEngineData(ser);

		waiting.SetMessage("Saving classes' data...");
		ser.SaveAsmBin(BANK_00, BANK00_OFFSET);
		ser.SaveAsmBin(BANK_02, BANK02_OFFSET);
		ser.SaveAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.SaveAsmBin(BIN_LEVELUPDATA, LEVELUP_OFFSET);
		ser.SaveAsmBin(BIN_BANK09DATA, BINBANK09DATA_OFFSET);
		ser.SaveAsmBin(BIN_BANK09GFXDATA, BINBANK09GFXDATA_OFFSET);
		ser.SaveInline(ASM_0B, {
			{ asmlabel, "lut_LvlUpHitRateBonus", {HITPCTINCREASE_OFFSET} },
			{ asmlabel, "lut_ExpToAdvance", {EXP_OFFSET} },
			{ asmlabel, "data_MaxHPPlusOne",{ HPMAX_OFFSET } },
			{ asmlabel, "lut_LvlUpMagDefBonus", {MAGDEFINCREASE_OFFSET} },
			{ asmopval, "op_MpCap", {MPCAP_OFFSET} },
			{ asmopval, "op_HpStrongRangeMin", {HPRANGEMIN_OFFSET} },
			{ asmopval, "op_HpStrongRangeMax", {HPRANGEMAX_OFFSET} },
		});

		// Battle sprites
		ser.SaveInline(ASM_0C, { { asmlabel, LUT_INBATTLECHARPALETTEASSIGN, {CHARBATTLEPALETTE_ASSIGNMENT1} } });
		ser.SaveInline(ASM_0F, {
			{ asmopval, "op_StartingMp", {STARTINGMP_OFFSET} },
			{ asmlabel, LUT_BATTLESPRITEPALETTES, {CHARBATTLEPALETTE_OFFSET} },
			{ asmlabel, LUT_CLASSBATSPRPALETTE, {CHARBATTLEPALETTE_ASSIGNMENT2} }
		});
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, (LPCSTR)Project->ProjectTypeName);
	}
}

void CClasses::LoadClassEngineData(GameSerializer & ser)
{
	auto LoadIgnorableClassCombo = [this](CComboBox & box) {
		LoadCombo(box, LoadClassEntries(*Project));
		AddEntry(box, "<ignore>", CLASS_COUNT);
	};
	auto CopyRange = [](const bytevector & src, size_t offset, size_t count) {
		bytevector vret(count);
		for (size_t st = 0; st < count; ++st) vret[st] = src[offset + st];
		return vret;
	};
	auto EnableControls = [this](bool checked, std::vector<CWnd*> controls) {
		BOOL nChecked = checked == true;
		for (auto wnd : controls) wnd->EnableWindow(nChecked);
		return checked;
	};

	CClassesEditorSettings stgs(*Project);
	auto hold1 = EnableControls(!stgs.IgnoreHoldMP1, { &m_comboclass1 });
	auto hold2 = EnableControls(!stgs.IgnoreHoldMP2, { &m_comboclass2 });
	auto cap1 = EnableControls(!stgs.IgnoreCapMP1, { &m_combopost1 });
	auto cap2 = EnableControls(!stgs.IgnoreCapMP2, { &m_combopost2 });
	auto bbma1 = EnableControls(!stgs.IgnoreBBMA1, { &m_bbmacombo1, &m_2xhitscombo1 });
	auto bbma2 = EnableControls(!stgs.IgnoreBBMA2, { &m_bbmacombo2, &m_2xhitscombo2 });
	auto mprange = EnableControls(!stgs.IgnoreMPRange, { &m_mpmincombo, &m_mpmaxcombo });

	// Load the combos with the class names
	AddEntry(m_comboclass1, LoadClassEntry(*Project, 0), 0);
	AddEntry(m_comboclass1, "<ignore>", CLASS_COUNT);
	LoadIgnorableClassCombo(m_comboclass2);
	LoadIgnorableClassCombo(m_combopost1);
	LoadIgnorableClassCombo(m_combopost2);
	LoadIgnorableClassCombo(m_bbmacombo1);
	LoadIgnorableClassCombo(m_bbmacombo2);
	LoadIgnorableClassCombo(m_2xhitscombo1);
	LoadIgnorableClassCombo(m_2xhitscombo2);
	LoadCombo(m_mpmincombo, LoadClassEntries(*Project));
	AddEntry(m_mpmincombo, "<none>", CLASS_COUNT);
	LoadCombo(m_mpmaxcombo, LoadClassEntries(*Project));

	m_holdmpbytes.clear();
	m_capmpbytes.clear();

	m_infobutton.ShowWindow((!hold1 || !hold2 || !cap1 || !cap2 || !bbma1 || !bbma2 || !mprange) ? SW_SHOW : SW_HIDE);

	// If nothing needs to load, then there's no need to continue.
	if (!hold1 && !hold2 && !cap1 && !cap2 && !bbma1 && !bbma2 && !mprange)
		return;

	auto CompareBytes = [](const bytevector & left, const bytevector & right, size_t offset, size_t count) {
		bool match = !left.empty() && !right.empty();
		for (size_t st = 0, index = offset; st < count; ++st) match = left[index] == right[index];
		return match;
	};
	auto SelectMPRangeClass = [this](CComboBox & box, int classid, int defaultid = 0) {
		if (classid < 0 || classid >= box.GetCount())
			classid = defaultid;
		box.SetCurSel(classid);
	};

	if (Project->IsRom()) {
		// If a check box is unchecked, the bytes for the related field are not modified.
		// Otherwise; if the bytes match, the class is selected, else <ignore> is selected.
		if (hold1 || hold2) {
			auto holdlength = ReadDec(Project->ValuesPath, "HoldMPCodeLength");
			auto holdmpbytes = ReadIni(Project->ValuesPath, HoldMPCode, PROP_VALUE, bytevector{});
			m_holdmpbytes = CopyRange(Project->ROM, HOLDMPCODE_OFFSET, holdlength);

			ASSERT(holdmpbytes.size() == m_holdmpbytes.size());
			if (holdlength == 0) THROWEXCEPTION(std::runtime_error, "HoldMP length cannot be zero length.");
			if (holdlength != (int)holdmpbytes.size()) THROWEXCEPTION(std::runtime_error, "HoldMP Values bytes length doesn't match the declared hold length.");

			if (hold1) {
				//DEVNOTE = by default, HoldMP1 implies the first class due to the structure of the assembly source code.
				auto matched = CompareBytes(m_holdmpbytes, holdmpbytes, HOLDMPCLASS1_OFFSET - HOLDMPCODE_OFFSET, 2);
				auto classid = matched ? 0 : CLASS_COUNT;
				SelectItemByData(m_comboclass1, classid);
			}
			if (hold2) {
				auto matched = CompareBytes(m_holdmpbytes, holdmpbytes, HOLDMPCLASS2_OFFSET - HOLDMPCODE_OFFSET, 2);
				auto classid = matched ? m_holdmpbytes[HOLDMPCLASSID2_OFFSET - HOLDMPCODE_OFFSET] : CLASS_COUNT;
				SelectItemByData(m_comboclass2, classid);
			}
		}
		if (cap1 || cap2) {
			auto caplength = ReadDec(Project->ValuesPath, "CapMPCodeLength");
			auto capmpbytes = ReadIni(Project->ValuesPath, CapMPCode, PROP_VALUE, bytevector{});
			m_capmpbytes = CopyRange(Project->ROM, CAPMPCODE_OFFSET, caplength);

			ASSERT(capmpbytes.size() == m_capmpbytes.size());
			if (caplength == 0) THROWEXCEPTION(std::runtime_error, "CapMP length cannot be zero length.");
			if (caplength != (int)capmpbytes.size()) THROWEXCEPTION(std::runtime_error, "CapMP Values bytes length doesn't match the declared cap length.");

			if (cap1) {
				auto matched = CompareBytes(m_capmpbytes, capmpbytes, CAPMPCLASS1_OFFSET - CAPMPCODE_OFFSET, 2);
				auto classid = matched ? m_capmpbytes[CAPMPCLASSID1_OFFSET - CAPMPCODE_OFFSET] : CLASS_COUNT;
				SelectItemByData(m_combopost1, classid);
			}
			if (cap2) {
				auto matched = CompareBytes(m_capmpbytes, capmpbytes, CAPMPCLASS2_OFFSET - CAPMPCODE_OFFSET, 2);
				auto classid = matched ? m_capmpbytes[CAPMPCLASSID2_OFFSET - CAPMPCODE_OFFSET] : CLASS_COUNT;
				SelectItemByData(m_combopost2, classid);
			}
		}

		// For BBMA, unadjust and readjust aren't read, as they are synched to stats when saved.
		if (bbma1) {
			auto statid = Project->ROM[BBMACLASS1STATS_OFFSET];
			if (!SelectItemByData(m_bbmacombo1, statid))
				if (m_bbmacombo1.GetCount() > 0) m_bbmacombo1.SetCurSel(0);
			auto hitid = Project->ROM[BBMACLASS1HITS_OFFSET];
			if (!SelectItemByData(m_2xhitscombo1, hitid))
				if (m_2xhitscombo1.GetCount() > 0) m_2xhitscombo1.SetCurSel(0);
		}
		if (bbma2) {
			auto statid = Project->ROM[BBMACLASS2STATS_OFFSET];
			if (!SelectItemByData(m_bbmacombo2, statid))
				if (m_bbmacombo2.GetCount() > 0) m_bbmacombo2.SetCurSel(0);
			auto hitid = Project->ROM[BBMACLASS2HITS_OFFSET];
			if (!SelectItemByData(m_2xhitscombo2, hitid))
				if (m_2xhitscombo2.GetCount() > 0) m_2xhitscombo2.SetCurSel(0);
		}

		if (mprange) {
			auto minid = Project->ROM[MAGICMIN_OFFSET];
			SelectMPRangeClass(m_mpmincombo, minid);
			SelectMPRangeClass(m_mpmaxcombo, Project->ROM[MAGICMAX_OFFSET] - 1, minid); // end range is exclusive, subtract 1 to get the class ID
		}
	}
	else if (Project->IsAsm()) {
		Project->ROM[HOLDMPCLASSID2_OFFSET] = (unsigned char)CLASS_COUNT; //(unsigned char)NopResult::Error;
		asmsourcemappingvector vasminlineb, vasminlinec, vasminlinef;
		if (hold1) {
			vasminlineb.push_back({ asmcond, "nop_HoldMpClass1", {HOLDMPCLASS1_OFFSET} });
		}
		if (hold2) {
			vasminlineb.push_back({ asmcond, "nop_HoldMpClass2", {HOLDMPCLASS2_OFFSET} });
			vasminlineb.push_back({ asmopval, "op_HoldMpClass2", {HOLDMPCLASSID2_OFFSET} });
		}
		if (cap1) {
			vasminlineb.push_back({ asmcond, "nop_HoldMpClassMpIncrement1", {CAPMPCLASS1_OFFSET} });
			vasminlineb.push_back({ asmopval, "op_HoldMpClassMpIncrement1", {CAPMPCLASSID1_OFFSET} });
		}
		if (cap2) {
			vasminlineb.push_back({ asmcond, "nop_HoldMpClassMpIncrement2", {CAPMPCLASS2_OFFSET} });
			vasminlineb.push_back({ asmopval, "op_HoldMpClassMpIncrement2", {CAPMPCLASSID2_OFFSET} });
		}
		if (cap1 || cap2) {
			vasminlineb.push_back({ asmopval, "op_HoldMpMpCap", {CAPMPLIMIT_OFFSET} });
		}
		// For BBMA, only load the stats values, the others (unadjust and readjust) will sync to it when saved.
		if (bbma1) {
			vasminlineb.push_back({ asmopval, "op_BBMAClass1Stats", {BBMACLASS1STATS_OFFSET} });
			vasminlinec.push_back({ asmopval, "op_BBMAClass1Hits", {BBMACLASS1HITS_OFFSET} });
		}
		if (bbma2) {
			vasminlineb.push_back({ asmopval, "op_BBMAClass2Stats", {BBMACLASS2STATS_OFFSET} });
			vasminlinec.push_back({ asmopval, "op_BBMAClass2Hits", {BBMACLASS2HITS_OFFSET} });
		}
		if (mprange) {
			vasminlinef.push_back({ asmopval, "op_MagicRangeMin", {MAGICMIN_OFFSET} });
			vasminlinef.push_back({ asmopval, "op_MagicRangeMax", {MAGICMAX_OFFSET} });
		}

		using asmnode = std::pair<asmsourcemappingvector*, CString>;
		auto thelist = std::vector<asmnode>{ {&vasminlineb, ASM_0B}, {&vasminlinec, ASM_0C}, {&vasminlinef, ASM_0F} };
		for (auto tl : thelist) {
			if (!tl.first->empty())
				ser.LoadInline(tl.second, *tl.first);
		}

		if (hold1) {
			//DEVNOTE = by default, HoldMP1 implies the first class due to the structure of the assembly source code.
			auto value = Project->ROM[HOLDMPCLASS1_OFFSET];
			auto matched = value == ASM_TRUE; //(unsigned char)NopResult::Match;
			auto classid = matched ? 0 : CLASS_COUNT;
			SelectItemByData(m_comboclass1, classid);
		}
		if (hold2) {
			auto value = Project->ROM[HOLDMPCLASS2_OFFSET];
			auto matched = value == ASM_TRUE;
			auto classid = matched ? Project->ROM[HOLDMPCLASSID2_OFFSET] : CLASS_COUNT;
			SelectItemByData(m_comboclass2, classid);
		}
		if (cap1) {
			//DEVNOTE = by default, HoldMP1 implies the first class due to the structure of the assembly source code.
			auto value = Project->ROM[CAPMPCLASS1_OFFSET];
			auto matched = value == ASM_TRUE;
			auto classid = matched ? Project->ROM[CAPMPCLASSID1_OFFSET] : CLASS_COUNT;
			SelectItemByData(m_combopost1, classid);
		}
		if (cap2) {
			auto value = Project->ROM[CAPMPCLASS2_OFFSET];
			auto matched = value == ASM_TRUE;
			auto classid = matched ? Project->ROM[CAPMPCLASSID2_OFFSET] : CLASS_COUNT;
			SelectItemByData(m_combopost2, classid);
		}
		// For BBMA, unadjust and readjust aren't read, as they are synched to stats when saved.
		if (bbma1) {
			auto statid = Project->ROM[BBMACLASS1STATS_OFFSET];
			if (!SelectItemByData(m_bbmacombo1, statid))
				if (m_bbmacombo1.GetCount() > 0) m_bbmacombo1.SetCurSel(0);
			auto hitid = Project->ROM[BBMACLASS1HITS_OFFSET];
			if (!SelectItemByData(m_2xhitscombo1, hitid))
				if (m_2xhitscombo1.GetCount() > 0) m_2xhitscombo1.SetCurSel(0);
		}
		if (bbma2) {
			auto statid = Project->ROM[BBMACLASS2STATS_OFFSET];
			if (!SelectItemByData(m_bbmacombo2, statid))
				if (m_bbmacombo2.GetCount() > 0) m_bbmacombo2.SetCurSel(0);
			auto hitid = Project->ROM[BBMACLASS2HITS_OFFSET];
			if (!SelectItemByData(m_2xhitscombo2, hitid))
				if (m_2xhitscombo2.GetCount() > 0) m_2xhitscombo2.SetCurSel(0);
		}

		if (mprange) {
			auto minid = Project->ROM[MAGICMIN_OFFSET];
			SelectMPRangeClass(m_mpmincombo, minid);
			SelectMPRangeClass(m_mpmaxcombo, Project->ROM[MAGICMAX_OFFSET] - 1, minid); // end range is exclusive, subtract 1 to get the class ID
		}
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, (LPCSTR)Project->ProjectTypeName);
	}
}

void CClasses::SaveClassEngineData(GameSerializer & ser)
{
	auto hold1 = UsingHoldMP(1);
	auto hold2 = UsingHoldMP(2);
	auto cap1 = UsingCapMP(1);
	auto cap2 = UsingCapMP(2);
	auto bbma1 = UsingBBMA(1);
	auto bbma2 = UsingBBMA(2);
	auto mprange = UsingMPRange();

	// If nothing needs to save, no need to proceed any farther
	if (!hold1 && !hold2 && !cap1 && !cap2 && !bbma1 && !bbma2 && !mprange)
		return;

	if (Project->IsRom()) {
		auto OverwriteRange = [](const bytevector & src, bytevector & dest, size_t offset, size_t count) {
			for (size_t st = 0, index = offset; st < count; ++st, ++index) {
				dest[index] = src[st];
			}
		};
		auto UpdateIni = [](const bytevector & src, bytevector & dest, const intvector & offsets) {
			for (auto offset : offsets) dest[offset] = src[offset];
		};

		// Copy the bytes in question into the arrays as needed
		if (hold1 || hold2) {
			auto holdlength = ReadDec(Project->ValuesPath, "HoldMPCodeLength");
			ASSERT(holdlength == (int)m_holdmpbytes.size());
			if (holdlength == 0) THROWEXCEPTION(std::runtime_error, "HoldMP length cannot be zero length.");
			if (holdlength != (int)m_holdmpbytes.size()) THROWEXCEPTION(std::runtime_error, "Can't save; HoldMP Values bytes length doesn't match the declared length.");

			auto holdmpbytes = ReadIni(Project->ValuesPath, HoldMPCode, PROP_VALUE, bytevector{});
			ASSERT(holdmpbytes.size() == m_holdmpbytes.size());

			if (hold1) {
				size_t start = HOLDMPCLASS1_OFFSET - HOLDMPCODE_OFFSET;
				auto classid = (int)GetSelectedItemData(m_comboclass1);
				if (classid == (int)CLASS_COUNT)
					for (size_t st = 0; st < 2; ++st) m_holdmpbytes[start + st] = 0xEA;
				else
					for (size_t st = 0; st < 2; ++st) m_holdmpbytes[start + st] = holdmpbytes[start + st];

				//DEVNOTE = by default, HoldMP1 implies the first class due to the structure of the assembly source code,
				//          so we don't need to set the class id (but we will have to for HoldMP class 2 below).
			}
			if (hold2) {
				size_t start = HOLDMPCLASS2_OFFSET - HOLDMPCODE_OFFSET;
				auto classid = (int)GetSelectedItemData(m_comboclass2);
				if (classid == (int)CLASS_COUNT)
					for (size_t st = 0; st < 2; ++st) m_holdmpbytes[start + st] = 0xEA;
				else
					for (size_t st = 0; st < 2; ++st) m_holdmpbytes[start + st] = holdmpbytes[start + st];
				m_holdmpbytes[HOLDMPCLASSID2_OFFSET - HOLDMPCODE_OFFSET] = (BYTE)classid;
			}

			// Don't write the EAs to the INI file, but do replace the class IDs.
			// Only write the class ID bytes to the INI entry, not the lock bytes.
			UpdateIni(m_holdmpbytes, holdmpbytes, { (int)(HOLDMPCLASSID2_OFFSET - HOLDMPCODE_OFFSET) });
			WriteIni(Project->ValuesPath, HoldMPCode, PROP_VALUE, holdmpbytes);
			OverwriteRange(m_holdmpbytes, Project->ROM, HOLDMPCODE_OFFSET, holdlength);
		}
		if (cap1 || cap2) {
			auto caplength = ReadDec(Project->ValuesPath, "CapMPCodeLength");
			ASSERT(caplength == (int)m_capmpbytes.size());
			if (caplength == 0) THROWEXCEPTION(std::runtime_error, "CapMP length cannot be zero length.");
			if (caplength != (int)m_capmpbytes.size()) THROWEXCEPTION(std::runtime_error, "Can't save; CapMP Values bytes length doesn't match the declared length.");

			auto capmpbytes = ReadIni(Project->ValuesPath, CapMPCode, PROP_VALUE, bytevector{});
			ASSERT(capmpbytes.size() == m_capmpbytes.size());

			if (cap1) {
				size_t start = CAPMPCLASS1_OFFSET - CAPMPCODE_OFFSET;
				auto classid = (int)GetSelectedItemData(m_combopost1);
				if (classid == (int)CLASS_COUNT)
					for (size_t st = 0; st < 2; ++st) m_capmpbytes[start + st] = 0xEA;
				else
					for (size_t st = 0; st < 2; ++st) m_capmpbytes[start + st] = capmpbytes[start + st];
				m_capmpbytes[CAPMPCLASSID1_OFFSET - CAPMPCODE_OFFSET] = (BYTE)classid;
			}
			if (cap2) {
				size_t start = CAPMPCLASS2_OFFSET - CAPMPCODE_OFFSET;
				auto classid = (int)GetSelectedItemData(m_combopost2);
				if (classid == (int)CLASS_COUNT)
					for (size_t st = 0; st < 2; ++st) m_capmpbytes[start + st] = 0xEA;
				else
					for (size_t st = 0; st < 2; ++st) m_capmpbytes[start + st] = capmpbytes[start + st];
				m_capmpbytes[CAPMPCLASSID2_OFFSET - CAPMPCODE_OFFSET] = (BYTE)classid;
			}

			// Don't write the EAs to the INI file, but do replace the class IDs.
			// Only write the class ID bytes to the INI entry, not the lock bytes.
			UpdateIni(m_capmpbytes, capmpbytes, { (int)(CAPMPCLASSID1_OFFSET - CAPMPCODE_OFFSET), (int)(CAPMPCLASSID2_OFFSET - CAPMPCODE_OFFSET) });
			WriteIni(Project->ValuesPath, CapMPCode, PROP_VALUE, capmpbytes);
			OverwriteRange(m_capmpbytes, Project->ROM, CAPMPCODE_OFFSET, caplength);
		}
		// The unadjust and readjust IDs are synched to stats when saving. Hits is independent.
		if (bbma1) {
			auto id = (unsigned char)GetSelectedItemData(m_bbmacombo1);
			auto idhits = (unsigned char)GetSelectedItemData(m_2xhitscombo1);
			Project->ROM[BBMACLASS1STATS_OFFSET] = id;
			Project->ROM[BBMACLASS1UNADJ_OFFSET] = id;
			Project->ROM[BBMACLASS1READJ_OFFSET] = id;
			Project->ROM[BBMACLASS1HITS_OFFSET] = idhits;
		}
		if (bbma2) {
			auto id = (unsigned char)GetSelectedItemData(m_bbmacombo2);
			auto idhits = (unsigned char)GetSelectedItemData(m_2xhitscombo2);
			Project->ROM[BBMACLASS2STATS_OFFSET] = id;
			Project->ROM[BBMACLASS2UNADJ_OFFSET] = id;
			Project->ROM[BBMACLASS2READJ_OFFSET] = id;
			Project->ROM[BBMACLASS2HITS_OFFSET] = idhits;
		}
		if (mprange) {
			auto temp = m_mpmincombo.GetCurSel();
			Project->ROM[MAGICMIN_OFFSET] = (BYTE)temp;
			temp = m_mpmaxcombo.GetCurSel();
			Project->ROM[MAGICMAX_OFFSET] = (BYTE)(temp + 1); // end range is exclusive, so add 1 to set the proper value
		}
	}
	else if (Project->IsAsm()) {
		unsigned char skipval = ASM_IGNORE;
		if (hold1) {
			auto id = GetSelectedItemData(m_comboclass1);
			Project->ROM[HOLDMPCLASS1_OFFSET] = id == CLASS_COUNT ? skipval : ASM_TRUE;
		}
		if (hold2) {
			auto id = (unsigned char)GetSelectedItemData(m_comboclass2);
			Project->ROM[HOLDMPCLASS2_OFFSET] = id == CLASS_COUNT ? skipval : ASM_TRUE;
			Project->ROM[HOLDMPCLASSID2_OFFSET] = id;
		}
		if (cap1) {
			auto id = (unsigned char)GetSelectedItemData(m_combopost1);
			Project->ROM[CAPMPCLASS1_OFFSET] = id == CLASS_COUNT ? skipval : ASM_TRUE;
			Project->ROM[CAPMPCLASSID1_OFFSET] = id;
		}
		if (cap2) {
			auto id = (unsigned char)GetSelectedItemData(m_combopost2);
			Project->ROM[CAPMPCLASS2_OFFSET] = id == CLASS_COUNT ? skipval : ASM_TRUE;
			Project->ROM[CAPMPCLASSID2_OFFSET] = id;
		}
		// All three values (stats, unadjust, readjust) are synched to the same class ID when saving.
		if (bbma1) {
			auto id = (unsigned char)GetSelectedItemData(m_bbmacombo1);
			auto idhits = (unsigned char)GetSelectedItemData(m_2xhitscombo1);
			Project->ROM[BBMACLASS1STATS_OFFSET] = id;
			Project->ROM[BBMACLASS1UNADJ_OFFSET] = id;
			Project->ROM[BBMACLASS1READJ_OFFSET] = id;
			Project->ROM[BBMACLASS1HITS_OFFSET] = idhits;
		}
		if (bbma2) {
			auto id = (unsigned char)GetSelectedItemData(m_bbmacombo2);
			auto idhits = (unsigned char)GetSelectedItemData(m_2xhitscombo2);
			Project->ROM[BBMACLASS2STATS_OFFSET] = id;
			Project->ROM[BBMACLASS2UNADJ_OFFSET] = id;
			Project->ROM[BBMACLASS2READJ_OFFSET] = id;
			Project->ROM[BBMACLASS2HITS_OFFSET] = idhits;
		}
		if (mprange) {
			auto temp = m_mpmincombo.GetCurSel();
			Project->ROM[MAGICMIN_OFFSET] = (BYTE)temp;
			temp = m_mpmaxcombo.GetCurSel();
			Project->ROM[MAGICMAX_OFFSET] = (BYTE)(temp + 1); // end range is exclusive, so add 1 to set the proper value
		}

		asmsourcemappingvector vasminlineb;
		asmsourcemappingvector vasminlinec;
		asmsourcemappingvector vasminlinef;
		auto vholdid2 = GetClassIdVector(Project->ROM[HOLDMPCLASSID2_OFFSET]);
		auto vcapid1 = GetClassIdVector(Project->ROM[CAPMPCLASSID1_OFFSET]);
		auto vcapid2 = GetClassIdVector(Project->ROM[CAPMPCLASSID2_OFFSET]);
		auto vbbma1 = GetClassIdVector(Project->ROM[BBMACLASS1STATS_OFFSET]);
		auto vbbma2 = GetClassIdVector(Project->ROM[BBMACLASS2STATS_OFFSET]);
		auto vhits1 = GetClassIdVector(Project->ROM[BBMACLASS1HITS_OFFSET]);
		auto vhits2 = GetClassIdVector(Project->ROM[BBMACLASS2HITS_OFFSET]);
		auto vmpmin = GetClassIdVector(Project->ROM[MAGICMIN_OFFSET]);
		auto vmpmax = GetClassIdVector(Project->ROM[MAGICMAX_OFFSET]);

		// Add mappings as dictated by the UI.
		auto GetClassIdVectorRef = [](stdstringvector & vec) -> stdstringvector& //DEVNOTE - this is mostly for testing purposes (it just returns a ref to the param)
		{
			return vec;
		};

		if (hold1) {
			vasminlineb.push_back({ asmcond, "nop_HoldMpClass1", { HOLDMPCLASS1_OFFSET } });
		}
		if (hold2) {
			vasminlineb.push_back({ asmcond, "nop_HoldMpClass2", { HOLDMPCLASS2_OFFSET } });
			vasminlineb.push_back({ asmopval, "op_HoldMpClass2", { HOLDMPCLASSID2_OFFSET }, GetClassIdVectorRef(vholdid2) });
		}
		if (cap1) {
			vasminlineb.push_back({ asmcond, "nop_HoldMpClassMpIncrement1", { CAPMPCLASS1_OFFSET } });
			vasminlineb.push_back({ asmopval, "op_HoldMpClassMpIncrement1", { CAPMPCLASSID1_OFFSET }, GetClassIdVectorRef(vcapid1) });
		}
		if (cap2) {
			vasminlineb.push_back({ asmcond, "nop_HoldMpClassMpIncrement2", { CAPMPCLASS2_OFFSET } });
			vasminlineb.push_back({ asmopval, "op_HoldMpClassMpIncrement2", { CAPMPCLASSID2_OFFSET }, GetClassIdVectorRef(vcapid2) });
		}
		if (cap1 || cap2) {
			vasminlineb.push_back({ asmopval, "op_HoldMpMpCap", { CAPMPLIMIT_OFFSET } });
		}
		// The unadjust and readjust IDs are synched to stats when saving. Hit count is independent.
		if (bbma1) {
			auto pvclsid = GetClassIdVectorRef(vbbma1);
			auto pvhitsid = GetClassIdVectorRef(vhits1);
			vasminlineb.push_back({ asmopval, "op_BBMAClass1Stats", { BBMACLASS1STATS_OFFSET }, pvclsid });
			vasminlinec.push_back({ asmopval, "op_BBMAClass1Hits", { BBMACLASS1HITS_OFFSET }, pvhitsid });
			vasminlinef.push_back({ asmopval, "op_BBMAClass1Unadjust", { BBMACLASS1UNADJ_OFFSET }, pvclsid });
			vasminlinef.push_back({ asmopval, "op_BBMAClass1Readjust", { BBMACLASS1READJ_OFFSET }, pvclsid });
		}
		if (bbma2) {
			auto pvclsid = GetClassIdVectorRef(vbbma2);
			auto pvhitsid = GetClassIdVectorRef(vhits2);
			vasminlineb.push_back({ asmopval, "op_BBMAClass2Stats", { BBMACLASS2STATS_OFFSET }, pvclsid });
			vasminlinec.push_back({ asmopval, "op_BBMAClass2Hits", { BBMACLASS2HITS_OFFSET }, pvhitsid });
			vasminlinef.push_back({ asmopval, "op_BBMAClass2Unadjust", { BBMACLASS2UNADJ_OFFSET }, pvclsid });
			vasminlinef.push_back({ asmopval, "op_BBMAClass2Readjust", { BBMACLASS2READJ_OFFSET }, pvclsid });
		}
		if (mprange) {
			auto pvmin = GetClassIdVectorRef(vmpmin);
			auto pvmax = GetClassIdVectorRef(vmpmax);
			vasminlinef.push_back({ asmopval, "op_MagicRangeMin", { MAGICMIN_OFFSET }, pvmin });
			vasminlinef.push_back({ asmopval, "op_MagicRangeMax", { MAGICMAX_OFFSET }, pvmax });
		}
		if (!vasminlineb.empty()) ser.SaveInline(ASM_0B, vasminlineb);
		if (!vasminlinec.empty()) ser.SaveInline(ASM_0C, vasminlinec);
		if (!vasminlinef.empty()) ser.SaveInline(ASM_0F, vasminlinef);
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, (LPCSTR)Project->ProjectTypeName);
	}
}

void CClasses::PaintClient(CDC & dc)
{
	__super::PaintClient(dc);
}

void CClasses::LoadValues()
{
	int offset = CLASS_OFFSET + (cur_class * CLASS_BYTES);
	int temp;
	CString text;

	//basic stats
	temp = Project->ROM[offset];
	text.Format("%02X", temp);
	m_classid.SetWindowText(text);

	text.Format("%d",Project->ROM[offset + 1]);
	m_startinghp.SetWindowText(text);
	text.Format("%d",Project->ROM[offset + 2]);
	m_str.SetWindowText(text);
	text.Format("%d",Project->ROM[offset + 3]);
	m_agility.SetWindowText(text);
	text.Format("%d",Project->ROM[offset + 4]);
	m_int.SetWindowText(text);
	text.Format("%d",Project->ROM[offset + 5]);
	m_vit.SetWindowText(text);
	text.Format("%d",Project->ROM[offset + 6]);
	m_luck.SetWindowText(text);
	text.Format("%d",Project->ROM[offset + 7]);
	m_damage.SetWindowText(text);
	text.Format("%d",Project->ROM[offset + 8]);
	m_hit.SetWindowText(text);
	text.Format("%d",Project->ROM[offset + 9]);
	m_evade.SetWindowText(text);
	text.Format("%d",Project->ROM[offset + 10]);
	m_magdef.SetWindowText(text);
	text.Format("%d",Project->ROM[HITPCTINCREASE_OFFSET + cur_class]);
	m_hitpctup.SetWindowText(text);
	text.Format("%d", Project->ROM[MAGDEFINCREASE_OFFSET + cur_class]);
	m_editMagDefUp.SetWindowText(text);

	// Ranges, Start MP, and HoldMP are editable in ROM projects.
	// Assembly projects are better off just editing the assembly.
	text.Format("%d", Project->ROM[STARTINGMP_OFFSET]);
	m_magic.SetWindowText(text);

	// This value axtually holds MaxHP + 1, so if it's not 0, subtract 1.
	auto maxhp = Project->ROM[HPMAX_OFFSET] | (Project->ROM[HPMAX_OFFSET + 1] << 8);
	if (maxhp > 0) --maxhp;
	text.Format("%d", maxhp);
	m_maxhpedit.SetWindowText(text);

	text.Format("%d", Project->ROM[HPRANGEMIN_OFFSET]);
	m_hprangemin.SetWindowText(text);
	text.Format("%d", Project->ROM[HPRANGEMAX_OFFSET]);
	m_hprangemax.SetWindowText(text);

	// Caps are stored as 1 greater than the limit, subtract 1 to display the actual limit in the UI.
	temp = Project->ROM[MPCAP_OFFSET] - 1; if (temp < 0) temp = 0;
	text.Format("%d", temp);
	m_editMpMaxAllChars.SetWindowText(text);

	temp = Project->ROM[CAPMPLIMIT_OFFSET] - 1;
	if (temp < 0) temp = 0; // should store N+1, but if 0 is entered, avoid underflow
	text.Format("%d", temp);
	m_editHoldMpMax.SetWindowText(text);

	//level up data
	for(int co = 0; co < 4; co++){
		offset = ((cur_class % 6) * 98) + ((level_offset + co) << 1) + GetLevelOffset();

		temp = Project->ROM[offset];
		SetCheckFlags(temp, std::vector<CStrikeCheck*>{ ar_luc[co], ar_vit[co], ar_int[co], ar_agi[co], ar_str[co], ar_strong[co] });

		temp = Project->ROM[offset + 1];
		SetCheckFlags(temp, std::vector<CStrikeCheck*>{ ar_1m[co], ar_2m[co], ar_3m[co], ar_4m[co], ar_5m[co], ar_6m[co], ar_7m[co], ar_8m[co] });

		offset = EXP_OFFSET + ((level_offset + co) * 3);
		temp = Project->ROM[offset] + (Project->ROM[offset + 1] << 8) + (Project->ROM[offset + 2] << 16);
		text.Format("%d",temp);
		ar_exp[co]->SetWindowText(text);
	}
	DisplayRunningTotals();
}

void CClasses::StoreValues()
{
	int offset = CLASS_OFFSET + (cur_class * CLASS_BYTES);
	int temp;
	CString text;

	//basic stats
	m_classid.GetWindowText(text); temp = StringToInt_HEX(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset] = (BYTE)temp;
	m_startinghp.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 1] = (BYTE)temp;
	m_str.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 2] = (BYTE)temp;
	m_agility.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 3] = (BYTE)temp;
	m_int.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 4] = (BYTE)temp;
	m_vit.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 5] = (BYTE)temp;
	m_luck.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 6] = (BYTE)temp;
	m_damage.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 7] = (BYTE)temp;
	m_hit.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 8] = (BYTE)temp;
	m_evade.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 9] = (BYTE)temp;
	m_magdef.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 10] = (BYTE)temp;
	m_hitpctup.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[HITPCTINCREASE_OFFSET + cur_class] = (BYTE)temp;
	m_editMagDefUp.GetWindowText(text); temp = StringToInt(text);
	if (temp > 0xFF) temp = 0xFF;
	Project->ROM[MAGDEFINCREASE_OFFSET + cur_class] = (BYTE)temp;

	// Ranges, Start MP, and HoldMP are editable in ROM projects.
	// Assembly projects are better off just editing the assembly.
	m_magic.GetWindowText(text); temp = StringToInt(text);
	if (temp > 0xFF) temp = 0xFF; Project->ROM[STARTINGMP_OFFSET] = (BYTE)temp;

	// This value actually holds MaxHP + 1, so add 1 if it's not already at the 16-bit max value
	m_maxhpedit.GetWindowText(text); temp = StringToInt(text);
	if (temp < 0xFFFF) ++temp;
	Project->ROM[HPMAX_OFFSET] = (BYTE)temp;
	Project->ROM[HPMAX_OFFSET + 1] = (BYTE)((temp >> 8) & 0xFF);

	m_hprangemin.GetWindowText(text); temp = StringToInt(text);
	if (temp > 0xFF) temp = 0xFF; Project->ROM[HPRANGEMIN_OFFSET] = (BYTE)temp;
	m_hprangemax.GetWindowText(text); temp = StringToInt(text);
	if (temp > 0xFF) temp = 0xFF; Project->ROM[HPRANGEMAX_OFFSET] = (BYTE)temp;

	m_editMpMaxAllChars.GetWindowText(text);
	temp = StringToInt(text) + 1;
	if (temp > 0xFF) temp = 0xFF; Project->ROM[MPCAP_OFFSET] = (BYTE)temp;

	m_editHoldMpMax.GetWindowText(text); temp = atol(text) + 1;
	if (temp > 0xFF) temp = 0xFF; Project->ROM[CAPMPLIMIT_OFFSET] = (BYTE)temp;

	//level up data
	for(int co = 0; co < 4; co++){
		offset = ((cur_class % 6) * 98) + ((level_offset + co) << 1) + GetLevelOffset();

		for (auto & wnd : std::vector<CStrikeCheck*>{ ar_luc[co], ar_vit[co], ar_int[co], ar_agi[co], ar_str[co], ar_strong[co] })
			wnd->Invalidate();

		temp = GetCheckFlags(std::vector<CStrikeCheck*>{ ar_luc[co], ar_vit[co], ar_int[co], ar_agi[co], ar_str[co], ar_strong[co] });
		Project->ROM[offset] = (BYTE)temp;

		temp = GetCheckFlags(std::vector<CStrikeCheck*>{ ar_1m[co], ar_2m[co], ar_3m[co], ar_4m[co], ar_5m[co], ar_6m[co], ar_7m[co], ar_8m[co] });
		Project->ROM[offset + 1] = (BYTE)temp;

		offset = EXP_OFFSET + ((level_offset + co) * 3);
		ar_exp[co]->GetWindowText(text);
		temp = StringToInt(text);
		if(temp > 0xFFFFFF) temp = 0xFFFFFF;
		Project->ROM[offset] = temp & 0xFF;
		Project->ROM[offset + 1] = (temp >> 8) & 0xFF;
		Project->ROM[offset + 2] = (BYTE)(temp >> 16);
	}
}

void CClasses::OnSelchangeClasslist() 
{
	if(cur_class != -1) StoreValues();
	cur_class = m_classlist.GetCurSel();
	LoadValues();
}

void CClasses::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UNREFERENCED_PARAMETER(pScrollBar);

	int offset = 0;
	switch (nSBCode) {
	case 0: offset = -1; break;
	case 1: offset = 1; break;
	case 2: offset = -4; break;
	case 3: offset = 4; break;
	case 5: offset = (nPos - level_offset); break;
	}
	ScrollXPViewBy(offset);
}

BOOL CClasses::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// Invert zDelta so that scrolling toward the user goes down the list
	int offset = (-zDelta / WHEEL_DELTA);
	if (offset != 0) ScrollXPViewBy(offset);
	return BaseClass::OnMouseWheel(nFlags, zDelta, pt);
}

void CClasses::DisplayRunningTotals()
{
	int classid = m_classlist.GetCurSel();
	bool skipmp = classid != LB_ERR && classid == (int)GetSelectedItemData(m_comboclass1) || classid == (int)GetSelectedItemData(m_comboclass2);

	int mpmax = 0;
	if (!skipmp) {
		bool limitmp = classid != LB_ERR && classid == (int)GetSelectedItemData(m_combopost1) || classid == (int)GetSelectedItemData(m_combopost2);
		int allmax = dec(GetControlText(m_editMpMaxAllChars));
		int capmax = dec(GetControlText(m_editHoldMpMax));
		mpmax = limitmp ? capmax : allmax;
	}

	auto hpmax = atoi(GetControlText(m_maxhpedit));

	CString text, texter;
	int lvl = level_offset;
	int co2;
	for(int co = 0; co < 4; co++, lvl++)
	{
		text.Format("Level: %d",lvl + 2);
		ar_level[co]->SetWindowText(text);

		text.Format("HP: %d", TotalHP[lvl] > hpmax ? hpmax : TotalHP[lvl]);
		ar_totalhp[co]->SetWindowText(text);

		text.Format("Str: %d",TotalStr[lvl]);
		ar_totalstr[co]->SetWindowText(text);
		text.Format("Int: %d",TotalInt[lvl]);
		ar_totalint[co]->SetWindowText(text);
		text.Format("Vit: %d",TotalVit[lvl]);
		ar_totalvit[co]->SetWindowText(text);
		text.Format("Agi: %d",TotalAgi[lvl]);
		ar_totalagi[co]->SetWindowText(text);
		text.Format("Luc: %d",TotalLuc[lvl]);
		ar_totalluc[co]->SetWindowText(text);

		text = "";
		for (co2 = 0; co2 < 8; co2++) {
			auto mp = (int)TotalMagic[lvl][co2];
			if (mp > mpmax) mp = mpmax;

			if (co2 & 0x03) text += "/";
			if (co2 == 4) text += "\n";
			texter.Format("%d", mp);
			text += texter;
		}
		ar_totalmagic[co]->SetWindowText(text);
	}
}

void CClasses::RecalculateRunningTotals()
{
	int offset;
	BYTE temp, line, tco;
	CString text;

	m_startinghp.GetWindowText(text);
	short RunningHP = (short)StringToInt(text);
	m_str.GetWindowText(text);
	short RunningStr = (short)(StringToInt(text) << 1);
	m_agility.GetWindowText(text);
	short RunningAgi = (short)(StringToInt(text) << 1);
	m_int.GetWindowText(text);
	short RunningInt = (short)(StringToInt(text) << 1);
	m_vit.GetWindowText(text);
	short RunningVit = (short)(StringToInt(text) << 1);
	m_luck.GetWindowText(text);
	short RunningLuc = (short)(StringToInt(text) << 1);

	int HPGain = 0; //REFACTOR - initializing to avoi resolve C4701
	m_hprangemax.GetWindowText(text);
	if(ViewScale) HPGain = StringToInt(text);
	m_hprangemin.GetWindowText(text);
	if(!ViewScale) HPGain = StringToInt(text);
	else if (ViewScale == 1) {
		temp = (BYTE)StringToInt(text);
		HPGain = ((HPGain - temp) >> 1) + temp;
	}
	
	m_magic.GetWindowText(text);
	temp = (BYTE)StringToInt(text);
	BYTE RunningMag[8] = {temp,0,0,0,0,0,0,0};
	offset = cur_class % 6;

	// if hprange isn't being used, then don't reset the running total for level 1 here (ranges are inclusive)
	if (UsingMPRange()) {
		if (offset < m_mpmincombo.GetCurSel()) RunningMag[0] = 0;
		if (offset > m_mpmaxcombo.GetCurSel()) RunningMag[0] = 0;
	}

	offset = ((cur_class % 6) * 98) + GetLevelOffset();
	for(int co = 0; co < 49; co++, offset += 2)
	{
		RunningHP += 1 + (RunningVit >> 3);
		temp = Project->ROM[offset];
		if(temp & 0x20) //if strong level up
			RunningHP += (short)HPGain;
		if(ViewScale == 2){
			RunningStr += 2;
			RunningAgi += 2;
			RunningInt += 2;
			RunningVit += 2;
			RunningLuc += 2;}
		else{
			if(temp & 0x10) RunningStr += 2;
			else RunningStr += ViewScale;
			if(temp & 0x08) RunningAgi += 2;
			else RunningAgi += ViewScale;
			if(temp & 0x04) RunningInt += 2;
			else RunningInt += ViewScale;
			if(temp & 0x02) RunningVit += 2;
			else RunningVit += ViewScale;
			if(temp & 0x01) RunningLuc += 2;
			else RunningLuc += ViewScale;}

		temp = Project->ROM[offset + 1];
		for (tco = 0, line = 1; tco < 8; tco++, line <<= 1) {
			RunningMag[tco] += (temp & line) != 0;
		}

		TotalHP[co] = RunningHP;
		TotalStr[co] = RunningStr >> 1;
		TotalAgi[co] = RunningAgi >> 1;
		TotalInt[co] = RunningInt >> 1;
		TotalVit[co] = RunningVit >> 1;
		TotalLuc[co] = RunningLuc >> 1;
		for(tco = 0; tco < 8; tco++)
			TotalMagic[co][tco] = RunningMag[tco];
	}

	DisplayRunningTotals();
}

void CClasses::ScrollXPViewBy(int offset)
{
	StoreValues();
	level_offset += (short)offset;
	if (level_offset < 0) level_offset = 0;
	if (level_offset > 45) level_offset = 45;

	m_scroll.SetScrollPos(level_offset);
	LoadValues();
}

int CClasses::GetLevelOffset()
{
	return LEVELUP_OFFSET;
}

stdstringvector CClasses::GetClassIdVector(unsigned char classid)
{
	stdstringvector vec;

	auto it = std::find_if(cbegin(Project->m_varmap), cend(Project->m_varmap), [classid](const auto & kv) {
		bool matched = kv.second == classid && kv.first.find("CLS_") == 0;
		return matched;
	});

	if (it != cend(Project->m_varmap)) vec.push_back(it->first);
	return vec;
}

void CClasses::HandleClassListContextMenu(CWnd * pWnd, CPoint point)
{
	CPoint listpoint = point;
	BOOL bOutside = FALSE;
	m_classlist.ScreenToClient(&listpoint);
	auto selclass = m_classlist.ItemFromPoint(listpoint, bOutside);
	if (selclass == LB_ERR) return;

	auto BuildPasteText = [](CString op, CString srccls, CString destcls) {
		if (!op.IsEmpty()) op += " ";
		CString cs;
		cs.Format("Paste %sfrom %s to %s", (LPCSTR)op, (LPCSTR)srccls, (LPCSTR)destcls);
		return cs;
	};

	auto strselclass = LoadClassEntry(*Project, selclass).name;
	m_classesmenu.AppendMenu(MF_STRING, CLASS_COPY, "&Copy " + strselclass);
	if (m_copiedclass != -1 && m_copiedclass != (int)selclass) {
		// if pasting, the clicked index is actually the destination
		auto strdestclass = LoadClassEntry(*Project, selclass).name.Trim();
		strselclass = LoadClassEntry(*Project, m_copiedclass).name.Trim();
		m_classesmenu.AppendMenu(MF_STRING, CLASS_PASTE, "&" + BuildPasteText("", strselclass, strdestclass) + "...");
	}

	auto selop = m_classesmenu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, point.x, point.y, pWnd);
	if (selop != 0) {
		if (selop == CLASS_COPY) CopyClass(selclass);
		if (selop == CLASS_PASTE) {
			auto strdestclass = LoadClassEntry(*Project, selclass).name.Trim();
			CDlgPasteTargets dlg(this);
			dlg.Title = BuildPasteText("", strselclass, strdestclass);
			dlg.SetTargets(std::vector<PasteTarget>{
				{ "Startup data", true}, { "SAIVL/Strong level data", true }, { "Spell charge level data", true }, { "Battle graphics", false },
				{ "Mapman graphics", false, "Copies only the image; select the palette entry below to also copy the palette." },
				{ "Mapman palette", false, "The overwritten mapman palette can't be restored by simply importing a saved bitmap." },
			});
			if (dlg.DoModal() == IDOK) {
				int start = dlg.IsChecked(0);
				if (start) PasteStartInfo(m_copiedclass, selclass);
				int saivl = dlg.IsChecked(1) ? PASTE_SAIVLDATA : 0;
				int level = dlg.IsChecked(2) ? PASTE_LEVELDATA : 0;
				PasteLevelInfo(m_copiedclass, selclass, saivl | level);
				int battle = dlg.IsChecked(3) ? PASTE_BATTLEGFX : 0;
				int mapman = dlg.IsChecked(4) ? PASTE_MAPMANGFX : 0;
				int mappal = dlg.IsChecked(5) ? PASTE_MAPMANPAL : 0;
				PasteSpriteAndPaletteInfo(m_copiedclass, selclass, battle | mapman | mappal);

				if (start | saivl | level | battle | mapman | mappal)
					LoadValues();
			}
		}
	}
}

void CClasses::CopyClass(int classindex)
{
	m_copiedclass = classindex;
}

void CClasses::PasteStartInfo(int srcindex, int destindex)
{
	int srcoffset = CLASS_OFFSET + (srcindex * CLASS_BYTES);
	int dstoffset = CLASS_OFFSET + (destindex * CLASS_BYTES);

	auto CopyBaseStat = [srcoffset, dstoffset, this](int step) {
		Project->ROM[dstoffset + step] = Project->ROM[srcoffset + step];
	};
	//CopyBaseStat(0); // class ID     // don't copy/paste the class ID
	CopyBaseStat(1); // starting HP
	CopyBaseStat(2); // STR
	CopyBaseStat(3); // AGI
	CopyBaseStat(4); // INT
	CopyBaseStat(5); // VIT
	CopyBaseStat(6); // LUCK
	CopyBaseStat(7); // DMG
	CopyBaseStat(8); // HIT
	CopyBaseStat(9); // EVADE
	CopyBaseStat(10); // MAGDEF
	CopyBaseStat(11); // Starting MP (lobybble), hinybble currently unused

	auto CopyExtStat = [srcindex, destindex, this](int baseoffset) {
		Project->ROM[baseoffset + destindex] = Project->ROM[baseoffset + srcindex];
	};
	//CopyExtStat(MPCAP_OFFSET); //REFACTOR - this is used in BCC, not vanilla, put there, then remove from here
	CopyExtStat(HITPCTINCREASE_OFFSET);
	CopyExtStat(MAGDEFINCREASE_OFFSET);
}

void CClasses::PasteLevelInfo(int srcindex, int destindex, int flags)
{
	bool pastestart = (flags & PASTE_SAIVLDATA) == PASTE_SAIVLDATA;
	bool pastelevel = (flags & PASTE_LEVELDATA) == PASTE_LEVELDATA;
	for (int level = 0; level < 50; ++level) {
		int srcoffset = ((srcindex % 6) * 98) + (level << 1) + GetLevelOffset();
		int dstoffset = ((destindex % 6) * 98) + (level << 1) + GetLevelOffset();
		if (pastestart) Project->ROM[dstoffset] = Project->ROM[srcoffset];
		if (pastelevel) Project->ROM[dstoffset + 1] = Project->ROM[srcoffset + 1];
	}
}

void CClasses::PasteSpriteAndPaletteInfo(int srcindex, int destindex, int flags)
{
	if (HasAnyFlag(flags, PASTE_BATTLEGFX)) {
		// This copies the palette assignments and the bitmaps, but doesn't touch the actual palettes since they're shared.
		// The user will need to edit the palette through the UI.
		Project->ROM[CHARBATTLEPALETTE_ASSIGNMENT1 + destindex] = Project->ROM[CHARBATTLEPALETTE_ASSIGNMENT1 + srcindex];
		Project->ROM[CHARBATTLEPALETTE_ASSIGNMENT2 + destindex] = Project->ROM[CHARBATTLEPALETTE_ASSIGNMENT2 + srcindex];
		const auto tileprod = BATTLESPRITETILE_COUNT * BATTLESPRITETILE_BYTES;
		auto srcbase = CHARBATTLEPIC_OFFSET + (srcindex * tileprod);
		auto dstbase = CHARBATTLEPIC_OFFSET + (destindex * tileprod);
		for (size_t st = 0; st < tileprod; ++st) {
			Project->ROM[dstbase++] = Project->ROM[srcbase++];
		}
	}
	if (HasAnyFlag(flags, PASTE_MAPMANGFX)) {
		// Copy the mapman tiles
		const auto tileprod = MAPMANSPRITETILE_COUNT * MAPMANSPRITETILE_BYTES;
		auto srcbase = MAPMANGRAPHIC_OFFSET + (srcindex * tileprod);
		auto dstbase = MAPMANGRAPHIC_OFFSET + (destindex * tileprod);
		for (size_t st = 0; st < tileprod; ++st) {
			Project->ROM[dstbase++] = Project->ROM[srcbase++];
		}
	}
	if (HasAnyFlag(flags, PASTE_MAPMANPAL)) {
		// Copy the top and bottom "index 2" palette colors for this character
		auto palsrc = MAPMANPALETTE_OFFSET + (srcindex * 2);
		auto paldst = MAPMANPALETTE_OFFSET + (destindex * 2);
		Project->ROM[paldst] = Project->ROM[palsrc];
		Project->ROM[paldst + 1] = Project->ROM[palsrc + 1];
	}
}

void CClasses::HandleSpellCheckContextMenu(CWnd * pWnd, CPoint point)
{
	m_classesmenu.AppendMenu(MF_STRING, CONTEXT_CHECKED, "&Check All Spells");
	m_classesmenu.AppendMenu(MF_STRING, CONTEXT_UNCHECKED, "&Uncheck All Spells");

	auto GetCheckGroup = [](UINT uID) {
		if (uID <= IDC_8M1) return 0;
		if (uID <= IDC_8M2) return 1;
		if (uID <= IDC_8M3) return 2;
		if (uID <= IDC_8M4) return 3;
		return -1;
	};

	auto selop = m_classesmenu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, point.x, point.y, pWnd);
	if (selop != 0) {
		switch (selop) {
		case CONTEXT_CHECKED:
		case CONTEXT_UNCHECKED:
			{
			bool check = selop == CONTEXT_CHECKED;
			ChangeSpellCheckGroup(GetCheckGroup(pWnd->GetDlgCtrlID()), check);
			}
			break;
		}
	}
}

void CClasses::HandleSaivlCheckContextMenu(CWnd* pWnd, CPoint point)
{
	m_classesmenu.AppendMenu(MF_STRING, CONTEXT_CHECKED, "&Check All SAIVL");
	m_classesmenu.AppendMenu(MF_STRING, CONTEXT_UNCHECKED, "&Uncheck All SAIVL");

	auto GetCheckGroup = [](UINT uID) {
		if (uID <= IDC_LUC1) return 0;
		if (uID <= IDC_LUC2) return 1;
		if (uID <= IDC_LUC3) return 2;
		if (uID <= IDC_LUC4) return 3;
		return -1;
	};

	auto selop = m_classesmenu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, point.x, point.y, pWnd);
	if (selop != 0) {
		switch (selop) {
		case CONTEXT_CHECKED:
		case CONTEXT_UNCHECKED:
		{
			bool check = selop == CONTEXT_CHECKED;
			ChangeSaivlCheckGroup(GetCheckGroup(pWnd->GetDlgCtrlID()), check);
		}
		break;
		}
	}
}

bool CClasses::IsSpellChargeCheck(CWnd * pWnd)
{
	for (int i = 0; i < 4; ++i) {
		if (
			ar_1m[i] == pWnd ||
			ar_2m[i] == pWnd ||
			ar_3m[i] == pWnd ||
			ar_4m[i] == pWnd ||
			ar_5m[i] == pWnd ||
			ar_6m[i] == pWnd ||
			ar_7m[i] == pWnd ||
			ar_8m[i] == pWnd
			)
			return true;
	}
	return false;
}

bool CClasses::IsSaivlChargeCheck(CWnd * pWnd)
{
	for (int i = 0; i < 4; ++i) {
		if (
			ar_strong[i] == pWnd ||
			ar_str[i] == pWnd ||
			ar_agi[i] == pWnd ||
			ar_int[i] == pWnd ||
			ar_vit[i] == pWnd ||
			ar_luc[i] == pWnd
			)
			return true;
	}
	return false;
}

void CClasses::ChangeSpellCheckGroup(int group, bool checked)
{
	if (group == -1) return;

	SetCheckValue(*ar_1m[group], checked);
	SetCheckValue(*ar_2m[group], checked);
	SetCheckValue(*ar_3m[group], checked);
	SetCheckValue(*ar_4m[group], checked);
	SetCheckValue(*ar_5m[group], checked);
	SetCheckValue(*ar_6m[group], checked);
	SetCheckValue(*ar_7m[group], checked);
	SetCheckValue(*ar_8m[group], checked);
	OnCheckBox();
}

void CClasses::ChangeSaivlCheckGroup(int group, bool checked)
{
	if (group == -1) return;

	//ar_strong[group]->SetCheck(checked); //DEVNOTE - to add strong back, uncomment and add it to the text in HandleSaivlCheckContextMenu()
	//SetCheckValue(*ar_strong[group], checked);
	SetCheckValue(*ar_str[group], checked);
	SetCheckValue(*ar_agi[group], checked);
	SetCheckValue(*ar_int[group], checked);
	SetCheckValue(*ar_vit[group], checked);
	SetCheckValue(*ar_luc[group], checked);
	OnCheckBox();
}


bool CClasses::UsingHoldMP(int slot)
{
	ASSERT(slot == 1 || slot == 2);
	CClassesEditorSettings stgs(*Project);
	auto nChecked = slot == 1 ? stgs.IgnoreHoldMP1 : stgs.IgnoreHoldMP2;
	return !nChecked;
}

bool CClasses::UsingCapMP(int slot)
{
	ASSERT(slot == 1 || slot == 2);
	CClassesEditorSettings stgs(*Project);
	auto nChecked = slot == 1 ? stgs.IgnoreCapMP1 : stgs.IgnoreCapMP2;
	return !nChecked;
}

bool CClasses::UsingMPRange()
{
	CClassesEditorSettings stgs(*Project);
	auto nChecked = stgs.IgnoreMPRange;
	return !nChecked;
}

bool CClasses::UsingBBMA(int slot)
{
	ASSERT(slot == 1 || slot == 2);
	CClassesEditorSettings stgs(*Project);
	auto nChecked = slot == 1 ? stgs.IgnoreBBMA1 : stgs.IgnoreBBMA2;
	return !nChecked;
}

void CClasses::OnMean() 
{ViewScale = 1; m_min.SetCheck(0); m_max.SetCheck(0); RecalculateRunningTotals();}
void CClasses::OnMax() 
{ViewScale = 2; m_min.SetCheck(0); m_mean.SetCheck(0); RecalculateRunningTotals();}
void CClasses::OnMin() 
{ViewScale = 0; m_max.SetCheck(0); m_mean.SetCheck(0); RecalculateRunningTotals();}
void CClasses::OnChangeHprangemax() 
{if(ViewScale != 0) RecalculateRunningTotals();}
void CClasses::OnChangeHprangemin() 
{if(ViewScale != 2) RecalculateRunningTotals();}
void CClasses::OnCheckBox()
{
	StoreValues();
	LoadValues();
	RecalculateRunningTotals();
}

void CClasses::OnBattlepic()
{
	CWaitCursor wait;
	CBattlePic dlg;
	dlg.cart = Project;
	dlg.Class = cur_class;
	dlg.InMemory = true;
	dlg.DoModal();
}

void CClasses::OnMapmanpic()
{
	CWaitCursor wait;
	CMapman dlg;
	dlg.cart = Project;
	dlg.graphicoffset = MAPMANGRAPHIC_OFFSET + (cur_class << 8);
	dlg.paletteoffset = -cur_class - 1;
	dlg.InMemory = true;
	dlg.DoModal();
}

void CClasses::OnContextMenu(CWnd* pWnd, CPoint point)
{
	m_classesmenu.DestroyMenu();
	if (!m_classesmenu.CreatePopupMenu()) return;

	if (pWnd == &m_classlist) {
		HandleClassListContextMenu(pWnd, point);
	}
	else if (IsSpellChargeCheck(pWnd)) {
		HandleSpellCheckContextMenu(pWnd, point);
	}
	else if (IsSaivlChargeCheck(pWnd)) {
		HandleSaivlCheckContextMenu(pWnd, point);
	}
}

void CClasses::OnCbnSelchangeComboMprangemin()
{
	int imin = m_mpmincombo.GetCurSel();
	int imax = m_mpmaxcombo.GetCurSel();
	if (imin > imax) m_mpmaxcombo.SetCurSel(imin);
	RecalculateRunningTotals();

	ASSERT(CLASS_COUNT > 0);
	auto minid = GetSelectedItemData(m_mpmincombo);
	m_mpmaxcombo.EnableWindow(minid != CLASS_COUNT);
}

void CClasses::OnCbnSelchangeComboMprangemax()
{
	int imin = m_mpmincombo.GetCurSel();
	int imax = m_mpmaxcombo.GetCurSel();
	if (imax < imin) m_mpmincombo.SetCurSel(imax);
	RecalculateRunningTotals();
}

void CClasses::OnBnClickedClassesInfo()
{
	CString msg;
	if (!UsingHoldMP(1)) msg.AppendFormat("Settings for the first Hold MP class are disabled.\n");
	if (!UsingHoldMP(2)) msg.AppendFormat("Settings for the second Hold MP class are disabled.\n");
	if (!UsingCapMP(1)) msg.AppendFormat("Settings for the first Post-Hold Cap MP class are disabled.\n");
	if (!UsingCapMP(2)) msg.AppendFormat("Settings for the second Post-Hold Cap MP class are disabled.\n");
	if (!UsingMPRange()) msg.AppendFormat("MP range settings are disabled.\n");
	if (!UsingBBMA(1)) msg.AppendFormat("Settings for the first BB/MA class are disabled.\n");
	if (!UsingBBMA(2)) msg.AppendFormat("Settings for the second BB/MA class are disabled.\n");
	if (!msg.IsEmpty())
		AfxMessageBox(msg, MB_ICONWARNING);
}

void CClasses::OnCbnSelchangeClasscombo1()
{
	RecalculateRunningTotals();
}

void CClasses::OnCbnSelchangeClasscombo2()
{
	RecalculateRunningTotals();
}

void CClasses::OnCbnSelchangePostcombo1()
{
	RecalculateRunningTotals();
}

void CClasses::OnCbnSelchangePostcombo2()
{
	RecalculateRunningTotals();
}

void CClasses::OnBnClickedClassesSettings()
{
	try {
		CClassesEditorSettingsDlg dlg(this);
		dlg.m_proj = Project;
		dlg.PreSaveOKCancelPrompt = "The Classes editor must reload if these settings are saved.\nDo you still want to save these changes?";
		if (dlg.DoModal() == IDOK) {
			//FUTURE - reloading is such a pain, so close the caller and reopen manually to
			//         re-apply the changed settings.
			//         These largely involve BBMA/HoldMP/PostHoldMP settings.
			//         Factor that code into a single function, then call that here and remove the
			//         hack for reload here and the prompt in CClassesEditorSettingsDlg.
			// Regardless of whether or not the save succeeds, the dialog must reload to re-apply the settings.
			m_bRequestReload = true;
			DoSave();
			OnCancel();
		}
	}
	catch (std::exception & ex) {
		AfxMessageBox("Unable to save ROM.\n" + CString(ex.what()), MB_ICONERROR);
	}
	catch (...) {
		AfxMessageBox("Unable to save ROM.", MB_ICONERROR);
	}
}