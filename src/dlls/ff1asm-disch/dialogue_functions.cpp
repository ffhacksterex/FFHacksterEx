#include "stdafx.h"
#include "dialogue_functions.h"
#include "data_functions.h"
#include "shared.h"

#include <FilePathRemover.h>
#include <general_functions.h>
#include <ini_functions.h>
#include <io_functions.h>
#include <map_types.h>
#include <path_functions.h>
#include <set_types.h>
#include <string_functions.h>
#include <vector_types.h>
#include <dialogue_helpers.h>

using namespace disch_functions;
using namespace disch_functions::shared;
using namespace Ini;
using namespace Io;
using namespace std_assembly;
using namespace std_assembly::shared;
using namespace Strings;

// ################################################################
// INTERNAL DECLATATIONS

class dialogue_file
{
public:
	dialogue_file(const char* filepath, std::string routine);
	~dialogue_file();

	static std::string break_raw_tag(std::string rawtag, std::string & rest);
	std::string read_marker(std::string tag);
	int read_elem_index(std::string tag);
	int read_bits(std::string tag);

private:
	const char* m_filepath;
	std::string m_routine;
};

dialogue_file::dialogue_file(const char* filepath, std::string routine) : m_filepath(filepath), m_routine(routine)
{
}

dialogue_file::~dialogue_file()
{
}

//STATIC
std::string dialogue_file::break_raw_tag(std::string rawtag, std::string & rest)
{
	auto front = rawtag.find("|:");
	if (front != 0)
		THROWEXCEPTION(std::invalid_argument, "unsupported tag format '" + rawtag + "'");

	// Remove the leading tag symbols
	rawtag.erase(0, 2);

	// Strip the rest (e.g. true/false parts) and return those as 'rest'
	auto ispace = rawtag.find(' ');
	auto cleantag = (ispace != std::string::npos) ? rawtag.substr(0, ispace) : rawtag;
	rest = (ispace != std::string::npos) ? rawtag.substr(ispace + 1) : "";

	return cleantag;
}

std::string dialogue_file::read_marker(std::string tag)
{
	auto e = dialogue_helpers::ReadElement(m_filepath, m_routine.c_str(), tag.c_str());
	return (LPCSTR)e.marker;
}

int dialogue_file::read_elem_index(std::string tag)
{
	//TODO - use the paramindex member instead
	auto e = dialogue_helpers::ReadElement(m_filepath, m_routine.c_str(), tag.c_str());
	//return atol(e.value);
	return e.paramindex;
}

int dialogue_file::read_bits(std::string tag)
{
	auto v = dialogue_helpers::ReadValuetype(m_filepath, tag.c_str());
	return v.bits;
}


std::string make_section_name(std::string label)
{
	return std_assembly::shared::ensure_label_nocolon(label);
}



namespace disch_functions
{
	const std::regex rx_talkroutine{ "Talk\\_[A-Za-z0-9_]+" };

	const std::regex rx_markertest{ "\\|\\:dlg (" RX_VARNAME  ").*" };
	const std::regex rx_fanfare{ "\\s*\\?\\s*\\\"(.*)\\\"\\s*\\:\\s*\\\"(.*)\\\"" };

	const std::set<std::string> HARDCODED_ELEMENTS
	{
		{ "hctext" },
		{ "hcspr" },
		{ "hcobj" },
		{ "hcevent" },
		{ "hcbattle" },
		{ "hcitem" },
		{ "hcmap" },
		{ "hcweapon" },
		{ "hcarmor" },
		{ "hcitemorcanoe" },
		{ "hcitemormap" },
		{ "hcfanfare" },
		{ "hcnntele", 8 },
		{ "hcnotele", 8 },
		{ "hcuint8" },
		{ "hcbyte" },
	};


	std::string UpdateDlgOpValue(std::smatch & match, std::string line, const talkelement & element);
	bool is_ignorable(std::string elemname, unsigned long intvalue);

	bool HasTalkEndParseMarker(const char* dialoguepath, const char* testline);

	int get_element_bits(dialogue_file & dlgfile, std::string elem);
	std::string get_element(std::string marker);
	bool is_element_hardcoded(std::string elem);

