#include "stdafx.h"
#include "ingame_text_functions.h"

#include <DataValueAccessor.h>
#include <FFHacksterProject.h>
#include <FFH2Project.h>

#include <dva_primitives.h>
#include "general_functions.h"
#include "ini_functions.h"
#include "path_functions.h"
#include "string_functions.h"
#include "ui_helpers.h"

using namespace Ini;
using namespace Strings;
using namespace Ui;
using ffh::fda::DataValueAccessor;

namespace Ingametext
{
	CString CStrIdentity(int, CString str)
	{
		return str;
	}

	CString PutHexToList(CFFHacksterProject* cart, int ptr, int ptradd, int last, bool DTE, CListBox* m_list, CComboBox* m_combo)
	{
		CString ret = "";
		int offset, co;
		BYTE temp;
		CString text, temptext;

		if (m_list != nullptr) m_list->SetRedraw(FALSE);
		if (m_combo != nullptr) m_combo->SetRedraw(FALSE);

		for (co = 0; co < last; co++, ptr += 2) {
			if (co) ret += "\\b";
			offset = cart->ROM[ptr] + (cart->ROM[ptr + 1] << 8) + ptradd;
			text = "";
			while (1) {
				temp = cart->ROM[offset];
				if (!temp) break;
				if (cart->Tables[DTE][temp] == "") {
					if (temp < 0x10) temptext.Format("{0%X}", temp);
					else temptext.Format("{%X}", temp);
					text += temptext;
				}
				else text += cart->Tables[DTE][temp];
				offset += 1;
			}
			ret += text;
			if (m_list != nullptr) InsertEntry(*m_list, -1, text, co);
			if (m_combo != nullptr) InsertEntry(*m_combo, -1, text, co);
		}

		if (m_list != nullptr) m_list->SetRedraw(TRUE);
		if (m_combo != nullptr) m_combo->SetRedraw(TRUE);
		return ret;
	}


	namespace {
		const int dialogue_maxlength = 20;
	}


	CString XformShortString(int, CString str)
	{
		return str.GetLength() < dialogue_maxlength ? str : str.Left(dialogue_maxlength - 3) + "...";
	}

	dataintnodevector LoadExpandedZeroBasedEntries(unsigned char* buffer, int ptr, int ptradd, int start, int count, CString table[], bool showindex, StrXform xform)
	{
		dataintnodevector dvec;
		int offset, co;
		BYTE temp;
		CString text, temptext;

		int last = start + count - 1;
		for (co = start; co <= last; co++, ptr += 2) {
			offset = buffer[ptr] + (buffer[ptr + 1] << 8) + ptradd;
			text = "";
			if (showindex) text.AppendFormat("%02X: ", co);

			while (1) {
				temp = buffer[offset];
				if (!temp) break;
				if (table[temp] == "") {
					temptext.Format("{%02X}", temp);
					text += temptext;
				}
				else text += table[temp];
				offset += 1;
			}
			CString xformtext = xform(co, text);
			dvec.push_back({ xformtext, co });
		}

		return dvec;
	}

	datanode<int> LoadExpandedZeroBasedEntry(unsigned char* buffer, int index, int ptr, int ptradd, CString table[], bool showindex, StrXform xform)
	{
		ASSERT(index >= 0);
		if (index >= 0) {
			// step forward by 'index' entries in the 2-byte/record pointer table
			auto indexedptr = ptr + (index * 2);
			auto vec = LoadExpandedZeroBasedEntries(buffer, indexedptr, ptradd, index, 1, table, showindex, xform);
			if (!vec.empty()) return vec.front();
		}
		return{ "<!error!>", -1 };
	}

	// Weapons IDs start with 0 as None, 1 as Wooden Nunchuk, but weapon text strings start with  0 as Wooden Nunchuk.
	// LoadExpandedZeroBasedEntries can't be used here because it will start the index at 0, we need it to start at 1.
	// index has to be passed in the [0,39] range, not [1,40] (at least for now).
	dataintnodevector LoadExpandedOneBasedEntries(unsigned char* buffer, int ptr, int ptradd, int start, int count, CString table[], bool showindex, StrXform xform)
	{
		dataintnodevector dvec;
		int offset, co;
		BYTE temp;
		CString text, temptext;

		int last = start + count - 1;
		for (co = start; co <= last; co++, ptr += 2) {
			offset = buffer[ptr] + (buffer[ptr + 1] << 8) + ptradd;
			text = "";
			if (showindex) text.AppendFormat("%02X: ", co + 1);

			while (1) {
				temp = buffer[offset];
				if (!temp) break;
				if (table[temp] == "") {
					temptext.Format("{%02X}", temp);
					text += temptext;
				}
				else text += table[temp];
				offset += 1;
			}
			CString xformtext = xform(co, text);
			dvec.push_back({ xformtext, co + 1 });
		}

		return dvec;
	}

	// Pass the 1-based index to this function and it will handle the decrement and call to the underlying entries function.
	datanode<int> LoadExpandedOneBasedEntry(unsigned char* buffer, int index, int ptr, int ptradd, CString table[], bool showindex, StrXform xform)
	{
		ASSERT(index >= 0);
		if (index >= 0) {
			// decrement the 1-based index by 1 to account for the first entry actually being index 0 (there's no "None" entry)
			// step forward by 'index' slots in the 2-byte/record pointer table
			int textindex = index - 1;
			auto indexedptr = ptr + (textindex * 2);
			auto vec = LoadExpandedOneBasedEntries(buffer, indexedptr, ptradd, textindex, 1, table, showindex, xform);
			if (!vec.empty()) return vec.front();
		}
		return{ "<!error!>", -1 };
	}

