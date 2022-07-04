#pragma once

#include "datanode.h"
class CFFHacksterProject;

class CEntriesLoader
{
public:
	CEntriesLoader(CFFHacksterProject & proj);
	virtual ~CEntriesLoader();

	virtual void Init();

	virtual dataintnodevector LoadTruncatedDialogueEntries(CFFHacksterProject & proj, int maxlength, bool showindex = false);
	virtual dataintnodevector LoadArmorEntries(CFFHacksterProject & proj, bool showindex = false);
	virtual dataintnodevector LoadAttackEntries(CFFHacksterProject & proj, bool showindex = false);
	virtual dataintnodevector LoadBattleMessageEntries(CFFHacksterProject & proj, bool showindex = false);
	virtual dataintnodevector LoadClassEntries(CFFHacksterProject & proj, bool showindex = false);
	virtual dataintnodevector LoadDialogueEntries(CFFHacksterProject & proj, bool showindex = false);
	virtual dataintnodevector LoadDialogueShortEntries(CFFHacksterProject & proj, bool showindex = false);
	virtual dataintnodevector LoadEnemyEntries(CFFHacksterProject & proj, bool showindex = false);
	virtual dataintnodevector LoadGoldEntries(CFFHacksterProject & proj, bool showindex = false);
	virtual dataintnodevector LoadItemEntries(CFFHacksterProject & proj, bool showindex = false);
	virtual dataintnodevector LoadMagicEntries(CFFHacksterProject & proj, bool showindex = false);
	virtual dataintnodevector LoadOutOfBattleMagicEntries(CFFHacksterProject & proj, bool showindex = false);
	virtual dataintnodevector LoadPotionEntries(CFFHacksterProject & proj, bool showindex = false);
	virtual dataintnodevector LoadWeaponEntries(CFFHacksterProject & proj, bool showindex = false);
	virtual dataintnodevector LoadTreasureItemEntries(CFFHacksterProject & proj, bool showindex = false);

	virtual dataintnode LoadTruncatedDialogueEntry(CFFHacksterProject & proj, int index, int maxlength, bool showindex = false);
	virtual dataintnode LoadArmorEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadAttackEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadBattleMessageEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadClassEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadDialogueEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadDialogueShortEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadEnemyEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadGoldEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadItemEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadItemHardcodedEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadMagicEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadOutOfBattleMagicEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadPotionEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadWeaponEntry(CFFHacksterProject & proj, int index, bool showindex = false);
	virtual dataintnode LoadTreasureItemEntry(CFFHacksterProject & proj, int index, bool showindex = false);

protected:
	CFFHacksterProject & m_proj;
};
