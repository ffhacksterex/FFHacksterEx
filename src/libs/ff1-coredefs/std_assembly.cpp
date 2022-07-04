#include "stdafx.h"
#include "std_assembly.h"
#include "io_functions.h"
#include "string_functions.h"
#include "type_support.h"
#include "vector_types.h"
#include "map_types.h"

using namespace Io;
using namespace Strings;
using namespace Types;



StepPos::StepPos(size_t afieldindex, size_t abytepos)
	: fieldindex(afieldindex)
	, bytepos(abytepos)
{
}

void StepPos::Step(int bytewidth)
{
	++fieldindex;
	bytepos += bytewidth;
}


namespace std_assembly
{
	namespace shared
	{
		const asm_sourcemapping * test_stream_for_mapping(std::istream & is, const asmsourcemappingvector & mappings)
		{
			auto line = preview_next_line(is);
			return find_first_mapping(mappings, line);
		}

		const asm_sourcemapping * find_first_mapping(const asmsourcemappingvector & mappings, std::string line)
		{
			if (mappings.empty())
				throw std::domain_error("mappings cannot be empty when calling find_first_mapping");

			auto iter = std::find_if(cbegin(mappings), cend(mappings), [&](const auto & mapping) { return found_mapping(mapping, line); });
			return iter != cend(mappings) ? &*iter : nullptr;
		}

		// Strips format markers '|:' from assembly source lines.
		std::string strip_marker(std::string & line)
		{
			auto mark = line.find("|:");
			if (mark == std::string::npos) return std::string();

			std::string marker = line.substr(mark);
			line.erase(mark, std::string::npos);
			return marker;
		}

		// Strips all comment text from an assembly source line.
		//N.B. - this also strips markers, which are embedded in comments.
		//       Either call strip_marker first or use break_line to get both the comment and marker.
		std::string strip_comment(std::string & line)
		{
			auto mark = line.find(';');
			if (mark == std::string::npos) return std::string();

			std::string comment = line.substr(mark);
			line.erase(mark, std::string::npos);
			return comment;
		}

		std::string & break_line(std::string & line, std::string & comment, std::string & marker)
		{
			// always strip the marker first since it's embedded within the comment
			marker = strip_marker(line);
			comment = strip_comment(line);
			return line;
		}

		std::string & get_and_break_line(std::istream & is, std::string & line, std::string & comment, std::string & marker)
		{
			std::getline(is, line);
			return break_line(line, comment, marker);
		}

		bool is_ignore_line(std::string line)
		{
			return line.find("|:ignore") != std::string::npos;
		}

