#include "stdafx.h"
#include "romasm_functions.h"
#include "ini_functions.h"
#include "path_functions.h"
#include "string_functions.h"
#include "type_support.h"

#include <fstream>
#include <regex>
#include <deque>
#include <map>
#include <vector>

namespace
{
    // an incbin starement at the start of line
    const std::regex rxIncbin(R"~(^\s*.INCBIN\s+"(.*)")~");

    // A standard or local label
    const std::regex rxStdOrLocalLabelLine(R"==(^\s*(@\w+:|\w+:))==");

    // This is a full line comment
    const std::regex rxComment(R"==(\s*;\s*(.*))==");

    // Looks like a datarow, just enough to extract the data directive.
    // Use with regex_search, not regex_match.
    //TODO - probably don't need the comment group at the end
    std::regex rxDatarowLook(R"==((\.BYTE|\.WORD|\.FARADDR)\s+(.*)\s*(?:;.*)?)==", std::regex::icase);

    const std::map<std::string, int> l_datarowwidths = {
        { ".byte", 1 },
        { ".word", 2 },
        { ".faraddr", 3 }
    };
}

namespace RomAsm
{
    std::string CondValues::ToString()
    {
        return asmtrue + "|" + asmfalse + "|" + romtrue + "|" + romfalse;
    }

    bool CondValues::FromString(std::string text)
    {
        Clear();
        auto parts = Strings::split(text, "|");
        if (parts.size() != 4)
            return false;

        asmtrue = parts[0];
        asmfalse = parts[1];
        romtrue = parts[2];
        romfalse = parts[3];
        return true;
    }

    void CondValues::Import(std::string text)
    {
        Clear();
        auto parts = Strings::split(text, "|");
        if (parts.size() != 4)
            throw std::domain_error("CondValues cannot be formed from text with " + std::to_string(parts.size()) +
                " parts instead of the expoected 4 (asmtrue|asmfalse|romtrue|romfalse).");

        asmtrue = parts[0];
        asmfalse = parts[1];
        romtrue = parts[2];
        romfalse = parts[3];
    }

    void CondValues::Clear()
    {
        asmtrue.clear();
        asmfalse.clear();
        romtrue.clear();
        romfalse.clear();
    }


    // FUNCTIONS

    CString ReadIniText(CString mapini, CString valini, CString section, CString key, CString defaultValue)
    {
        auto text = Ini::ReadIni(mapini, section, key, "");
        // If it's a @valini ref, then read the value from there using text as the key
        if (text.GetLength() > 0) {
            if (text[0] == '@')
                text = Ini::ReadIni(valini, text.Mid(1), "value", defaultValue);
        }

        return text;
    }

    int ReadIniInt(CString mapini, CString valini, CString section, CString key, CString defaultValue)
    {
        using namespace Types;
        auto text = ReadIniText(mapini, valini, section, key, defaultValue);
        try {
            return Types::to_int(text);
        }
        catch (std::exception& ex) {
            throw std::runtime_error(key + " is not an integer (" + ex.what() + ").");
        }
    }

    std::string ReadSource(CString mapini, CString valini, CString section, CString key)
    {
        auto text = Ini::ReadIni(mapini, section, key, "");
        if (!text.IsEmpty()) {
            // If it's a !mapini or @valini ref, then read the value from there using text as the key
            // If it's a @valini ref, then read the value from there using text as the key
            //TODO - for !, do the @ action if the result is blank as a fallback
            if (text[0] == '!')
                text = Ini::ReadIni(mapini, "sources", text.Mid(1), "");
            else if (text[0] == '@')
                text = Ini::ReadIni(valini, text.Mid(1), "value", "");
        }

        if (text.IsEmpty())
            throw std::runtime_error(key + " is a source reference and cannot be empty.");
        return (LPCSTR)text;
    }

    namespace Traverse
    {
        void SeekToInstruction(std::istream& is, std::string nappingname, std::string label, std::string sublabel, int instoffset)
        {
            bool found = true;
            std::string lastlabel;
            auto markers = RomAsm::BuildLabelQueue(label, sublabel);
            while (found && !markers.empty()) {
                lastlabel = markers.front();
                found = RomAsm::LabelSearch(is, lastlabel);
                markers.pop_front();
            }

            // Step past the label (note that the next instruction might be on the same line as the label)
            if (!lastlabel.empty()) {
                auto steplen = lastlabel.size();
                is.seekg(steplen, std::ios::cur);
            }

            found = found && RomAsm::InstructionSearch(is, instoffset);

            // A marker/instoffset referring to the last line of the file is unusable and is reported as not found.
            if (is.eof()) {
                throw std::runtime_error("Mapping '" + nappingname + "' did not find its specified marker in the file.");
            }
            else if (!is) {
                throw std::runtime_error("Mapping '" + nappingname + "' failed due to an unexpected error while reading the source file.");
            }
        }

