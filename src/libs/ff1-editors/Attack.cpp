// Attack.cpp : implementation file
//

#include "stdafx.h"
#include "Attack.h"
#include "AsmFiles.h"
#include <core_exceptions.h>
#include "editor_label_functions.h"
#include <FFH2Project.h>
#include <FFHDataValue_dva.hpp>
#include "GameSerializer.h"
#include "general_functions.h"
#include "imaging_helpers.h"
#include "ingame_text_functions.h"
#include "ini_functions.h"
#include "io_functions.h"
#include <string_conversions.hpp>
#include "string_functions.h"
#include "ui_helpers.h"

using namespace Editorlabels;
using namespace Imaging;
using namespace Ini;
using namespace Ingametext;
using namespace Io;
using namespace Strings;
using namespace Ui;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttack dialog
#define BaseClass CEditorWithBackground


CAttack::CAttack(CWnd* pParent /*= nullptr */)
	: CEditorWithBackground(CAttack::IDD, pParent)
{
}

void CAttack::DoDataExchange(CDataExchange* pDX)
{
	CEditorWithBackground::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EFF_EFFECT, m_eff_effect);
	DDX_Control(pDX, IDC_EFF_DAMAGE, m_eff_damage);
	DDX_Control(pDX, IDC_UNKNOWN, m_unknown);
	DDX_Control(pDX, IDC_TARGET, m_target);
	DDX_Control(pDX, IDC_ELEM8, m_elem8);
	DDX_Control(pDX, IDC_ELEM7, m_elem7);
	DDX_Control(pDX, IDC_ELEM6, m_elem6);
	DDX_Control(pDX, IDC_ELEM5, m_elem5);
	DDX_Control(pDX, IDC_ELEM4, m_elem4);
	DDX_Control(pDX, IDC_ELEM3, m_elem3);
	DDX_Control(pDX, IDC_ELEM2, m_elem2);
	DDX_Control(pDX, IDC_ELEM1, m_elem1);
	DDX_Control(pDX, IDC_EFFECTBOX, m_effectbox);
	DDX_Control(pDX, IDC_EFF8, m_eff8);
	DDX_Control(pDX, IDC_EFF7, m_eff7);
	DDX_Control(pDX, IDC_EFF6, m_eff6);
	DDX_Control(pDX, IDC_EFF5, m_eff5);
	DDX_Control(pDX, IDC_EFF4, m_eff4);
	DDX_Control(pDX, IDC_EFF3, m_eff3);
	DDX_Control(pDX, IDC_EFF2, m_eff2);
	DDX_Control(pDX, IDC_EFF1, m_eff1);
	DDX_Control(pDX, IDC_DAMAGETEXT, m_damagetext);
	DDX_Control(pDX, IDC_DAMAGE, m_damage);
	DDX_Control(pDX, IDC_ATTACKLIST, m_attacklist);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDC_SAVE, m_savebutton);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
}

BEGIN_MESSAGE_MAP(CAttack, CEditorWithBackground)
	ON_LBN_SELCHANGE(IDC_ATTACKLIST, OnSelchangeAttacklist)
	ON_BN_CLICKED(IDC_EFF_DAMAGE, OnEffDamage)
	ON_BN_CLICKED(IDC_EFF_EFFECT, OnEffEffect)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
END_MESSAGE_MAP()


