// Enemy.cpp : implementation file
//
#include "stdafx.h"
#include "Enemy.h"
#include "AsmFiles.h"
#include "collection_helpers.h"
#include "editor_label_functions.h"
#include "FFHacksterProject.h"
#include "GameSerializer.h"
#include "general_functions.h"
#include "imaging_helpers.h"
#include "ingame_text_functions.h"
#include "ini_functions.h"
#include "io_functions.h"
#include "string_functions.h"
#include "ui_helpers.h"

#include "BattlePatternTables.h"
#include "EnemyEditorSettings.h"
#include "EnemyEditorSettingsDlg.h"
#include "NewLabel.h"

#include <path_functions.h>
#include <type_support.h>

using namespace Editorlabels;
using namespace Imaging;
using namespace Ingametext;
using namespace Ini;
using namespace Io;
using namespace Ui;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnemy dialog
#define BaseClass CEditorWithBackground


CEnemy::CEnemy(CWnd* pParent /*= nullptr */)
	: BaseClass(CEnemy::IDD, pParent)
{
}

void CEnemy::DoDataExchange(CDataExchange* pDX)
{
	BaseClass::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ATKELEM8, m_atkelem8);
	DDX_Control(pDX, IDC_ATKELEM7, m_atkelem7);
	DDX_Control(pDX, IDC_ATKELEM6, m_atkelem6);
	DDX_Control(pDX, IDC_ATKELEM5, m_atkelem5);
	DDX_Control(pDX, IDC_ATKELEM4, m_atkelem4);
	DDX_Control(pDX, IDC_ATKELEM3, m_atkelem3);
	DDX_Control(pDX, IDC_ATKELEM2, m_atkelem2);
	DDX_Control(pDX, IDC_ATKELEM1, m_atkelem1);
	DDX_Control(pDX, IDC_EDITLABEL, m_editlabel);
	DDX_Control(pDX, IDC_WEAK8, m_weak8);
	DDX_Control(pDX, IDC_WEAK7, m_weak7);
	DDX_Control(pDX, IDC_WEAK6, m_weak6);
	DDX_Control(pDX, IDC_WEAK5, m_weak5);
	DDX_Control(pDX, IDC_WEAK4, m_weak4);
	DDX_Control(pDX, IDC_WEAK3, m_weak3);
	DDX_Control(pDX, IDC_WEAK2, m_weak2);
	DDX_Control(pDX, IDC_WEAK1, m_weak1);
	DDX_Control(pDX, IDC_UNKNOWN, m_unknown);
	DDX_Control(pDX, IDC_STR, m_str);
	DDX_Control(pDX, IDC_RES8, m_res8);
	DDX_Control(pDX, IDC_RES7, m_res7);
	DDX_Control(pDX, IDC_RES6, m_res6);
	DDX_Control(pDX, IDC_RES5, m_res5);
	DDX_Control(pDX, IDC_RES4, m_res4);
	DDX_Control(pDX, IDC_RES3, m_res3);
	DDX_Control(pDX, IDC_RES2, m_res2);
	DDX_Control(pDX, IDC_RES1, m_res1);
	DDX_Control(pDX, IDC_MORALE, m_morale);
	DDX_Control(pDX, IDC_MAGDEF, m_magdef);
	DDX_Control(pDX, IDC_HP, m_hp);
	DDX_Control(pDX, IDC_HITS, m_hits);
	DDX_Control(pDX, IDC_HITPCT, m_hit);
	DDX_Control(pDX, IDC_GOLD, m_gold);
	DDX_Control(pDX, IDC_EXP, m_exp);
	DDX_Control(pDX, IDC_ENEMYLIST, m_enemylist);
	DDX_Control(pDX, IDC_EFF20, m_eff8);
	DDX_Control(pDX, IDC_EFF19, m_eff7);
	DDX_Control(pDX, IDC_EFF18, m_eff6);
	DDX_Control(pDX, IDC_EFF5, m_eff5);
	DDX_Control(pDX, IDC_EFF4, m_eff4);
	DDX_Control(pDX, IDC_EFF3, m_eff3);
	DDX_Control(pDX, IDC_EFF2, m_eff2);
	DDX_Control(pDX, IDC_EFF1, m_eff1);
	DDX_Control(pDX, IDC_DEF, m_def);
	DDX_Control(pDX, IDC_CRITICAL, m_critical);
	DDX_Control(pDX, IDC_CAT8, m_cat8);
	DDX_Control(pDX, IDC_CAT7, m_cat7);
	DDX_Control(pDX, IDC_CAT6, m_cat6);
	DDX_Control(pDX, IDC_CAT5, m_cat5);
	DDX_Control(pDX, IDC_CAT4, m_cat4);
	DDX_Control(pDX, IDC_CAT3, m_cat3);
	DDX_Control(pDX, IDC_CAT2, m_cat2);
	DDX_Control(pDX, IDC_CAT1, m_cat1);
	DDX_Control(pDX, IDC_AI_MAGICRATE, m_ai_magicrate);
	DDX_Control(pDX, IDC_AI_MAGIC8, m_ai_magic8);
	DDX_Control(pDX, IDC_AI_MAGIC7, m_ai_magic7);
	DDX_Control(pDX, IDC_AI_MAGIC6, m_ai_magic6);
	DDX_Control(pDX, IDC_AI_MAGIC5, m_ai_magic5);
	DDX_Control(pDX, IDC_AI_MAGIC4, m_ai_magic4);
	DDX_Control(pDX, IDC_AI_MAGIC3, m_ai_magic3);
	DDX_Control(pDX, IDC_AI_MAGIC2, m_ai_magic2);
	DDX_Control(pDX, IDC_AI_MAGIC1, m_ai_magic1);
	DDX_Control(pDX, IDC_AI_ABILITYRATE, m_ai_abilityrate);
	DDX_Control(pDX, IDC_AI_ABILITY4, m_ai_ability4);
	DDX_Control(pDX, IDC_AI_ABILITY3, m_ai_ability3);
	DDX_Control(pDX, IDC_AI_ABILITY2, m_ai_ability2);
	DDX_Control(pDX, IDC_AI_ABILITY1, m_ai_ability1);
	DDX_Control(pDX, IDC_AI, m_ai);
	DDX_Control(pDX, IDC_AGILITY, m_agility);
	DDX_Control(pDX, IDC_ENEMY_GROUP_BYTE15, m_groupByte15);
	DDX_Control(pDX, IDC_ENEMY_GROUP_WEAK, m_groupWeak);
	DDX_Control(pDX, IDC_ENEMY_GROUP_ATTACK, m_groupAttack);
	DDX_Control(pDX, IDC_ENEMY_STATIC_BYTE15DETAIL, m_statAttackElem);
	DDX_Control(pDX, IDC_ENEMY_GROUP_AI, m_groupAi);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDC_CLASSES_SETTINGS, m_settingsbutton);
	DDX_Control(pDX, IDC_SAVE, m_savebutton);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDC_EDITPIC, m_editgfxbutton);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
	DDX_Control(pDX, IDC_ENEMY_BTN_VIEWUSE, m_viewuse);
}

