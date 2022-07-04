#include "stdafx.h"
#include "rc_functions.h"
#include <sstream>
#include <regex>
#include "collection_helpers.h"
#include "path_functions.h"
#include "string_functions.h"


namespace Rc
{

	#define RT_EX_HFILE "RES_H"
	#define RT_EX_RCFILE "RES_RC"

	#define DEFSYMBOL(sym) {#sym, sym}


	// Utility functions
	namespace
	{
		const std::regex rxinclude{ "\\#include [\\\"|\\<](.*)[\\\"|\\>]" };
		const static std::string UNLABELLED_CONTROLNAMES = "EDITTEXT COMBOBOX LISTBOX";

		const std::regex rx_hfile{ "\\#include [\\\"|\\<](.*\\.h)[\\\"|\\>]" };

		enum class ProcessRcStatus {
			SkipDialog,
			Stop,
			Continue,
		};

		struct sProcessRcDialogState {
			std::string rcname;
			std::string currentline;
			std::string dialogname;
		};

		// Processes at least the current line.
		// Return 0 to continue processing, > 0 to stop, < 0 to skip the current dialog.
		using RcDialogProc = std::function<ProcessRcStatus(std::map<CString, UINT> & idmap, std::istream & is, sProcessRcDialogState & state)>;



		// Reads a header or resource script file that was saved as a resource in text format.
		// Note that this routine changes \r\n to \n.
		CString GetResourceTextData(HMODULE module, LPCSTR resourcename, LPCSTR resourcetype)
		{
			CString data;
			auto hrsrc = FindResource(module, resourcename, resourcetype);
			auto ressize = SizeofResource(module, hrsrc);
			// valid H/RC files will have enough bytes to test the first several characters
			if (ressize > 4) {
				auto hres = LoadResource(module, hrsrc);
				auto pres = (CHAR*)LockResource(hres);
				if (pres != nullptr) {
					//REFACTOR - there has to be a smarter way...
					//	There's no way to know if a maintainer stores these as ASCII or UNICODE.
					//	And also we're not really handling UTF-NN.
					//	For now, we check for particular values in the first byte or a 0 in the 4th byte.
					//  If it's a standard RC or H file, then it would be in the EN-US locale; the only way
					//	the 4th byte would be 0 is if it's a Unicode file (i.e. the 4th byte is the high byte
					//	of the 2nd Unicode character), and for these files would be in ASCII printable range (32-126),
					//	implying a high-byte of 0.
					if (*pres == -1 || *pres == -2 || pres[3] == 0) {
						// It's actually a unicode string, convert it now
						//CStringW wdata((WCHAR*)pres);
						//data = wdata;
						data = (WCHAR*)pres;
					}
					else {
						data = pres;
					}
					data.Replace("\r\n", "\n");
				}
				FreeResource(hres);
			}
			return data;
		}

		CString GetHFileData(HMODULE module, LPCSTR rcname)
		{
			return GetResourceTextData(module, rcname, RT_EX_HFILE);
		}

		CString GetRcFileData(HMODULE module, LPCSTR rcname)
		{
			return GetResourceTextData(module, rcname, RT_EX_RCFILE);
		}

		size_t AddToIdMap(std::map<CString, UINT> & idmap, CString hfiledata)
		{
			size_t added = 0;
			std::istringstream iss((LPCSTR)hfiledata);
			std::string line;
			while (getline(iss, line).good()) {
				iss >> std::ws;

				if (line.find("#define") != 0)
					continue;
				if (line.find("_APS_NEXT_") != std::string::npos)
					break;

				//format: #define ID_SOME_SYMBOL                  256
				std::string define, name;
				int value = 0;
				std::istringstream iline(line);
				iline >> define >> name >> value;

				// don't allow empty names, non-positive values, or duplicates
				if (name.empty()) continue;
				if (value < 1) continue;
				if (idmap.find(name.c_str()) != cend(idmap)) continue;

				idmap[name.c_str()] = value;
				++added;
			}
			return added;
		}

		std::vector<CString> GetAllRcFles(HMODULE hmod)
		{
			auto PrintResName = [](HMODULE, LPCSTR, LPSTR name, LONG_PTR lparam)
			{
				//DEVNOTE - Second param (LPCSTR type) is not used here
				std::vector<CString> & rcfiles = *(std::vector<CString>*)lparam;
				rcfiles.push_back(name);
				return TRUE;
			};

			std::vector<CString> rcfiles;
			EnumResourceNames(hmod, RT_EX_RCFILE, PrintResName, (LONG_PTR)&rcfiles);
			return rcfiles;
		}