	dataintnodevector LoadTruncatedDialogueEntries(CFFHacksterProject & proj, int maxlength, bool showindex)
	{
		if (showindex) maxlength += 5; // allow at least a 2 digit index, 2 spaces, and a colon.
		auto xform = [maxlength](int, CString str) -> CString {
			return str.GetLength() < maxlength ? str : str.Left(maxlength - 3) + "...";
		};

		int DIALOGUE_OFFSET = ReadHex(proj.ValuesPath, "DIALOGUE_OFFSET");
		int DIALOGUE_PTRADD = ReadHex(proj.ValuesPath, "DIALOGUE_PTRADD");
		int DIALOGUE_COUNT = ReadDec(proj.ValuesPath, "DIALOGUE_COUNT");
		return LoadExpandedZeroBasedEntries(address(proj.ROM), DIALOGUE_OFFSET, DIALOGUE_PTRADD, 0, DIALOGUE_COUNT, proj.GetTable(8), showindex, xform);
	}

	dataintnode LoadTruncatedDialogueEntry(CFFHacksterProject & proj, int index, int maxlength, bool showindex)
	{
		if (showindex) maxlength += 5; // allow at least a 2 digit index, 2 spaces, and a colon.

		auto xform = [maxlength](int, CString str) -> CString {
			return str.GetLength() < maxlength ? str : str.Left(maxlength - 3) + "...";
		};

		int DIALOGUE_OFFSET = ReadHex(proj.ValuesPath, "DIALOGUE_OFFSET");
		int DIALOGUE_PTRADD = ReadHex(proj.ValuesPath, "DIALOGUE_PTRADD");
		return LoadExpandedZeroBasedEntry(address(proj.ROM), index, DIALOGUE_OFFSET, DIALOGUE_PTRADD, proj.GetTable(8), showindex, xform);
	}

	// CFFHackstrProject-based methods

