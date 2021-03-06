#include "stdafx.h"
#include "GameSerializer.h"
#include "FFHacksterProject.h"
#include "asmdll_impl.h"
#include "collection_helpers.h"
#include "dialogue_helpers.h"
#include "general_functions.h"
#include "ini_functions.h"
#include "interop_assembly_interface.h"
#include "io_functions.h"
#include "LoadLibraryHandleScope.h"
#include "logging_functions.h"
#include "path_functions.h"
#include "regex_helpers.h"
#include "std_assembly.h"
#include "string_functions.h"
#include "type_support.h"
#include "IProgress.h"
#include <fstream>
#include <regex>
#include <memory>

using namespace asmdll_impl;
using namespace Ini;
using namespace Io;
using namespace regex_helpers;
using namespace std_assembly::shared;
using namespace Strings;


std::string build_asm_path(CFFHacksterProject & proj, CString relfilename)
{
	if (!proj.IsAsm())
		return std::string();

	CString asmfile;
	asmfile.Format("%s\\asm\\%s", (LPCSTR)proj.ProjectFolder, (LPCSTR)relfilename);
	return (LPCSTR)asmfile;
}

std::istream & open_asmstream(std::ifstream & ifs, CFFHacksterProject & proj, CString relfilename)
{
	if (ifs.is_open()) ifs.close();

	if (!proj.IsAsm())
		THROWEXCEPTION(std::runtime_error, "Can't load ASM inline, project is actually of type " + std::string((LPCSTR)proj.ProjectTypeName));

	auto asmfile = build_asm_path(proj, relfilename);
	if (!Paths::FileExists(asmfile.c_str())) {
		THROWEXCEPTION(std::runtime_error, "Can't load ASM inline, can't locate assembly file " + asmfile);
	}

	if (!Io::open_file(asmfile.c_str(), ifs))
		THROWEXCEPTION(std::runtime_error, "Can't load ASM inline, can't open the assembly file " + asmfile + " to load as inline assembly");

	return ifs;
}



GameSerializer::GameSerializer(CFFHacksterProject & proj, CWnd* mainwnd)
	: m_proj(proj)
	, MainWindow(mainwnd)
{
}

GameSerializer::~GameSerializer()
{
}

void GameSerializer::LoadAsmBin(CString relfilename, int offset, int expectedlength, bool allowzerolength)
{
	if (!m_proj.IsAsm())
		throw wrongprojectype_exception(EXCEPTIONPOINT, m_proj, "asm");

	CString asmbin;
	asmbin.Format("%s\\asm\\%s", (LPCSTR)m_proj.ProjectFolder, (LPCSTR)relfilename);

	//QUICK-N-DIRTY METHOD
	// create asmbuffer of length count * bytewidth
	// read relfilename into asmbuffer
	// overwrite cart->ROM with asmbuffer statrting at offset
	bytevector asmbuffer;
	load_binary(asmbin, asmbuffer, allowzerolength);

	auto expcount = (size_t)expectedlength;
	if (expcount != -1 && asmbuffer.size() != expcount) {
		THROWEXCEPTION(std::runtime_error,
			"buffer size mismatch: expected " + std::to_string(expcount) + ", but found " + std::to_string(asmbuffer.size()) + " bytes.");
	}
	if (!allowzerolength && asmbuffer.empty())
		THROWEXCEPTION(std::runtime_error, "Not allowed to read from zero-length source binary file " + std::string((LPCSTR)asmbin));

	expcount = asmbuffer.size();
	overwrite(m_proj.ROM, asmbuffer, offset, expcount);
}

void GameSerializer::LoadAsmBin(CString relfilename, int offset, bool allowzerolength)
{
	LoadAsmBin(relfilename, offset, -1, allowzerolength);
}