        void SeekToInstruction(std::istream& is, std::string nappingname, std::string label, int instoffset)
        {
            SeekToInstruction(is, nappingname, label, "", instoffset);
        }

        void SeekToInstruction(std::istream& is, int instoffset, std::string contextname)
        {
            RomAsm::InstructionSearch(is, instoffset);

            // A marker/instoffset referring to the last line of the file is unusable and is reported as not found.
            if (is.eof()) {
                throw std::runtime_error("'" + contextname + "' did not find its specified marker in the file.");
            }
            else if (!is) {
                throw std::runtime_error("'" + contextname + "' failed due to an unexpected error while reading the source file.");
            }
        }
    }

    namespace Modify
    {
        // Appends the source stream bytes from sourcestart to the current position to the destination stream.
        // Retusns false if source is EOF upon entry.
        // Throws if sourcestart is beyond the current  position.
        // Details:
        // Appends the content from source.tellg() through source.seekg(sourcestart) to dest.
        bool RomAsm::Modify::SyncDestToCurrent(std::istream& source, std::ostream& dest, std::streampos sourcestart)
        {
            if (source.eof())
                return false;

            auto to = source.tellg();
            if (to < sourcestart)
                throw std::runtime_error("Cannot sync source stream to dest stream becuase the destination position " + std::to_string(to)
                    + " is before the start (" + std::to_string(sourcestart) + ").");

            if (to == sourcestart)
                return true; // already synched, don't count as EOF

            source.seekg(sourcestart);

            auto size = to - sourcestart;
            std::string out;
            out.reserve(size);

            auto pos = source.tellg();
            do {
                out.append(1, source.get());
                pos = source.tellg();
            } while (pos < to);

            dest << out;
            dest.flush();
            return true;
        }

        // Appends the source stream bytes from sourcestart to the end of source to the destination stream.
        // Retusns false if source is EOF upon entry.
        // Details:
        // Appends the content from source.tellg() through source.seekg(0, std::ios_base::end) to dest.
        bool SyncDestToEnd(std::istream& source, std::ostream& dest)
        {
            if (source.eof())
                return false;

            auto sourcestart = source.tellg();
            source.seekg(0, std::ios_base::end);
            return SyncDestToCurrent(source, dest, sourcestart);
        }

        // Overwrite the specified file with a portion of the contents of the rom buffer.
        std::string SaveToBinFile(std::string binpath, int offset, int count, const std::vector<unsigned char>& rom)
        {
            std::ofstream ofs(binpath, std::ios::binary);
            std::vector<unsigned char> v(count);
            overwrite(v, rom, 0, count, offset);
            ofs.seekp(0);
            ofs.write((char*)&v[0], v.size());
            return {};
        }
    }

    namespace Query
    {
        // This is a line composed entirely of whitespace characters.
        // Note that an empty line fails this test.
        bool IsWhitespaceLine(std::string line)
        {
            if (line.empty())
                return false;

            std::regex rxNonWhitespace("[\\s]+");
            bool matched = std::regex_match(line, rxNonWhitespace);
            return matched;
        }

        // This line is either blank or composed entirely of whitespace characters.
        bool IsBlankOrWhitespaceLine(std::string line)
        {
            if (line.empty())
                return true;

            //TODO - which regex is the better general option?
            //std::regex rxBlankWhite("^\\s*$");
            //bool matched = std::regex_match(line, rxBlankWhite);
            //return matched;

            std::regex rxNonWhitespace("[\\s]+");
            bool matched = std::regex_match(line, rxNonWhitespace);
            return matched;
        }

        // This is a full line comment
        bool IsComment(const std::string& line)
        {
            auto found = LineMatchesRegex(line, rxComment);
            return found;
        }

        // This line has the initial form of a data row (.BYTE, .WORD, or .FARADDR).
        // This is a relaxed search used to identify data rows when there isn't a need to
        // be exact about the contents.
        bool IsApparentDataRow(const std::string& line)
        {
            auto found = std::regex_search(line, rxDatarowLook);
            return found;
        }