	dataintnodevector LoadArmorEntries(CFFHacksterProject & proj, bool showindex)
	{
		int ARMORTEXT_OFFSET = ReadHex(proj.ValuesPath, "ARMORTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		int ARMOR_COUNT = ReadDec(proj.ValuesPath, "ARMOR_COUNT");
		return LoadExpandedOneBasedEntries(address(proj.ROM), ARMORTEXT_OFFSET, BASICTEXT_PTRADD, 0, ARMOR_COUNT, proj.GetTable(2), showindex);
	}

	dataintnodevector LoadAttackEntries(CFFHacksterProject & proj, bool showindex)
	{
		int ATTACKTEXT_OFFSET = ReadHex(proj.ValuesPath, "ATTACKTEXT_OFFSET");
		int ATTACKTEXT_PTRADD = ReadHex(proj.ValuesPath, "ATTACKTEXT_PTRADD");
		int ATTACK_COUNT = ReadDec(proj.ValuesPath, "ATTACK_COUNT");
		return LoadExpandedZeroBasedEntries(address(proj.ROM), ATTACKTEXT_OFFSET, ATTACKTEXT_PTRADD, 0, ATTACK_COUNT, proj.GetTable(6), showindex);
	}

	dataintnodevector LoadBattleMessageEntries(CFFHacksterProject & proj, bool showindex)
	{
		int BATTLEMESSAGETEXT_OFFSET = ReadHex(proj.ValuesPath, "BATTLEMESSAGETEXT_OFFSET");
		int BATTLEMESSAGETEXT_PTRADD = ReadHex(proj.ValuesPath, "BATTLEMESSAGETEXT_PTRADD");
		int BATTLEMESSAGE_COUNT = ReadDec(proj.ValuesPath, "BATTLEMESSAGE_COUNT");
		return LoadExpandedZeroBasedEntries(address(proj.ROM), BATTLEMESSAGETEXT_OFFSET, BATTLEMESSAGETEXT_PTRADD, 0, BATTLEMESSAGE_COUNT, proj.GetTable(9), showindex);
	}

	dataintnodevector LoadClassEntries(CFFHacksterProject & proj, bool showindex)
	{
		int CLASSTEXT_OFFSET = ReadHex(proj.ValuesPath, "CLASSTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		int CLASS_COUNT = ReadDec(proj.ValuesPath, "CLASS_COUNT");
		return LoadExpandedZeroBasedEntries(address(proj.ROM), CLASSTEXT_OFFSET, BASICTEXT_PTRADD, 0, CLASS_COUNT, proj.GetTable(5),  showindex);
	}

	dataintnodevector LoadDialogueEntries(CFFHacksterProject & proj, bool showindex)
	{
		int DIALOGUE_OFFSET = ReadHex(proj.ValuesPath, "DIALOGUE_OFFSET");
		int DIALOGUE_PTRADD = ReadHex(proj.ValuesPath, "DIALOGUE_PTRADD");
		int DIALOGUE_COUNT = ReadDec(proj.ValuesPath, "DIALOGUE_COUNT");
		return LoadExpandedZeroBasedEntries(address(proj.ROM), DIALOGUE_OFFSET, DIALOGUE_PTRADD, 0, DIALOGUE_COUNT, proj.GetTable(8), showindex);
	}

	dataintnodevector LoadDialogueShortEntries(CFFHacksterProject & proj, bool showindex)
	{
		int DIALOGUE_OFFSET = ReadHex(proj.ValuesPath, "DIALOGUE_OFFSET");
		int DIALOGUE_PTRADD = ReadHex(proj.ValuesPath, "DIALOGUE_PTRADD");
		int DIALOGUE_COUNT = ReadDec(proj.ValuesPath, "DIALOGUE_COUNT");
		return LoadExpandedZeroBasedEntries(address(proj.ROM), DIALOGUE_OFFSET, DIALOGUE_PTRADD, 0, DIALOGUE_COUNT, proj.GetTable(8), showindex, XformShortString);
	}

	dataintnodevector LoadEnemyEntries(CFFHacksterProject & proj, bool showindex)
	{
		int ENEMYTEXT_OFFSET = ReadHex(proj.ValuesPath, "ENEMYTEXT_OFFSET");
		int ENEMYTEXT_PTRADD = ReadHex(proj.ValuesPath, "ENEMYTEXT_PTRADD");
		int ENEMY_COUNT = ReadDec(proj.ValuesPath, "ENEMY_COUNT");
		return LoadExpandedZeroBasedEntries(address(proj.ROM), ENEMYTEXT_OFFSET, ENEMYTEXT_PTRADD, 0, ENEMY_COUNT, proj.GetTable(7), showindex);
	}

	dataintnodevector LoadGoldEntries(CFFHacksterProject & proj, bool showindex)
	{
		int GOLDITEMTEXT_OFFSET = ReadHex(proj.ValuesPath, "GOLDITEMTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		int GOLDITEM_COUNT = ReadDec(proj.ValuesPath, "GOLDITEM_COUNT");
		return LoadExpandedZeroBasedEntries(address(proj.ROM), GOLDITEMTEXT_OFFSET, BASICTEXT_PTRADD, 0, GOLDITEM_COUNT, proj.GetTable(3), showindex);
	}

	dataintnodevector LoadItemEntries(CFFHacksterProject & proj, bool showindex)
	{
		int BASICTEXT_OFFSET = ReadHex(proj.ValuesPath, "BASICTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		auto ITEMTEXT_OFFSET = BASICTEXT_OFFSET;
		auto ITEMTEXT_PTRADD = BASICTEXT_PTRADD;
		int ITEM_COUNT = ReadDec(proj.ValuesPath, "ITEM_COUNT");
		return LoadExpandedOneBasedEntries(address(proj.ROM), ITEMTEXT_OFFSET, ITEMTEXT_PTRADD, 0, ITEM_COUNT, proj.GetTable(0), showindex);
	}

	dataintnodevector LoadMagicEntries(CFFHacksterProject & proj, bool showindex)
	{
		int MAGICTEXT_OFFSET = ReadHex(proj.ValuesPath, "MAGICTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		int MAGIC_COUNT = ReadDec(proj.ValuesPath, "MAGIC_COUNT");
		return LoadExpandedZeroBasedEntries(address(proj.ROM), MAGICTEXT_OFFSET, BASICTEXT_PTRADD, 0, MAGIC_COUNT, proj.GetTable(4), showindex);
	}

	namespace {
		dataintnode ReadOutOfBattleMagicROMNode(CFFHacksterProject & proj, int mgbase, std::string name, int index, bool showindex)
		{
			int MAGICTEXT_OFFSET = ReadHex(proj.ValuesPath, "MAGICTEXT_OFFSET");
			int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");

			CString text = name.c_str();
			int spindex = ReadHex(proj.ValuesPath, name.c_str());
			if (spindex >= 0) {
				int bytevalue = proj.ROM[spindex];
				int textindex = bytevalue - mgbase;
				text = LoadExpandedZeroBasedEntry(address(proj.ROM), textindex, MAGICTEXT_OFFSET, BASICTEXT_PTRADD, proj.GetTable(4), showindex).name;
			}
			else {
				spindex = mgbase + index;
			}
			return{ text, spindex };
		}

		dataintnodevector LoadOutofBattleMagicROMEntries(CFFHacksterProject & proj, bool showindex)
		{
			int OOBMAGIC_COUNT = ReadDec(proj.ValuesPath, "OOBMAGIC_COUNT");
			int MG_START = ReadHex(proj.ValuesPath, "MG_START");

			auto symnames = ReadIni(proj.ValuesPath, "OOBMAGIC_SYMNAMES", "value", mfcstringvector{});

			ASSERT(OOBMAGIC_COUNT > 0);
			ASSERT(MG_START >= 0);

			dataintnodevector dvec;
			for (size_t st = 0; st < symnames.size(); ++st) {
				auto name = symnames[st];
				auto node = ReadOutOfBattleMagicROMNode(proj, MG_START, (LPCSTR)name, (int)st, showindex);
				dvec.push_back(node);
			}
			return dvec;
		}

		//DEVNOTE - If the index isn't found, default to mgbase + indxx
		dataintnode ReadOutOfBattleMagicAssemblyNode(CFFHacksterProject & proj, int mgbase, std::string name, int index, bool showindex)
		{
			int MAGICTEXT_OFFSET = ReadHex(proj.ValuesPath, "MAGICTEXT_OFFSET");
			int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");

			CString text = name.c_str();
			auto spindex = find_or_default(proj.m_varmap, name, -1);
			if (spindex >= 0) {
				int textindex = spindex - mgbase;
				text = LoadExpandedZeroBasedEntry(address(proj.ROM), textindex, MAGICTEXT_OFFSET, BASICTEXT_PTRADD, proj.GetTable(4), showindex).name;
			}
			else {
				spindex = mgbase + index;
			}
			return{ text, spindex };
		}

		dataintnodevector LoadOutofBattleMagicAssemblyEntries(CFFHacksterProject & proj, bool showindex)
		{
			int MG_START = find_or_default(proj.m_varmap, std::string("MG_START"), -1);
			if (MG_START < 0) {
				ErrorHere << "could not find MG_START, unable to resolve OOBMAGIC entries" << std::endl;
				return{};
			}

			size_t OOBMAGIC_COUNT = (size_t)ReadDec(proj.ValuesPath, "OOBMAGIC_COUNT");

			auto symnames = ReadIni(proj.ValuesPath, "OOBMAGIC_SYMNAMES", "value", stdstringvector());

			ASSERT(OOBMAGIC_COUNT > 0);
			ASSERT(MG_START >= 0);
			ASSERT(symnames.size() == OOBMAGIC_COUNT);
			if (symnames.size() != OOBMAGIC_COUNT || symnames.empty()) {
				ErrorHere << "OOBMAGIC count doesn't match the number of OOBMAGIC names" << std::endl;
				return{};
			}

			dataintnodevector dvec;
			for (size_t st = 0; st < symnames.size(); ++st) {
				auto name = symnames[st];
				auto node = ReadOutOfBattleMagicAssemblyNode(proj, MG_START, name, (int)st, showindex);
				dvec.push_back(node);
			}

			ASSERT(dvec.size() == OOBMAGIC_COUNT);
			return dvec;
		}
	}

	dataintnodevector LoadOutOfBattleMagicEntries(CFFHacksterProject & proj, bool showindex)
	{
		if (proj.IsRom())
			return LoadOutofBattleMagicROMEntries(proj, showindex);
		if (proj.IsAsm())
			return LoadOutofBattleMagicAssemblyEntries(proj, showindex);
		return {};
	}

	dataintnodevector LoadPotionEntries(CFFHacksterProject & proj, bool showindex)
	{
		int BASICTEXT_OFFSET = ReadHex(proj.ValuesPath, "BASICTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		int POTION_COUNT = ReadDec(proj.ValuesPath, "POTION_COUNT");
		int POTIONSTART_INDEX = ReadDec(proj.ValuesPath, "POTIONSTART_INDEX");

		auto POTIONTEXT_OFFSET = BASICTEXT_OFFSET + (POTIONSTART_INDEX * 2);
		auto POTIONTEXT_PTRADD = BASICTEXT_PTRADD;
		return LoadExpandedZeroBasedEntries(address(proj.ROM), POTIONTEXT_OFFSET, POTIONTEXT_PTRADD, 0, POTION_COUNT, proj.GetTable(0), showindex);
	}

	dataintnodevector LoadWeaponEntries(CFFHacksterProject & proj, bool showindex)
	{
		int WEAPONTEXT_OFFSET = ReadHex(proj.ValuesPath, "WEAPONTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		int WEAPON_COUNT = ReadDec(proj.ValuesPath, "WEAPON_COUNT");
		return LoadExpandedOneBasedEntries(address(proj.ROM), WEAPONTEXT_OFFSET, BASICTEXT_PTRADD, 0, WEAPON_COUNT, proj.GetTable(1), showindex);
	}

	dataintnode LoadArmorEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int ARMORTEXT_OFFSET = ReadHex(proj.ValuesPath, "ARMORTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		return LoadExpandedOneBasedEntry(address(proj.ROM), index, ARMORTEXT_OFFSET, BASICTEXT_PTRADD, proj.GetTable(2), showindex);
	}

	dataintnode LoadAttackEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int ATTACKTEXT_OFFSET = ReadHex(proj.ValuesPath, "ATTACKTEXT_OFFSET");
		int ATTACKTEXT_PTRADD = ReadHex(proj.ValuesPath, "ATTACKTEXT_PTRADD");
		return LoadExpandedZeroBasedEntry(address(proj.ROM), index, ATTACKTEXT_OFFSET, ATTACKTEXT_PTRADD, proj.GetTable(6), showindex);
	}

	dataintnode LoadBattleMessageEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int BATTLEMESSAGETEXT_OFFSET = ReadHex(proj.ValuesPath, "BATTLEMESSAGETEXT_OFFSET");
		int BATTLEMESSAGETEXT_PTRADD = ReadHex(proj.ValuesPath, "BATTLEMESSAGETEXT_PTRADD");
		return LoadExpandedZeroBasedEntry(address(proj.ROM), index, BATTLEMESSAGETEXT_OFFSET, BATTLEMESSAGETEXT_PTRADD, proj.GetTable(9), showindex);
	}

	dataintnode LoadClassEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int CLASSTEXT_OFFSET = ReadHex(proj.ValuesPath, "CLASSTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		return LoadExpandedZeroBasedEntry(address(proj.ROM), index, CLASSTEXT_OFFSET, BASICTEXT_PTRADD, proj.GetTable(5), showindex);
	}

	dataintnode LoadDialogueEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int DIALOGUE_OFFSET = ReadHex(proj.ValuesPath, "DIALOGUE_OFFSET");
		int DIALOGUE_PTRADD = ReadHex(proj.ValuesPath, "DIALOGUE_PTRADD");
		return LoadExpandedZeroBasedEntry(address(proj.ROM), index, DIALOGUE_OFFSET, DIALOGUE_PTRADD, proj.GetTable(8), showindex);
	}

	dataintnode LoadDialogueShortEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int DIALOGUE_OFFSET = ReadHex(proj.ValuesPath, "DIALOGUE_OFFSET");
		int DIALOGUE_PTRADD = ReadHex(proj.ValuesPath, "DIALOGUE_PTRADD");
		return LoadExpandedZeroBasedEntry(address(proj.ROM), index, DIALOGUE_OFFSET, DIALOGUE_PTRADD, proj.GetTable(8), showindex, XformShortString);
	}

	dataintnode LoadEnemyEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int ENEMYTEXT_OFFSET = ReadHex(proj.ValuesPath, "ENEMYTEXT_OFFSET");
		int ENEMYTEXT_PTRADD = ReadHex(proj.ValuesPath, "ENEMYTEXT_PTRADD");
		return LoadExpandedZeroBasedEntry(address(proj.ROM), index, ENEMYTEXT_OFFSET, ENEMYTEXT_PTRADD, proj.GetTable(7), showindex);
	}

	dataintnode LoadGoldEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int GOLDITEMTEXT_OFFSET = ReadHex(proj.ValuesPath, "GOLDITEMTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		return LoadExpandedZeroBasedEntry(address(proj.ROM), index, GOLDITEMTEXT_OFFSET, BASICTEXT_PTRADD, proj.GetTable(3), showindex);
	}

	dataintnode LoadItemEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int BASICTEXT_OFFSET = ReadHex(proj.ValuesPath, "BASICTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");

		auto ITEMTEXT_OFFSET = BASICTEXT_OFFSET;
		auto ITEMTEXT_PTRADD = BASICTEXT_PTRADD;
		return LoadExpandedOneBasedEntry(address(proj.ROM), index, ITEMTEXT_OFFSET, ITEMTEXT_PTRADD, proj.GetTable(0), showindex);
	}

	dataintnode LoadItemHardcodedEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int BASICTEXT_OFFSET = ReadHex(proj.ValuesPath, "BASICTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");

		auto ITEMTEXT_OFFSET = BASICTEXT_OFFSET;
		auto ITEMTEXT_PTRADD = BASICTEXT_PTRADD;
		auto node = LoadExpandedOneBasedEntry(address(proj.ROM), index, ITEMTEXT_OFFSET, ITEMTEXT_PTRADD, proj.GetTable(0), showindex);
		return node;
	}