void GameSerializer::SaveAsmBin(CString relfilename, int offset, bool allowzerolength)
{
	if (!m_proj.IsAsm())
		throw wrongprojectype_exception(EXCEPTIONPOINT, m_proj, "asm");
	if (!allowzerolength && m_proj.ROM.empty())
		THROWEXCEPTION(std::invalid_argument, "Can't write assembly binary data into a zero-byte ROM buffer at offset " + std::to_string(offset));

	CString asmbin;
	asmbin.Format("%s\\asm\\%s", (LPCSTR)m_proj.ProjectFolder, (LPCSTR)relfilename);
	if (!Paths::FileExists(asmbin))
		THROWEXCEPTION(std::runtime_error, "Can't write assembly binary data file " + std::string((LPCSTR)asmbin));

	// Write the same number of bytes as the size of the file (i.e. the write should't arbitrarily change the file size).
	size_t filesize = Paths::FileSize(asmbin);
	if (!allowzerolength && filesize == 0)
		THROWEXCEPTION(std::runtime_error, "Not allowed to write to zero-length destination binary file " + std::string((LPCSTR)asmbin));

	bytevector asmbuffer(filesize, 0); 
	overwrite(asmbuffer, m_proj.ROM, 0, filesize, offset);

	save_binary(asmbin, asmbuffer);
}

void GameSerializer::LoadAsmBinBuffer(bytevector & ROM, CString relfilename, int offset, bool allowzerolength)
{
	if (!m_proj.IsAsm())
		throw wrongprojectype_exception(EXCEPTIONPOINT, m_proj, "asm");
	if (!allowzerolength && ROM.empty())
		THROWEXCEPTION(std::invalid_argument, "Can't load assembly binary data buffer into a zero-byte ROM buffer at offset " + std::to_string(offset));

	CString asmbin;
	asmbin.Format("%s\\asm\\%s", (LPCSTR)m_proj.ProjectFolder, (LPCSTR)relfilename);

	//QUICK-N-DIRTY METHOD
	// create asmbuffer of length count * bytewidth
	// read relfilename into asmbuffer
	// overwrite cart->ROM with asmbuffer statrting at offset
	bytevector asmbuffer;
	load_binary(asmbin, asmbuffer, allowzerolength);
	if (!allowzerolength && asmbuffer.empty())
		THROWEXCEPTION(std::runtime_error, "Not allowed to read from zero-length source binary file " + std::string((LPCSTR)asmbin));

	auto expcount = asmbuffer.size();
	overwrite(ROM, asmbuffer, offset, expcount);
}

void GameSerializer::SaveAsmBinBuffer(const bytevector & ROM, CString relfilename, int offset, bool allowzerolength)
{
	if (!m_proj.IsAsm())
		throw wrongprojectype_exception(EXCEPTIONPOINT, m_proj, "asm");
	if (!allowzerolength && ROM.empty())
		THROWEXCEPTION(std::invalid_argument, "Can't write assembly binary data buffer into a zero-byte ROM buffer at offset " + std::to_string(offset));

	CString asmbin;
	asmbin.Format("%s\\asm\\%s", (LPCSTR)m_proj.ProjectFolder, (LPCSTR)relfilename);
	if (!Paths::FileExists(asmbin))
		THROWEXCEPTION(std::runtime_error, "Can't write assembly binary data file " + std::string((LPCSTR)asmbin));

	// Write the same number of bytes as the size of the file (i.e. the write should't arbitrarily change the file size).
	size_t filesize = Paths::FileSize(asmbin);
	if (!allowzerolength && filesize == 0)
		THROWEXCEPTION(std::runtime_error, "Not allowed to write to zero-length destination binary file " + std::string((LPCSTR)asmbin));

	bytevector asmbuffer(filesize, 0);
	overwrite(asmbuffer, ROM, 0, filesize, offset);

	save_binary(asmbin, asmbuffer);
}

void GameSerializer::LoadInline(CString relfilename, const asmsourcemappingvector & entries)
{
	try {
		LoadLibraryHandleScope hmodule(m_proj.AsmDLLPath);
		AsmDllModule asmdll(hmodule, m_proj, true, nullptr);
		auto asmfile = build_asm_path(m_proj, relfilename);
		if (!asmdll.GetRomDataFromAsm(asmfile.c_str(), m_proj.m_varmap, entries, m_proj.ROM))
			THROWEXCEPTION(std::runtime_error, "the call to get ROM data from assembly source failed");
	}
	catch (std::exception ex) {
		ErrorHere << __FUNCTION__ " failed due to an exception: " << ex.what() << std::endl;
		throw;
	}
	catch (...) {
		ErrorHere << __FUNCTION__ " failed due to an unknown exception" << std::endl;
		throw;
	}
}

