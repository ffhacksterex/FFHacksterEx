#include "stdafx.h"
#include "RomAsmMappingBinary.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "path_functions.h"
#include "romasm_functions.h"
#include "string_functions.h"

#include <fstream>
#include <regex>

RomAsmMappingBinary::RomAsmMappingBinary(CFFHacksterProject& project, std::string mapping)
{
    CString romasmini = project.GetIniFilePath(FFHFILE_RomAsmMappingsPath);
    CString section = mapping.c_str();
    std::string edtype = (LPCSTR)Ini::ReadIni(romasmini, section, "type", "");
    ASSERT(edtype == maptype);

    CString valini = project.GetIniFilePath(FFHFILE_ValuesPath);
    name = mapping;
    type = edtype;
    romoffset = RomAsm::ReadIniInt(romasmini, valini, section, "offset");
    source = RomAsm::ReadSource(romasmini, section, section, "source");
    count = RomAsm::ReadIniInt(romasmini, valini, section, "count");
    readonly = Ini::ReadIni(romasmini, section, "readonly", "false") == "true";
    projectfolder = (LPCSTR)project.ProjectFolder;
    label = Ini::ReadIni(romasmini, section, "label", "");
    sublabel = Ini::ReadIni(romasmini, section, "sublabel", "");
    instoffset = atol(Ini::ReadIni(romasmini, section, "instoffset", "-1")); //TODO - is atol safe here?
    recwidth = atol(Ini::ReadIni(romasmini, section, "recwidth", ""));
    format = Ini::ReadIni(romasmini, section, "format", format.c_str());
    groupsize = RomAsm::ReadIniInt(romasmini, valini, section, "groupsize");
    groupspacecount = RomAsm::ReadIniInt(romasmini, valini, section, "groupspacecount");

    std::vector<std::string> errors;
    if (type != maptype) errors.push_back("type should be " + maptype + ", not '" + edtype + "'");
    if (romoffset < 1) errors.push_back("romoffset must be greater than zero");
    if (label.empty()) errors.push_back("label cannot be blank");
    if (instoffset < 1) errors.push_back("instoffset must be greater than zero");
    if (recwidth < 1) errors.push_back("recwidth must be greater than zero");
    if (!errors.empty())
        throw std::runtime_error("Editor '" + mapping + "' config is malformed: " + Strings::join(errors, "; "));

    // If not using groups, set both to zero; otherwise, if grouping, ensure groupspacecount is at least 1
    if (groupsize < 1) {
        groupsize = -1;
        groupspacecount = 0;
    }
    else if (groupspacecount < 1) {
        groupspacecount = 1;
    }
}

RomAsmMappingBinary::~RomAsmMappingBinary()
{
}

namespace // DECLARATIONS
{
    // Looks like a datarow, just enugh to extract the data directive.
    // Use with regex_search, not regex_match.
    //TODO - probably don't need the comment group at the end
    std::regex rxDatarowLook(R"==((\.BYTE|\.WORD|\.FARADDR)\s+(.*)\s*(?:;.*)?)==", std::regex::icase);

    // Used for explicit data row comparison and extraction.
    // Use with regex_match.
    std::regex rxAsmDataRow(R"==((\s*(?::|@\w+:)?\s*)(\.(?:BYTE|WORD|FARADDR))(\s*)((?:[$%]?[A-Fa-f0-9]+(?:,\s*)?)+)(\s*;.*)?)==", std::regex::icase);

    const std::map<std::string, int> l_datarowwidths = {
        { ".byte", 1 },
        { ".word", 2 },
        { ".faraddr", 3 }
    };

    bool LooksLikeDataRow(const std::string& line);
    int DataDirectiveToByteWidth(std::string directive);
    int ExtractDataRowRecWidth(std::string& line);
    int TermToInt(const std::string& term, int recwidth);

    void AddToBuffer(int recwidth, int value, std::vector<unsigned char>& v); //TODO - add to RomAsm

} // end namespace / DECLARATIONS


void RomAsmMappingBinary::Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    auto& ifs = sourcefile;
    auto testpos = ifs.tellg();

    RomAsm::Traverse::SeekToInstruction(ifs, name, label, sublabel, instoffset);

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
                continue; //TODO - might need to explicitly check for a line of whitespace as well, i.e. IsWhitespaceLine(newline)

            std::string line = rawline;

            // extract the header, then walk through the values until a comment or EOL
            if (LooksLikeDataRow(line)) {
                int recwidth = ExtractDataRowRecWidth(line);
                auto terms = Strings::splitrx(line, "\\s,", true);
                for (const auto& term : terms) {
                    int value = TermToInt(term, recwidth);
                    AddToBuffer(recwidth, value, v);
                }
            }
            //TODO - review IsComment
            else if (!RomAsm::Query::IsComment(rawline)) {
                // the comment check only happens if the data row check failed first.
                // If this row is anything but a data row or comment at this point, we're done.
                break;
            }
        }
    }
    catch (std::exception& ex) {
        throw std::runtime_error("Mapping '" + this->name + "' encounter the following processing error starting at file position "
            + std::to_string(bookmark) + ": " + ex.what());
    }

    ASSERT(v.size() == totalsize);
    if (v.size() != totalsize) {
        throw std::runtime_error("Mapping '" + this->name + "' retrieved " + std::to_string(v.size()) + " bytes instead of the expected "
            + std::to_string(totalsize) + ".");
    }

    overwrite(rom, v, this->romoffset, v.size());
}

