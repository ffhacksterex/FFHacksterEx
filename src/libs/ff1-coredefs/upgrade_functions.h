#pragma once

#include "pair_result.h"

namespace Upgrades
{
	enum version_continuecode { versionok, formatchanged, needsconversion, versionfailure };
	using version_result = std::pair<version_continuecode, CString>;

	bool NeedsConversion(CString projectpath);
	pair_result<CString> PrepareProjectUpgrade(CString projectini);
	pair_result<CString> CommitProjectUpgrade(CString projectfolder, CString tempfolder);

	bool IsIniFormat(CString projectpath); //TODO - make this internal?
	std::string IniToJson(CString inipath); //TODO - move to a private class, similar to Upgrades_ini
}
