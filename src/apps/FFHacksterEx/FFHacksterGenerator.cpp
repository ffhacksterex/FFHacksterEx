#include "stdafx.h"
#include "FFHacksterGenerator.h"
#include <editor_label_defaults.h>
#include <editor_label_functions.h>
#include <ini_functions.h>
#include <io_functions.h>
#include <path_functions.h>
#include <string_functions.h>
#include <type_support.h>
#include <ui_prompts.h>
#include <AppSettings.h>
#include <ProjectLoader.h>

#include <DirPathRemover.h>
#include <FFHacksterProject.h>
#include <DlgPickNamedDestination.h>

#include "sProjectGeneratorValues.h"
#include "FFHackster.h"
#include "FFHacksterDlg.h"
#include "DlgCreateNewProject.h"
#include "WaitingDlg.h"
#include "EditorDigestDlg.h"

#include <fstream>
#include <sstream>
#include <iostream>

using namespace Editorlabels;
using namespace Ini;
using namespace Strings;
using namespace Types;
using namespace Ui;

namespace {
	void WriteFileSection(std::ostream & os, LPCSTR name) {
		os << "[" << name << "]" << std::endl;
	}

	void WriteFileValue(std::ostream & os, LPCSTR name, LPCSTR data) {
		os << name << "=" << data << std::endl;
	}

	void WriteAllEditorInfos(CFFHacksterProject & proj, const Editors2::sEditorInfoVector & infos)
	{
		Editors2::EditorSettingsSerializer serializer;
		serializer.ProjectFolder = proj.ProjectFolder;
		serializer.EditorSettingsPath = proj.EditorSettingsPath;
		serializer.ProjectAdditionModulesFolder = proj.AdditionalModulesFolder;
		serializer.WriteAllEditorInfos(infos);
	}
}

CFFHacksterGenerator::CFFHacksterGenerator(AppSettings & appsettings)
	: AppStgs(&appsettings) //REFACTOR - change Settings to a reference?
{
}

CFFHacksterGenerator::~CFFHacksterGenerator()
{
}

//FUTURE - replace this mess with well-defined commandlines
CString CFFHacksterGenerator::ProcessAction(CString curaction, CString appinipath, CString actionparam)
{
	pair_result<CString> result = { false, "'" + curaction + "' is not recognized: no action was taken." };

	try {
		if (curaction == "immediaterom")
			result = EditROM(actionparam);
		else if (curaction == "newrom")
			result = CreateROMProject();
		else if (curaction == "newasm")
			result = CreateAsmProject();
		else if (curaction == "open")
			result = OpenProject();
		else if (curaction == "unzip")
			result = UnzipProject();
		else if (curaction == "recent")
			result = OpenRecentFile(actionparam);

		//REFACTOR - develop a cleaner way to test and collect app params
		else if (curaction.Find("open|") == 0) {
			result = pair_result<CString>(true, extract_term(curaction, "open|"));
			curaction = "open";
		}
		else if (curaction.Find("openquit|") == 0) {
			result = pair_result<CString>(true, extract_term(curaction, "openquit|"));
			curaction = "openquit";
		}
		else if (curaction.Find("edit|") == 0) {
			result = pair_result<CString>(true, extract_term(curaction, "edit|"));
			curaction = "edit";
		}
		else if (curaction.Find("editquit|") == 0) {
			result = pair_result<CString>(true, extract_term(curaction, "editquit|"));
			curaction = "editquit";
		}

		if (result)
			return ExecuteActionResult(curaction, result.value, appinipath);
	}
	catch (std::exception & ex) {
		result = { false, "An exception was caught: " + CString(ex.what()) };
	}
	catch (...) {
		result = { false, "An unexpected exception was caught" };
	}

	if (result.value != "prompt" && !result.value.IsEmpty()) {
		CString msg;
		msg.Format("Unexpected command '%s' will be ignored, and a prompt will occur instead.", (LPCSTR)result.value);
		AfxMessageBox(msg);
	}
	return "prompt";
}

