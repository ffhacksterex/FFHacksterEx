#pragma once

#include <pair_result.h>
class CFFHacksterProject;
class IProgress;

class ProjectLoader_ini
{
public:
	ProjectLoader_ini(CFFHacksterProject& proj);
	pair_result<CString> ProjectLoader_ini::Load(CString projectpath, IProgress* progress);
	bool Save();

private:
	CFFHacksterProject& Project;
};
