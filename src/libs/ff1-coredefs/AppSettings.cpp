#include "stdafx.h"
#include "AppSettings.h"
#include "ini_functions.h"
#include "path_functions.h"
#include "string_functions.h"
#include "type_support.h"
#include <algorithm>

using namespace Ini;
using Types::dec;

void sAppFolderPrefs::DecodePrefPaths()
{
	auto paths = get_path_reflist();
	const CString appfolder = Paths::GetProgramFolder();
	for (CString* pstr : paths) {
		CString& str = *pstr;
		str.Replace("*APP", appfolder);
	}
}

void sAppFolderPrefs::EncodePrefPaths()
{
	auto paths = get_path_reflist();
	const CString appfolder = Paths::GetProgramFolder();
	for (CString* pstr : paths) {
		CString& str = *pstr;
		str.Replace(appfolder, "*APP");
	}
}

std::vector<CString*> sAppFolderPrefs::get_path_reflist()
{
	return std::vector<CString*>{
		&PrefProjectParentFolder,
			& PrefCleanFolder,
			& PrefAdditionalModulesFolder,
			& PrefPublishFolder,
			& PrefArchiveFolder,
			& PrefMapImportExportFolder,
			& PrefImageImportExportFolder,
			& PrefAsmDllFolder
	};
}


#define READBOOL(ini,name) name = ReadIniBool(ini, #name, "value", name)
#define READSTR(ini,name) name = ReadIni(ini, #name, "value", name)
#define READINT(ini,name) name = dec(ReadIni(ini, #name, "value", dec((int)name)))
#define READHELPTYPE(ini,name) { int temp = dec(ReadIni(inifile, #name, "value", dec((int)name))); name = (HelpType)temp; }

#define WRITEBOOL(ini,name) WriteIniBool(ini, #name, "value", name)
#define WRITESTR(ini,name) WriteIni(ini, #name, "value", name)
#define WRITEINT(ini,name) WriteIni(ini, #name, "value", dec((int)name))
#define WRITEHELPTYPE WRITEINT

//TODO - eventually move the existing properties to use these new macros
#define READ_SETTING_STR(name) name = Ini::ReadIni(m_inifile, #name, "value", name)
#define WRITE_SETTING_STR(name) Ini::WriteIni(m_inifile, #name, "value", name)

#define READ_SETTING_DEC(name) name = Types::dec(Ini::ReadIni(m_inifile, #name, "value", Types::dec(name)))
#define WRITE_SETTING_DEC(name) Ini::WriteIni(m_inifile, #name, "value", Types::dec(name))

#define READ_SETTING_HEX(name) name = Types::hex(Ini::ReadIni(m_inifile, #name, "value", Types::hex(name)))
#define WRITE_SETTING_HEX(name) Ini::WriteIni(m_inifile, #name, "value", Types::hex(name))

#define READ_SETTING_BOOL(name) name = Ini::ReadIniBool(m_inifile, #name, "value", name)
#define WRITE_SETTING_BOOL(name) Ini::WriteIniBool(m_inifile, #name, "value", name)

#define READ_SETTING_HELP(name) name = (HelpType)Types::dec(Ini::ReadIni(m_inifile, #name, "value", Types::dec(name)))
#define WRITE_SETTING_HELP WRITE_SETTING_DEC

AppSettings::AppSettings(CString inifile)
	: m_inifile(inifile)
	, PromptOnClose(false)
	, PurgeLogFileOnStartup(true)
	, SaveHexAsUppercase(true)
	, ShowBackgroundArt(false)
	, MruMaxCount(10)
	, ForceNewLabelsForNewProjects(false)
	, DisplayStrikeChecksAsNormalCheckboxes(true)
	, StrikeCheckedColor(RGB(64,128,255))
	, CompileBeforeRun(true)
	, UseFileDialogToBrowseFolders(true)
	, EnforceAssemblyDllCompatibility(false)
	, WarnAssemblyDllCompatibility(true)
	, HelpTypeId(HelpType::LocalApp)
	, EnableHelpChoice(false)
{
	m_inifile.Trim();
	if (m_inifile.IsEmpty())
		throw std::exception("App settings must receive a non-empty settings path.");
}

AppSettings::~AppSettings()
{
}

//STATIC
CString AppSettings::GetAppSettingsPath()
{
	CString progfolder(Paths::GetProgramFolder());
	CString progname(Paths::GetProgramName());
	CString appini;
	appini.Format("%s\\%s.appsettings", (LPCSTR)progfolder, (LPCSTR)progname);
	return appini;
}

//STATIC
AppSettings AppSettings::ReadCurrentSettings()
{
	auto filepath = AppSettings::GetAppSettingsPath();
	AppSettings stgs(filepath);
	stgs.Read();
	return stgs;
}

