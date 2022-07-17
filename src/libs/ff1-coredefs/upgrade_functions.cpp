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

#include <sstream>
#include <fstream>

using namespace Io;
using namespace Ini;
using namespace Paths;
using namespace Strings;
using namespace Types;


namespace Upgrades
{
	// LOCAL HELPER DECLARATIONS
	namespace {
		enum version_continuecode { versionok, needsconversion, versionfailure };
		using version_result = std::pair<version_continuecode, CString>;
		using value_entry = std::tuple<CString, int, CString, CString>;

		pair_result<CString> PrepareProjectUpgrade(CString projectini);
		pair_result<CString> CommitProjectUpgrade(CString projectfolder, CString tempfolder);
		pair_result<CString> AddTypeToArray(CString arraytext);

		version_result PreConversionCheck(CString inipath, int sourceversion, int targetversion);

		pair_result<CString> AddValuesToKeys(CString projectini, const std::vector<value_entry> & newentries);
		pair_result<CString> AddLabels(CString stringsini, CString section, const mfcstringvector & strings);
	}

	pair_result<CString> UpgradeProject_none_to_1(int oldver, int newver, CString projectfolder, CString projectini);
	pair_result<CString> UpgradeProject_1_to_2(int oldver, int newver, CString projectfolder, CString projectini);
	pair_result<CString> UpgradeProject_2_to_3(int oldver, int newver, CString projectfolder, CString projectini);
	pair_result<CString> UpgradeProject_3_to_4(int oldver, int newver, CString projectfolder, CString projectini);
	pair_result<CString> UpgradeProject_4_to_5(int oldver, int newver, CString projectfolder, CString projectini);
	pair_result<CString> UpgradeProject_5_to_6(int oldver, int newver, CString projectfolder, CString projectini);
	pair_result<CString> UpgradeProject_6_to_7(int oldver, int newver, CString projectfolder, CString projectini);
	pair_result<CString> UpgradeProject_7_to_8(int oldver, int newver, CString projectfolder, CString projectini);
	pair_result<CString> UpgradeProject_8_to_9(int oldver, int newver, CString projectfolder, CString projectini);
	pair_result<CString> UpgradeProject_9_to_10(int oldver, int newver, CString projectfolder, CString projectini);
	pair_result<CString> UpgradeProject_10_to_971(int oldver, int newver, CString projectfolder, CString projectini);


	// PUBLIC IMPLEMENTATION

	const int CURRENT_VERSION = CFFHacksterProject::Version;

	bool NeedsConversion(CString projectpath)
	{
		int curver = CFFHacksterProject::ReadProjectVersion(projectpath);
		return (curver < CURRENT_VERSION);
	}

	pair_result<CString> UpgradeProject(CString projectpath)
	{
		if (!Paths::FileExists(projectpath))
			return{ false, "The project file can't be upgraded because it wasn't found" };

		// If the versions match, no conversion is needed, but newer files can't be supported.
		int curver = CFFHacksterProject::ReadProjectVersion(projectpath);
		if (curver == CURRENT_VERSION)
			return{ true, "" };

		if (curver > CURRENT_VERSION) {
			CString msg;
			msg.Format("Version %d is newer than the latest version than this program can support (%d).", curver, CURRENT_VERSION);
			return{ false, msg };
		}

		// We need to convert, back up the project and restore if successful (wipe out the backup in any case).
		CString projectfolder = Paths::GetDirectoryPath(projectpath);

		auto prepresult = PrepareProjectUpgrade(projectpath);
		if (!prepresult) return{ false, prepresult.value };

		auto tempini = prepresult.value;
		auto tempfolder = Paths::GetDirectoryPath(tempini);
		DirPathRemover remover(tempfolder);

		const auto DoProjectUpgrade = [](int oldver, int newver, CString projectfolder, CString projectini, auto func) -> pair_result<CString> {
			auto verresult = PreConversionCheck(projectini, oldver, newver);
			if (verresult.first != needsconversion)
				return{ verresult.first == versionok, verresult.first == versionok ? "" : verresult.second };

			auto converesult = func(oldver, newver, projectfolder, projectini);
			if (converesult)
				CFFHacksterProject::WriteProjectVersion(projectini, newver);
			return converesult;
		};

		auto errlogpath = Paths::Combine({ tempfolder, Paths::GetFileStem(projectpath) + ".upgrade-err.txt" });
		if (Paths::FileExists(errlogpath)) Paths::FileDelete(errlogpath);

		//N.B. - the initial alphas didn't have a version, so they'll return source version -1
		auto upgresult = DoProjectUpgrade(-1, 1, tempfolder, tempini, UpgradeProject_none_to_1);
		if (upgresult)
			upgresult = DoProjectUpgrade(1, 2, tempfolder, tempini, UpgradeProject_1_to_2);
		if (upgresult)
			upgresult = DoProjectUpgrade(2, 3, tempfolder, tempini, UpgradeProject_2_to_3);
		if (upgresult)
			upgresult = DoProjectUpgrade(3, 4, tempfolder, tempini, UpgradeProject_3_to_4);
		if (upgresult)
			upgresult = DoProjectUpgrade(4, 5, tempfolder, tempini, UpgradeProject_4_to_5);
		if (upgresult)
			upgresult = DoProjectUpgrade(5, 6, tempfolder, tempini, UpgradeProject_5_to_6);
		if (upgresult)
			upgresult = DoProjectUpgrade(6, 7, tempfolder, tempini, UpgradeProject_6_to_7);
		if (upgresult)
			upgresult = DoProjectUpgrade(7, 8, tempfolder, tempini, UpgradeProject_7_to_8);
		if (upgresult)
			upgresult = DoProjectUpgrade(8, 9, tempfolder, tempini, UpgradeProject_8_to_9);
		if (upgresult)
			upgresult = DoProjectUpgrade(9, 10, tempfolder, tempini, UpgradeProject_9_to_10);
		if (upgresult)
			upgresult = DoProjectUpgrade(10, 971, tempfolder, tempini, UpgradeProject_10_to_971);

		if (Paths::FileExists(errlogpath))
			Paths::FileMoveToFolder(errlogpath, projectfolder);

		if (upgresult)
			return CommitProjectUpgrade(projectfolder, tempfolder);

		return{ false, upgresult.value };
	}


