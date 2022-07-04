#include "stdafx.h"
#include "data_functions.h"
#include "general_functions.h"
#include "shared.h"

// This file contains an implemention that serializes assembly in the Disch format.
// That is, any disassembly that is based on Disch's v1.0 release that uses the
// annotation marker format supported by this editor.
#include <FilePathRemover.h>
#include <interop_assembly_interface.h>
#include <io_functions.h>
#include <path_functions.h>
#include <string_functions.h>
#include <vector_types.h>

using namespace Io;
using namespace std_assembly;
using namespace std_assembly::shared;
using namespace Strings;
using namespace disch_functions::shared;


// ################################################################
// INTERNAL DECLARATIONS

namespace disch_functions
{
	std::string UpdateOpValue(std::smatch & match, std::string line, int newdata, const asm_sourcemapping & mapping, const stdstrintmap & varmap);

	asm_tabletype get_asmtabletype(std::string line);
	std::string get_asmtable_prefix(asm_tabletype type);
	int get_asmtable_bytelength(asm_tabletype type);

	std::string make_mappingname(asm_sourcetype type, std::string name);
	stdstringvector get_mappingnames(const asmsourcemappingvector & mappings);
	bool found_mapping(const asm_sourcemapping & mapping, std::string line);

	size_t get_mappingpos(std::string line, const asm_sourcemapping & mapping);
	size_t get_mappingpos(std::string line, std::string markertext);

	bytevector read_datarow(std::string line, const stdstrintmap & varmap);
	bytevector read_datarow(std::string line, const stdstrintmap& varmap, size_t count);
	size_t save_datarow(std::ostream& os, std::string line, const stdstrintmap& varmap, const asm_sourcemapping& mapping, const bytevector& ROM, StepPos& steppos, size_t count = (size_t)-1);

	size_t do_table_read(std::istream & is, const asm_sourcemapping & mapping, const stdstrintmap & varmap, bytevector & ROM);
	size_t do_labelval_read(std::istream& is, const asm_sourcemapping& mapping, const stdstrintmap& varmap, bytevector& ROM);
	size_t read_opval(std::istream & is, const asm_sourcemapping & mapping, const stdstrintmap & varmap, bytevector & ROM);
	size_t read_cond(std::istream & is, const asm_sourcemapping & mapping, const stdstrintmap & varmap, bytevector & ROM);
	size_t read_label(std::istream & is, const asm_sourcemapping & mapping, const stdstrintmap & varmap, bytevector & ROM);
	size_t read_table(std::istream & is, const asm_sourcemapping & mapping, const stdstrintmap & varmap, bytevector & ROM);

	size_t save_opval(std::istream & is, std::ostream & os, const asm_sourcemapping & mapping, const stdstrintmap & varmap, const bytevector & ROM);
	size_t save_cond(std::istream & is, std::ostream & os, const asm_sourcemapping & mapping, const stdstrintmap & varmap, const bytevector & ROM);
	size_t save_label(std::istream & is, std::ostream & os, const asm_sourcemapping & mapping, const stdstrintmap & varmap, const bytevector & ROM);
	size_t save_table(std::istream & is, std::ostream & os, const asm_sourcemapping & mapping, const stdstrintmap & varmap, const bytevector & ROM);
}



// ################################################################
// INTERNAL IMPLEMENTATIONS

