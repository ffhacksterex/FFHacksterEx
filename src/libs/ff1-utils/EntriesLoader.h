#pragma once

#include "datanode.h"
class CFFHacksterProject;
class FFH2Project;

class CEntriesLoader
{
public:
	CEntriesLoader(CFFHacksterProject & proj);
	CEntriesLoader(FFH2Project& proj);
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

	// FFH2project versions
	virtual dataintnodevector LoadTruncatedDialogueEntries(FFH2Project& proj, int maxlength, bool showindex = false);
	virtual dataintnodevector LoadArmorEntries(FFH2Project& proj, bool showindex = false);
	virtual dataintnodevector LoadAttackEntries(FFH2Project& proj, bool showindex = false);
	virtual dataintnodevector LoadBattleMessageEntries(FFH2Project& proj, bool showindex = false);
	virtual dataintnodevector LoadClassEntries(FFH2Project& proj, bool showindex = false);
	virtual dataintnodevector LoadDialogueEntries(FFH2Project& proj, bool showindex = false);
	virtual dataintnodevector LoadDialogueShortEntries(FFH2Project& proj, bool showindex = false);
	virtual dataintnodevector LoadEnemyEntries(FFH2Project& proj, bool showindex = false);
	virtual dataintnodevector LoadGoldEntries(FFH2Project& proj, bool showindex = false);
	virtual dataintnodevector LoadItemEntries(FFH2Project& proj, bool showindex = false);
	virtual dataintnodevector LoadMagicEntries(FFH2Project& proj, bool showindex = false);
	virtual dataintnodevector LoadOutOfBattleMagicEntries(FFH2Project& proj, bool showindex = false);
	virtual dataintnodevector LoadPotionEntries(FFH2Project& proj, bool showindex = false);
	virtual dataintnodevector LoadWeaponEntries(FFH2Project& proj, bool showindex = false);
	virtual dataintnodevector LoadTreasureItemEntries(FFH2Project& proj, bool showindex = false);

	virtual dataintnode LoadTruncatedDialogueEntry(FFH2Project& proj, int index, int maxlength, bool showindex = false);
	virtual dataintnode LoadArmorEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadAttackEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadBattleMessageEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadClassEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadDialogueEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadDialogueShortEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadEnemyEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadGoldEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadItemEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadItemHardcodedEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadMagicEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadOutOfBattleMagicEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadPotionEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadWeaponEntry(FFH2Project& proj, int index, bool showindex = false);
	virtual dataintnode LoadTreasureItemEntry(FFH2Project& proj, int index, bool showindex = false);

protected:
	CFFHacksterProject & m_proj;
	FFH2Project& m_prj2;
};
