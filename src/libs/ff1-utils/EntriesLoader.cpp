#include "stdafx.h"
#include "EntriesLoader.h"
#include <FFHacksterProject.h>
#include "ingame_text_functions.h"
#include <ini_functions.h>

using namespace Ingametext;
using namespace Ini;

CEntriesLoader::CEntriesLoader(CFFHacksterProject & proj)
	: m_proj(proj)
{
}

CEntriesLoader::~CEntriesLoader()
{
}

void CEntriesLoader::Init()
{
}

dataintnodevector CEntriesLoader::LoadTruncatedDialogueEntries(CFFHacksterProject & proj, int maxlength, bool showindex)
{
	return Ingametext::LoadTruncatedDialogueEntries(proj, maxlength, showindex);
}

dataintnodevector CEntriesLoader::LoadArmorEntries(CFFHacksterProject & proj, bool showindex)
{
	return Ingametext::LoadArmorEntries(proj, showindex);
}

dataintnodevector CEntriesLoader::LoadAttackEntries(CFFHacksterProject & proj, bool showindex)
{
	return Ingametext::LoadAttackEntries(proj, showindex);
}

dataintnodevector CEntriesLoader::LoadBattleMessageEntries(CFFHacksterProject & proj, bool showindex)
{
	return Ingametext::LoadBattleMessageEntries(proj, showindex);
}

dataintnodevector CEntriesLoader::LoadClassEntries(CFFHacksterProject & proj, bool showindex)
{
	return Ingametext::LoadClassEntries(proj, showindex);
}

dataintnodevector CEntriesLoader::LoadDialogueEntries(CFFHacksterProject & proj, bool showindex)
{
	return Ingametext::LoadDialogueEntries(proj, showindex);
}

dataintnodevector CEntriesLoader::LoadDialogueShortEntries(CFFHacksterProject & proj, bool showindex)
{
	return Ingametext::LoadDialogueShortEntries(proj, showindex);
}

dataintnodevector CEntriesLoader::LoadEnemyEntries(CFFHacksterProject & proj, bool showindex)
{
	return Ingametext::LoadEnemyEntries(proj, showindex);
}

dataintnodevector CEntriesLoader::LoadGoldEntries(CFFHacksterProject & proj, bool showindex)
{
	return Ingametext::LoadGoldEntries(proj, showindex);
}

dataintnodevector CEntriesLoader::LoadItemEntries(CFFHacksterProject & proj, bool showindex)
{
	return Ingametext::LoadItemEntries(proj, showindex);
}

dataintnodevector CEntriesLoader::LoadMagicEntries(CFFHacksterProject & proj, bool showindex)
{
	return Ingametext::LoadMagicEntries(proj, showindex);
}

dataintnodevector CEntriesLoader::LoadOutOfBattleMagicEntries(CFFHacksterProject & proj, bool showindex)
{
	return Ingametext::LoadOutOfBattleMagicEntries(proj, showindex);
}

dataintnodevector CEntriesLoader::LoadPotionEntries(CFFHacksterProject & proj, bool showindex)
{
	return Ingametext::LoadPotionEntries(proj, showindex);
}

dataintnodevector CEntriesLoader::LoadWeaponEntries(CFFHacksterProject & proj, bool showindex)
{
	return Ingametext::LoadWeaponEntries(proj, showindex);
}

dataintnodevector CEntriesLoader::LoadTreasureItemEntries(CFFHacksterProject & proj, bool showindex)
{
	auto entries = LoadItemEntries(proj, showindex) + LoadWeaponEntries(proj, showindex) + LoadArmorEntries(proj, showindex) +
		LoadGoldEntries(proj, showindex) + LoadMagicEntries(proj, showindex);

	// We have to renumber this list; it's currently multiple concatenated sublists, each starting at 0.
	for (int i = 0; i < (int)entries.size(); ++i) {
		entries[i].value = i;
	}

	return entries;
}



dataintnode CEntriesLoader::LoadTruncatedDialogueEntry(CFFHacksterProject & proj, int index, int maxlength, bool showindex)
{
	return Ingametext::LoadTruncatedDialogueEntry(proj, index, maxlength, showindex);
}

dataintnode CEntriesLoader::LoadArmorEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadArmorEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadAttackEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadAttackEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadBattleMessageEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadBattleMessageEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadClassEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadClassEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadDialogueEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadDialogueEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadDialogueShortEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadDialogueShortEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadEnemyEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadEnemyEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadGoldEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadGoldEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadItemEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadItemEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadItemHardcodedEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadItemHardcodedEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadMagicEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadMagicEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadOutOfBattleMagicEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadOutOfBattleMagicEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadPotionEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadPotionEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadWeaponEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	return Ingametext::LoadWeaponEntry(proj, index, showindex);
}

dataintnode CEntriesLoader::LoadTreasureItemEntry(CFFHacksterProject & proj, int index, bool showindex)
{
	if (index < 0)
		THROWEXCEPTION(std::invalid_argument, std::to_string(index) + " is below the range of treasure item entries");

	auto ITEM_COUNT = ReadDec(proj.ValuesPath, "ITEM_COUNT");
	if (index < ITEM_COUNT) return LoadItemEntry(proj, index, showindex);

	auto base = ITEM_COUNT;
	auto WEAPON_COUNT = ReadDec(proj.ValuesPath, "WEAPON_COUNT");
	if (index < WEAPON_COUNT) return LoadWeaponEntry(proj, index - base, showindex);

	base += WEAPON_COUNT;
	auto ARMOR_COUNT = ReadDec(proj.ValuesPath, "ARMOR_COUNT");
	if (index < ARMOR_COUNT) return LoadArmorEntry(proj, index - base, showindex);

	base += ARMOR_COUNT;
	auto GOLDITEM_COUNT = ReadDec(proj.ValuesPath, "GOLDITEM_COUNT");
	if (index < GOLDITEM_COUNT) return LoadGoldEntry(proj, index - base, showindex);

	base += GOLDITEM_COUNT;
	auto MAGIC_COUNT = ReadDec(proj.ValuesPath, "MAGIC_COUNT");
	if (index < MAGIC_COUNT) return LoadMagicEntry(proj, index - base, showindex);

	THROWEXCEPTION(std::invalid_argument, std::to_string(index) + " is beyond the range of treasure item entries");
}