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

    std::vector<std::string> errors;
    if (type != maptype) errors.push_back("type should be " + maptype + ", not '" + edtype + "'");
    if (romoffset < 1) errors.push_back("romoffset must be greater than zero");
    if (label.empty()) errors.push_back("label cannot be blank");
    if (instoffset < 1) errors.push_back("instoffset must be greater than zero");
    if (recwidth < 1) errors.push_back("recwidth must be greater than zero");
    if (!errors.empty())
        throw std::runtime_error("Editor '" + mapping + "' config is malformed: " + Strings::join(errors, "; "));
}

RomAsmMappingBinary::~RomAsmMappingBinary()
{
}

namespace
{
    std::regex rxDatarow(R"==((\.BYTE|\.WORD|\.FARADDR)\s+(.*)\s*(?:;.*)?)==", std::regex::icase);

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

        //TODO - not sure why comments aren't being trimmed off by the regex, look into it later.
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

        return iter->second;
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

void RomAsmMappingBinary::Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    auto& ifs = sourcefile;
    auto testpos = ifs.tellg();

    bool found = true;
    auto markers = RomAsm::BuildLabelQueue(label, sublabel);
    while (found && !markers.empty()) {
        found = RomAsm::LabelSearch(ifs, markers.front());
        markers.pop_front();
    }

    found = found && RomAsm::InstructionSearch(ifs, instoffset);

    // A marker/instoffset referring to the last line of the file is unusable and is reported as not found.
    if (ifs.eof()) {
        throw std::runtime_error("Mapping '" + this->name + "' did not find its specified marker in the file.");
    }
    else if (!ifs) {
        throw std::runtime_error("Mapping '" + this->name + "' failed due to an unexpected error while reading the source file.");
    }
    else {
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
                if (IsDataRow(line)) {
                    int recwidth = ExtractDataRowRecWidth(line);

                    auto terms = Strings::splitrx(line, "\\s,", true);
                    for (const auto& term : terms) {
                        int value = TermToInt(term, recwidth);
                        AddToBuffer(recwidth, value, v);
                    }
                }
                //TODO - review IsComment
                else if (!RomAsm::IsComment(rawline)) {
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
}

void RomAsmMappingBinary::Save(std::istream& sourcefile, std::ostream& destfile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
	throw std::runtime_error(__FUNCTION__ " coming soon");
}