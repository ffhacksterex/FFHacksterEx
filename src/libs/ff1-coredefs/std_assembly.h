#pragma once

#include <functional>
#include <iostream>
#include "assembly_types.h"


class StepPos
{
public:
	StepPos(size_t afieldindex = 0, size_t abytepos = 0);
	void Step(int bytewidth = 1);

	size_t fieldindex;
	size_t bytepos;
};

enum class getlabelstyle { includecolon, stripcolon };

namespace std_assembly
{
	namespace shared
	{

		const asm_sourcemapping * test_stream_for_mapping(std::istream & is, const asmsourcemappingvector & mappings);

		const asm_sourcemapping * find_first_mapping(const asmsourcemappingvector & mappings, std::string line);

		std::string strip_marker(std::string & line);
		std::string strip_comment(std::string & line);
		std::string & break_line(std::string & line, std::string & comment, std::string & marker);
		std::string & get_and_break_line(std::istream & is, std::string & line, std::string & comment, std::string & marker);

		bool is_ignore_line(std::string line);
		bool is_cond_match(std::string line, std::string part);

		std::string ensure_label_nocolon(std::string origlabel);
		std::string ensure_label_colon(std::string origlabel);

		std::string GetConditionalReplacement(std::string line, int value, std::string truepart, std::string falsepart);

		bool is_sourceline(std::string line);
		bool is_asmcomment(std::string line);
		std::string get_asmsource_prefix(asm_sourcetype type);
		bool has_done_marker(std::string line);

		void write_to(int value, int bytelength, bytevector & ROM, const uintvector & offsets);
		void append_to(int value, int bytelength, bytevector & ROM);
		int get_int(const bytevector & ROM, size_t offset, int bytelength);

		bool is_asmvar(std::string field);
		int get_asmvar(const stdstrintmap & varmap, std::string varname); // could have a leading negative sign
		std::string asmhex(int value, size_t minwidth);
		std::string match_hex_case(std::string origvalue, std::string newvalue);

		int translate_value(std::string field, const stdstrintmap & varmap);
		std::string update_value(std::string oldvalue, const bytevector & ROM, size_t bytepos, int bytelength,
			const asm_sourcemapping & mapping, size_t mappingindex, const stdstrintmap & varmap);
		std::string int_to_string(int value, int base, int bytelength = -1);

		int get_num_base(std::string text);
		int get_num_base_or_default(std::string text, int defbase);

		std::string make_mappingname(asm_sourcetype type, std::string name);
		stdstringvector get_mappingnames(const asmsourcemappingvector & mappings);
		bool found_mapping(const asm_sourcemapping & mapping, std::string line);

	}

	void process_assembly_source(std::istream & is, const asmsourcemappingvector & mappings, process_asmsrc_handler handler);
}



#define RX_OPCODE "[A-Z]+"
#define RX_VARNAME "[A-Za-z_][0-9A-Za-z_]+"

#define RX_DEC "\\d+"
#define RX_BIN "\\%[0-1]+"
#define RX_HEX "\\$[0-9A-Fa-f]+"
#define RX_VALUE RX_HEX "|" RX_BIN "|" RX_DEC

#define RX_VAR RX_VARNAME
#define RX_VAL0RVAR RX_VALUE "|" RX_VAR

#define RX_TABLETERM  RX_VAL0RVAR

// Data condition flag
#define COND_IGNORE_VALUE 255

// Dialogue talk condition flags
#define ASM_FALSE 0
#define ASM_TRUE 1
#define ASM_IGNORE 2


// Defined in core_exceptions.cpp
class asminline_unusedmappingexception : public std::exception
{
public:
	asminline_unusedmappingexception(std::string file, int line, std::string function, exceptop op, const asmsourcemappingvector & mappings, const uintvector & hits);
	virtual ~asminline_unusedmappingexception();

	virtual const char* what() const;
private:
	std::string m_message;
	size_t m_matchcount;
	size_t m_total;
};