namespace disch_functions
{
	std::string UpdateOpValue(std::smatch & match, std::string line, int newdata, const asm_sourcemapping & mapping, const stdstrintmap & varmap)
	{
		auto matchpos = match.position(1);
		auto matchlen = match[1].length();

		auto pre = line.substr(0, matchpos);
		auto post = line.substr(matchpos + matchlen);
		auto oldvalue = match[1].str();

		// If the newdata matches the supplied name's value, go with that as the new value.
		// Otherwise, convert the newdata to a string using the base of the old value.
		std::string newvalue;
		if (mapping.varnamesforvalues.size() > 0) {
			// if the values match, use the name instead of the value
			auto itmap = varmap.find(mapping.varnamesforvalues[0]);
			if (itmap != cend(varmap) && itmap->second == newdata)
				newvalue = itmap->first;
		}
		if (newvalue.empty()) {
			//UGH... this could be a constant, which means it won't have a numeric prefix since the map stores ints
			//		possibly add base to the mapping entry, as currently all constants will be base 10
			auto base = get_num_base_or_default(oldvalue, 10);
			newvalue = int_to_string(newdata, base, 1);
		}
		return !newvalue.empty() ? pre + newvalue + post : "";
	}

	asm_tabletype get_asmtabletype(std::string line)
	{
		auto IsMarkedLine = [&](std::string prefix) { return IsUncommentedDataRow(line, prefix); };

		if (IsMarkedLine(".BYTE")) return asm_tabletype::bytes;
		if (IsMarkedLine(".WORD")) return asm_tabletype::words;
		if (IsMarkedLine(".FARADDR")) return asm_tabletype::faraddrs;

		throw std::invalid_argument("no recognized asm_tabletype is defined on the line " + line);
	}

	std::string get_asmtable_prefix(asm_tabletype type)
	{
		switch (type)
		{
		case asm_tabletype::bytes: return std::string(".BYTE");
		case asm_tabletype::words: return std::string(".WORD");
		case asm_tabletype::faraddrs: return std::string(".FARADDR");
		case asm_tabletype::none:
			throw std::invalid_argument("asm_tabletype::none does not have a valid prefix");
		}

		throw std::invalid_argument("No prefix for unrecognized asm_tabletype value " + std::to_string(static_cast<int>(type)));
	}

	int get_asmtable_bytelength(asm_tabletype type)
	{
		switch (type)
		{
		case asm_tabletype::bytes: return 1;
		case asm_tabletype::words: return 2;
		case asm_tabletype::faraddrs: return 3;
		case asm_tabletype::none:
			throw std::invalid_argument("asm_tabletype::none does not have a valid bytevalue");
		}

		throw std::invalid_argument("No bytelength for unrecognized asm_tabletype value " + std::to_string(static_cast<int>(type)));
	}

	std::string make_mappingname(asm_sourcetype type, std::string name)
	{
		if (type == asm_sourcetype::label)
			return name + ":";
		if (type == asm_sourcetype::labelval)
			return name + ":";
		return get_asmsource_prefix(type) + " " + name;
	}

	stdstringvector get_mappingnames(const asmsourcemappingvector & mappings)
	{
		stdstringvector names(mappings.size());
		std::transform(cbegin(mappings), cend(mappings), begin(names), [](const auto & m) { return make_mappingname(m.type, m.name); });
		return names;
	}

	bool found_mapping(const asm_sourcemapping & mapping, std::string line)
	{
		auto name = make_mappingname(mapping.type, mapping.name);
		switch (mapping.type) {
		case asm_sourcetype::label:
			return !is_asmcomment(line) && line.find(mapping.name + ":") != std::string::npos;

		case asm_sourcetype::labelval:
			return !is_asmcomment(line) && line.find(mapping.name + ":") != std::string::npos;

		case asm_sourcetype::table:
			if (!is_asmcomment(line) && line.find(mapping.name + ":") != std::string::npos)
				return true;
			// fall-through is intentional, table also supports explicitly markers
		case asm_sourcetype::opval:
		case asm_sourcetype::cond:
		case asm_sourcetype::dlg:
			return line.find(make_mappingname(mapping.type, mapping.name)) != std::string::npos;
		}
		throw std::domain_error("mapping " + mapping.name + " has an invalid type");
	}

	size_t get_mappingpos(std::string line, const asm_sourcemapping & mapping)
	{
		return get_mappingpos(line, make_mappingname(mapping.type, mapping.name));
	}