		// Add mappings for some standard symbols (e.g. message box control IDs).
		void AddStandardSymbols(std::map<CString, UINT> & idmap)
		{
			std::vector<std::pair<CString, int>> symbols = {
				DEFSYMBOL(IDOK), DEFSYMBOL(IDCANCEL), DEFSYMBOL(IDABORT), DEFSYMBOL(IDRETRY), DEFSYMBOL(IDIGNORE), DEFSYMBOL(IDYES),
				DEFSYMBOL(IDNO), DEFSYMBOL(IDCLOSE), DEFSYMBOL(IDHELP), DEFSYMBOL(IDTRYAGAIN), DEFSYMBOL(IDCONTINUE), DEFSYMBOL(IDTIMEOUT)
			};
			for (auto sym : symbols) {
				//REFACTOR - this doesn't yet handle signed int vs. unsigned UINT... maybe change the map value type to UINT
				ASSERT(sym.second != 0); // ZERO ID values aren't allowed

				auto iter = idmap.find(sym.first);
				if (iter == cend(idmap) && (sym.second != 0))
					idmap[sym.first] = sym.second;
			}
		}

		void ProcessRcDialog(HMODULE hmod, CString rcname, RcDialogProc proc)
		{
			auto rcfiledata = GetRcFileData(hmod, rcname);
			std::istringstream is((LPCSTR)rcfiledata);
			std::string line;
			std::map<CString, UINT> idmap;
			sProcessRcDialogState state;
			state.rcname = rcname;

			AddStandardSymbols(idmap);

			while (std::getline(is, line).good()) {
				is >> std::ws;

				// read header files by name
				if (line.find("#include") != std::string::npos) {
					// Match a header file (must be at the start of the line)
					std::smatch match;
					if (std::regex_match(line, match, rxinclude)) {
						if (match.position(0) == 0) {
							auto resname = Paths::GetFileName(match[1].str().c_str());
							auto hfiledata = GetHFileData(hmod, resname);
							AddToIdMap(idmap, hfiledata);
						}
					}
				}
				else if (line.find(" DIALOGEX ") != std::string::npos || line.find(" DIALOG ") != std::string::npos) {
					// Get the dialog name, then retrieve its ID, but skip this if an entry is already present
					std::string dialogname;
					{
						std::istringstream idlg(line);
						idlg >> dialogname;
					}
					state.dialogname = dialogname;

					// Skip the opening of the dialog
					while (std::getline(is, line).good()) {
						if (line.find("BEGIN") == 0)
							break;
					}
					if (line.find("BEGIN") == 0) {
						// Walk through each line, breaking if we hit the end of either this dialog or the dialog group
						bool skip = false;
						bool stop = false;
						while (std::getline(is, line).good()) {
							if (line.find("END") == 0)
								break;
							if (skip)
								continue; // don't processing any more content from this dialog

							state.currentline = line;
							auto result = proc(idmap, is, state);

							if (result == ProcessRcStatus::SkipDialog)
								skip = true;
							else if (result == ProcessRcStatus::Stop) {
								stop = true;
								break;
							}
						}
						if (stop) break; // stop processing altogether
					}
				}
				else if (line.find("AFX_DIALOG_LAYOUT") != std::string::npos) {
					break; // no more dialog declarations after this
				}
			}
		}

	} // end unnamed namespace (Utility functions)


	IdDlgDataMap GetIdToDlgNmmeMap(HMODULE hmod)
	{

		IdDlgDataMap idtodlgmap;

		const auto RcAddIdToDlgMapping = [&](std::map<CString, UINT> & idmap, std::istream &, sProcessRcDialogState & state) -> ProcessRcStatus
		{
			// Get the ID for this dialogname
			auto iter = idmap.find(state.dialogname.c_str());
			if (iter != cend(idmap)) {
				// Is it in idtodlgmap? If so, then map the ID to this name and RC file
				if (idtodlgmap.find(iter->second) == cend(idtodlgmap)) {
					idtodlgmap[iter->second] = { state.dialogname.c_str(), state.rcname.c_str() };
				}
			}

			return ProcessRcStatus::SkipDialog; // no need to process anything else in this dialog
		};

		auto rcnames = GetAllRcFles(hmod);
		for (auto rcname : rcnames)
			ProcessRcDialog(hmod, rcname, RcAddIdToDlgMapping);

		return idtodlgmap;
	}

