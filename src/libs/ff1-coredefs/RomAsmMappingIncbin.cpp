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

    RomAsm::Traverse::SeekToInstruction(sourcefile, name, label, sublabel, instoffset);

    // Extract the incbin path from this line.
    std::string instruction;
    std::getline(sourcefile, instruction);
    std::string asmrelpath = (LPCSTR)Paths::Combine({ "asm", RomAsm::ExtractIncbinPath(name, instruction).c_str() });

    // Map the binary file to a destination temp file (used during Save below).
    // We do it on each load since we don't know the actual incbin at time of construction
    // (i.e. the constructor doesn't read the assembly source file).
    savefilemappings.clear();
    savefilemappings[asmrelpath] = asmrelpath + ".~new";

    // Form the full path to the live binary file, then load it into rom[]
    auto binpath = Paths::Combine({ projectfolder.c_str(), asmrelpath.c_str() });
    auto status = RomAsm::LoadFromBinFile((LPCSTR)binpath, this->romoffset, this->count, rom);
    if (!status.empty())
        throw std::runtime_error("Mapping '" + this->name + "' " + status);
}

void RomAsmMappingIncbin::Save(std::istream& sourcefile, std::ostream& destfile,
    const std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    auto bookmark = sourcefile.tellg();
    RomAsm::Traverse::SeekToInstruction(sourcefile, name, label, sublabel, instoffset);
    RomAsm::Modify::SyncDestToCurrent(sourcefile, destfile, bookmark);

    // Write the incbin statement from the source file to dest
    std::string instruction;
    std::getline(sourcefile, instruction);
    destfile << instruction << '\n';

    // Add a save file mapping for the **TEMP** binary file.
    // We write to the temp file and let the serializer do the file ratification.
    std::string incbinrelpath = RomAsm::ExtractIncbinPath(name, instruction);
    std::string asmrelpathkey = (LPCSTR)Paths::Combine({ "asm", incbinrelpath.c_str() });
    auto iter = savefilemappings.find(asmrelpathkey);
    if (iter == savefilemappings.end())
        throw std::runtime_error("Mapping '" + this->name + "' cannot find savefile mapping for '" + asmrelpathkey + "'.");

    // Specify the TEMP binary file as the destination and build its fullpath.
    auto asmrelpath = iter->second;
    CString binpath = Paths::Combine({ projectfolder.c_str(), asmrelpath.c_str() });

    // Now save the portion of rom[] to that file.
    auto status = RomAsm::Modify::SaveToBinFile((LPCSTR)binpath, this->romoffset, this->count, rom);
    if (!status.empty())
        throw std::runtime_error("Mapping '" + this->name + "' " + status);
}

std::map<std::string, std::string> RomAsmMappingIncbin::GetSaveFileMappings() const
{
    return savefilemappings;
}
