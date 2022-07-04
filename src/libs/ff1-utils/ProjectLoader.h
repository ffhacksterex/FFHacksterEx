#pragma once

#include <pair_result.h>
class CFFHacksterProject;
class IProgress;

class ProjectLoader
{
public:
	ProjectLoader(CFFHacksterProject & proj);
	~ProjectLoader();

	pair_result<CString> Load(CString projectpath, IProgress * progress = nullptr);
	bool Save();

	static pair_result<CString> InjectHacksterDAT(CString projectfile, CString hacksterfile);
	static pair_result<CString> IsProjectDllCompatible(CFFHacksterProject & project, CString asmdllpath);

protected:
	CFFHacksterProject & Project;
};


pair_result<CString> LoadProject(CFFHacksterProject & proj, CString projectpath, IProgress * progress = nullptr);
bool SaveProject(CFFHacksterProject & proj);
