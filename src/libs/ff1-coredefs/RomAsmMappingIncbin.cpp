#include "stdafx.h"
#include "RomAsmMappingIncbin.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "path_functions.h"
#include "romasm_functions.h"
#include "string_functions.h"

#include <fstream>

RomAsmMappingIncbin::RomAsmMappingIncbin(CFFHacksterProject& project, std::string mapping)
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
    count = RomAsm::ReadIniInt(romasmini, valini, section, "count");
    readonly = Ini::ReadIni(romasmini, section, "readonly", "false") == "true";
    projectfolder = (LPCSTR)project.ProjectFolder;
    label = Ini::ReadIni(romasmini, section, "label", "");
    sublabel = Ini::ReadIni(romasmini, section, "sublabel", "");
    instoffset = atol(Ini::ReadIni(romasmini, section, "instoffset", "-1"));

    std::vector<std::string> errors;
    if (type != maptype) errors.push_back("type should be " + maptype + ", not '" + edtype + "'");
    if (romoffset < 1) errors.push_back("romoffset must be greater than zero");
    if (source.empty()) errors.push_back("source cannot be blank");
    if (count < 1) errors.push_back("count must be greater than zero");
    if (label.empty()) errors.push_back("label cannot be blank");
    if (instoffset < 1) errors.push_back("instoffset must be greater than zero");
    if (!errors.empty())
        throw std::runtime_error("Editor '" + mapping + "' config is malformed: " + Strings::join(errors, "; "));
}

RomAsmMappingIncbin::~RomAsmMappingIncbin()
{
}

void RomAsmMappingIncbin::Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    auto testpos = sourcefile.tellg();

    bool found = true;
    auto markers = RomAsm::BuildLabelQueue(label, sublabel);
    while (found && !markers.empty()) {
        found = RomAsm::LabelSearch(sourcefile, markers.front());
        markers.pop_front();
    }

    found = found && RomAsm::InstructionSearch(sourcefile, instoffset);

    // A marker/instoffset referring to the last line of the file is unusable and is reported as not found.
    if (!found || sourcefile.eof()) {
        throw std::runtime_error("Mapping '" + this->name + "' did not find its specified marker in the file.");
    }
    else if (!sourcefile) {
        throw std::runtime_error("Mapping '" + this->name + "' failed due to an unexpected error while reading the source file.");
    }
    else {
        // This is the position, now extract the incbin path from this line.
        std::string instruction;
        std::getline(sourcefile, instruction);
        auto asmrelpath = RomAsm::ExtractIncbinPath(name, instruction);
        auto binpath = Paths::Combine({ projectfolder.c_str(), "asm", asmrelpath.c_str() });
        auto status = RomAsm::LoadFromBinFile((LPCSTR)binpath, this->romoffset, this->count, rom);
        if (!status.empty())
            throw std::runtime_error("Mapping '" + this->name + "' " + status);
    }
}

void RomAsmMappingIncbin::Save(std::istream& sourcefile, std::ostream& destfile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    throw std::runtime_error(__FUNCTION__ " coming soon");
}