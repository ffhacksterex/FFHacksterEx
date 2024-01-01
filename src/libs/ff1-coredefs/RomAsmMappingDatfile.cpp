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

void RomAsmMappingDatfile::Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    UNREFERENCED_PARAMETER(sourcefile);

    // Ignore the incoming stream and copy the file to tom directly.
    std::ifstream ifs(m_destfile, std::ios::binary | std::ios::ate);
    if ((int)ifs.tellg() != this->count)
        throw std::runtime_error(
            "Mapping '" + this->name + "' count property must equal datfile '" + this->source + "' length " + std::to_string(ifs.tellg()) + ".");

    std::vector<unsigned char> v(this->count);
    ifs.seekg(0);
    ifs.read((char*)&v[0], v.size());
    overwrite(rom, v, this->romoffset, this->count);
}

void RomAsmMappingDatfile::Save(std::istream& sourcefile, std::ostream& destfile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    throw std::runtime_error(__FUNCTION__ " coming soon.");
}