	std::string ensure_label_nocolon(std::string origlabel);
	std::string ensure_label_colon(std::string origlabel);

	size_t read_talkmappingrow(std::istream & is, stdstringvector & talkroutinenames);
	size_t save_talkmappingrow(std::istream & is, std::ostream & os, size_t & index, const stdstringvector & talkroutinenames);

	void read_talk_mapping_list(std::istream & is, std::string mappinglabel, stdstringvector & routinetable);
	void save_talk_mapping_list(std::istream & is, std::ostream & os, std::string mappinglabel, const stdstringvector & routinenames);

	talkelementvector read_tagged_talk_handler(const char* dialoguepath, std::istream & is, std::string label, const stdstrintmap & varmap);
	void save_tagged_talk_handler(const char* dialoguepath, std::istream & is, std::ostream & os, std::string label, const stdstrintmap & varmap, const talkelementvector & elements);
}


// ################################################################
// INTERNAL IMPLEMENTATIONS

namespace disch_functions
{
	std::string UpdateDlgOpValue(std::smatch & match, std::string line, const talkelement & element)
	{
		auto matchpos = match.position(1);
		auto matchlen = match[1].length();

		auto pre = line.substr(0, matchpos);
		auto post = line.substr(matchpos + matchlen);

		// If the newdata matches the supplied name's value, go with that as the new value.
		// Otherwise, convert the newdata to a string using the base of the old value.
		std::string newvalue = element.value;
		return !newvalue.empty() ? pre + newvalue + post : "";
	}

	bool is_ignorable(std::string elemname, unsigned long intvalue)
	{
		bool value = (elemname == "hcfanfare" && intvalue == ASM_IGNORE);
		return value;
	}

	bool HasTalkEndParseMarker(const char* dialoguepath, const char* testline)
	{
		auto endmarker = ReadIni(dialoguepath, "Label_EndParsingMarker", "value", "");
		if (endmarker.IsEmpty())
			THROWEXCEPTION(std::runtime_error, "Label_EndParsingMarker not defined in the dialogueini file");

		return strstr(testline, endmarker) != nullptr;
	}

	int get_element_bits(dialogue_file & dlgfile, std::string elem)
	{
		auto bits = dlgfile.read_bits(elem);
		if (bits == -1)
			THROWEXCEPTION(std::runtime_error, "unable to read bits for marker " + elem);
		
		return bits;
	}

	std::string get_element(std::string marker)
	{
		return marker;
	}

	bool is_element_hardcoded(std::string elem)
	{
		return elem.find("hc") == 0;
	}

	std::string ensure_label_nocolon(std::string origlabel)
	{
		std::string label = origlabel;
		if (!label.empty() && label.back() == ':') label.pop_back();
		return label;
	}

	std::string ensure_label_colon(std::string origlabel)
	{
		std::string newlabel = (!origlabel.empty() && origlabel.back() != ':') ? origlabel + ":" : origlabel;
		return newlabel;
	}