        //TODO - add IsDataRow when ready
        // This line is a data row (.BYTE, .WORD, or .FARADDR) followed by one or more values.
        // The data row can be optionally preceded by a chea or local label,
        // and can be optionally followed by a comment.
    }

    namespace Options
    {
        // Returns trye if an option has the value of either "true" or "on" (case sensitive).
        bool On(const std::map<std::string, std::string>& options, std::string name)
        {
            auto iter = options.find(name);
            if (iter != options.end()) {
                return iter->second == "true" || iter->second == "on";
            }
            return false;
        }
    }


    namespace Parse
    {
        int RomToInt (const std::vector<unsigned char>& rom, int offset, int recwidth, std::string line)
        {
            int newvalue = 0;
            for (int i = 0; i < recwidth; ++i) {
                int bits = 8 * i;
                int index = offset + i;
                newvalue |= (rom[index] << bits);
            }
            return newvalue;
        }

        std::string IntToAsm (int value, std::string format)
        {
            char buf[20] = { 0 };
            auto fmt = !format.empty() ? format : "%d";
            std::sprintf(buf, fmt.c_str(), value);
            std::string asmtext = buf;
            return asmtext;
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

        namespace {
            std::regex rx_white2(R"-(^\s*(.+?)\s*$)-");
            std::regex rx_blank("^\\s+$");
        }

        // Extract the contents of the line after  labels, before comments, and optionally without surrounding whitespace.
        std::string ExtractLineContent(std::string entry, bool trimWhitespace)
        {
            std::string result;
            if (!entry.empty()) {
                const auto npos = std::string::npos;
                auto semi = entry.find(';');
                auto colon = entry.find(':');

                // cases:
                // no semi, no colon    (i.e. no comment, no label)
                // semi, no colon       (i.e. comment, but no label)
                // colon, no semi       (i.e. label, but no comment)
                // colon before semi    (i.e. label and comment)
                // otherwise, no match  (i.e. whole line comment, blank line, whitespace line)

                // Neither label nor comment
                if (colon == npos && semi == npos) {
                    result = entry;
                }
                // Both label and comment
                else if (colon != npos && semi != npos) {
                    if (colon < semi)
                        result = entry.substr(colon + 1, semi - colon - 1); // colon, then endline comment
                    else {
                        //cout << "**** semi before colon\n";
                        result = entry.substr(0, semi); // colon must be after semi, hence part of the comment
                    }
                }
                // So, it's one or the other
                else if (semi != npos) {
                    result = entry.substr(0, semi); // take what comes before the comment
                }
                else {
                    result = entry.substr(colon + 1, entry.size() - colon - 1); // skip the label
                }

                // optionally remove leading and trailing whitespace
                if (trimWhitespace)
                    result = Strings::trim(result);
            }
            return result;
        }
    }


    namespace Dialogue
    {
        std::map<std::string, int> l_elementsizes = {
            { "hctext", 1 },        // Hardcoded Text ID
            { "hcspr", 1 },         // Hardcoded Sprite ID
            { "hcobj", 1 },         // Hardcoded Sprite ID (object ID)
            { "hcevent", 1 },       // Hardcoded Sprite ID (event flag check ID)
            { "hcbattle", 1 },      // Hardcoded Battle ID
            { "hcitem", 2 },        // Hardcoded Item
            { "hcmap", 2 },         // Hardcoded Map Obj
            { "hcweapon", 1 },      // Hardcoded Weapon Index
            { "hcarmor", 1 },       // Hardcoded Armor Index
            { "hcitemormap", 2 },   // Hardcoded Item or OverworldMap sprite
            { "hcitemorcanoe", 2 }, // Hardcoded Item or Canoe
            { "hcfanfare", 2 },     // Hardcoded Increment Fanfare instruction and opcode
            { "hcnntele", 1 },      // Hardcoded In-to-In Teleport ID
            { "hcnotele", 1 },      // Hardcoded In-to-Out Teleport ID
            { "hcbyte", 1 },        // Hardcoded hexadecimal byte value
            { "hcint1", 1 },        // Hardcoded decimal byte value
            { "hcuint1", 1 },       // Hardcoded decimal byte value
        };

        // Get the number of bytes expected by the dialogue element type.
        int GetByteCountByElementType(std::string type)
        {
            if (type.empty())
                throw std::runtime_error(__FUNCTION__ ": cannot get byte count of a blank element type name.");

            //TODO - for now, simply check to see if it's hardcoded.
            // 16 bits if hardcoded, 8 bits otherwise.
            if (type.find("hc") != 0)
                return 1;

            auto i = l_elementsizes.find(type);
            if (i == l_elementsizes.end())
                throw std::runtime_error(__FUNCTION__ ": cannot get byte size or unknown hardcoded type '" + type + "'.");

            return i->second;
        }
    }