	// LOCAL HELPER IMPLEMENTATIONS
	namespace {

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

		// Changes untyoed arrays into typed arrays.
		// e.g. { one tow three } becomes str[]{ one two three }
		//      [ true false true } becomes bool[]{ true false false }
		//      { 0x00 0x7F 0xFE } becomes byte[]{ 0x00 0x7F 0xFE }
		pair_result<CString> AddTypeToArray(CString arraytext)
		{
			bool changed = false;
			if (arraytext.Find('{') == 0) {
				// there's no automatic way to determine if the values are integers or strings.
				// Try to test the first value to see if we can deduce something.
				CString typemarker;
				std::istringstream istr((LPCSTR)arraytext);
				std::string s;
				istr.ignore(1);
				istr >> s >> std::ws;
				if (!s.empty()) {
					if (s.find("0x") < 2 || s.find("$") < 2 || isdigit(s[0]) || isdigit(s[1])) {
						typemarker.Format("%s", BYTETAG.c_str());
					}
					else if (s == "true" || s == "false") {
						typemarker.Format("%s", BOOLTAG.c_str());
					}
					else {
						typemarker.Format("%s", STRTAG.c_str());
					}

					if (!typemarker.IsEmpty()) {
						std::string rest;
						std::getline(istr, rest, '}');
						rest = trim(rest);
						arraytext.Format("%s{ %s %s }", (LPCSTR)typemarker, s.c_str(), rest.c_str());
						changed = true;
					}
				}
			}
			return{ changed, arraytext };
		}

		version_result PreConversionCheck(CString inipath, int sourceversion, int targetversion)
		{
			int version = CFFHacksterProject::ReadProjectVersion(inipath);
			if (version >= targetversion) return{ versionok, "" };
			if (version == sourceversion) return{ needsconversion, "" };

			CString err;
			err.Format("Unable to perform conversion from version %d to %d", sourceversion, targetversion);
			return{ versionfailure, err };
		}

		pair_result<CString> AddValuesToKeys(CString projectini, const std::vector<value_entry>& newentries)
		{
			mfcstringvector valuekeys{ "values", "revertvals" };
			for (auto valuekey : valuekeys) {
				auto valuesini = CFFHacksterProject::GetIniFilePath(projectini, valuekey);
				if (!Paths::FileExists(valuesini))
					return{ false, "Values file for key '" + valuekey + "' couldn't be upgraded because it can't be found" };

				for (auto entry : newentries) {
					// 0=keyname, 1=value, 2=group, 3=description
					CString keyname = std::get<0>(entry);
					if (!HasIni(valuesini, keyname, "value")) {
						AddDec(valuesini, keyname, std::get<1>(entry), "uint8");
						WriteIni(valuesini, keyname, "group", std::get<2>(entry));
						WriteIni(valuesini, keyname, "desc", std::get<3>(entry));
					}
				}
			}

			return{ true, "" };
		}

		pair_result<CString> AddLabels(CString stringsini, CString section, const mfcstringvector& strings)
		{
			if (!CopyLabels(stringsini, section, address(strings), strings.size()))
				return{ false, "Failed to add labels to section " + section };

			return{ true, "" };
		}

	} // END LOCAL HELPER IMPLEMENTATIONS