pair_result<CString> CFFHacksterGenerator::EditROM(CString rompath)
{
	// If a project already exists for this ROM, just open the project.
	// The project is created in a folder beside the ROM with the same file title.
	auto projectpath = BuildEditRomProjectPath(rompath);
	if (Paths::FileExists(projectpath))
		return{ true, projectpath };

	// Otherwise, prompt for project creation, but pre-populate and freeze the folder paths.
	auto projectfolder = Paths::GetDirectoryPath(projectpath);
	auto parentfolder = Paths::GetDirectoryPath(projectfolder);
	auto projectname = Paths::GetFileStem(projectpath);

	CDlgCreateNewProject dlgproj;
	dlgproj.AppStgs = AppStgs;
	dlgproj.LockDefinedInputs = true;
	dlgproj.ShowBackgroundArt = AppStgs->ShowBackgroundArt;
	dlgproj.ProjectType = "editrom";
	dlgproj.ForceNewLabels = AppStgs->ForceNewLabelsForNewProjects;
	dlgproj.ParentFolder = parentfolder;
	dlgproj.ProjectName = projectname;
	dlgproj.RevertPath = rompath;
	dlgproj.PublishPath = Paths::Combine({ projectfolder, Paths::GetFileName(rompath) });
	if (dlgproj.DoModal() != IDOK)
		return{ false, "" };

	sProjectGeneratorValues values;
	values.parentfolder = dlgproj.ParentFolder;
	values.projectname = dlgproj.ProjectName;
	values.romfile = dlgproj.RevertPath;
	values.publishromfile = dlgproj.PublishPath;
	values.datfile = dlgproj.DATPath;
	values.addlmodfolder = dlgproj.AdditionalModulesFolder;
	values.forcenewlabels = dlgproj.ForceNewLabels;
	return DoCreateRomProject(values);
}

pair_result<CString> CFFHacksterGenerator::CreateROMProject()
{
	CDlgCreateNewProject dlgproj;
	dlgproj.AppStgs = AppStgs;
	dlgproj.ShowBackgroundArt = AppStgs->ShowBackgroundArt;
	dlgproj.ProjectType = "rom";
	dlgproj.ForceNewLabels = AppStgs->ForceNewLabelsForNewProjects;
	if (dlgproj.DoModal() != IDOK)
		return{ false, "" };

	sProjectGeneratorValues values;
	values.parentfolder = dlgproj.ParentFolder;
	values.projectname = dlgproj.ProjectName;
	values.romfile = dlgproj.RevertPath;
	values.publishromfile = dlgproj.PublishPath;
	values.datfile = dlgproj.DATPath;
	values.addlmodfolder = dlgproj.AdditionalModulesFolder;
	values.forcenewlabels = dlgproj.ForceNewLabels;
	return DoCreateRomProject(values);
}

