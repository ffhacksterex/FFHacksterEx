#pragma once

#include "vector_types.h"
#include "map_types.h"
#include "datanode.h"
#include <map>
class CFFHacksterProject;

namespace Ramvalues
{
	struct ramvalue //FUTURE - this is essentially a dataintnode. change it to be dataintnode to make some calls more compatible?
					//			that might eliminate the need for some of the Ramvalues types and functions.
	{
		CString name;
		int addr;
	};

	using ramvaluevector = std::vector<ramvalue>;
	using ramvaluemap = std::map<decltype(ramvalue::name), decltype(ramvalue::addr)>;

	int ReadRamAddress(CFFHacksterProject & proj, CString key);

	ramvaluevector ReadRamValuesToVector(CFFHacksterProject & proj, const std::vector<CString> & sectionnames);
	ramvaluevector ReadRamValuesToVector(CFFHacksterProject & proj, CString key);

	CString GetItemFallbackName(CFFHacksterProject& proj, CString name, int itemaddr);

	void AdjustRamItemEntries(CFFHacksterProject & proj, dataintnodevector & nodes, CString indexformat, int adjustby);


	// ### STRINGS

	dataintnodevector LoadMapSpriteEntries(CFFHacksterProject& proj, bool showindex = false);
	dataintnodevector LoadItemOrCanoeEntries(CFFHacksterProject& proj, bool showindex = false);
	dataintnodevector LoadDialogSoundEntries(CFFHacksterProject& proj, bool showindex = false);
	dataintnodevector LoadCanoeEntries(CFFHacksterProject& proj, bool showindex = false);


	// ### SINGLE STRINGS

	dataintnode LoadMapSpriteEntry(CFFHacksterProject& proj, int value, bool showindex = false);

}