	// HELPER IMPLEMENTATIONS

	// Add types to the arrays in the project settings, values, and revertvals files.
	pair_result<CString> UpgradeProject_none_to_1(int oldver, int newver, CString projectfolder, CString projectini)
	{
		UNREFERENCED_PARAMETER(oldver);
		UNREFERENCED_PARAMETER(newver);

		// Make sure the FILES entries no longer use fullpaths.
		{
			auto FILES = "FILES";
			auto filekeys = ReadIniKeyNames(projectini, FILES);
			for (auto key : filekeys) {
				auto thepath = ReadIni(projectini, FILES, key, "");
				WriteIni(projectini, FILES, key, Paths::GetFileName(thepath));
			}
		}
		{
			auto PROJECT = "PROJECT";
			auto projkeys = { "revert", "workrom" };
			for (auto key : projkeys) {
				auto thepath = ReadIni(projectini, PROJECT, key, "");
				WriteIni(projectini, PROJECT, key, Paths::GetFileName(thepath));
			}
		}

		// Add types to the project settings arrays
		{
			size_t processed = 0;
			CString failures;
			auto nodes = mfcstringvector{ "SETTINGS|TextViewInDTE", "SETTINGS|TintTiles", "SETTINGS|SmartTools" };
			for (auto node : nodes) {
				auto parts = split(node, "|");
				auto value = ReadIni(projectini, parts[0], parts[1], "");
				int found = value.Find('{');

				if (found == -1 || found > 0)
					++processed; // key not found or doesn't have a leading curly brace
				else {
					auto newval = AddTypeToArray(value);
					if (newval.result) {
						WriteIni(projectini, parts[0], parts[1], newval.value);
						++processed;
					}
					else
						failures.AppendFormat(newval + "\n");
				}
			}
			if (processed != nodes.size())
				failures.AppendFormat("%lu out of %lu keys failed to upgrade\n", nodes.size() - processed, nodes.size());

			if (!failures.IsEmpty()) return{ false, failures };
		}

		// If there's no revertvals file at the requested path (or if the path is blank), copy values to it.
		{
			auto mainvalini = CFFHacksterProject::GetIniFilePath(projectini, FFHFILE_ValuesPath);
			auto revvalini = CFFHacksterProject::GetIniFilePath(projectini, FFHFILE_RevertValuesPath);
			if (revvalini.IsEmpty()) {
				revvalini = Paths::ReplaceExtension(mainvalini, "revert.values");
				// Note that code below expects to move this value from PROJECT to FILES in the mormal case.
				// Put this entry into PROJECT to accomodate it.
				WriteIni(projectini, "PROJECT", FFHFILE_RevertValuesPath, Paths::GetFileName(revvalini));
			}
			if (!Paths::FileExists(revvalini))
				Paths::FileCopy(mainvalini, revvalini);
		}

		// Move revertvals from section PROJECT to FILES
		MoveValue(projectini, "PROJECT", FFHFILE_RevertValuesPath, "FILES");

		// Add types to the values file arrays and add the type
		mfcstringvector valuekeys{ "values", FFHFILE_RevertValuesPath };
		for (auto valuekey: valuekeys) {
			auto valuesini = CFFHacksterProject::GetIniFilePath(projectini, valuekey);
			if (!Paths::FileExists(valuesini))
				return{ false, "Values file for key '" + valuekey + "' couldn't be upgraded because it can't be found" };

			CString failures;
			auto sections = ReadIniSectionNames(valuesini);
			for (auto section : sections) {
				auto value = ReadIni(valuesini, section, "value", "");
				if (value.Find('{') == 0) {
					auto newval = AddTypeToArray(value);
					auto newtype = get_arraytype(newval);
					if (newval.result) {
						WriteIni(valuesini, section, "value", newval.value);
						WriteIni(valuesini, section, "type", newtype);
					}
					else
						failures.AppendFormat(newval + "\n");
				}
			}
			if (!failures.IsEmpty()) return{ false, failures };
		}

		return{ true, "" };
	}

	const CString section = "STRINGCOUNTS";

