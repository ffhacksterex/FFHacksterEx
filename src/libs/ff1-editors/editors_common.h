#pragma once

#include <datanode.h>
#include <vector>
class CFFHacksterProject;
class FFH2Project;

// These functions build on those declared in ff1-utils, and are specifically for use in this library.
namespace Editors
{
	dataintnodevector LoadGameTextEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnode LoadGameTextEntry(CFFHacksterProject & proj, int index, bool showindex = false);

	dataintnodevector LoadGameTextEntries(FFH2Project& proj, bool showindex = false);
	dataintnode LoadGameTextEntry(FFH2Project& proj, int index, bool showindex = false);
}

namespace Maps
{
	void DecompressMap(CFFHacksterProject& proj, int mapindex, int mapbase, int ptradd, BYTE DecompressedMap[0x40][0x40]);
	void DecompressMap(FFH2Project& proj, int mapindex, int mapbase, int ptradd, BYTE DecompressedMap[0x40][0x40]);
	void DecompressMap(std::vector<unsigned char>& rom, int mapindex, int mapbase, int ptradd, BYTE DecompressedMap[0x40][0x40]);
}
