#include "stdafx.h"
#include "ProjectLoader.h"
#include <AppSettings.h>
#include <AsmFiles.h>
#include <asmdll_impl.h>
#include <DATA.h>
#include <DirPathRemover.h>
#include <FFHacksterProject.h>
#include <FFH2Project.h>
#include <FilePathRemover.h>
#include <GameSerializer.h>
#include <IProgress.h>
#include <ini_functions.h>
#include <io_functions.h>
#include <path_functions.h>
#include <string_conversions.hpp>

#include <upgrade_functions.h>
#include <Upgrades_ini.h>

#include "DlgPromptProjectBackup.h"
#include "ProjectLoader_ini.h"
#include "FFH2Project_IniToJson_Defs.hpp"

#include <fstream>
#include <filesystem>
#include <regex>

//STDFILESYS - see path_functions.cpp for more info
namespace fs = std::experimental;
using namespace asmdll_impl;


ProjectLoader::ProjectLoader(FFH2Project & prj2, CFFHacksterProject& proj, AppSettings& appstgs)
	: Project(proj), Proj2(prj2), Appstgs(appstgs)
{
}

ProjectLoader::~ProjectLoader()
{
}

namespace {
	bool IsUpgradeNeeded(std::string projectpath, bool isIni);
	std::string HandleBackup(std::string projectpath, AppSettings& appstgs, bool forceBackup);
	std::string PrepareForUpgrade(std::string projectpath);
	std::string UpgradeIniProject(std::string projectpath);
	void UpgradeJsonProject(std::string projectpath);
	void CommitUpgrade(std::string projectpath, std::string preppath);
	FFH2Project LoadJsonProject(std::string projectpath, AppSettings& appstgs);
	std::string ExtractJsonProjectVersion(std::string projectpath);
	bool LoadVariablesAndConstants(FFH2Project& proj, IProgress* progress);
}

CString ProjectLoader::Load(CString projectpath, IProgress* progress)
{
	ASSERT(Paths::FileExists(projectpath));

	CWaitCursor wait;
	if (!Paths::FileExists(projectpath))
		throw std::runtime_error("No project found at the selected path.");

	// Upgrade if needed; for INI files, backup prior to upgrade is mandatory.
	std::string currentprojpath = (LPCSTR)projectpath;
	auto isIni = Upgrades::IsIniFormat(currentprojpath.c_str());
	auto needUpgrade = IsUpgradeNeeded(currentprojpath, isIni);
	if (needUpgrade) {
		auto backuppath = HandleBackup(currentprojpath, Appstgs, isIni);
		auto preppath = PrepareForUpgrade(currentprojpath);
		Io::FilePathRemover backupremover(backuppath);
		Io::DirPathRemover prepremover(Paths::GetDirectoryPath(preppath));

		//NOTE: if isIni, then set currentprojpath to the new JSON filename, which is in the same directory.
		currentprojpath = !isIni ? preppath : UpgradeIniProject(preppath);
		UpgradeJsonProject(currentprojpath);
		if (isIni)
			backupremover.Revoke(); // keep the backup if INI forced us to create it.
		CommitUpgrade((LPCSTR)projectpath, preppath);
	}

	// The commit above moves the JSON project file back to projectfolder,
	// but we need to update the path accordinagly.
	std::string newjsonpath = (LPCSTR)Paths::ReplaceFileName(projectpath, Paths::GetFileName(currentprojpath.c_str()));
	Proj2 = LoadJsonProject(newjsonpath, Appstgs);
	LoadVariablesAndConstants(Proj2, progress);
	
	return Proj2.ProjectPath.c_str();
}

bool ProjectLoader::Save()
{
	Proj2.Save();
	return true;
}