	size_t get_mappingpos(std::string line, std::string markertext)
	{
		return line.find(markertext);
	}


	bytevector read_datarow(std::string line, const stdstrintmap & varmap)
	{
		const std::regex rx_datarow{ RX_TABLETERM };

		std::string comment = strip_comment(line);

		auto tabletype = get_asmtabletype(line);
		auto tableprefix = get_asmtable_prefix(tabletype);
		auto bytelength = get_asmtable_bytelength(tabletype);
		auto mark = line.find(tableprefix);

		if (tableprefix.empty())
			throw std::runtime_error("an empty tabletype prefix is not allowed on line " + line);
		if (mark == std::string::npos)
			throw std::runtime_error("unable to find tabletype prefix " + tableprefix + " on line " + line);

		bytevector bytes;
		auto startiter = cbegin(line) + mark + tableprefix.length();
		std::sregex_iterator end;
		std::sregex_iterator iter(startiter, cend(line), rx_datarow);
		for (; iter != end; ++iter) {
			auto field = iter->str();
			int value = translate_value(field, varmap);
			append_to(value, bytelength, bytes);
		}
		return bytes;
	}

	bytevector read_datarow(std::string line, const stdstrintmap& varmap, size_t count)
	{
		ASSERT(count > 0);
		const std::regex rx_datarow{ RX_TABLETERM };

		std::string comment = strip_comment(line);

		auto tabletype = get_asmtabletype(line);
		auto tableprefix = get_asmtable_prefix(tabletype);
		auto bytelength = get_asmtable_bytelength(tabletype);
		auto mark = line.find(tableprefix);

		if (tableprefix.empty())
			throw std::runtime_error("an empty tabletype prefix is not allowed on line " + line);
		if (mark == std::string::npos)
			throw std::runtime_error("unable to find tabletype prefix " + tableprefix + " on line " + line);

		bytevector bytes;
		auto startiter = cbegin(line) + mark + tableprefix.length();
		std::sregex_iterator end;
		std::sregex_iterator iter(startiter, cend(line), rx_datarow);
		for (size_t readc = 0; iter != end && readc < count; ++iter, ++readc) {
			auto field = iter->str();
			int value = translate_value(field, varmap);
			append_to(value, bytelength, bytes);
		}
		return bytes;
	}

	size_t save_datarow(std::ostream& os, std::string line, const stdstrintmap& varmap,
		const asm_sourcemapping& mapping,const bytevector& ROM, StepPos& steppos, size_t count)
	{
		const std::regex rx_datarow{ RX_TABLETERM };

		auto comment = strip_comment(line);

		auto tabletype = get_asmtabletype(line);
		auto tableprefix = get_asmtable_prefix(tabletype);
		auto bytelength = get_asmtable_bytelength(tabletype);
		auto mark = line.find(tableprefix);

		if (tableprefix.empty())
			throw std::runtime_error("an empty tabletype prefix is not allowed on line " + line);
		if (mark == std::string::npos)
			throw std::runtime_error("unable to find tabletype prefix " + tableprefix + " on line " + line);

		auto startiter = cbegin(line) + mark + tableprefix.length();
		std::sregex_iterator iter(startiter, cend(line), rx_datarow);
		std::sregex_iterator end;
		if (iter == end)
			throw std::runtime_error("unable to find any data on datarow line: " + line);

		// So we found at least one match, replace it with the data from ROM
		//write_next_line(os, line.substr(0, mark + tableprefix.length()));
		if (count == (size_t)-1) {
			write_next_line(os, line.substr(0, mark + tableprefix.length()));
		}
		else {
			write_current_line(os, line.substr(0, mark + tableprefix.length()));
		}

		std::sregex_iterator last;
		size_t addcount = 0;
		size_t limit = count == (size_t)-1 ? 1 : count;
		size_t inc = count == (size_t)-1 ? 0 : 1;
		for (size_t i = 0; iter != end && i < limit; ++iter, i += inc) {
			auto oldvalue = iter->str();
			auto newvalue = update_value(oldvalue, ROM, steppos.bytepos, bytelength, mapping, steppos.fieldindex, varmap);

			auto pref = iter->prefix().str();
			auto suff = iter->suffix().str();
			os << pref;
			os << newvalue;

			steppos.Step(bytelength);
			addcount += bytelength;
			last = iter;
		}

		if (last != end) os << last->suffix();
		os << comment;
		return addcount;
	}