	size_t read_dlgline(dialogue_file & dlgfile, std::istream & is, const stdstrintmap & varmap, talkelementvector & elements)
	{
		UNREFERENCED_PARAMETER(varmap);

		const auto MakeCleanComment = [](std::string comm) {
			std::string newcomm = comm;
			if (!newcomm.empty() && newcomm[0] == ';') newcomm.erase(0, 1);
			if (!newcomm.empty() && newcomm.back() == ';') newcomm.pop_back();
			return newcomm;
		};

		std::string line, comment, rawtag, truefalse;
		get_and_break_line(is, line, comment, rawtag);
		if (trim(line).empty())
			return 0;

		auto fullline = line + comment + rawtag;
		auto tag = dialogue_file::break_raw_tag(rawtag, truefalse);
		auto marker = dlgfile.read_marker(tag);
		if (marker.empty()) {
			WarnHere << " - '" << rawtag << "' has no matching entry in the dialogueini, this line won't be processed" << std::endl;
			return 0; // identifies an element, but there's no matching entry inthe dialogueini
		}

		std::string elemcomment = MakeCleanComment(comment);
		std::string elem;
		std::smatch match;
		if (marker == "hcfanfare") {
			if (!std::regex_match(truefalse, match, rx_fanfare))
				THROWEXCEPTION(std::runtime_error, "no fanfare match found on line: " + fullline);
			if (match.size() != 3)
				THROWEXCEPTION(std::runtime_error, "didn't find 3 fanfare match components on line: " + fullline);

			auto truepart = match[1].str();
			auto falsepart = match[2].str();
			int fanfare = ASM_IGNORE;

			if (is_cond_match(line, truepart)) fanfare = ASM_TRUE;
			if (is_cond_match(line, falsepart)) fanfare = ASM_FALSE;

			int bits = dlgfile.read_bits(marker); //dlgfile.read_bits(tag); //get_element_bits(marker);
			elements.push_back({ marker, tag, std::to_string(fanfare), bits, elemcomment });
			return 1;
		}
		else
		{
			if (is_element_hardcoded(marker)) {
				if (!std::regex_match(line, match, rx_opval_hardcode))
					THROWEXCEPTION(std::runtime_error, "no marker match found on line: " + fullline);
				if (match.size() != 2)
					THROWEXCEPTION(std::runtime_error, "didn't find marker name on line: " + fullline);

				int bits = dlgfile.read_bits(marker); //dlgfile.read_bits(tag); //get_element_bits(marker);
				elements.push_back({ marker, tag, match[1].str(), bits, elemcomment }); // could be an int value or a constant
				return 1;
			}
			else {
				auto paramindex = dlgfile.read_elem_index(tag);
				if (paramindex == -1)
					THROWEXCEPTION(std::runtime_error, "unable to get param index from tag " + tag + " on line: " + fullline);

				int bits = dlgfile.read_bits(marker); //int bits = dlgfile.read_bits(tag);
				elements.push_back({ marker, tag, std::to_string(paramindex), bits, elemcomment }); // could be an int value or a constant
				return 1;
			}
		}
		return 0;
	}

	size_t save_dlgline(dialogue_file & dlgfile, std::istream & is, std::ostream & os, size_t & elemindex, const stdstrintmap & varmap, const talkelementvector & elements)
	{
		UNREFERENCED_PARAMETER(varmap);

		std::string line, comment, rawtag, truefalse;
		get_and_break_line(is, line, comment, rawtag);
		if (trim(line).empty())
			return 0;

		auto fullline = line + comment + rawtag;
		auto tag = dialogue_file::break_raw_tag(rawtag, truefalse);
		auto marker = dlgfile.read_marker(tag);
		if (marker.empty()) {
			ErrorHere << " - '" << rawtag << "' has no matching entry in the dialogueini, this line won't be processed" << std::endl;
			return 0; // identifies an element, but there's no matching entry inthe dialogueini
		}

		// If the line contains a marker, then replace the line portion with update info as dictated by the element
		auto index = elemindex++;
		auto & refelem = elements[index];
		if (refelem.element != tag)
			throw std::runtime_error("elem " + refelem.element + " does not match the expected type " + tag);

		// Handle the fanfare ignore case right up front
		//DEVNOTE - std::stoul throws an exception instead of returning 0 if no conversion is performed
		auto intvalue = std::strtoul(refelem.value.c_str(), nullptr, 10);
		if (is_ignorable(tag, intvalue)) {
			write_next_line(os, line) << comment << rawtag;
			return 0;
		}

		std::smatch match;
		if (marker == "hcfanfare") {
			if (!std::regex_match(truefalse, match, rx_fanfare))
				THROWEXCEPTION(std::runtime_error, "no fanfare match found on line: " + fullline);
			if (match.size() != 3)
				THROWEXCEPTION(std::runtime_error, "didn't find 3 fanfare match components on line: " + fullline);
			if (index >= elements.size())
				THROWEXCEPTION(std::invalid_argument, "index " + std::to_string(index) + " is beyond the end of the elements list");

			auto truepart = match[1].str();
			auto falsepart = match[2].str();

			auto replacement = GetConditionalReplacement(line, intvalue, truepart, falsepart);
			write_next_line(os, replacement) << comment << rawtag;
			return 1;
		}
		else
		{
			if (is_element_hardcoded(marker))
			{
				// for now, all other markers use the standard dlg marker format
				if (!std::regex_match(line, match, rx_opval_hardcode))
					THROWEXCEPTION(std::runtime_error, "no marker match found on line: " + fullline);
				if (match.size() != 2)
					THROWEXCEPTION(std::runtime_error, "didn't find marker name on line: " + fullline);

				auto replacement = UpdateDlgOpValue(match, line, refelem);
				if (replacement.empty()) return (size_t)-1;

				size_t origwidth = line.size();
				if (!comment.empty() || !marker.empty())
					write_next_line(os, right_pad(trimright(replacement), (int)origwidth)) << comment << rawtag;
				else
					write_next_line(os, replacement) << comment << rawtag;
				return 1;
			}
			else {
				// parameterized elements actually save their changes to the parameters stored in game ROM, so this
				// element's assembly source doesn't change. Return 1 to acknowledge that we don't have to process
				// it, but should be counted in the processed element count.
				write_next_line(os, line) << comment << rawtag;
				return 1;
			}
		}

		write_next_line(os, line) << comment << marker;
		return 0;
	}