void AppSettings::Read()
{
	CString& inifile = m_inifile;
	try {
		READBOOL(inifile, PromptOnClose);
		READBOOL(inifile, PurgeLogFileOnStartup);
		READSTR(inifile, RunExe);
		READSTR(inifile, RunParams);
		READBOOL(inifile, SaveHexAsUppercase);
		READBOOL(inifile, ShowBackgroundArt);
		READBOOL(inifile, ForceNewLabelsForNewProjects);
		READBOOL(inifile, DisplayStrikeChecksAsNormalCheckboxes);
		READINT(inifile, StrikeCheckedColor);
		READBOOL(inifile, CompileBeforeRun);
		READBOOL(inifile, UseFileDialogToBrowseFolders);
		READBOOL(inifile, EnforceAssemblyDllCompatibility);
		READBOOL(inifile, WarnAssemblyDllCompatibility);
		READHELPTYPE(inifile, HelpTypeId);
		READSTR(inifile, HelpPath);
		READBOOL(inifile, EnableHelpChoice);
		ReadMru();

		READ_SETTING_STR(PrefProjectParentFolder);
		READ_SETTING_STR(PrefCleanFolder);
		READ_SETTING_STR(PrefAdditionalModulesFolder);
		READ_SETTING_STR(PrefPublishFolder);
		READ_SETTING_STR(PrefArchiveFolder);
		READ_SETTING_STR(PrefMapImportExportFolder);
		READ_SETTING_STR(PrefImageImportExportFolder);
		READ_SETTING_STR(PrefAsmDllFolder);
		this->DecodePrefPaths(); // so clients have the actual app folder, not *APP
	}
	catch (std::exception & ex) {
		AfxMessageBox(CString("Unable to read app settings:\n") + ex.what());
	}
	catch (...) {
		AfxMessageBox("An unexpected exception prevented the app from reading app settings.");
	}
}

void AppSettings::Write()
{
	CString& inifile = m_inifile;
	try {
		WRITEBOOL(inifile, PromptOnClose);
		WRITEBOOL(inifile, PurgeLogFileOnStartup);
		WRITESTR(inifile, RunExe);
		WRITESTR(inifile, RunParams);
		WRITEBOOL(inifile, SaveHexAsUppercase);
		WRITEBOOL(inifile, ShowBackgroundArt);
		WRITEBOOL(inifile, ForceNewLabelsForNewProjects);
		WRITEBOOL(inifile, DisplayStrikeChecksAsNormalCheckboxes);
		WRITEINT(inifile, StrikeCheckedColor);
		WRITEBOOL(inifile, CompileBeforeRun);
		WRITEBOOL(inifile, UseFileDialogToBrowseFolders);
		WRITEBOOL(inifile, EnforceAssemblyDllCompatibility);
		WRITEBOOL(inifile, WarnAssemblyDllCompatibility);
		WRITEHELPTYPE(inifile, HelpTypeId);
		WRITESTR(inifile, HelpPath);
		WRITEBOOL(inifile, EnableHelpChoice);
		WriteMru();

		this->EncodePrefPaths(); // so that the file contains *APP
		WRITE_SETTING_STR(PrefProjectParentFolder);
		WRITE_SETTING_STR(PrefCleanFolder);
		WRITE_SETTING_STR(PrefAdditionalModulesFolder);
		WRITE_SETTING_STR(PrefPublishFolder);
		WRITE_SETTING_STR(PrefArchiveFolder);
		WRITE_SETTING_STR(PrefMapImportExportFolder);
		WRITE_SETTING_STR(PrefImageImportExportFolder);
		WRITE_SETTING_STR(PrefAsmDllFolder);
		this->DecodePrefPaths(); // so clients have the actual app folder, not *APP
	}
	catch (std::exception & ex) {
		AfxMessageBox(CString("Unable to write app settings:\n") + ex.what());
	}
	catch (...) {
		AfxMessageBox("An unexpected exception prevented the app from writing app settings.");
	}
}

void AppSettings::ReadMru()
{
	CString& inifile = m_inifile;
	try {
		READINT(inifile, MruMaxCount);
		if (MruMaxCount < MRULOWLIMIT) MruMaxCount = MRULOWLIMIT;
		else if (MruMaxCount > MRUMAXLIMIT) MruMaxCount = MRUMAXLIMIT;

		MruPaths = ReadIniKeyValues(inifile, "MRU");
		MruPaths.erase(std::remove_if(begin(MruPaths), end(MruPaths), [](auto str) { return str.IsEmpty(); }), end(MruPaths));
		if (MruPaths.size() > MruMaxCount) MruPaths.resize(MruMaxCount);
	}
	catch (std::exception & ex) {
		AfxMessageBox(CString("Unable to read app MRU settings:\n") + ex.what());
	}
	catch (...) {
		AfxMessageBox("An unexpected exception prevented the app from reading app MRU settings.");
	}
}

void AppSettings::WriteMru()
{
	CString& inifile = m_inifile;
	try {
		// Remove the section and rewrite it
		RemoveSection(inifile, "MRU");
		for (size_t st = 0; st < MruPaths.size(); ++st) {
			CString key;
			key.Format("mru%u", st);
			WriteIni(inifile, "MRU", key, MruPaths[st]);
		}

		WRITEINT(inifile, MruMaxCount);
	}
	catch (std::exception & ex) {
		AfxMessageBox(CString("Unable to write app MRU settings:\n") + ex.what());
	}
	catch (...) {
		AfxMessageBox("An unexpected exception prevented the app from writing app MRU settings.");
	}
}

void AppSettings::UpdateMru(CString mostrecentpath)
{
	// If the path already exists, remove ite existing entry.
	MruPaths.erase(std::remove(begin(MruPaths), end(MruPaths), mostrecentpath), end(MruPaths));

	// If we've already crossed the max count, remove the last (oldest) entry
	if (MruPaths.size() >= MruMaxCount) MruPaths.pop_back();

	// Now insert the updated entry at the front
	MruPaths.insert(begin(MruPaths), mostrecentpath);
}

void AppSettings::RemoveMru(CString pathtoremove)
{
	MruPaths.erase(std::remove(begin(MruPaths), end(MruPaths), pathtoremove));
}