	size_t do_table_read(std::istream & is, const asm_sourcemapping & mapping, const stdstrintmap & varmap, bytevector & ROM)
	{
		// read until hitting another label or EOF
		std::string line;
		bytevector addbytes;
		while (is && !is.eof()) {
			auto pos = is.tellg();
			std::getline(is, line);

			if (is_ignore_line(line))
				continue;

			if (has_label(line) || has_done_marker(line)) {
				clear_eof(is);
				is.seekg(pos);
				break;
			}
			else if (is_asmdatarow(line)) {
				auto bytes = read_datarow(line, varmap);
				if (bytes.empty())
					return TraceFailCode(-1, "no data on datarow " + line);

				append(addbytes, bytes);
			}
		}

		for (auto offset : mapping.offsets) {
			overwrite(ROM, addbytes, offset, addbytes.size());
		}

		return mapping.offsets.size() * addbytes.size();
	}

	size_t do_labelval_read(std::istream& is, const asm_sourcemapping& mapping, const stdstrintmap& varmap, bytevector& ROM)
	{
		// read only the first value after the label
		std::string line;
		bytevector addbytes;
		while (is && !is.eof()) {
			auto pos = is.tellg();
			std::getline(is, line);

			if (is_ignore_line(line))
				continue;

			if (has_label(line) || has_done_marker(line)) {
				clear_eof(is);
				is.seekg(pos);
				break;
			}
			else if (is_asmdatarow(line)) {
				//TODO - only read the first N items from the row of data
				//DEVNOTE - for now, N always == 1
				//	In the future, add a count to the mapping if possible.
				auto bytes = read_datarow(line, varmap, 1);
				if (bytes.empty())
					return TraceFailCode(-1, "no data on datarow " + line);

				append(addbytes, bytes);
				break;
			}
		}

		for (auto offset : mapping.offsets) {
			overwrite(ROM, addbytes, offset, addbytes.size());
		}

		return mapping.offsets.size() * addbytes.size();
	}

	size_t read_opval(std::istream & is, const asm_sourcemapping & mapping, const stdstrintmap & varmap, bytevector & ROM)
	{
		std::string line;
		std::getline(is, line);

		std::string comment, marker;
		break_line(line, comment, marker);

		std::smatch match;
		if (!std::regex_match(line, match, rx_opval_hardcode))
			return TraceFailCode(-1, "no opval matches found");
		if (match.size() != 2)
			return TraceFailCode(-1, "did not find the expected 2 opval matches");

		std::string opparam = match[1].str();
		int value = translate_value(opparam, varmap);

		write_to(value, 1, ROM, mapping.offsets);
		return mapping.offsets.size();
	}

