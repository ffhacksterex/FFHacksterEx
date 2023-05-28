#include "stdafx.h"
#include "GameSerializer.h"
#include "FFHacksterProject.h"
#include "FFH2Project.h"
#include "asmdll_impl.h"
#include "collection_helpers.h"
#include "ValueDataAccessor.h"
#include "vda_std_collections.h"
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

using ffh::str::tomfc;
using ffh::str::tostd;

using namespace asmdll_impl;
using namespace Ini;
using namespace Io;
using namespace regex_helpers;
using namespace std_assembly::shared;
using namespace Strings;

namespace {
	CFFHacksterProject ffdummy;
	FFH2Project ff2dummy;
}

std::string build_asm_path(FFH2Project& proj, std::string relfilename)
{
	if (!proj.IsAsm())
		return std::string();

	auto asmfile = Paths::Combine({ proj.ProjectFolder.c_str(), "asm", relfilename.c_str() });
	return (LPCSTR)asmfile;
}

std::istream& open_asmstream(std::ifstream& ifs, FFH2Project& proj, std::string relfilename)
{
	if (ifs.is_open()) ifs.close();

	if (!proj.IsAsm())
		THROWEXCEPTION(std::runtime_error, "Can't load ASM inline, project is actually of type " + proj.info.type);

	auto asmfile = build_asm_path(proj, relfilename);
	if (!Paths::FileExists(asmfile)) {
		THROWEXCEPTION(std::runtime_error, "Can't load ASM inline, can't locate assembly file " + asmfile);
	}

	if (!Io::open_file(asmfile.c_str(), ifs))
		THROWEXCEPTION(std::runtime_error, "Can't load ASM inline, can't open the assembly file " + asmfile + " to load as inline assembly");

	return ifs;
}

// CFFHacksterProject versions below
std::string build_asm_path(CFFHacksterProject & proj, CString relfilename)
{
	SWITCH_OLDFFH_PTR_CHECK(&proj);
	if (!proj.IsAsm())

		return std::string();

	CString asmfile;
	asmfile.Format("%s\\asm\\%s", (LPCSTR)proj.ProjectFolder, (LPCSTR)relfilename);
	return (LPCSTR)asmfile;
}

std::istream & open_asmstream(std::ifstream & ifs, CFFHacksterProject & proj, CString relfilename)
{
	SWITCH_OLDFFH_PTR_CHECK(&proj);
	if (ifs.is_open()) ifs.close();

	if (!proj.IsAsm())
		THROWEXCEPTION(std::runtime_error, "Can't load ASM inline, project is actually of type " + std::string((LPCSTR)proj.ProjectTypeName));

	auto asmfile = build_asm_path(proj, relfilename);
	if (!Paths::FileExists(asmfile)) {
		THROWEXCEPTION(std::runtime_error, "Can't load ASM inline, can't locate assembly file " + asmfile);
	}

	if (!Io::open_file(asmfile.c_str(), ifs))
		THROWEXCEPTION(std::runtime_error, "Can't load ASM inline, can't open the assembly file " + asmfile + " to load as inline assembly");

	return ifs;
}

GameSerializer::GameSerializer(CFFHacksterProject & proj, CWnd* mainwnd)
	: m_proj(proj)
	, MainWindow(mainwnd)
	, m_prj2(ff2dummy)
{
	SWITCH_OLDFFH_PTR_CHECK(&proj);
}

GameSerializer::GameSerializer(FFH2Project& prj2, CWnd* mainwnd)
	: m_proj(ffdummy)
	, MainWindow(mainwnd)
	, m_prj2(prj2)
{
}

GameSerializer::~GameSerializer()
{
}

