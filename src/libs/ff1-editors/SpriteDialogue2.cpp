// SpriteDialogue2.cpp : implementation file
//

#include "stdafx.h"
#include "SpriteDialogue2.h"
#include "afxdialogex.h"
#include "editors_common.h"

#include <EntriesLoader.h>
#include <FFHacksterProject.h>
#include <GameSerializer.h>
#include <WaitingDlg.h>

#include <AsmFiles.h>
#include <dialogue_helpers.h>
#include <editor_label_functions.h>
#include <general_functions.h>
#include <imaging_helpers.h>
#include <ini_functions.h>
#include <io_functions.h>
#include <map_types.h>
#include <path_functions.h>
#include <string_functions.h>
#include <type_support.h>
#include <ui_helpers.h>

#include <FFHacksterProject.h>
#include <SpriteDialogueSettings.h>
#include <SpriteDialogueSettingsDlg.h>
#include <iterator>

using namespace Editors;
using namespace Editorlabels;
using namespace Imaging;
using namespace Ini;
using namespace Io;
using namespace Ramvalues;
using namespace Strings;
using namespace Types;
using namespace Ui;

#define IGNOREDITEM -1

namespace SpriteDialogue_Helpers // DECLARATION
{
	enum class asm_search { nomatch, match, prefix, inimatch };

	mfcstringvector read_names_vector(CFFHacksterProject & proj, mfcstringvector keynames);
	std::string search_for_match(CFFHacksterProject & proj, asm_search searchtype, std::string key, int index);

	stdstringvector GetIniTalkRoutineNames(CFFHacksterProject & proj);

}
using namespace SpriteDialogue_Helpers;


// CSpriteDialogue2 class

IMPLEMENT_DYNAMIC(CSpriteDialogue2, CEditorWithBackground)

CSpriteDialogue2::CSpriteDialogue2(CFFHacksterProject & project, CWnd* pParent /*=NULL*/)
	: CEditorWithBackground(IDD_SPRITE_DIALOGUE2, pParent)
	, m_proj(project)
	, m_dialoguepathrestorer(project.DialoguePath)
	, m_curindex(-1)
{
}

CSpriteDialogue2::~CSpriteDialogue2()
{
}