	dataintnode LoadMagicEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int MAGICTEXT_OFFSET = ReadHex(proj.ValuesPath, "MAGICTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		return LoadExpandedZeroBasedEntry(address(proj.ROM), index, MAGICTEXT_OFFSET, BASICTEXT_PTRADD, proj.GetTable(4), showindex);
	}

	dataintnode LoadOutOfBattleMagicEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int OOBMAGIC_COUNT = ReadDec(proj.ValuesPath, "OOBMAGIC_COUNT");
		auto symnames = ReadIni(proj.ValuesPath, "OOBMAGIC_SYMNAMES", "value", stdstringvector{});

		if (index >= OOBMAGIC_COUNT) {
			ErrorHere << "index " << index << " is out of OOBMAGIC range (" << OOBMAGIC_COUNT << ")" << std::endl;
		}
		else if (index < (int)symnames.size()) {
			ErrorHere << "index " << index << " is out of symnames range (" << symnames.size() << ")" << std::endl;
		}
		else if (proj.IsRom()) {
			int MG_START = ReadHex(proj.ValuesPath, "MG_START");
			std::string name = index < (int)symnames.size() ? symnames[index] : "OOBSpell" + std::to_string(index);
			return ReadOutOfBattleMagicROMNode(proj, MG_START, name, index, showindex);
		}
		else if (proj.IsAsm()) {
			int MG_START = find_or_default(proj.m_varmap, std::string("MG_START"), -1);
			std::string name = index < (int)symnames.size() ? symnames[index] : "OOBSpell" + std::to_string(index);
			return ReadOutOfBattleMagicAssemblyNode(proj, MG_START, name, index, showindex);
		}
		else {
			ErrorHere << "unknown project type, can't return OOBMAGIC entry name" << std::endl;
		}
		return { "", -1 };
	}

	dataintnode LoadPotionEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int BASICTEXT_OFFSET = ReadHex(proj.ValuesPath, "BASICTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		int POTIONSTART_INDEX = ReadHex(proj.ValuesPath, "POTIONSTART_INDEX");

		// Only Heal and Pure can be used for magic effects during battle, so only they are represented in this collection.
		//FUTURE - add POTIONTEXT_OFFSET and POTIONTEXT_PTRADD to the values file?
		auto POTIONTEXT_OFFSET = BASICTEXT_OFFSET + (POTIONSTART_INDEX * 2);
		auto POTIONTEXT_PTRADD = BASICTEXT_PTRADD;
		return LoadExpandedZeroBasedEntry(address(proj.ROM), index, POTIONTEXT_OFFSET, POTIONTEXT_PTRADD, proj.GetTable(0), showindex);
	}

	dataintnode LoadWeaponEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		int WEAPONTEXT_OFFSET = ReadHex(proj.ValuesPath, "WEAPONTEXT_OFFSET");
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		// Weapons are range [$01, $28], but there's no string for 0 "None"
		// This would make "Wooden Nunchuk" 0 (instead of 1) and "Masmune" is now 39/$27 (instead of 40/$28)
		// LoadExpandedOneBasedEntry should handle the indexing.
		return LoadExpandedOneBasedEntry(address(proj.ROM), index, WEAPONTEXT_OFFSET, BASICTEXT_PTRADD, proj.GetTable(1), showindex);
	}

	void PasteSwapStringBytes(bool swapping, CFFHacksterProject& proj, int context, int sourceindex, int destindex)
	{
		if (context == INTROTEXT) {
			return; //TODO - or throw? it's not applicable here
		}

		//TODO - read this from shared or context-specific settings (see Values File Split for more info)
		//		and move it to an object that can be passed int.
		int BASICTEXT_PTRADD = ReadHex(proj.ValuesPath, "BASICTEXT_PTRADD");
		int BASICTEXT_OFFSET = ReadHex(proj.ValuesPath, "BASICTEXT_OFFSET");
		int BASICTEXT_COUNT = ReadDec(proj.ValuesPath, "BASICTEXT_COUNT");
		int ITEM_COUNT = ReadDec(proj.ValuesPath, "ITEM_COUNT");
		int WEAPON_COUNT = ReadDec(proj.ValuesPath, "WEAPON_COUNT");
		int ARMOR_COUNT = ReadDec(proj.ValuesPath, "ARMOR_COUNT");
		int GOLDITEM_COUNT = ReadDec(proj.ValuesPath, "GOLDITEM_COUNT");
		int MAGIC_COUNT = ReadDec(proj.ValuesPath, "MAGIC_COUNT");
		int CLASS_COUNT = ReadDec(proj.ValuesPath, "CLASS_COUNT");
		int INTROTEXT_OFFSET = ReadHex(proj.ValuesPath, "INTROTEXT_OFFSET"); // no pointer table, just a single complex string

		// Set up the offsets
		const int ptradd = BASICTEXT_PTRADD;
		int ptroffset = BASICTEXT_OFFSET;
		int count = ITEM_COUNT;
		if (context < ENEMYATTACKS) {
			if (context > 0) { ptroffset += count << 1; count = WEAPON_COUNT; }
			if (context > 1) { ptroffset += count << 1; count = ARMOR_COUNT; }
			if (context > 2) { ptroffset += count << 1; count = GOLDITEM_COUNT; }
			if (context > 3) { ptroffset += count << 1; count = MAGIC_COUNT; }
			if (context > 4) { ptroffset += count << 1; count = CLASS_COUNT; }
		}
		else {
			CString fmt;
			fmt.Format("Can't swap ingame text indexes %d and %d for unrecognized context %d.",
				sourceindex, destindex, context);
			throw std::runtime_error((LPCSTR)fmt);
		}

		const auto ReadBytes = [&](int index) {
			std::vector<unsigned char> bytes;
			size_t offset = (size_t)(ptroffset + (index << 1));
			offset = (size_t)(proj.ROM[offset] + (proj.ROM[offset + 1] << 8) + ptradd);
			for (; proj.ROM[offset]; ++offset)
				bytes.push_back(proj.ROM[offset]);
			return bytes;
		};
		const auto WriteBytes = [&](int index, const std::vector<unsigned char>& bytes) {
			size_t offset = (size_t)(ptroffset + (index << 1));
			offset = (size_t)(proj.ROM[offset] + (proj.ROM[offset + 1] << 8) + ptradd);
			for (size_t i = 0; i < bytes.size(); ++i, ++offset)
				proj.ROM[offset] = bytes[i];
		};

		auto srcbytes = ReadBytes(sourceindex);
		auto dstbytes = ReadBytes(destindex);

			ASSERT(srcbytes.size() == dstbytes.size());
		if (srcbytes.size() != dstbytes.size())
			throw std::runtime_error("Source and dest byte lengths don't match.");

		// We always write srcbytes to the dest index.
		// if swapping, also write dstbytes to the source.
		if (swapping) WriteBytes(sourceindex, dstbytes);
		WriteBytes(destindex, srcbytes);
	}

	void SwapStringBytes(CFFHacksterProject& proj, int context, int sourceindex, int destindex)
	{
		PasteSwapStringBytes(true, proj, context, sourceindex, destindex);
	}

	void OverwriteStringBytes(CFFHacksterProject& proj, int context, int sourceindex, int destindex)
	{
		PasteSwapStringBytes(false, proj, context, sourceindex, destindex);
	}

} // end namespace Ingametext