void GameSerializer::LoadAsmBin(CString relfilename, int offset, int expectedlength, bool allowzerolength)
{
	bool is2 = (&m_prj2 != &ff2dummy);
	bool isasm = is2 ? m_prj2.IsAsm() : m_proj.IsAsm();
	CString projectfolder = is2 ? tomfc(m_prj2.ProjectFolder) : m_proj.ProjectFolder;
	auto& rom = is2 ? m_prj2.ROM : m_proj.ROM;

	//--
	if (!isasm)
		throw std::runtime_error("Cannot load assembly binary data into a non-assembly project.");
		//throw wrongprojectype_exception(EXCEPTIONPOINT, m_proj, "asm"); //TODO - update for FFH2Project

	CString asmbin;
	asmbin.Format("%s\\asm\\%s", projectfolder, (LPCSTR)relfilename);

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
	overwrite(rom, asmbuffer, offset, expcount);
}

void GameSerializer::LoadAsmBin(CString relfilename, int offset, bool allowzerolength)
{
	LoadAsmBin(relfilename, offset, -1, allowzerolength);
}

void GameSerializer::SaveAsmBin(CString relfilename, int offset, bool allowzerolength)
{
	bool is2 = (&m_prj2 != &ff2dummy);
	bool isasm = is2 ? m_prj2.IsAsm() : m_proj.IsAsm();
	CString projectfolder = is2 ? tomfc(m_prj2.ProjectFolder) : m_proj.ProjectFolder;
	const auto& rom = is2 ? m_prj2.ROM : m_proj.ROM;

	if (!isasm)
		throw std::runtime_error("Cannot write assembly binary data into a non-assembly project.");
		//throw wrongprojectype_exception(EXCEPTIONPOINT, m_proj, "asm");

	if (!allowzerolength && rom.empty())
		THROWEXCEPTION(std::invalid_argument, "Can't write assembly binary data into a zero-byte ROM buffer at offset " + std::to_string(offset));

	CString asmbin;
	asmbin.Format("%s\\asm\\%s", projectfolder, relfilename);
	if (!Paths::FileExists(asmbin))
		THROWEXCEPTION(std::runtime_error, "Can't write assembly binary data file " + tostd(asmbin));

	// Write the same number of bytes as the size of the file (i.e. the write should't arbitrarily change the file size).
	size_t filesize = Paths::FileSize(asmbin);
	if (!allowzerolength && filesize == 0)
		THROWEXCEPTION(std::runtime_error, "Not allowed to write to zero-length destination binary file " + tostd(asmbin));

	bytevector asmbuffer(filesize, 0); 
	overwrite(asmbuffer, rom, 0, filesize, offset);

	save_binary(asmbin, asmbuffer);
}

void GameSerializer::LoadAsmBinBuffer(bytevector & ROM, CString relfilename, int offset, bool allowzerolength)
{
	bool is2 = (&m_prj2 != &ff2dummy);
	bool isasm = is2 ? m_prj2.IsAsm() : m_proj.IsAsm();
	CString projectfolder = is2 ? tomfc(m_prj2.ProjectFolder) : m_proj.ProjectFolder;

	if (!isasm)
		throw std::runtime_error("Cannot load assembly binary data into a non-assembly project.");

	if (!allowzerolength && ROM.empty())
		THROWEXCEPTION(std::invalid_argument, "Can't load assembly binary data buffer into a zero-byte ROM buffer at offset " + std::to_string(offset));

	CString asmbin;
	asmbin.Format("%s\\asm\\%s", projectfolder, relfilename);

	//QUICK-N-DIRTY METHOD
	// create asmbuffer of length count * bytewidth
	// read relfilename into asmbuffer
	// overwrite cart->ROM with asmbuffer statrting at offset
	bytevector asmbuffer;
	load_binary(asmbin, asmbuffer, allowzerolength);
	if (!allowzerolength && asmbuffer.empty())
		THROWEXCEPTION(std::runtime_error, "Not allowed to read from zero-length source binary file " + tostd(asmbin));

	auto expcount = asmbuffer.size();
	overwrite(ROM, asmbuffer, offset, expcount);
}