	size_t read_talkmappingrow(std::istream & is, stdstringvector & talkroutinenames)
	{
		std::string line;
		std::getline(is, line);

		if (!is_asmdatarow(line)) return 0;

		std::sregex_iterator end;
		std::sregex_iterator it(cbegin(line), cend(line), rx_talkroutine);
		if (it == end) return 0;

		size_t count = 0;
		for (; it != end; ++it, ++count) {
			talkroutinenames.push_back(it->str());
		}
		return count;
	}

	size_t save_talkmappingrow(std::istream & is, std::ostream & os, size_t & index, const stdstringvector & talkroutinenames)
	{
		std::string line;
		std::getline(is, line);

		std::sregex_iterator end;
		std::sregex_iterator it(cbegin(line), cend(line), rx_talkroutine);
		if (it == end) {
			write_next_line(os, line);
			return 0;
		}

		write_next_line(os, "");

		size_t count = 0;
		decltype(it) last = it;
		for (; it != end; ++it, ++count, ++index) {
			if (index >= talkroutinenames.size())
				throw std::out_of_range("talkroutine name can't write to index " + std::to_string(index) + " because it's beyond the end of the list of names");

			os << it->prefix() << talkroutinenames[index];
			last = it;
		}

		os << last->suffix();

		return count;
	}

	void read_talk_mapping_list(std::istream & is, std::string mappinglabel, stdstringvector & routinetable)
	{
		std::string mainline;
		std::getline(is, mainline);

		mappinglabel = ensure_label_colon(mappinglabel);
		if (get_label(mainline, getlabelstyle::includecolon) != mappinglabel)
			THROWEXCEPTION(std::invalid_argument, "didn't find mappinglabel '" + mappinglabel + "' on line " + mainline);

		while (is && !is.eof()) {
			auto line = preview_next_line(is);
			if (has_label(line)) break;

			stdstringvector names;
			read_talkmappingrow(is, names);
			append(routinetable, names);
		}
	}

	void save_talk_mapping_list(std::istream & is, std::ostream & os, std::string mappinglabel, const stdstringvector & routinenames)
	{
		std::string mainline;
		std::getline(is, mainline);

		mappinglabel = ensure_label_colon(mappinglabel);
		if (get_label(mainline, getlabelstyle::includecolon) != mappinglabel)
			THROWEXCEPTION(std::invalid_argument, "didn't find mappinglabel '" + mappinglabel + "' on line " + mainline);

		write_next_line(os, mainline);

		size_t index = 0;
		while (is && !is.eof()) {
			auto line = preview_next_line(is);
			if (has_label(line)) break;

			if (is_asmdatarow(line))
				save_talkmappingrow(is, os, index, routinenames);
			else {
				std::getline(is, line);
				write_next_line(os, line);
			}
		}
	}



	// Reads lines until encountering a new label or EOF, then returns all elements read during the process.