BOOL CEnemy::OnInitDialog()
{
	BaseClass::OnInitDialog();

	try {
		LoadRom();

		SetRedraw(FALSE);

		CEnemyEditorSettings stgs(*Project);
		m_wasflags = stgs.Byte15AsFlags;

		auto sizedelta = CSize(0, GetDistance(&m_atkelem1, &m_statAttackElem).cy);
		MoveControlBy(&m_statAttackElem, sizedelta);
		MoveControlBy(&m_unknown, sizedelta);

		LoadCaptions(std::vector<CWnd*>{ &m_cat1, &m_cat2, &m_cat3, &m_cat4, &m_cat5, &m_cat6, &m_cat7, &m_cat8 }, LoadEnemyCategoryLabels(*Project));
		LoadCaptions(std::vector<CWnd*>{ &m_eff1, &m_eff2, &m_eff3, &m_eff4, &m_eff5, &m_eff6, &m_eff7, &m_eff8 }, LoadAilEffectLabels(*Project));
		LoadCaptions(std::vector<CWnd*>{ &m_weak1, &m_weak2, &m_weak3, &m_weak4, &m_weak5, &m_weak6, &m_weak7, &m_weak8 }, LoadElementLabels(*Project));
		LoadCaptions(std::vector<CWnd*>{ &m_weak1, &m_weak2, &m_weak3, &m_weak4, &m_weak5, &m_weak6, &m_weak7, &m_weak8 }, LoadElementLabels(*Project));
		LoadCaptions(std::vector<CWnd*>{ &m_atkelem1, &m_atkelem2, &m_atkelem3, &m_atkelem4, &m_atkelem5, &m_atkelem6, &m_atkelem7, &m_atkelem8 }, LoadElementLabels(*Project));

		LoadListBox(m_enemylist, LoadEnemyEntries(*Project));

		for (auto wnd : { &m_ai_magic1,&m_ai_magic2,&m_ai_magic3,&m_ai_magic4,&m_ai_magic5,&m_ai_magic6,&m_ai_magic7,&m_ai_magic8 })
			LoadCombo(*wnd, LoadMagicEntries(*Project));
		for (auto wnd : { &m_ai_ability1,&m_ai_ability2,&m_ai_ability3,&m_ai_ability4 })
			LoadCombo(*wnd, LoadAttackEntries(*Project));

		auto AILabels = get_names(LoadAILabels(*Project));
		for (int co = 0; co < AI_COUNT; co++)
			m_ai.InsertString(co, AILabels[co]);

		m_ai.InsertString(0, "--none--");
		m_ai_magic1.InsertString(0, "--none--");
		m_ai_magic2.InsertString(0, "--none--");
		m_ai_magic3.InsertString(0, "--none--");
		m_ai_magic4.InsertString(0, "--none--");
		m_ai_magic5.InsertString(0, "--none--");
		m_ai_magic6.InsertString(0, "--none--");
		m_ai_magic7.InsertString(0, "--none--");
		m_ai_magic8.InsertString(0, "--none--");
		m_ai_ability1.InsertString(0, "--none--");
		m_ai_ability2.InsertString(0, "--none--");
		m_ai_ability3.InsertString(0, "--none--");
		m_ai_ability4.InsertString(0, "--none--");

		m_usagedataon = stgs.ViewUsage;
		if (m_usagedataon) {
			try {
				CWaitCursor wait;
				m_usedata.SetProject(*Project);
				if (m_dlgdatalist.CreateModeless(this)) {
					// Put the modeless toward the right edge.
					//DEVNOTE - ran into some oddities with positioning, I'll deal with this later if it causes problems.
					auto rcthis = Ui::GetWindowRect(this);
					auto rclist = Ui::GetWindowRect(&m_dlgdatalist);
					auto rclb = Ui::GetControlRect(&m_enemylist);
					m_dlgdatalist.SetWindowPos(nullptr, rcthis.right - rclist.Width(), rcthis.top + rclb.top, -1, -1,
						SWP_NOZORDER | SWP_NOSIZE);
				}
				ASSERT(::IsWindow(m_dlgdatalist.GetSafeHwnd()));
			}
			catch (std::exception& ex) {
				m_usagedataon = false;
				CString msg;
				msg.Format("Enemy usage data was unable to load an will be unavailable:\n%s", ex.what());
				AfxMessageBox(msg);
			}
		}
		if (!m_usagedataon) m_viewuse.ShowWindow(SW_HIDE);

		cur = -1;
		m_enemylist.SetCurSel(0);
		OnSelchangeEnemylist();

		m_banner.Set(this, RGB(0, 0, 0), RGB(255, 32, 64), "Enemies");

		SetRedraw(TRUE);
		Invalidate();
		CenterToParent(this);
	}
	catch (std::exception & ex) {
		return Ui::AbortFail(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortFail(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}

void CEnemy::OnSelchangeEnemylist()
{
	if (cur != -1) StoreValues();
	cur = m_enemylist.GetCurSel();
	LoadValues();

	if (m_usagedataon && UpdateUsageData(cur) && m_dlgdatalist.IsWindowVisible())
		m_dlgdatalist.ShowWindow(SW_SHOW);
}

bool CEnemy::UpdateUsageData(int enemyindex)
{
	auto ok = false;
	try {
		m_usedata.Reset();

		// form 1: ids [2,3,4,5] check qtys [6,7,8,9]
		// form 2: ids [2,3] check qtys [E,F]
		static const std::vector<int> formenm1 = { 2, 3, 4, 5 };
		static const std::vector<int> formqty1 = { 6, 7, 8, 9 }; 
		static const std::vector<int> formenm2 = { 2, 3 };
		static const std::vector<int> formqty2 = { 0xE, 0xF };

		// Include if the battle has this enemy with a non-zero qty in this formation.
		const auto includer = [&](int keyindex, int battleindex) -> bool {
			auto battleid = (battleindex & 0x7F);
			auto formation = (battleindex & 0x80) ? 2 : 1;
			auto offset = BATTLE_OFFSET + (battleid * BATTLE_BYTES);

			const auto& renm = formation == 1 ? formenm1 : formenm2;
			const auto& rqty = formation == 1 ? formqty1 : formqty2;
			ASSERT(renm.size() == rqty.size());
			for (auto i = 0u; i < renm.size(); ++i) {
				auto ienm = renm[i];
				auto iqty = rqty[i];
				auto sloten = Project->ROM[offset + ienm];
				if (sloten == keyindex) {
					auto slotqty = Project->ROM[offset + iqty] & 0xF;
					if (slotqty > 0)
						return true;
				}
			}
			return false;
		};
		ok = m_usedata.UpdateUseData(enemyindex, includer, UsageDataFormatter);
	}
	catch (std::exception& ex) {
		ErrorHere << "Can't calculate use data: " << ex.what() << std::endl;
	}
	catch (...) {
		ErrorHere << "Can't calculate use data due to an unexpected exception." << std::endl;
	}

	CString caption;
	caption.Format("Where is enemy %02X used?", enemyindex);
	m_dlgdatalist.SetWindowText(caption);
	m_dlgdatalist.Load(m_usedata.m_usedatarefs);
	return ok;
}

//STATIC
CString CEnemy::UsageDataFormatter(CFFHacksterProject& proj, const sUseData& u)
{
	CString fmt;
	switch (u.type)
	{
	case UseDataType::Overworld:
	{
		fmt.Format("Overworld %d,%d \tBattle %02X \tSlot %d \tF%d",
			u.x, u.y, u.battleid, u.slot+1, u.formation);
		break;
	}
	case UseDataType::StdMap:
	{
		fmt.Format("%s \tBattle %02X \tSlot %d \tF%d", Editorlabels::LoadMapLabel(proj, (int)u.mapid, false).name,
			u.battleid, u.slot+1, u.formation);
		break;
	}
	case UseDataType::SpikedSquare:
	{
		fmt.Format("%s [Tile %X,%X] \tBattle %02X \t \tF%d", Editorlabels::LoadMapLabel(proj, (int)u.mapid, false).name,
			u.x, u.y, u.battleid, u.formation);
		break;
	}
	case UseDataType::SpriteDialogue:
	{
		fmt.Format("Sprite %02X %s \tBattle %02X \tF%d", u.mapid,
			Editorlabels::LoadSpriteLabel(proj, (int)u.mapid, false).name, (int)u.battleid, u.formation);
		break;
	}
	default:
		throw std::runtime_error("Unknown UseData type " + std::to_string((int)u.type));
	}
	return fmt;
}

void CEnemy::LoadCustomizedControls()
{
	CEnemyEditorSettings stgs(*Project);
	bool isflags = stgs.Byte15AsFlags;
	CString name15 = stgs.Byte15Name;

	for (auto wnd : { &m_atkelem1,&m_atkelem2,&m_atkelem3,&m_atkelem4,&m_atkelem5,&m_atkelem6,&m_atkelem7,&m_atkelem8 })
		wnd->ShowWindow(isflags ? SW_SHOW : SW_HIDE);

	m_unknown.ShowWindow(isflags ? SW_HIDE : SW_SHOW);
	m_statAttackElem.ShowWindow(isflags ? SW_HIDE : SW_SHOW);

	CString csbyte15;
	if (isflags) {
		csbyte15.Format("Byte 15: %s Flags", (LPCSTR)name15);
		m_groupByte15.SetWindowText(csbyte15);
		m_statAttackElem.SetWindowText("");

		//TODO - this if feels like a holdover from the previous implementation (though the block is still needed)
		if (!m_wasflags) {
			SetByte15FlagsFromValue(GetByte15HaxValue());
		}
	}
	else {
		csbyte15.Format("%s", (LPCSTR)name15);
		m_statAttackElem.SetWindowText(csbyte15);
		m_groupByte15.SetWindowText("Byte 15 (hex)");

		//TODO - this if feels like a holdover from the previous implementation (though the block is still needed)
		if (m_wasflags) {
			CString cs;
			cs.Format("%02X", GetByte15ValueFromFlags());
			m_unknown.SetWindowText(cs);
		}
	}
}

void CEnemy::LoadRom()
{
	Project->ClearROM();

	ENEMY_OFFSET = ReadHex(Project->ValuesPath, "ENEMY_OFFSET");
	ENEMY_BYTES = ReadDec(Project->ValuesPath, "ENEMY_BYTES");
	AI_OFFSET = ReadHex(Project->ValuesPath, "AI_OFFSET");
	AI_COUNT = ReadDec(Project->ValuesPath, "AI_COUNT");
	ENEMYTEXT_OFFSET = ReadHex(Project->ValuesPath, "ENEMYTEXT_OFFSET");
	BATTLE_OFFSET = ReadHex(Project->ValuesPath, "BATTLE_OFFSET");
	BATTLE_BYTES = ReadHex(Project->ValuesPath, "BATTLE_BYTES");

	BANK0A_OFFSET = ReadHex(Project->ValuesPath, "BANK0A_OFFSET");
	BINENEMYDATA_OFFSET = ENEMY_OFFSET; //ReadHex(Project->ValuesPath, "ENEMY_OFFSET");
	BINAIDATA_OFFSET = ReadHex(Project->ValuesPath, "AI_OFFSET");
	BINBATTLEFORMATIONS_OFFSET = ReadHex(Project->ValuesPath, "BINBATTLEFORMATIONS_OFFSET");

	// used by subeditors
	BINBATTLEPALETTES_OFFSET = ReadHex(Project->ValuesPath, "BINBATTLEPALETTES_OFFSET");
	BINCHAOSTSA_OFFSET = ReadHex(Project->ValuesPath, "BINCHAOSTSA_OFFSET");
	BINFIENDTSA_OFFSET = ReadHex(Project->ValuesPath, "BINFIENDTSA_OFFSET");
	BANK00_OFFSET = ReadHex(Project->ValuesPath, "BANK00_OFFSET");
	BANK07_OFFSET = ReadHex(Project->ValuesPath, "BANK07_OFFSET");

	// Now load the data
	if (Project->IsRom()) {
		load_binary(Project->WorkRomPath, Project->ROM);
	}
	else if (Project->IsAsm()) {
		GameSerializer ser(*Project);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.LoadAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.LoadAsmBin(BIN_AIDATA, BINAIDATA_OFFSET);
		ser.LoadAsmBin(BIN_BATTLEFORMATIONS, BINBATTLEFORMATIONS_OFFSET);
		ser.LoadAsmBin(BIN_ENEMYNAMES, ENEMYTEXT_OFFSET);
		ser.LoadAsmBin(BIN_ENEMYDATA, BINENEMYDATA_OFFSET);

		// used by subeditors
		ser.LoadAsmBin(BANK_00, BANK00_OFFSET);
		ser.LoadAsmBin(BANK_07, BANK07_OFFSET); 
		ser.LoadAsmBin(BIN_BATTLEPALETTES, BINBATTLEPALETTES_OFFSET); 
		ser.LoadAsmBin(BIN_CHAOSTSA, BINCHAOSTSA_OFFSET);
		ser.LoadAsmBin(BIN_FIENDTSA, BINFIENDTSA_OFFSET);

		if (m_usagedataon) {
			// Used by EnemyBattleUsageData (read-only)
			//TODO - use CLoading instead?
			CWaitCursor wait;
			auto BANK04_OFFSET = ReadHex(Project->ValuesPath, "BANK04_OFFSET");
			auto BANK05_OFFSET = ReadHex(Project->ValuesPath, "BANK05_OFFSET");
			auto BANK06_OFFSET = ReadHex(Project->ValuesPath, "BANK06_OFFSET");
			auto BATTLEDOMAIN_OFFSET = ReadHex(Project->ValuesPath, "BATTLEDOMAIN_OFFSET");
			unsigned int BATTLEPROBABILITY_OFFSET = ReadHex(Project->ValuesPath, "BATTLEPROBABILITY_OFFSET");
			auto TALKROUTINEDATA_OFFSET = Ini::ReadHex(Project->ValuesPath, "TALKROUTINEDATA_OFFSET");
			ser.LoadAsmBin(BANK_04, BANK04_OFFSET);
			ser.LoadAsmBin(BANK_05, BANK05_OFFSET);
			ser.LoadAsmBin(BANK_06, BANK06_OFFSET);
			ser.LoadAsmBin(BIN_OBJECTDATA, TALKROUTINEDATA_OFFSET);
			ser.LoadAsmBin(BIN_BATTLEDOMAINDATA, BATTLEDOMAIN_OFFSET);
			ser.LoadInline(ASM_0F, { { asmlabel, "lut_FormationWeight", { BATTLEPROBABILITY_OFFSET } } });
			ser.PreloadTalkAsmData(ASM_0E);
		}
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, (LPCSTR)Project->ProjectTypeName);
	}
}

void CEnemy::SaveRom()
{
	if (Project->IsRom()) {
		save_binary(Project->WorkRomPath, Project->ROM);
	}
	else if (Project->IsAsm()) {
		GameSerializer ser(*Project);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.SaveAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.SaveAsmBin(BIN_AIDATA, BINAIDATA_OFFSET);
		ser.SaveAsmBin(BIN_BATTLEFORMATIONS, BINBATTLEFORMATIONS_OFFSET);
		ser.SaveAsmBin(BIN_ENEMYNAMES, ENEMYTEXT_OFFSET);
		ser.SaveAsmBin(BIN_ENEMYDATA, BINENEMYDATA_OFFSET);

		// used by subeditors
		ser.SaveAsmBin(BANK_00, BANK00_OFFSET);
		ser.SaveAsmBin(BANK_07, BANK07_OFFSET);
		ser.SaveAsmBin(BIN_BATTLEPALETTES, BINBATTLEPALETTES_OFFSET);
		ser.SaveAsmBin(BIN_CHAOSTSA, BINCHAOSTSA_OFFSET);
		ser.SaveAsmBin(BIN_FIENDTSA, BINFIENDTSA_OFFSET);
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, (LPCSTR)Project->ProjectTypeName);
	}
}

void CEnemy::LoadValues()
{
	int offset = ENEMY_OFFSET + (cur * ENEMY_BYTES);
	int temp;
	CString text;

	temp = Project->ROM[offset] + (Project->ROM[offset + 1] << 8);
	text.Format("%d",temp);
	m_exp.SetWindowText(text);

	temp = Project->ROM[offset + 2] + (Project->ROM[offset + 3] << 8);
	text.Format("%d",temp);
	m_gold.SetWindowText(text);

	temp = Project->ROM[offset + 4] + (Project->ROM[offset + 5] << 8);
	text.Format("%d",temp);
	m_hp.SetWindowText(text);

	text.Format("%d",Project->ROM[offset + 6]);
	m_morale.SetWindowText(text);

	cur_ai = (Project->ROM[offset + 7] + 1) & 0xFF;
	m_ai.SetCurSel(cur_ai);
	LoadAI();

	text.Format("%d",Project->ROM[offset + 8]);
	m_agility.SetWindowText(text);

	text.Format("%d",Project->ROM[offset + 9]);
	m_def.SetWindowText(text);
	
	text.Format("%d",Project->ROM[offset + 10]);
	m_hits.SetWindowText(text);
	
	text.Format("%d",Project->ROM[offset + 11]);
	m_hit.SetWindowText(text);
	
	text.Format("%d",Project->ROM[offset + 12]);
	m_str.SetWindowText(text);
	
	text.Format("%d",Project->ROM[offset + 13]);
	m_critical.SetWindowText(text);

	CEnemyEditorSettings stgs(*Project);
	if (stgs.Byte15AsFlags)
	{
		SetByte15FlagsFromValue(Project->ROM[offset + 14]);
	}
	else {
		text.Format("%02X", Project->ROM[offset + 14]);
		m_unknown.SetWindowText(text);
	}
	LoadCustomizedControls();

	temp = Project->ROM[offset + 15];
	SetCheckFlags(temp, std::vector<CStrikeCheck*>{ &m_eff1, &m_eff2, &m_eff3, &m_eff4, &m_eff5, &m_eff6, &m_eff7, &m_eff8 });

	
	temp = Project->ROM[offset + 16];
	SetCheckFlags(temp, std::vector<CStrikeCheck*>{ &m_cat1, &m_cat2, &m_cat3, &m_cat4, &m_cat5, &m_cat6, &m_cat7, &m_cat8 });
	
	text.Format("%d",Project->ROM[offset + 17]);
	m_magdef.SetWindowText(text);
	
	temp = Project->ROM[offset + 18];
	SetCheckFlags(temp, std::vector<CStrikeCheck*>{ &m_weak1, &m_weak2, &m_weak3, &m_weak4, &m_weak5, &m_weak6, &m_weak7, &m_weak8 });
	
	temp = Project->ROM[offset + 19];
	SetCheckFlags(temp, std::vector<CStrikeCheck*>{ &m_res1, &m_res2, &m_res3, &m_res4, &m_res5, &m_res6, &m_res7, &m_res8 });
}

void CEnemy::StoreValues()
{
	if(m_ai.GetCurSel()) StoreAI();

	int offset = ENEMY_OFFSET + (cur * ENEMY_BYTES);
	int temp;
	CString text;

	m_exp.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFFFF) temp = 0xFFFF;
	Project->ROM[offset] = temp & 0xFF; Project->ROM[offset + 1] = (BYTE)(temp >> 8);

	m_gold.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFFFF) temp = 0xFFFF;
	Project->ROM[offset + 2] = temp & 0xFF; Project->ROM[offset + 3] = (BYTE)(temp >> 8);

	m_hp.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFFFF) temp = 0xFFFF;
	Project->ROM[offset + 4] = temp & 0xFF; Project->ROM[offset + 5] = (BYTE)(temp >> 8);

	m_morale.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 6] = (BYTE)temp;

	temp = m_ai.GetCurSel(); if(!temp) temp = 0x100;
	Project->ROM[offset + 7] = (BYTE)(temp - 1);

	m_agility.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 8] = (BYTE)temp;

	m_def.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 9] = (BYTE)temp;

	m_hits.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 10] = (BYTE)temp;

	m_hit.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 11] = (BYTE)temp;

	m_str.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 12] = (BYTE)temp;

	m_critical.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 13] = (BYTE)temp;

	CEnemyEditorSettings stgs(*Project);
	if (stgs.Byte15AsFlags)
	{
		temp = GetByte15ValueFromFlags();
	}
	else {
		m_unknown.GetWindowText(text);
		temp = StringToInt_HEX(text);
		if (temp > 0xFF) temp = 0xFF;
	}
	Project->ROM[offset + 14] = (BYTE)temp;

	temp = GetCheckFlags(std::vector<CStrikeCheck*>{ &m_eff1, &m_eff2, &m_eff3, &m_eff4, &m_eff5, &m_eff6, &m_eff7, &m_eff8 });
	Project->ROM[offset + 15] = (BYTE)temp;
	
	temp = GetCheckFlags(std::vector<CStrikeCheck*>{ &m_cat1, &m_cat2, &m_cat3, &m_cat4, &m_cat5, &m_cat6, &m_cat7, &m_cat8 });
	Project->ROM[offset + 16] = (BYTE)temp;

	m_magdef.GetWindowText(text); temp = StringToInt(text);
	if(temp > 0xFF) temp = 0xFF;
	Project->ROM[offset + 17] = (BYTE)temp;

	temp = 0;
	temp = GetCheckFlags(std::vector<CStrikeCheck*>{ &m_weak1, &m_weak2, &m_weak3, &m_weak4, &m_weak5, &m_weak6, &m_weak7, &m_weak8 });
	Project->ROM[offset + 18] = (BYTE)temp;

	temp = 0;
	temp = GetCheckFlags(std::vector<CStrikeCheck*>{ &m_res1, &m_res2, &m_res3, &m_res4, &m_res5, &m_res6, &m_res7, &m_res8 });
	Project->ROM[offset + 19] = (BYTE)temp;
}

