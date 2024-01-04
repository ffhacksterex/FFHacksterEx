#include "stdafx.h"
#include "RomAsmMappingDatfile.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "path_functions.h"
#include "romasm_functions.h"
#include "string_functions.h"

#include <fstream>

RomAsmMappingDatfile::RomAsmMappingDatfile(CFFHacksterProject& project, std::string mapping)
{
	CString romasmini = project.GetIniFilePath(FFHFILE_RomAsmMappingsPath);
    CString section = mapping.c_str();
    std::string edtype = (LPCSTR)Ini::ReadIni(romasmini, section, "type", "");
    ASSERT(edtype == "datfile");

    auto valini = project.GetIniFilePath(FFHFILE_ValuesPath);
    name = mapping;
    type = edtype;
    romoffset = RomAsm::ReadIniInt(romasmini, valini, section, "offset");
    source = Strings::trimleft(RomAsm::ReadSource(romasmini, section, section, "source"));
    count = RomAsm::ReadIniInt(romasmini, valini, section, "count");
    readonly = Ini::ReadIni(romasmini, section, "readonly", "false") == "true";
    m_destfile = (LPCSTR)Paths::Combine({ project.ProjectFolder, source.c_str() });

    std::vector<std::string> errors;
    if (type != "datfile") errors.push_back("type should be datfile, not '" + edtype + "'");
    if (romoffset < 1) errors.push_back("romoffset must be greater than zero");
    if (source.empty()) errors.push_back("source cannot be blank");
    if (count < 1) errors.push_back("count must be greater than zero");
    if (!Paths::FileExists(m_destfile.c_str())) errors.push_back("source file '" + source + "' not found in the project");
    if (!errors.empty())
        throw std::runtime_error("Editor '" + mapping + "' config is malformed: " + Strings::join(errors, "; "));
}

RomAsmMappingDatfile::~RomAsmMappingDatfile()
{
}

// Reads the entire source stream and copies its contents into a portion of rom[].
// Throws:
// runtime_error if the source stream size does not match the expected byte count.
// Details:
// This function does not rewind the stream position before returning.
void RomAsmMappingDatfile::Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    UNREFERENCED_PARAMETER(sourcefile);

    // Ignore the incoming stream and copy the file to tom directly.
    std::ifstream ifs(m_destfile, std::ios::binary | std::ios::ate);
    if ((int)ifs.tellg() != this->count)
        throw std::runtime_error(
            "Mapping '" + this->name + "' count property must equal datfile '" + this->source + "' length " + std::to_string(ifs.tellg()) + ".");

    // Read the entire datfile into a buffer.
    // Then overwrite the mapped portion of rom[] with that buffer.
    std::vector<unsigned char> v(this->count);
    ifs.seekg(0);
    ifs.read((char*)&v[0], v.size());
    overwrite(rom, v, this->romoffset, this->count);
}

// Copies a portion of rom[] to the entire dest stream.
// The dest stream is expected to refer to an open, truncated stream (at position 0).
// Throws:
// runtime_error if destfile is not zero-length.
// runtime_error if the source stream size does not match the expected byte count.
// Details:
// This function does not rewind the dest stream position before returning.
void RomAsmMappingDatfile::Save(std::istream& sourcefile, std::ostream& destfile, const std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    UNREFERENCED_PARAMETER(sourcefile);
    UNREFERENCED_PARAMETER(destfile);

    if (readonly)
        return;

    //TODO - this is an indication that I need to split the interface so that params don' tneed to be ignored.
    //      IRomAsmMappingDiscrete and IRomAsmMappingSequential will help here.

    // Ignore the incoming streams and copy rom to the file directly
    std::fstream ofs(m_destfile, std::ios::binary | std::ios::ate);
    if ((int)ofs.tellp() != this->count)
        throw std::runtime_error(
            "Mapping '" + this->name + "' count property must equal datfile '" + this->source + "' length " + std::to_string(ofs.tellp()) + ".");

    // Copy the mapped portion of rom[] into a buffer.
    // Then overwrite the entire datfile with that buffer.
    std::vector<unsigned char> v(this->count);
    overwrite(v, rom, this->romoffset, this->count);
    ofs.seekp(0);
    ofs.write((char*)&v[0], v.size());
}