BOOL CSpriteDialogue2::OnInitDialog()
{
	CEditorWithBackground::OnInitDialog();

	if (!m_proj.IsRom() && !m_proj.IsAsm())
		return AbortInitDialog(this, "This editor doesn't support '" + m_proj.ProjectTypeName + "'.");	
	if (Enloader == nullptr)
		return AbortInitDialog(this, "No entry loader was specified for this editor");

	try {
		CWaitCursor wait;
		LoadListBox(m_mainlist, LoadSpriteLabels(m_proj, true));
		this->LoadOffsets();
		this->LoadRom();
		SetupLayout();
		FormatElementList();
		m_mainlist.SetCurSel(0);
		OnSelchangeList();
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

void CSpriteDialogue2::LoadOffsets()
{
	TALKROUTINEPTRTABLE_OFFSET = ReadHex(m_proj.ValuesPath, "TALKROUTINEPTRTABLE_OFFSET");
	TALKROUTINEPTRTABLE_BYTES = ReadDec(m_proj.ValuesPath, "TALKROUTINEPTRTABLE_BYTES");
	TALKROUTINEPTRTABLE_PTRADD = ReadHex(m_proj.ValuesPath, "TALKROUTINEPTRTABLE_PTRADD");
	TALKROUTINEDATA_OFFSET = ReadHex(m_proj.ValuesPath, "TALKROUTINEDATA_OFFSET");
	TALKROUTINEDATA_BYTES = ReadDec(m_proj.ValuesPath, "TALKROUTINEDATA_BYTES");
	BANK0A_OFFSET = ReadHex(m_proj.ValuesPath, "BANK0A_OFFSET");
}

void CSpriteDialogue2::LoadRom()
{
	m_proj.ClearROM();

	// Build a reference map for fanfare values up front. For any missing value, warn and lean on FF1 defaults.
	auto AddFanfareValue = [this](CString key, int usabilityvalue, int hexdefault) {
		auto value = hex(ReadIni(m_proj.ValuesPath, key, "value", "0x0"));
		if (value != 0)
			m_fanfarevalues[value] = usabilityvalue;
		else {
			AfxMessageBox("Can't find the fanfare value " + key + ": falling back to the FF1 default value.");
			m_fanfarevalues[hexdefault] = usabilityvalue;
		}
	};
	AddFanfareValue("talkfanfare", ASM_TRUE, 0x7DE6);
	AddFanfareValue("notalksnd", ASM_FALSE, 0xEAEA);

	if (m_proj.IsRom()) {
		load_binary(m_proj.WorkRomPath, m_proj.ROM);

		m_itemsaddr = ReadRamAddress(m_proj, "items");
		m_unsramaddr = ReadRamAddress(m_proj, "unsram");

		LoadRomTalkData();
	}
	else if (m_proj.IsAsm()) {
		CWaitingDlg waiting(this);
		waiting.Init();
		waiting.SetMessage("Loading sprite dialogue data...");
		GameSerializer ser(m_proj, &waiting);
		ser.LoadAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.LoadAsmBin(BIN_OBJECTDATA, TALKROUTINEDATA_OFFSET);

		m_itemsaddr = findvalue(m_proj.m_varmap, std::string("items"));
		m_unsramaddr = findvalue(m_proj.m_varmap, std::string("unsram"));

		LoadAsmTalkData(ser);
	}
}

void CSpriteDialogue2::SaveRom()
{
	m_dialoguepathrestorer.Save();

	if (m_proj.IsRom()) {
		SaveRomTalkData();
		save_binary(m_proj.WorkRomPath, m_proj.ROM);
	}
	else if (m_proj.IsAsm()) {
		CWaitingDlg waiting(this);
		waiting.Init();
		waiting.SetMessage("Saving sprite dialogue data...");
		GameSerializer ser(m_proj, &waiting);

		const auto & prenames = m_asmroutinenames;
		const auto & premap = m_asmroutinemap;
		SaveAsmTalkData(ser);

#ifdef TEST_SAVE
		return; //TESTING - remove when done testing
#endif

		ser.SaveAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.SaveAsmBin(BIN_OBJECTDATA, TALKROUTINEDATA_OFFSET);
	}
}

void CSpriteDialogue2::LoadRomTalkData()
{
	m_comboRoutineType.ResetContent();
	m_routineaddrmap.clear();

	// Load the names from the .dialogue file into the routine combo.
	auto sectionnames = ReadIniSectionNames(m_proj.DialoguePath);
	sectionnames.erase(std::remove_if(begin(sectionnames), end(sectionnames), [](const auto& n) { return _strnicmp(n, "talk", 4) != 0; }), end(sectionnames));

	// Load the routine address map
	for (const auto & name : sectionnames) {
		int bankaddr = hex(ReadIni(m_proj.DialoguePath, name, "bankaddr", "0x0"));
		if (!has(m_routineaddrmap, name)) {
			Ui::AddEntry(m_comboRoutineType, name, bankaddr);
			m_routineaddrmap[name] = bankaddr;
		}
	}

	EnaureSynchedHandlerTables();
	EnsureValidSpriteHandlers();
}

void CSpriteDialogue2::SaveRomTalkData()
{
	// nothing to do here for now
}

void CSpriteDialogue2::LoadAsmTalkData(GameSerializer & ser)
{
	m_comboRoutineType.ResetContent();
	m_routineaddrmap.clear();

	std::string label = (LPCSTR)ReadIni(m_proj.DialoguePath, "Label_TalkJumpTable", "value", "");
	auto asmfile = Paths::Combine({ m_proj.ProjectFolder, "asm", ASM_0E });
	ser.LoadDialogue(asmfile, label, m_asmroutinenames, m_asmroutinemap);

	// Load the routine address map.
	// Also load the hardcoded elements from the assembly source into the working ROM.
	stdstringset srcset;
	for (const auto & kv : m_asmroutinemap) {
		const std::string & routine = kv.first;
		const talkelementvector & lines = kv.second;
		CString csroutinename = routine.c_str();

		srcset.insert(routine);

		auto bankaddr = hex(ReadIni(m_proj.DialoguePath, csroutinename, "bankaddr", "0x0"));
		if (bankaddr == 0)
			continue; // this routine is defined in source, but isn't known to the editor ... can't edit it.

		if (!has(m_routineaddrmap, csroutinename)) {
			Ui::AddEntry(m_comboRoutineType, csroutinename, bankaddr);
			m_routineaddrmap[csroutinename] = bankaddr;
		}

		for (auto & line : lines) {
			auto delem = dialogue_helpers::ReadElement(m_proj, csroutinename, line.element.c_str());
			const auto & marker = delem.marker;
			if (marker.Find("hc") == 0) {
				//N.B. - the Write calls below are writing assembly file data to the working ROM buffer
				//		This allows the FFHEx editor to manipulate values from the assembly file.
				//		This is done using markers, which is why the Annotated Disch assembly is required.
				auto routineoffset = delem.routineoffset;
				auto offset = bankaddr + routineoffset;
				auto newvalue = TextToValue(marker, line.value.c_str());
				if (marker == "hcfanfare") {
					auto iter = find_by_data(m_fanfarevalues, newvalue);
					if (iter == cend(m_fanfarevalues))
						Write16(offset, ASM_IGNORE);  // write the ignore value
					else
						Write16(offset, iter->first); // write the 16-bit value (opcode/opparam or nop/nop)
				}
				else {
					if (line.bits == 8)
						Write8(offset, (BYTE)newvalue);
					else
						Write16(offset, newvalue);
				}
			}
		}
	}

	// Now write the entries that we know (from the dialogueini file) into
	// the working ROM buffer's routine pointer table.
	// We can do this because m_asmroutinenames is 1-to-1 with the main sprite list,
	// and both refer to sprites using the same ordering.
	for (size_t index = 0; index < m_asmroutinenames.size(); ++index) {
		const auto& name = m_asmroutinenames[index];
		auto iter = m_routineaddrmap.find(name.c_str());
		auto bankaddr = iter != cend(m_routineaddrmap) ? iter->second : 0;
		if (bankaddr != 0) {
			this->WriteHandlerAddr((int)index, bankaddr);
		}
	}

	// Build the known handler list from the INI, since it's how the game
	// knows what to read and where to read it in both ROM and ASM.
	EnaureSynchedHandlerTables();
	EnsureValidSpriteHandlers();
}

void CSpriteDialogue2::SaveAsmTalkData(GameSerializer & ser)
{
	size_t failcount = 0;
	for (int mainindex = 0; mainindex < m_mainlist.GetCount(); ++mainindex) {
		if (m_mainlist.GetItemData(mainindex) == IGNOREDITEM) continue;

		auto newbankaddr = ReadHandlerAddr(mainindex);
		auto iter = find_by_data(m_routineaddrmap, newbankaddr);
		if (iter == end(m_routineaddrmap)) {
			// This means that the sprite's current bankaddr refers to a bank address that
			// DOES NOT match any address in the routine table TALKROUTINEPTRTABLE_OFFSET.
			ErrorHere << "Unmapped routines cannot be used, so sprite " << mainindex << " will not be saved" << std::endl;
			++failcount;
			continue;
		}

		// Update the routine table to reflect the new name, and the routine map to reflect the new address.
		//TODO - for now, there's no facility to rename routine labels, so this has no effect.
		std::string routinename = (LPCSTR)iter->first;
		m_asmroutinenames[mainindex] = routinename.c_str();
		iter->second = newbankaddr;

		// Update the hardcoded elements.
		// Params are already saved inplace in the parameters table TALKROUTINEDATA_OFFSET.
		auto & lines = m_asmroutinemap[routinename];
		for (auto & line : lines) {
			auto delem = dialogue_helpers::ReadElement(m_proj, routinename.c_str(), line.element.c_str());
			const auto & marker = delem.marker;

			// We only process hardcoded elements when writing to assembly files.
			if (marker.Find("hc") == 0) {
				//N.B. - the Read calls below are reading values from the working ROM buffer.
				//		This allows FFHEx (via the asmdll) to write those values to the assembly source file.
				//		This is done using markers, which is why the Annotated Disch assembly is required.
				auto offset = newbankaddr + delem.routineoffset;
				if (marker == "hcfanfare") {
					auto newvalue = Read16(offset);
					auto faniter = m_fanfarevalues.find(newvalue);
					if (faniter != cend(m_fanfarevalues))
						line.value = ValueToText(marker, faniter->second); // get the ASM_TRUE/FALSE/IGNORE value
				}
				else {
					auto newvalue = (line.bits == 8) ?
						(int)Read8(offset) :
						Read16(offset);
					line.value = ValueToText(marker, newvalue); // get the ASM_TRUE/FALSE/IGNORE value
				}
			}
		}
	}

	std::string label = (LPCSTR)ReadIni(m_proj.DialoguePath, "Label_TalkJumpTable", "value", "");

#ifndef TEST_SAVE
	auto asmfile = Paths::Combine({ m_proj.ProjectFolder, "asm", ASM_0E });
#else
	//TESTING - test code writes to a temp file instead of destroying the real one
	//-- 
	auto origasm = Paths::Combine({ m_proj.ProjectFolder, "asm", ASM_0E });
	auto asmfile = Paths::Combine({ m_proj.ProjectFolder, "asm", ASM_0E + CString(".test") });
	Paths::FileDelete(asmfile);
	Paths::FileCopy(origasm, asmfile);
	//--
#endif

	ser.SaveDialogue(asmfile, label, m_asmroutinenames, m_asmroutinemap);
}

void CSpriteDialogue2::EnsureValidSpriteHandlers()
{
	// Define local functions for ROM and ASM handling
	std::function<bool(int)> romhandler = [this](int i) -> bool {
		int addr = ReadHandlerAddr(i);
		bool found = find_by_data(m_routineaddrmap, addr) != cend(m_routineaddrmap);
		bool mapped = (addr != 0) && found;
		return mapped;
	};
	std::function<bool(int)> asmhandler = [this](int i) -> bool {
		const auto& name = m_asmroutinenames[i];
		bool found = has(m_asmroutinemap, name);
		bool mapped = !name.empty() && found;
		return mapped;
	};

	std::function<CString(int)> romerrfunc = [this](int i) {
		int addr = ReadHandlerAddr(i);
		CString cs, label;
		m_mainlist.GetText(i, label);
		cs.Format("%s: bank address '%04X'", label, addr);
		return cs;
	};
	std::function<CString(int)> asmerrfunc = [this](int i) {
		CString cs, label;
		CString str = this->m_asmroutinenames[i].c_str();
		m_mainlist.GetText(i, label);
		cs.Format("%s: handler label '%s'", label, str);
		return cs;
	};

	const auto& handler = Project->IsRom() ? romhandler : asmhandler;
	const auto& errfunc = Project->IsRom() ? romerrfunc : asmerrfunc;
	static const int limit = 20;

	// Ignore missing handlers (if we aren't throwing on errors)
	CSpriteDialogueSettings stgs(*Project);
	bool throws = stgs.ThrowOnBadSpriteAddr;
	int errcount = 0;
	CString missing;
	for (int i = 0; i < m_mainlist.GetCount(); ++i) {
		auto mapped = handler(i);
		int ignore = mapped ? 0 : IGNOREDITEM;
		m_mainlist.SetItemData(i, ignore);
		if (!mapped) {
			++errcount;
			if (throws && errcount <= limit) missing.AppendFormat("%s\n", errfunc(i));
		}
	}

	if (errcount > 0) {
		if (throws) {
			missing.Insert(0, "Found one or more sprites with invalid handlers.\n");
			if (errcount > limit) missing.AppendFormat("Only displaying the first %d of them here.", limit);
			throw std::runtime_error((LPCSTR)missing);
		}
		else {
			missing.Format("Disabling %d sprite(s) due to an invalid handler.", errcount);
			AfxMessageBox(missing);
		}
	}
}

void CSpriteDialogue2::EnaureSynchedHandlerTables()
{
	CSpriteDialogueSettings stgs(*Project);
	bool throws = stgs.ThrowOnBadSpriteAddr;
	auto ininames = GetIniTalkRoutineNames(*Project);

	// Ensure thewe have the same handler count in INI and ASM/ROM
	if (ininames.size() != m_routineaddrmap.size()) {
		CString msg;
		msg.Format("Dialogue INI defines %d handlers, but the game only defines %d.",
			ininames.size(), m_routineaddrmap.size());
		if (throws) throw std::runtime_error((LPCSTR)msg);

		msg.Append("\n\nWARNING: be careful editing, it's advised to ensure the handlers "
			"are synchronized between the INI and the game before continuing.");
		AfxMessageBox(msg);
	}

	// Report on which sprite handlers aren't used in the game
	static const int limit = 20;
	int errors = 0;
	CString err;
	if (Project->IsRom()) {
		std::set<int> iniset;
		for (const auto& name : ininames) {
			int bankaddr = hex(ReadIni(m_proj.DialoguePath, name.c_str(), "bankaddr", "0x0"));
			if (bankaddr != 0)
				iniset.insert(bankaddr);
		}

		for (const auto& kv : m_routineaddrmap) {
			int bankaddr = kv.second;
			if (!has(iniset, bankaddr)) {
				++errors;
				if (errors <= limit)
					err.AppendFormat("ROM routine addr '%04X' not in the dialogue INI.\n", bankaddr);
			}
		}
	}
	else if (Project->IsAsm()) {
		// Compare the names since ASM doesn't have the bank addresses
		// Identify ASM routines that aren't declared in the dialogue INI
		std::set<std::string> iniset;
		for (const auto & name : ininames) iniset.insert(name);

		for (const auto & kv: m_asmroutinemap) {
			const auto & asmname = kv.first;
			if (!has(iniset, asmname)) {
				++errors;
				if (errors <= limit)
					err.AppendFormat("ASM routine '%s' not in the dialogue INI.\n", asmname.c_str());
			}
		}
	}

	if (errors > 0) {
		CString msg;
		msg.Format("%d routine(s) found in the game but not in the INI:\n", errors);
		err.Insert(0, msg);
		if (errors > limit) err.AppendFormat("Only the first %d will display here.", limit);
		if (throws) throw std::runtime_error((LPCSTR)err);

		err.Append("\n\nWARNING: be careful editing, it's advised to ensure that all routines "
			"defined in the game are also specified in the dialogue INI file before continuing.");
		AfxMessageBox(msg);
	}
}


void CSpriteDialogue2::LoadValues()
{
	ASSERT(m_curindex >= 0);
	ASSERT(m_curindex < m_mainlist.GetCount());

	auto bankaddr = ReadHandlerAddr(m_curindex);
	bool ignored = m_mainlist.GetItemData(m_curindex) == IGNOREDITEM;
	int nshow = ignored ? SW_HIDE : SW_SHOW;
	m_elementlist.ShowWindow(nshow);
	m_elementlist.EnableWindow(!ignored);
	m_comboRoutineType.ShowWindow(nshow);
	m_comboRoutineType.EnableWindow(!ignored);
	if (ignored) {
		CString msg;
		if (bankaddr != 0)
			msg.Format("Can't edit this sprite because it uses an unmapped routine at bank address $%04x", bankaddr);
		else
			msg = "Can't edit this sprite because it uses an unknown routine";
		m_unknownfuncaddrstatic.SetWindowText(msg);
		m_unknownfuncaddrstatic.ShowWindow(SW_SHOW);
		m_elementlist.DeleteAllItems();
	} else {
		m_unknownfuncaddrstatic.SetWindowText("");
		m_unknownfuncaddrstatic.ShowWindow(SW_HIDE);
		SelectItemByData(m_comboRoutineType, bankaddr);
		OnSelchangeComboTalkroutinetype();
	}
}

void CSpriteDialogue2::StoreValues()
{
	ASSERT(m_curindex >= 0);
	ASSERT(m_curindex < m_mainlist.GetCount());

	if (m_mainlist.GetItemData(m_curindex) == IGNOREDITEM) return;

	auto funcaddr = GetSelectedItemData(m_comboRoutineType);
	WriteHandlerAddr(m_curindex, (int)funcaddr);
	SaveElementList();
}

void CSpriteDialogue2::FormatElementList()
{
	m_elementlist.SetExtendedStyle(m_elementlist.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_elementlist.DeleteAllItems();
	while (m_elementlist.GetHeaderCtrl()->GetItemCount() > 0)
		m_elementlist.DeleteColumn(0);

	int col = 0;
	m_elementlist.InsertColumn(col++, "Type", 0, 96);
	m_elementlist.InsertColumn(col++, "Value", 0, 256);
	m_elementlist.InsertColumn(col++, "Desc", 0, 384);
}

void CSpriteDialogue2::SetupLayout()
{
	SetWindowPos(nullptr, -1, -1, 960, 480, SWP_NOMOVE | SWP_NOZORDER);
	ModifyStyle(WS_OVERLAPPED | WS_THICKFRAME, 0);

	m_banner.Set(this, RGB(0, 0, 0), RGB(255, 32, 64), "Sprite Dialogue");

	m_inplacecombo.Create(m_inplacecombo.IDD, &m_elementlist);
	m_inplaceedit.Create(m_inplaceedit.IDD, &m_elementlist);
	m_inplaceedit.SetOwner(&m_elementlist);

	AlignToTopLeft(&m_comboRoutineType, {&m_unknownfuncaddrstatic});
}

void CSpriteDialogue2::LoadElementList()
{
	m_elementlist.DeleteAllItems();

	CString routinename;
	m_comboRoutineType.GetWindowText(routinename);
	auto bankaddr = hex(ReadIni(m_proj.DialoguePath, routinename, "bankaddr", ""));
	auto elemnames = Ini::ReadIniKeyNames(m_proj.DialoguePath, routinename);
	for (const auto & elem : elemnames) {
		if (elem.Find("elem") != 0) continue;

		int elemindex = -1;
		sscanf(elem, "elem%d", &elemindex);
		if (elemindex == -1) continue;

		auto newindex = m_elementlist.InsertItem(elemindex, "");
		ASSERT(newindex == elemindex);
		LoadElement(newindex, bankaddr, routinename);
	}
}

void CSpriteDialogue2::SaveElementList()
{
	CString routinename;
	m_comboRoutineType.GetWindowText(routinename);
	auto bankaddr = hex(ReadIni(m_proj.DialoguePath, routinename, "bankaddr", ""));
	auto elemnames = Ini::ReadIniKeyNames(m_proj.DialoguePath, routinename);
	for (const auto & elem : elemnames) {
		if (elem.Find("elem") != 0) continue;

		int elemindex = -1;
		sscanf(elem, "elem%d", &elemindex);
		if (elemindex == -1) continue;

		SaveElement(elemindex, bankaddr, routinename);
	}
}

void CSpriteDialogue2::LoadElement(int elementindex, int bankaddr, CString routinename)
{
	CString elem;
	elem.Format("elem%d", elementindex);

	auto delem = dialogue_helpers::ReadElement(m_proj, routinename, elem);
	const auto & marker = delem.marker;
	const auto & comment = delem.comment;
	const auto & inputvalue = delem.isHardcoded() ? hex(delem.routineoffset) : dec(delem.paramindex);

	if (m_proj.IsRom()) {
		//TODO - this function has to change to complete the move from parts to delem
		//		so pass deleme to it directly instead of marker and inputvalue
		auto elemvalue = GetElementValue(elementindex, bankaddr, marker, inputvalue);
		m_elementlist.SetItemText(elementindex, 0, marker);
		m_elementlist.SetItemText(elementindex, 1, elemvalue.first);
		m_elementlist.SetItemText(elementindex, 2, comment);
		m_elementlist.SetItemData(elementindex, elemvalue.second); // reference value, if == IGNOREDITEM, then ignore this element
	}
	else if (m_proj.IsAsm()) {
		//REFACTOR - instead of throwing exceptions, just display a message that the element can't be edited?
		try {
			auto iter = m_asmroutinemap.find((LPCSTR)routinename);
			if (iter == end(m_asmroutinemap))
				THROWEXCEPTION(std::runtime_error, "can't find elementlist for routine '" + std::string((LPCSTR)routinename) + "' while loading element " + std::to_string(elementindex));
			if (elementindex >= (int)iter->second.size())
				THROWEXCEPTION(std::runtime_error, "Index '" + std::to_string(elementindex) + "' is out of range for routine '" + std::string((LPCSTR)routinename) + "', can't load element ");
			if (iter->second[elementindex].element.c_str() != elem)
				THROWEXCEPTION(std::runtime_error, "Index '" + std::to_string(elementindex) + "' element doesn't match the element specified in routine '" + std::string((LPCSTR)routinename) + "'");

			auto elemvalue = GetElementValue(elementindex, bankaddr, marker, inputvalue);
			m_elementlist.SetItemText(elementindex, 0, marker);
			m_elementlist.SetItemText(elementindex, 1, elemvalue.first);
			m_elementlist.SetItemText(elementindex, 2, comment);
			m_elementlist.SetItemData(elementindex, elemvalue.second); // reference value, if == IGNOREDITEM, then ignore this element
		}
		catch (std::exception & ex) {
			ErrorHere << ex.what() << std::endl;
			CString text;
			text.Format("Error loading '%s'", (LPCSTR)elem);
			m_elementlist.SetItemText(elementindex, 0, marker);
			m_elementlist.SetItemText(elementindex, 1, text);
			m_elementlist.SetItemText(elementindex, 2, ex.what());
			m_elementlist.SetItemData(elementindex, (DWORD_PTR)IGNOREDITEM);
		}
	}
}

void CSpriteDialogue2::SaveElement(int elementindex, int bankaddr, CString routinename)
{
	if (m_elementlist.GetItemData(elementindex) == IGNOREDITEM) return; // don't save ignored items (don't save their comments either)

	CString elem;
	elem.Format("elem%d", elementindex);

	auto newcomment = m_elementlist.GetItemText(elementindex, 2);
	auto delem = dialogue_helpers::ReadElement(m_proj, routinename, elem);
	const auto & marker = delem.marker;
	const auto & inputvalue = delem.isHardcoded() ? hex(delem.routineoffset) : dec(delem.paramindex);

	// Update ROM
	//TODO - this function might also need to change to move from parts to delem
	SetRomValue(elementindex, bankaddr, marker, inputvalue, m_elementlist.GetItemText(elementindex, 1));

	//TODO - currently, the ASM comment and dialogue file comment are UNRELATED
	//		and this will NOT change the comment written to the ASM source file.
	//		It's not clear if the comment here SHOULD change the asm source comment,
	//		but for now it does not.

	// If this is an assembly project, update the comment for this routine's element
	if (m_proj.IsAsm()) {
		auto iter = m_asmroutinemap.find((LPCSTR)routinename);
		if (iter != end(m_asmroutinemap))
			iter->second[elementindex].comment = newcomment;
	}

	// Now update the comment in the dialogue file
	delem.comment = newcomment;
	dialogue_helpers::WriteElement(m_proj, routinename, elem, delem);
}

void CSpriteDialogue2::EditElementValue(int elementindex, CString marker, CString value)
{
	if (m_elementlist.GetItemData(elementindex) == IGNOREDITEM) return; // don't edit ignored items

	auto rcitem = Ui::GetSubitemRect(m_elementlist, elementindex, 1);

	const auto DoEditDropdown = [&rcitem,elementindex,this](LoadEntryFunc func, bool showindex = true) {
		m_inplacecombo.SetPosition(rcitem);
		m_inplacecombo.ClearData();
		auto & box = m_inplacecombo.m_combo;
		LoadCombo(m_inplacecombo.m_combo, func(m_proj, showindex));

		auto itemdata = m_elementlist.GetItemData(elementindex);
		if (!SelectItemByData(box, itemdata))
			SelectFirstItem(box);

		m_inplacecombo.OKFunc = [&, this, elementindex](int selindex, DWORD_PTR newitemdata) {
			CString text;
			box.GetLBText(selindex, text);
			m_elementlist.SetItemText(elementindex, 1, text);
			m_elementlist.SetItemData(elementindex, newitemdata);
			return true;
		};
		return true;
	};

	const auto DoLoadItemEntries = [marker,this](CFFHacksterProject & proj, bool showindex) {
		UNREFERENCED_PARAMETER(showindex);
		// Load all entries without the index, then replace the index with the appropriate format and value
		auto entries = Enloader->LoadItemEntries(proj, false);
		bool hardcoded = marker.Find("hc") == 0;
		CString indexformat = hardcoded ? "%04X" : "%02X";
		auto adjustby = hardcoded ? m_itemsaddr : 0;
		AdjustRamItemEntries(proj, entries, indexformat, adjustby);
		return entries;
	};
	const auto DoLoadItemOrMapEntries = [marker,DoLoadItemEntries,this](CFFHacksterProject & proj, bool showindex) {
		auto entries = LoadMapSpriteEntries(proj, showindex) + DoLoadItemEntries(proj, showindex);
		return entries;
	};
	const auto DoLoadItemOrCanoeEntries = [marker, DoLoadItemEntries, this](CFFHacksterProject & proj, bool showindex) {
		auto entries = LoadCanoeEntries(proj, showindex) + DoLoadItemEntries(proj, showindex);
		return entries;
	};
	const auto DoLoadFanfareEntries = [marker,elementindex,this](CFFHacksterProject & proj, bool showindex) {
		UNREFERENCED_PARAMETER(showindex);
		bool hardcoded = marker.Find("hc") == 0; // starts with "hc" if hardcoded
		if (!hardcoded)
			THROWEXCEPTION(std::invalid_argument, "elem" + std::to_string(elementindex) + std::string("fanfare must be hardcoded, but uses non-hardcoded marker ") + (LPCSTR)marker);

		const auto LoadEntry = [&](int value, int index) {
			auto node = LoadFanfareLabel(proj, index, false);
			CString newname;
			newname.Format("%04X: %s", value, (LPCSTR)node.name);
			node.name = newname;
			node.value = value;
			return node;
		};
		return dataintnodevector{ LoadEntry(0x7DE6, ASM_TRUE), LoadEntry(0xEAEA, ASM_FALSE) };
	};
	const auto DoInplaceEdit = [&,this](CString (*tostr)(int)) {
		int oldvalue = (int)m_elementlist.GetItemData(elementindex);
		m_inplaceedit.ClearData();
		m_inplaceedit.SetPosition(rcitem);
		m_inplaceedit.SetText(tostr(oldvalue));
		m_inplaceedit.m_edit.SetSel(0, -1);
		m_inplaceedit.OKFunc = [&,elementindex,tostr](CString newtext) {
			// Convert to int and back to ensure we save in the correct format. The wrong format throws an exception.
			BYTE newint = (BYTE)to_int(newtext);
			auto str = tostr(newint);
			m_elementlist.SetItemText(elementindex, 1, tostr(newint));
			m_elementlist.SetItemData(elementindex, newint);
			return true;
		};
	};
	const auto LoadBattleLabels2 = [&](CFFHacksterProject& proj, bool showindex) {
		return LoadBattleLabelsEx(proj, false, showindex);
	};

	if (marker == "text" || marker == "hctext")
		DoEditDropdown(LoadGameTextEntries);
	else if (marker == "obj" || marker == "hcobj" || marker == "event" || marker == "hcevent" || marker == "spr" || marker == "hcspr")
		DoEditDropdown(LoadSpriteLabels);
	else if (marker == "battle" || marker == "hcbattle")
		DoEditDropdown(LoadBattleLabels2);
	else if (marker == "item" || marker == "hcitem")
		DoEditDropdown(DoLoadItemEntries);
	else if (marker == "hcmap")
		DoEditDropdown(LoadMapSpriteEntries);
	else if (marker == "weapon" || marker == "hcweapon")
		DoEditDropdown([this](CFFHacksterProject & proj, bool showindex) { return Enloader->LoadWeaponEntries(proj, showindex); });
	else if (marker == "hcitemormap")
		DoEditDropdown(DoLoadItemOrMapEntries);
	else if (marker == "hcitemorcanoe")
		DoEditDropdown(DoLoadItemOrCanoeEntries);
	else if (marker == "hcfanfare")
		DoEditDropdown(DoLoadFanfareEntries);
	else if (marker == "hcnntele")
		DoEditDropdown(LoadNNTeleportLabels);
	else if (marker == "hcnotele")
		DoEditDropdown(LoadNOTeleportLabels);
	else if (marker == "hcuint8")
		DoInplaceEdit(from_dec);
	else if (marker == "hcbyte")
		DoInplaceEdit(from_addr);
	else if (marker == "gold" || marker == "hcgold")
		DoEditDropdown([this](CFFHacksterProject & proj, bool showindex) { return Enloader->LoadGoldEntries(proj, showindex); });
	else
		ErrorHere << " Unable to edit unknown element: type '" << marker << "' value '" << value << "'" << std::endl;
}

// inputvalue: from the .dialogue file, and it's either the talkparam index or the offset from bankaddr to the deisred hardcoded value.
std::pair<CString, int> CSpriteDialogue2::GetElementValue(int elementindex, int bankaddr, CString marker, CString inputvalue)
{
	const auto DoLoadEntry = [bankaddr, this](LoadSingleEntryFunc func, CString marker, CString inputvalue) -> std::pair<CString, int> {
		bool hardcoded = marker.Find("hc") == 0; // starts with "hc" if hardcoded
		int itemref = hardcoded ? Read8(bankaddr + hex(inputvalue)) : ReadParam(dec(inputvalue));
		return{ func(m_proj, itemref, true).name, itemref };
	};
	const auto DoLoadEntry16 = [bankaddr, elementindex, this](LoadSingleEntryFunc func, CString marker, CString inputvalue) -> std::pair<CString, int> {
		bool hardcoded = marker.Find("hc") == 0; // starts with "hc" if hardcoded
		if (!hardcoded)
			THROWEXCEPTION(std::invalid_argument, "elem" + std::to_string(elementindex) + std::string(": 16-bit reads must be hardcoded, but uses non-hardcoded marker ") + (LPCSTR)marker);

		int itemref = Read16(bankaddr + hex(inputvalue));
		return{ func(m_proj, itemref, true).name, itemref };
	};
	const auto DoLoadItemEntry = [bankaddr, this](CString marker, CString inputvalue) -> std::pair<CString, int> {
		// We want the item name (or a fallback from the values file if it's blank).
		bool hardcoded = marker.Find("hc") == 0;
		int itemref = hardcoded ? Read16(bankaddr + hex(inputvalue)) : ReadParam(dec(inputvalue));
		int itemindex = hardcoded ? itemref - m_itemsaddr : itemref;
		int fallbackref = hardcoded ? itemref : itemref + m_itemsaddr;
		auto entry = Enloader->LoadItemEntry(m_proj, itemindex, false);
		CString format = hardcoded ? "%04X: " : "%02X: ";
		CString name;
		name.Format(format, itemref);
		name += GetItemFallbackName(m_proj, entry.name, fallbackref);
		return{ name, itemref };
	};
	const auto DoLoadItemOrMap = [bankaddr, DoLoadEntry16, DoLoadItemEntry, this](CString marker, CString inputvalue) -> std::pair<CString, int> {
		bool hardcoded = marker.Find("hc") == 0; // starts with "hc" if hardcoded
		if (hardcoded) {
			// If hardcoded AND the itemref is below the items address, then it's really a map sprite.
			// Map sprites can't be parameterized: params are 1 byte each, and the single-byte range would overlap with that of param items.
			int itemref = hardcoded ? Read16(bankaddr + hex(inputvalue)) : ReadParam(dec(inputvalue));
			if (itemref < m_itemsaddr)
				return DoLoadEntry16(LoadMapSpriteEntry, marker, inputvalue);
		}
		// Otherwise assume it's an item (which can be either a param or hardcoded entry).
		return DoLoadItemEntry(marker, inputvalue);
	};
	const auto DoLoadItemOrCanoe = [bankaddr, DoLoadEntry16, DoLoadItemEntry, this](CString marker, CString inputvalue) -> std::pair<CString, int> {
		// If not hardcoded, then it must be an item (can't store both mapsprites and items as 1-byte params, the slot index ranges overlap)
		bool hardcoded = marker.Find("hc") == 0; // starts with "hc" if hardcoded
		if (hardcoded) {
			int itemref = hardcoded ? Read16(bankaddr + hex(inputvalue)) : ReadParam(dec(inputvalue));
			if (itemref < m_itemsaddr)
				return DoLoadEntry16(LoadMapSpriteEntry, marker, inputvalue);
		}
		// Otherwise assume it's an item (which can be either a param or hardcoded entry).
		return DoLoadItemEntry(marker, inputvalue);
	};
	const auto DoLoadFanfareEntry = [bankaddr, elementindex, DoLoadEntry, this](CString marker, CString inputvalue) -> std::pair<CString, int> {
		bool hardcoded = marker.Find("hc") == 0; // starts with "hc" if hardcoded
		if (!hardcoded)
			THROWEXCEPTION(std::invalid_argument, "elem" + std::to_string(elementindex) + std::string(": fanfare must be hardcoded, but uses non-hardcoded marker ") + (LPCSTR)marker);

		auto itemref = Read16(bankaddr + hex(inputvalue));
		auto iter = m_fanfarevalues.find(itemref);
		if (iter == cend(m_fanfarevalues))
			return{ "IGNORED", IGNOREDITEM };

		auto entry = LoadFanfareLabel(m_proj, iter->second, false);
		CString newname;
		newname.Format("%04X: %s", itemref, (LPCSTR)entry.name);
		return{ newname, itemref };
	};
	const auto DoLoadNumber8 = [&, this](CString marker, CString inputvalue, CString(*formatter)(int)) -> std::pair<CString, int> {
		bool hardcoded = marker.Find("hc") == 0; // starts with "hc" if hardcoded
		auto intvalue = hardcoded ? Read8(bankaddr + hex(inputvalue)) : ReadParam(dec(inputvalue));
		return{ formatter(intvalue), intvalue };
	};
	const auto LoadBattleLabel2 = [this](CFFHacksterProject& proj, int index, bool showindex) {
		return LoadBattleLabelEx(proj, index, showindex);
	};

	if (marker == "text" || marker == "hctext")
		return DoLoadEntry(LoadGameTextEntry, marker, inputvalue);
	if (marker == "obj" || marker == "hcobj" || marker == "event" || marker == "hcevent" || marker == "spr" || marker == "hcspr")
		return DoLoadEntry(LoadSpriteLabel, marker, inputvalue);
	if (marker == "battle" || marker == "hcbattle")
		return DoLoadEntry(LoadBattleLabel2, marker, inputvalue);
	if (marker == "item" || marker == "hcitem")
		return DoLoadItemEntry(marker, inputvalue);
	if (marker == "hcmap")
		return DoLoadEntry16(LoadMapSpriteEntry, marker, inputvalue);
	if (marker == "weapon" || marker == "hcweapon")
		return DoLoadEntry(
			[this](CFFHacksterProject & proj, int index, bool showindex) { return Enloader->LoadWeaponEntry(proj, index, showindex); },
			marker, inputvalue);
	if (marker == "hcitemormap")
		return DoLoadItemOrMap(marker, inputvalue);
	if (marker == "hcitemorcanoe")
		return DoLoadItemOrCanoe(marker, inputvalue);
	if (marker == "hcfanfare")
		return DoLoadFanfareEntry(marker, inputvalue);
	if (marker == "nntele" || marker == "hcnntele")
		return DoLoadEntry(LoadNNTeleportLabel, marker, inputvalue);
	if (marker == "notele" || marker == "hcnotele")
		return DoLoadEntry(LoadNOTeleportLabel, marker, inputvalue);
	if (marker == "hcuint8")
		return DoLoadNumber8(marker, inputvalue, dec);
	if (marker == "hcbyte")
		return DoLoadNumber8(marker, inputvalue, addr);
	if (marker == "gold" || marker == "hcgold")
		return DoLoadEntry(
			[this](CFFHacksterProject & proj, int index, bool showindex) { return Enloader->LoadGoldEntry(proj, index, showindex); },
			marker, inputvalue);

	return{ "<unsupported>", IGNOREDITEM };
}

// inputvalue: from the .dialogue file, and it's either the talkparam index or the offset from bankaddr to the deisred hardcoded value.
void CSpriteDialogue2::SetRomValue(int elementindex, int bankaddr, CString marker, CString inputvalue, CString newvalue)
{
	const auto DoWriteValue = [bankaddr, this](CString marker, CString inputvalue, CString newvalue) {
		bool hardcoded = marker.Find("hc") == 0; // starts with "hc" if hardcoded
		if (hardcoded)
			Write8(bankaddr + hex(inputvalue), (BYTE)strtoul(newvalue, nullptr, 16));
		else
			WriteParam(dec(inputvalue), (BYTE)strtoul(newvalue, nullptr, 16));
	};
	const auto DoWriteValue16 = [bankaddr, elementindex, this](CString marker, CString inputvalue, CString newvalue) {
		bool hardcoded = marker.Find("hc") == 0;
		if (!hardcoded)
			THROWEXCEPTION(std::invalid_argument, "elem" + std::to_string(elementindex) + std::string(": 16-bit writes must be hardcoded, but uses non-hardcoded marker ") + (LPCSTR)marker);

		Write16(bankaddr + hex(inputvalue), strtoul(newvalue, nullptr, 16));
	};
	const auto DoWriteItemValue = [bankaddr, this](CString marker, CString inputvalue, CString newvalue) {
		bool hardcoded = marker.Find("hc") == 0;
		if (hardcoded)
			Write16(bankaddr + hex(inputvalue), strtoul(newvalue, nullptr, 16));
		else
			WriteParam(dec(inputvalue), (BYTE)strtoul(newvalue, nullptr, 16));
	};
	const auto DoWriteFanfareValue = [bankaddr, elementindex, this](CString marker, CString inputvalue, CString newvalue) {
		bool hardcoded = marker.Find("hc") == 0;
		if (!hardcoded)
			THROWEXCEPTION(std::invalid_argument, "elem" + std::to_string(elementindex) + std::string(": fanfare must be hardcoded, but uses non-hardcoded marker ") + (LPCSTR)marker);

		int intvalue = strtoul(newvalue, nullptr, 16);
		Write16(bankaddr + hex(inputvalue), intvalue);
	};
	const auto DoWriteNumber8 = [bankaddr, this](CString marker, CString inputvalue, CString newvalue, int (*formatter)(CString text)) {
		int intvalue = formatter(newvalue);
		bool hardcoded = marker.Find("hc") == 0;
		if (hardcoded)
			Write8(bankaddr + hex(inputvalue), (BYTE)intvalue);
		else
			WriteParam(dec(inputvalue), (BYTE)intvalue);
	};

	if (marker == "text" || marker == "hctext")
		DoWriteValue(marker, inputvalue, newvalue);
	else if (marker == "obj" || marker == "hcobj" || marker == "event" || marker == "hcevent" || marker == "spr" || marker == "hcspr")
		DoWriteValue(marker, inputvalue, newvalue);
	else if (marker == "battle" || marker == "hcbattle")
		DoWriteValue(marker, inputvalue, newvalue);
	else if (marker == "item" || marker == "hcitem")
		DoWriteItemValue(marker, inputvalue, newvalue);
	else if (marker == "hcmap")
		DoWriteValue16(marker, inputvalue, newvalue);
	else if (marker == "weapon" || marker == "hcweapon")
		DoWriteValue(marker, inputvalue, newvalue);
	else if (marker == "hcitemormap")
		DoWriteValue(marker, inputvalue, newvalue);
	else if (marker == "hcitemorcanoe")
		DoWriteValue(marker, inputvalue, newvalue);
	else if (marker == "hcfanfare")
		DoWriteFanfareValue(marker, inputvalue, newvalue);
	else if (marker == "nntele" || marker == "hcnntele")
		DoWriteValue(marker, inputvalue, newvalue);
	else if (marker == "notele" || marker == "hcnotele")
		DoWriteValue(marker, inputvalue, newvalue);
	else if (marker == "hcuint8")
		DoWriteNumber8(marker, inputvalue, newvalue, dec);
	else if (marker == "hcbyte")
		DoWriteNumber8(marker, inputvalue, newvalue, addr);
	else if (marker == "gold" || marker == "hcgold")
		DoWriteValue(marker, inputvalue, newvalue);
}

void CSpriteDialogue2::EditElementComment(int elementindex, CString comment)
{
	if (m_elementlist.GetItemData(elementindex) == IGNOREDITEM) return; // don't edit ignored items

	auto rcitem = Ui::GetSubitemRect(m_elementlist, elementindex, 2);
	m_inplaceedit.InitEdit(rcitem, comment);
	m_inplaceedit.OKFunc = [&, this, elementindex](CString newcomment) {
		bool edited = false;
		try {
			m_elementlist.SetItemText(elementindex, 2, newcomment);
			edited = true;
		}
		catch (std::exception & ex) {
			ErrorHere << "EditElementComment caught an exception: " << ex.what() << ".\n\tthe edit will be ignored." << std::endl;
		}
		catch (...) {
			ErrorHere << "EditElementComment caught an unknown exception:\n\tthe edit will be ignored." << std::endl;
		}
		return edited;
	};
}

int CSpriteDialogue2::ReadHandlerAddr(int index)
{
	int romoffset = TALKROUTINEPTRTABLE_OFFSET + (index * TALKROUTINEPTRTABLE_BYTES);
	int value = m_proj.ROM[romoffset] + (m_proj.ROM[romoffset + 1] << 8);
	return value;
}

void CSpriteDialogue2::WriteHandlerAddr(int index, int data)
{
	int romoffset = TALKROUTINEPTRTABLE_OFFSET + (index * TALKROUTINEPTRTABLE_BYTES);
	m_proj.ROM[romoffset] = (data) & 0xFF;
	m_proj.ROM[romoffset + 1] = (data >> 8) & 0xFF;
}

BYTE CSpriteDialogue2::Read8(int addedoffset)
{
	int romoffset = TALKROUTINEPTRTABLE_PTRADD + addedoffset;
	BYTE value = m_proj.ROM[romoffset];
	return value;
}

void CSpriteDialogue2::Write8(int addedoffset, BYTE data)
{
	int romoffset = TALKROUTINEPTRTABLE_PTRADD + addedoffset;
	m_proj.ROM[romoffset] = data;
}

int CSpriteDialogue2::Read16(int addedoffset)
{
	int romoffset = TALKROUTINEPTRTABLE_PTRADD + addedoffset;
	int value = m_proj.ROM[romoffset] + (m_proj.ROM[romoffset + 1] << 8);
	return value;
}

void CSpriteDialogue2::Write16(int addedoffset, int data)
{
	int romoffset = TALKROUTINEPTRTABLE_PTRADD + addedoffset;
	m_proj.ROM[romoffset] = (data) & 0xFF;
	m_proj.ROM[romoffset + 1] = (data >> 8) & 0xFF;
}

BYTE CSpriteDialogue2::ReadParam(int index)
{
	int romoffset = TALKROUTINEDATA_OFFSET + (m_curindex * TALKROUTINEDATA_BYTES) + index;
	BYTE value = m_proj.ROM[romoffset];
	return value;
}

void CSpriteDialogue2::WriteParam(int index, BYTE value)
{
	int romoffset = TALKROUTINEDATA_OFFSET + (m_curindex * TALKROUTINEDATA_BYTES) + index;
	m_proj.ROM[romoffset] = value;
}

int CSpriteDialogue2::TextToValue(CString marker, std::string inputvalue)
{
	auto iter = m_proj.m_varmap.find(inputvalue);
	if (iter != cend(m_proj.m_varmap))
		return iter->second;

	if (!inputvalue.empty() && isalpha(inputvalue.front()))
		THROWEXCEPTION(std::invalid_argument, "Can't translate " + inputvalue + " into an index.");

	// These are usually in either dec or addr format (remember, assembly doesn't use C++ 0x hex format).
	CString csvalue = inputvalue.c_str();
	if (is_dec(csvalue))
		return dec(csvalue);
	if (is_addr(csvalue))
		return addr(csvalue);
	if (is_hex(csvalue))
		return hex(csvalue);

	THROWEXCEPTION(std::invalid_argument, "The type of '" + inputvalue + "' is unknown and can't be converted.");
}

std::string CSpriteDialogue2::ValueToText(CString marker, int newvalue)
{
	ASSERT(marker.Find("hc") == 0);
	if (marker.Find("hc") != 0)
		THROWEXCEPTION(std::runtime_error, "can't write value to asm text because marker '" + std::string((LPCSTR)marker) + "' is not a hardcoded marker");

	const auto SetHCIndexData = [marker,newvalue,this](asm_search searchtype, std::string keyorprefixnames)
	{
		auto strnewvalue = search_for_match(m_proj, searchtype, keyorprefixnames, newvalue);
		return strnewvalue;
	};
	const auto SaveFanfareItem = [marker](int fanvalue) {
		// this should only write true or false values, NOT ignore.
		switch (fanvalue) {
		case ASM_TRUE:
		case ASM_FALSE: return std::to_string(fanvalue);
		}

		if (fanvalue != ASM_IGNORE)
			ErrorHere << "marker " << (LPCSTR)marker << "' has invalid conditional value '" << fanvalue << "'" << std::endl;
		ASSERT(FALSE); // invalid values will default to ASM_IGNORE with a log entry

		THROWEXCEPTION(std::runtime_error, "marker " + std::string((LPCSTR)marker) + "' has invalid conditional value '" + std::to_string(fanvalue) + "'");
	};

	if (marker == "hcspr" || marker == "hcobj" || marker == "hcevent")
		return SetHCIndexData(asm_search::prefix, "OBJID_");
	if (marker == "hctext")
		return SetHCIndexData(asm_search::prefix, "DLGID_");
	if (marker == "hcmap")
		return SetHCIndexData(asm_search::inimatch, "MAPOBJ_NAMES");
	if (marker == "hcbattle")
		return SetHCIndexData(asm_search::prefix, "BTL_");
	if (marker == "hcitem")
		return SetHCIndexData(asm_search::inimatch, "KEYITEM_NAMES PROVISION_NAMES ORB_NAMES");
	if (marker == "hcweapon")
		return SetHCIndexData(asm_search::prefix, "WPNID_");
	if (marker == "hcitemorcanoe")
		return SetHCIndexData(asm_search::inimatch, "KEYITEM_NAMES PROVISION_NAMES ORB_NAMES CANOE_NAMES");
	if (marker == "hcitemormap")
		return SetHCIndexData(asm_search::inimatch, "KEYITEM_NAMES PROVISION_NAMES ORB_NAMES CANOE_NAMES MAPOBJ_NAMES");
	if (marker == "hcnntele")
		return SetHCIndexData(asm_search::prefix, "NORMTELE_");
	if (marker == "hcnotele")
		return SetHCIndexData(asm_search::prefix, "NOUTTELE_");
	if (marker == "hcfanfare")
		return SaveFanfareItem(newvalue);
	if (marker == "hcuint8")
		return (LPCSTR)dec(newvalue & 0xFF);
	if (marker == "hcbyte")
		return (LPCSTR)addr(newvalue & 0xFF);

	// If a new marker type is added, for now just return it as an addr value.
	// For now, don't emit any warnings to avoid spamming the log (an exception will generate an error and log entries).
	return (LPCSTR)addr(newvalue & 0xFF);
}

void CSpriteDialogue2::DoDataExchange(CDataExchange* pDX)
{
	CEditorWithBackground::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_mainlist);
	DDX_Control(pDX, IDC_LIST_TALKELEMENTS, m_elementlist);
	DDX_Control(pDX, IDC_COMBO_TALKROUTINETYPE, m_comboRoutineType);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDC_SPRITE_DIALOGUE_SETTINGS, m_settingsbutton);
	DDX_Control(pDX, IDC_STATIC1, m_unknownfuncaddrstatic);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
}


BEGIN_MESSAGE_MAP(CSpriteDialogue2, CEditorWithBackground)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_SPRITE_DIALOGUE_SETTINGS, &CSpriteDialogue2::OnBnClickedSettings)
	ON_BN_CLICKED(IDC_SAVE, &CSpriteDialogue2::OnBnClickedSave)
	ON_LBN_SELCHANGE(IDC_LIST, &CSpriteDialogue2::OnSelchangeList)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TALKELEMENTS, &CSpriteDialogue2::OnNMClickListTalkelements)
	ON_CBN_SELCHANGE(IDC_COMBO_TALKROUTINETYPE, &CSpriteDialogue2::OnSelchangeComboTalkroutinetype)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSpriteDialogue2 message handlers