void CEnemy::LoadAI()
{
	m_ai_magicrate.EnableWindow(cur_ai);
	m_ai_magic1.EnableWindow(cur_ai);
	m_ai_magic2.EnableWindow(cur_ai);
	m_ai_magic3.EnableWindow(cur_ai);
	m_ai_magic4.EnableWindow(cur_ai);
	m_ai_magic5.EnableWindow(cur_ai);
	m_ai_magic6.EnableWindow(cur_ai);
	m_ai_magic7.EnableWindow(cur_ai);
	m_ai_magic8.EnableWindow(cur_ai);
	m_ai_abilityrate.EnableWindow(cur_ai);
	m_ai_ability1.EnableWindow(cur_ai);
	m_ai_ability2.EnableWindow(cur_ai);
	m_ai_ability3.EnableWindow(cur_ai);
	m_ai_ability4.EnableWindow(cur_ai);
	m_editlabel.EnableWindow(cur_ai);

	CString text = "";
	if(!cur_ai){
		m_ai_magicrate.SetWindowText(text);
		m_ai_abilityrate.SetWindowText(text);
		m_ai_magic1.SetCurSel(0);
		m_ai_magic2.SetCurSel(0);
		m_ai_magic3.SetCurSel(0);
		m_ai_magic4.SetCurSel(0);
		m_ai_magic5.SetCurSel(0);
		m_ai_magic6.SetCurSel(0);
		m_ai_magic7.SetCurSel(0);
		m_ai_magic8.SetCurSel(0);
		m_ai_ability1.SetCurSel(0);
		m_ai_ability2.SetCurSel(0);
		m_ai_ability3.SetCurSel(0);
		m_ai_ability4.SetCurSel(0);
		return;}

	int ref = AI_OFFSET + ((cur_ai - 1) << 4);

	text.Format("%d",Project->ROM[ref]);
	m_ai_magicrate.SetWindowText(text);
	
	text.Format("%d",Project->ROM[ref + 1]);
	m_ai_abilityrate.SetWindowText(text);

	m_ai_magic1.SetCurSel((Project->ROM[ref + 2] + 1) & 0xFF);
	m_ai_magic2.SetCurSel((Project->ROM[ref + 3] + 1) & 0xFF);
	m_ai_magic3.SetCurSel((Project->ROM[ref + 4] + 1) & 0xFF);
	m_ai_magic4.SetCurSel((Project->ROM[ref + 5] + 1) & 0xFF);
	m_ai_magic5.SetCurSel((Project->ROM[ref + 6] + 1) & 0xFF);
	m_ai_magic6.SetCurSel((Project->ROM[ref + 7] + 1) & 0xFF);
	m_ai_magic7.SetCurSel((Project->ROM[ref + 8] + 1) & 0xFF);
	m_ai_magic8.SetCurSel((Project->ROM[ref + 9] + 1) & 0xFF);

	m_ai_ability1.SetCurSel((Project->ROM[ref + 11] + 1) & 0xFF);
	m_ai_ability2.SetCurSel((Project->ROM[ref + 12] + 1) & 0xFF);
	m_ai_ability3.SetCurSel((Project->ROM[ref + 13] + 1) & 0xFF);
	m_ai_ability4.SetCurSel((Project->ROM[ref + 14] + 1) & 0xFF);
}