	size_t read_cond(std::istream & is, const asm_sourcemapping & mapping, const stdstrintmap & varmap, bytevector & ROM)
	{
		UNREFERENCED_PARAMETER(varmap);

		// regex might not be the best way to nreak this one apart, because the source text can be almost anything.
		// instead, search for the true and false parts and see if we get a match.

		//if sourcetetx matches truepart, set byte to 1
		//if sourcetext matches falsepart, set byte to 0
		//else (sourcetext matches neither), set byte to 0xFF
		std::string line, comment, marker;
		get_and_break_line(is, line, comment, marker);

		const std::regex rx_cond{ "\\|\\:cond\\s+(" RX_VAR ")\\s+\\?\\s*\\\"(.*)\\\"\\s*\\:\\s*\\\"(.*)\\\"" };
		std::smatch match;
		if (!std::regex_match(marker, match, rx_cond))
			return TraceFailCode(-1, "no cond matches were found");
		if (match.size() != 4)
			return TraceFailCode(-1, "did not find the expected 3 cond match terms");
		if (match[1].str() != mapping.name)
			return TraceFailCode(-1, "cond mapping " + mapping.name + " not found");

		auto truepart = match[2].str();
		auto falsepart = match[3].str();
		unsigned char byteval = COND_IGNORE_VALUE;

		if (is_cond_match(line, truepart)) byteval = 1;
		if (is_cond_match(line, falsepart)) byteval = 0;

		write_to(byteval, 1, ROM, mapping.offsets);
		return byteval != COND_IGNORE_VALUE ? mapping.offsets.size() : 0;
	}

	size_t read_label(std::istream & is, const asm_sourcemapping & mapping, const stdstrintmap & varmap, bytevector & ROM)
	{
		if (mapping.type != asm_sourcetype::label && mapping.type != asm_sourcetype::labelval)
			return TraceFailCode(-1, "read_label can't process mapping type " + mapping.name);

		auto testline = preview_next_line(is);

		// read the line with the label, but back up to the point after the label
		auto labeltext = make_mappingname(mapping.type, mapping.name);
		auto mark = testline.find(labeltext);
		if (mark == std::string::npos)
			return TraceFailCode(-1, "can't find label '" + labeltext + "' on line " + testline);

		is.seekg((std::streamoff)labeltext.length(), std::ios::cur);

		//TODO - if labelval, only read one value, e.g. lut_SomeLabel: .BYTE #4F stops after reading 4F
		//	by contrast, label continues reading until it hits another label or EOF.
		if (mapping.type == asm_sourcetype::label)
			return do_table_read(is, mapping, varmap, ROM);

		return do_labelval_read(is, mapping, varmap, ROM);
	}

	size_t read_table(std::istream & is, const asm_sourcemapping & mapping, const stdstrintmap & varmap, bytevector & ROM)
	{
		if (mapping.type != asm_sourcetype::table)
			return TraceFailCode(-1, "read_table can't process mapping type " + mapping.name);

		auto testline = preview_next_line(is);

		// this line is on a comment, so skip to the next line and go if it's a commented table
		auto labeltext = make_mappingname(mapping.type, mapping.name);
		auto mark = testline.find(labeltext);
		if (mark == std::string::npos) return
			TraceFailCode(-1, "can't find table marker '" + labeltext + "' on line " + testline);

		return do_table_read(is, mapping, varmap, ROM);
	}

	size_t do_table_save(std::istream & is, std::ostream & os, const asm_sourcemapping & mapping, const stdstrintmap & varmap, const bytevector & ROM)
	{
		if (mapping.offsets.empty())
			THROWEXCEPTION(std::invalid_argument, "do_table_save expects the mapping '" + mapping.name + " 'to have at least one offset");

		std::string line;
		bytevector addbytes;
		stdstringvector intactlines;
		size_t processcount = 0;
		StepPos steppos(0, mapping.offsets.front());

		// note that the label might have been on ethe same line as the first datarow, so test for that up front.
		// if it's not a datarow, write it as is with no newline at the end.
		auto testline = preview_next_line(is);
		if (!is_asmdatarow(testline)) {
			std::getline(is, line);
			os << line;
		}

		while (is && !is.eof()) {
			auto pos = is.tellg();
			std::getline(is, line);

			if (is_ignore_line(line)) {
				write_next_line(os, line);
				continue;
			}

			if (is_asmdatarow(line)) {
				for (auto intactline : intactlines) {
					write_next_line(os, intactline);
				}
				intactlines.clear();

				size_t addcount = save_datarow(os, line, varmap, mapping, ROM, steppos);
				if (addcount == -1)
					throw std::runtime_error("unabled to save datarow on line: " + line);
				processcount += addcount;
			}
			else if (has_label(line) || has_done_marker(line)) {
				clear_eof(is);
				is.seekg(pos);
				break;
			}
			else {
				intactlines.push_back(line);
			}
		}
		for (auto intactline : intactlines) {
			write_next_line(os, intactline);
		}

		return processcount;
	}

