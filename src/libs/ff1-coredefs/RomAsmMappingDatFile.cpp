#include "stdafx.h"
#include "RomAsmMappingDatFile.h"

#include "collection_helpers.h"
#include "path_functions.h"

#include <fstream>


RomAsmMappingDatFile::RomAsmMappingDatFile()
{
}

RomAsmMappingDatFile::~RomAsmMappingDatFile()
{
}

std::string RomAsmMappingDatFile::Load(const std::map<std::string, std::string>& options, std::vector<unsigned char>& rom)
{
    // Open the file referred to by the source property in the project folder.
    // Read count bytes from the start of the file.
    // Write those bytes to ROM[offset].
    // But fail if it goes past the end of ROM[].
    // And fail if the count does not equal the size of the file.

    //TODO - should probably also be greater than the length of the NES ROM header, if there is one.
    if (offset < 0)
        return "Mapping '" + this->name + "' offset must be greater than zero.";

    if (this->count < 1)
        return "Mapping '" + this->name + "' count property must be greater than zero.";

    auto destend = this->offset + this->count;
    if (destend >= rom.size())
        return "Mapping '" + this->name + "' cannot overwrite the end of the ROM buffer (check offset + count against ROM[] length).";

    auto datfile = Paths::Combine({ this->projectfolder.c_str(), this->source.c_str() });
    if (!Paths::FileExists(datfile))
        return "Mapping '" + this->name + "' source file '" + this->source + "' not found in the project folder.";

    std::ifstream ifs((LPCSTR)datfile, std::ios::binary | std::ios::ate);
    if ((int)ifs.tellg() != this->count)
        return "Mapping '" + this->name + "' count property must equal datfile '" + this->source + "' length " + std::to_string(ifs.tellg()) + ".";

    std::vector<unsigned char> v(this->count);
    ifs.seekg(0);
    ifs.read((char*)&v[0], v.size());

    overwrite(rom, v, this->offset, this->count);
    
    return {};
}

std::string RomAsmMappingDatFile::Save(const std::map<std::string, std::string>& options, const std::vector<unsigned char>& rom)
{
    // Read count bytes from offset.
    // Overwrite the file with these bytes starting at 0.
    // But fail if it extends past the ROM[] buffer.
    // And fail if the count does not equal the size of the file.

    if (!this->readonly) {
        if (offset < 0)
            return "Mapping '" + this->name + "' offset must be greater than zero.";

        if (this->count < 1)
            return "Mapping '" + this->name + "' count property must be greater than zero.";

        auto destend = this->offset + this->count;
        if (destend >= rom.size())
            return "Mapping '" + this->name + "' cannot overwrite the end of the ROM buffer (check offset + count against ROM[] length).";

        auto datfile = Paths::Combine({ this->projectfolder.c_str(), this->source.c_str() });
        if (!Paths::FileExists(datfile))
            return "Mapping '" + this->name + "' source file '" + this->source + "' not found in the project folder.";

        // This is intended to replace the entire file, so count must equal the file length.
        std::ofstream ofs((LPCSTR)datfile, std::ios::binary | std::ios::in | std::ios::ate);
        if ((int)ofs.tellp() != this->count)
            return "Mapping '" + this->name + "' count property must equal datfile '" + this->source + "' length " + std::to_string(ofs.tellp()) + ".";

        std::vector<unsigned char> dest(this->count);
        overwrite(dest, rom, 0, this->count, this->offset);

        ofs.seekp(0);
        ofs.write((char*)&dest[0], dest.size());
    }
    return {};
}