void CEnemy::StoreAI()
{
	int ref = AI_OFFSET + ((cur_ai - 1) << 4);
	CString text;
	int temp;

	m_ai_magicrate.GetWindowText(text);
	temp = StringToInt(text); if(temp > 0xFF) temp = 0xFF;
	Project->ROM[ref] = (BYTE)temp;

	m_ai_abilityrate.GetWindowText(text);
	temp = StringToInt(text); if(temp > 0xFF) temp = 0xFF;
	Project->ROM[ref + 1] = (BYTE)temp;

	temp = m_ai_magic1.GetCurSel();
	if(temp) Project->ROM[ref + 2] = (BYTE)(temp - 1);
	else Project->ROM[ref + 2] = 0xFF;
	temp = m_ai_magic2.GetCurSel();
	if(temp) Project->ROM[ref + 3] = (BYTE)(temp - 1);
	else Project->ROM[ref + 3] = 0xFF;
	temp = m_ai_magic3.GetCurSel();
	if(temp) Project->ROM[ref + 4] = (BYTE)(temp - 1);
	else Project->ROM[ref + 4] = 0xFF;
	temp = m_ai_magic4.GetCurSel();
	if(temp) Project->ROM[ref + 5] = (BYTE)(temp - 1);
	else Project->ROM[ref + 5] = 0xFF;
	temp = m_ai_magic5.GetCurSel();
	if(temp) Project->ROM[ref + 6] = (BYTE)(temp - 1);
	else Project->ROM[ref + 6] = 0xFF;
	temp = m_ai_magic6.GetCurSel();
	if(temp) Project->ROM[ref + 7] = (BYTE)(temp - 1);
	else Project->ROM[ref + 7] = 0xFF;
	temp = m_ai_magic7.GetCurSel();
	if(temp) Project->ROM[ref + 8] = (BYTE)(temp - 1);
	else Project->ROM[ref + 8] = 0xFF;
	temp = m_ai_magic8.GetCurSel();
	if(temp) Project->ROM[ref + 9] = (BYTE)(temp - 1);
	else Project->ROM[ref + 9] = 0xFF;

	temp = m_ai_ability1.GetCurSel();
	if(temp) Project->ROM[ref + 11] = (BYTE)(temp - 1);
	else Project->ROM[ref + 11] = 0xFF;
	temp = m_ai_ability2.GetCurSel();
	if(temp) Project->ROM[ref + 12] = (BYTE)(temp - 1);
	else Project->ROM[ref + 12] = 0xFF;
	temp = m_ai_ability3.GetCurSel();
	if(temp) Project->ROM[ref + 13] = (BYTE)(temp - 1);
	else Project->ROM[ref + 13] = 0xFF;
	temp = m_ai_ability4.GetCurSel();
	if(temp) Project->ROM[ref + 14] = (BYTE)(temp - 1);
	else Project->ROM[ref + 14] = 0xFF;
}