	const sIdToDlgData * GetDialogData(const IdDlgDataMap & imap, int dlgid)
	{
		auto iter = imap.find(dlgid);
		return iter != cend(imap) ? &iter->second : nullptr;
	}

	dialogmappings GetDialogControlMapping(const IdDlgDataMap& imap, int dlgid, HMODULE hmod)
	{
		auto dlgdata = Rc::GetDialogData(imap, dlgid);
		auto dlgmappings = dlgdata != nullptr
			? Rc::GetDialogControlMapping(hmod, *dlgdata)
			: Rc::dialogmappings{};
		return dlgmappings;
	}

	dialogmappings GetDialogControlMapping(HMODULE hmod, const sIdToDlgData& dlgdata)
	{
		auto rcfiledata = GetRcFileData(hmod, dlgdata.rcname);
		std::istringstream is((LPCSTR)rcfiledata);
		std::string line;
		dialogmappings idmap;
		dialogmappings ctlidmap;

		AddStandardSymbols(idmap);

		// Do this in passes:
		// read header files
		// find our dialog
		// extract the data for the dialog's controls

		while (std::getline(is, line).good()) {
			is >> std::ws;
			// don't read header files past the APSTUDIO_READONLY_SYMBOLS
			if (line.find("#undef APSTUDIO_READONLY_SYMBOLS") != std::string::npos)
				break;

			// read header files by name
			if (line.find("#include") == 0 && line.find(".h") != std::string::npos) {
				// Match a header file (must be at the start of the line)
				std::smatch match;
				if (std::regex_match(line, match, rx_hfile)) {
					if (match.position(0) == 0) {
						auto resname = Paths::GetFileName(match[1].str().c_str());
						auto hfiledata = GetHFileData(hmod, resname);
						AddToIdMap(idmap, hfiledata);
					}
				}
			}
		}

		// Find our dialog
		bool found = false;
		while (std::getline(is, line).good()) {
			if (line.find(" DIALOGEX ") != std::string::npos || line.find(" DIALOG ") != std::string::npos) {
				if (line.find(dlgdata.name) != std::string::npos) {
					found = true;
					break;
				}
			}
		}

		if (!found)
			return {};

		// Get our controla and find the maps
		while (std::getline(is, line).good()) {
			if (line.find("AFX_DIALOG_LAYOUT") != std::string::npos || line.find("END")== 0 )
				break; // no more dialog declarations after this

			std::istringstream iline(line);
			std::string type, dummy, ctlname;
			iline >> std::ws >> type >> std::ws;
			if (UNLABELLED_CONTROLNAMES.find(type) == std::string::npos) {
				std::getline(iline, dummy, ',') >> std::ws;
			}
			std::getline(iline, ctlname, ',') >> std::ws;

			CString strctl = ctlname.c_str();
			auto iter = idmap.find(strctl);
			if (iter != cend(idmap)) {
				ctlidmap[strctl] = iter->second;
			}
		}
		return ctlidmap;
	}

	CString GetControlNameFromId(HMODULE hmod, CString rcname, CString dialogname, UINT childid)
	{
		CString returnedcontrolname;

		const auto RcFindChildId = [&](std::map<CString, UINT> & idmap, std::istream &, sProcessRcDialogState & state) -> ProcessRcStatus
		{
			if (dialogname != state.dialogname.c_str()) return ProcessRcStatus::SkipDialog; // keep processing, but skip this dialog

			// If this line has a control ID, get its int value and compare to our child ID
			std::istringstream iline(state.currentline);
			std::string type, dummy, ctlname;
			iline >> std::ws >> type;
			if (UNLABELLED_CONTROLNAMES.find(type) == std::string::npos) {
				std::getline(iline, dummy, ',') >> std::ws;
			}
			std::getline(iline, ctlname, ',') >> std::ws;

			auto iter = idmap.find(ctlname.c_str());

			// If we find the ID, set the return controlname and stop processing. Otherwise, continue processing.
			if (iter != cend(idmap)) {
				if (iter->second == childid) {
					returnedcontrolname = ctlname.c_str();
					return ProcessRcStatus::Stop;
				}
			}
			return ProcessRcStatus::Continue;
		};

		ProcessRcDialog(hmod, rcname, RcFindChildId);
		return returnedcontrolname;
	}

}