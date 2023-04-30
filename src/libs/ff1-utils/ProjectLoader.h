#pragma once

#include <pair_result.h>
class CFFHacksterProject;
class FFH2Project;
class IProgress;

class ProjectLoader
{
public:
	ProjectLoader(FFH2Project & prj2, CFFHacksterProject & proj);
	~ProjectLoader();

	CString Load(CString projectpath, IProgress* progress = nullptr);
	bool Save();

	static pair_result<CString> InjectHacksterDAT(CString projectfile, CString hacksterfile);
	static pair_result<CString> IsProjectDllCompatible(CFFHacksterProject & project, CString asmdllpath);

protected:
	FFH2Project& Proj2;
	CFFHacksterProject & Project;
};


pair_result<CString> LoadProject(FFH2Project & prj2, CFFHacksterProject & proj, CString projectpath, IProgress * progress = nullptr);
bool SaveProject(FFH2Project& prj2, CFFHacksterProject & proj);
