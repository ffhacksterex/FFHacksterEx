#pragma once

#include "pair_result.h"
class CFFHacksterApp;
class AppSettings;
struct sProjectGeneratorValues;

class CFFHacksterGenerator
{
public:
	CFFHacksterGenerator(AppSettings & appsettings);
	~CFFHacksterGenerator();

	AppSettings* AppStgs = nullptr;

	CString ProcessAction(CString curaction, CString AppIni, CString actionparam);

	pair_result<CString> EditROM(CString rompath);
	pair_result<CString> CreateROMProject();
	pair_result<CString> CreateAsmProject();
	pair_result<CString> OpenProject();
	pair_result<CString> UnzipProject();
	pair_result<CString> OpenRecentFile(CString recentpath);

private:
	static CString GetAppFilePrefix();
	static CString FormatAppFileName(CString filetype);

	pair_result<CString> CopyAppFileType(CString filetype, CString folder, CString projectname);
	pair_result<CString> CreateEditorLabels(CString folder, CString newfilename, bool forcerecreation);

	CString ExecuteActionResult(CString curaction, CString resultstring, CString AppIni); // filename or command
	static pair_result<CString> UnzipAndOpenProject(CString pathname, CString dotextension);
	static CString BuildEditRomProjectPath(CString rompath);

	pair_result<CString> DoCreateRomProject(const sProjectGeneratorValues & genvalues);
	pair_result<CString> SelectAndWriteEditors(CString projectfolder, CString projectpath, CString additionalmodulesfolder, CString editorsettingspath);
};