	namespace v2to3 {
		bool WriteStringCounts(CString projectini)
		{
			WriteIni(projectini, section, "WEPMAGLABELS", "WEAPONMAGICGRAPHIC_COUNT");
			WriteIni(projectini, section, "AILABELS", "AI_COUNT");
			WriteIni(projectini, section, "SHOPLABELS", "SHOP_COUNT");
			WriteIni(projectini, section, "PATTERNTABLELABELS", "BATTLEBACKDROP_COUNT");
			WriteIni(projectini, section, "BACKDROPLABELS", "BATTLEBACKDROP_COUNT");
			WriteIni(projectini, section, "ONTELEPORTLABELS", "ONTELEPORT_COUNT");
			WriteIni(projectini, section, "NNTELEPORTLABELS", "NNTELEPORT_COUNT");
			WriteIni(projectini, section, "NOTELEPORTLABELS", "NOTELEPORT_COUNT");
			WriteIni(projectini, section, "TREASURELABELS", "TREASURE_COUNT");
			WriteIni(projectini, section, "STDMAPLABELS", "MAP_COUNT");
			WriteIni(projectini, section, "TILESETLABELS", "TILESET_COUNT");
			WriteIni(projectini, section, "SPRITELABELS", "SPRITE_COUNT");
			WriteIni(projectini, section, "SPRITEGRAPHICLABELS", "MAPSPRITEGRAPHIC_COUNT");
			WriteIni(projectini, section, "BATTLELABELS", "BATTLE_COUNT");
			WriteIni(projectini, section, "DIALOGUELABELS", "DIALOGUE_COUNT");
			WriteIni(projectini, section, "SPECIALTILELABELS", "SPECIAL_COUNT");
			WriteIni(projectini, section, "MISCCOORDLABELS", "MISCCOORDOFFSET_COUNT");
			WriteIni(projectini, section, "OOBMAGICLABELS", "OOBMAGIC_COUNT");
			return true;
		}
	}

	// Add the STRINGCOUNTS section to the project file.
	pair_result<CString> UpgradeProject_1_to_2(int oldver, int newver, CString projectfolder, CString projectini)
	{
		UNREFERENCED_PARAMETER(oldver);
		UNREFERENCED_PARAMETER(newver);

		// Write the string countname mappings
		if (!v2to3::WriteStringCounts(projectini))
			return{ false, "Unable to write stringcounts" };

		return{ true, "" };
	}

	// Move the editor order settings from the project file to editorsettings. //REFACTOR - this is now obselete, how to handle it?
	// Add battle sprite variables to the values and revert values file if they don't already exist.
	pair_result<CString> UpgradeProject_2_to_3(int oldver, int newver, CString projectfolder, CString projectini)
	{
		UNREFERENCED_PARAMETER(oldver);
		UNREFERENCED_PARAMETER(newver);

		{
			auto editorini = CFFHacksterProject::GetIniFilePath(projectini, FFHFILE_EditorSettingsPath);
			if (Paths::FileExists(editorini)) {
				// If missing, defaults will autocreate, but existing settings should be moved.
				try {
					if (HasIniSection(projectini, "EDITORS")) {
						if (!MoveIniSection(projectini, editorini, "EDITORS"))
							return{ false, "Unable to move the EDITORS section to editorsettings" };
					}
				}
				catch (std::exception & ex) {
					return{ false, "Unable to move the EDITORS section to editorsettings:\n" + CString(ex.what()) };
				}
			}
		}

		// Add values to the Values file
		{
			// 0=keyname, 1=value, 2=group, 3=description
			std::vector<value_entry> newentries = {
				{ "BATTLESPRITETILE_COUNT", 32, "Graphics", "The number of tiles in a battle character composite graphic" },
				{ "BATTLESPRITETILE_BYTES", 16, "Graphics", "The size of each battle character composite tile in bytes" },
				{ "MAPMANSPRITETILE_COUNT", 16, "Graphics", "The number of tiles in a mapman character composite graphic" },
				{ "MAPMANSPRITETILE_BYTES", 16, "Graphics", "The size of each mapman character composite tile in bytes" }
			};

			auto result = AddValuesToKeys(projectini, newentries);
			if (!result)
				return result;
		}

		return{ true, "" };
	}


	namespace v3to4 {
		bool WriteStringCounts(CString projectini)
		{
			const CString section = "STRINGCOUNTS";
			WriteIni(projectini, section, "AILEFFECTLABELS", "AILEFFECT_COUNT");
			WriteIni(projectini, section, "ELEMENTLABELS", "ELEMENT_COUNT");
			WriteIni(projectini, section, "ENEMYCATEGORYLABELS", "ENEMYCATEGORY_COUNT");
			return true;
		}
	}