pair_result<CString> CFFHacksterGenerator::CreateAsmProject()
{
	// Unzip the asm archive, then do the same thing that CreateROMProject does to
	// generate all of the other files.
	CString parentfolder = "";
	CString projectname = "";
	CString basezipasm = "";
	CString asmdllpath = "";
	CString publishrom = "";
	CString datfile = "";
	CString addlmodfolder = "";
	bool forcenewlabels = AppStgs->ForceNewLabelsForNewProjects;

	{
		CDlgCreateNewProject dlgproj;
		dlgproj.AppStgs = AppStgs;
		dlgproj.ShowBackgroundArt = AppStgs->ShowBackgroundArt;
		dlgproj.ProjectType = "asm";
		dlgproj.ForceNewLabels = forcenewlabels;
		if (dlgproj.DoModal() != IDOK)
			return{ false, "" };

		parentfolder = dlgproj.ParentFolder;
		projectname = dlgproj.ProjectName;
		basezipasm = dlgproj.RevertPath;
		asmdllpath = dlgproj.AsmDllPath;
		publishrom = dlgproj.PublishPath;
		datfile = dlgproj.DATPath;
		addlmodfolder = dlgproj.AdditionalModulesFolder;
		forcenewlabels = dlgproj.ForceNewLabels;
	}

	CWaitCursor wait;
	CWaitingDlg waiting;
	waiting.Init();
	waiting.SetMessage("Creating a new Assembly project...");

	// Create the folder
	CString thisfolder;
	thisfolder.Format("%s\\%s", (LPCSTR)parentfolder, (LPCSTR)projectname);
	if (!Paths::DirCreate(thisfolder))
		return{ false, "Can't create new Asm parent folder " + thisfolder };

	Io::DirPathRemover removethisfolder(thisfolder);

	// Asm-specific initialization
	CString revertzip = thisfolder + "\\" + projectname + ".revert.zip";
	if (!Paths::FileCopy(basezipasm, revertzip))
		return{ false, "Couldn't move reversion data to " + revertzip };
	pair_result<CString> revertfile = { true, revertzip };

	CString asmfolder;
	asmfolder.Format("%s\\asm", (LPCSTR)thisfolder);

	// Unzip to a subfolder named "asm"
	if (!Io::Unzip(revertfile.value, asmfolder))
		return{ false, "Can't create Asm folder " + asmfolder + ".\nEnsure that 7za is in the program folder." };

	Io::MakeWritable(asmfolder);

	//   Since the Asm hasn't been compiled, we can't copy the work or publish ROMs.
	//   Form the names, but don't test anything.
	CString destrom;
	destrom.Format("%s\\%s.work.nes", (LPCSTR)thisfolder, (LPCSTR)projectname);
	if (publishrom.IsEmpty())
		publishrom.Format("%s\\%s.nes", (LPCSTR)thisfolder, (LPCSTR)projectname);

	// Settings initialization
	auto talkfile = CopyAppFileType("dialogue", thisfolder, projectname);
	if (!talkfile) return talkfile;
	auto dtetable = CopyAppFileType("dte.tbl", thisfolder, projectname);
	if (!dtetable) return dtetable;
	auto editorsettingsfile = CopyAppFileType("editorsettings", thisfolder, projectname);
	if (!editorsettingsfile) return editorsettingsfile;
	auto palfile = CopyAppFileType("pal", thisfolder, projectname);
	if (!palfile) return palfile;
	auto stdtable = CopyAppFileType("std.tbl", thisfolder, projectname);
	if (!stdtable) return stdtable;
	auto stringsfile = CreateEditorLabels(thisfolder, projectname, forcenewlabels);
	if (!stringsfile) return stringsfile;
	auto valuesfile = CopyAppFileType("values", thisfolder, projectname);
	if (!valuesfile) return valuesfile;
	auto revertvaluesfile = CopyAppFileType("values", thisfolder, projectname + ".revert");
	if (!revertvaluesfile) return revertvaluesfile;
	//auto reservedfile = CopyAppFileType("reserved", thisfolder, projectname); //REMOVERESERVED
	//TODO - autocreate if missing
	//if (!reservedfile) return reservedfile;

	// Create the file and write the basic settings
	CString projectpath;
	projectpath.Format("%s\\%s.ff1asm", (LPCSTR)thisfolder, (LPCSTR)projectname);

	WriteIni(projectpath, PROJECTSECTION, "name", projectname);
	WriteIni(projectpath, PROJECTSECTION, "version", dec(CFFHacksterProject::Version));
	WriteIni(projectpath, PROJECTSECTION, "type", "asm");
	WriteIni(projectpath, PROJECTSECTION, "revert", Paths::GetFileName(revertfile.value));
	WriteIni(projectpath, PROJECTSECTION, "workrom", Paths::GetFileName(destrom));

	try
	{
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_ValuesPath, Paths::GetFileName(valuesfile.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_RevertValuesPath, Paths::GetFileName(revertvaluesfile.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_StringsPath, Paths::GetFileName(stringsfile.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_EditorSettingsPath, Paths::GetFileName(editorsettingsfile.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_PalettePath, Paths::GetFileName(palfile.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_StdTablePath, Paths::GetFileName(stdtable.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_DteTablePath, Paths::GetFileName(dtetable.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_DialoguePath, Paths::GetFileName(talkfile.value));
		//CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_ReservedPath, Paths::GetFileName(reservedfile.value)); //REMOVERESERVED

		CFFHacksterProject::SetIniRefDir(projectpath, FFHREFDIR_Publish, publishrom);
		CFFHacksterProject::SetIniRefDir(projectpath, FFHREFDIR_AddlModFolder, addlmodfolder);
		CFFHacksterProject::SetIniRefDir(projectpath, FFHREFDIR_AsmDLLPath, asmdllpath);
	}
	catch (std::exception & ex) {
		return{ false, "Couldn't set INI file paths, " + CString(ex.what()) };
	}
	catch (...) {
		return{ false, "Couldn't set INI file paths due to an unexpected exception" };
	}

	if (!Paths::FileExists(projectpath))
		return{ false, "Failed to create new Asm project file " + projectpath };

	// Now assign the initial editors
	auto editorresult = SelectAndWriteEditors(thisfolder, projectpath, addlmodfolder, editorsettingsfile);
	if (!editorresult)
		return editorresult;

	if (!CFFHacksterProject::WriteStringCounts(projectpath))
		return{ false, "Unabled to write stringcounts" };

	if (Paths::FileExists(datfile)) {
		// Attempt to import the Hackster DAT file now
		auto datresult = ProjectLoader::InjectHacksterDAT(projectpath, datfile);
		if (!datresult)
			return datresult;
	}

	removethisfolder.Revoke();

	waiting.DestroyWindow();
	return{ true, projectpath };
}

// Select and open a project, otherwise prompt.
pair_result<CString> CFFHacksterGenerator::OpenProject()
{
	CFileDialog dlg(TRUE, nullptr, nullptr, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | Ui::COMMON_OFN,
		"FF1 Projects (*.ff1rom;*.ff1asm)|*.ff1rom;*.ff1asm|"
		"FF1 ROM Projects (*.ff1rom)|*.ff1rom|"
		"FF1 Assembly Projects (*.ff1asm)|*.ff1asm|"
		"|",
		AfxGetMainWnd());

	auto modalresult = dlg.DoModal();
	if (modalresult == IDOK)
		return{ true, dlg.GetPathName() };
	if (modalresult == IDCANCEL)
		return{ false, "prompt" };

	CString err;
	err.Format("Failed while prompting to open a project (modal result %d)", modalresult);
	return{ false, err };
}

// Unzip an archive file that contains one of the supported project types, otherwise prompt.
pair_result<CString> CFFHacksterGenerator::UnzipProject()
{
	CFileDialog dlg(TRUE, nullptr, nullptr, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | Ui::COMMON_OFN,
		"FF1 Zipped Projects (*.ff1zip)|*.ff1zip|"
		"|",
		AfxGetMainWnd());

	auto modalresult = dlg.DoModal();
	if (modalresult == IDCANCEL)
		return{ false, "prompt" };

	if (modalresult == IDOK) {
		CString pathname = dlg.GetPathName();
		auto ziptypes = std::vector<CString>{ ".ff1rom", ".ff1asm" };
		auto index = Io::ZipHasFileExtension(pathname, ziptypes);
		switch (index) {
		case 0:
		case 1:
		{
			// Attempt to extract the project archive.
			// On success, returns a command that will open the extracted project.
			auto ret = UnzipAndOpenProject(pathname, ziptypes[index]);
			if (ret) return ret;

			if (!ret.value.IsEmpty()) AfxMessageBox(ret.value);
			break;
		}
		}
		return{ false, "prompt" };
	}

	CString err;
	err.Format("Failed while prompting to unzip a project (modal result %d)", modalresult);
	return{ false, err };
}

pair_result<CString> CFFHacksterGenerator::OpenRecentFile(CString recentpath)
{
	CString filetoopen;

	if (Paths::FileExists(recentpath)) {
		filetoopen = recentpath;
	}
	else {
		// If the file is on the MRU but doesn't exist, then ask to remove it from the MRU.
		int prompt = AfxMessageBox("The following file does not exist:\n" + recentpath +
			"\n\nDo you want to remove the reference from the Recent Files list?", MB_YESNO | MB_ICONQUESTION);
		if (prompt == IDYES) {
			// Remove the file from the MRU, then update the MRU.
			//m_appsettings.ReadMru(AppIni);
			AppStgs->RemoveMru(recentpath);
			//m_appsettings.WriteMru(AppIni);
		}
	}
	
	bool hasfile = Paths::FileExists(filetoopen);
	return{ hasfile, filetoopen };
}

CString CFFHacksterGenerator::GetAppFilePrefix()
{
	return "FFHacksterEx";
}

CString CFFHacksterGenerator::FormatAppFileName(CString filetype)
{
	CString filename;
	filename.Format("%s.%s.template", (LPCSTR)GetAppFilePrefix(), (LPCSTR)filetype);
	return filename;
}

pair_result<CString> CFFHacksterGenerator::CopyAppFileType(CString filetype, CString folder, CString projectname)
{
	// build the app folder template file name
	CString filename = FormatAppFileName(filetype);
	CString srcpath = Paths::Combine({ Paths::GetProgramFolder(), filename });

	// remove the .template from the filename
	// replace the filetitle with the provided project name
	CString newfilename = filename;
	newfilename.Replace(".template", "");
	newfilename = Paths::ReplaceFileTitle(newfilename, projectname);

	CString destpath = Paths::Combine({ folder, newfilename });
	bool copied = false;
	CString strresult;
	if (CopyFile(srcpath, destpath, FALSE)) {
		copied = true;
		strresult = destpath;
	}
	else {
		strresult.Format("Failed to copy '%s' into the project as '%s", (LPCSTR)srcpath, (LPCSTR)newfilename);
	}
	return{ copied, strresult };
}

// Creates editor labels from either a native LBLS file or internal defaults.
// If using internal defaults, the values file ***MUST*** have been created first.
pair_result<CString> CFFHacksterGenerator::CreateEditorLabels(CString folder, CString newfilename, bool forcerecreation)
{
	CString programdir(Paths::GetCWD());

	// If not forcing creation from internal defaults, can we copy from the app's strings.template file?
	if (!forcerecreation) {
		auto result = CopyAppFileType("strings", folder, newfilename);
		if (result)
			return result;
		else
			WarnHere << "Unable to create labels from the app 'strings.template', falling back to internal defaults." << std::endl;
	}

	// Otherwise, can we create from scratch?
	const int DIALOGUE_COUNT = 256;
	const int BATTLE_COUNT = 0x80;

	CString inifile;
	inifile.Format("%s\\%s", (LPCSTR)folder, (LPCSTR)newfilename);
	Paths::FileDelete(inifile);

	CopyLabels(inifile, "WEPMAGLABELS", editordefs_wepmagtable, _countof(editordefs_wepmagtable));
	CopyLabels(inifile, "AILABELS", editordefs_aitable, _countof(editordefs_aitable));
	CopyLabels(inifile, "SHOPLABELS", editordefs_shoptable, _countof(editordefs_shoptable));
	CopyLabels(inifile, "PATTERNTABLELABELS", editordefs_patterntables, _countof(editordefs_patterntables));
	CopyLabels(inifile, "BACKDROPLABELS", editordefs_backdroptables, _countof(editordefs_backdroptables));
	CopyLabels(inifile, "ONTELEPORTLABELS", editordefs_onteleporttables, _countof(editordefs_onteleporttables), "Enter");
	CopyLabels(inifile, "NNTELEPORTLABELS", editordefs_nnteleporttables, _countof(editordefs_nnteleporttables));
	CopyLabels(inifile, "NOTELEPORTLABELS", editordefs_noteleporttables, _countof(editordefs_noteleporttables), "Exit");
	CopyLabels(inifile, "TREASURELABELS", editordefs_treasuretables, _countof(editordefs_treasuretables));
	CopyLabels(inifile, "STDMAPLABELS", editordefs_maptables, _countof(editordefs_maptables));
	CopyLabels(inifile, "TILESETLABELS", editordefs_tilesettables, _countof(editordefs_tilesettables));
	CopyLabels(inifile, "SPRITELABELS", editordefs_spritetables, _countof(editordefs_spritetables));
	CopyLabels(inifile, "SPRITEGRAPHICLABELS", editordefs_spritegraphictables, _countof(editordefs_spritegraphictables));
	GenerateLabels(inifile, "BATTLELABELS", BATTLE_COUNT, "Battle");
	GenerateLabels(inifile, "DIALOGUELABELS", DIALOGUE_COUNT, "Text");
	CopyLabels(inifile, "SPECIALTILELABELS", editordefs_specialtext, _countof(editordefs_specialtext));
	CopyLabels(inifile, "MISCCOORDLABELS", editordefs_misccoordtext, _countof(editordefs_misccoordtext));
	CopyLabels(inifile, "AILEFFECTLABELS", editordefs_aileffects, _countof(editordefs_aileffects));
	CopyLabels(inifile, "ELEMENTLABELS", editordefs_elements, _countof(editordefs_elements));
	CopyLabels(inifile, "ENEMYCATEGORYLABELS", editordefs_enemycategories, _countof(editordefs_enemycategories));

	bool exists = Paths::FileExists(inifile);
	CString str = exists ? inifile : inifile + " could not be created.";
	return{ exists, str };
}

CString CFFHacksterGenerator::ExecuteActionResult(CString curaction, CString resultstring, CString AppIni)
{
	// Wrap the dialog (which might loop for reloading) into a single call.
	// Note that single-edit invocations don't loop
	// because they don't invoke the main project UI processing path.
	const auto RunDialog = [](CFFHacksterDlg & dlg) {
		do {
			dlg.DoModal();
		} while (dlg.WantsToReload());

		//N.B. - If the main hackster dialog class fails to initialize, then it sets ExitAction to prompt.
		//	However, that behavior might change, or another dialog could be used in its place that
		//	doesn't set the action. Set it to prompt if the action is blank on a failed dialog.
		if (dlg.InitFailed() && dlg.ExitAction.IsEmpty())
			dlg.ExitAction = "prompt";
	};

	CString nextaction = "prompt";
	if (is_in(curaction, std::set<CString>{"newrom", "newasm", "open", "openquit", "immediaterom", "recent" }))
	{
		CFFHacksterDlg dlg;
		//dlg.AppIni = AppIni;
		dlg.AppStgs = this->AppStgs;
		dlg.ProjectFile = resultstring;
		if (curaction == "openquit") dlg.ExitAction = "quit";
		RunDialog(dlg);
		nextaction = dlg.ExitAction;
	}
	else if (curaction == "edit" || curaction == "editquit") {
		std::istringstream istr((LPCSTR)resultstring);
		std::string editor, projectpath;
		std::getline(istr, editor, '|');
		std::getline(istr, projectpath);

		CFFHacksterDlg dlg;
		dlg.InitialEditorName = editor.c_str();
		//dlg.AppIni = AppIni;
		dlg.AppStgs = this->AppStgs;
		dlg.ProjectFile = projectpath.c_str();
		if (curaction == "editquit") dlg.ExitAction = "quit";
		dlg.DoModal(); // single-edit, don't call RunDialog for this
		nextaction = dlg.ExitAction;
	}
	else if (curaction == "unzip") {
		// This block actually opens the extracted project.
		CFFHacksterDlg dlg;
		//dlg.AppIni = AppIni;
		dlg.AppStgs = this->AppStgs;
		dlg.ProjectFile = resultstring;
		curaction = "open";
		RunDialog(dlg);
		nextaction = dlg.ExitAction;
	}
	else {
		CString err;
		err.Format("Action '%s: %s' does not support execution (yet).", (LPCSTR)curaction, (LPCSTR)resultstring);
		AfxMessageBox(err, MB_ICONERROR);
	}
	return nextaction;
}

pair_result<CString> CFFHacksterGenerator::UnzipAndOpenProject(CString pathname, CString dotextension)
{
	CString ext = dotextension;
	ext.MakeLower();
	const auto FindProjectFile = [ext](CString filepath) -> bool {
		filepath.MakeLower(); // this is a copy, CString uses copy-on-write semantics
		return filepath.Find(ext) != -1;
	};

	// Prompt for destination folder
	CDlgPickNamedDestination pick(AfxGetMainWnd());
	pick.Title = "Extract Project Archive";
	pick.Blurb = "Specify a parent folder and a name for the new project.\n"
		"The project will be created as a subdirectory with that name inside the parent folder.";
	pick.StartInFolder = Paths::GetDirectoryPath(pathname);

	const auto& destfolder = pick.DestFolderPath;
	if (pick.DoModal() != IDOK)
		return { false, "" }; // cancelled
	if (Paths::DirExists(destfolder) && !Paths::DirEmpty(destfolder))
		return { false, "Pleaase specify a new or empty folder as the destination." };

	// Unzip to a destination folder, attach a dir remover to it
	Io::DirPathRemover remover(destfolder);
	if (!Io::Unzip(pathname, destfolder))
		return { false, "Can't extract archive to '" + destfolder + "'." };

	// Find the project file by dotextension
	auto files = Paths::GetFiles(destfolder, Paths::GetFilesScope::NotRecursive, FindProjectFile);
	if (files.empty()) return { false, "No project file was found in the archive." };

	// Rename the project's files.
	// Then find the project file (which will have a transformed name) and return it.
	auto srctitle = Paths::GetFileStem(files[0]);
	auto desttitle = Paths::GetFileStem(destfolder);
	CFFHacksterProject::RenameProjectFiles(destfolder, files[0], srctitle, desttitle);
	auto newfiles = Paths::GetFiles(destfolder, Paths::GetFilesScope::NotRecursive, FindProjectFile);
	if (!newfiles.empty()) {
		remover.Revoke();
		return{ true, newfiles[0] };
	}

	return { false, "Failed to rename the extracted project files; archive extraction attempt abandoned." };
}

CString CFFHacksterGenerator::BuildEditRomProjectPath(CString rompath)
{
	CString projectpath;
	projectpath.Format("%s\\%s\\%s.ff1rom", (LPCSTR)Paths::GetDirectoryPath(rompath),
		(LPCSTR)Paths::GetFileStem(rompath), (LPCSTR)Paths::GetFileStem(rompath));
	return projectpath;
}

pair_result<CString> CFFHacksterGenerator::DoCreateRomProject(const sProjectGeneratorValues & values)
{
	auto parentfolder = values.parentfolder;
	auto projectname = values.projectname;
	auto romfile = values.romfile;
	auto publishromfile = values.publishromfile;
	auto datfile = values.datfile;
	auto addlmodfolder = values.addlmodfolder;
	auto forcenewlabels = values.forcenewlabels;

	CWaitCursor wait;
	CWaitingDlg waiting;
	waiting.Init();
	waiting.SetMessage("Creating a new ROM project...");

	// Create the folder
	CString thisfolder;
	thisfolder.Format("%s\\%s", (LPCSTR)parentfolder, (LPCSTR)projectname);
	if (!Paths::DirCreate(thisfolder))
		return{ false, "Can't create new ROM parent folder " + thisfolder };

	Io::DirPathRemover removethisfolder(thisfolder);

	// ROM-specific initialization
	CString revertromfile;
	revertromfile.Format("%s\\%s.rom.clean", (LPCSTR)thisfolder, (LPCSTR)projectname);
	if (!Paths::FileCopy(romfile, revertromfile))
		return{ false, "Can't create clean reversion ROM " + revertromfile };

	// Since we aren't compiling anything in this case, the work ROM must always exist.
	CString destromfile;
	destromfile.Format("%s\\%s.work.nes", (LPCSTR)thisfolder, (LPCSTR)projectname);
	if (!Paths::FileCopy(romfile, destromfile))
		return{ false, "Can't create work ROM " + destromfile };

	Io::MakeWritable(destromfile);

	CString publishromsettingpath;
	if (!publishromfile.IsEmpty()) {
		publishromsettingpath = publishromsettingpath;
	}
	else {
		// Auto-populate with a filepath inside this folder, and make sure the setting uses a relative path.
		publishromsettingpath.Format("%s.nes", (LPCSTR)projectname);
		publishromfile.Format("%s\\%s", (LPCSTR)thisfolder, (LPCSTR)publishromsettingpath);
	}
	if (!Paths::FileCopy(romfile, publishromfile))
		return{ false, "Can't create publish ROM " + publishromfile };

	Io::MakeWritable(destromfile);
	Io::MakeWritable(revertromfile);
	Io::MakeWritable(publishromfile);

	// Settings initialization
	auto talkfile = CopyAppFileType("dialogue", thisfolder, projectname);
	if (!talkfile) return talkfile;
	auto dtetable = CopyAppFileType("dte.tbl", thisfolder, projectname);
	if (!dtetable) return dtetable;
	auto editorsettingsfile = CopyAppFileType("editorsettings", thisfolder, projectname);
	if (!editorsettingsfile) return editorsettingsfile;
	auto palfile = CopyAppFileType("pal", thisfolder, projectname);
	if (!palfile) return palfile;
	auto stdtable = CopyAppFileType("std.tbl", thisfolder, projectname);
	if (!stdtable) return stdtable;
	auto stringsfile = CreateEditorLabels(thisfolder, projectname, forcenewlabels);
	if (!stringsfile) return stringsfile;
	auto valuesfile = CopyAppFileType("values", thisfolder, projectname);
	if (!valuesfile) return valuesfile;
	auto revertvaluesfile = CopyAppFileType("values", thisfolder, projectname + ".revert");
	if (!revertvaluesfile) return revertvaluesfile;
	//auto reservedfile = CopyAppFileType("reserved", thisfolder, projectname); //REMOVERESERVED
	//TODO - do this for EditROM, NewROM, and NewAsm
	//TODO - if not created, create defaults immediately
	//if (!reservedfile) return reservedfile;

	// Create the file and write the basic settings.
	//N.B. - most of these settings use relative filenames
	CString projectpath;
	projectpath.Format("%s\\%s.ff1rom", (LPCSTR)thisfolder, (LPCSTR)projectname);

	WriteIni(projectpath, PROJECTSECTION, "name", projectname);
	WriteIni(projectpath, PROJECTSECTION, "version", dec(CFFHacksterProject::Version));
	WriteIni(projectpath, PROJECTSECTION, "type", "rom");
	WriteIni(projectpath, PROJECTSECTION, "revert", Paths::GetFileName(revertromfile));
	WriteIni(projectpath, PROJECTSECTION, "workrom", Paths::GetFileName(destromfile));

	try {
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_ValuesPath, Paths::GetFileName(valuesfile.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_RevertValuesPath, Paths::GetFileName(revertvaluesfile.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_StringsPath, Paths::GetFileName(stringsfile.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_EditorSettingsPath, Paths::GetFileName(editorsettingsfile.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_PalettePath, Paths::GetFileName(palfile.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_StdTablePath, Paths::GetFileName(stdtable.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_DteTablePath, Paths::GetFileName(dtetable.value));
		CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_DialoguePath, Paths::GetFileName(talkfile.value));
		//CFFHacksterProject::SetIniFilePath(projectpath, FFHFILE_ReservedPath, Paths::GetFileName(reservedfile.value)); //REMOVERESERVED

		CFFHacksterProject::SetIniRefDir(projectpath, FFHREFDIR_Publish, publishromsettingpath);
		CFFHacksterProject::SetIniRefDir(projectpath, FFHREFDIR_AddlModFolder, addlmodfolder);
	}
	catch (std::exception & ex) {
		return { false, "Couldn't set INI file paths, " + CString(ex.what()) };
	}
	catch (...) {
		return { false, "Couldn't set INI file paths due to an unexpected exception" };
	}

	if (!Paths::FileExists(projectpath))
		return{ false, "Failed to create new ROM project file " + projectpath };

	// Now assign the initial editors
	auto editorresult = SelectAndWriteEditors(thisfolder, projectpath, addlmodfolder, editorsettingsfile);
	if (!editorresult)
		return editorresult;

	if (!CFFHacksterProject::WriteStringCounts(projectpath))
		return{ false, "Unabled to write stringcounts" };

	if (Paths::FileExists(datfile)) {
		// Attempt to import the Hackster DAT file now
		auto datresult = ProjectLoader::InjectHacksterDAT(projectpath, datfile);
		if (!datresult)
			return datresult;
	}

	removethisfolder.Revoke();

	waiting.DestroyWindow();
	return{ true, projectpath };
}

pair_result<CString> CFFHacksterGenerator::SelectAndWriteEditors(CString projectfolder, CString projectpath, CString additionalmodulesfolder, CString editorsettingspath)
{
	CFFHacksterProject tempproj;
	tempproj.AppSettings = AppStgs;
	tempproj.ProjectFolder = projectfolder;
	tempproj.ProjectPath = projectpath;
	tempproj.AdditionalModulesFolder = additionalmodulesfolder;
	tempproj.EditorSettingsPath = editorsettingspath;

	CEditorDigestDlg dlgeditors;
	dlgeditors.EditorInfos = Editors2::GetDefaultEditorInfos();
	//dlgeditors.AppFolder = Paths::GetProgramFolder();
	dlgeditors.Project = &tempproj;
	if (dlgeditors.DoModal() != IDOK)
		return{ false, "Editors must be selected before a project can be created" };

	WriteAllEditorInfos(tempproj, dlgeditors.EditorInfos);
	return { true, "" };
}