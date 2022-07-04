#pragma once

#include "map_types.h"
#include "vector_types.h"
#include "stream_ops.h"
#include <functional>

enum class asm_tabletype { none, bytes, words, faraddrs };
enum class asm_sourcetype { none,
	opval,		// opcode value, e.g. LDA #3A
	cond,		// conditional
	table,		// one or more rows of values following a label
	label,		// similar to a table, but usually handles just one row
	dlg,		// dialogue
	labelval	// a single value following a label
};

const asm_sourcetype asmnone = asm_sourcetype::none;
const asm_sourcetype asmopval = asm_sourcetype::opval;
const asm_sourcetype asmcond = asm_sourcetype::cond;
const asm_sourcetype asmtable = asm_sourcetype::table;
const asm_sourcetype asmlabel = asm_sourcetype::label;
const asm_sourcetype asmdlg = asm_sourcetype::dlg;
const asm_sourcetype asmlabelval = asm_sourcetype::labelval;


//DEVNOTE - offsets is a vector because i originally supported multiple offsets per entry.
//			It became a bit complex to manage, so I never implemented it.
//			you'll see that code only uses the first index of te offsets vector member.
struct asm_sourcemapping
{
	asm_sourcetype type;
	std::string name;
	uintvector offsets;					// one or more ROM offsets.
										//REFACTOR - should this be changed to just a single offset?
	                                        // the ROM data is written to each offset.
	                                        // if multiple offsets are specified, only data from the
	                                        // last-read offtset is retained.
	stdstringvector varnamesforvalues;	// one-to-one vector of names written in place of each ROM value (whether byte/word/etc.)
	                                        // each name will be written to the source file instead of the value,
										    // e,g,
	                                        // if the mapping handles 4 BYTE values { 0, 1, 6, 7 },
										    // and varnamesforvalues is set to { CLS_FT, CLS_TH, CLS_KN, CLS_NJ },
										    // then in the assembly source, instead of writing
	                                        //    .BYTE 0, 1, 6, 7
	                                        // the names will be written
	                                        //    .BYTE CLS_FT, CLS_TH, CLS_KN, CLS_NJ
	                                        //
	                                        // note that varnamesforvalues is the same length as the values,
	                                        // NOT the bytes, e.g. if the mapping handles 2 WORD values, we'd
	                                        // expect varnamesforvalues to be a 2-element list vector,
	                                        // even though the ROM data would be 4 bytes long.
};
using asmsourcemappingvector = std::vector<asm_sourcemapping>;


bool operator==(const asm_sourcemapping & left, const asm_sourcemapping & right);
bool operator!=(const asm_sourcemapping & left, const asm_sourcemapping & right);

std::string asmsrcmapvec(const asmsourcemappingvector & v);
asmsourcemappingvector asmsrcmapvec(std::string text);

// Overloaded operators for streaming assembly-related types

std::ostream & operator << (std::ostream & os, const asm_sourcemapping & srcmap);
std::istream & operator >> (std::istream & is, asm_sourcemapping & srcmap);

std::ostream & operator << (std::ostream & os, const asmsourcemappingvector & vec);
std::istream & operator >> (std::istream & is, asmsourcemappingvector & vec);



struct talkelement
{
	std::string marker;    // the marker (e.g. battle, hcweapon, etc.)
	std::string element;   // blank (means it's a non-element line copied as is), item, hcitem, text, etc.
	std::string value;     // could be an int, constant, or variable (or for fanfare, 0 or 1)
	int bits;              // for now, 8 or 16 //FUTURE - should this be an enum instead to eliminate the possibility of errors?
						   // enum class elembits { e8bits, e16bits };
						   // int get_elembit_count(elembits ebits) throw(std::invalid_argument);

	std::string comment;
};
using talkelementvector = std::vector<talkelement>;
using talknamevectormap = std::map<std::string, talkelementvector>;

using process_asmsrc_handler = std::function<void(std::istream & is, uintvector & mappinghits, const asmsourcemappingvector & mappings)>;


std::ostream & operator << (std::ostream & os, const talkelement & elem);
std::istream & operator >> (std::istream & is, talkelement & elem);

std::ostream & operator << (std::ostream & os, const talkelementvector & vec);
std::istream & operator >> (std::istream & is, talkelementvector & vec);

std::ostream & operator << (std::ostream & os, const talknamevectormap & cmap);
std::istream & operator >> (std::istream & is, talknamevectormap & cmap);



using variablemap = std::map<std::string, int>;
using asm_process_handler = std::function<void(std::istream & is, const asmsourcemappingvector & mappings)>;

