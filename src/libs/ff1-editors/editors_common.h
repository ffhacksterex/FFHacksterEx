#pragma once

#include <datanode.h>
#include <vector>
class CFFHacksterProject;

// These functions build on those declared in ff1-utils, and are specifically for use in this library.
namespace Editors
{
	dataintnodevector LoadGameTextEntries(CFFHacksterProject & proj, bool showindex = false);
	dataintnode LoadGameTextEntry(CFFHacksterProject & proj, int index, bool showindex = false);
}

namespace Maps
{
	void DecompressMap(CFFHacksterProject& proj, int mapindex, int mapbase, int ptradd, BYTE DecompressedMap[0x40][0x40]);
}