BOOL CAttack::OnInitDialog()
{
	CEditorWithBackground::OnInitDialog();

	try {
		this->LoadOffsets();
		this->LoadRom();

		LoadListBox(m_attacklist, LoadAttackEntries(*Proj2));
		LoadCaptions(std::vector<CWnd*>{ &m_elem1, &m_elem2, &m_elem3, &m_elem4, &m_elem5, &m_elem6, &m_elem7, &m_elem8 }, LoadElementLabels(*Proj2));
		LoadCaptions(std::vector<CWnd*>{ &m_eff1, &m_eff2, &m_eff3, &m_eff4, &m_eff5, &m_eff6, &m_eff7, &m_eff8 }, LoadAilEffectLabels(*Proj2));

		cur = -1;
		m_attacklist.SetCurSel(0);
		OnSelchangeAttacklist();

		m_banner.Set(this, COLOR_BLACK, RGB(255, 224, 32), "Attacks");
		SetODDefButtonID(IDOK);
	}
	catch (std::exception & ex) {
		return Ui::AbortInitDialog(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortInitDialog(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}

void CAttack::LoadOffsets()
{
	ff1coredefs::DataValueAccessor d(*Proj2);
	ATTACK_OFFSET = d.get<int>("ATTACK_OFFSET");
	ATTACK_BYTES = d.get<int>("ATTACK_BYTES");
	BANK0A_OFFSET = d.get<int>("BANK0A_OFFSET");
	MAGIC_OFFSET = d.get<int>("MAGIC_OFFSET");
}

void CAttack::LoadRom()
{
	Proj2->ClearROM();
	if (Proj2->IsRom()) {
		load_binary(tomfc(Proj2->info.workrom), Proj2->ROM);
	}
	else if (Proj2->IsAsm()) {
		GameSerializer ser(*Proj2);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.LoadAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.LoadAsmBin(BIN_MAGICDATA, MAGIC_OFFSET);
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, Proj2->info.type);
	}
}

void CAttack::SaveRom()
{
	if (Proj2->IsRom()) {
		Proj2->SaveROM();
	}
	else if (Proj2->IsAsm()) {
		GameSerializer ser(*Proj2);
		ser.SaveAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.SaveAsmBin(BIN_MAGICDATA, MAGIC_OFFSET);
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, Proj2->info.type);
	}
}

void CAttack::LoadValues()
{
	int offset = ATTACK_OFFSET + (cur * ATTACK_BYTES);
	CString text;
	int temp;

	text.Format("%X", Proj2->ROM[offset]);
	if (text.GetLength() == 1) text = "0" + text;
	m_unknown.SetWindowText(text);

	m_target.SetCheck(Proj2->ROM[offset + 3] == 1);

	temp = Proj2->ROM[offset + 2];
	SetCheckFlags(temp, std::vector<CStrikeCheck*>{ &m_elem1, &m_elem2, &m_elem3, &m_elem4, &m_elem5, &m_elem6, &m_elem7, &m_elem8 });

	temp = Proj2->ROM[offset + 4];
	m_damagetext.ShowWindow(temp == 1);
	m_damage.ShowWindow(temp == 1);
	m_effectbox.ShowWindow(temp == 3);
	m_eff1.ShowWindow(temp == 3);
	m_eff2.ShowWindow(temp == 3);
	m_eff3.ShowWindow(temp == 3);
	m_eff4.ShowWindow(temp == 3);
	m_eff5.ShowWindow(temp == 3);
	m_eff6.ShowWindow(temp == 3);
	m_eff7.ShowWindow(temp == 3);
	m_eff8.ShowWindow(temp == 3);
	m_eff_damage.SetCheck(temp == 1);
	m_eff_effect.SetCheck(temp == 3);

	if (temp == 1) {
		text.Format("%d", Proj2->ROM[offset + 1]);
		m_damage.SetWindowText(text);
	}
	else {
		temp = Proj2->ROM[offset + 1];
		SetCheckFlags(temp, std::vector<CStrikeCheck*>{ &m_eff1, &m_eff2, &m_eff3, &m_eff4, &m_eff5, &m_eff6, &m_eff7, &m_eff8 });
	}
}

void CAttack::StoreValues()
{
	int offset = ATTACK_OFFSET + (cur * ATTACK_BYTES);
	CString text;
	int temp;

	m_unknown.GetWindowText(text); temp = StringToInt_HEX(text);
	if (temp > 0xFF) temp = 0xFF;
	Proj2->ROM[offset] = (BYTE)temp;

	temp = 0;
	if (m_eff_damage.GetCheck()) {
		m_damage.GetWindowText(text); temp = StringToInt(text);
		if (temp > 0xFF) temp = 0xFF;
	}
	if (m_eff_effect.GetCheck()) {
		temp = GetCheckFlags(std::vector<CStrikeCheck*>{ &m_eff1, &m_eff2, &m_eff3, &m_eff4, &m_eff5, &m_eff6, &m_eff7, &m_eff8 });
	}
	Proj2->ROM[offset + 1] = (BYTE)temp;

	temp = 0;
	temp = GetCheckFlags(std::vector<CStrikeCheck*>{ &m_elem1, &m_elem2, &m_elem3, &m_elem4, &m_elem5, &m_elem6, &m_elem7, &m_elem8 });
	Proj2->ROM[offset + 2] = (BYTE)temp;

	Proj2->ROM[offset + 3] = (BYTE)(2 - m_target.GetCheck());

	temp = 1;
	if (m_eff_effect.GetCheck()) temp = 3;
	Proj2->ROM[offset + 4] = (BYTE)temp;
}

void CAttack::ChangeEffect(bool eff)
{
	m_eff_damage.SetCheck(!eff);
	m_damagetext.ShowWindow(!eff);
	m_damage.ShowWindow(!eff);
	m_effectbox.ShowWindow(eff);
	m_eff_effect.SetCheck(eff);
	m_eff1.ShowWindow(eff);
	m_eff2.ShowWindow(eff);
	m_eff3.ShowWindow(eff);
	m_eff4.ShowWindow(eff);
	m_eff5.ShowWindow(eff);
	m_eff6.ShowWindow(eff);
	m_eff7.ShowWindow(eff);
	m_eff8.ShowWindow(eff);
}

void CAttack::PaintClient(CDC & dc)
{
	__super::PaintClient(dc);
}


/////////////////////////////////////////////////////////////////////////////
// CAttack message handlers

void CAttack::OnSelchangeAttacklist() 
{
	if(cur != -1) StoreValues();

	cur = m_attacklist.GetCurSel();

	LoadValues();
}

void CAttack::OnEffDamage()
{
	ChangeEffect(0);
}

void CAttack::OnEffEffect()
{
	ChangeEffect(1);
}