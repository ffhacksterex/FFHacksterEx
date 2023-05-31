#include "stdafx.h"
#include "dialogue_helpers.h"
#include "logging_functions.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "string_functions.h"
#include "type_support.h"
#include <regex>

#define RX_VARNAME "[A-Za-z_][0-9A-Za-z_]+"

namespace dialogue_helpers
{
	bool HasLabel(std::string line)
	{
		const std::regex rx{ RX_VARNAME "\\:" };
		std::sregex_iterator end;
		std::sregex_iterator iter(cbegin(line), cend(line), rx);
		if (iter != end) {
			auto pos = iter->position();
			if (pos == 0 || line[pos - 1] != '@') {
				auto comm = line.find(';');
				return comm == std::string::npos || (size_t)pos < comm;
			}
		}
		return false;
	}

	sValuetype ReadValuetype(CString inipath, CString marker)
	{
		static const int PARTCOUNT = 2;
		auto line = Ini::ReadIni(inipath, "VALUETYPES", marker, "");
		auto parts = Strings::split(line, "|");

		ASSERT(parts.size() == PARTCOUNT);
		if (parts.size() != PARTCOUNT) {
			CString msg;
			msg.Format("Marker '%s' valuetype info contained %d parts instead of the expected %d.", marker, parts.size(), PARTCOUNT);
			THROWEXCEPTION(std::runtime_error, std::string((LPCSTR)msg));
		}

		sValuetype v;
		v.bits = atol(parts[0]);
		v.desc = parts[1];
		return v;
	}

	sValuetype ReadValuetype(CFFHacksterProject& proj, CString marker)
	{
		return ReadValuetype(proj.DialoguePath, marker);
	}

	sDialogueElement ReadElement(CFFHacksterProject& proj, CString routinename, int index)
	{
		CString elem;
		elem.Format("elem%d", index);
		return ReadElement(proj.DialoguePath, routinename, elem);
	}

	sDialogueElement ReadElement(CFFHacksterProject& proj, CString routinename, CString elemname)
	{
		return ReadElement(proj.DialoguePath, routinename, elemname);
	}

	static const int ELEM_PARTCOUNT = 4;

	//TODO - replace with the actual element struct
	sDialogueElement ReadElement(CString inipath, CString routinename, CString elemname)
	{
		throw std::runtime_error("Use the actual element instead.");

		auto line = Ini::ReadIni(inipath, routinename, elemname, "");
		auto parts = Strings::split(line, "|");

		ASSERT(parts.size() == ELEM_PARTCOUNT);
		if (parts.size() != ELEM_PARTCOUNT) {
			CString msg;
			msg.Format("Routine '%s' read Element '%s' contained %d parts instead of the expected %d.",
				routinename, elemname, parts.size(), ELEM_PARTCOUNT);
			THROWEXCEPTION(std::runtime_error, std::string((LPCSTR)msg));
		}

		//TODO - remove once dialogue.template updates are complete
#pragma message(PRAGHERE "remove the 0x` check once dialogue.template updates are complete.")
		if (parts[1] == "0x`") {
			CString msg;
			msg.Format("Routine '%s' Read Element '%s' routineoffset '%s' is invalid.", routinename, elemname, parts[1]);
			THROWEXCEPTION(std::runtime_error, std::string((LPCSTR)msg));
		}

		sDialogueElement e;
		e.marker = parts[0];
		e.routineoffset = strtoul(parts[1], nullptr, 16);
		e.paramindex = atol(parts[2]);
		e.comment = parts[3];
		return e;
	}

	void WriteElement(CFFHacksterProject& proj, CString routinename, int index, sDialogueElement e)
	{
		CString elemname;
		elemname.Format("elem%d", index);
		WriteElement(proj.DialoguePath, routinename, elemname, e);
	}

	void WriteElement(CFFHacksterProject& proj, CString routinename, CString elemname, sDialogueElement e)
	{
		WriteElement(proj.DialoguePath, routinename, elemname, e);
	}

	void WriteElement(CString inipath, CString routinename, CString elemname, sDialogueElement e)
	{
		CString roff;
		roff.Format("0x%X", e.routineoffset);
		CString param;
		param.Format("%d", e.paramindex);

		auto p4 = std::vector<CString>{
			e.marker,
			Types::hex(e.routineoffset),
			Types::dec(e.paramindex),
			e.comment
		};
		auto line = Strings::join(p4, "|");
		Ini::WriteIni(inipath, routinename, elemname, line);
	}
} // end namespace dialogue_helpers
