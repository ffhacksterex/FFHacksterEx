// Text.cpp : implementation file
//

#include "stdafx.h"
#include "Text.h"
#include <FFH2Project.h>
#include "collection_helpers.h"
#include <core_exceptions.h>
#include <DataValueAccessor.h>
#include <dva_primitives.h>
#include <editor_label_functions.h>
#include "general_functions.h"
#include "ini_functions.h"
#include "string_functions.h"
#include "ingame_text_functions.h"
#include "io_functions.h"
#include "AsmFiles.h"
#include "GameSerializer.h"
#include "path_functions.h"
#include "ui_helpers.h"
#include "imaging_helpers.h"
#include "NewLabel.h"
#include <TextEditorSettings.h>

using namespace Editorlabels;
using namespace Ini;
using namespace Ingametext;
using namespace Io;
using namespace Ui;
using namespace Imaging;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CText dialog


CText::CText(CWnd* pParent /*= nullptr */)
	: CEditorWithBackground(CText::IDD, pParent)
{
}

void CText::DoDataExchange(CDataExchange* pDX)
{
	CEditorWithBackground::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHANGEPTR, m_changeptrbutton);
	DDX_Control(pDX, IDC_NEWSLOT, m_newslotbutton);
	DDX_Control(pDX, IDC_DELETESLOT, m_deleteslotbutton);
	DDX_Control(pDX, IDC_EDITLABEL, m_editlabelbutton);
	DDX_Control(pDX, IDC_KAB, m_kab);
	DDX_Control(pDX, IDC_POINTER, m_pointer);
	DDX_Control(pDX, IDC_DTE, m_dte);
	DDX_Control(pDX, IDC_DIALOGUEBOX, m_textmultilinebox);
	DDX_Control(pDX, IDC_TEXTBOX, m_textbox);
	DDX_Control(pDX, IDC_TEXTLIST, m_textlist);
	DDX_Control(pDX, IDC_MAINLIST, m_mainlist);
	DDX_Control(pDX, IDC_TEXT_SPIN_PROLOGUE, m_spinPrologue);
	DDX_Control(pDX, IDC_TEXT_STATIC_PROLOGUE, m_staticPrologue);
	DDX_Control(pDX, IDC_TEXT_STATIC_EPILOGUE, m_staticEpilogue);
	DDX_Control(pDX, IDC_TEXT_STATIC_SCRWIDTH, m_staticScrWidth);
	DDX_Control(pDX, IDC_TEXT_STATIC_SCRLINE, m_staticScrLine);
	DDX_Control(pDX, IDC_TEXT_STATIC_PPUADDR, m_staticPpuAddr);
	DDX_Control(pDX, IDC_TEXT_EDIT_PPUADDR, m_editPpuAddr);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDINFO, m_infobutton);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
	DDX_Control(pDX, IDC_UPDATE, m_update);
	DDX_Control(pDX, IDC_SAVE, m_save);
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDCANCEL, m_cancel);
	DDX_Control(pDX, IDC_STATIC_TEXTLISTINDEX, m_listindexstatic);
	DDX_Control(pDX, IDC_TEXT_BTN_FIND, m_findbutton);
	DDX_Control(pDX, IDC_TEXT_EDIT_FIND, m_findedit);
}


BEGIN_MESSAGE_MAP(CText, CEditorWithBackground)
	ON_LBN_SELCHANGE(IDC_MAINLIST, OnSelchangeMainlist)
	ON_LBN_SELCHANGE(IDC_TEXTLIST, OnSelchangeTextlist)
	ON_BN_CLICKED(IDC_EDITLABEL, OnEditlabel)
	ON_BN_CLICKED(IDC_DTE, OnDte)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_EN_UPDATE(IDC_TEXTBOX, OnUpdateTextbox)
	ON_EN_UPDATE(IDC_POINTER, OnUpdatePointer)
	ON_BN_CLICKED(IDC_CHANGEPTR, OnChangeptr)
	ON_BN_CLICKED(IDC_DELETESLOT, OnDeleteslot)
	ON_EN_UPDATE(IDC_DIALOGUEBOX, OnUpdateTextbox)
	ON_BN_CLICKED(IDC_NEWSLOT, OnNewslot)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TEXT_SPIN_PROLOGUE, &CText::OnSpinPrologueDeltaPos)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDINFO, &CText::OnBnClickedInfo2)
//	ON_WM_KEYUP()
ON_BN_CLICKED(IDC_TEXT_BTN_FIND, &CText::OnBnClickedTextBtnFind)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CText message handlers