// For each line:
// - test to see if this is a compatible data row
//   * if not, skip if it's a blank line and comment
//   * else, end processing (we must've hit either EOF or a non-data instruction)
// - get the value width
//   * for now, we don't enforce this against the mapping->recwidth
// - extract the value string, count the number of values found
// - format a string with that number of values
//   * if the string overflows the number of values, ERROR*****
// - output the line to dest
void RomAsmMappingBinary::Save(std::istream& sourcefile, std::ostream& destfile, const std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    // Go to the label and instruction offset
    auto bookmark = sourcefile.tellg();
    RomAsm::Traverse::SeekToInstruction(sourcefile, name, label, sublabel, instoffset);
    RomAsm::Modify::SyncDestToCurrent(sourcefile, destfile, bookmark);

    //---
    auto restpos = sourcefile.tellg();
    sourcefile.seekg(restpos);
    //---

    const int totalsize = recwidth * count;
    auto pos = sourcefile.tellg();
    auto offset = this->romoffset;
    auto byteswritten = offset - this->romoffset;
    while (sourcefile && !sourcefile.eof() && byteswritten < totalsize)
    {
        pos = sourcefile.tellg();
        std::string rawline;
        std::getline(sourcefile, rawline);

        if (rawline.empty() || RomAsm::Query::IsWhitespaceLine(rawline) || RomAsm::Query::IsComment(rawline)) {
            destfile << rawline << '\n';
        } else if (LooksLikeDataRow(rawline)) {
            auto newline = FormatAsmSourceLine(rawline, rom, offset);
            //TODO - need to be careful about adding a newline to dest if there isn't one at end of source.
            destfile << newline << '\n';
            byteswritten = offset - this->romoffset;

            // update pos so we don't return to this same line if this loop iteration will end done processing
            pos = sourcefile.tellg();
        }
        else {
            break; // stop processing when we've a standard label or some other instruction
        }
    }

    // Once we've stopped, go back to the last position we marked (it's OK if it's EOF at this point).
    // That's where the next mapping - if any - will start.
    sourcefile.seekg(pos);

    // Ff the number of values written is incorrect, ERROR*****
    if (byteswritten != totalsize)
        throw std::runtime_error("Wrote " + std::to_string(byteswritten) + " bytes instead of the expected " + std::to_string(totalsize) + ".");
}

std::string RomAsmMappingBinary::FormatAsmSourceLine(std::string line, const std::vector<unsigned char>& rom, int& offset)
{
    std::string asmline;
    std::smatch m;
    //TODO - potentially shorten the exception message to just the first 30 characters
    if (!std::regex_match(line, m, rxAsmDataRow))
        throw std::runtime_error("Unable to parse this data row (" + asmline + ").");

    auto prefix = m[1].str();
    auto type = m[2].str();
    auto typespace = m[3].str();
    auto terms = m[4].str();
    auto suffix = m[5].str();
    int valsize = DataDirectiveToByteWidth(type);

    // the terms come in as a comm-separated list
    auto newterms = terms;
    auto textvalues = Strings::split(terms, ",");

    //TODO - is using -1 a cleaner solution than 1 << 31?
    //int grpcounter = groupsize > 0 ? groupsize : 1 << 31; //N.B. - use a large number to avoid mod and if checks in the false case
    int grpcounter = groupsize;
    std::string grpws(groupspacecount, ' ');
    std::vector<std::string> newvalues;

    //TODO - while this does work, there's a cleaner way... build newterms in pieces amd do grouping at that point
    //      instead of inside the lambda.
    
    // Replace each of the extracted values from the sourcefile into a formatted value from rom[]
    std::transform(textvalues.begin(), textvalues.end(), std::back_inserter(newvalues),
        [&offset, &rom, &grpcounter, &grpws, valsize, this](const std::string& s)
        {
            auto newvalue = RomAsm::Parse::RomToInt(rom, offset, valsize, s);
            auto replacement = RomAsm::Parse::IntToAsm(newvalue, format);
            if (grpcounter-- == 0) {
                replacement.insert(0, grpws);
                grpcounter = groupsize - 1;
            }
            offset += valsize;
            return replacement;
        });

    // Join the terms into string, then combined the parts to form the new assembly source line and return.
    newterms = Strings::join(newvalues, ","); //TODO - add option: space after separator
    return prefix + type + typespace + newterms + suffix;
}


namespace // IMPLEMENTATION
{
    // A slightly relaxed search that can easily find a data row.
    // Used when we need to tell if a rom is a data row without needed to extract or replace any of its content.
    bool LooksLikeDataRow(const std::string& line)
    {
        auto found = std::regex_search(line, rxDatarowLook);
        return found;
    }

    int DataDirectiveToByteWidth(std::string directive)
    {
        auto key = Strings::to_lower(directive);
        auto iter = l_datarowwidths.find(key);
        if (iter == l_datarowwidths.end())
            throw std::runtime_error("'" + directive + "' is not a data row header supported by binary mappings.");

        return iter->second;
    }

    // If the data row header is found, returns the matching rewidth and
    // strips the text up to the end of the rowheader from the front of the line,
    // returning the altered line through the 'line' param;
    //TODO - change from return param to a tuple or return struct instead?
    int ExtractDataRowRecWidth(std::string& line)
    {
        std::smatch m;
        if (!std::regex_search(line, m, rxDatarowLook)) //TODO  might need a more concise regex for this then rxDatarowLook
            return false;

        auto recwidth = DataDirectiveToByteWidth(m[1].str());
        auto newline = m[2].str();

        //TODO - not sure why comments aren't being trimmed off by the regex, see rx call above.
        //line = newline;
        //--
        auto semi = newline.find(';');
        if (semi != std::string::npos) {
            line = Strings::trim(newline.substr(0, semi));
        }
        else {
            line = newline;
        }
        //--

        return recwidth;
    }

    int TermToInt(const std::string& term, int recwidth)
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

} // end namespace (unnamed)