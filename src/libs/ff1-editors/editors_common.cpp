#include "stdafx.h"
#include "editors_common.h"

#include "logging_functions.h"
#include "FFHacksterProject.h" //REMOVE
#include <FFH2Project.h>
#include "SpriteDialogueSettings.h"
#include "editor_label_functions.h"
#include "ingame_text_functions.h"

namespace Editors
{

	using namespace Editorlabels;
	using namespace Ingametext;

	dataintnodevector LoadGameTextEntries(CFFHacksterProject & proj, bool showindex)
	{
		CSpriteDialogueSettings stgs(proj);
		return stgs.ShowActualText ?
			LoadTruncatedDialogueEntries(proj, stgs.ShortTextLength, showindex):
			LoadTextLabels(proj, showindex);
	}

	dataintnode LoadGameTextEntry(CFFHacksterProject & proj, int index, bool showindex)
	{
		CSpriteDialogueSettings stgs(proj);
		return stgs.ShowActualText ?
			LoadTruncatedDialogueEntry(proj, index, stgs.ShortTextLength, showindex) :
			LoadTextLabel(proj, index, showindex);
	}

	dataintnodevector LoadGameTextEntries(FFH2Project& proj, bool showindex)
	{
		CSpriteDialogueSettings stgs(proj);
		return stgs.ShowActualText ?
			LoadTruncatedDialogueEntries(proj, stgs.ShortTextLength, showindex):
			Labels2::LoadTextLabels(proj, showindex);
	}

	dataintnode LoadGameTextEntry(FFH2Project& proj, int index, bool showindex)
	{
		CSpriteDialogueSettings stgs(proj);
		return stgs.ShowActualText ?
			LoadTruncatedDialogueEntry(proj, index, stgs.ShortTextLength, showindex) :
			Labels2::LoadTextLabel(proj, index, showindex);
	}
}


namespace Maps {

	void DecompressMap(CFFHacksterProject& proj, int mapindex, int mapbase, int ptradd, BYTE DecompressedMap[0x40][0x40])
	{
		DecompressMap(proj.ROM, mapindex, mapbase, ptradd, DecompressedMap);
	}

	void DecompressMap(FFH2Project& proj, int mapindex, int mapbase, int ptradd, BYTE DecompressedMap[0x40][0x40])
	{
		DecompressMap(proj.ROM, mapindex, mapbase, ptradd, DecompressedMap);
	}

	//DEVNOTE - currently uses hardcoded dimensions of 64 cols x 64 rows
	void DecompressMap(std::vector<unsigned char>& rom, int mapindex, int mapbase, int ptradd, BYTE DecompressedMap[0x40][0x40])
	{
		//memset(DecompressedMap, 0, 0x40 * 0x40 * sizeof(DecompressedMap[0]));

		auto offset = mapbase + (mapindex << 1);
		offset = rom[offset] + (rom[offset + 1] << 8) + ptradd;
		int ThisRun, RunLength;
		BYTE temp;
		int coY, coX;

		ThisRun = 0; RunLength = 0;
		bool stop = 0;
		for (coY = 0; coY < 64 && !stop; coY++) {
			for (coX = 0; coX < 64 && !stop; coX++) {
				if (!RunLength) {
					temp = rom[offset];
					if (temp == 0xFF) stop = 1;
					if (temp < 0x80) {
						RunLength = 1; ThisRun = temp; offset += 1;
					}
					else {
						RunLength = rom[offset + 1];
						ThisRun = temp - 0x80;
						offset += 2;
						if (RunLength == 0xFF) stop = 1;
						if (RunLength == 0x00) RunLength = 0x100;
					}
				}
				DecompressedMap[coY][coX] = (BYTE)ThisRun;
				RunLength -= 1;
			}
		}
	}

}