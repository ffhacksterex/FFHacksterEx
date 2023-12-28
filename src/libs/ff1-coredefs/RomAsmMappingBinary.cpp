#include "stdafx.h"
#include "RomAsmMappingBinary.h"

#include "collection_helpers.h"
#include "path_functions.h"
#include "romasm_helpers.h"
#include "string_functions.h"

#include <deque>
#include <string>
#include <fstream>
#include <regex>
#include <sstream>

RomAsmMappingBinary::RomAsmMappingBinary()
{
}

RomAsmMappingBinary::~RomAsmMappingBinary()
{
}

namespace
{
    //std::regex rxDatarow(R"==((\.BYTE|\.WORD|\.FARADDR))==");

    //std::regex rxDatarow(R"==((\.BYTE|\.WORD|\.FARADDR)\s+(.*)(?:;.*)?)==");
    std::regex rxDatarow(R"==((\.BYTE|\.WORD|\.FARADDR)\s+(.*)(?:;.*)?)==", std::regex::icase);

    const std::map<std::string, int> l_datarowwidths = {
        { ".byte", 1 },
        { ".word", 2 },
        { ".faraddr", 3 }
    };

    bool IsDataRow(const std::string& line)
    {
        auto found = std::regex_search(line, rxDatarow);
        return found;
    }

    // If the data row header is found, returns the matching rewidth and
    // strips the text up to the end of the rowheader from the front of the line,
    // returning thje altered line through the 'line' param;
    //TODO - change from return param to a tuple or return struct instead?

    int ExtractDataRowRecWidth(std::string& line)
    {
        std::smatch m;
        if (!std::regex_search(line, m, rxDatarow)) //TODO  specify icase here?
            return false;

        auto rawkey = m[1].str();
        auto key = Strings::to_lower(rawkey);
        auto iter = l_datarowwidths.find(key);
        if (iter == l_datarowwidths.end())
            throw std::runtime_error("'" + rawkey + "' is not a data row header supported by binary mappings.");

        auto newline = m[2].str();
        line = newline;

        return iter->second;
    }

    int TermToInt(const std::string & term, int recwidth)
    {
        // It's one of these formats:
        // 14
        // $0E
        // %1110
        // The length of the value doesn't inform its width, e.g. $E and $000E are both
        // 2 bytes if recwidth is 2 (meaning it's in a .WORD data row).
        
        // However, range checks do apply, e.g. it's odd for (say) a 1-byte value to be $C07F.
        //TODO - for now, don't handle value range checks

        if (term.empty()) throw std::domain_error("Cannot extract an integer from an empty data row term.");

        // We rely on the assembler conventions here, so it's not robust.
        if (term[0] == '$') {
            return strtoul(term.c_str() + 1, nullptr, 16);
        }
        else if (term[0] == '%') {
            return strtoul(term.c_str() + 1, nullptr, 2);
        }
        else {
            return strtoul(term.c_str(), nullptr, 10);
        }
    }

    void AddToBuffer(int recwidth, int value, std::vector<unsigned char>& v)
    {
        switch (recwidth) {
        case 1:
            v.push_back((unsigned char)(0xFF & value));
            return;
        case 2:
            v.push_back((unsigned char)(0xFF & value));
            v.push_back((unsigned char)(0xFF & (value >> 8)));
            return;
        case 3:
            v.push_back((unsigned char)(0xFF & value));
            v.push_back((unsigned char)(0xFF & (value >> 8)));
            v.push_back((unsigned char)(0xFF & (value >> 16)));
            return;
        }

        throw std::domain_error("Data row terms with a recwidth that isn't 1, 2, or 3 are not supported.");
    }
}

std::string RomAsmMappingBinary::Load(const std::map<std::string, std::string>& options, std::vector<unsigned char>& rom)
{
    // Open the assembly source file.
    // Find the marker and submarker (if specified)
    // Move instoffset instructions-1
    // until the next instruction is not a data row (byte/word/faraddr)
    // - read all entries on the row and convert to an int
    // - store the int as bytes according to data row (byte/word/faraddr)
    // - stop processing once we've read recwidth * count bytes.
    // fail if we haven't read exactly recwidth * count bytes.

    //TODO - consider using @ notation to allow count to be read from INI.
    //  recwidth is provided to help keep counts relevant to objects and not bytes.
    //  there are 40 weapons, and each has a 2 byte permissions, so
    //      count=@WEAPON_COUNT
    //      recwidth=2
    //  is more symantically meaningful than
    //      count=80
    //  plus it cuts down on duplicating the counts.

    using namespace RomAsmHelpers;

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

    //TODO - firm up the traversal construction, then put it in romasm_helpers
    //TODO - we currently don't handle the case of the datarow being on the same line as the label.
    //TODO - we currently don't handle instructions starting with a label (usually @local or : cheap)

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
        ifs.seekg(pos);

        // until the next instruction is not a data row (byte/word/faraddr)
        // - read all entries on the row and convert to an int
        // - store the int as bytes according to data row (byte/word/faraddr)
        // - stop processing once we've read recwidth * count bytes.
        // fail if we haven't read exactly recwidth * count bytes.

        std::streampos bookmark;
        const int totalsize = recwidth * count;
        std::vector<unsigned char> v;
        v.reserve(totalsize);
        
        try {
            while (ifs && !ifs.eof() && v.size() < totalsize) {
                bookmark = ifs.tellg();

                std::string rawline;
                std::getline(ifs, rawline);
                if (rawline.empty())
                    continue;

                std::string line = rawline;

                // extract the header, then walk through the values until a comment or EOL
                if (IsDataRow(line)) {
                    int recwidth = ExtractDataRowRecWidth(line);

                    auto terms = Strings::splitrx(line, "\\s,", true);
                    for (const auto& term : terms) {
                        int value = TermToInt(term, recwidth);
                        AddToBuffer(recwidth, value, v);
                    }
                }
                else if (!IsComment(rawline)) {
                    // the comment check only happens if the data row check failed first.
                    break;
                }

                //else {
                //    // break if we hit another label or EOL. Allow @local and : cheap labels for now
                //    // This lessens walking through the entire file if we read less bytes than expected.
                //    if (ifs.eof() || IsStdOrLocalLabel(rawline))
                //        break;
                //}
            }
        }
        catch (std::exception& ex) {
            return "Mapping '" + this->name + "' encounter the following processing error starting at file position "
                + std::to_string(bookmark) + ": " + ex.what();
        }

        ASSERT(v.size() == totalsize);
        if (v.size() != totalsize) {
            return "Mapping '" + this->name + "' retrieved " + std::to_string(v.size()) + " bytes instead of the expected "
                + std::to_string(totalsize) + ".";
        }

        overwrite(rom, v, this->offset, v.size());
        return {};
    }
}

std::string RomAsmMappingBinary::Save(const std::map<std::string, std::string>& options, const std::vector<unsigned char>& rom)
{
    return __FUNCTION__ " save not implemented yet.";
}