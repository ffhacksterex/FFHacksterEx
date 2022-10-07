#pragma once

#include <vector>
#include "datanode.h"
#include <function_types.h>
class CListBox;
class CComboBox;
class CFFHacksterProject;

namespace Editorlabels
{
	// ################################################################
	// COLLECTION READERS

	dataintnodevector LoadOnScreenSpriteLabels(CFFHacksterProject & proj, bool showindex = false);

	dataintnodevector LoadWepMagicLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadAILabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadShopLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadBattlePatternTableLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadBackdropLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadONTeleportLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadNNTeleportLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadNOTeleportLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadTreasureLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadMapLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadTilesetLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadSpriteLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadSpriteGraphicLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadBattleLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadBattleLabelsEx(CFFHacksterProject& proj, bool form2only, bool showindex = false);
	dataintnodevector LoadTextLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadSpecialTileLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadMiscCoordLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadAilEffectLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadElementLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadEnemyCategoryLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadArmorTypes(CFFHacksterProject& proj, bool showindex = false);

	dataintnodevector LoadFanfareLabels(CFFHacksterProject & proj, bool showindex = false);
	dataintnodevector LoadFanfareTrueFalseLabels(CFFHacksterProject & proj, bool showindex = false);



	// ################################################################
	// SINGLE ENTRY READERS

	dataintnode LoadOnScreenSpriteLabel(CFFHacksterProject & proj, int index, bool showindex = false);

	dataintnode LoadWepMagicLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadAILabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadShopLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadBattlePatternTableLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadBackdropLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadONTeleportLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadNNTeleportLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadNOTeleportLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadTreasureLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadMapLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadTilesetLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadSpriteLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadSpriteGraphicLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadBattleLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadBattleLabelEx(CFFHacksterProject& proj, int index, bool showindex = false);
	dataintnode LoadTextLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadSpecialTileLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadMiscCoordLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadAilEffectLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadElementLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadEnemyCategoryLabel(CFFHacksterProject & proj, int index, bool showindex = false);
	dataintnode LoadArmorTypeLabel(CFFHacksterProject& proj, int index, bool showindex = false);

	dataintnode LoadFanfareLabel(CFFHacksterProject & proj, int index, bool showindex = false);



	// ################################################################
	// SINGLE ENTRY WRITERS

	void WriteWepMagicLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteAILabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteShopLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteBattlePatternTableLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteBackdropLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteONTeleportLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteNNTeleportLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteNOTeleportLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteTreasureLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteMapLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteTilesetLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteSpriteLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteSpriteGraphicLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteBattleLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteTextLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteSpecialTileLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteMiscCoordLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteAilEffectLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteElementLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteEnemyCategoryLabel(CFFHacksterProject & proj, int index, CString newvalue);
	void WriteArmorTypeLabel(CFFHacksterProject& proj, int index, CString newvalue);
}