//STATIC
// Save settings from a Hackster DAT file to an FFHacksterEx project file.
pair_result<CString> ProjectLoader::InjectHacksterDAT(CString projectfile, CString hacksterfile)
{
	CFFHacksterProject proj;
	auto refresult = proj.LoadFileRefs(projectfile);
	if (!refresult)
		return refresult;

	proj.LoadSharedSettings();
	proj.ImportHacksterDAT(hacksterfile);
	proj.SaveSharedSettings();
	return { true, "" };
}

//STATIC
pair_result<CString> ProjectLoader::IsProjectDllCompatible(CFFHacksterProject & project, CString asmdllpath)
{
	auto asmresult = asmdll_impl::IsDirDllCompatible(project.GetContentFolder(), asmdllpath);
	if (!asmresult) {
		if (project.AppSettings->EnforceAssemblyDllCompatibility) {
			return { false, "Can't use the selected assembly:\n" + asmresult.value + "." };
		}
		else if (project.AppSettings->WarnAssemblyDllCompatibility) {
			return { true, "Warning:\n" + asmresult.value + ".\n\n"
				"You can suppress this warning either app - wide(App Settings)\nor per - project(Project Settings)." };
		}
	}
	return asmresult;
}

pair_result<CString> LoadProject(FFH2Project& prj2, CFFHacksterProject & proj, CString projectpath, AppSettings& appstgs, IProgress * progress)
{
	ProjectLoader loader(prj2, proj, appstgs);
	return{ true, loader.Load(projectpath, progress) };
}

bool SaveProject(FFH2Project& prj2, CFFHacksterProject & proj)
{
	ASSERT(prj2.AppSettings != nullptr);

	if (prj2.AppSettings == nullptr)
		throw std::runtime_error("Failed to save the project; app settings are inaccessible.");

	ProjectLoader loader(prj2, proj, *prj2.AppSettings);
	return loader.Save();
}

// === NEW IMPLEMENTATION
namespace {

	bool IsUpgradeNeeded(std::string projectpath, bool isIni)
	{
		if (isIni) {
			// We'll treat all INI files as needing conversion.
			return true;
		}
		else {
			//TODO - for now, no later version, eventually, call this:
			//auto strver = ExtractJsonProjectVersion(projectpath)
			return false;
		}
		//throw std::runtime_error(__FUNCTION__ " not implemented yet");
	}

	std::string HandleBackup(std::string projectpath, AppSettings & appstgs, bool forceBackup)
	{
		// either ask for an upgrade or force the issue
		//TODO - temporary, add appsettings to the FFH2Project or pass it in separately?
		//auto appstgs = AppSettings(AppSettings::GetAppSettingsPath());
		CString startdir = FOLDERPREF(&appstgs, PrefArchiveFolder);
		if (!Paths::DirExists(startdir)) startdir = Paths::GetParentDirectory(projectpath.c_str());

		//TODO - consider the forced backup to be an autonamed zip placed in the project folder.
		//		Display the filename to the user.
		//		Place a cleanup button on the project settings folder to allow its removal later.

		auto done = false;
		CString backuppath;
		do {
			CDlgPromptProjectBackup dlgup(AfxGetMainWnd());
			dlgup.AppStgs = &appstgs;
			dlgup.ProjectFilePath = projectpath.c_str();
			dlgup.Filter = "Project archives (*.ff1zip)|*.ff1zip||";
			dlgup.StartInFolder = startdir;

			backuppath.Empty();
			auto canup = dlgup.DoModal();
			backuppath = dlgup.BackupFilePath;

			if (Paths::FileExists(backuppath)) {
				// Ask again if overwriting an existing backup isn't OK with the user
				auto ask = AfxMessageBox("OK to backup to existing file " + dlgup.BackupFilePath + "?\n",
					MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION);
				if (ask != IDYES) done = false;
			}
			else if (backuppath.IsEmpty() && !forceBackup) {
				done = true;
			}
			else if (backuppath.IsEmpty() && forceBackup) {
				// If forcing, then throw if the user tries to skip the backup altogether.
				auto ask = AfxMessageBox("A backup is required in this scenario, selcting No will cancel the process.",
					MB_YESNO | MB_DEFBUTTON1 | MB_ICONSTOP);
				if (ask != IDYES) throw std::runtime_error("Upgrade cannot continue without a backup.");
			}
			else {
				// the file doesn't exist and the path is not empty (must be a new filepath).
				done = true;
			}
		} while (!done);

		if (!backuppath.IsEmpty()) {
			//CString projfolder = Paths::GetDirectoryPath((CString)projectpath.c_str());
			CString projfolder = tomfc(Paths::GetDirectoryPath(projectpath));
			if (!Io::Zip(backuppath, projfolder, true))
				throw std::runtime_error("Upgrade failure - couldn't make the requested backup.");
		}

		return (LPCSTR)backuppath;
	}