void CEnemy::PaintClient(CDC & dc)
{
	__super::PaintClient(dc);
}

unsigned char CEnemy::GetByte15HaxValue()
{
	CString cs;
	m_unknown.GetWindowText(cs);
	unsigned char temp = StringToInt_HEX(cs) & 0xFF;
	return temp;
}

unsigned char CEnemy::GetByte15ValueFromFlags()
{
	return 0xFF & GetCheckFlags(std::vector<CButton*>{&m_atkelem1, &m_atkelem2, &m_atkelem3, &m_atkelem4, &m_atkelem5, &m_atkelem6, &m_atkelem7, &m_atkelem8 });
}

void CEnemy::SetByte15FlagsFromValue(unsigned char value)
{
	SetCheckFlags(value & 0xFF, std::vector<CButton*>{&m_atkelem1, &m_atkelem2, &m_atkelem3, &m_atkelem4, &m_atkelem5, &m_atkelem6, &m_atkelem7, &m_atkelem8 });
}


BEGIN_MESSAGE_MAP(CEnemy, BaseClass)
	ON_LBN_SELCHANGE(IDC_ENEMYLIST, OnSelchangeEnemylist)
	ON_CBN_SELCHANGE(IDC_AI, OnSelchangeAi)
	ON_BN_CLICKED(IDC_EDITLABEL, OnEditlabel)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_EDITPIC, OnEditpic)
	ON_BN_CLICKED(IDC_CLASSES_SETTINGS, &CEnemy::OnBnClickedSettings)
	ON_BN_CLICKED(IDC_ENEMY_BTN_VIEWUSE, &CEnemy::OnBnClickedEnemyBtnViewuse)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEnemy message handlers