namespace Ingametext // FFH2Project veersions
{
	// INTERNAL LOADING HELPERS
	namespace {
		dataintnodevector LoadExpandedZeroBasedEntriesEx(FFH2Project& proj,
			std::string ptrname, std::string ptraddname, std::string startname, std::string countname,
			int tableindex, bool showindex, StrXform xform = CStrIdentity)
		{
			DataValueAccessor d(proj);
			unsigned char* buffer = address(proj.ROM);
			int ptr = d.get<int>(ptrname);
			int ptradd = d.get<int>(ptraddname);
			int start = startname.empty() ? 0 : d.get<int>(startname);
			int count = d.get<int>(countname);
			const std::string* table = proj.GetTable(tableindex);

			dataintnodevector dvec;
			int offset, co;
			BYTE temp;
			CString text, temptext;

			int last = start + count - 1;
			for (co = start; co <= last; co++, ptr += 2) {
				offset = buffer[ptr] + (buffer[ptr + 1] << 8) + ptradd;
				text = "";
				if (showindex) text.AppendFormat("%02X: ", co);

				while (1) {
					temp = buffer[offset];
					if (!temp) break;
					if (table[temp] == "") {
						temptext.Format("{%02X}", temp);
						text += temptext;
					}
					else text += table[temp].c_str();
					offset += 1;
				}
				CString xformtext = xform(co, text);
				dvec.push_back({ xformtext, co });
			}

			return dvec;
		}

