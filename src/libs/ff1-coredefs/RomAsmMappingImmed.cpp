#include "stdafx.h"
#include "RomAsmMappingImmed.h"

#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "path_functions.h"
#include "romasm_functions.h"
#include "string_functions.h"
#include "type_support.h"

#include <fstream>

namespace {
    std::regex rxImmed(R"==(\s*(?::|@\w+:)?\s*(\w{3})\s+#([$%]?[A-Fa-f0-9]+)\s*(?:;.*)?)==");

    std::regex rxImmedReplace(R"==(\s*(?::|@\w+:)?\s*(?:\w{3})\s+#([$%]?[A-Fa-f0-9]+)\s*(?:;.*)?)==");
}

RomAsmMappingImmed::RomAsmMappingImmed(CFFHacksterProject& project, std::string mapping)
{
    CString romasmini = project.GetIniFilePath(FFHFILE_RomAsmMappingsPath);
    CString section = mapping.c_str();
    std::string edtype = (LPCSTR)Ini::ReadIni(romasmini, section, "type", "");
    ASSERT(edtype == maptype);

    CString valini = project.GetIniFilePath(FFHFILE_ValuesPath);
    name = mapping;
    type = edtype;
    romoffset = RomAsm::ReadIniInt(romasmini, valini, section, "offset");
    source = Strings::trimleft(RomAsm::ReadSource(romasmini, section, section, "source"));
    readonly = Ini::ReadIni(romasmini, section, "readonly", "false") == "true";
    projectfolder = (LPCSTR)project.ProjectFolder;
    label = Ini::ReadIni(romasmini, section, "label", "");
    sublabel = Ini::ReadIni(romasmini, section, "sublabel", "");
    instoffset = atol(Ini::ReadIni(romasmini, section, "instoffset", "-1"));
    format = Ini::ReadIni(romasmini, section, "format", format.c_str());

    count = 1; // only suports a single one-byte immediate values by design

    std::vector<std::string> errors;
    if (type != maptype) errors.push_back("type should be " + maptype + ", not '" + edtype + "'");
    if (romoffset < 1) errors.push_back("romoffset must be greater than zero");
    if (source.empty()) errors.push_back("source cannot be blank");
    if (label.empty()) errors.push_back("label cannot be blank");
    if (instoffset < 1) errors.push_back("instoffset must be greater than zero");
    if (!errors.empty())
        throw std::runtime_error("Editor '" + mapping + "' config is malformed: " + Strings::join(errors, "; "));
}

RomAsmMappingImmed::~RomAsmMappingImmed()
{
}

void RomAsmMappingImmed::Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    auto testpos = sourcefile.tellg();

    RomAsm::Traverse::SeekToInstruction(sourcefile, name, label, sublabel, instoffset);

    // The line must be formatted as an immediate command.
    // Immediate instructions in 6502 are as follows:
    // ADC AND CMP CPX CPY EOR LDA LDX LDY ORA SBC
    //TODO - for the moment, we don't enforce the instruction supporting an immediate addressing mode.
    //  If that becomes a requirement, add an issue to track the effort.

    auto bookmark = sourcefile.tellg();
    std::string line;
    std::getline(sourcefile, line);

    std::smatch m;
    if (!std::regex_match(line, m, rxImmed))
        throw std::runtime_error("Unable to parse the immediate command at file position " + std::to_string(bookmark) + ".");

    auto instruction = m[1].str();
    auto operand = m[2].str();
    int value = Types::to_int(operand);
    rom[this->romoffset] = (unsigned char)(value & 0xFF);
}

void RomAsmMappingImmed::Save(std::istream& sourcefile, std::ostream& destfile, const std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    // Go to the label and instruction offset, then retrieve the line.
    auto bookmark = sourcefile.tellg();
    RomAsm::Traverse::SeekToInstruction(sourcefile, name, label, sublabel, instoffset);
    RomAsm::Modify::SyncDestToCurrent(sourcefile, destfile, bookmark);
    std::string line;
    std::getline(sourcefile, line);

    // Convert the ROM[] bytes into an int value, then format thatinto assembly source-compatible text
    auto newvalue = RomAsm::Parse::RomToInt(rom, this->romoffset, 1, line);
    auto replacement = RomAsm::Parse::IntToAsm(newvalue, this->format);

    // Find the immediate value...
    std::regex rx3(R"==((\s*(?::|@\w+:)?\s*(?:\w{3})\s+#)([$%]?[A-Fa-f0-9]+)(\s*(?:;.*)?))==");
    std::smatch m;
    if (!std::regex_match(line, m, rx3))
        throw std::runtime_error("Unable to parse the immediate command at file position " + std::to_string(bookmark) + ".");

    // ... then replace it with out formatted replacement text.
    std::string newline = m[1].str() + replacement + m[3].str();
    destfile << newline << '\n';
}