BOOL CText::OnInitDialog()
{
	CEditorWithBackground::OnInitDialog();

	SWITCH_OLDFFH_PTR_CHECK(Project);
	MUST_SPECIFY_PROJECT(Proj2, "Text editor");

	try {
		this->LoadOffsets();
		this->LoadRom();

		int epiloguepages = Proj2->ROM[EPILOGUELASTPAGE_OFFSET];
		CString text;
		text.Format("Epilogue Last Page: %d", epiloguepages);
		m_staticEpilogue.SetWindowText(text);

		int prologuepages = Proj2->ROM[PROLOGUELASTPAGE_OFFSET];
		m_spinPrologue.SetRange32(0, STORYTEXT_COUNT);
		m_spinPrologue.SetPos32(prologuepages);
		UpdateSpinChange(prologuepages);

		//FUTURE - once PPU support is added, CREDIT text can be added here (it stores PPU addr/coords in the first bytes).
		m_editcontexts = {
			ITEMS, WEAPONS, ARMOR, GOLD, STDMAGIC, CLASSES, ENEMYATTACKS, ENEMIES, DIALOGUE, BATTLEMESSAGES,
			INTROTEXT, SHOPTEXT, STATUSMENUTEXT, STORY
		};
		m_editcontextnames = {
			"Items","Weapons","Armor","Gold","Magic","Classes","Enemy Attacks","Enemies","Dialogue","Battle Messages",
			"Intro", "Shop", "Status Menu", "Story"
		};

		for (const auto & contextname : m_editcontextnames) m_mainlist.AddString(contextname);
		m_mainlist.SetCurSel(0);
		cur_main = -1;
		OnSelchangeMainlist();

		m_banner.Set(this, RGB(0, 0, 0), RGB(255, 32, 64), "Text");
	}
	catch (std::exception & ex) {
		return Ui::AbortFail(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortFail(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}

void CText::OnOK()
{
	//HACK - need accelarators and cusom buttons to handle things like this
	auto focus = GetFocus();
	if (focus == &m_findedit) {
		this->SendMessage(WM_COMMAND, MAKELONG(m_findbutton.GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_findbutton.GetSafeHwnd());
	}
	else {
		CEditorWithBackground::OnOK();
	}
}

void CText::LoadOffsets()
{
	ffh::fda::DataValueAccessor d(*Proj2);
	BASICTEXT_PTRADD = d.get<int>("BASICTEXT_PTRADD");
	BASICTEXT_OFFSET = d.get<int>("BASICTEXT_OFFSET");
	BASICTEXT_COUNT = d.get<int>("BASICTEXT_COUNT");
	ITEM_COUNT = d.get<int>("ITEM_COUNT");
	WEAPON_COUNT = d.get<int>("WEAPON_COUNT");
	ARMOR_COUNT = d.get<int>("ARMOR_COUNT");
	GOLDITEM_COUNT = d.get<int>("GOLDITEM_COUNT");
	MAGIC_COUNT = d.get<int>("MAGIC_COUNT");
	CLASS_COUNT = d.get<int>("CLASS_COUNT");
	ATTACKTEXT_OFFSET = d.get<int>("ATTACKTEXT_OFFSET");
	ATTACKTEXT_PTRADD = d.get<int>("ATTACKTEXT_PTRADD");
	ATTACK_COUNT = d.get<int>("ATTACK_COUNT");
	ENEMYTEXT_OFFSET = d.get<int>("ENEMYTEXT_OFFSET");
	ENEMYTEXT_PTRADD = d.get<int>("ENEMYTEXT_PTRADD");
	ENEMY_COUNT = d.get<int>("ENEMY_COUNT");
	DIALOGUE_OFFSET = d.get<int>("DIALOGUE_OFFSET");
	DIALOGUE_PTRADD = d.get<int>("DIALOGUE_PTRADD");
	DIALOGUE_COUNT = d.get<int>("DIALOGUE_COUNT");
	BATTLEMESSAGETEXT_OFFSET = d.get<int>("BATTLEMESSAGETEXT_OFFSET");
	BATTLEMESSAGETEXT_PTRADD = d.get<int>("BATTLEMESSAGETEXT_PTRADD");
	BATTLEMESSAGE_COUNT = d.get<int>("BATTLEMESSAGE_COUNT");
	BASICTEXT_START = d.get<int>("BASICTEXT_START");
	BASICTEXT_END = d.get<int>("BASICTEXT_END");
	ATTACKTEXT_START = d.get<int>("ATTACKTEXT_START");
	ATTACKTEXT_END = d.get<int>("ATTACKTEXT_END");
	ENEMYTEXT_START = d.get<int>("ENEMYTEXT_START");
	ENEMYTEXT_END = d.get<int>("ENEMYTEXT_END");
	DIALOGUE_START = d.get<int>("DIALOGUE_START");
	DIALOGUE_START = d.get<int>("DIALOGUE_START");
	DIALOGUE_END = d.get<int>("DIALOGUE_END");
	BATTLEMESSAGETEXT_START = d.get<int>("BATTLEMESSAGETEXT_START");
	BATTLEMESSAGETEXT_END = d.get<int>("BATTLEMESSAGETEXT_END");
	BATTLEMESSAGE_OFFSET = d.get<int>("BATTLEMESSAGE_OFFSET");

	NOTHINGHAPPENS_OFFSET = d.get<int>("NOTHINGHAPPENS_OFFSET");
	BANK0A_OFFSET = d.get<int>("BANK0A_OFFSET");

	INTROTEXT_OFFSET = d.get<int>("INTROTEXT_OFFSET"); // no pointer table, just a single complex string
	INTROTEXT_START = d.get<int>("INTROTEXT_START");
	INTROTEXT_END = d.get<int>("INTROTEXT_END");

	SHOPTEXTPTR_OFFSET = d.get<int>("SHOPTEXTPTR_OFFSET");
	SHOPTEXT_OFFSET = d.get<int>("SHOPTEXT_OFFSET");
	SHOPTEXT_PTRADD = d.get<int>("SHOPTEXT_PTRADD");
	SHOPTEXT_START = d.get<int>("SHOPTEXT_START");
	SHOPTEXT_END = d.get<int>("SHOPTEXT_END");
	SHOPTEXT_COUNT = d.get<int>("SHOPTEXT_COUNT");

	STATUSMENUTEXTPTR_OFFSET = d.get<int>("STATUSMENUTEXTPTR_OFFSET");
	STATUSMENUTEXT_OFFSET = d.get<int>("STATUSMENUTEXT_OFFSET");
	STATUSMENUTEXT_PTRADD = d.get<int>("STATUSMENUTEXT_PTRADD");
	STATUSMENUTEXT_START = d.get<int>("STATUSMENUTEXT_START");
	STATUSMENUTEXT_END = d.get<int>("STATUSMENUTEXT_END");
	STATUSMENUTEXT_COUNT = d.get<int>("STATUSMENUTEXT_COUNT");

	STORYTEXTPTR_OFFSET = d.get<int>("STORYTEXTPTR_OFFSET");
	STORYTEXT_OFFSET = d.get<int>("STORYTEXT_OFFSET");
	STORYTEXT_PTRADD = d.get<int>("STORYTEXT_PTRADD");
	STORYTEXT_START = d.get<int>("STORYTEXT_START");
	STORYTEXT_END = d.get<int>("STORYTEXT_END");
	STORYTEXT_COUNT = d.get<int>("STORYTEXT_COUNT");

	PROLOGUELASTPAGE_OFFSET = d.get<int>("PROLOGUELASTPAGE_OFFSET");
	EPILOGUELASTPAGE_OFFSET = d.get<int>("EPILOGUELASTPAGE_OFFSET");
}

void CText::LoadRom()
{
	Proj2->ClearROM();
	if (Proj2->IsAsm()) {
		ffh::fda::DataValueAccessor d(*Proj2);
		CREDITTEXTPTR_OFFSET = d.get<int>("CREDITTEXTPTR_OFFSET");
		CREDITTEXT_OFFSET = d.get<int>("CREDITTEXT_OFFSET");
		CREDITTEXT_PTRADD = d.get<int>("CREDITTEXT_PTRADD");
		CREDITTEXT_START = d.get<int>("CREDITTEXT_START");
		CREDITTEXT_END = d.get<int>("CREDITTEXT_END");
		CREDITTEXT_COUNT = d.get<int>("CREDITTEXT_COUNT");
	}

	// Now load the data
	if (Proj2->IsRom()) {
		Proj2->LoadROM();
	}
	else if (Proj2->IsAsm()) {
		// Instead of writing to the entire buffer, just write to the parts we need
		GameSerializer ser(*Proj2);
		ser.LoadAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.LoadAsmBin(BIN_ENEMYNAMES, ENEMYTEXT_OFFSET);
		ser.LoadAsmBin(BIN_BATTLEMESSAGES, BATTLEMESSAGETEXT_START);
		ser.LoadAsmBin(BIN_NOTHINGHAPPENS, NOTHINGHAPPENS_OFFSET);
		ser.LoadAsmBin(BIN_INTROTEXT, INTROTEXT_OFFSET);
		ser.LoadAsmBin(BIN_SHOPTEXT, SHOPTEXTPTR_OFFSET);
		ser.LoadAsmBin(BIN_STATUSMENUTEXT, STATUSMENUTEXTPTR_OFFSET);
		ser.LoadAsmBin(BIN_EPILOGUETEXT, STORYTEXT_OFFSET);
		ser.LoadInline(ASM_0D, {
			{ asmlabelval, "lut_Bridge_LastPage",{ PROLOGUELASTPAGE_OFFSET } },
			{ asmlabelval, "lut_Ending_LastPage",{ EPILOGUELASTPAGE_OFFSET } }
			});

		if (CREDITTEXTPTR_OFFSET != -1) {
			ser.LoadAsmBin(BIN_CREDITTEXT, CREDITTEXTPTR_OFFSET);
		}
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, Proj2->info.type);
	}
}

void CText::SaveRom()
{
	if (Proj2->IsRom()) {
		Proj2->SaveROM();
	}
	else if (Proj2->IsAsm()) {
		GameSerializer ser(*Proj2);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.SaveAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.SaveAsmBin(BIN_ENEMYNAMES, ENEMYTEXT_OFFSET);
		ser.SaveAsmBin(BIN_BATTLEMESSAGES, BATTLEMESSAGETEXT_START);
		ser.SaveAsmBin(BIN_NOTHINGHAPPENS, NOTHINGHAPPENS_OFFSET);
		ser.SaveAsmBin(BIN_INTROTEXT, INTROTEXT_OFFSET);
		ser.SaveAsmBin(BIN_SHOPTEXT, SHOPTEXTPTR_OFFSET);
		ser.SaveAsmBin(BIN_STATUSMENUTEXT, STATUSMENUTEXTPTR_OFFSET);
		ser.SaveAsmBin(BIN_EPILOGUETEXT, STORYTEXT_OFFSET);
		ser.SaveInline(ASM_0D, {
			{ asmlabelval, "lut_Bridge_LastPage",{ PROLOGUELASTPAGE_OFFSET } },
			{ asmlabelval, "lut_Ending_LastPage",{ EPILOGUELASTPAGE_OFFSET } }
			});

		if (CREDITTEXTPTR_OFFSET != -1) {
			ser.SaveAsmBin(BIN_CREDITTEXT, CREDITTEXTPTR_OFFSET);
		}
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, Proj2->info.type);
	}
}

void CText::LoadValues()
{
	CEdit* m_field = &m_textbox;
	bool showdialogue = CanShowDialogueEdit(cur_main);
	if (showdialogue) m_field = &m_textmultilinebox;

	CRect rcborder;
	WindowToParentRect(&m_textmultilinebox, &rcborder);
	rcborder.InflateRect(2, 2);
	InvalidateRect(&rcborder);

	CString text, temptext;
	int offset;
	if (cur_main == INTROTEXT) {
		// Not using pointers or text list, just a single complex string
		offset = INTROTEXT_OFFSET;
	}
	else {
		offset = ptroffset + (cur_text << 1);
		offset = Proj2->ROM[offset] + (Proj2->ROM[offset + 1] << 8) + ptradd;
	}
	text.Format("%X", offset);
	m_pointer.SetWindowText(text);

	text = "";
	temptext = "";
	for (; Proj2->ROM[offset]; offset++) {
		temptext = Proj2->tables[viewDTE][Proj2->ROM[offset]].c_str();
		if (!temptext.GetLength()) {
			if (Proj2->ROM[offset] < 0x10) temptext.Format("{0%X}", Proj2->ROM[offset]);
			else temptext.Format("{%X}", Proj2->ROM[offset]);
		}
		text += temptext;
	}

	// Expand the special case DTE newline characters
	text.Replace("\\n", "\r\n");     // "\\n" -> single NL
	text.Replace("\\2", "\r\n\r\n"); // "\\2" -> double NL

	m_field->SetWindowText(text);

	if (!showdialogue) {
		CString text2;
		m_textlist.GetText(cur_text, text2);
		if (text2 != text)
			UpdateTextList();
	}

	int tempcount = count;
	switch (cur_main) {
	case 6: textstart = ATTACKTEXT_START; textend = ATTACKTEXT_END; ptrtblstart = ATTACKTEXT_OFFSET; break;
	case 7: textstart = ENEMYTEXT_START; textend = ENEMYTEXT_END; ptrtblstart = ENEMYTEXT_OFFSET; break;
	case 8: textstart = DIALOGUE_START; textend = DIALOGUE_END; ptrtblstart = DIALOGUE_OFFSET; break;
	case 9: textstart = BATTLEMESSAGETEXT_START; textend = BATTLEMESSAGETEXT_END; ptrtblstart = BATTLEMESSAGE_OFFSET; break;
	case INTROTEXT: textstart = INTROTEXT_START; textend = INTROTEXT_END; ptrtblstart = 0; break;
	case SHOPTEXT: textstart = SHOPTEXT_START; textend = SHOPTEXT_END; ptrtblstart = SHOPTEXTPTR_OFFSET; break;
	case STATUSMENUTEXT: textstart = STATUSMENUTEXT_START; textend = STATUSMENUTEXT_END; ptrtblstart = STATUSMENUTEXTPTR_OFFSET; break;
	case STORY: textstart = STORYTEXT_START; textend = STORYTEXT_END; ptrtblstart = STORYTEXTPTR_OFFSET; break;
	case CREDIT: textstart = CREDITTEXT_START; textend = CREDITTEXT_END; ptrtblstart = CREDITTEXTPTR_OFFSET; break;
	default: textstart = BASICTEXT_START; textend = BASICTEXT_END; ptrtblstart = BASICTEXT_OFFSET; tempcount = BASICTEXT_COUNT; break;
	}

	ptrtblend = ptrtblstart + (tempcount << 1);

	if (cur_main == STORY) {
		int prologuepages = Proj2->ROM[PROLOGUELASTPAGE_OFFSET];
		UpdateSpinChange(prologuepages);
	}
	else if (cur_main == CREDIT) {
		// Read the PPU address for the credit text
		//FUTURE - CREDIT text is not currently used
		int credoffset = GetOffset(CREDITTEXTPTR_OFFSET, CREDITTEXTPTR_OFFSET, cur_text);
		int addr = Proj2->ROM[credoffset] + (Proj2->ROM[credoffset + 1] << 8);
		CString csaddr;
		csaddr.Format("%04X", addr);
		m_editPpuAddr.SetWindowText(csaddr);
	}

	FindKAB();

	// Only active when user types in the pointer field.
	m_changeptrbutton.EnableWindow(0);

	// Don't use pointer controls for the intro, since it has no pointer table.
	BOOL useptrcontrols = (cur_main != INTROTEXT);
	m_newslotbutton.EnableWindow(useptrcontrols && (kab > 0));
	m_pointer.EnableWindow(useptrcontrols);
	m_editlabelbutton.EnableWindow(useptrcontrols);
	m_deleteslotbutton.EnableWindow(useptrcontrols);
	m_textlist.ShowWindow(useptrcontrols ? SW_SHOW : SW_HIDE);

	// Invalidate the border around the textlist
	WindowToParentRect(&m_textlist, &rcborder);
	rcborder.InflateRect(2, 2);
	InvalidateRect(&rcborder);

	// Don't show story controls if not editing story text.
	int showstorycontols = (cur_main == STORY) ? SW_SHOW : SW_HIDE;
	m_spinPrologue.ShowWindow(showstorycontols);
	m_staticPrologue.ShowWindow(showstorycontols);
	m_staticEpilogue.ShowWindow(showstorycontols);

	// Show the screen width guide if using the dialogue edit text box.
	m_staticScrWidth.ShowWindow(showdialogue ? SW_SHOW : SW_HIDE);
	m_staticScrLine.ShowWindow(showdialogue ? SW_SHOW : SW_HIDE);

	// Credit text can't use DTE
	m_dte.ShowWindow(cur_main != CREDIT ? SW_SHOW : SW_HIDE);

	// Don't show PPU controls if not editing credits
	m_staticPpuAddr.ShowWindow(cur_main == CREDIT ? SW_SHOW : SW_HIDE);
	m_editPpuAddr.ShowWindow(cur_main == CREDIT ? SW_SHOW : SW_HIDE);

	madechange = 0;

	if (showdialogue) InvalidateThinBorder(m_textmultilinebox);
	if (useptrcontrols) InvalidateThinBorder(m_textlist);

	LogHere << "LoadValues: "
		<< " cur_main " << cur_main << " textstart " << textstart << " textend " << textend 
		<< " ptrtblstart " << ptrtblstart << " tempcount " << tempcount << " ptrtblend " << ptrtblend;

	//REMOVE
	//TRACE(__FUNCTION__
	//	"\ncur_main %d\ntextstart %05x\ntextend %05x\nptrtblstart %05x\ntempcount %d\nptrtblend %05x\n"
	//	"\n",
	//	cur_main, textstart, textend, ptrtblstart, tempcount, ptrtblend
	//);
}

void CText::StoreValues()
{
	if (cur_main == STORY) {
		// The prologue last page value is saved regardless of whether or not the text can be saved.
		int prologuepages = m_spinPrologue.GetPos32();
		Proj2->ROM[PROLOGUELASTPAGE_OFFSET] = (BYTE)(prologuepages & 0xFF);
	}
	else if (cur_main == CREDIT) {
		// Write the PPU address for the credit text
		CString csaddr;
		m_editPpuAddr.GetWindowText(csaddr);
		int addr = (unsigned short)strtoul(csaddr, nullptr, 16);
		int offset = GetOffset(CREDITTEXTPTR_OFFSET, CREDITTEXTPTR_OFFSET, cur_text);
		Proj2->ROM[offset] = addr & 0xFF;
		Proj2->ROM[offset + 1] = (addr >> 8) & 0xFF;
	}

	//Store values is kind of tricky.. first let's check to see if the text is in the *normal*
	//  text block.  If it isn't... warn the user.
	//  If not... compare the new length with the old length.. make sure we have room, and shift
	//  everything over. (very similar to map data)

	//Step 1:  find: the length of the text (in bytes)...
	//               the offset of the text
	//               the Hex values of the text

	int newlength, oldlength;
	int temp;
	int offset;
	int ptrstart, ptrend;

	if (cur_main == INTROTEXT) {
		offset = textstart;
		ptrstart = ptrend = 0;
	}
	else {
		ptrstart = ptroffset;
		ptrend = ptrstart + (count << 1);
		if (cur_main < 6) {
			ptrstart = BASICTEXT_OFFSET;
			ptrend = ptrstart + (BASICTEXT_COUNT << 1);
		}

		offset = ptroffset + (cur_text << 1);
		offset = Proj2->ROM[offset] + (Proj2->ROM[offset + 1] << 8) + ptradd;
	}

	int co;
	for (co = offset; Proj2->ROM[co]; co++);
	oldlength = co - offset + 1;		//add 1 to include the 00

	bool showdialogue = CanShowDialogueEdit(cur_main);
	CString text;
	if (showdialogue) m_textmultilinebox.GetWindowText(text);
	else m_textbox.GetWindowText(text);

	// Pad the buffer to be twice as long as the input string (that much space won't likely be needed)
	std::vector<BYTE> vbuffer(text.GetLength() * 2, 0);
	BYTE * buffer = address(vbuffer);
	newlength = ConvertText(buffer, text);

	int dif = newlength - oldlength;
	bool normaltext = true;

	//Step 2:  check to see if this is *normally* stored text.  If it is... make sure we have room
	//		   in the kab.. if not.. give an error message and change m_kab (*NOT* the int kab).
	//         if the text is not *normal*, then compare the new size to the old:
	//           if we're making text smaller (or not changing the size) don't worry about it
	//           if we're making it bigger... alert the user and only proceed if they confirm

	if (offset >= textstart && offset < textend) {	//normal text
		if (kab - dif < 0) {
			AfxMessageBox("Not enough free space in ROM.", MB_ICONERROR);
			text.Format("%d", kab - dif);
			m_kab.SetWindowText(text);
			return;
		}
	}
	else {											//non-normal text
		normaltext = false;
		if (dif > 0) {
			if (AfxMessageBox("This text is outside of the standard text block.\n"
				"Increasing its size may write over other information.\nAre you sure you want to continue?",
				MB_YESNO | MB_ICONQUESTION) == IDNO)
				return;
		}
	}


	//Step 3:  if this is normal text... move all the text around to fit the new text

	if (cur_main != INTROTEXT) {
		if (normaltext && dif) {
			if (dif > 0) {	//shift to the right
				for (co = textend - 1; co > offset; co--)
					Proj2->ROM[co] = Proj2->ROM[co - dif];
			}
			if (dif < 0) {	//shift to the left
				for (co = offset; co < textend; co++)
					Proj2->ROM[co] = Proj2->ROM[co - dif];
			}
			for (co = ptrstart; co < ptrend; co += 2) {
				temp = Proj2->ROM[co] + (Proj2->ROM[co + 1] << 8) + ptradd;
				if ((temp < textend) && (temp > offset)) {
					temp += dif - ptradd;
					Proj2->ROM[co] = temp & 0xFF;
					Proj2->ROM[co + 1] = (BYTE)(temp >> 8);
				}
			}
		}
	}

	//Step 4:  insert the new text!

	for (co = 0; co < newlength; co++)
		Proj2->ROM[co + offset] = buffer[co];

	madechange = 0;
	m_changeptrbutton.EnableWindow(0);

	TRACE(__FUNCTION__
		"\ncur_main %d\ntextstart %05x\ntextend %05x\nptrtblstart %05x\nptrtblend %05x\n"
		"offset %05x\nptrstart %05x\nptrend %05x\ndif %d\nptradd %05x\n"
		"\n",
		cur_main, textstart, textend, ptrtblstart, ptrtblend,
		offset, ptrstart, ptrend, dif, ptradd
	);
}

CString CText::GetInfoText()
{
	return "In the text list:\n"
		"- single newlines {05} are shown as \\n;\n"
		"- double-newlines {01} are shown as \\2.\n\n"
		"In the text edit box:\n"
		"- with DTE on, adjacent single newline pairs will always save as double-newlines.\n"
		"- to save consecutive single newline characters, you'll have to turn DTE off.\n"
		"  e.g. three single newlines will save as \\2\\n (double newline followed by newline);\n"
		"       four single newlines will save as \\2\\2 (consecutive double newlines).";
}

bool CText::ShowCurrentTextItemIndex() const
{
	return (cur_main != -1) && (cur_main != INTROTEXT);
}

void CText::OnSelchangeMainlist()
{
	if (cur_main != -1 && madechange)
		StoreValues();

	cur_main = m_editcontexts[m_mainlist.GetCurSel()];
	viewDTE = Proj2->session.textViewInDTE[cur_main];

	auto showdialogue = CanShowDialogueEdit(cur_main);
	m_textbox.ShowWindow(!showdialogue ? SW_SHOW : SW_HIDE);
	m_editlabelbutton.ShowWindow(cur_main == 8);
	m_textmultilinebox.ShowWindow(showdialogue);
	m_dte.SetCheck(viewDTE);

	ResetTextList();

	cur_text = -1;
	m_textlist.SetCurSel(0);
	OnSelchangeTextlist();
}

void CText::ResetTextList()
{
	ptradd = BASICTEXT_PTRADD;
	ptroffset = BASICTEXT_OFFSET;
	count = ITEM_COUNT;
	if (cur_main < 6) {
		if (cur_main > 0) { ptroffset += count << 1; count = WEAPON_COUNT; }
		if (cur_main > 1) { ptroffset += count << 1; count = ARMOR_COUNT; }
		if (cur_main > 2) { ptroffset += count << 1; count = GOLDITEM_COUNT; }
		if (cur_main > 3) { ptroffset += count << 1; count = MAGIC_COUNT; }
		if (cur_main > 4) { ptroffset += count << 1; count = CLASS_COUNT; }
	}
	else if (cur_main == 6) {
		ptroffset = ATTACKTEXT_OFFSET;
		ptradd = ATTACKTEXT_PTRADD;
		count = ATTACK_COUNT;
	}
	else if (cur_main == 7) {
		ptroffset = ENEMYTEXT_OFFSET;
		ptradd = ENEMYTEXT_PTRADD;
		count = ENEMY_COUNT;
	}
	else if (cur_main == 8) {
		ptroffset = DIALOGUE_OFFSET;
		ptradd = DIALOGUE_PTRADD;
		count = DIALOGUE_COUNT;
	}
	else if (cur_main == 9) {
		ptroffset = BATTLEMESSAGETEXT_OFFSET;
		ptradd = BATTLEMESSAGETEXT_PTRADD;
		count = BATTLEMESSAGE_COUNT;
	}
	else if (cur_main == INTROTEXT) {
		ptroffset = INTROTEXT_OFFSET;
		ptradd = 0;
		count = 1;
	}
	else if (cur_main == SHOPTEXT) {
		ptroffset = SHOPTEXTPTR_OFFSET;
		ptradd = SHOPTEXT_PTRADD;
		count = SHOPTEXT_COUNT;
	}
	else if (cur_main == STATUSMENUTEXT) {
		ptroffset = STATUSMENUTEXTPTR_OFFSET;
		ptradd = STATUSMENUTEXT_PTRADD;
		count = STATUSMENUTEXT_COUNT;
	}
	else if (cur_main == STORY) {
		ptroffset = STORYTEXTPTR_OFFSET;
		ptradd = STORYTEXT_PTRADD;
		count = STORYTEXT_COUNT;
	}
	else if (cur_main == CREDIT) {
		ptroffset = CREDITTEXTPTR_OFFSET;
		ptradd = CREDITTEXT_PTRADD;
		count = CREDITTEXT_COUNT;
	}

	Ui::RedrawScope redraw(&m_textlist);
	//while (m_textlist.GetCount()) m_textlist.DeleteString(0);
	m_textlist.ResetContent();

	switch (cur_main) {
	case 8:
	{
		TextEditorSettings stgs(*Proj2);
		for (int co = 0; co < DIALOGUE_COUNT; co++) {
			CString str = stgs.ShowActualText ? Ingametext::LoadDialogueEntry(*Proj2, co, true).name : LoadTextLabel(*Proj2, co).name;
			m_textlist.InsertString(co, str);
		}
		break;
	}
	case CREDIT:
		//FUTURE - credit text either needs a separate editor, or this one needs to track PPUADDR and page number per string.
		//	Not terribly not hard, but this dialog is already too complex for its own good.
		//	Not sure if/how this is affected by the NASIR CRC that's tied to the credit text handler, so skip this for now.
		//PutCreditTextToList(Proj2, ptroffset, ptradd, count, &m_textlist, nullptr);
		break;
	default:
		PutHexToList(Proj2, ptroffset, ptradd, count, viewDTE, &m_textlist, nullptr);
		break;
	}
}

void CText::FindKAB()
{
	int co;
	if (cur_main == INTROTEXT) {
		// Introtext doesn't use pointers, it's jsut a single complex string.
		// In this case, the start and end are offsets to the actual text and
		// are used in the loop directly.
		for (co = textstart; Proj2->ROM[co] && co < textend; co++);
	}
	else if (cur_main == CREDIT)
	{
		//FUTURE -  Uses a PPU-address-prefix simple string (no DTE or control codes).
		co = textend + 1;
	}
	else
	{
		int ptrset = BASICTEXT_OFFSET;
		int ptrcount = count << 1;
		if (cur_main <= 5) ptrcount = BASICTEXT_COUNT << 1;
		if (cur_main == 6) ptrset = ATTACKTEXT_OFFSET;
		if (cur_main == 7) ptrset = ENEMYTEXT_OFFSET;
		if (cur_main == 8) ptrset = DIALOGUE_OFFSET;
		if (cur_main == 9) ptrset = BATTLEMESSAGETEXT_OFFSET;
		if (cur_main == SHOPTEXT) ptrset = SHOPTEXTPTR_OFFSET;
		if (cur_main == STATUSMENUTEXT) ptrset = STATUSMENUTEXTPTR_OFFSET;
		if (cur_main == STORY) ptrset = STORYTEXTPTR_OFFSET;

		int largestpointer = textstart;
		int thispointer;
		ptrcount += ptrset;

		for (co = ptrset; co < ptrcount; co += 2) {
			thispointer = Proj2->ROM[co] + (Proj2->ROM[co + 1] << 8) + ptradd;
			if (thispointer < textend && thispointer > largestpointer) largestpointer = thispointer;
		}

		for (co = largestpointer; Proj2->ROM[co]; co++);
	}

	kab = textend - co - 1;
	CString text;
	text.Format("%d", kab);
	m_kab.SetWindowText(text);
}

int CText::ConvertText(BYTE* buffer, CString text)
{
	int bytelength = 0;
	CString temp1;
	CString temp2;
	int co;

	// Normalize the text to prepare for STD/DTE conversion
	text.Replace("\r\n", "\n");  // CRLF -> NL
	text.Replace("\n\n", "\\2"); // double NL -> "\\2"
	text.Replace("\n", "\\n");   // single NL -> "\\n"

	int length = text.GetLength();

	while (length > 0) {
		temp1 = text.Left(1);
		temp2 = text.Left(2);
		if (temp1 == "{") {
			co = text.Find("}");
			if (co == -1) co = text.GetLength();
			temp2 = text.Left(co);
			buffer[bytelength] = StringToInt_HEX(temp2) & 0xFF;
			length -= co;
			bytelength += 1;
		}
		else {
			for (co = 0; co < 256; co++) {
				if (temp2 == Proj2->tables[viewDTE][co].c_str()) break;
			}
			if (co != 256) {
				buffer[bytelength] = (BYTE)co;
				bytelength += 1; length -= 1;
			}
			else {
				for (co = 0; co < 256; co++) {
					if (temp1 == Proj2->tables[viewDTE][co].c_str()) break;
				}
				if (co != 256) {
					buffer[bytelength] = (BYTE)co; bytelength += 1;
				}
			}
			length -= 1;
		}

		text = text.Right(length);
	}

	buffer[bytelength] = 0;
	return bytelength + 1;
}

void CText::UpdateSpinChange(int newpos)
{
	CString text;
	text.Format("Prologue Last Page: %d", newpos);
	m_staticPrologue.SetWindowText(text);
}

bool CText::CanShowDialogueEdit(int context)
{
	switch (context) {
	case 8:
	case INTROTEXT:
	case SHOPTEXT:
	case STATUSMENUTEXT:
	case STORY:
	case CREDIT:
		return true;
	}
	return false;
}

int CText::GetOffset(int theptroffset, int theptradd, int index)
{
	// say theptroffset = 35010, ptradd = 2C010, index = 3
	// 35010 + (3 * 2) = 35016
	// ROM[35016] = 1A
	// ROM[35017] = B0
	// 2C010 + B01A = 3702A
	// ROM[3702A] = first byte of a complex string, charclass data, etc.
	// ...
	int base = theptroffset + (index << 1);
	int offset = Proj2->ROM[base] + (Proj2->ROM[base + 1] << 8) + theptradd;
	return offset;
}

void CText::OnSelchangeTextlist()
{
	if (cur_text != -1 && madechange) {
		StoreValues();
		if (cur_main != 8)
			UpdateTextList();
	}

	cur_text = m_textlist.GetCurSel();

	m_listindexstatic.ShowWindow(ShowCurrentTextItemIndex() ? SW_SHOW : SW_HIDE);
	if (cur_main == -1 || cur_text == -1) {
		m_listindexstatic.SetWindowText("No selection");
	}
	else {
		CString cstextindex, csmain;
		m_mainlist.GetText(cur_main, csmain);
		cstextindex.Format("%s # %d", csmain, cur_text + 1);
		m_listindexstatic.SetWindowText(cstextindex);
	}

	LoadValues();
}

void CText::UpdateTextList()
{
	int temp = m_textlist.GetCurSel();
	if (cur_main == 8) return;

	Ui::RedrawScope redraw(&m_textlist);
	CString text = "";
	CString temptext;
	int offset = ptroffset + (cur_text << 1);
	offset = Proj2->ROM[offset] + (Proj2->ROM[offset + 1] << 8) + ptradd;
	for (int co = offset; Proj2->ROM[co]; co++) {
		temptext = Proj2->tables[viewDTE][Proj2->ROM[co]].c_str();
		if (temptext == "") {
			temptext.Format("{%02X}", Proj2->ROM[co]);
		}
		text += temptext;
	}

	m_textlist.DeleteString(cur_text);
	m_textlist.InsertString(cur_text, text);
	m_textlist.SetCurSel(temp);
}

void CText::OnEditlabel()
{
	//TODO - implement
	//ChangeLabel(*Proj2, -1, LoadTextLabel(*Proj2, cur_text), WriteTextLabel, cur_text, &m_textlist, nullptr);
}

void CText::OnDte()
{
	viewDTE = 0;
	if (m_dte.GetCheck()) viewDTE = 1;
	Proj2->session.textViewInDTE[cur_main] = viewDTE;
	if (cur_main != 8) ResetTextList();
	m_textlist.SetCurSel(cur_text);
}

void CText::OnUpdate()
{
	StoreValues();
	FindKAB();
	UpdateTextList();
}

void CText::OnUpdateTextbox()
{
	madechange = 1;
}

void CText::OnUpdatePointer()
{
	m_changeptrbutton.EnableWindow(1);
}

void CText::OnChangeptr()
{
	//Make sure the pointer is in bounds
	CString text;
	m_pointer.GetWindowText(text);
	int pointer = StringToInt_HEX(text);

	if (pointer < ptradd || pointer >(ptradd + 0xFFFF)) {
		text.Format("Pointer is out of range.\nMust be between %X - %X", ptradd, ptradd + 0xFFFF);
		AfxMessageBox(text, MB_ICONERROR);
		LoadValues();
		return;
	}

	int offset = ptroffset + (cur_text << 1);
	pointer -= ptradd;
	Proj2->ROM[offset] = pointer & 0xFF;
	Proj2->ROM[offset + 1] = (BYTE)(pointer >> 8);
	UpdateTextList();
	LoadValues();
}

void CText::OnDeleteslot()
{
	int offset = ptroffset + (cur_text << 1);
	offset = Proj2->ROM[offset] + (Proj2->ROM[offset + 1] << 8) + ptradd;
	if (offset < textstart || offset > textend) {
		AfxMessageBox("The slot is not in the standard text boundaries.\nCannot remove slot.", MB_ICONERROR);
		return;
	}

	int largestptr = textstart;
	int thisptr;
	int co;

	for (co = ptrtblstart; co < ptrtblend; co += 2) {
		thisptr = Proj2->ROM[co] + (Proj2->ROM[co + 1] << 8) + ptradd;
		if (thisptr > largestptr && thisptr < textend)
			largestptr = thisptr;
	}

	if (largestptr == offset) {
		AfxMessageBox("This is the last slot.\nCannot delete the last slot.", MB_ICONERROR);
		return;
	}

	if (AfxMessageBox("Really remove current slot?\nThis text will be deleted.", MB_YESNO | MB_ICONQUESTION) == IDNO) return;

	int currentlength;
	for (currentlength = offset; Proj2->ROM[currentlength]; currentlength += 1);
	currentlength -= offset;
	currentlength += 1;


	int temp;
	int ptrstart, ptrend;
	ptrstart = ptroffset;
	ptrend = ptrstart + (count << 1);
	if (cur_main < 6) {
		ptrstart = BASICTEXT_OFFSET;
		ptrend = ptrstart + (BASICTEXT_COUNT << 1);
	}

	//adjust pointers
	for (co = ptrstart; co < ptrend; co += 2) {
		temp = Proj2->ROM[co] + (Proj2->ROM[co + 1] << 8) + ptradd;
		if (temp > offset && temp < textend) {
			temp -= currentlength;
			temp -= ptradd;
			Proj2->ROM[co] = temp & 0xFF;
			Proj2->ROM[co + 1] = (BYTE)(temp >> 8);
		}
	}
	//shift all text
	for (co = offset; co < textend; co++)
		Proj2->ROM[co] = Proj2->ROM[co + currentlength];

	UpdateTextList();
	LoadValues();
}

void CText::OnNewslot()
{
	if (kab < 1) {
		AfxMessageBox("Not enough space in ROM.\nYou must have at least 1 byte free", MB_ICONERROR);
		return;
	}

	CString text;
	int slot = textend - kab;
	text.Format("%X", slot);
	m_pointer.SetWindowText(text);
	OnChangeptr();
}

void CText::OnSpinPrologueDeltaPos(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pud = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	CString text;
	int newpos = pud->iPos + pud->iDelta;
	int lo = -1, hi = -1;
	m_spinPrologue.GetRange32(lo, hi);
	if (lo == hi && lo == -1) {
		AfxMessageBox("Range wasn't set.", MB_ICONERROR);
	}
	else if (newpos < lo || newpos > hi) {
		//REFACTOR - the user gets the idea when the range stops. Does this dialog box disrupt user flow too much to be useful?
		//text.Format("Value %d is outside of the range [%d,%d].", newpos, lo, hi);
		//AfxMessageBox(text, MB_ICONERROR);
	}
	else {
		UpdateSpinChange(newpos);
	}
	*pResult = 0;
}

void CText::OnPaint()
{
	CPaintDC dc(this);
	m_banner.Render(dc, 8, 8);
}

void CText::OnBnClickedInfo2()
{
	AfxMessageBox(GetInfoText(), MB_ICONINFORMATION);
}

void CText::OnBnClickedTextBtnFind()
{
	auto term = Ui::GetControlText(m_findedit).MakeLower();
	if (!term.IsEmpty()) {
		switch (cur_main) {
		case INTROTEXT:
		{
			auto text = Ui::GetControlText(m_textmultilinebox).MakeLower();
			int start = -1, end = -1;
			m_textmultilinebox.GetSel(start, end);

			auto loop = 2; // go through at most twice
			do {
				--loop;
				// If start == end, then it's the both are the caret position.
				// Else, end is the first character that isn't selected.
				auto mark = (start == end) ? start : start + 1;
				auto found = text.Find(term, mark);
				if (found != -1) {
					m_textmultilinebox.SetSel(found, found + term.GetLength(), FALSE);
					break;
				}
				// reset the caret position since we're looping again
				start = end = 0;
			} while (loop);
			break;
		}

		default:
		{
			// Start from the current selection or 0
			auto start = m_textlist.GetCurSel();
			if (start == LB_ERR) start = 0;

			m_textlist.SetRedraw(FALSE);
			auto loop = 2; // go through at most twice
			do {
				--loop;
				for (auto icount = m_textlist.GetCount(), next = start + 1; next < icount; ++next) {
					CString text;
					m_textlist.GetText(next, text);
					if (text.MakeLower().Find(term) != -1) {
						m_textlist.SetCurSel(next);
						OnSelchangeTextlist();
						loop = 0; // found it, set to 0 to end the outer do loop.
						break;
					}
				}
				start = 0; // reset in case we need to loop back to the beginning
			} while (loop);
			m_textlist.SetRedraw(TRUE);
			m_textlist.Invalidate();
			break;
		}
		}
	}
}