	size_t do_labelval_save(std::istream& is, std::ostream& os, const asm_sourcemapping& mapping, const stdstrintmap& varmap, const bytevector& ROM)
	{
		if (mapping.offsets.empty())
			THROWEXCEPTION(std::invalid_argument, "do_table_save expects the mapping '" + mapping.name + " 'to have at least one offset");

		std::string line;
		bytevector addbytes;
		stdstringvector intactlines;
		size_t processcount = 0;
		StepPos steppos(0, mapping.offsets.front());

		// note that the label might have been on the same line as the first datarow, so test for that up front.
		// if it's not a datarow, write it as is with no newline at the end.
		auto testline = preview_next_line(is);
		if (!is_asmdatarow(testline)) {
			std::getline(is, line);
			os << line;
		}

		while (is && !is.eof()) {
			auto pos = is.tellg();
			std::getline(is, line);

			if (is_ignore_line(line)) {
				write_next_line(os, line);
				continue;
			}

			if (is_asmdatarow(line)) {
				for (auto intactline : intactlines) {
					write_next_line(os, intactline);
				}
				intactlines.clear();

				//TODO - see read_datarow
				//	For now, N always == 1
				size_t addcount = save_datarow(os, line, varmap, mapping, ROM, steppos, 1);
				if (addcount == -1)
					throw std::runtime_error("unabled to save datarow on line: " + line);
				processcount += addcount;
				break;
			}
			else if (has_label(line) || has_done_marker(line)) {
				clear_eof(is);
				is.seekg(pos);
				break;
			}
			else {
				intactlines.push_back(line);
			}
		}
		for (auto intactline : intactlines) {
			write_next_line(os, intactline);
		}

		return processcount;
	}

	size_t save_opval(std::istream & is, std::ostream & os, const asm_sourcemapping & mapping, const stdstrintmap & varmap, const bytevector & ROM)
	{
		std::string line, comment, marker;
		get_and_break_line(is, line, comment, marker);

		std::smatch match;
		if (!std::regex_match(line, match, rx_opval_hardcode))
			return TraceFailCode(-1, "no opval matches found");
		if (match.size() != 2)
			return TraceFailCode(-1, "did not find the expected 2 opval matches");

		auto replacement = UpdateOpValue(match, line, ROM[mapping.offsets[0]], mapping, varmap);
		if (replacement.empty()) return (size_t)-1;
		write_next_line(os, replacement) << comment << marker;
		return 1;
	}

	size_t save_cond(std::istream & is, std::ostream & os, const asm_sourcemapping & mapping, const stdstrintmap & varmap, const bytevector & ROM)
	{
		UNREFERENCED_PARAMETER(varmap);

		// regex might not be the best way to nreak this one apart, because the source text can be almost anything.
		// instead, search for the true and false parts and see if we get a match.

		unsigned char byteval = ROM[mapping.offsets.front()];
		if (byteval == COND_IGNORE_VALUE) {
			std::string line;
			std::getline(is, line);
			//os << line;
			write_next_line(os, line);
			return 0;
		}

		// Otherwise, it's one of the two alternatives
		std::string line, comment, marker;
		get_and_break_line(is, line, comment, marker);

		const std::regex rx_cond{ "\\|\\:cond\\s(" RX_VAR ")\\s+\\?\\s*\\\"(.*)\\\"\\s*\\:\\s*\\\"(.*)\\\"" };
		std::smatch match;
		if (!std::regex_match(marker, match, rx_cond))
			return TraceFailCode(-1, "no cond matches were found");
		if (match.size() != 4)
			return TraceFailCode(-1, "did not find the expected 3 cond match terms");
		if (match[1].str() != mapping.name)
			return TraceFailCode(-1, "cond mapping " + mapping.name + " not found");

		auto truepart = match[2].str();
		auto falsepart = match[3].str();
		auto replacement = GetConditionalReplacement(line, byteval, truepart, falsepart);
		write_next_line(os, replacement) << comment << marker;
		return 1;
	}

