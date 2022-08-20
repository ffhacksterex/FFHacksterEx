//REFACTOR
#include "stdafx.h"
#include "ProjectLoader.h"
#include <FFHacksterProject.h>
#include <AppSettings.h>
#include <AsmFiles.h>
#include <DATA.h>
#include <GameSerializer.h>
#include <IProgress.h>

#include <asmdll_impl.h>
#include <assembly_types.h>
#include <collection_helpers.h>
#include <general_functions.h>
#include <ini_functions.h>
#include <interop_assembly_interface.h>
#include <io_functions.h>
#include <path_functions.h>
#include <std_project_properties.h>
#include <type_support.h>
#include <upgrade_functions.h>
#include "ui_prompts.h"
#include "FilePathRemover.h"
#include <fstream>
#include <filesystem>
#include <ATLComTime.h>

//STDFILESYS - see path_functions.cpp for more info
namespace fs = std::experimental;
using namespace asmdll_impl;
using namespace Ini;
using namespace Types;
using namespace Ui;


ProjectLoader::ProjectLoader(CFFHacksterProject & proj)
	: Project(proj)
{
}

ProjectLoader::~ProjectLoader()
{
}

pair_result<CString> ProjectLoader::Load(CString projectpath, IProgress * progress)
{
	CWaitCursor wait;
	// Perform project upgrades if needed.
	{
		if (progress) progress->AddSteps(1);
		if (progress) progress->StepAndProgressText("Loading Project...");
		if (Upgrades::NeedsConversion(projectpath)) {
			// Ask to upgrade
			auto answer = AfxMessageBox("Are you sure you want to upgrade this project?\nBe sure to back it up before attemping to upgrade.", MB_OKCANCEL | MB_DEFBUTTON2);
			if (answer != IDOK) return{ false, "You declined to upgrade at this time." };

			if (progress) progress->SetProgressText("Upgrading Project...");

			// Ask for a folder to create a backup
			auto zipbackfolder = Ui::PromptForFolder(nullptr, "Select a folder to hold the project backup file",
				Paths::GetParentDirectory(projectpath));
			if (!zipbackfolder) return{ false, "You declined to upgrade at this time.\nA backup must be created before upgrading." };

			// Create a zipped backup file
			//auto dt = COleDateTime::GetCurrentTime().Format("%Y-%m-%d@%H-%M-%S.zip");
			auto dt = ".backup.zip";
			CString backstem;
			backstem.Format("%s.v%d.%s", Paths::GetFileStem(projectpath),
				CFFHacksterProject::ReadProjectVersion(projectpath), dt);
			auto zipbackfile = Paths::Combine({ zipbackfolder, backstem });
			if (!Io::Zip(zipbackfile, Paths::GetDirectoryPath(projectpath), true))
				return { false, "Failed to create a backup at\n" + zipbackfile + "." };

			// Process the upgrade.
			// If the upgrade passes, fails, then we don't need the backup since the project didn't change.
			Io::FilePathRemover zipcleanup(zipbackfile);
			auto result = Upgrades::UpgradeProject(projectpath);
			if (!result)
				return{ false, result.value };

			zipcleanup.Revoke(); // keep the backup file in case we want to roll back.
		}
	}

	// Load file references and verify the existence of the required files.
	{
		auto result = Project.LoadFileRefs(projectpath);
		if (!result)
			return result;
	}

	Project.LoadSharedSettings();

	CString msg;
	if (Project.IsAsm()) {
		// For assembly projects, the DLL might not exist (if the project was copied or files were moved around).
		// Prompt for a new DLL in that case, and block loading if one isn't selected.

		// Loop as long as we haven't cancelled and don't have a valid DLL file.
		// valid means:
		// 1) file exists
		// 2) is_ff1_asmdll is true
		// 3) compatibility either matches or is not enforced

		const auto IsAsmValid = [&](CString asmdllpath) {
			if (!Paths::FileExists(asmdllpath))
				return AfxMessageBox("No file found at the selected DLL path."), false;
			if (!asmdll_impl::is_ff1_asmdll(asmdllpath))
				return AfxMessageBox("The selected file is not recognized as an FF1 assembly DLL."), false;

			auto warnasm = Project.ShouldWarnAssemblyDllMismatch();
			//if (Project.AppSettings->EnforceAssemblyDllCompatibility || Project.AppSettings->WarnAssemblyDllCompatibility)
			if (Project.AppSettings->EnforceAssemblyDllCompatibility || warnasm)
			{
				auto asmresult = asmdll_impl::IsDirDllCompatible(Project.GetContentFolder(), asmdllpath);
				if (!asmresult) {
					if (Project.AppSettings->EnforceAssemblyDllCompatibility)
						return AfxMessageBox("Can't use the selected assembly:\n" + asmresult.value + "."), false;
					//else if (Project.AppSettings->WarnAssemblyDllCompatibility)
					else if (warnasm)
						AfxMessageBox("Warning:\n" + asmresult.value + "\n\n"
							"You can suppress this warning either app-wide (App Settings)\nor per-project (Project Settings).");
				}
			}
			return true;
		};

		auto asmdllpath = Project.AsmDLLPath;
		auto asmchanged = false;

		while (!IsAsmValid(asmdllpath)) {
			// We can't proceed without a valid assembly DLL, so either prompt until we get one or abort when the user cancels.
			INT_PTR choice = AfxMessageBox("Assembly projects require an assembly DLL.\nIf you don't select one now, "
								"the project can't be opened.\n\nSelect an assembly DLL now?", MB_OKCANCEL | MB_ICONSTOP | MB_DEFBUTTON1);
			if (choice == IDCANCEL)
				return { false, "Can't load an assembly project without an assembly DLL" };

			auto selresult = PromptForAsmDll(nullptr, Paths::GetProgramFolder());
			if (selresult.result)
				asmdllpath = selresult.value;
			else
				asmdllpath = "";
				//return { false, "Can't load an assembly project without an assembly DLL" };
			asmchanged = true;
		}

		if (asmchanged) {
			Project.SetIniRefDir(FFHREFDIR_AsmDLLPath, asmdllpath);
			Project.AsmDLLPath = asmdllpath;
		}
	}

	if (msg.IsEmpty()) {
		Project.ROMSize = hex(ReadIni(Project.ValuesPath, "ROM_SIZE", "value", "-0x1"));
		if (Project.ROMSize == (decltype(Project.ROMSize))-1)
			msg.Format("Could not read ROM_SIZE value, can't initialize ROM buffer.");
		else if (Project.ROMSize == 0)
			msg.Format("ROM_SIZE cannot be zero.");
		else
			Project.ROM.resize(Project.ROMSize);
	}

	if (msg.IsEmpty() && Project.IsRom()) {
		try {
			Io::load_binary(Project.WorkRomPath, Project.ROM);
		}
		catch (...) {
			ErrorHere << "Unknown exception caught while trying to load ROM buffer (file " << (LPCSTR)Project.WorkRomPath << "." << std::endl;
			msg.Format("An unknown exception was caught while loading the ROM buffer from'\n'%s.", (LPCSTR)Project.WorkRomPath);
		}
	}

	if (msg.IsEmpty() && Project.IsAsm()) {
		bool loadedvars = Project.LoadVariablesAndConstants(progress);
		if (!loadedvars) msg = "Failed to load variables and constants.";
	}

	//TODO - moved this call before assembly-specific loading.
	//if (msg.IsEmpty()) {
	//	Project.LoadEditorSettings();
	//}

	if (msg.IsEmpty())
		msg = Project.LoadCartData();

	return pair_result<CString>(msg.IsEmpty(), msg);
}

bool ProjectLoader::Save()
{
	CString msg;
	bool saved = Project.SaveRefPaths();
	if (!saved)
		msg.AppendFormat("Failed to save ref paths.\n");

	if (saved) {
		Project.SaveSharedSettings();
	}

	if (!msg.IsEmpty())
		AfxMessageBox("Failed to save project:\n" + msg);
	return saved;
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

pair_result<CString> LoadProject(CFFHacksterProject & proj, CString projectpath, IProgress * progress)
{
	ProjectLoader loader(proj);
	return loader.Load(projectpath, progress);
}

bool SaveProject(CFFHacksterProject & proj)
{
	ProjectLoader loader(proj);
	return loader.Save();
}