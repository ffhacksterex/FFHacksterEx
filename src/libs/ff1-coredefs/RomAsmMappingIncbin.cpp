#include "stdafx.h"
#include "RomAsmMappingIncbin.h"

#include "collection_helpers.h"
#include "path_functions.h"
#include "romasm_helpers.h"

#include <string>
#include <fstream>
#include <deque>
#include <regex>

RomAsmMappingIncbin::RomAsmMappingIncbin()
{
}

RomAsmMappingIncbin::~RomAsmMappingIncbin()
{
}

std::string RomAsmMappingIncbin::Load(const std::map<std::string, std::string>& options, std::vector<unsigned char>& rom)
{
    using namespace RomAsmHelpers;

    // Open the assembly source file.
    // Find the marker and submarker (if specified)
    // Move instoffset instructions-1
    // Extract the relfilepath from the incbin instruction
    // Do the same thing that datfile does at this point

    //TODO - wrap this into a function and pass a delegate to do the actual work. Reduces copypasta.

    std::string asmfile = (LPCSTR)Paths::Combine({ projectfolder.c_str(), source.c_str() });
    std::ifstream ifs(asmfile);

    // Put the markers in a dequeue
    // Special case: label=* means start from the beginning of the file and ignore sublabel
    std::deque<std::string> markers;
    if (label != "*") {
        // label comes in as just the name, but sublabel is expected to be a local label
        // prefixed with @. We'll add the colon here to keep it out of the file.
        markers.push_back(label + ":");
        if (!sublabel.empty()) markers.push_back(sublabel + ":");
    }

    while (!markers.empty() && ifs && !ifs.eof()) {
        // Ensure that the line contains our label, but isn't a commented-out line.
        std::string line;
        std::getline(ifs, line);
        if (LineHasLabel(line, markers.front())) {
            // Pop the front, if we have a submarker it becomes the new front; else, we're done.
            markers.pop_front();
        }
    }

    //TODO - we currently don't handle the case of the datarow being on the same line as the label.
    //TODO - we currently don't handle instructions starting with a label (usually @local or : cheap)
    //TODO - firm up the traversal construction, then put it in romasm_helpers

    // Either we hit the marker or EOF. Track the start of the located line
    int insthits = instoffset;
    std::streampos pos = ifs.tellg();
    if (!ifs.eof()) {
        while (insthits > 0 && ifs && !ifs.eof()) {
            pos = ifs.tellg();
            std::string line;
            std::getline(ifs, line);
            if (IsInstruction(line))
                --insthits;
        }
    }

    // A marker/instoffset referring to the last line of the file is unusable and is reported as not found.
    if (ifs.eof()) {
        return "Mapping '" + this->name + "' did not find its specified marker in the file.";
    }
    else if (!ifs) {
        return "Mapping '" + this->name + "' failed due to an unexpected error while reading the source file.";
    }
    else {
        // This is the position, now extract the incbin path from this line.
        ifs.seekg(pos);
        std::string instruction;
        std::getline(ifs, instruction);
        auto asmrelpath = ExtractIncbinPath(name, instruction);
        auto binpath = Paths::Combine({ projectfolder.c_str(), "asm", asmrelpath.c_str()});

        //TODO- this is the similar to the Datfile mapping
        if (!Paths::FileExists(binpath))
            return "Mapping '" + this->name + "' incbin file '" + asmrelpath + "' not found in the project folder.";

        std::ifstream ifs((LPCSTR)binpath, std::ios::binary | std::ios::ate);
        if ((int)ifs.tellg() != this->count)
            return "Mapping '" + this->name + "' count property must equal datfile '" + this->source + "' length " + std::to_string(ifs.tellg()) + ".";

        std::vector<unsigned char> v(this->count);
        ifs.seekg(0);
        ifs.read((char*)&v[0], v.size());

        overwrite(rom, v, this->offset, this->count);
        //ENDTODO this is the same...

        return {};
    }
}

std::string RomAsmMappingIncbin::Save(const std::map<std::string, std::string>& options, const std::vector<unsigned char>& rom)
{
    return "Mapping '" + this->name + "' save failed because incbin is not yet implemented.";
}
