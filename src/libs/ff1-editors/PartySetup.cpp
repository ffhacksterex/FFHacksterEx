// FixedParty.cpp : implementation file
//

#include "stdafx.h"
#include "PartySetup.h"
#include "AsmFiles.h"
#include "FFHacksterProject.h"
#include "GameSerializer.h"
#include "general_functions.h"
#include "imaging_helpers.h"
#include "ingame_text_functions.h"
#include "ini_functions.h"
#include "io_functions.h"
#include "ui_helpers.h"

#include <RomAsmMappingFactory.h>
#include <RomAsmSerializer.h>

using namespace Imaging;
using namespace Ini;
using namespace Ingametext;
using namespace Io;
using namespace Ui;

// CPartySetup dialog
#define BaseClass CEditorWithBackground


IMPLEMENT_DYNAMIC(CPartySetup, BaseClass)

CPartySetup::CPartySetup(CWnd* pParent /*= nullptr */)
	: BaseClass(IDD_PARTYSETUP, pParent)
{
}

CPartySetup::~CPartySetup()
{
}

void CPartySetup::LoadOffsets()
{
	RomAsmMappingFactory fac;
	m_groupedmappings = fac.ReadGroupedMappings(*Project, "partysetup");

	CLASS_COUNT = ReadDec(Project->ValuesPath, "CLASS_COUNT");
	NEWPARTYCLASSCOUNT_OFFSET = ReadHex(Project->ValuesPath, "NEWPARTYCLASSCOUNT_OFFSET");
	NEWPARTYCLASSINC_OFFSET = ReadHex(Project->ValuesPath, "NEWPARTYCLASSINC_OFFSET");
	PTYGEN_OFFSET = ReadHex(Project->ValuesPath, "PTYGEN_OFFSET");
	BANK0A_OFFSET = ReadHex(Project->ValuesPath, "BANK0A_OFFSET");
}

void CPartySetup::LoadRom()
{
	Project->ClearROM();
	RomAsmSerializer ras(*Project);
	ras.Load(m_groupedmappings, Project->ROM);

	if (Project->IsRom()) {
		load_binary(Project->WorkRomPath, Project->ROM);
	}
	else if (Project->IsAsm()) {
		//TODO - eventually add rom support the serializer
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, (LPCSTR)Project->ProjectTypeName);
	}
}

void CPartySetup::SaveRom()
{
	if (Project->IsRom()) {
		save_binary(Project->WorkRomPath, Project->ROM);
	}
	else if (Project->IsAsm()) {
		GameSerializer ser(*Project);
		//ser.ReadConstants("constants.inc");
		// Instead of writing to the entire buffer, just write to the parts we need
		//NOTE: bank A is read-only in this editor, no need to write it
		ser.SaveInline(ASM_0E, {
			{ asmopval, "op_NewPartyClassCount",{ NEWPARTYCLASSCOUNT_OFFSET } },
			{ asmopval, "op_NewPartyClassInc",{ NEWPARTYCLASSINC_OFFSET } },
			{ asmlabel, "lut_PtyGenBuf",{ PTYGEN_OFFSET } },
		});
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::writing, (LPCSTR)Project->ProjectTypeName);
	}
}

void CPartySetup::StoreValues()
{
	unsigned long newpartyclasscount = 0;
	CString text;

	m_newgamecountedit.GetWindowText(text);
	newpartyclasscount = strtoul(text, nullptr, 10);
	if (newpartyclasscount > (unsigned long)CLASS_COUNT) {
		text.Format("The class count cannot exceed %d.", CLASS_COUNT);
		AfxMessageBox(text, MB_ICONERROR);
		newpartyclasscount = CLASS_COUNT;
		return;
	}

	bool enabled = m_fixedpartycheck.GetCheck() == BST_CHECKED;

	auto SaveCombo = [this](CComboBox & box, int classoffset) {
		int offset = this->PTYGEN_OFFSET + classoffset;
		int iclass = box.GetCurSel();
		Project->ROM[offset] = (BYTE)iclass;
	};

	Project->ROM[NEWPARTYCLASSINC_OFFSET] = enabled ? 0 : 1;
	Project->ROM[NEWPARTYCLASSCOUNT_OFFSET] = (BYTE)newpartyclasscount;
	SaveCombo(m_classcombo1, 0x00);
	SaveCombo(m_classcombo2, 0x10);
	SaveCombo(m_classcombo3, 0x20);
	SaveCombo(m_classcombo4, 0x30);
}

void CPartySetup::DoDataExchange(CDataExchange* pDX)
{
	BaseClass::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDC_FIXEDPARTY_COMBO1, m_classcombo1);
	DDX_Control(pDX, IDC_FIXEDPARTY_COMBO2, m_classcombo2);
	DDX_Control(pDX, IDC_FIXEDPARTY_COMBO3, m_classcombo3);
	DDX_Control(pDX, IDC_FIXEDPARTY_COMBO4, m_classcombo4);
	DDX_Control(pDX, IDC_CLASS_NEWPARTYCLASSCOUNT, m_newgamecountedit);
	DDX_Control(pDX, IDC_CLASS_FIXEDPARTYCHECK, m_fixedpartycheck);
	DDX_Control(pDX, IDC_SAVE, m_savebutton);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
}


BEGIN_MESSAGE_MAP(CPartySetup, BaseClass)
	ON_BN_CLICKED(IDC_SAVE, &CPartySetup::OnSave)
END_MESSAGE_MAP()


// CPartySetup message handlers


BOOL CPartySetup::OnInitDialog()
{
	BaseClass::OnInitDialog();

	try {
		this->LoadOffsets();
		this->LoadRom();

		m_banner.Set(this, COLOR_BLACK, COLOR_ORANGE, "Party Setup");

		auto LoadClassCombo = [this](CComboBox & box, int classoffset) {
			int offset = Project->ROM[this->PTYGEN_OFFSET + classoffset];
			box.ResetContent();
			LoadCombo(box, LoadClassEntries(*Project));
			if (offset >= 0 && offset < box.GetCount())
				box.SetCurSel(offset);
			else if (box.GetCount() > 0)
				box.SetCurSel(0);
		};

		CString text;
		text.Format("%d", Project->ROM[NEWPARTYCLASSCOUNT_OFFSET]);
		m_newgamecountedit.SetWindowText(text);

		bool enabled = Project->ROM[NEWPARTYCLASSINC_OFFSET] == 0;
		m_fixedpartycheck.SetCheck(enabled ? BST_CHECKED : BST_UNCHECKED);
		LoadClassCombo(m_classcombo1, 0x00);
		LoadClassCombo(m_classcombo2, 0x10);
		LoadClassCombo(m_classcombo3, 0x20);
		LoadClassCombo(m_classcombo4, 0x30);
	}
	catch (std::exception & ex) {
		return Ui::AbortInitDialog(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortInitDialog(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}