void CSpriteDialogue2::OnPaint()
{
	CPaintDC dc(this);
	m_banner.Render(dc, 8, 8);
}

void CSpriteDialogue2::OnBnClickedSettings()
{
	CSpriteDialogueSettingsDlg dlg(this);
	dlg.m_proj = &m_proj;
	if (dlg.DoModal() == IDOK) {
		OnSelchangeList();
	}
}

void CSpriteDialogue2::OnBnClickedSave()
{
	DoSave();
}

void CSpriteDialogue2::OnSelchangeList()
{
	try {
		if (m_curindex != -1) StoreValues();
		m_curindex = m_mainlist.GetCurSel();
		LoadValues();
	}
	catch (std::exception & ex) {
		Ui::AbortFail(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		Ui::AbortFail(this, "An unexpected exception prevented game data from loading.");
	}
}

void CSpriteDialogue2::OnSelchangeComboTalkroutinetype()
{
	LoadElementList();
}

void CSpriteDialogue2::OnNMClickListTalkelements(NMHDR *pNMHDR, LRESULT *pResult)
{
	UNREFERENCED_PARAMETER(pResult);
	LPNMLISTVIEW pnm = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pnm != nullptr && pnm->iItem >= 0 && pnm->iItem < m_elementlist.GetItemCount()) {
		switch (pnm->iSubItem) {
		case 1:
			EditElementValue(pnm->iItem, m_elementlist.GetItemText(pnm->iItem, 0), m_elementlist.GetItemText(pnm->iItem, pnm->iSubItem));
			break;
		case 2:
			EditElementComment(pnm->iItem, m_elementlist.GetItemText(pnm->iItem, pnm->iSubItem));
			break;
		}
	}
}