	size_t save_label(std::istream & is, std::ostream & os, const asm_sourcemapping & mapping, const stdstrintmap & varmap, const bytevector & ROM)
	{
		if (mapping.type != asm_sourcetype::label && mapping.type != asm_sourcetype::labelval)
			return TraceFailCode(-1, "save_label can't process mapping type " + mapping.name);

		auto testline = preview_next_line(is);

		// read the line with the label, but back up to the point after the label
		auto labeltext = make_mappingname(mapping.type, mapping.name);
		auto mark = testline.find(labeltext);
		if (mark == std::string::npos)
			return TraceFailCode(-1, "save_label can't find label '" + labeltext + "' on line " + testline);

		write_next_line(os, "");
		for (size_t st = 0; st < mark + labeltext.length(); ++st) {
			os << (char)is.get();
		}

		if (mapping.type == asm_sourcetype::label)
			return do_table_save(is, os, mapping, varmap, ROM);

		return do_labelval_save(is, os, mapping, varmap, ROM);
	}

	size_t save_table(std::istream & is, std::ostream & os, const asm_sourcemapping & mapping, const stdstrintmap & varmap, const bytevector & ROM)
	{
		if (mapping.type != asm_sourcetype::table)
			return TraceFailCode(-1, "save_table can't process mapping type " + mapping.name);

		auto pos = is.tellg();
		std::string line;
		std::getline(is, line);

		// this line is on a comment, so skip to the next line and go if it's a commented table
		auto labeltext = make_mappingname(mapping.type, mapping.name);
		auto mark = line.find(labeltext);
		if (mark == std::string::npos) return
			TraceFailCode(-1, "save_table can't find table marker '" + labeltext + "' on line " + line);

		// It's in a comment, so write the entire line
		write_next_line(os, line);

		return do_table_save(is, os, mapping, varmap, ROM);
	}
}

// ################################################################
// PUBLIC IMPLEMENTATION

namespace disch_functions
{
	namespace data {
		int read_assembly_data(const char * asmfile, const char* varmaptext, const char * inputtext, unsigned char * rom, size_t romlength)
		{
			// do the assembly source processing here, but parse it to an intermediate form that the receiving
			// app can handle. Sending back asm text.
			std::ifstream ifs;
			ifs.open(asmfile);

			stdstrintmap varmap;
			asmsourcemappingvector sourcemappings;
			bytevector ROM(romlength);
			{
				std::istringstream ivar(varmaptext);
				std::istringstream isrc(inputtext);
				ivar >> varmap;
				isrc >> sourcemappings;
			}

			memcpy(address(ROM), rom, romlength);

			//SendMessage(appwindow, RWM_FFHACKSTEREX_SHOWWAITMESSAGE, NULL, (LPARAM)"Reading source into ROM buffer...");

			// Instead of relying on code in ff1-coredefs to do the assembly translation, define the translation
			// functions here and pass them to coredefs. Since coredefs is a static lib, its definitions are compiled
			// into this module. They aren't subject to the C++ ABI problem. and can be passed directly.
			auto readhandler = [varmap, &ROM](std::istream & is, uintvector & mappinghits, const asmsourcemappingvector & mappings)
			{
				auto * mapping = test_stream_for_mapping(is, mappings);
				if (mapping == nullptr) {
					discard_line(is);
					return;
				}

				auto * base = address(mappings);
				size_t mapindex = mapping - base;
				switch (mapping->type)
				{
				case asm_sourcetype::opval: read_opval(is, *mapping, varmap, ROM); break;
				case asm_sourcetype::cond: read_cond(is, *mapping, varmap, ROM); break;
				case asm_sourcetype::table: read_table(is, *mapping, varmap, ROM); break;
				case asm_sourcetype::label: read_label(is, *mapping, varmap, ROM); break;
				case asm_sourcetype::labelval: read_label(is, *mapping, varmap, ROM); break;
				default:
					THROWEXCEPTION(std::runtime_error, "mapping " + mapping->name + " has unsupported type " + std::to_string((size_t)mapping->type));
				}

				++mappinghits[mapindex];
			};
			std_assembly::process_assembly_source(ifs, sourcemappings, readhandler);

			//N.B. - std::vector doesn't support attaching to existing memory without a custom allocator, so copy
			//       the updated ROM vector's data back to the rom buffer.
			memcpy(rom, address(ROM), romlength);

			return ASMFF1_SUCCESS;
		}

