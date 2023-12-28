#include "stdafx.h"
#include "romasm_helpers.h"

namespace {
    // one of { standard label | local label | cheap Label | full-line comment } at the start of line
    const std::regex rxNotInstruction(R"(^\s*(?:[@:;]|\w+:))");

    // one of { standard label | local label } at the start of line 
    const std::regex rxLabel(R"(^\s*(?:@?\w+:))");

    const std::regex rxStdOrLocalLabelLine(R"==(^\s*(@\w+:|\w+:))==");

    // an incbin starement at the start of line
    const std::regex rxIncbin(R"~(^\s*.INCBIN\s+"(.*)")~");

    // a comment at start of line
    const std::regex rxComment(R"(^\s*;)");

}

namespace RomAsmHelpers
{
    bool LineMatchesRegex(const std::string line, const std::regex& rx)
    {
        if (line.empty())
            return false;

        if (std::regex_search(line, rx)) {
            return false;
        }
        return true;
    }

    // Ensure that this is a label that isn't part of a commented out line.
    bool LineHasLabel(const std::string line, const std::string label)
    {
        if (line.empty() || (line.find(label) == std::string::npos))
            return false;

        auto semi = line.find(';');
        auto mark = line.find(label);
        return mark < semi; // comment cannot start before the marker (and they're never interleaved)

        //TODO - figure out why regex rxLabel doesn't work
        //if (!LineMatchesRegex(line, rxLabel))
        //    return false;

        //return line.find(label) != std::string::npos;
    }

    //TODO - IsInstruction criteria must change, because instructions can occur on the same line as the label.
    //      This is especially true for cheap labels.
    //      Leave it for now, as it mainly applies to the final mapping type we'll implement,
    //      opval.

    // This line is an instruction if it is not:
    // a fill line comment
    // a blank line
    // a label of any kind (standard:, @local, or : 'cheap')
    bool IsInstruction(const std::string & line)
    {
        return LineMatchesRegex(line, rxNotInstruction);
    }

    // This line is a line starting with a standard or local label
    bool IsStdOrLocalLabel(const std::string& line)
    {
        return LineMatchesRegex(line, rxStdOrLocalLabelLine);
    }

    // This is a full line comment
    bool IsComment(const std::string & line)
    {
        return LineMatchesRegex(line, rxComment);
    }

    std::string ExtractIncbinPath(std::string mappingName, std::string thepath)
    {
        //.INCBIN "bin/bank_09_data.bin"
        std::smatch m;
        if (!std::regex_match(thepath, m, rxIncbin))
            throw std::runtime_error("Mapping '" + mappingName + "' did not find an incbin instruction at the specified source location.");

        return m[1].str();
    }
}