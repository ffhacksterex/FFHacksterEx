#include "stdafx.h"
#include "upgrade_functions.h"
#include "ini_functions.h"
#include "io_functions.h"
#include "path_functions.h"
#include "string_functions.h"
#include "type_support.h"

#include "DirPathRemover.h"
#include "Editors2.h"
#include "FFHacksterProject.h"
#include "Upgrades_ini.h"

#include <sstream>
#include <fstream>

using namespace Io;
using namespace Paths;
using namespace Strings;


namespace Upgrades
{
	// PUBLIC IMPLEMENTATION

	bool NeedsConversion(CString projectpath)
	{
		//TODO - do the following:
		//	needs conversion if it's INI format, regardless of the version.
		if (IsIniFormat(projectpath)) {
			return Upgrades_ini::NeedsConversion(projectpath);
		}
		else {
			//TODO - changethis to a JSON upgrade check once we start supporting JSON
			//int curver = ExtractJsonVersion(projectpath);
			//return (curver < CURRENT_SEMVER);
			throw std::runtime_error("Project types other than INI are not currently supported.");
		}

		// So should look like this when done
		// return IniFormat ? true : jsonNeedsCOnversion()
	}

	pair_result<CString> PrepareProjectUpgrade(CString projectini)
	{
		// Copy the top-level files in this folder to a temp folder.
		// If the function fails, delete the temp folder before leaving this function.
		auto tempfolder = Paths::BuildTempFilePath() + "ffh";
		if (!Paths::DirCreate(tempfolder))
			return{ false, "Can't back up project to temp folder before upgrading" };

		DirPathRemover remover(tempfolder);
		CString projectfolder = Paths::GetDirectoryPath(projectini);
		if (!Paths::DirShallowCopy(projectfolder, tempfolder))
			return{ false, "Can't copy project files to project upgrade backup storage" };

		CString tempini = Paths::Combine({ tempfolder, Paths::GetFileName(projectini) });
		if (!Paths::FileExists(tempini))
			return{ false, "The working copy of the project to upgrade was not successfully created" };

		remover.Revoke();
		return{ true, tempini };
	}

	pair_result<CString> CommitProjectUpgrade(CString projectfolder, CString tempfolder)
	{
		// Bring back the folders that have changed
		if (!Paths::DirShallowUpdate(tempfolder, projectfolder))
			return{ false, "Unable to copy upgrade files back to the project" };

		return{ true, "" };
	}

	//pair_result<CString> UpgradeProject(CString projectpath)
	//{
	//	if (!Paths::FileExists(projectpath))
	//		return{ false, "The project file can't be upgraded because it wasn't found" };

	//	if (IsIniFormat(projectpath)) {
	//		return Upgrades_ini::UpgradeProject(projectpath);
	//	}
	//	else {
	//		throw std::runtime_error("Not implemented yet");
	//	}
	//}

	[[deprecated]]
	version_result UpgradeProject(CString projectpath)
	{
		throw std::runtime_error("Don't call " __FUNCTION__);
		//if (!Paths::FileExists(projectpath))
		//	return{ versionfailure, "The project file can't be upgraded because it wasn't found" };

		//if (IsIniFormat(projectpath)) {
		//	auto result = Upgrades_ini::UpgradeProject(projectpath);
		//	return version_result{ result ? versionok : versionfailure, result.value };
		//}
		//else {
		//	throw std::runtime_error("Not implemented yet");
		//}
	}

} // end namespace Upgrades