	// Write string counts for AILEFFECT, ELEMENT, and ENEMYCATEGORY labels.
	// Add value and string entries for them as well.
	pair_result<CString> UpgradeProject_3_to_4(int oldver, int newver, CString projectfolder, CString projectini)
	{
		UNREFERENCED_PARAMETER(oldver);
		UNREFERENCED_PARAMETER(newver);

		// Write the string countname mappings
		if (!v3to4::WriteStringCounts(projectini))
			return{ false, "Unable to write stringcounts" };

		// 0=keyname, 1=value, 2=group, 3=description
		{
			std::vector<value_entry> newentries = {
				{ "ELEMENT_COUNT", 8, "Inventory and Counts", "The number of elements" },
				{ "ENEMYCATEGORY_COUNT", 8, "Inventory and Counts", "The number of enemy hurt categories" },
				{ "AILEFFECT_COUNT", 8, "Inventory and Counts", "The number of enemy hurt categories" },
			};
			auto result = AddValuesToKeys(projectini, newentries);
			if (!result)
				return result;
		}

		{
			auto stringsini = CFFHacksterProject::GetIniFilePath(projectini, FFHFILE_StringsPath);
			if (!Paths::FileExists(stringsini))
				return{ false, "Values file for key 'strings' couldn't be upgraded because it can't be found" };

			const mfcstringvector elements = { "Status", "Poi/Stone", "Time/Space", "Death", "Fire", "Ice", "Lit", "Earth" };
			const mfcstringvector aileffects = { "Death", "Stine", "Poison", "Dark", "Stun", "Sleep", "Mute", "Conf" };
			const mfcstringvector enemycategories = { "Special", "Dragon", "Giant", "Undead", "Were", "Water", "Mage", "Regenerative" };
			auto strresult = AddLabels(stringsini, "AILEFFECTLABELS", aileffects);
			if (strresult)
				strresult = AddLabels(stringsini, "ELEMENTLABELS", elements);
			if (strresult)
				strresult = AddLabels(stringsini, "ENEMYCATEGORYLABELS", enemycategories);
			if (strresult)
				return strresult;
		}

		return{ true, "" };
	}

	pair_result<CString> UpgradeProject_4_to_5(int oldver, int newver, CString projectfolder, CString projectini)
	{
		UNREFERENCED_PARAMETER(oldver);

		CString strversion;
		strversion.Format("%d", newver);

		auto labelsini = CFFHacksterProject::GetIniFilePath(projectini, FFHFILE_StringsPath);
		if (!Paths::FileExists(labelsini))
			return{ false, "Couldn't access strings file when attempting to upgrade to version " + strversion };

		// In most cases, this won't happen in the wild, but it helps some older testing/private/alpha projects.

		// An error in the previous version created the AdditionalModuleFolder in the EXT section,
		// but the project creation UI didn't allow this value to be set.
		// The project would read/write the value in the FILES section, so that's where it normally appears.
		if (HasIni(projectini, "FILES", FFHREFDIR_AddlModFolder))
			MoveValue(projectini, "FILES", FFHREFDIR_AddlModFolder, "REF");

		// However, debug and test projects might have manually inserted this in EXTENSIONS, so just delete that.
		RemoveValue(projectini, "EXTENSIONS", FFHREFDIR_AddlModFolder);

		// Move publish from PROJECT to REF
		if (HasIni(projectini, "PROJECT", FFHREFDIR_Publish))
			MoveValue(projectini, "PROJECT", FFHREFDIR_Publish, "REF");

		// Move asmdll from FILES to REF
		// Debug and test versions might have manually included it, and there's no UI to change it,
		// so only move it if it's already defined. In most cases, this will be skipped.
		if (HasIni(projectini, "FILES", FFHREFDIR_AsmDLLPath))
			MoveValue(projectini, "FILES", FFHREFDIR_AsmDLLPath, "REF");

		// Update BACKROPLABELS=BATTLEBACKDROP_COUNT to BACKDROPLABELS=BATTLEBACKDROP_COUNT (missing the D)
		if (!RenameValue(projectini, "STRINGCOUNTS", "BACKROPLABELS", "BACKDROPLABELS"))
			return{ false, "Can't find new value BACKDROPLABELS, and also can't move the old incorrect value BACKROPLABELS to the new one." };

		// Rename the bad section from the strings file
		RenameSection(labelsini, "BACKROPLABELS", "BACKDROPLABELS");

		// Change the pattern table value with the correct data
		UpdateIfValue(projectini, "STRINGCOUNTS", "PATTERNTABLELABELS", "BATTLEBACKDROP_COUNT", "BATTLEPATTERNTABLE_COUNT");

		// Remove the EDITORS section from the editors INI, it's no longer used (replaced by editors digest)
		RemoveSection(projectini, "EDITORS");

		return{ true,"" };
	}