		dataintnodevector LoadExpandedOneBasedEntriesEx(FFH2Project& proj,
			std::string ptrname, std::string ptraddname, std::string startname, std::string countname,
			int tableindex, bool showindex, StrXform xform = CStrIdentity)
		{
			DataValueAccessor d(proj);
			unsigned char* buffer = address(proj.ROM);
			int ptr = d.get<int>(ptrname);
			int ptradd = d.get<int>(ptraddname);
			int start = startname.empty() ? 0 : d.get<int>(startname);
			int count = d.get<int>(countname);
			const std::string* table = proj.GetTable(tableindex);

			dataintnodevector dvec;
			int offset, co;
			BYTE temp;
			CString text, temptext;

			int last = start + count - 1;
			for (co = start; co <= last; co++, ptr += 2) {
				offset = buffer[ptr] + (buffer[ptr + 1] << 8) + ptradd;
				text = "";
				if (showindex) text.AppendFormat("%02X: ", co + 1);

				while (1) {
					temp = buffer[offset];
					if (!temp) break;
					if (table[temp] == "") {
						temptext.Format("{%02X}", temp);
						text += temptext;
					}
					else {
						text += table[temp].c_str();
					}
					offset += 1;
				}
				CString xformtext = xform(co, text);
				dvec.push_back({ xformtext, co + 1 });
			}

			return dvec;
		}