		bool is_cond_match(std::string line, std::string part)
		{
			// find it, and if found, one of the following must be true:
			// matching part is at the end of line;
			// matching part is followed by whitespace;
			// matching part is followed by a semicolon.
			auto mark = line.find(part);
			if (mark != std::string::npos) {
				// Implication: mark + part.length() can't exceed line.;ength()
				auto endmark = mark + part.length();
				if (endmark == line.length()) return true;
				auto ch = line[endmark + 1];
				if (isspace(ch) || ch == ';') return true;
			}
			return false;
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

		std::string GetConditionalReplacement(std::string line, int value, std::string truepart, std::string falsepart)
		{
			ASSERT(value == ASM_TRUE || value == ASM_FALSE);
			if (value != ASM_TRUE && value != ASM_FALSE) return{};

			// See which part is in the line, mark that position, and use that as the length padding value.
			auto mark = line.find(truepart);
			size_t length = 0;
			if (mark != std::string::npos) {
				length = truepart.length();
			}
			else {
				mark = line.find(falsepart);
				length = falsepart.length();
			}
			if (mark == std::string::npos || length == 0)
				THROWEXCEPTION(std::runtime_error, " can't find original text to replace");

			// Now build the inline replacement text.
			auto pre = line.substr(0, mark);
			auto post = line.substr(mark + length);
			std::string replacement = pre + right_pad(value == ASM_TRUE ? truepart : falsepart, (int)length) + post;
			return replacement;
		};

		bool is_sourceline(std::string line)
		{
			return !isemptyorwhitespace(line) && !is_asmcomment(line);
		}

		// a comment has its first non-whitespace character as a semicolon.
		// for these purposes, a blank line is NOT counted as a comment.
		bool is_asmcomment(std::string line)
		{
			size_t index = 0;
			while (index < line.size() && isspace(line[index])) ++index;
			return index < line.size() && line[index] == ';';
		}

		std::string get_asmsource_prefix(asm_sourcetype type)
		{
			switch (type)
			{
			case asm_sourcetype::none: THROWEXCEPTION(std::domain_error, "asm_sourcetype::none is not a valid source type");
			case asm_sourcetype::label:  THROWEXCEPTION(std::domain_error, "asm_sourcetype::label does not have a source type prefix");
			case asm_sourcetype::opval: return std::string("|:opval");
			case asm_sourcetype::cond: return std::string("|:cond");
			case asm_sourcetype::table: return std::string("|:table");
			case asm_sourcetype::dlg: return std::string("|:dlg");
			case asm_sourcetype::labelval: THROWEXCEPTION(std::domain_error, "asm_sourcetype::labelval does not have a source type prefix");
			}

			THROWEXCEPTION(std::invalid_argument, "No prefix defined for unrecognizd asm+sourcetype value " + std::to_string(static_cast<int>(type)));
		}

		bool has_done_marker(std::string line)
		{
			return line.find("|:endtable") != std::string::npos;
		}


		void write_to(int value, int bytelength, bytevector & ROM, const uintvector & offsets)
		{
			for (auto offset : offsets) {
				ROM[offset] = (value) & 0xFF;
				if (bytelength > 1) ROM[offset + 1] = (value >> 8) & 0xFF;
				if (bytelength > 2) ROM[offset + 2] = (value >> 16) & 0xFF;
				if (bytelength > 3) ROM[offset + 3] = (value >> 24) & 0xFF;
			}
		}

		void append_to(int value, int bytelength, bytevector & bytes)
		{
			bytes.push_back((value) & 0xFF);
			if (bytelength > 1) bytes.push_back((value >> 8) & 0xFF);
			if (bytelength > 2) bytes.push_back((value >> 16) & 0xFF);
			if (bytelength > 3) bytes.push_back((value >> 24) & 0xFF);
		}

		int get_int(const bytevector & ROM, size_t offset, int bytelength)
		{
			int result = ROM[offset];
			if (bytelength > 1) result |= (ROM[offset + 1]) << 8;
			if (bytelength > 2) result |= (ROM[offset + 2]) << 16;
			if (bytelength > 3) result |= (ROM[offset + 3]) << 24;
			return result;
		}

		bool is_asmvar(std::string field)
		{
			// instead of regex, just look for the possible negative sign and leading alpha character.
			// In context, this check doesn't need to be as stringent as the others.
			if (field.empty()) return false;

			auto * sz = field.data();
			if (*sz == '-') ++sz;
			return isalpha(*sz) == TRUE;
		}

		// Returns the named constant's value if on the supplied varmap.
		// Supports a leading negative sign as well.
		//N.B. - to check the format before attempting to convert, call is_asmvar first.
		int get_asmvar(const stdstrintmap & varmap, std::string varname)
		{
			// instead of regex, just look for the possible negative sign and leading alpha character.
			// In context, this check doesn't need to be as stringent as the others.
			if (!varmap.empty()) {
				auto * sz = varname.data();
				bool neg = *sz == '-';
				if (neg) ++sz;
				if (isalpha(*sz)) {
					int sign = neg ? -1 : 1;
					auto iter = varmap.find(varname);
					if (iter != cend(varmap))
						return iter->second * sign;
				}
			}

			THROWEXCEPTION(std::invalid_argument, "'" + varname + "' not found in the assembly varmap.");
		}

		// 6502 Assembly only recognizes hex in address format, e.g. "$12AB" is supported, but "0x12AB" (C-notation) is not.
		std::string asmhex(int value, size_t minwidth)
		{
			CString strnum, sign;
			bool negate = value < 0;
			if (negate) {
				value = -value;
				sign = "-";
			}

			if (minwidth > 16) minwidth = 16;
			strnum.Format("%s$%0*X", (LPCSTR)sign, minwidth, value);
			return std::string((LPCSTR)strnum);
		}

		// Attempts to make the new value match the same case as the original hex value.
		// Handles $ and 0x prefixed hex values, as well as negative signed values.
		std::string match_hex_case(std::string origvalue, std::string newvalue)
		{
			if (!origvalue.empty()) {
				size_t idx = 0;
				bool proceed = false;
				if (origvalue[idx] == '-') ++idx;

				if (origvalue[idx] == '$') {
					++idx;
					proceed = true;
				}
				else if (origvalue.find("0x") == idx || origvalue.find("0X") == idx) {
					idx += 2;
					proceed = true;
				}
				if (proceed)
				{
					size_t eligibles = 0;
					size_t capscount = 0;
					size_t lowscount = 0;
					for (size_t st = idx; st < origvalue.length(); ++st) {
						auto ch = origvalue[st];
						if (isupper(ch)) ++capscount;
						else if (islower(ch)) ++lowscount;
						if (isxdigit(ch) && !isdigit(ch)) ++eligibles;
					}
					if (capscount == eligibles)
						return to_upper(newvalue);
					if (lowscount == eligibles)
						return to_lower(newvalue);
				}
			}
			return newvalue;
		}

		// Attempts to find the field in the varmap (if it's a variable name) or translates it
		// as a literal value (hex, dec, addr, or bin).
		int translate_value(std::string field, const stdstrintmap & varmap)
		{
			if (field.empty()) throw std::invalid_argument("translate_value requires a non-empty parameter");

			if (is_asmvar(field))
				return get_asmvar(varmap, field);

			CString csfield = field.c_str();
			if (is_addr(csfield))
				return addr(csfield);
			if (is_bin(csfield))
				return bin(csfield);
			if (is_hex(csfield))
				return hex(csfield);
			if (is_dec(csfield))
				return dec(csfield);

			throw std::invalid_argument("translate_value can't convert '" + field + "' into a usable ROM int value");
		}

		std::string update_value(std::string oldvalue, const bytevector & ROM, size_t bytepos, int bytelength,
			const asm_sourcemapping & mapping, size_t mappingindex, const stdstrintmap & varmap)
		{
			std::string newvalue;
			auto newdata = get_int(ROM, bytepos, bytelength);
			if (mappingindex < mapping.varnamesforvalues.size()) {
				// if the values match, use the name instead of the value
				auto itmap = varmap.find(mapping.varnamesforvalues[mappingindex]);
				if (itmap != cend(varmap) && itmap->second == newdata)
					newvalue = itmap->first;
			}
			if (newvalue.empty()) {
				auto base = get_num_base(oldvalue);
				newvalue = int_to_string(newdata, base, bytelength);
				newvalue = match_hex_case(oldvalue, newvalue);
			}
			return newvalue;
		}

		std::string int_to_string(int value, int base, int bytelength)
		{
			CString fmt;
			switch (base) {
			case 2: return (LPCSTR)bin(value);
			case 10: return (LPCSTR)dec(value);
			case 16: return asmhex(value, bytelength != -1 ? bytelength * 2 : 2);
			}

			THROWEXCEPTION(std::invalid_argument, "can't convert value " + std::to_string(value) + " to base " + std::to_string(base));
		}

		int get_num_base(std::string text)
		{
			const char ch = !text.empty() ? text[0] : '0'; // empty strings default to decimal
			if (ch == '$') return 16;
			if (ch == '%') return 2;
			if (isdigit(ch)) return 10;

			throw std::invalid_argument("can't get numeric base from string " + text);
		}

		int get_num_base_or_default(std::string text, int defbase)
		{
			const char ch = !text.empty() ? text[0] : '0'; // empty strings default to decimal
			if (ch == '$') return 16;
			if (ch == '%') return 2;
			if (isdigit(ch)) return 10;

			return defbase;
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

	} // end namespace shared


	void process_assembly_source(std::istream & is, const asmsourcemappingvector & mappings, process_asmsrc_handler handler)
	{
		uintvector hits(mappings.size(), 0);

		while (is && !is.eof()) {
			auto pos = is.tellg();
			handler(is, hits, mappings);

			// If the file pointer doesn't move, an infinite loop will likely occur.
			// If it moves backward, duplicate reads might occur. Fail in both cases.
			if (!is.eof() && (is.tellg() <= pos))
				throw std::runtime_error("process_assembly_source handler must move the file pointer");
		}

		if (std::any_of(cbegin(hits), cend(hits), [](auto numhits) { return numhits == 0;}))
			throw asminline_unusedmappingexception(EXCEPTIONPOINT, exceptop::reading, mappings, hits);
	}

}