	pair_result<CString> UpgradeProject_5_to_6(int oldver, int newver, CString projectfolder, CString projectini)
	{
		UNREFERENCED_PARAMETER(oldver);
		UNREFERENCED_PARAMETER(newver);

		// If the editorsettings file isn't specified, set the path now (the file will autocreate)
		{
			auto editorpath = CFFHacksterProject::GetIniFilePath(projectini, FFHFILE_EditorSettingsPath);
			if (editorpath.IsEmpty()) {
				auto thename = Paths::ReplaceExtension(Paths::GetFileName(projectini), FFHFILE_EditorSettingsPath);
				WriteIni(projectini, FILESSECTION, FFHFILE_EditorSettingsPath, thename);
			}
		}

		mfcstringvector valuekeys{ FFHFILE_ValuesPath, FFHFILE_RevertValuesPath };
		for (auto valuekey : valuekeys) {
			auto valuesini = CFFHacksterProject::GetIniFilePath(projectini, valuekey);

			// Remove chestfanfare, chestopen, nochestsnd
			RemoveSection(valuesini, "chestfanfare");
			RemoveSection(valuesini, "chestopen");
			RemoveSection(valuesini, "nochestsnd");

			// Set the colors to RGB
			{
				for (auto colkey : { "TRANSPARENTCOLOR", "TRANSPARENTCOLORREPLACEMENT" }) {
					if (HasIniSection(valuesini, colkey))
						WriteIni(valuesini, colkey, "type", "rgb");
				}
			}

			// Deduce types for every value that doesn't have one. Do this for both values and revertvals
			auto sections = ReadIniSectionNames(valuesini);
			for (const auto & section : sections) {
				auto thevalue = ReadIni(valuesini, section, "value", "");
				if (!thevalue.IsEmpty()) {
					auto thetype = ReadIni(valuesini, section, "type", "");
					if (thetype.IsEmpty()) {
						CString newtype;
						if (thevalue.Find("0x") == 0) {
							newtype = "bword";
						}
						else if (thevalue.Find("$") == 0) {
							newtype = "addr";
						}
						else if (thevalue.SpanIncluding("0123456789").GetLength() == thevalue.GetLength()) {
							newtype = "uint8"; // full decimal number with no other characters
						}
						if (!newtype.IsEmpty())
							WriteIni(valuesini, section, "type", newtype);
					}
				}
			}
		}

		return{ true,"" };
	}

	pair_result<CString> UpgradeProject_6_to_7(int oldver, int newver, CString projectfolder, CString projectini)
	{
		UNREFERENCED_PARAMETER(oldver);
		UNREFERENCED_PARAMETER(newver);

		using namespace Editors2;

		// If new editor designations don't exist, translate the old ones into new ones
		auto editorsini = CFFHacksterProject::GetIniFilePath(projectini, FFHFILE_EditorSettingsPath);
		if (!Paths::FileExists(editorsini))
			return{ true, "" }; // nothing to do

		auto sections = ReadIniSectionNames(editorsini);
		auto iter = std::remove_if(begin(sections), end(sections), [](CString sect) { return Editors2::IsEditorSectionName(sect); });
		sections.erase(iter, end(sections));

		for (const auto & oldsection : sections) {
			auto show = ReadIni(editorsini, oldsection, "status", "") == "include";
			if (show) {
				// The editor is visible, if a new editor doesn't exist with this name, create that section now
				auto name = ReadIni(editorsini, oldsection, "name", "");
				auto newsection = GetEditorSectionName(name);
				if (!name.IsEmpty() && !HasIniSection(editorsini, newsection)) {
					// If the module path exists, then write the name and modulepath to the new editor
					auto modulepath = ReadIni(editorsini, oldsection, "path", "");
					if (!modulepath.IsEmpty()) {
						WriteIni(editorsini, newsection, "name", name);
						WriteIni(editorsini, newsection, "modulepath", modulepath);
					}
				}
			}

			// Regardless of what happened above, remove the old editor
			Ini::RemoveSection(editorsini, oldsection);
		}

		return{ true,"" };
	}

	pair_result<CString> UpgradeProject_7_to_8(int oldver, int newver, CString projectfolder, CString projectini)
	{
		UNREFERENCED_PARAMETER(oldver);
		UNREFERENCED_PARAMETER(newver);

		CString allerrors;
		mfcstringvector valuekeys{ FFHFILE_ValuesPath, FFHFILE_RevertValuesPath };

		//TODO - do we need to upgrade the ReservedPath as well?

		mfcstringvector names{ "HPMAX_OFFSET" };
		mfcstringvector groups{ "Class Info" };
		intvector addrs{ 0x2D9D6,  0x37F0A };
		mfcstringvector descs{ "Offset to the 16-bit max HP limit + 1" };
		for (size_t i = 0; i < names.size(); ++i) {
			auto key = names[i];
			auto addr = addrs[i];
			auto group = groups[i];
			auto desc = descs[i];

			for (auto valuekey : valuekeys) {
				CString errors;
				auto valuesini = CFFHacksterProject::GetIniFilePath(projectini, valuekey);
				if (!HasIniSection(valuesini, key)) {
					if (!Ini::WriteGroupedValue(valuesini, key, hex(addr, 5), group))
						errors.AppendFormat("- can't write the new key '%s' to INI\n", (LPCSTR)key);
					else {
						WriteIni(valuesini, key, "type", "bword");
						if (ReadIni(valuesini, key, "type", "") != "bword")
							errors.AppendFormat("- wrote the new key '%s' to INI, but couldn't write its type\n", (LPCSTR)key);
						else
							WriteIni(valuesini, key, "desc", desc);
					}
				}
				if (!errors.IsEmpty()) errors.Insert(0, "Encountered problems writing new key to " + valuesini + ":\n");
				allerrors.Append(errors);
			}
		}

		return{ allerrors.IsEmpty(), allerrors };
	}

