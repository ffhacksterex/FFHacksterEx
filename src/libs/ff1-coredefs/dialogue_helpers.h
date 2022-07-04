#pragma once
#include <vector>
class CFFHacksterProject;

namespace dialogue_helpers
{
	bool HasLabel(std::string line);

	struct sValuetype {
		int bits = 0;
		CString desc;
	};

	sValuetype ReadValuetype(CString inipath, CString marker);
	sValuetype ReadValuetype(CFFHacksterProject& proj, CString marker);

	struct sDialogueElement {
		CString marker;
		int routineoffset = 0;
		int paramindex = -1;
		CString comment;

		bool isHardcoded() const { return marker.Find("hc") == 0; }
	};

	sDialogueElement ReadElement(CFFHacksterProject& proj, CString routinename, int index);
	sDialogueElement ReadElement(CFFHacksterProject& proj, CString routinename, CString elemname);
	sDialogueElement ReadElement(CString inipath, CString routinename, CString elemname);
	void WriteElement(CFFHacksterProject& proj, CString routinename, int index, sDialogueElement e);
	void WriteElement(CFFHacksterProject& proj, CString routinename, CString elemname, sDialogueElement e);
	void WriteElement(CString inipath, CString routinename, CString elemname, sDialogueElement e);
}
