#pragma once

#include <afx.h> // for CString

struct sProjectGeneratorValues
{
public:
	CString parentfolder;
	CString projectname;
	CString romfile;
	CString publishromfile;
	CString datfile;
	CString addlmodfolder;
	bool forcenewlabels;
};