namespace SpriteDialogue_Helpers // IMPLEMENTATION
{

	mfcstringvector read_names_vector(CFFHacksterProject & proj, mfcstringvector keynames)
	{
		mfcstringvector allvalnames;
		for (const auto & keyname : keynames) {
			mfcstringvector valnames;
			try {
				valnames = ReadIni(proj.ValuesPath, keyname, "value", mfcstringvector{});
			}
			catch (...) {
				THROWEXCEPTION(std::invalid_argument, (LPCSTR)keyname + std::string(" is not a str[], and therefore cannot be uded\nto retrieve varmap values."));
			}
			append(allvalnames, valnames);
		}
		return allvalnames;
	}

	std::string search_for_match(CFFHacksterProject & proj, asm_search searchtype, std::string key, int index)
	{
		stdstrintmap & varmap = proj.m_varmap;
		std::string result;

		//FUTURE - this currently searches EVERY entry in the varmap, change it to search specific sublists formed from the varmap?
		switch (searchtype) {
		case asm_search::match:
			{
				auto iter = std::find_if(cbegin(varmap), cend(varmap), [key, index](const auto & node) -> bool {
					return node.second == index && node.first == key;
				});
				if (iter != cend(varmap)) result = iter->first;
			}
			break;

		case asm_search::prefix:
			{
				auto prefixes = split(key, " ");
				auto iter = std::find_if(cbegin(varmap), cend(varmap), [index, prefixes](const auto & node) -> bool {
					for (const auto & prefix : prefixes) {
						if (node.second == index && node.first.find(prefix) == 0) return true;
					}
					return false;
				});
				if (iter != cend(varmap)) result = iter->first;
			}
			break;

		case asm_search::inimatch:
			{
				auto keynames = split(key.c_str(), CString(" "));
				auto names = read_names_vector(proj, keynames);
				auto iter = std::find_if(cbegin(varmap), cend(varmap), [index, names](const auto & node) -> bool {
					for (const auto & name : names) {
						if (node.second == index && node.first == (LPCSTR)name)
							return true;
					}
					return false;
				});
				if (iter != cend(varmap)) result = iter->first;
			}
			break;
		}

		if (result.empty()) result = std_assembly::shared::int_to_string(index, 16);
		return result;
	}