		// Weapons IDs start with 0 as None, 1 as Wooden Nunchuk, but weapon text strings start with  0 as Wooden Nunchuk.
		// LoadExpandedZeroBasedEntries can't be used here because it will start the index at 0, we need it to start at 1.
		// index has to be passed in the [0,39] range, not [1,40] (at least for now).
		dataintnodevector LoadExpandedOneBasedEntries(FFH2Project& proj, int ptr, int ptradd, int start, int count,
			int tableindex, bool showindex, StrXform xform = CStrIdentity)
		{
			unsigned char* buffer = address(proj.ROM);
			const std::string* table = proj.GetTable(tableindex);

			dataintnodevector dvec;
			int offset, co;
			BYTE temp;
			CString text, temptext;

			int last = start + count - 1;
			for (co = start; co <= last; co++, ptr += 2) {
				offset = buffer[ptr] + (buffer[ptr + 1] << 8) + ptradd;
				text = "";
				if (showindex) text.AppendFormat("%02X: ", co + 1);

				while (1) {
					temp = buffer[offset];
					if (!temp) break;
					if (table[temp] == "") {
						temptext.Format("{%02X}", temp);
						text += temptext;
					}
					else {
						text += table[temp].c_str();
					}
					offset += 1;
				}
				CString xformtext = xform(co, text);
				dvec.push_back({ xformtext, co + 1 });
			}

			return dvec;
		}

		// Pass the 1-based index to this function and it will handle the decrement and call to the underlying entries function.
		datanode<int> LoadExpandedOneBasedEntry(FFH2Project& proj, int index,
			std::string ptrname, std::string ptraddname,
			int tableindex, bool showindex, StrXform xform = CStrIdentity
			//unsigned char* buffer, int index, int ptr, int ptradd, CString table[], bool showindex, StrXform xform
		)
		{
			DataValueAccessor d(proj);
			int ptr = d.get<int>(ptrname);
			int ptradd = d.get<int>(ptraddname);

			ASSERT(index >= 0);
			if (index >= 0) {
				// decrement the 1-based index by 1 to account for the first entry actually being index 0 (there's no "None" entry)
				// step forward by 'index' slots in the 2-byte/record pointer table
				int textindex = index - 1;
				auto indexedptr = ptr + (textindex * 2);
				auto vec = LoadExpandedOneBasedEntries(proj, indexedptr, ptradd, textindex, 1, tableindex, showindex, xform);
				if (!vec.empty()) return vec.front();
			}
			return{ "<!error!>", -1 };
		}

	} // end namespace (unnamed)

	// LABEL LISTS

	dataintnodevector LoadArmorEntries(FFH2Project& proj, bool showindex)
	{
		return LoadExpandedOneBasedEntriesEx(proj, "ARMORTEXT_OFFSET", "BASICTEXT_PTRADD", "", "ARMOR_COUNT", 2, showindex);
	}

	dataintnodevector LoadAttackEntries(FFH2Project& proj, bool showindex)
	{
		return LoadExpandedZeroBasedEntriesEx(proj, "ATTACKTEXT_OFFSET", "ATTACKTEXT_PTRADD", "", "ATTACK_COUNT", 6, showindex);
	}

