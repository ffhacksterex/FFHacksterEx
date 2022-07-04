#pragma once

#include <functional>
#include "datanode.h"
class CListBox;
class CComboBox;
class CFFHacksterProject;

namespace Ingametext
{

	using StrXform = std::function<CString(int, CString)>;
	CString CStrIdentity(int index, CString str);

	CString PutHexToList(CFFHacksterProject* cart, int ptr, int ptradd, int count, bool DTE, CListBox* m_list, CComboBox* m_combo);

	dataintnodevector LoadExpandedZeroBasedEntries(unsigned char* buffer, int ptr, int ptradd, int start, int count, CString table[], bool showindex, StrXform xform = CStrIdentity);
	datanode<int> LoadExpandedZeroBasedEntry(unsigned char* buffer, int index, int ptr, int ptradd, CString table[], bool showindex, StrXform xform = CStrIdentity);
	dataintnodevector LoadExpandedOneBasedEntries(unsigned char* buffer, int ptr, int ptradd, int start, int count, CString table[], bool showindex, StrXform xform = CStrIdentity);
	datanode<int> LoadExpandedOneBasedEntry(unsigned char* buffer, int index, int ptr, int ptradd, CString table[], bool showindex, StrXform xform = CStrIdentity);

	dataintnodevector LoadTruncatedDialogueEntries(CFFHacksterProject & proj, int maxlength, bool showindex = false);
	dataintnode LoadTruncatedDialogueEntry(CFFHacksterProject & proj, int index, int maxlength, bool showindex = false);

	dataintnodevector LoadArmorEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadAttackEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadBattleMessageEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadClassEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadDialogueEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadDialogueShortEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadEnemyEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadGoldEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadItemEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadMagicEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadOutOfBattleMagicEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadPotionEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadWeaponEntries(CFFHacksterProject & proj, bool showindex = false);

	dataintnode LoadArmorEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadAttackEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadBattleMessageEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadClassEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadDialogueEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadDialogueShortEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadEnemyEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadGoldEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadItemEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadItemHardcodedEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadMagicEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadOutOfBattleMagicEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadPotionEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadWeaponEntry(CFFHacksterProject & proj, int index, bool showindex = false);

} // end namespace Ingametext