void GameSerializer::SaveInline(CString relfilename, const asmsourcemappingvector & entries)
{
	try {
		LoadLibraryHandleScope hmodule(m_proj.AsmDLLPath);
		AsmDllModule asmdll(hmodule, m_proj, true, nullptr);
		auto asmfile = build_asm_path(m_proj, relfilename);
		if (!asmdll.SetAsmFromRomData(asmfile.c_str(), m_proj.m_varmap, entries, m_proj.ROM))
			THROWEXCEPTION(std::runtime_error, "the call to save ROM data to assembly source failed");
	}
	catch (std::exception ex) {
		ErrorHere << __FUNCTION__ " failed due to an exception: " << ex.what() << std::endl;
	}
	catch (...) {
		ErrorHere << __FUNCTION__ " failed due to an unknown exception" << std::endl;
	}
}

void GameSerializer::LoadDialogue(CString filename, std::string mappinglabel, stdstringvector & routinenames, talknamevectormap & routinemappings)
{
	if (mappinglabel.empty())
		THROWEXCEPTION(std::invalid_argument, "can't load dialogue with an empty mapping label");

	try {
		LoadLibraryHandleScope hmodule(m_proj.AsmDLLPath);
		AsmDllModule asmdll(hmodule, m_proj, true, nullptr);
		if (!asmdll.ReadDialogue(filename, m_proj.m_varmap, mappinglabel, routinenames, routinemappings))
			THROWEXCEPTION(std::runtime_error, "the call to get dialogue data from the assembly source failed");
	}
	catch (std::exception ex) {
		ErrorHere << __FUNCTION__ " failed due to an exception: " << ex.what() << std::endl;
		throw;
	}
	catch (...) {
		ErrorHere << __FUNCTION__ " failed due to an unknown exception" << std::endl;
		throw;
	}
}

void GameSerializer::SaveDialogue(CString filename, std::string mappinglabel, const stdstringvector & routinenames, const talknamevectormap & routinemappings)
{
	if (mappinglabel.empty())
		THROWEXCEPTION(std::invalid_argument, "can't save dialogue with an empty mapping label");

	try {
		LoadLibraryHandleScope hmodule(m_proj.AsmDLLPath);
		AsmDllModule asmdll(hmodule, m_proj, true, nullptr);
		if (!asmdll.WriteDialogue(filename, m_proj.m_varmap, mappinglabel, routinenames, routinemappings))
			THROWEXCEPTION(std::runtime_error, "the call to set dialogue data to the assembly source failed");
	}
	catch (std::exception ex) {
		ErrorHere << __FUNCTION__ " failed due to an exception: " << ex.what() << std::endl;
		throw;
	}
	catch (...) {
		ErrorHere << __FUNCTION__ " failed due to an unknown exception" << std::endl;
		throw;
	}
}

void GameSerializer::LoadTalkPointerTable(CString asmfilename)
{
	auto label = ReadIni(m_proj.DialoguePath, "Label_TalkJumpTable", "value", "");
	auto asmfile = Paths::Combine({ m_proj.ProjectFolder, "asm", asmfilename });
	auto routinenames = DoReadAsmTable(asmfile, label);

	auto TALKROUTINEPTRTABLE_OFFSET = Ini::ReadHex(m_proj.ValuesPath, "TALKROUTINEPTRTABLE_OFFSET");
	for (auto sp = 0u; sp < routinenames.size(); ++sp) {
		const auto & name = routinenames[sp];
		auto bankaddr = strtoul(Ini::ReadIni(m_proj.DialoguePath, name.c_str(), "bankaddr", "0x0"), nullptr, 16);
		if (bankaddr == 0)
			throw std::runtime_error("Element '" + name + "' has a zero bankaddr, the element is invalid.");

		auto offset = TALKROUTINEPTRTABLE_OFFSET + (sp * 2);
		m_proj.ROM[offset] = bankaddr & 0xFF;
		m_proj.ROM[offset + 1] = (bankaddr >> 8) & 0xFF;
	}
}