	pair_result<CString> UpgradeProject_8_to_9(int oldver, int newver, CString projectfolder, CString projectini)
	{
		UNREFERENCED_PARAMETER(oldver);
		UNREFERENCED_PARAMETER(newver);

		struct errnode { CString destfile; CString section; CString name; CString reason; };
		std::vector<errnode> errors;

		using UpdateFunc = void(*)(CString, CString, bool);
		auto AddOrUpdate = [&errors](CString appini, CString destini, CString section, UpdateFunc updatefunc) {
			// if the section exists, then run updatefunc on it, else copy it from the app ini.
			if (Ini::HasIniSection(destini, section)) {
				updatefunc(destini, section, true);
			}
			else {
				if (!Ini::CopyIniSection(appini, destini, section))
					errors.push_back(errnode{ destini, section, "", "Section copy from app INI failed." });
			}
		};

		auto errlogpath = Paths::Combine({ projectfolder, Paths::GetFileStem(projectini) + ".upgrade-err.txt"});
		auto valuesini = CFFHacksterProject::GetIniFilePath(projectini, FFHFILE_ValuesPath);
		auto revertini = CFFHacksterProject::GetIniFilePath(projectini, FFHFILE_RevertValuesPath);
		auto appini = Paths::Combine({ Paths::GetProgramFolder(), Paths::GetProgramName() + ".values.template" });

		// Terminal special cases:
		// values file is missing: just copy app values and revert values to temp dest files.
		// values is there, revert  is missing: copy values to revert (do this after the loop below).
		if (!Paths::FileExists(valuesini)) {
			// In this case, we'll discard the revert file and replace both with current options.
			if (!Paths::FileCopy(appini, valuesini))
				return { false, "Unable to replace missing values file with app template." };

			Paths::FileCopy(appini, revertini);
			AfxMessageBox("Values file is missing; it and the revert values file will be replaced with app defaults.");
			return { true, "" };
		}

		mfcstringvector destinations;
		if (Paths::FileExists(valuesini)) destinations.push_back(valuesini);
		if (Paths::FileExists(revertini)) destinations.push_back(revertini);

		for (const auto& destini : destinations) {
			const auto RESERVED_LISTS = "RESERVED_LISTS";
			auto reslists = Ini::ReadIniKeyNames(appini, RESERVED_LISTS);
			Ini::WriteIniRefonly(destini, RESERVED_LISTS, true);
			for (const auto& listname : reslists) {
				if (listname == "refonly") continue;
				Ini::WriteIni(destini, RESERVED_LISTS, listname, "1");
				AddOrUpdate(appini, destini, listname, Ini::WriteIniReserved);
				auto subkeys = Ini::ReadIni(appini, listname, "value", mfcstringvector());
				for (const auto& sub : subkeys) {
					AddOrUpdate(appini, destini, sub, Ini::WriteIniReserved);
				}
			}

			const auto RESERVED_FILTERS = "RESERVED_FILTERS";
			auto filters = Ini::ReadIniKeyNames(appini, RESERVED_FILTERS);
			Ini::WriteIniRefonly(destini, RESERVED_FILTERS, true);
			for (const auto& filtname : filters) {
				if (filtname == "refonly") continue;
				Ini::WriteIni(destini, RESERVED_FILTERS, filtname, "1");
				AddOrUpdate(appini, destini, filtname, Ini::WriteIniReserved);
			}

			const auto standalones = {
				"ROM_SIZE", "BANK_SIZE", "TINT_VARIANT", "TRANSPARENTCOLOR", "TRANSPARENTCOLORREPLACEMENT",
				"unsram", "bridgescene", "gold", "items", "talkfanfare", "notalksnd"
			};
			for (const auto& sname : standalones) {
				AddOrUpdate(appini, destini, sname, Ini::WriteIniReserved);
			}
		}

		if (destinations.empty())
			return { false, "No values files were found.\nTODO: see comments for how to handle this situation." };

		if (errors.empty()) {
			// If the project file's STRINGCOUNTS section erroneously
			// maps PATTERNTABLELABELS=BATTLEBACKDROP_COUNT,
			// then remap it as PATTERNTABLELABELS=BATTLEPATTERNTABLE_COUNT.
			// Note that we won't unconditionally fail on this because some hacks
			// might have removed it.
			// If not found, leave it as is.
			auto hasbptc = Ini::HasIni(valuesini, "BATTLEPATTERNTABLE_COUNT", "value");
			auto oldval = Ini::ReadIni(projectini, "STRINGCOUNTS", "PATTERNTABLELABELS", "");
			if (hasbptc && oldval == "BATTLEBACKDROP_COUNT") {
				if (oldval == "BATTLEBACKDROP_COUNT") {
					Ini::WriteIni(projectini, "STRINGCOUNTS", "PATTERNTABLELABELS", "BATTLEPATTERNTABLE_COUNT");
				}
			}
		}

		if (!errors.empty()) {
			// Write the errors to a file
			{
				std::ofstream ofs;
				ofs.open((LPCSTR)errlogpath, std::ios::trunc);
				for (const auto& err : errors) {
					ErrorMsg << (LPCSTR)err.reason << "\n -> " << (LPCSTR)err.destfile << ", section: "
						<< (LPCSTR)err.section << ", value: " << (LPCSTR)err.name << std::endl;
					ofs << (LPCSTR)err.reason << "\n -> " << (LPCSTR)err.destfile << ", section: "
						<< (LPCSTR)err.section << ", value: " << (LPCSTR)err.name << std::endl;
				}
			}
			CString msg;
			msg.Format("Encounted %d error(s) while processing reserved INI values.\n"
				"If you continue, you might have to manually modify the values and/or revert values files.\n"
				"Failure to do so coul dprevent multiple editors from running.\n\n"
				"Click OK to take this risk and proceed.\n"
				"Click Cancel to abort the upgrade attempt.", errors.size());
			auto result = AfxMessageBox(msg, MB_OKCANCEL | MB_ICONERROR | MB_DEFBUTTON2);
			if (result == IDCANCEL)
				return { false, "You chose to abort the upgrade.\n"
					"Errors were written to the project folder file '" + Paths::GetFileName(errlogpath) + "'." };
		}

		if (!Paths::FileExists(revertini))
			Paths::FileCopy(valuesini, revertini);

		return { true, "" };
	}