		int write_assembly_data(const char * asmfile, const char* varmaptext, const char * inputtext, const unsigned char * rom, size_t romlength)
		{
			auto temppath = asmfile + std::string(".new");
			FilePathRemover remover(temppath);

			// Using this scope to ensure that the ofstream object is closed and flushed
			{
				std::ifstream ifs;
				if (!Io::open_file(asmfile, ifs))
					THROWEXCEPTION(std::runtime_error, "unable to open the input assembly file " + std::string(asmfile));

				std::ofstream ofs;
				create_asmstream(ofs, temppath);

				stdstrintmap varmap;
				asmsourcemappingvector sourcemappings;
				bytevector ROM(romlength);
				{
					std::istringstream ivar(varmaptext);
					std::istringstream isrc(inputtext);
					ivar >> varmap;
					isrc >> sourcemappings;
				}

				memcpy(address(ROM), rom, romlength);

				//SendMessage(appwindow, RWM_FFHACKSTEREX_SHOWWAITMESSAGE, NULL, (LPARAM)"Writing source from ROM buffer...");

				// Instead of relying on code in ff1-coredefs to do the assembly translation, define the translation
				// functions here and pass them to coredefs. Since coredefs is a static lib, its definitions are compiled
				// into this module. They aren't subject to the C++ ABI problem. and can be passed directly.
				auto writehandler = [&ofs, varmap, ROM](std::istream & is, uintvector & mappinghits, const asmsourcemappingvector & mappings)
				{
					auto * mapping = test_stream_for_mapping(is, mappings);
					if (mapping == nullptr) {
						copy_next_line(is, ofs);
						return;
					}

					auto * base = address(mappings);
					size_t mapindex = mapping - base;
					switch (mapping->type)
					{
					case asm_sourcetype::opval: save_opval(is, ofs, *mapping, varmap, ROM); break;
					case asm_sourcetype::cond: save_cond(is, ofs, *mapping, varmap, ROM); break;
					case asm_sourcetype::table: save_table(is, ofs, *mapping, varmap, ROM); break;
					case asm_sourcetype::label: save_label(is, ofs, *mapping, varmap, ROM); break;
					case asm_sourcetype::labelval: save_label(is, ofs, *mapping, varmap, ROM); break;
					default:
						THROWEXCEPTION(std::runtime_error, "mapping " + mapping->name + " has unsupported type " + std::to_string((size_t)mapping->type));
					}

					++mappinghits[mapindex];
				};

				std_assembly::process_assembly_source(ifs, sourcemappings, writehandler);
			}

			if (!Paths::FileMove(temppath.c_str(), asmfile))
				throw std::ios_base::failure("Can't save inline assembly changes: unable to overwrite the project assembly with the working temp file.");

			return ASMFF1_SUCCESS;
		}
	}
}