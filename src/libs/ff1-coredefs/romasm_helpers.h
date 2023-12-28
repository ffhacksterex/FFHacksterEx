#pragma once

#include <regex>

namespace RomAsmHelpers
{
    //TODO - change the const std::string & parms to just std::string?

    bool LineMatchesRegex(const std::string line, const std::regex& rx);
    bool LineHasLabel(const std::string line, const std::string label);
    bool IsInstruction(const std::string & line);
    bool IsStdOrLocalLabel(const std::string& line);
    bool IsComment(const std::string& line);
    std::string ExtractIncbinPath(std::string mappingName, std::string thepath);
}
