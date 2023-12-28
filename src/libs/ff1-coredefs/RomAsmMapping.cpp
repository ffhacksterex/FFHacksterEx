#include "stdafx.h"
#include "RomAsmMapping.h"

std::string RomAsmMapping::GetProjectType(const std::map<std::string, std::string>& options) const
{
    auto iter = options.find("projecttype");
    return iter != options.end() ? iter->second : "";
}