	dataintnodevector LoadClassEntries(FFH2Project& proj, bool showindex)
	{
		return LoadExpandedZeroBasedEntriesEx(proj, "CLASSTEXT_OFFSET", "BASICTEXT_PTRADD", "", "CLASS_COUNT", 5, showindex);
	}

	dataintnodevector LoadEnemyEntries(FFH2Project& proj, bool showindex)
	{
		return LoadExpandedZeroBasedEntriesEx(proj, "ENEMYTEXT_OFFSET", "ENEMYTEXT_PTRADD", "", "ENEMY_COUNT", 7, showindex);
	}

	dataintnodevector LoadMagicEntries(FFH2Project& proj, bool showindex)
	{
		return LoadExpandedZeroBasedEntriesEx(proj, "MAGICTEXT_OFFSET", "BASICTEXT_PTRADD", "", "MAGIC_COUNT", 4, showindex);
	}


	// SINGLE ENTRIES

	dataintnode LoadArmorEntry(FFH2Project& proj, int index, bool showindex)
	{
		DataValueAccessor d(proj);
		int ARMORTEXT_OFFSET = d.get<int>("ARMORTEXT_OFFSET");
		int BASICTEXT_PTRADD = d.get<int>("BASICTEXT_PTRADD");
		return LoadExpandedOneBasedEntry(proj, index, "ARMORTEXT_OFFSET", "BASICTEXT_PTRADD", 2, showindex);
	}


	void PasteSwapStringBytes(bool swapping, FFH2Project& proj, int context, int sourceindex, int destindex)
	{
		if (context == INTROTEXT) {
			return; //TODO - or throw? it's not applicable here
		}

		DataValueAccessor d(proj);
		int BASICTEXT_PTRADD = d.get<int>("BASICTEXT_PTRADD");
		int BASICTEXT_OFFSET = d.get<int>("BASICTEXT_OFFSET");
		int BASICTEXT_COUNT = d.get<int>("BASICTEXT_COUNT");
		int ITEM_COUNT = d.get<int>("ITEM_COUNT");
		int WEAPON_COUNT = d.get<int>("WEAPON_COUNT");
		int ARMOR_COUNT = d.get<int>("ARMOR_COUNT");
		int GOLDITEM_COUNT = d.get<int>("GOLDITEM_COUNT");
		int MAGIC_COUNT = d.get<int>("MAGIC_COUNT");
		int CLASS_COUNT = d.get<int>("CLASS_COUNT");
		int INTROTEXT_OFFSET = d.get<int>("INTROTEXT_OFFSET"); // no pointer table, just a single complex string

		// Set up the offsets
		const int ptradd = BASICTEXT_PTRADD;
		int ptroffset = BASICTEXT_OFFSET;
		int count = ITEM_COUNT;
		if (context < ENEMYATTACKS) {
			if (context > 0) { ptroffset += count << 1; count = WEAPON_COUNT; }
			if (context > 1) { ptroffset += count << 1; count = ARMOR_COUNT; }
			if (context > 2) { ptroffset += count << 1; count = GOLDITEM_COUNT; }
			if (context > 3) { ptroffset += count << 1; count = MAGIC_COUNT; }
			if (context > 4) { ptroffset += count << 1; count = CLASS_COUNT; }
		}
		else {
			CString fmt;
			fmt.Format("Can't swap ingame text indexes %d and %d for unrecognized context %d.",
				sourceindex, destindex, context);
			throw std::runtime_error((LPCSTR)fmt);
		}

		const auto ReadBytes = [&](int index) {
			std::vector<unsigned char> bytes;
			size_t offset = (size_t)(ptroffset + (index << 1));
			offset = (size_t)(proj.ROM[offset] + (proj.ROM[offset + 1] << 8) + ptradd);
			for (; proj.ROM[offset]; ++offset)
				bytes.push_back(proj.ROM[offset]);
			return bytes;
		};
		const auto WriteBytes = [&](int index, const std::vector<unsigned char>& bytes) {
			size_t offset = (size_t)(ptroffset + (index << 1));
			offset = (size_t)(proj.ROM[offset] + (proj.ROM[offset + 1] << 8) + ptradd);
			for (size_t i = 0; i < bytes.size(); ++i, ++offset)
				proj.ROM[offset] = bytes[i];
		};

		auto srcbytes = ReadBytes(sourceindex);
		auto dstbytes = ReadBytes(destindex);

		ASSERT(srcbytes.size() == dstbytes.size());
		if (srcbytes.size() != dstbytes.size())
			throw std::runtime_error("Source and dest byte lengths don't match.");

		// We always write srcbytes to the dest index.
		// if swapping, also write dstbytes to the source.
		if (swapping) WriteBytes(sourceindex, dstbytes);
		WriteBytes(destindex, srcbytes);
	}

	void SwapStringBytes(FFH2Project& proj, int context, int sourceindex, int destindex)
	{
		PasteSwapStringBytes(true, proj, context, sourceindex, destindex);
	}

	void OverwriteStringBytes(FFH2Project& proj, int context, int sourceindex, int destindex)
	{
		PasteSwapStringBytes(false, proj, context, sourceindex, destindex);
	}

} // end namespace Ingametext // FFH2Project veersions