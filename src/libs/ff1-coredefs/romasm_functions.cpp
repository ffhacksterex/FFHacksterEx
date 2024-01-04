#include "stdafx.h"
#include "romasm_functions.h"
#include "ini_functions.h"
#include "path_functions.h"
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
}

namespace RomAsm
{
    CString ReadIniText(CString mapini, CString valini, CString section, CString key)
    {
        auto text = Ini::ReadIni(mapini, section, key, "");
        // If it's a @valini ref, then read the value from there using text as the key
        if (text.GetLength() > 0) {
            if (text[0] == '@')
                text = Ini::ReadIni(valini, text.Mid(1), "value", "");
        }

        return text;
    }

    int ReadIniInt(CString mapini, CString valini, CString section, CString key)
    {
        using namespace Types;
        auto text = ReadIniText(mapini, valini, section, key);
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
            auto markers = RomAsm::BuildLabelQueue(label, sublabel);
            while (found && !markers.empty()) {
                found = RomAsm::LabelSearch(is, markers.front());
                markers.pop_front();
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

            //auto size = to - sourcestart;
            //std::vector<unsigned char> v(size);
            //source.seekg(sourcestart);
            //source.read((char*)&v[0], v.size());
            ////----
            //std::string copied(v.begin(), v.end());
            ////----
            //dest.write((char*)& v[0], v.size());

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
            //overwrite(v, rom, offset, count);
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

        // This is a full line comment
        bool IsComment(const std::string& line)
        {
            auto found = LineMatchesRegex(line, rxComment);
            return found;
        }
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
            //TODO - if regex keeps failing, then do the label vs. semicolon search and compare positions
        }

        if (done) is.seekg(pos);
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

        //---
        std::string lastline;
        //---

        while (count > 0 && is && !is.eof()) {
            pos = is.tellg();
            std::string line;
            std::getline(is, line);
            lastline = line;
            //TODO - if regex keeps failing, then do the label vs. semicolon search and compare positions
            auto isinstruction = !line.empty() && !std::regex_match(line, m, rx);   // If the line matches, then it is NOT an instruction
            if (isinstruction)
                --count;
        }

        ASSERT(count >= 0);
        bool done = count == 0;

        if (done) {
            is.seekg(pos);
            //---
            std::string sanitycheck;
            std::getline(is, sanitycheck);
            is.seekg(pos);
            //---
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

    //TODO _ move to Modifu
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