void GameSerializer::PreloadTalkAsmData(CString asmfilename)
{
	std::string label = (LPCSTR)ReadIni(m_proj.DialoguePath, "Label_TalkJumpTable", "value", "");
	auto asmfile = Paths::Combine({ m_proj.ProjectFolder, "asm", asmfilename });

	stdstringvector routinenames;
	talknamevectormap routinemappings;
	LoadDialogue(asmfile, label, routinenames, routinemappings);

	stdstrintmap valuesmap;
	ReadAllVariables(m_proj.ProjectFolder, valuesmap, nullptr);

	auto TALKROUTINEPTRTABLE_OFFSET = Ini::ReadHex(m_proj.ValuesPath, "TALKROUTINEPTRTABLE_OFFSET");
	auto TALKROUTINEPTRTABLE_PTRADD = Ini::ReadHex(m_proj.ValuesPath, "TALKROUTINEPTRTABLE_PTRADD");
	for (auto sp = 0u; sp < routinenames.size(); ++sp) {
		const auto & name = routinenames[sp];
		auto iter = routinemappings.find(name);
		if (iter != cend(routinemappings)) {
			const auto& elements = iter->second;
			auto bankaddr = strtoul(Ini::ReadIni(m_proj.DialoguePath, name.c_str(), "bankaddr", "0x0"), nullptr, 16);
			if (bankaddr == 0)
				throw std::runtime_error("Element '" + name + "' has a zero bankaddr, the element is invalid.");

			auto offset = TALKROUTINEPTRTABLE_OFFSET + (sp * 2);
			m_proj.ROM[offset] = bankaddr & 0xFF;
			m_proj.ROM[offset + 1] = (bankaddr >> 8) & 0xFF;

			// Write bytes to ROM for hardcoded values
			auto index = -1;
			for (const auto& e : elements) {
				++index;
				auto hardcoded = e.marker.find("hc") == 0;
				if (hardcoded) {
					auto delem = dialogue_helpers::ReadElement(m_proj, name.c_str(), index);
					auto roffset = delem.routineoffset;
					auto bankoffset = bankaddr + roffset;
					auto hcoffset = TALKROUTINEPTRTABLE_PTRADD + bankoffset;
					DoWriteAsBytes(hcoffset, valuesmap, e);
				}
			}
		}
	}
}

//DEVNOTE - this can be used in any assembly. those that supports the constant naming format will find variables.
//    other assemblies will simply find no variables instead of failing.
bool GameSerializer::ReadAllVariables(CString projectpath, stdstrintmap & valuesmap, IProgress * progress)
{
	const auto NUMBERFORMAT = "(\\#?\\-?[$|%]?[0-9A-Fa-f]+)";

	std::set<std::string> varstosave = read_varnames();
	stdstringvector varprefixes = read_prefixes();

	const auto ReadVars = [projectpath, varstosave, varprefixes, NUMBERFORMAT](CString filename, stdstrintmap & vmap) {
		auto fullpath = Paths::Combine({ projectpath, "asm", filename });
		return read_variables((LPCSTR)fullpath, varstosave, varprefixes, NUMBERFORMAT, vmap);
	};

	const auto Step = [progress](CString message) {
		if (progress != nullptr) progress->StepAndProgressText(message);
	};

	if (progress) progress->AddSteps(3);

	const auto constinc = "constants.inc";
	Step("Loading constants...");
	if (!ReadVars(constinc, valuesmap))
		return false;

	const auto varsinc = "variables.inc";
 	Step("Loading variables...");
	if (!ReadVars(varsinc, valuesmap))
		return false;

	Step("Cleaning up...");
	return true;
}

stdstringset GameSerializer::read_varnames()
{
	stdstringset theset;
	//RAM_NAMES
	// read the RAM values
	auto ramnames = ReadIni(m_proj.ValuesPath, "RAM_NAMES", "value", stdstringvector{});
	for (const auto & name : ramnames) theset.insert(name);

	//EDITOR_NAME_FILTER
	// read the names in each of the filters in the name filter
	auto filters = ReadIni(m_proj.ValuesPath, "EDITOR_NAME_FILTER", "value", stdstringvector{});
	for (const auto & filter : filters) {
		auto strvalue = ReadIni(m_proj.ValuesPath, filter.c_str(), "value", "");
		auto names = split(strvalue, " ");
		for (const auto & name : names) theset.insert((LPCSTR)name);
	}
	return theset;
}