	talkelementvector read_tagged_talk_handler(const char* dialoguepath, std::istream & is, std::string label, const stdstrintmap & varmap)
	{
		// In this implementation, talk handlers always put the label on its own line.
		std::string labelline;
		std::getline(is, labelline);

		label = ensure_label_colon(label);
		if (get_label(labelline, getlabelstyle::includecolon) != label)
			THROWEXCEPTION(std::invalid_argument, "read_talk_handler: label " + label + " isn't found on the current line: " + labelline);

		dialogue_file dlgfile(dialoguepath, make_section_name(label));

		// Each line calls read_dlg if it's a dialog line
		talkelementvector elements;
		size_t count = 0;
		while (is && !is.eof()) {
			auto testline = preview_next_line(is);

			// before anything else happens, if the endparsing marker is on this line, parsing stops at this point.
			// the line with the endparsing marker is left for the caller to handle.
			if (HasTalkEndParseMarker(dialoguepath, testline.c_str()))
				break;

			if (has_label(testline)) break;

			auto elemstart = testline.find("|:elem");
			if (elemstart != std::string::npos) {
				auto addcount = read_dlgline(dlgfile, is, varmap, elements); // read_dlgline never returns -1 cast as size_t, so no check's needed
				count += addcount;
				if (addcount == 0) ErrorHere << "   routine '" << label << "' failed to read line with element " << testline.substr(elemstart + 2) << std::endl;
			}
			else
				discard_line(is);
		}

		if (count != elements.size())
			THROWEXCEPTION(std::runtime_error, "read_talk_handler: found " + std::to_string(elements.size()) + " elements, but processed " + std::to_string(count));
		return elements;
	}

	void save_tagged_talk_handler(const char* dialoguepath, std::istream & is, std::ostream & os, std::string label, const stdstrintmap & varmap, const talkelementvector & elements)
	{
		// In this implementation, talk handlers always put the label on its own line.
		std::string labelline;
		std::getline(is, labelline);
		if (get_label(labelline, getlabelstyle::stripcolon) != label)
			THROWEXCEPTION(std::invalid_argument, "save_talk_handler: label " + label + " isn't found on the current line: " + labelline);

		write_next_line(os, labelline);

		dialogue_file dlgfile(dialoguepath, make_section_name(label));

		// Each line calls save_dlg if it's a dialog line
		size_t count = 0;
		size_t index = 0;
		while (is && !is.eof()) {
			auto testline = preview_next_line(is);

			// before anything else happens, if the endparsing marker is on this line, parsing stops at this point.
			// the line with the endparsing marker is left for the caller to handle.
			if (HasTalkEndParseMarker(dialoguepath, testline.c_str()))
				break;

			if (has_label(testline)) break;

			auto elemstart = testline.find("|:elem");
			if (elemstart != std::string::npos) {
				if (index >= elements.size())
					THROWEXCEPTION(std::runtime_error, "index " + std::to_string(index) + " exceeds element list count " + std::to_string(elements.size()));

				auto addcount = save_dlgline(dlgfile, is, os, index, varmap, elements); // read_dlgline never returns -1 cast as size_t, so no check's needed
				count += addcount;
				if (addcount == 0) WarnHere << "   routine '" << label << "' skipped processing unlinked line with element " << testline.substr(elemstart + 2) << std::endl;
			}
			else {
				std::string line;
				std::getline(is, line);
				write_next_line(os, line);
			}
		}

		if (count != elements.size())
			THROWEXCEPTION(std::runtime_error, "found " + std::to_string(elements.size()) + " elements, but processed " + std::to_string(count));
		if (index != elements.size())
			THROWEXCEPTION(std::runtime_error, "found " + std::to_string(elements.size()) + " elements, but index went to " + std::to_string(index));
	}



	// PUBLIC IMPLEMENTATIONS