    std::deque<std::string> BuildLabelQueue(std::string label, std::string sublabel)
    {
        // label comes in as just the name, but sublabel is expected to be a local label
        // prefixed with @.
        // We'll add the colons here to keep them out of the mapping config file.
        // label equals "*" means to use the beginning of the source file as the starting point.
        std::deque<std::string> labels;
        if (label != "*") {
            labels.push_back(label + ":");
            if (!sublabel.empty()) labels.push_back(sublabel + ":");
        }
        return labels;
    }

    //TOD - change the mechanics of this to return the position and throw if not found?
    //  implies it should it leave the file at its original positiion regardless of outcome.

    // Locates a label and sets the stream position to the line of that label.
    // The label has the colon on the end.
    bool LabelSearch(std::istream& is, std::string label)
    {
        if (!is || is.eof())
            return false;

        std::regex rx("\\s*" + label + "\\s*.*", std::regex::icase);
        std::smatch m;
        auto origpos = is.tellg();
        auto pos = origpos;
        auto done = false;

        while (!done && is && !is.eof()) {
            pos = is.tellg();
            std::string line;
            std::getline(is, line);
            done = std::regex_match(line, m, rx);
        }

        if (done) {
            is.seekg(pos);
        }
        else {
            is.clear(std::ios_base::eofbit);
            is.seekg(origpos);
        }
        return done;
    }

    bool InstructionSearch(std::istream& is, int count)
    {
        // Exclude blank lines, full-line comments, and standard labels
        if (!is || is.eof())
            return false;

        std::regex rx(R"==(^(?:\s*@?\w+:|\s*;\s*.*|\s+$))==", std::regex::icase);
        std::smatch m;
        auto origpos = is.tellg();
        auto pos = origpos;

        while (count > 0 && is && !is.eof()) {
            pos = is.tellg();
            std::string line;
            std::getline(is, line);

            //TODO - if regex keeps failing, then do the label vs. semicolon search and compare positions
            //auto isinstruction = !line.empty() && !std::regex_match(line, m, rx);   // If the line matches, then it is NOT an instruction

            auto content = RomAsm::Parse::ExtractLineContent(line, true);
            auto isinstruction = !content.empty();

            if (isinstruction)
                --count;
        }

        ASSERT(count >= 0);
        bool done = count == 0;

        if (done) {
            is.seekg(pos);
        }
        else {
            is.clear(std::ios_base::eofbit);
            is.seekg(origpos);
        }
        return done;
    }

    std::string ExtractIncbinPath(std::string mappingName, std::string thepath)
    {
        // Exmaple: .INCBIN "bin/bank_09_data.bin"
        std::smatch m;
        if (!std::regex_match(thepath, m, rxIncbin))
            throw std::runtime_error("Mapping '" + mappingName + "' did not find an incbin instruction at the specified source location.");

        return m[1].str();
    }

    //TODO - move to RomAsm::Modify namespace?  Not sure if anything else needs to use this pattern
    std::string LoadFromBinFile(std::string binpath, int offset, int count, std::vector<unsigned char>& rom)
    {
        if (!Paths::FileExists(binpath.c_str()))
            return "bin file '" + binpath + "' not found in the project folder.";

        std::ifstream binifs(binpath, std::ios::binary | std::ios::ate);
        if ((int)binifs.tellg() != count)
            return " count property doesn't match binary file length " + std::to_string(binifs.tellg()) + ".";

        std::vector<unsigned char> v(count);
        binifs.seekg(0);
        binifs.read((char*)&v[0], v.size());
        overwrite(rom, v, offset, count);
        return {};
    }

    bool LineContainsRegex(const std::string line, const std::regex& rx)
    {
        if (line.empty())
            return false;

        if (!std::regex_search(line, rx)) {
            return false;
        }
        return true;
    }

    bool LineMatchesRegex(const std::string line, const std::regex& rx)
    {
        if (line.empty())
            return false;

        if (!std::regex_match(line, rx)) {
            return false;
        }
        return true;
    }

    // This line is a line starting with a standard or local label
    bool IsStdOrLocalLabel(const std::string& line)
    {
        return LineContainsRegex(line, rxStdOrLocalLabelLine);
    }
}