	pair_result<CString> UpgradeProject_9_to_10(int oldver, int newver, CString projectfolder, CString projectini)
	{
		UNREFERENCED_PARAMETER(oldver);
		UNREFERENCED_PARAMETER(newver);

		auto appini = Paths::Combine({ Paths::GetProgramFolder(), Paths::GetProgramName() + ".values.template" });
		auto valuesini = CFFHacksterProject::GetIniFilePath(projectini, FFHFILE_ValuesPath);
		auto names = mfcstringvector{ "MAPBATTLERATE_OFFSET", "OWBATTLERATE_LAND_OFFSET", "OWBATTLERATE_SEA_OFFSET" };
		for (const auto& n : names) {
			if (!Ini::HasIniSection(valuesini, n)) {
				// copy from the template
				Ini::CopyIniSection(appini, valuesini, n);
			}
		}
		return { true, "" };
	}

	pair_result<CString> UpgradeProject_10_to_971(int oldver, int newver, CString projectfolder, CString projectini)
	{
		UNREFERENCED_PARAMETER(oldver);
		UNREFERENCED_PARAMETER(newver);

		// Add new values if they aren't already there
		auto appini = Paths::Combine({ Paths::GetProgramFolder(), Paths::GetProgramName() + ".values.template" });
		auto valuesini = CFFHacksterProject::GetIniFilePath(projectini, FFHFILE_ValuesPath);
		auto names = mfcstringvector{
			"SHIPVIS_OFFSET", "AIRSHIPVIS_OFFSET", "BRIDGEVIS_OFFSET", "CANALVIS_OFFSET",
			"SONGS_BASEINDEX", "SONGLABELS_COUNT", "TILESETSONGLIST_OFFSET", "TILESETSONGLIST_ASMLABEL"};
		for (const auto& n : names) {
			if (!Ini::HasIniSection(valuesini, n)) {
				// copy from the template
				Ini::CopyIniSection(appini, valuesini, n);
			}
		}

		// Add labels if not already there
		auto stringsections = mfcstringvector{ "SONGLABELS" };
		auto applabelsini = Paths::Combine({ Paths::GetProgramFolder(), Paths::GetProgramName() + ".strings.template" });
		auto stringsini = CFFHacksterProject::GetIniFilePath(projectini, FFHFILE_StringsPath);
		for (const CString& sect : stringsections) {
			if (!HasIniSection(stringsini, sect)) {
				Ini::CopyIniSection(applabelsini, stringsini, sect);
			}
		}

		// Set necessary stringcounts mappings
		const CString section = "STRINGCOUNTS";
		WriteIni(projectini, section, "SONGLABELS", "SONGLABELS_COUNT");
		return { true, "" };
	}

} // end namespace Upgrades