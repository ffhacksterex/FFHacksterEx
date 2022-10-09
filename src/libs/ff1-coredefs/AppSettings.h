#pragma once

#include <vector>
#include "HelpType.h"

#define MRULOWLIMIT 4
#define MRUMAXLIMIT 25

#define FOLDERPREF(as,name) (((as) != nullptr) && ((as)->UseFolderPrefs) ? ((as)->name) : "")
#define FOLDERPREF_OR(as,name,altpath) (((as) != nullptr) && ((as)->UseFolderPrefs) ? ((as)->name) : (altpath))

struct sAppFolderPrefs
{
public:
	CString PrefProjectParentFolder;
	CString PrefCleanFolder;
	CString PrefAdditionalModulesFolder;
	CString PrefPublishFolder;
	CString PrefArchiveFolder;
	CString PrefMapImportExportFolder;
	CString PrefImageImportExportFolder;
	CString PrefAsmDllFolder;

	void DecodePrefPaths();
	void EncodePrefPaths();

protected:
	std::vector<CString*> get_path_reflist();
};

class AppSettings : public sAppFolderPrefs
{
public:
	//TODO - move to the base app
	static CString GetAppSettingsPath();
	static AppSettings ReadCurrentSettings();

	AppSettings(CString inifile);
	~AppSettings();

	void Read();
	void Write();
	void ReadMru();
	void WriteMru();

	void UpdateMru(CString mostrecentpath);
	void RemoveMru(CString pathtoremove);

	bool PromptOnClose;
	bool PurgeLogFileOnStartup;
	CString RunExe;
	CString RunParams;
	bool SaveHexAsUppercase;
	bool ShowBackgroundArt;
	bool ForceNewLabelsForNewProjects;
	bool DisplayStrikeChecksAsNormalCheckboxes;
	COLORREF StrikeCheckedColor;
	bool CompileBeforeRun;
	bool UseFileDialogToBrowseFolders;
	bool EnforceAssemblyDllCompatibility;
	bool WarnAssemblyDllCompatibility;
	HelpType HelpTypeId;
	CString HelpPath;
	bool UseFolderPrefs;

	std::vector<CString> MruPaths;
	size_t MruMaxCount;

	// "Unpublished" settings with no UI.
	// These options are loaded and saved, but there's no UI to edit them.
	bool EnableHelpChoice;

private:
	CString m_inifile;
};