stdstringvector GameSerializer::read_prefixes()
{
	//PREFIX_FILTER
	stdstringvector vec = ReadIni(m_proj.ValuesPath, "PREFIX_FILTER", "value", stdstringvector{});
	return vec;
}

bool GameSerializer::read_variables(std::string filepath, const stdstringset & nameset, const stdstringvector & prefixes, std::string numberformat, stdstrintmap & valuesmap)
{
	std::ifstream ifs;
	ifs.open(filepath);
	if (!ifs.is_open()) return false;

	std::regex rxassignment("(\\w+)\\s*\\=\\s*(.*)");

	bool ok = false;
	try {
		while (ifs && !ifs.eof()) {
			std::string line;
			std::getline(ifs, line) >> std::ws;

			//REFACTOR - is it better to use strip_comment() here instead?
			auto commentmark = line.find(';');
			if (commentmark != std::string::npos) line = line.substr(0, commentmark);

			std::smatch match;
			if (std::regex_match(line, match, rxassignment)) {
				auto name = match[1].str();

				// take this entry if it's name or prefix is a match
				if (nameset.find(name) == std::cend(nameset)) {
					auto iter = std::find_if(cbegin(prefixes), cend(prefixes), [name](const auto & val) { return name.find(val) == 0;});
					if (iter == cend(prefixes)) continue;
				}

				// It's a match in either case, add to valuesmap, but throw if it's a duplicate
				if (valuesmap.find(name) != end(valuesmap))
					throw std::domain_error("duplicate variable " + name + " is not allowed");

				auto expression = match[2].str();
				auto result = regex_helpers::eval_formula(expression, valuesmap, numberformat);
				valuesmap[name] = result;
			}
		}
		ok = true;
	}
	catch (std::exception & e) {
		ErrorHere << "failed with an exception: " << e.what() << std::endl;
	}
	catch (...) {
		ErrorHere << "failed with an unknown exception" << std::endl;
	}
	return ok;
}

stdstringvector GameSerializer::DoReadAsmTable(CString asmfile, CString label)
{
	std::ifstream ifs;
	ifs.open((LPCSTR)asmfile, std::ios::in);
	if (!ifs.is_open()) {
		CString msg;
		msg.Format("Unable to read routine names for label '%s' from '%s'.", label, asmfile);
		throw std::runtime_error((LPCSTR)msg);
	}

	stdstringvector routinenames;

	// go line-by-line to find the label
	auto flabel = label + ":";
	auto found = false;
	std::string line;
	while (!found && ifs && std::getline(ifs, line)) {
		if (line.find(flabel) != std::string::npos)
			found = true;
	}

	if (found) {
		// read values, progress until we find another label or end-of-file.
		while (ifs && std::getline(ifs, line)) {
			// for each line, starts with .WORD and contains a csv list of routine names
			// strip the comment if any
			auto f = line.find(".WORD");
			f = (f != std::string::npos) ? f : line.find(".word");
			if (f != std::string::npos) {

				// remove the comment, then split
				auto comment = line.find(";");
				if (comment != std::string::npos)
					line.erase(comment, line.size() - comment);
				line.erase(0, f + strlen(".WORD "));

				auto names = Strings::split(line, ",");
				for (const auto& n : names)
					routinenames.push_back(n);
			}
			else if (dialogue_helpers::HasLabel(line)) {
				break;
			}
		}
	}
	return routinenames;
}

void GameSerializer::DoWriteAsBytes(int hcoffset, stdstrintmap& valuesmap, const talkelement& elem)
{
	int value = 0;
	auto it = valuesmap.find(elem.value);
	if (it != cend(valuesmap)) {
		value = it->second;
	}
	else {
		using namespace Types;
		CString cval = elem.value.c_str();
		if (is_hex(cval)) {
			value = hex(cval);
		}
		else if (is_addr(cval)) {
			value = addr(cval);
		}
		else {
			value = dec(cval);
		}
	}
	auto bytes = elem.bits / 8;
	ASSERT(bytes > 0);
	for (auto b = 0; b < bytes; ++b) {
		auto ofs = hcoffset + b;
		m_proj.ROM[ofs] = value & 0xFF;
		value >>= 8;
	}
}