	stdstringvector GetIniTalkRoutineNames(CFFHacksterProject & proj)
	{
		auto ininames = ReadIniSectionNames(proj.DialoguePath);
		ininames.erase(std::remove_if(begin(ininames), end(ininames), [](const auto & n) { return _strnicmp(n, "talk", 4) != 0; }), end(ininames));
		return convert(ininames);
	}

} // end namespace SpriteDialogue_Helpers IMPLEMENTATION

void CSpriteDialogue2::OnSize(UINT nType, int cx, int cy)
{
	CEditorWithBackground::OnSize(nType, cx, cy);

	//DEVNOTE - for an inexplicable reason, setting the IDC_COMBO_TALKROUTINETYPE to use Dynamic Layout with
	//		Sizing Type = Horizontal and Sizing X = 100
	//		shrinks the height of the drop list to less than 10 pixels, making the drop list portion unusable.
	//		sigh ... resize the list manually for now.
	//		The combo's right edge aligns to the right edge of the cancel button.
	if (::IsWindow(m_comboRoutineType.GetSafeHwnd())) {
		auto cancel = GetDlgItem(IDCANCEL);
		auto rccancel = Ui::GetControlRect(cancel);
		auto rcroutine = Ui::GetControlRect(&m_comboRoutineType);
		rcroutine.right = rccancel.right;
		m_comboRoutineType.SetWindowPos(nullptr, -1, -1, rcroutine.Width(), rcroutine.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}
}