	namespace dialogue
	{
		std::string read_dialogue_data(const char * asmfile, const char* dialoguepath, const char * szmappinglabel, const char * varmaptext)
		{
			std::ifstream is;
			is.open(asmfile);

			std::string mappinglabel = szmappinglabel;
			stdstrintmap varmap;
			{
				std::istringstream ivar(varmaptext);
				ivar >> varmap;
			}

			stdstringvector routinenames;
			talknamevectormap routinemappings;

			const auto AddDefs = [&](std::string label) {
				if (routinemappings.find(label) != end(routinemappings)) {
					ErrorHere << "Can't add duplicate talk routine handler label " << label << std::endl;
					THROWEXCEPTION(std::runtime_error, "Can't add duplicate talk routine handler label " + label);
				}

				routinemappings[label] = read_tagged_talk_handler(dialoguepath, is, label, varmap);
			};

			//DEVNOTE - if the handler is enabled on the main dialog, send text to it with this message.
			//SendMessage(HWND_BROADCAST, RWM_FFHACKSTEREX_SHOWWAITMESSAGE, NULL, (LPARAM)"Loading assembly source...");

			mappinglabel = ensure_label_colon(mappinglabel);

			while (is && !is.eof()) {
				auto line = preview_next_line(is);
				auto label = get_label(line, getlabelstyle::includecolon);

				// before anything else happens, if the endparsing marker is on this line, parsing stops at this point.
				// discard the line and bail.
				if (HasTalkEndParseMarker(dialoguepath, line.c_str())) {
					discard_line(is);
					break;
				}

				// if this is the routine mapping table, build that table now.
				// if this is a label with "Talk_", then it's a routine, gather the routine components (if any).
				// Otherwise, discard the line
				if (label == mappinglabel)
					read_talk_mapping_list(is, mappinglabel, routinenames);
				else if (label.find("Talk_") != std::string::npos)
					AddDefs(ensure_label_nocolon(label));
				else
					discard_line(is);
			}

			// Format the names vector and routine mappings into a string.
			std::ostringstream ostr;
			ostr << routinenames << std::endl;
			ostr << routinemappings;
			return ostr.str();
		}

		void write_dialogue_data(const char* asmfile, const char* dialoguepath, const char* varmaptext, const char* inputtext)
		{
			auto temppath = asmfile + std::string(".talk");
			FilePathRemover remover(temppath);
			{
				std::ifstream is;
				if (!Io::open_file(asmfile, is))
					THROWEXCEPTION(std::runtime_error, "unable to open the input assembly file " + std::string(asmfile));

				std::ofstream os;
				create_asmstream(os, temppath);

				stdstrintmap varmap;
				std::istringstream istr(varmaptext);
				istr >> varmap;

				// The names vector and routine mappings cross module boundaries as a long, newline-separated string.
				std::string mappinglabel;
				stdstringvector routinenames;
				talknamevectormap routinemappings;
				init_stringstream(istr, inputtext);

				istr >> mappinglabel >> std::ws; // removes \n
				istr >> routinenames >> std::ws; // removes \n
				istr >> routinemappings;

				const auto SaveHandler = [&](const std::string & labelname) {
					auto itmap = routinemappings.find(labelname);
					if (itmap == cend(routinemappings))
						throw std::runtime_error("can't save talk handler " + labelname + ": it's not in the handler mapping collection");

					save_tagged_talk_handler(dialoguepath, is, os, labelname, varmap, itmap->second);
				};

				mappinglabel = ensure_label_colon(mappinglabel);

				while (is && !is.eof()) {
					auto line = preview_next_line(is);

					// before anything else happens, if the endparsing marker is on this line, parsing stops at this point.
					// copy the line, ignoring any other commands on it, and bail.
					if (HasTalkEndParseMarker(dialoguepath, line.c_str())) {
						copy_next_line(is, os);
						break;
					}

					auto label = get_label(line, getlabelstyle::includecolon);
					auto labelname = ensure_label_nocolon(label);

					// if this is a label with "Talk_", then it's a routine, gather the routine components (if any)
					// if this is the mapping table, gather the mapping components.
					if (label == mappinglabel) {
						save_talk_mapping_list(is, os, mappinglabel, routinenames);
					}
					else if (labelname.find("Talk_") != std::string::npos) {
						SaveHandler(labelname);
					}
					else {
						copy_next_line(is, os);
					}
				}
			}

			// The above enclosing scope's dtors should have closed the files and detached the streams.
			// If the file copy fails, except for a race condition, it shouldn't be due to the files still being open.
			if (!Paths::FileCopy(temppath.c_str(), asmfile))
				THROWEXCEPTION(std::ios_base::failure, "failed to copy file '" + temppath + "' to " + std::string(asmfile));
		}
	}
}