void GameSerializer::SaveAsmBinBuffer(const bytevector & ROM, CString relfilename, int offset, bool allowzerolength)
{
	bool is2 = (&m_prj2 != &ff2dummy);
	bool isasm = is2 ? m_prj2.IsAsm() : m_proj.IsAsm();
	CString projectfolder = is2 ? tomfc(m_prj2.ProjectFolder) : m_proj.ProjectFolder;

	if (!isasm)
		throw std::runtime_error("Cannot write assembly binary data into a non-assembly project.");

	if (!allowzerolength && ROM.empty())
		THROWEXCEPTION(std::invalid_argument, "Can't write assembly binary data buffer into a zero-byte ROM buffer at offset " + std::to_string(offset));

	CString asmbin;
	asmbin.Format("%s\\asm\\%s", projectfolder, relfilename);
	if (!Paths::FileExists(asmbin))
		THROWEXCEPTION(std::runtime_error, "Can't write assembly binary data file " + tostd(asmbin));

	// Write the same number of bytes as the size of the file (i.e. the write should't arbitrarily change the file size).
	size_t filesize = Paths::FileSize(asmbin);
	if (!allowzerolength && filesize == 0)
		THROWEXCEPTION(std::runtime_error, "Not allowed to write to zero-length destination binary file " + tostd(asmbin));

	bytevector asmbuffer(filesize, 0);
	overwrite(asmbuffer, ROM, 0, filesize, offset);

	save_binary(asmbin, asmbuffer);
}

void GameSerializer::LoadInline(CString relfilename, const asmsourcemappingvector & entries)
{
	//TODO - remove CFFHacksterProject
	bool is2 = (&m_prj2 != &ff2dummy);
	auto& rom = is2 ? m_prj2.ROM : m_proj.ROM;
	const CString& asmpath = is2 ? tomfc(m_prj2.info.asmdll) : m_proj.AsmDLLPath;

	try {
		LoadLibraryHandleScope hmodule(asmpath);
		AsmDllModule asmdll(hmodule, m_proj, true, nullptr);
		auto asmfile = is2 ? build_asm_path(m_prj2, (LPCSTR)relfilename) : build_asm_path(m_proj, relfilename);
		if (!asmdll.GetRomDataFromAsm(asmfile.c_str(), m_proj.m_varmap, entries, rom))
			THROWEXCEPTION(std::runtime_error, "Failed to get ROM data from assembly source [" + asmfile + "].");
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
	bool is2 = (&m_prj2 != &ff2dummy);
	auto& rom = is2 ? m_prj2.ROM : m_proj.ROM;
	const CString& asmpath = is2 ? tomfc(m_prj2.info.asmdll) : m_proj.AsmDLLPath;

	try {
		LoadLibraryHandleScope hmodule(asmpath);

		AsmDllModule asmdll(hmodule, m_prj2, true, nullptr);

		auto asmfile = is2 ? build_asm_path(m_prj2, (LPCSTR)relfilename) : build_asm_path(m_proj, relfilename);
		if (!asmdll.SetAsmFromRomData(asmfile.c_str(), m_proj.m_varmap, entries, rom))
			THROWEXCEPTION(std::runtime_error, "Failed to save ROM data to assembly source [" + asmfile + "].");
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

	//TODO - specify the filenames (constants, variables) in a value.
	//		That lets the designer specify what inc files to parse, so projects
	//		can add new ones without having to rebuild FFHEx.

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

	// Read the ram values into the string set (specified  in RAM_NAMES)
	ffh::acc::ValueDataAccessor d(m_prj2);
	auto ramnames = d.get<std::vector<std::string>>("RAM_NAMES");
	for (const auto& name : ramnames) theset.insert(name);

	// Next, the editor names (in EDITOR_NAME_FILTER)
	// read the names in each of the filters in the name filter
	auto filters = d.get<std::vector<std::string>>("EDITOR_NAME_FILTER");
	for (const auto& filter : filters) {
		// Get the names from each filter and add them to the set
		auto names = d.get<std::vector<std::string>>(filter);
		for (const auto& name : names) theset.insert(name);
	}
	return theset;
}

stdstringvector GameSerializer::read_prefixes()
{
	// Load in the prefix lists (from PREFIX_FILTER)
	ffh::acc::ValueDataAccessor d(m_prj2);
	return d.get<std::vector<std::string>>("PREFIX_FILTER");
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