void CEnemy::OnSelchangeAi() 
{
	if(cur_ai > 0) StoreAI();

	cur_ai = m_ai.GetCurSel();

	LoadAI();
}

void CEnemy::OnEditlabel() 
{
	int temp = m_ai.GetCurSel();
	ChangeLabel(*Project, -1, LoadAILabel(*Project, temp - 1), WriteAILabel, temp, nullptr, &m_ai);
}

void CEnemy::OnEditpic()
{
	OnSave();
	BYTE temp;
	BYTE battle = 0;
	BYTE pal[2] = { 0 };
	BYTE graphic;
	BYTE enref = 0xFF;
	BYTE battletype;
	int en = m_enemylist.GetCurSel();
	int co;
	for(co = BATTLE_OFFSET; battle < 0x80; co += 0x10, battle++){
		for(temp = 2; temp < 6; temp++){
			if(Project->ROM[co + temp] == en){
				enref = temp - 2;
				break;}}
		if(enref != 0xFF) break;}

	if(enref == 0xFF){
		AfxMessageBox("Enemy is not placed in any battles.\nCould not find enemy pic", MB_ICONERROR);
		return;}

	battletype = Project->ROM[co] >> 4;
	if(battletype < 3) battletype = 0;
	else if(battletype == 4) battletype = 5;
	else battletype = ((Project->ROM[co + 1] >> (enref << 1)) & 3) + 1;
	

	graphic = Project->ROM[co] & 0x0F;
	pal[0] = Project->ROM[co + 10];
	pal[1] = Project->ROM[co + 11];
	if(!battletype){
		if((Project->ROM[co + 13] >> (7 - enref)) & 1)
			pal[0] = pal[1];
		else
			pal[1] = pal[0];}

	CBattlePatternTables dlg;
	dlg.cart = Project;
	dlg.patterntable = graphic;
	dlg.palvalues[0] = pal[0];
	dlg.palvalues[1] = pal[1];
	dlg.view = battletype;

	dlg.DoModal();
}

void CEnemy::OnBnClickedSettings()
{
	// Hang on to the current setting, which will become the previous setting if the dialog is OK'd.
	CEnemyEditorSettings stgs(*Project);
	bool useflags = stgs.Byte15AsFlags;
	bool wasViewUsage = stgs.ViewUsage;

	CEnemyEditorSettingsDlg dlg(this);
	dlg.Project = Project;
	if (dlg.DoModal() == IDOK) {
		// Update the previous setting and reload the affected controls.
		m_wasflags = useflags;
		LoadCustomizedControls();

		// Don't change the View Usage setting now, let the user know that it will change on reload.
		stgs.Read();
		if (!wasViewUsage && stgs.ViewUsage) {
			AfxMessageBox("View Usage will be available when you restart the editor.");
		}
		else if (wasViewUsage && !stgs.ViewUsage) {
			AfxMessageBox("View Usage will be hidden when you restart the editor.");
		}
	}
}

void CEnemy::OnBnClickedEnemyBtnViewuse()
{
	m_dlgdatalist.ShowWindow(SW_SHOW);
}