	std::string PrepareForUpgrade(std::string projectpath)
	{
		auto result = Upgrades::PrepareProjectUpgrade(projectpath.c_str());
		if (!result)
			throw std::runtime_error(result.value); // on failure, it's an error message

		return (LPCSTR)result.value; // on success, it's the newly created temp folder
	}

	std::string UpgradeIniProject(std::string projectpath)
	{
		Upgrades_ini::Upgrade(projectpath);
		auto jsonpath = Upgrades::IniToJson(projectpath.c_str());
		return jsonpath;
	}

	void UpgradeJsonProject(std::string projectpath)
	{
		auto strver = ExtractJsonProjectVersion(projectpath);
		TRACE("JSON project version = %s\n", strver.c_str());

		//TODO - add semantic comparison object, for now just return
	}

	void CommitUpgrade(std::string projectpath, std::string preppath)
	{
		auto projectfolder = tomfc(Paths::GetDirectoryPath(projectpath));
		auto prepfolder = tomfc(Paths::GetDirectoryPath(preppath));
		if (!Paths::DirShallowUpdate(prepfolder, projectfolder))
			throw std::runtime_error("Unable to copy upgrade files back to the project");
	}

	FFH2Project LoadJsonProject(std::string projectpath, AppSettings& appstgs)
	{
		if (!Paths::FileExists(projectpath))
			throw std::runtime_error("Cannot find the path to load:\n" + projectpath);

		FFH2Project prj2;
		prj2.Load(projectpath);

		// Set the appsettings as a member of the project
		prj2.AppSettings = &appstgs;
		return prj2;
	}

	std::string ExtractJsonProjectVersion(std::string projectpath)
	{
		//TODO - here's a test, scrape the ffheader object
		std::ifstream ifs(projectpath);
		std::string partialheader;
		size_t maxcount = 512;
		partialheader.reserve(maxcount);
		for (size_t st = 0; st < maxcount; ++st) partialheader.push_back(ifs.get());

		//DEVNOTE - this relies on using nlohmann::ordered_json or a similar approach to keep ffheader as the first property
		auto head = partialheader.find("ffheader");
		auto hopen = head != std::string::npos ? partialheader.find("{", head) : head;
		auto hend = hopen != std::string::npos ? partialheader.find('}', hopen) : hopen;
		if (hopen == std::string::npos)
			throw std::runtime_error("Cannot extract version from header.");

		auto newpartial = partialheader.substr(hopen, hend - hopen + 1);
		auto oj = ojson::parse(newpartial);
		auto v = oj["version"];
		std::string strver = v.get<std::string>();
		return strver;
	}

	bool LoadVariablesAndConstants(FFH2Project & proj, IProgress* progress)
	{
		//TODO - add a progress step

		// Load the variables and constants. If this fails, then many editors become unusable.
		proj.m_varmap.clear();
		if (proj.IsAsm()) {
			GameSerializer gs(proj);
			return gs.ReadAllVariables(tomfc(proj.ProjectFolder), proj.m_varmap, progress);
		}
		if (proj.IsRom()) {
			return proj.UpdateVarsAndConstants();
		}

		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::initializing, "unknown project types can't load vars/constants");
	}


} // end namespace (unnamed)